#include "bridge_runtime.inl"
#include "bridge_semantic.inl"

namespace {

bool StartsWith(const wchar_t* text, const wchar_t* prefix) {
    if (!text || !prefix) return false;
    while (*prefix) {
        if (*text++ != *prefix++) return false;
    }
    return true;
}

void SetStage(BridgeStage stage) {
    if (g_shared) InterlockedExchange(&g_shared->stage, static_cast<LONG>(stage));
}

bool PrepareSemanticMainThread(wchar_t* detail, std::size_t cap) {
    if (!g_api.Load(detail, cap)) return false;
    SetStage(BridgeStage::Il2CppReady);
    SetStage(BridgeStage::MainThreadProof);
    if (!ProveUnityMainThread(detail, cap)) return false;
    return true;
}

bool EnsureShared() {
    if (g_shared) return true;
    wchar_t name[96]{};
    MappingName(GetCurrentProcessId(), name, _countof(name));
    g_mapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, name);
    if (!g_mapping) return false;
    g_shared = reinterpret_cast<SharedBlock*>(MapViewOfFile(
        g_mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedBlock)));
    if (!g_shared || g_shared->magic != kMagic ||
        g_shared->protocolVersion != kProtocolVersion ||
        g_shared->targetPid != GetCurrentProcessId()) {
        if (g_shared) UnmapViewOfFile(g_shared);
        if (g_mapping) CloseHandle(g_mapping);
        g_shared = nullptr;
        g_mapping = nullptr;
        return false;
    }
    InterlockedExchange(&g_shared->bridgeLoaded, 1);
    return true;
}

void FinishRequest(LONG seq, Response& response, bool ok,
                   const wchar_t* detail, const wchar_t* data) {
    response.ok = ok ? 1 : 0;
    SetText(response.detail, _countof(response.detail), detail);
    SetText(response.data, _countof(response.data), data);
    g_shared->response = response;
    MemoryBarrier();
    SetStage(BridgeStage::ResponseReady);
    InterlockedExchange(&g_shared->completedSeq, seq);
    InterlockedExchange(&g_shared->bridgeBusy, 0);
}

void ProcessRequest() {
    if (!EnsureShared()) return;
    const LONG seq = g_shared->requestSeq;
    if (seq <= 0 || seq == g_shared->completedSeq) return;
    InterlockedExchange(&g_shared->callbackSeq, seq);
    SetStage(BridgeStage::HookEntered);
    if (InterlockedCompareExchange(&g_shared->bridgeBusy, 1, 0) != 0) return;
    SetStage(BridgeStage::RequestAccepted);

    Response response{};
    response.callbackThreadId = GetCurrentThreadId();
    wchar_t detail[512]{};
    wchar_t data[4096]{};
    bool ok = false;
    const Request request = g_shared->request;

    if (response.callbackThreadId != g_shared->targetWindowThreadId) {
        SetText(detail, _countof(detail), L"Sai callback thread; action bị chặn");
        response.errorCode = 1001;
        FinishRequest(seq, response, false, detail, data);
        return;
    }

    // v0.1.1: Probe is deliberately non-blocking. It proves only the Windows hook,
    // shared mapping and callback thread. Heavy IL2CPP/Lua work must never hide this proof.
    if (static_cast<Command>(request.command) == Command::Probe) {
        ok = true;
        response.errorCode = 0;
        SetText(detail, _countof(detail), L"HOOK + SHARED MEMORY PASS; callback đã vào bridge DLL");
        SetText(data, _countof(data), L"HOOK_READY|NO_DOSTRING");
        FinishRequest(seq, response, ok, detail, data);
        return;
    }

    if (!PrepareSemanticMainThread(detail, _countof(detail))) {
        response.errorCode = 1101;
        FinishRequest(seq, response, false, detail, data);
        return;
    }

    SetStage(BridgeStage::SemanticCall);
    switch (static_cast<Command>(request.command)) {
        case Command::QueryTeam:
            SetStage(BridgeStage::ScanPlayers);
            ok = RunQueryTeam(data, _countof(data), detail, _countof(detail));
            response.errorCode = ok ? 0 : 1301;
            break;

        case Command::SelectTarget:
            if (!request.roleId) {
                SetText(detail, _countof(detail), L"RoleID bằng 0");
                response.errorCode = 1401;
                break;
            }
            if (!SafeForWorldAction(detail, _countof(detail))) {
                response.errorCode = 1402;
                break;
            }
            SetStage(BridgeStage::SelectTarget);
            ok = RunSelectTarget(request.roleId, data, _countof(data), detail, _countof(detail));
            if (ok && StartsWith(data, L"OK|")) {
                response.selectedRoleId = request.roleId;
                response.errorCode = 0;
            } else {
                ok = false;
                response.errorCode = 1403;
            }
            break;

        case Command::SelectAndTrade:
            if (!request.roleId) {
                SetText(detail, _countof(detail), L"RoleID bằng 0");
                response.errorCode = 1501;
                break;
            }
            if (!SafeForWorldAction(detail, _countof(detail))) {
                response.errorCode = 1502;
                break;
            }
            SetStage(BridgeStage::SelectTarget);
            ok = RunSelectAndTrade(request.roleId, data, _countof(data), detail, _countof(detail));
            if (ok && StartsWith(data, L"OK|")) {
                response.selectedRoleId = request.roleId;
                response.errorCode = 0;
            } else {
                ok = false;
                response.errorCode = 1503;
            }
            break;

        case Command::QueryTradeUi:
            SetStage(BridgeStage::QueryTradeUi);
            ok = RunQueryTradeUi(data, _countof(data), detail, _countof(detail));
            if (ok) {
                response.tradeUiVisible = StartsWith(data, L"OPEN|") ? 1 : 0;
                response.errorCode = 0;
            } else {
                response.errorCode = 1601;
            }
            break;

        default:
            SetText(detail, _countof(detail), L"Command không hợp lệ");
            response.errorCode = 1002;
            break;
    }

    FinishRequest(seq, response, ok, detail, data);
}

} // namespace

extern "C" __declspec(dllexport) LRESULT CALLBACK TlcGetMessageHook(int code, WPARAM wParam, LPARAM lParam) {
    (void)wParam;
    if (code >= 0 && lParam) {
        const MSG* msg = reinterpret_cast<const MSG*>(lParam);
        if (msg->message == kWakeMessage) ProcessRequest();
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
    } else if (reason == DLL_PROCESS_DETACH) {
        if (g_shared) UnmapViewOfFile(g_shared);
        if (g_mapping) CloseHandle(g_mapping);
        g_shared = nullptr;
        g_mapping = nullptr;
    }
    return TRUE;
}
