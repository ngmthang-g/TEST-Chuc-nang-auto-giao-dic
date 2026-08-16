#include <windows.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>
#include <algorithm>
#include "protocol.h"

using namespace autotradetest;

namespace {

constexpr wchar_t kTitle[] = L"Thần Long Auto Giao Dịch Test v0.1.0";
constexpr wchar_t kGameModule[] = L"GameAssembly.dll";
constexpr wchar_t kBridgeDll[] = L"ThanLongAutoTradeTestBridge.dll";
constexpr UINT_PTR kTradePollTimer = 9100;

constexpr int IDC_CLIENT = 100;
constexpr int IDC_SCAN = 101;
constexpr int IDC_PROBE = 102;
constexpr int IDC_TEAM = 110;
constexpr int IDC_TEAM_SCAN = 111;
constexpr int IDC_TARGET = 120;
constexpr int IDC_TRADE = 121;
constexpr int IDC_CHECK_UI = 122;
constexpr int IDC_STATUS = 130;
constexpr int IDC_LOG = 131;

template <typename T>
bool ResolveProc(HMODULE module, const char* name, T& out) {
    out = nullptr;
    FARPROC raw = GetProcAddress(module, name);
    if (!raw) return false;
    static_assert(sizeof(raw) == sizeof(out), "pointer-size mismatch");
    std::memcpy(&out, &raw, sizeof(out));
    return out != nullptr;
}

std::wstring ExeDir() {
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, _countof(path));
    if (wchar_t* slash = wcsrchr(path, L'\\')) *slash = 0;
    return path;
}

void SetControlText(HWND hwnd, const std::wstring& text) {
    SetWindowTextW(hwnd, text.c_str());
}

bool HasModule(DWORD pid, const wchar_t* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snapshot == INVALID_HANDLE_VALUE) return false;
    MODULEENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    bool found = false;
    if (Module32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szModule, name) == 0) {
                found = true;
                break;
            }
        } while (Module32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return found;
}

struct GameClient {
    DWORD pid = 0;
    DWORD threadId = 0;
    HWND window = nullptr;
    std::wstring title;
};

BOOL CALLBACK EnumGameWindows(HWND hwnd, LPARAM param) {
    if (!IsWindowVisible(hwnd) || GetWindowTextLengthW(hwnd) <= 0) return TRUE;
    DWORD pid = 0;
    const DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || !tid || !HasModule(pid, kGameModule)) return TRUE;
    auto* clients = reinterpret_cast<std::vector<GameClient>*>(param);
    for (const auto& client : *clients) {
        if (client.pid == pid) return TRUE;
    }
    wchar_t title[512]{};
    GetWindowTextW(hwnd, title, _countof(title));
    clients->push_back({pid, tid, hwnd, title});
    return TRUE;
}

std::vector<GameClient> FindClients() {
    std::vector<GameClient> clients;
    EnumWindows(EnumGameWindows, reinterpret_cast<LPARAM>(&clients));
    std::sort(clients.begin(), clients.end(), [](const GameClient& a, const GameClient& b) {
        return a.pid < b.pid;
    });
    return clients;
}

class BridgeClient {
public:
    ~BridgeClient() { Close(); }

    bool Attach(const GameClient& game, std::wstring& error) {
        Close();
        game_ = game;

        wchar_t mappingName[96]{};
        MappingName(game.pid, mappingName, _countof(mappingName));
        mapping_ = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
                                      sizeof(SharedBlock), mappingName);
        if (!mapping_) {
            error = L"Không tạo được shared memory";
            return false;
        }
        shared_ = reinterpret_cast<SharedBlock*>(MapViewOfFile(
            mapping_, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedBlock)));
        if (!shared_) {
            error = L"Không map được shared memory";
            Close();
            return false;
        }
        ZeroMemory(shared_, sizeof(*shared_));
        shared_->magic = kMagic;
        shared_->protocolVersion = kProtocolVersion;
        shared_->targetPid = game.pid;
        shared_->targetWindowThreadId = game.threadId;

        const std::wstring dllPath = ExeDir() + L"\\" + kBridgeDll;
        localDll_ = LoadLibraryW(dllPath.c_str());
        if (!localDll_) {
            error = L"Thiếu ThanLongAutoTradeTestBridge.dll cạnh EXE";
            Close();
            return false;
        }
        HOOKPROC hookProc = nullptr;
        if (!ResolveProc(localDll_, "TlcGetMessageHook", hookProc)) {
            error = L"Bridge DLL thiếu TlcGetMessageHook";
            Close();
            return false;
        }
        hook_ = SetWindowsHookExW(WH_GETMESSAGE, hookProc, localDll_, game.threadId);
        if (!hook_) {
            error = L"Không hook được game; chạy tool cùng mức quyền với game";
            Close();
            return false;
        }
        if (!PostThreadMessageW(game.threadId, kWakeMessage, 0, 0)) {
            error = L"Không đánh thức được game thread";
            Close();
            return false;
        }
        attached_ = true;
        return true;
    }

    void Close() {
        if (hook_) UnhookWindowsHookEx(hook_);
        if (localDll_) FreeLibrary(localDll_);
        if (shared_) UnmapViewOfFile(shared_);
        if (mapping_) CloseHandle(mapping_);
        hook_ = nullptr;
        localDll_ = nullptr;
        shared_ = nullptr;
        mapping_ = nullptr;
        attached_ = false;
    }

    bool AttachedTo(DWORD pid) const {
        return attached_ && game_.pid == pid;
    }

    bool Call(Command command, std::uint64_t roleId, Response& out,
              std::wstring& error, DWORD timeoutMs = 2500) {
        if (!attached_ || !shared_) {
            error = L"Bridge chưa attach";
            return false;
        }
        const LONG next = shared_->requestSeq + 1;
        shared_->request = {};
        shared_->request.command = static_cast<std::uint32_t>(command);
        shared_->request.roleId = roleId;
        MemoryBarrier();
        InterlockedExchange(&shared_->requestSeq, next);
        if (!PostThreadMessageW(game_.threadId, kWakeMessage, 0, 0)) {
            error = L"Không đánh thức được game thread";
            return false;
        }
        const DWORD begin = GetTickCount();
        while (GetTickCount() - begin < timeoutMs) {
            if (shared_->completedSeq == next) {
                MemoryBarrier();
                out = shared_->response;
                if (!out.ok) {
                    error = out.detail[0] ? out.detail : L"Bridge trả lỗi";
                    return false;
                }
                return true;
            }
            Sleep(5);
        }
        error = L"Bridge timeout; không tự retry action";
        return false;
    }

private:
    GameClient game_{};
    HANDLE mapping_ = nullptr;
    SharedBlock* shared_ = nullptr;
    HMODULE localDll_ = nullptr;
    HHOOK hook_ = nullptr;
    bool attached_ = false;
};

} // namespace
