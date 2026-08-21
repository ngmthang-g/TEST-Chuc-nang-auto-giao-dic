#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <utility>
#include "protocol.h"
#include "route_logic.h"
#include "rotation_logic.h"
#include "trade_coordinator_logic.h"
#include "fixed_slot_sell_logic.h"
#include "internal_ui_click_logic.h"
#include "travel_fight_guard_logic.h"

using namespace cleanroute;
using namespace cleanroute_logic;
using namespace cleanroute_rotation;
using namespace itemtrade_coordinator;

namespace {

constexpr wchar_t kTitle[] = L"Thần Long Item Consolidator v0.6.1.6 • PER-CLIENT HIDDEN ACTIONS";
constexpr wchar_t kGameModule[] = L"GameAssembly.dll";
constexpr UINT_PTR kTimer = 1;
constexpr UINT_PTR kRecordTimer = 2;
constexpr int kCaptureHotkeyId = 9001;
constexpr int kPauseHotkeyId = 9002;
constexpr DWORD kClientStableResumeMs = 2000;
constexpr DWORD kBridgeNudgeMs = 750;
constexpr DWORD kReadFailLogIntervalMs = 2000;
constexpr UINT kWindowResponsiveProbeMs = 120;
constexpr DWORD kTrainPositionCheckMs = 180000;
constexpr DWORD kAutoFightRecheckMs = 60000;
constexpr DWORD kMountRetryWaitMs = 5000;
constexpr DWORD kFootWalkMaxMs = 15000;
constexpr DWORD kMountFightBoostMs = 10000;
constexpr DWORD kPriorityAutoVerifyMs = 1300;
constexpr int kUnderworldMapId = 87;
constexpr int kLauLanMapId = 5;
constexpr DWORD kLauLanGateStallMs = 3000;
constexpr DWORD kLauLanConfirmRetryMs = 3000;
constexpr DWORD kAutoPathFightConflictRetryMs = 1000;
constexpr DWORD kRouteOwnershipStopRetryMs = 1200;
constexpr int kRouteOwnershipStopMaxAttempts = 3;
constexpr DWORD kTradeBagStableMs = 600;       // MAIN FreeBagSpace must stay unchanged this long after a pass.
constexpr DWORD kTradeBagVerifyMaxMs = 2200;   // Bounded wait; prevents an unstable snapshot from freezing trade forever.
constexpr int kRotateDeathLimitDefault = 10;
constexpr int kRotateDeathWindowMinDefault = 10;
constexpr int kRotateNoFullBagMinDefault = 15;
constexpr int kRotateDeathLimitMin = 1;
constexpr int kRotateDeathLimitMax = 100;
constexpr int kRotateWindowMin = 1;
constexpr int kRotateWindowMax = 180;






constexpr int IDC_CLIENT_LIST = 100;
constexpr int IDC_SCAN = 101;
constexpr int IDC_START_CHECKED = 102;
constexpr int IDC_STOP_CHECKED = 103;
constexpr int IDC_SELECTED = 104;
constexpr int IDC_LIVE = 105;
constexpr int IDC_TARGET_NAME = 110;
constexpr int IDC_SAVE_TARGET = 111;
constexpr int IDC_TARGET_TEXT = 112;
constexpr int IDC_TOLERANCE = 113;
constexpr int IDC_SPOT_COMBO = 114;
constexpr int IDC_DELETE_SPOT = 115;
constexpr int IDC_ENABLE_REVIVE = 120;
constexpr int IDC_ENABLE_CONFIRM = 121;
constexpr int IDC_ENABLE_FIGHT = 122;
constexpr int IDC_ENABLE_SELL = 123;
constexpr int IDC_SELL_NPC = 124;
constexpr int IDC_SELL_NPC_X = 125;
constexpr int IDC_SELL_NPC_Y = 126;
constexpr int IDC_SELL_NPC_CAPTURE = 127;
constexpr int IDC_SELL_NPC_POS = 128;
constexpr int IDC_CAPTURE_CONFIRM = 130;
constexpr int IDC_CAPTURE_REVIVE = 131;
constexpr int IDC_CAPTURE_AUTO = 132;
constexpr int IDC_CAPTURE_ATTACK = 133;
constexpr int IDC_CAPTURE_STOP_AUTO_2 = 135;
constexpr int IDC_POINT_CONFIRM = 140;
constexpr int IDC_POINT_REVIVE = 141;
constexpr int IDC_POINT_AUTO = 142;
constexpr int IDC_POINT_ATTACK = 143;
constexpr int IDC_POINT_STOP_AUTO_2 = 145;
constexpr int IDC_TEST_CONFIRM = 150;
constexpr int IDC_TEST_REVIVE = 151;
constexpr int IDC_TEST_AUTO = 152;
constexpr int IDC_TEST_ATTACK = 153;
constexpr int IDC_TEST_STOP_AUTO_2 = 155;
constexpr int IDC_SELL_MACRO_LIST = 170;
constexpr int IDC_SELL_ADD = 171;
constexpr int IDC_SELL_DELETE = 172;
constexpr int IDC_SELL_DESC = 173;
constexpr int IDC_SELL_DELAY = 174;
constexpr int IDC_SELL_REPEAT = 175;
constexpr int IDC_SELL_SAVE = 176;
constexpr int IDC_SELL_CAPTURE = 177;
constexpr int IDC_SELL_TEST = 178;
constexpr int IDC_LOG = 160;
constexpr int IDC_ROTATION_LIST = 186;
constexpr int IDC_ROTATE_DEATH_LIMIT = 187;
constexpr int IDC_ROTATE_DEATH_WINDOW = 188;
constexpr int IDC_ROTATE_NO_BAG = 189;
constexpr int IDC_TRADE_ROLE = 190;
constexpr int IDC_MAIN_SELL_THRESHOLD = 193;
constexpr int IDC_TRADE_RENDEZVOUS_CAPTURE = 195;
constexpr int IDC_SELL_SEQUENCE = 197;
constexpr int IDC_MAIN_TRADE_SEQUENCE = 198;
constexpr int IDC_CHILD_TRADE_SEQUENCE = 199;
constexpr int IDC_COPY_CLICKS = 200;
constexpr int IDC_SELL_REC = 201;
constexpr int IDC_SELL_COPY = 202;
constexpr int IDC_SELL_PASTE = 203;
constexpr int IDC_SELL_COPY_ACCOUNT = 204;
constexpr int IDC_CONSOLIDATE_TOGGLE = 205;
constexpr int IDC_MAIN_TAB = 207;
constexpr int IDC_SEQ_LIST = 300;
constexpr int IDC_SEQ_TARGET = 301;
constexpr int IDC_SEQ_KIND = 302;
constexpr int IDC_SEQ_DESC = 303;
constexpr int IDC_SEQ_DELAY = 304;
constexpr int IDC_SEQ_REPEAT = 305;
constexpr int IDC_SEQ_ADD = 306;
constexpr int IDC_SEQ_DELETE = 307;
constexpr int IDC_SEQ_UP = 308;
constexpr int IDC_SEQ_DOWN = 309;
constexpr int IDC_SEQ_SAVE = 310;
constexpr int IDC_SEQ_CAPTURE = 311;
constexpr int IDC_SEQ_TEST = 312;
constexpr int IDC_SEQ_CLOSE = 313;
constexpr int IDC_SEQ_REC = 314;
constexpr int IDC_SEQ_COPY = 315;
constexpr int IDC_SEQ_PASTE = 316;
constexpr int IDC_SEQ_GROUP_REPEAT = 317;
constexpr int IDC_SEQ_GROUP_SELECTED = 318;
constexpr int IDC_SEQ_UNGROUP = 319;

constexpr std::array<const wchar_t*, 5> kClickKeys = {
    L"Confirm", L"Revive", L"AutoMenu", L"Attack", L"StopAuto2"
};
constexpr std::array<const wchar_t*, 5> kClickLabels = {
    L"XÁC NHẬN RA MAP", L"ĐẦU THAI", L"AUTO", L"ĐÁNH QUÁI", L"DỪNG AUTO 2"
};

enum class ClickSlot : int {
    None = -1,
    Confirm = 0,
    Revive = 1,
    AutoMenu = 2, // one saved UI point named AUTO replaces old DỪNG AUTO 1
    Attack = 3,
    StopAuto2 = 4,
};

enum class PriorityAutoOwner : int {
    None = 0,
    TravelGuardStop,
    TravelGuardReset,
    Train,
    MountRecovery,
};

struct ClickPoint {
    int x = 0;
    int y = 0;
    int baseW = 0;
    int baseH = 0;
    bool valid = false;
};

struct SellMacroStep {
    std::wstring description;
    ClickPoint point{};
    int delayMs = 600;
    int repeat = 1;
};

struct TradeSequenceStep {
    // v0.2.7 child workflow semantics:
    // target=0 => active CON uses this row's own point.
    // target=1 => MAIN executes shared step mainRef from the MAIN common sequence.
    int target = 0;
    int mainRef = -1;
    int kind = 0;   // 0=normal click, 1=item-transfer click (CON only)
    std::wstring description;
    ClickPoint point{};
    int delayMs = 500;
    int repeat = 1;       // repeat this individual row
    int groupId = 0;      // 0=not grouped; >0=contiguous mini-sequence
    int groupRepeat = 1;  // repeat the whole mini-sequence before continuing
};

enum class RecorderMode : int { None = 0, Sell = 1, TradeMain = 2, TradeChild = 3 };

struct RecordedClick {
    DWORD pid = 0;
    ClickPoint point{};
    DWORD tick = 0;
};

struct SellNpcPreset {
    const wchar_t* name;
    int mapID;
    int npcID;
};

constexpr std::array<SellNpcPreset, 2> kSellNpcs = {{
    {L"Mã Kiêu Minh • M5 • ID 373", 5, 373},
    {L"Dược Đại Phu • Hỏa Diệm Sơn M55 • ID 279", 55, 279},
}};

struct SellNpcPosition {
    int x = 0;
    int y = 0;
    bool valid = false;
};

struct TargetProfile {
    std::wstring name;
    int mapID = 0;
    int x = 0;
    int y = 0;
    bool valid = false;
};

struct AccountProfile {
    std::wstring section;
    // 0=NONE, 1=MAIN, 2..7=CON1..CON6. Persisted by RoleID profile.
    int tradeRole = 0;
    // Per-CON release condition: after each full trade click sequence, keep the same
    // child in the workflow until FreeBagSpace reaches this target. Default = 30.
    std::wstring selectedSpot;
    int tolerance = 120;
    bool enableRevive = true;
    bool enableConfirm = true;
    bool enableFight = true;
    bool enableSell = false;
    int sellNpcPreset = 0;
    std::vector<std::wstring> rotationSpots{};
    int rotateDeathLimit = kRotateDeathLimitDefault;
    int rotateDeathWindowMin = kRotateDeathWindowMinDefault;
    int rotateNoFullBagMin = kRotateNoFullBagMinDefault;
    TargetProfile target{};
    std::array<ClickPoint, 5> points{};
    std::vector<SellMacroStep> sellMacro{};
    // Legacy v0.2.3-v0.2.6 per-CON workflow kept only for one-time v0.2.7 migration.
    // Active v0.2.7 uses one global childTradeSequence_ shared by every CON.
    std::vector<TradeSequenceStep> childTradeSequence{};
};

struct GameClient {
    DWORD pid = 0;
    DWORD threadId = 0;
    HWND window = nullptr;
    std::wstring title;
};

struct RuntimeState {
    bool running = false;
    std::wstring status = L"Đã dừng";
    int qualifiedMap = 0;
    int candidateMap = 0;
    int candidateCount = 0;
    DWORD lastActionTick = 0;
    Action lastAction = Action::Wait;

    DWORD deadSinceTick = 0;
    int revivePhase = 0;
    DWORD revivePhaseTick = 0;
    DWORD lastReviveClickTick = 0;

    int lastObservedMap = 0;
    int lastObservedX = 0;
    int lastObservedY = 0;
    DWORD lastMovementTick = 0;
    bool crossMapSeenAutoPath = false;
    DWORD stallSinceTick = 0;
    int confirmAttempts = 0;
    DWORD lastLauLanConfirmTick = 0;
    DWORD suppressRouteSinceTick = 0;

    int fightPhase = 0;
    DWORD fightPhaseTick = 0;
    int fightAttempts = 0;
    bool wasAtTarget = false;

    // Once AUTO fight is confirmed at the training spot, position is intentionally
    // checked only every 3 minutes. Death/bag state are still observed every tick.
    bool trainPositionMonitorArmed = false;
    DWORD lastTrainPositionCheckTick = 0;
    DWORD lastAutoFightCheckTick = 0;
    int trainRecoveryPhase = 0;
    DWORD trainRecoveryTick = 0;

    // Dedicated trade-rendezvous state. AutoFight stopping is no longer duplicated here;
    // every StartPath is protected by the shared v0.3 AutoFight Travel Guard.
    int tradeTravelPhase = 0;
    DWORD tradeTravelTick = 0;
    bool tradeTravelReady = false;
    std::uint64_t tradeWorkflowEntrySeq = 0; // R7: immutable FIFO ticket while staged in workflow.

    // Priority-AUTO request/result mailbox. v0.6.1.6 dispatches configured points
    // through InputSyncManager. An Attack request owns both AUTO then ĐÁNH QUÁI
    // phases and publishes one result only after click 2 finishes.
    ClickSlot priorityAutoRequestSlot = ClickSlot::None;
    PriorityAutoOwner priorityAutoRequestOwner = PriorityAutoOwner::None;
    ClickSlot priorityAutoCompletedSlot = ClickSlot::None;
    PriorityAutoOwner priorityAutoCompletedOwner = PriorityAutoOwner::None;
    bool priorityAutoCompletedOk = false;
    DWORD priorityAutoCompletedTick = 0;
    std::wstring priorityAutoReason{};
    int priorityAutoPointPhase = 0;
    DWORD priorityAutoPointTick = 0;

    // v0.3 shared AutoFight Travel Guard. Any StartPath must prove authoritative
    // AutoFight OFF. Two failed stop cycles trigger AUTO->Attack reset, then stop retries.
    int travelFightGuardPhase = 0;
    DWORD travelFightGuardTick = 0;
    int travelFightStopAttempts = 0;

    // Hard runtime invariant. If a snapshot ever exposes AutoPath=ON together
    // with AutoFight=ON, stop the path first, finish the normal two-stop/reset
    // Travel Guard, and require both states OFF before any route may resume.
    bool autoPathFightConflictLatched = false;
    DWORD autoPathFightConflictTick = 0;
    int autoPathFightConflictStopAttempts = 0;

    // Shared robust-travel helper: mount x2 -> fight 10s -> stop fight -> mount x2 ->
    // if still no mount, foot AutoPath <=15s -> StopPath -> repeat the whole cycle.
    int travelMountAttempts = 0;
    DWORD travelMountTick = 0;
    int travelMountCycle = 0; // 0=before fight boost, 1=after 10s fight boost
    int travelFightBoostPhase = 0;
    DWORD travelFightBoostTick = 0;
    bool travelFootFallback = false;
    DWORD travelFootTick = 0;

    bool crossMapRouteArmed = false;
    bool crossMapRouteMoved = false;

    int sellPhase = 0;
    DWORD sellPhaseTick = 0;
    int sellOpenAttempts = 0;
    int sellMacroIndex = 0;
    int sellMacroRepeatDone = 0;
    DWORD sellMacroNextTick = 0;
    DWORD sellMacroCompletionDueTick = 0; // R6: keep SELL macro completion state through final configured delay.
    int sellMacroPass = 0;
    int sellLastFreeBag = -1;
    DWORD sellBagStableSince = 0;
    bool sellTriggeredByFullBag = false;

    // Global per-PID transition/unresponsive safety gate. While active, no mutable
    // gameplay/window action may be dispatched. Read-only state polling continues
    // until the client is continuously healthy for kClientStableResumeMs.
    bool clientFreezeActive = false;
    DWORD clientFreezeSinceTick = 0;
    DWORD clientStableSinceTick = 0;
    int readStateFailStreak = 0;
    DWORD lastReadFailureLogTick = 0;

    // Map 87 = Địa Phủ uses the same v0.3 AutoFight Travel Guard as every other
    // movement flow. This flag is log-only; there is no separate M87 stop state machine.
    bool underworldGuardLogged = false;

    // A tool-runtime reset does not stop the game client's real AutoPath. After a
    // fresh Start or revive cold-start, reacquire route ownership by forcing any
    // stale AutoPath OFF and verifying it before a new StartPath may arm Confirm.
    bool routeOwnershipResetPending = false;
    DWORD routeOwnershipStopTick = 0;
    int routeOwnershipStopAttempts = 0;
    bool routeOwnershipResetLogged = false;
};

template <typename T>
bool ResolveProc(HMODULE module, const char* name, T& out) {
    out = nullptr;
    FARPROC raw = GetProcAddress(module, name);
    if (!raw) return false;
    static_assert(sizeof(raw) == sizeof(out), "pointer size mismatch");
    std::memcpy(&out, &raw, sizeof(out));
    return out != nullptr;
}

std::wstring ExeDir() {
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, _countof(path));
    if (wchar_t* slash = wcsrchr(path, L'\\')) *slash = 0;
    return path;
}

std::wstring LegacyConfigPath() { return ExeDir() + L"\\ThanLongCleanRoute.accounts.ini"; }

std::wstring ConfigDir() {
    wchar_t localAppData[4096]{};
    const DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, _countof(localAppData));
    if (n > 0 && n < _countof(localAppData)) {
        std::wstring dir = std::wstring(localAppData) + L"\\ThanLongCleanRoute";
        (void)CreateDirectoryW(dir.c_str(), nullptr);
        return dir;
    }
    return ExeDir();
}

std::wstring ConfigPath() {
    static const std::wstring path = ConfigDir() + L"\\ThanLongCleanRoute.accounts.ini";
    return path;
}

void MigrateLegacyConfigIfNeeded() {
    const std::wstring current = ConfigPath();
    const std::wstring legacy = LegacyConfigPath();
    if (current == legacy) return;
    if (GetFileAttributesW(current.c_str()) != INVALID_FILE_ATTRIBUTES) return;
    if (GetFileAttributesW(legacy.c_str()) == INVALID_FILE_ATTRIBUTES) return;
    (void)CopyFileW(legacy.c_str(), current.c_str(), TRUE);
}

void FlushIni() {
    (void)WritePrivateProfileStringW(nullptr, nullptr, nullptr, ConfigPath().c_str());
}

void EnsureUnicodeIni() {
    const std::wstring path = ConfigPath();
    if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) return;
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    const BYTE bom[2] = {0xFF, 0xFE};
    DWORD done = 0;
    (void)WriteFile(h, bom, 2, &done, nullptr);
    CloseHandle(h);
}

int ReadIniInt(const std::wstring& section, const std::wstring& key, int fallback) {
    return static_cast<int>(GetPrivateProfileIntW(section.c_str(), key.c_str(), fallback, ConfigPath().c_str()));
}

void WriteIniInt(const std::wstring& section, const std::wstring& key, int value) {
    wchar_t text[32]{};
    wsprintfW(text, L"%d", value);
    WritePrivateProfileStringW(section.c_str(), key.c_str(), text, ConfigPath().c_str());
}

std::wstring ReadIniText(const std::wstring& section, const std::wstring& key) {
    wchar_t text[512]{};
    GetPrivateProfileStringW(section.c_str(), key.c_str(), L"", text, _countof(text), ConfigPath().c_str());
    return text;
}

void WriteIniText(const std::wstring& section, const std::wstring& key, const std::wstring& value) {
    WritePrivateProfileStringW(section.c_str(), key.c_str(), value.c_str(), ConfigPath().c_str());
}

std::array<SellNpcPosition, kSellNpcs.size()> LoadSharedSellNpcPositions() {
    std::array<SellNpcPosition, kSellNpcs.size()> positions{};
    const std::wstring section = L"SellNpcPositions";
    for (std::size_t i = 0; i < kSellNpcs.size(); ++i) {
        const std::wstring prefix = L"SellNpcPos_" + std::to_wstring(i) + L"_";
        SellNpcPosition& pos = positions[i];
        pos.x = ReadIniInt(section, prefix + L"X", -1);
        pos.y = ReadIniInt(section, prefix + L"Y", -1);
        pos.valid = pos.x >= 0 && pos.y >= 0 && ReadIniInt(section, prefix + L"Valid", 0) != 0;
    }
    return positions;
}

void SaveSharedSellNpcPositions(const std::array<SellNpcPosition, kSellNpcs.size()>& positions) {
    EnsureUnicodeIni();
    const std::wstring section = L"SellNpcPositions";
    for (std::size_t i = 0; i < kSellNpcs.size(); ++i) {
        const std::wstring prefix = L"SellNpcPos_" + std::to_wstring(i) + L"_";
        const SellNpcPosition& pos = positions[i];
        WriteIniInt(section, prefix + L"X", pos.valid ? pos.x : -1);
        WriteIniInt(section, prefix + L"Y", pos.valid ? pos.y : -1);
        WriteIniInt(section, prefix + L"Valid", pos.valid ? 1 : 0);
    }
    FlushIni();
}

AccountProfile LoadProfile(const std::wstring& section) {
    AccountProfile p{};
    p.section = section;
    p.tradeRole = ReadIniInt(section, L"TradeRole", 0);
    if (p.tradeRole < 0 || p.tradeRole > 7) p.tradeRole = 0;
    p.tolerance = ReadIniInt(section, L"Tolerance", 120);
    if (p.tolerance < 20) p.tolerance = 20;
    if (p.tolerance > 2000) p.tolerance = 2000;
    p.enableRevive = ReadIniInt(section, L"EnableRevive", 1) != 0;
    p.enableConfirm = ReadIniInt(section, L"EnableConfirm", 1) != 0;
    p.enableFight = ReadIniInt(section, L"EnableFight", 1) != 0;
    p.enableSell = ReadIniInt(section, L"EnableSell", 0) != 0;
    p.sellNpcPreset = ReadIniInt(section, L"SellNpcPreset", 0);
    if (p.sellNpcPreset < 0 || p.sellNpcPreset >= static_cast<int>(kSellNpcs.size())) p.sellNpcPreset = 0;
    p.selectedSpot = ReadIniText(section, L"SelectedSpot");
    p.rotateDeathLimit = ReadIniInt(section, L"RotateDeathLimit", kRotateDeathLimitDefault);
    if (p.rotateDeathLimit < kRotateDeathLimitMin) p.rotateDeathLimit = kRotateDeathLimitMin;
    if (p.rotateDeathLimit > kRotateDeathLimitMax) p.rotateDeathLimit = kRotateDeathLimitMax;
    p.rotateDeathWindowMin = ReadIniInt(section, L"RotateDeathWindowMin", kRotateDeathWindowMinDefault);
    if (p.rotateDeathWindowMin < kRotateWindowMin) p.rotateDeathWindowMin = kRotateWindowMin;
    if (p.rotateDeathWindowMin > kRotateWindowMax) p.rotateDeathWindowMin = kRotateWindowMax;
    p.rotateNoFullBagMin = ReadIniInt(section, L"RotateNoFullBagMin", kRotateNoFullBagMinDefault);
    if (p.rotateNoFullBagMin < kRotateWindowMin) p.rotateNoFullBagMin = kRotateWindowMin;
    if (p.rotateNoFullBagMin > kRotateWindowMax) p.rotateNoFullBagMin = kRotateWindowMax;
    int rotationCount = ReadIniInt(section, L"RotationCount", 0);
    if (rotationCount < 0) rotationCount = 0;
    if (rotationCount > 64) rotationCount = 64;
    for (int i = 0; i < rotationCount; ++i) {
        std::wstring name = ReadIniText(section, L"RotationSpot_" + std::to_wstring(i));
        if (!name.empty() && std::none_of(p.rotationSpots.begin(), p.rotationSpots.end(), [&](const std::wstring& x){ return _wcsicmp(x.c_str(), name.c_str()) == 0; })) {
            p.rotationSpots.push_back(std::move(name));
        }
    }
    p.target.name = ReadIniText(section, L"TargetName");
    p.target.mapID = ReadIniInt(section, L"TargetMap", 0);
    p.target.x = ReadIniInt(section, L"TargetX", 0);
    p.target.y = ReadIniInt(section, L"TargetY", 0);
    p.target.valid = p.target.mapID > 0 && ReadIniInt(section, L"TargetValid", 0) != 0;
    if (p.selectedSpot.empty() && p.target.valid) p.selectedSpot = p.target.name;
    for (int i = 0; i < 5; ++i) {
        const std::wstring prefix = kClickKeys[static_cast<std::size_t>(i)];
        ClickPoint& c = p.points[static_cast<std::size_t>(i)];
        c.x = ReadIniInt(section, prefix + L"X", -1);
        c.y = ReadIniInt(section, prefix + L"Y", -1);
        c.baseW = ReadIniInt(section, prefix + L"W", 0);
        c.baseH = ReadIniInt(section, prefix + L"H", 0);
        c.valid = c.x >= 0 && c.y >= 0 && c.baseW > 0 && c.baseH > 0;
    }
    int macroCount = ReadIniInt(section, L"SellMacroCount", 0);
    if (macroCount < 0) macroCount = 0;
    if (macroCount > 64) macroCount = 64;
    for (int i = 0; i < macroCount; ++i) {
        SellMacroStep step{};
        const std::wstring prefix = L"Sell_" + std::to_wstring(i) + L"_";
        step.description = ReadIniText(section, prefix + L"Desc");
        step.point.x = ReadIniInt(section, prefix + L"X", -1);
        step.point.y = ReadIniInt(section, prefix + L"Y", -1);
        step.point.baseW = ReadIniInt(section, prefix + L"W", 0);
        step.point.baseH = ReadIniInt(section, prefix + L"H", 0);
        step.point.valid = step.point.x >= 0 && step.point.y >= 0 && step.point.baseW > 0 && step.point.baseH > 0;
        step.delayMs = ReadIniInt(section, prefix + L"Delay", 600);
        if (step.delayMs < 50) step.delayMs = 50;
        if (step.delayMs > 60000) step.delayMs = 60000;
        step.repeat = ReadIniInt(section, prefix + L"Repeat", 1);
        if (step.repeat < 1) step.repeat = 1;
        if (step.repeat > 999) step.repeat = 999;
        p.sellMacro.push_back(step);
    }
    int childTradeCount = ReadIniInt(section, L"ChildTradeCount", 0);
    childTradeCount = std::clamp(childTradeCount, 0, 64);
    for (int i = 0; i < childTradeCount; ++i) {
        TradeSequenceStep step{};
        const std::wstring prefix = L"ChildTrade_" + std::to_wstring(i) + L"_";
        step.target = std::clamp(ReadIniInt(section, prefix + L"Target", 0), 0, 1);
        step.mainRef = ReadIniInt(section, prefix + L"MainRef", -1);
        step.kind = std::clamp(ReadIniInt(section, prefix + L"Kind", 0), 0, 1);
        step.description = ReadIniText(section, prefix + L"Desc");
        step.point.x = ReadIniInt(section, prefix + L"X", -1);
        step.point.y = ReadIniInt(section, prefix + L"Y", -1);
        step.point.baseW = ReadIniInt(section, prefix + L"W", 0);
        step.point.baseH = ReadIniInt(section, prefix + L"H", 0);
        step.point.valid = step.point.x >= 0 && step.point.y >= 0 && step.point.baseW > 0 && step.point.baseH > 0;
        step.delayMs = std::clamp(ReadIniInt(section, prefix + L"Delay", 500), 50, 60000);
        step.repeat = std::clamp(ReadIniInt(section, prefix + L"Repeat", 1), 1, 999);
        step.groupId = std::max(0, ReadIniInt(section, prefix + L"GroupId", 0));
        step.groupRepeat = std::clamp(ReadIniInt(section, prefix + L"GroupRepeat", 1), 1, 999);
        if (step.kind == 1) { step.target = 0; step.mainRef = -1; }
        p.childTradeSequence.push_back(step);
    }
    return p;
}

void SaveProfile(const AccountProfile& p) {
    EnsureUnicodeIni();
    WriteIniInt(p.section, L"TradeRole", p.tradeRole);
    WriteIniInt(p.section, L"Tolerance", p.tolerance);
    WriteIniInt(p.section, L"EnableRevive", p.enableRevive ? 1 : 0);
    WriteIniInt(p.section, L"EnableConfirm", p.enableConfirm ? 1 : 0);
    WriteIniInt(p.section, L"EnableFight", p.enableFight ? 1 : 0);
    WriteIniInt(p.section, L"EnableSell", p.enableSell ? 1 : 0);
    WriteIniInt(p.section, L"SellNpcPreset", p.sellNpcPreset);
    WriteIniText(p.section, L"SelectedSpot", p.selectedSpot);
    WriteIniInt(p.section, L"RotateDeathLimit", p.rotateDeathLimit);
    WriteIniInt(p.section, L"RotateDeathWindowMin", p.rotateDeathWindowMin);
    WriteIniInt(p.section, L"RotateNoFullBagMin", p.rotateNoFullBagMin);
    WriteIniInt(p.section, L"RotationCount", static_cast<int>(p.rotationSpots.size()));
    for (std::size_t i = 0; i < p.rotationSpots.size(); ++i) {
        WriteIniText(p.section, L"RotationSpot_" + std::to_wstring(i), p.rotationSpots[i]);
    }
    WriteIniText(p.section, L"TargetName", p.target.name);
    WriteIniInt(p.section, L"TargetMap", p.target.mapID);
    WriteIniInt(p.section, L"TargetX", p.target.x);
    WriteIniInt(p.section, L"TargetY", p.target.y);
    WriteIniInt(p.section, L"TargetValid", p.target.valid ? 1 : 0);
    for (int i = 0; i < 5; ++i) {
        const std::wstring prefix = kClickKeys[static_cast<std::size_t>(i)];
        const ClickPoint& c = p.points[static_cast<std::size_t>(i)];
        WriteIniInt(p.section, prefix + L"X", c.valid ? c.x : -1);
        WriteIniInt(p.section, prefix + L"Y", c.valid ? c.y : -1);
        WriteIniInt(p.section, prefix + L"W", c.valid ? c.baseW : 0);
        WriteIniInt(p.section, prefix + L"H", c.valid ? c.baseH : 0);
    }
    WriteIniInt(p.section, L"SellMacroCount", static_cast<int>(p.sellMacro.size()));
    for (std::size_t i = 0; i < p.sellMacro.size(); ++i) {
        const SellMacroStep& step = p.sellMacro[i];
        const std::wstring prefix = L"Sell_" + std::to_wstring(i) + L"_";
        WriteIniText(p.section, prefix + L"Desc", step.description);
        WriteIniInt(p.section, prefix + L"X", step.point.valid ? step.point.x : -1);
        WriteIniInt(p.section, prefix + L"Y", step.point.valid ? step.point.y : -1);
        WriteIniInt(p.section, prefix + L"W", step.point.valid ? step.point.baseW : 0);
        WriteIniInt(p.section, prefix + L"H", step.point.valid ? step.point.baseH : 0);
        WriteIniInt(p.section, prefix + L"Delay", step.delayMs);
        WriteIniInt(p.section, prefix + L"Repeat", step.repeat);
    }
    WriteIniInt(p.section, L"ChildTradeCount", static_cast<int>(p.childTradeSequence.size()));
    for (std::size_t i = 0; i < p.childTradeSequence.size(); ++i) {
        const TradeSequenceStep& step = p.childTradeSequence[i];
        const std::wstring prefix = L"ChildTrade_" + std::to_wstring(i) + L"_";
        WriteIniInt(p.section, prefix + L"Target", step.target);
        WriteIniInt(p.section, prefix + L"MainRef", step.mainRef);
        WriteIniInt(p.section, prefix + L"Kind", step.kind);
        WriteIniText(p.section, prefix + L"Desc", step.description);
        WriteIniInt(p.section, prefix + L"X", step.point.valid ? step.point.x : -1);
        WriteIniInt(p.section, prefix + L"Y", step.point.valid ? step.point.y : -1);
        WriteIniInt(p.section, prefix + L"W", step.point.valid ? step.point.baseW : 0);
        WriteIniInt(p.section, prefix + L"H", step.point.valid ? step.point.baseH : 0);
        WriteIniInt(p.section, prefix + L"Delay", step.delayMs);
        WriteIniInt(p.section, prefix + L"Repeat", step.repeat);
        WriteIniInt(p.section, prefix + L"GroupId", step.groupId);
        WriteIniInt(p.section, prefix + L"GroupRepeat", step.groupRepeat);
    }
    FlushIni();
}


std::wstring SpotsPath() { return ExeDir() + L"\\ThanLongCleanRoute.spots.tsv"; }

std::wstring Utf8ToWide(const std::string& input) {
    if (input.empty()) return {};
    const int needed = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input.data(),
                                           static_cast<int>(input.size()), nullptr, 0);
    if (needed <= 0) return {};
    std::wstring out(static_cast<std::size_t>(needed), L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input.data(), static_cast<int>(input.size()),
                        out.data(), needed);
    return out;
}

std::string WideToUtf8(const std::wstring& input) {
    if (input.empty()) return {};
    const int needed = WideCharToMultiByte(CP_UTF8, 0, input.data(), static_cast<int>(input.size()),
                                           nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return {};
    std::string out(static_cast<std::size_t>(needed), '\0');
    WideCharToMultiByte(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), out.data(), needed,
                        nullptr, nullptr);
    return out;
}

std::wstring SanitizeSpotName(std::wstring name) {
    for (wchar_t& c : name) {
        if (c == L'\t' || c == L'\r' || c == L'\n') c = L' ';
    }
    while (!name.empty() && name.front() == L' ') name.erase(name.begin());
    while (!name.empty() && name.back() == L' ') name.pop_back();
    return name;
}

std::vector<std::wstring> SplitSpotLine(const std::wstring& line) {
    wchar_t separator = L'\t';
    if (line.find(L'\t') == std::wstring::npos) {
        if (line.find(L'|') != std::wstring::npos) separator = L'|';
        else if (line.find(L';') != std::wstring::npos) separator = L';';
        else return {};
    }
    std::vector<std::wstring> fields;
    std::size_t start = 0;
    while (start <= line.size()) {
        const std::size_t pos = line.find(separator, start);
        if (pos == std::wstring::npos) {
            fields.push_back(line.substr(start));
            break;
        }
        fields.push_back(line.substr(start, pos - start));
        start = pos + 1;
    }
    return fields;
}

int FindSpotIndex(const std::vector<TargetProfile>& spots, const std::wstring& name);

std::vector<TargetProfile> LoadSharedSpots() {
    std::vector<TargetProfile> out;
    const std::wstring path = SpotsPath();
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return out;
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(h, &size) || size.QuadPart <= 0 || size.QuadPart > 4 * 1024 * 1024) {
        CloseHandle(h);
        return out;
    }
    std::string bytes(static_cast<std::size_t>(size.QuadPart), '\0');
    DWORD read = 0;
    if (!ReadFile(h, bytes.data(), static_cast<DWORD>(bytes.size()), &read, nullptr)) {
        CloseHandle(h);
        return out;
    }
    CloseHandle(h);
    bytes.resize(read);

    std::wstring text;
    if (bytes.size() >= 2 && static_cast<unsigned char>(bytes[0]) == 0xFF &&
        static_cast<unsigned char>(bytes[1]) == 0xFE) {
        const std::size_t wcharCount = (bytes.size() - 2) / 2;
        text.resize(wcharCount);
        for (std::size_t i = 0; i < wcharCount; ++i) {
            const unsigned char lo = static_cast<unsigned char>(bytes[2 + i * 2]);
            const unsigned char hi = static_cast<unsigned char>(bytes[3 + i * 2]);
            text[i] = static_cast<wchar_t>(lo | (static_cast<unsigned int>(hi) << 8));
        }
    } else {
        if (bytes.size() >= 3 && static_cast<unsigned char>(bytes[0]) == 0xEF &&
            static_cast<unsigned char>(bytes[1]) == 0xBB && static_cast<unsigned char>(bytes[2]) == 0xBF) {
            bytes.erase(0, 3);
        }
        text = Utf8ToWide(bytes);
    }
    std::size_t lineStart = 0;
    while (lineStart <= text.size()) {
        std::size_t lineEnd = text.find(L'\n', lineStart);
        if (lineEnd == std::wstring::npos) lineEnd = text.size();
        std::wstring line = text.substr(lineStart, lineEnd - lineStart);
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (!line.empty() && line[0] != L'#') {
            const auto f = SplitSpotLine(line);
            if (f.size() >= 4) {
                TargetProfile t{};
                const bool firstNumeric = !f[0].empty() && (f[0][0] >= L'0' && f[0][0] <= L'9');
                if (firstNumeric) {
                    t.mapID = _wtoi(f[0].c_str());
                    t.x = _wtoi(f[1].c_str());
                    t.y = _wtoi(f[2].c_str());
                    t.name = SanitizeSpotName(f[3]);
                } else {
                    t.name = SanitizeSpotName(f[0]);
                    t.mapID = _wtoi(f[1].c_str());
                    t.x = _wtoi(f[2].c_str());
                    t.y = _wtoi(f[3].c_str());
                }
                t.valid = !t.name.empty() && t.mapID > 0;
                if (t.valid && FindSpotIndex(out, t.name) < 0) out.push_back(std::move(t));
            }
        }
        if (lineEnd == text.size()) break;
        lineStart = lineEnd + 1;
    }
    return out;
}

void SaveSharedSpots(const std::vector<TargetProfile>& spots) {
    std::wstring wide;
    for (const auto& spot : spots) {
        if (!spot.valid || spot.mapID <= 0 || spot.name.empty()) continue;
        wide += SanitizeSpotName(spot.name) + L"\t" + std::to_wstring(spot.mapID) + L"\t" +
                std::to_wstring(spot.x) + L"\t" + std::to_wstring(spot.y) + L"\r\n";
    }
    const std::string bytes = WideToUtf8(wide);
    const std::wstring path = SpotsPath();
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    if (!bytes.empty()) (void)WriteFile(h, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr);
    CloseHandle(h);
}

int FindSpotIndex(const std::vector<TargetProfile>& spots, const std::wstring& name) {
    if (name.empty()) return -1;
    for (std::size_t i = 0; i < spots.size(); ++i) {
        if (_wcsicmp(spots[i].name.c_str(), name.c_str()) == 0) return static_cast<int>(i);
    }
    return -1;
}

std::wstring GetText(HWND h) {
    const int n = GetWindowTextLengthW(h);
    std::wstring out(static_cast<std::size_t>(n) + 1, L'\0');
    if (n > 0) GetWindowTextW(h, out.data(), n + 1);
    out.resize(static_cast<std::size_t>(n));
    return out;
}

void SetText(HWND h, const std::wstring& s) { SetWindowTextW(h, s.c_str()); }

bool HasModule(DWORD pid, const wchar_t* name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap == INVALID_HANDLE_VALUE) return false;
    MODULEENTRY32W e{};
    e.dwSize = sizeof(e);
    bool found = false;
    if (Module32FirstW(snap, &e)) {
        do {
            if (_wcsicmp(e.szModule, name) == 0) { found = true; break; }
        } while (Module32NextW(snap, &e));
    }
    CloseHandle(snap);
    return found;
}

BOOL CALLBACK EnumGameWindows(HWND hwnd, LPARAM param) {
    if (!IsWindowVisible(hwnd) || GetWindowTextLengthW(hwnd) <= 0) return TRUE;
    DWORD pid = 0;
    const DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || !tid || !HasModule(pid, kGameModule)) return TRUE;
    auto* out = reinterpret_cast<std::vector<GameClient>*>(param);
    for (const auto& g : *out) if (g.pid == pid) return TRUE;
    wchar_t title[512]{};
    GetWindowTextW(hwnd, title, _countof(title));
    out->push_back({pid, tid, hwnd, title});
    return TRUE;
}

std::vector<GameClient> FindClients() {
    std::vector<GameClient> out;
    EnumWindows(EnumGameWindows, reinterpret_cast<LPARAM>(&out));
    std::sort(out.begin(), out.end(), [](const GameClient& a, const GameClient& b){ return a.pid < b.pid; });
    return out;
}

class BridgeClient {
public:
    BridgeClient() = default;
    BridgeClient(const BridgeClient&) = delete;
    BridgeClient& operator=(const BridgeClient&) = delete;
    ~BridgeClient() { Close(); }

    bool Attach(const GameClient& game, std::wstring& error) {
        Close();
        game_ = game;
        wchar_t mappingName[96]{};
        MappingName(game.pid, mappingName, _countof(mappingName));
        mapping_ = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
                                      sizeof(SharedBlock), mappingName);
        if (!mapping_) { error = L"Không tạo được shared memory"; return false; }
        shared_ = reinterpret_cast<SharedBlock*>(MapViewOfFile(mapping_, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedBlock)));
        if (!shared_) { error = L"Không map được shared memory"; Close(); return false; }
        ZeroMemory(shared_, sizeof(*shared_));
        shared_->magic = kMagic;
        shared_->protocolVersion = kProtocolVersion;
        shared_->targetPid = game.pid;
        shared_->targetWindowThreadId = game.threadId;

        const std::wstring path = ExeDir() + L"\\ThanLongCleanRouteBridge.dll";
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            error = L"Thiếu ThanLongCleanRouteBridge.dll cạnh EXE";
            Close();
            return false;
        }
        SetLastError(ERROR_SUCCESS);
        localDll_ = LoadLibraryW(path.c_str());
        const DWORD loadError = GetLastError();
        if (!localDll_) {
            error = L"Có Bridge DLL nhưng LoadLibrary thất bại Win32=" + std::to_wstring(loadError);
            Close();
            return false;
        }
        HOOKPROC proc = nullptr;
        if (!ResolveProc(localDll_, "TlcGetMessageHook", proc)) {
            error = L"Bridge DLL thiếu TlcGetMessageHook";
            Close();
            return false;
        }
        hook_ = SetWindowsHookExW(WH_GETMESSAGE, proc, localDll_, game.threadId);
        if (!hook_) {
            error = L"Không hook được game; hãy chạy tool cùng quyền với game";
            Close();
            return false;
        }
        if (!PostThreadMessageW(game.threadId, kWakeMessage, 0, 0)) {
            error = L"Không đánh thức được message thread game";
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
        pendingSeq_ = 0;
        pendingWakeTick_ = 0;
    }

    bool AttachedTo(DWORD pid) const { return attached_ && game_.pid == pid; }
    bool Attached() const { return attached_; }

    bool Call(Command command, int a0, int a1, int a2, Response& out,
              std::wstring& error, DWORD timeoutMs = 1000) {
        if (!attached_ || !shared_) { error = L"Bridge chưa attach"; return false; }

        // Never overwrite a request that timed out on the controller side but may still
        // be executing on the game thread. A late completion is discarded safely; until
        // then only re-post the SAME wake message when the bridge is not busy.
        if (pendingSeq_ > 0) {
            if (shared_->completedSeq == pendingSeq_) {
                MemoryBarrier();
                pendingSeq_ = 0;
                pendingWakeTick_ = 0;
            } else {
                const DWORD now = GetTickCount();
                if (shared_->bridgeBusy == 0 &&
                    (pendingWakeTick_ == 0 || now - pendingWakeTick_ >= kBridgeNudgeMs)) {
                    (void)PostThreadMessageW(game_.threadId, kWakeMessage, 0, 0);
                    pendingWakeTick_ = now;
                }
                error = L"Bridge còn bận sau timeout; không gửi chồng request";
                return false;
            }
        }
        if (shared_->bridgeBusy != 0) {
            error = L"Bridge busy; không gửi chồng request";
            return false;
        }

        const LONG next = shared_->requestSeq + 1;
        shared_->request = {};
        shared_->request.command = static_cast<std::uint32_t>(command);
        shared_->request.arg0 = a0;
        shared_->request.arg1 = a1;
        shared_->request.arg2 = a2;
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
                pendingSeq_ = 0;
                pendingWakeTick_ = 0;
                out = shared_->response;
                if (!out.ok) {
                    error = out.detail[0] ? out.detail : L"Bridge trả lỗi";
                    return false;
                }
                return true;
            }
            Sleep(2);
        }
        pendingSeq_ = next;
        pendingWakeTick_ = GetTickCount();
        error = L"Bridge timeout; fail-closed";
        return false;
    }

private:
    GameClient game_{};
    HANDLE mapping_ = nullptr;
    SharedBlock* shared_ = nullptr;
    HMODULE localDll_ = nullptr;
    HHOOK hook_ = nullptr;
    bool attached_ = false;
    LONG pendingSeq_ = 0;
    DWORD pendingWakeTick_ = 0;
};

struct Account {
    GameClient game{};
    BridgeClient bridge{};
    Snapshot snapshot{};
    bool snapshotValid = false;
    std::wstring displayName;
    AccountProfile profile{};
    RuntimeState runtime{};

    // Lifecycle latch intentionally lives OUTSIDE RuntimeState. ResetRuntime() may wipe
    // every automation phase at death/alive boundaries without forgetting that both
    // snapshots still belong to the same death session.
    bool deathSessionLatched = false;

    // Rotation metrics intentionally live OUTSIDE RuntimeState so death/alive cold
    // resets do not erase the rolling death window or productive-train timer.
    std::vector<DWORD> rotationDeathTicks{};
    DWORD rotationMetricTick = 0;
    std::uint64_t rotationActiveTrainMs = 0;
    bool rotationBagWasFull = false;

    // Trade coordinator owns only the paired MAIN/CON while a transaction is active.
    // Snapshot polling continues; normal route/death FSM resumes immediately after abort/release.
    bool tradeHeld = false;

    // Adaptive Step 5 value restored from v0.5 for the fixed-slot internal seller.
    // It intentionally lives outside RuntimeState so death/alive cold resets and the
    // next completed train trip retain the count learned from stable FreeBagSpace.
    int sellStep5LearnedRepeat = -1;
};


std::wstring TradeRoleLabel(int role) {
    switch (role) {
        case 1: return L"MAIN";
        case 2: return L"CON 1";
        case 3: return L"CON 2";
        case 4: return L"CON 3";
        case 5: return L"CON 4";
        case 6: return L"CON 5";
        case 7: return L"CON 6";
        default: return L"-";
    }
}

std::wstring PointDescription(const ClickPoint& p) {
    if (!p.valid) return L"CHƯA LẤY";
    return std::to_wstring(p.x) + L"," + std::to_wstring(p.y) + L" @ " +
           std::to_wstring(p.baseW) + L"x" + std::to_wstring(p.baseH);
}

bool ScaleClickPoint(const GameClient& game, const ClickPoint& saved, POINT& point, std::wstring& error) {
    if (!saved.valid) { error = L"Chưa lấy tọa độ click"; return false; }
    if (!game.window || !IsWindow(game.window)) { error = L"Cửa sổ game không còn tồn tại"; return false; }
    RECT rc{};
    if (!GetClientRect(game.window, &rc)) { error = L"Không đọc được client rect"; return false; }
    const int width = rc.right - rc.left;
    const int height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0 || saved.baseW <= 0 || saved.baseH <= 0) {
        error = L"Kích thước cửa sổ không hợp lệ";
        return false;
    }
    point.x = MulDiv(saved.x, width, saved.baseW);
    point.y = MulDiv(saved.y, height, saved.baseH);
    if (point.x < 0 || point.y < 0 || point.x >= width || point.y >= height) {
        error = L"Tọa độ sau scale nằm ngoài cửa sổ";
        return false;
    }
    return true;
}

bool NormalizeClickPointForBridge(const GameClient& game, const ClickPoint& saved,
                                  int& normalizedX, int& normalizedY,
                                  std::wstring& error) {
    POINT point{};
    if (!ScaleClickPoint(game, saved, point, error)) return false;
    RECT rc{};
    if (!GetClientRect(game.window, &rc)) {
        error = L"Không đọc được client rect để chuẩn hóa tọa độ";
        return false;
    }
    const int width = rc.right - rc.left;
    const int height = rc.bottom - rc.top;
    normalizedX = fixed_slot_sell_logic::NormalizeClientCoordinate(point.x, width);
    normalizedY = fixed_slot_sell_logic::NormalizeClientCoordinate(point.y, height);
    if (normalizedX < 0 || normalizedY < 0) {
        error = L"Không chuẩn hóa được tọa độ UI nội bộ";
        return false;
    }
    return true;
}

bool Elapsed(DWORD now, DWORD since, DWORD delay) {
    return since != 0 && now - since >= delay;
}

void ResetRuntime(RuntimeState& r) {
    const bool running = r.running;
    r = RuntimeState{};
    r.running = running;
    r.status = running ? L"Đang giám sát" : L"Đã dừng";
}

enum class TradePhase { Idle, Rendezvous, Sequence };

class App {
public:
    bool Create(HINSTANCE instance) {
        instance_ = instance;
        MigrateLegacyConfigIfNeeded();
        EnsureUnicodeIni();
        LoadTradeSettings();
        LoadTradeSequence();
        spots_ = LoadSharedSpots();
        INITCOMMONCONTROLSEX ic{sizeof(ic), ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES};
        InitCommonControlsEx(&ic);
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = L"ThanLongCleanRouteMultiWindow";
        if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;
        hwnd_ = CreateWindowExW(0, wc.lpszClassName, kTitle,
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT, 1060, 1030,
                                nullptr, nullptr, instance, this);
        return hwnd_ != nullptr;
    }

    void Show(int cmd) {
        ShowWindow(hwnd_, cmd);
        UpdateWindow(hwnd_);
    }

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        App* self = reinterpret_cast<App*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (msg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
            self = reinterpret_cast<App*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            self->hwnd_ = hwnd;
        }
        return self ? self->Handle(msg, wp, lp) : DefWindowProcW(hwnd, msg, wp, lp);
    }

    static LRESULT CALLBACK TradeEditorWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        App* self = reinterpret_cast<App*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (msg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
            self = reinterpret_cast<App*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            if (self) self->tradeEditor_ = hwnd;
        }
        if (!self) return DefWindowProcW(hwnd, msg, wp, lp);
        return self->HandleTradeEditor(hwnd, msg, wp, lp);
    }

    static LRESULT CALLBACK TradeSequenceListSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                                         UINT_PTR, DWORD_PTR refData) {
        App* self = reinterpret_cast<App*>(refData);
        if (!self) return DefSubclassProc(hwnd, msg, wp, lp);
        switch (msg) {
            case WM_LBUTTONDOWN: {
                const LRESULT result = DefSubclassProc(hwnd, msg, wp, lp);
                LVHITTESTINFO hit{};
                hit.pt.x = GET_X_LPARAM(lp);
                hit.pt.y = GET_Y_LPARAM(lp);
                const int row = ListView_HitTest(hwnd, &hit);
                if (row >= 0) {
                    self->tradeSeqDragSelecting_ = true;
                    self->tradeSeqDragStartRow_ = row;
                    self->SelectTradeSequenceDragRange(row, row);
                    SetCapture(hwnd);
                }
                return result;
            }
            case WM_MOUSEMOVE:
                if (self->tradeSeqDragSelecting_ && (wp & MK_LBUTTON)) {
                    LVHITTESTINFO hit{};
                    hit.pt.x = GET_X_LPARAM(lp);
                    hit.pt.y = GET_Y_LPARAM(lp);
                    int row = ListView_HitTest(hwnd, &hit);
                    if (row < 0) {
                        const int count = ListView_GetItemCount(hwnd);
                        RECT rc{}; GetClientRect(hwnd, &rc);
                        if (count > 0 && hit.pt.y < rc.top) row = 0;
                        else if (count > 0 && hit.pt.y >= rc.bottom) row = count - 1;
                    }
                    if (row >= 0) self->SelectTradeSequenceDragRange(self->tradeSeqDragStartRow_, row);
                    return 0;
                }
                break;
            case WM_LBUTTONUP:
                if (self->tradeSeqDragSelecting_) {
                    self->tradeSeqDragSelecting_ = false;
                    self->tradeSeqDragStartRow_ = -1;
                    if (GetCapture() == hwnd) ReleaseCapture();
                }
                break;
            case WM_CAPTURECHANGED:
                self->tradeSeqDragSelecting_ = false;
                self->tradeSeqDragStartRow_ = -1;
                break;
            case WM_NCDESTROY:
                RemoveWindowSubclass(hwnd, TradeSequenceListSubclassProc, 1);
                break;
        }
        return DefSubclassProc(hwnd, msg, wp, lp);
    }

    HWND MakeIn(HWND parent, const wchar_t* cls, const wchar_t* text, DWORD style,
                int x, int y, int w, int h, int id) {
        HWND hWnd = CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style,
                                    x, y, w, h, parent,
                                    reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), instance_, nullptr);
        if (hWnd) SendMessageW(hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
        return hWnd;
    }

    HWND Make(const wchar_t* cls, const wchar_t* text, DWORD style,
              int x, int y, int w, int h, int id) {
        return CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style,
                               x, y, w, h, hwnd_,
                               reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), nullptr, nullptr);
    }

    void AddListColumn(int index, int width, const wchar_t* text) {
        LVCOLUMNW c{};
        c.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        c.pszText = const_cast<wchar_t*>(text);
        c.cx = width;
        c.iSubItem = index;
        ListView_InsertColumn(clientList_, index, &c);
    }

    void AddMacroColumn(int index, int width, const wchar_t* text) {
        LVCOLUMNW c{};
        c.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        c.pszText = const_cast<wchar_t*>(text);
        c.cx = width;
        c.iSubItem = index;
        ListView_InsertColumn(sellMacroList_, index, &c);
    }

    void AddRotationColumn(int index, int width, const wchar_t* text) {
        LVCOLUMNW c{};
        c.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        c.pszText = const_cast<wchar_t*>(text);
        c.cx = width;
        c.iSubItem = index;
        ListView_InsertColumn(rotationList_, index, &c);
    }

    void BuildUi() {
        HFONT font = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
        auto addFont = [font](HWND h){ if (h) SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE); };

        mainTab_ = Make(WC_TABCONTROLW, L"", WS_CLIPSIBLINGS, 18, 4, 205, 32, IDC_MAIN_TAB); addFont(mainTab_);
        if (mainTab_) {
            TCITEMW tab{}; tab.mask = TCIF_TEXT;
            tab.pszText = const_cast<wchar_t*>(L"AUTO"); TabCtrl_InsertItem(mainTab_, 0, &tab);
            tab.pszText = const_cast<wchar_t*>(L"GIỚI THIỆU"); TabCtrl_InsertItem(mainTab_, 1, &tab);
            TabCtrl_SetCurSel(mainTab_, 0);
        }
        tradeStatus_ = Make(L"STATIC", L"ĐIỀU PHỐI: khởi động...", SS_LEFT | SS_CENTERIMAGE | WS_BORDER,
                            225, 6, 798, 27, 0); addFont(tradeStatus_);
        clientList_ = Make(WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_BORDER,
                           18, 40, 1005, 157, IDC_CLIENT_LIST);
        addFont(clientList_);
        ListView_SetExtendedListViewStyle(clientList_, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
        AddListColumn(0, 170, L"Nhân vật / RoleID");
        AddListColumn(1, 72, L"Vai trò");
        AddListColumn(2, 62, L"PID");
        AddListColumn(3, 220, L"Trạng thái");
        AddListColumn(4, 190, L"Map / X,Y / Túi");
        AddListColumn(5, 280, L"Bãi train");

        addFont(Make(L"BUTTON", L"QUÉT CLIENT", BS_PUSHBUTTON, 18, 205, 120, 30, IDC_SCAN));
        addFont(Make(L"BUTTON", L"BẮT ĐẦU ACC TICK", BS_DEFPUSHBUTTON, 148, 205, 175, 30, IDC_START_CHECKED));
        addFont(Make(L"BUTTON", L"DỪNG ACC TICK", BS_PUSHBUTTON, 333, 205, 155, 30, IDC_STOP_CHECKED));
        addFont(Make(L"STATIC", L"Vai trò:", SS_LEFT | SS_CENTERIMAGE, 500, 205, 55, 30, 0));
        tradeRoleCombo_ = Make(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_VSCROLL, 558, 205, 105, 220, IDC_TRADE_ROLE); addFont(tradeRoleCombo_);
        for (const wchar_t* r : {L"KHÔNG", L"MAIN", L"CON 1", L"CON 2", L"CON 3", L"CON 4", L"CON 5", L"CON 6"})
            SendMessageW(tradeRoleCombo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(r));
        selected_ = Make(L"STATIC", L"ACC ĐANG CHỈNH: chưa chọn", SS_LEFT | SS_CENTERIMAGE | WS_BORDER,
                         675, 205, 348, 30, IDC_SELECTED); addFont(selected_);

        live_ = Make(L"STATIC", L"STATE: chưa có", SS_LEFT | SS_CENTERIMAGE | WS_BORDER,
                     18, 243, 510, 38, IDC_LIVE); addFont(live_);
        tradeEnable_ = Make(L"BUTTON", tradeEnabled_ ? L"DỒN ĐỒ: BẬT" : L"DỒN ĐỒ: TẮT",
                            BS_PUSHBUTTON, 538, 247, 120, 27, IDC_CONSOLIDATE_TOGGLE); addFont(tradeEnable_);
        addFont(Make(L"STATIC", L"CON FULL = 0 ô", 0, 663, 252, 92, 22, 0));
        addFont(Make(L"STATIC", L"MAIN bán ≤", 0, 758, 252, 70, 22, 0));
        mainSellThresholdEdit_ = Make(L"EDIT", std::to_wstring(mainSellThreshold_).c_str(), WS_BORDER | ES_NUMBER | ES_CENTER, 828, 247, 42, 27, IDC_MAIN_SELL_THRESHOLD); addFont(mainSellThresholdEdit_);
        sellSequenceButton_ = Make(L"BUTTON", L"MACRO BÁN CŨ", BS_PUSHBUTTON, 878, 247, 145, 27, IDC_SELL_SEQUENCE); addFont(sellSequenceButton_);
        tradeRendezvousCaptureButton_ = Make(L"BUTTON", L"TỌA GD • LẤY", BS_PUSHBUTTON, 538, 273, 110, 24, IDC_TRADE_RENDEZVOUS_CAPTURE); addFont(tradeRendezvousCaptureButton_);
        tradeRendezvousLabel_ = Make(L"STATIC", L"CHƯA LẤY TỌA GD", SS_LEFT | SS_CENTERIMAGE | WS_BORDER, 655, 273, 210, 24, 0); addFont(tradeRendezvousLabel_);
        mainTradeSequenceButton_ = Make(L"BUTTON", L"CHUỖI GD MAIN", BS_PUSHBUTTON, 872, 273, 151, 24, IDC_MAIN_TRADE_SEQUENCE); addFont(mainTradeSequenceButton_);
        childTradeSequenceButton_ = Make(L"BUTTON", L"CHUỖI GD ACC CON", BS_PUSHBUTTON, 872, 273, 151, 24, IDC_CHILD_TRADE_SEQUENCE); addFont(childTradeSequenceButton_);

        addFont(Make(L"STATIC", L"SETTING RIÊNG ACC", 0, 18, 290, 150, 20, 0));
        addFont(Make(L"STATIC", L"GD CON: tự đổi acc khi pass cuối làm MAIN nhận ≤8 slot", 0, 560, 288, 463, 20, 0));
        addFont(Make(L"STATIC", L"Bãi:", 0, 18, 316, 45, 22, 0));
        spotCombo_ = Make(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_VSCROLL, 63, 312, 220, 240, IDC_SPOT_COMBO); addFont(spotCombo_);
        addFont(Make(L"STATIC", L"Tên lưu:", 0, 292, 316, 60, 22, 0));
        targetName_ = Make(L"EDIT", L"", WS_BORDER | ES_AUTOHSCROLL, 352, 312, 135, 27, IDC_TARGET_NAME); addFont(targetName_);
        addFont(Make(L"BUTTON", L"LƯU/CẬP NHẬT", BS_PUSHBUTTON, 497, 312, 150, 28, IDC_SAVE_TARGET));
        addFont(Make(L"BUTTON", L"XÓA BÃI", BS_PUSHBUTTON, 657, 312, 90, 28, IDC_DELETE_SPOT));
        targetText_ = Make(L"STATIC", L"CHƯA CHỌN", SS_LEFT | SS_CENTERIMAGE | WS_BORDER, 757, 312, 266, 28, IDC_TARGET_TEXT); addFont(targetText_);

        addFont(Make(L"STATIC", L"Sai số:", 0, 18, 350, 55, 22, 0));
        tolerance_ = Make(L"EDIT", L"120", WS_BORDER | ES_NUMBER | ES_CENTER, 73, 346, 70, 27, IDC_TOLERANCE); addFont(tolerance_);
        enableRevive_ = Make(L"BUTTON", L"Tự Đầu thai", BS_AUTOCHECKBOX, 160, 347, 125, 24, IDC_ENABLE_REVIVE); addFont(enableRevive_);
        enableConfirm_ = Make(L"BUTTON", L"XN Lâu Lan mắc cổng", BS_AUTOCHECKBOX, 300, 347, 175, 24, IDC_ENABLE_CONFIRM); addFont(enableConfirm_);
        addFont(Make(L"STATIC", L"M5 • AutoPath đứng ≈3s → callback XN nội bộ", 0, 480, 350, 245, 22, 0));
        enableFight_ = Make(L"BUTTON", L"AUTO → Đánh quái", BS_AUTOCHECKBOX, 730, 347, 145, 24, IDC_ENABLE_FIGHT); addFont(enableFight_);
        addFont(Make(L"STATIC", L"Không foreground/không chiếm chuột", 0, 878, 350, 145, 22, 0));

        addFont(Make(L"STATIC", L"XOAY BÃI TRAIN — mặc định chỉ bãi đang chọn; chỉ bật xoay khi tự tick thêm bãi thứ 2", 0, 18, 382, 1005, 20, 0));
        rotationList_ = Make(WC_LISTVIEWW, L"", LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER,
                             18, 404, 1005, 90, IDC_ROTATION_LIST);
        addFont(rotationList_);
        ListView_SetExtendedListViewStyle(rotationList_, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
        AddRotationColumn(0, 515, L"Bãi train");
        AddRotationColumn(1, 95, L"Map");
        AddRotationColumn(2, 160, L"X,Y");
        AddRotationColumn(3, 220, L"Ghi chú");

        addFont(Make(L"STATIC", L"Đổi bãi nếu chết quá", 0, 18, 500, 120, 22, 0));
        rotateDeathLimit_ = Make(L"EDIT", L"10", WS_BORDER | ES_NUMBER | ES_CENTER, 140, 497, 45, 27, IDC_ROTATE_DEATH_LIMIT); addFont(rotateDeathLimit_);
        addFont(Make(L"STATIC", L"lần /", 0, 190, 500, 38, 22, 0));
        rotateDeathWindow_ = Make(L"EDIT", L"10", WS_BORDER | ES_NUMBER | ES_CENTER, 230, 497, 45, 27, IDC_ROTATE_DEATH_WINDOW); addFont(rotateDeathWindow_);
        addFont(Make(L"STATIC", L"phút", 0, 280, 500, 38, 22, 0));
        addFont(Make(L"STATIC", L"• Đổi bãi nếu chưa FULL túi trong", 0, 335, 500, 190, 22, 0));
        rotateNoFullBag_ = Make(L"EDIT", L"15", WS_BORDER | ES_NUMBER | ES_CENTER, 530, 497, 45, 27, IDC_ROTATE_NO_BAG); addFont(rotateNoFullBag_);
        addFont(Make(L"STATIC", L"phút train thực • 1 bãi = không đổi • nhiều bãi = vòng lại bãi 1", 0, 580, 500, 443, 22, 0));

        addFont(Make(L"STATIC", L"5 ĐIỂM F8 — AUTO/ĐÁNH QUÁI/DỪNG dùng InputSync; XN/Đầu thai dùng callback semantic", 0, 18, 530, 760, 20, 0));
        addFont(Make(L"BUTTON", L"LẤY 5 CLICK CỦA ACC...", BS_PUSHBUTTON, 755, 526, 268, 27, IDC_COPY_CLICKS));
        const int rowY[5] = {552, 578, 604, 630, 656};
        const int pointIds[5] = {IDC_POINT_CONFIRM, IDC_POINT_REVIVE, IDC_POINT_AUTO, IDC_POINT_ATTACK, IDC_POINT_STOP_AUTO_2};
        const int captureIds[5] = {IDC_CAPTURE_CONFIRM, IDC_CAPTURE_REVIVE, IDC_CAPTURE_AUTO, IDC_CAPTURE_ATTACK, IDC_CAPTURE_STOP_AUTO_2};
        const int testIds[5] = {IDC_TEST_CONFIRM, IDC_TEST_REVIVE, IDC_TEST_AUTO, IDC_TEST_ATTACK, IDC_TEST_STOP_AUTO_2};
        for (int i = 0; i < 5; ++i) {
            addFont(Make(L"STATIC", kClickLabels[static_cast<std::size_t>(i)], SS_LEFT | SS_CENTERIMAGE, 18, rowY[i], 150, 24, 0));
            pointLabels_[static_cast<std::size_t>(i)] = Make(L"STATIC", L"CHƯA LẤY", SS_LEFT | SS_CENTERIMAGE | WS_BORDER, 172, rowY[i], 430, 24, pointIds[i]);
            addFont(pointLabels_[static_cast<std::size_t>(i)]);
            addFont(Make(L"BUTTON", L"LẤY F8", BS_PUSHBUTTON, 612, rowY[i], 115, 24, captureIds[i]));
            addFont(Make(L"BUTTON", L"TEST", BS_PUSHBUTTON, 737, rowY[i], 90, 24, testIds[i]));
        }

        enableSell_ = Make(L"BUTTON", L"AUTO BÁN ĐỒ KHI TÚI FULL", BS_AUTOCHECKBOX, 18, 712, 220, 25, IDC_ENABLE_SELL); addFont(enableSell_);
        addFont(Make(L"STATIC", L"NPC bán:", 0, 250, 715, 65, 22, 0));
        sellNpcCombo_ = Make(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_VSCROLL, 315, 710, 250, 180, IDC_SELL_NPC); addFont(sellNpcCombo_);
        for (const auto& npc : kSellNpcs) SendMessageW(sellNpcCombo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(npc.name));
        addFont(Make(L"STATIC", L"X:", 0, 574, 715, 18, 22, 0));
        sellNpcX_ = Make(L"EDIT", L"", WS_BORDER | ES_NUMBER | ES_CENTER, 592, 710, 58, 27, IDC_SELL_NPC_X); addFont(sellNpcX_);
        addFont(Make(L"STATIC", L"Y:", 0, 658, 715, 18, 22, 0));
        sellNpcY_ = Make(L"EDIT", L"", WS_BORDER | ES_NUMBER | ES_CENTER, 676, 710, 58, 27, IDC_SELL_NPC_Y); addFont(sellNpcY_);
        addFont(Make(L"BUTTON", L"LẤY VỊ TRÍ", BS_PUSHBUTTON, 742, 710, 112, 27, IDC_SELL_NPC_CAPTURE));
        sellNpcPosText_ = Make(L"STATIC", L"CHƯA LẤY", SS_LEFT | SS_CENTERIMAGE | WS_BORDER, 862, 710, 161, 27, IDC_SELL_NPC_POS); addFont(sellNpcPosText_);

        sellMacroList_ = Make(WC_LISTVIEWW, L"", LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER, 18, 742, 1005, 72, IDC_SELL_MACRO_LIST);
        addFont(sellMacroList_);
        ListView_SetExtendedListViewStyle(sellMacroList_, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        AddMacroColumn(0, 36, L"#");
        AddMacroColumn(1, 400, L"Mô tả bước bán");
        AddMacroColumn(2, 230, L"Tọa độ");
        AddMacroColumn(3, 110, L"Delay ms");
        AddMacroColumn(4, 90, L"Lặp");

        sellMacroControls_.push_back(sellMacroList_);
        sellMacroControls_.push_back(Make(L"BUTTON", L"+ THÊM", BS_PUSHBUTTON, 18, 818, 82, 27, IDC_SELL_ADD));
        sellMacroControls_.push_back(Make(L"BUTTON", L"- XÓA", BS_PUSHBUTTON, 108, 818, 82, 27, IDC_SELL_DELETE));
        sellDesc_ = Make(L"EDIT", L"", WS_BORDER | ES_AUTOHSCROLL, 202, 818, 260, 27, IDC_SELL_DESC); addFont(sellDesc_); sellMacroControls_.push_back(sellDesc_);
        sellDelay_ = Make(L"EDIT", L"600", WS_BORDER | ES_NUMBER | ES_CENTER, 470, 818, 75, 27, IDC_SELL_DELAY); addFont(sellDelay_); sellMacroControls_.push_back(sellDelay_);
        sellRepeat_ = Make(L"EDIT", L"1", WS_BORDER | ES_NUMBER | ES_CENTER, 553, 818, 55, 27, IDC_SELL_REPEAT); addFont(sellRepeat_); sellMacroControls_.push_back(sellRepeat_);
        sellMacroControls_.push_back(Make(L"BUTTON", L"LƯU DÒNG", BS_PUSHBUTTON, 616, 818, 100, 27, IDC_SELL_SAVE));
        sellMacroControls_.push_back(Make(L"BUTTON", L"LẤY DÒNG (F8)", BS_PUSHBUTTON, 724, 818, 130, 27, IDC_SELL_CAPTURE));
        sellMacroControls_.push_back(Make(L"BUTTON", L"TEST DÒNG", BS_PUSHBUTTON, 862, 818, 112, 27, IDC_SELL_TEST));
        sellRecordButton_ = Make(L"BUTTON", L"REC", BS_PUSHBUTTON, 18, 850, 92, 27, IDC_SELL_REC); addFont(sellRecordButton_); sellMacroControls_.push_back(sellRecordButton_);
        sellMacroControls_.push_back(Make(L"BUTTON", L"SAO CHÉP", BS_PUSHBUTTON, 118, 850, 104, 27, IDC_SELL_COPY));
        sellMacroControls_.push_back(Make(L"BUTTON", L"DÁN", BS_PUSHBUTTON, 230, 850, 80, 27, IDC_SELL_PASTE));
        sellMacroControls_.push_back(Make(L"BUTTON", L"LẤY CHUỖI CỦA ACC...", BS_PUSHBUTTON, 318, 850, 190, 27, IDC_SELL_COPY_ACCOUNT));
        sellRecordStatus_ = Make(L"STATIC", L"REC: sẵn sàng • chọn một hoặc nhiều dòng để SAO CHÉP", SS_LEFT | SS_CENTERIMAGE, 322, 850, 652, 27, 0); addFont(sellRecordStatus_); sellMacroControls_.push_back(sellRecordStatus_);
        for (HWND h : sellMacroControls_) if (h) ShowWindow(h, SW_HIDE);

        logCaption_ = Make(L"STATIC", L"LOG / BỘ ĐIỀU PHỐI", 0, 18, 742, 190, 20, 0); addFont(logCaption_);
        log_ = Make(L"EDIT", L"", WS_BORDER | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL, 18, 764, 1005, 159, IDC_LOG); addFont(log_);

        aboutControls_.push_back(Make(L"STATIC", L"GIỚI THIỆU", SS_CENTER | SS_CENTERIMAGE, 150, 250, 745, 55, 0));
        aboutControls_.push_back(Make(L"STATIC", L"Thiết kế và phát triển bởi Thắng Nguyễn - ĐỒ LONG",
                                          SS_CENTER | SS_CENTERIMAGE | WS_BORDER, 150, 330, 745, 65, 0));
        aboutControls_.push_back(Make(L"STATIC", L"Thần Long Item Consolidator • v0.6.1.6",
                                          SS_CENTER | SS_CENTERIMAGE, 150, 415, 745, 36, 0));
        for (HWND h : aboutControls_) { addFont(h); if (h) ShowWindow(h, SW_HIDE); }

        if (!RegisterHotKey(hwnd_, kCaptureHotkeyId, MOD_NOREPEAT, VK_F8)) {
            Log(L"CẢNH BÁO: không đăng ký được F8 global.");
        }
        if (!RegisterHotKey(hwnd_, kPauseHotkeyId, MOD_NOREPEAT, VK_F4)) {
            Log(L"CẢNH BÁO: không đăng ký được F4 global.");
        }
        Log(L"HIDDEN ACTION ENGINE ON • auto-click dùng InputSync nội bộ; không chiếm chuột Windows.");
        SetTimer(hwnd_, kTimer, 250, nullptr);
        UpdateTradeRendezvousLabel();
        UpdateRoleActionButtons();
        ScanClients();
    }

    bool IsAboutControl(HWND h) const {
        return std::find(aboutControls_.begin(), aboutControls_.end(), h) != aboutControls_.end();
    }

    void SwitchMainTab(int index) {
        if (!mainTab_) return;
        index = (index == 1) ? 1 : 0;
        if (index == mainTabIndex_) return;

        if (index == 1) {
            autoTabVisibility_.clear();
            for (HWND child = GetWindow(hwnd_, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
                if (child == mainTab_ || IsAboutControl(child)) continue;
                autoTabVisibility_.push_back({child, IsWindowVisible(child) != FALSE});
                ShowWindow(child, SW_HIDE);
            }
            for (HWND h : aboutControls_) if (h) ShowWindow(h, SW_SHOW);
        } else {
            for (HWND h : aboutControls_) if (h) ShowWindow(h, SW_HIDE);
            for (const auto& saved : autoTabVisibility_) {
                if (saved.first && IsWindow(saved.first)) ShowWindow(saved.first, saved.second ? SW_SHOW : SW_HIDE);
            }
            autoTabVisibility_.clear();
        }
        mainTabIndex_ = index;
    }

    Account* ActiveSellClickSequenceAccount() {
        for (auto& item : accounts_) {
            Account& a = *item;
            if (a.runtime.running && a.runtime.sellPhase == 6) return &a;
        }
        return nullptr;
    }

    void Log(const std::wstring& text) {
        if (!log_) return;
        SYSTEMTIME st{};
        GetLocalTime(&st);
        wchar_t prefix[32]{};
        wsprintfW(prefix, L"[%02u:%02u:%02u] ", st.wHour, st.wMinute, st.wSecond);
        std::wstring line = prefix + text + L"\r\n";
        const int len = GetWindowTextLengthW(log_);
        SendMessageW(log_, EM_SETSEL, len, len);
        SendMessageW(log_, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(line.c_str()));
        SendMessageW(log_, EM_SCROLLCARET, 0, 0);
    }

    std::wstring AccountTag(const Account& a) const {
        if (!a.displayName.empty()) return a.displayName + L"/PID " + std::to_wstring(a.game.pid);
        return L"PID " + std::to_wstring(a.game.pid);
    }

    void LogAccount(const Account& a, const std::wstring& text) {
        Log(L"[" + AccountTag(a) + L"] " + text);
    }

    int SelectedIndex() const {
        if (!clientList_) return -1;
        return ListView_GetNextItem(clientList_, -1, LVNI_SELECTED);
    }

    Account* SelectedAccount() {
        const int i = SelectedIndex();
        if (i < 0 || i >= static_cast<int>(accounts_.size())) return nullptr;
        return accounts_[static_cast<std::size_t>(i)].get();
    }

    Account* AccountByPid(DWORD pid) {
        for (auto& a : accounts_) if (a->game.pid == pid) return a.get();
        return nullptr;
    }

    static std::wstring ProfileSection(const Snapshot& s, DWORD pid) {
        if ((s.validMask & ValidIdentity) && s.roleID > 0) return L"Role_" + std::to_wstring(s.roleID);
        return L"PID_" + std::to_wstring(pid);
    }

    static std::wstring DisplayName(const Snapshot& s, DWORD pid) {
        std::wstring name = s.characterName[0] ? s.characterName : L"?";
        if ((s.validMask & ValidIdentity) && s.roleID > 0) {
            return name + L" • " + std::to_wstring(s.roleID);
        }
        return name + L" • PID " + std::to_wstring(pid);
    }

    bool EnsureAttach(Account& a, std::wstring& error) {
        if (a.bridge.AttachedTo(a.game.pid)) return true;
        if (!IsWindow(a.game.window)) { error = L"Cửa sổ game đã mất"; return false; }
        return a.bridge.Attach(a.game, error);
    }

    bool ReadSnapshot(Account& a, std::wstring& error, DWORD timeout = 850) {
        if (!EnsureAttach(a, error)) return false;
        Response r{};
        if (!a.bridge.Call(Command::ReadState, 0, 0, 0, r, error, timeout)) return false;
        a.snapshot = r.snapshot;
        a.snapshotValid = true;
        return true;
    }

    void ScanClients() {
        ReleaseTradeHolds();
        tradeTxn_ = TradeTxn{};
        captureSlot_ = ClickSlot::None;
        captureMacroIndex_ = -1;
        capturePid_ = 0;
        for (auto& a : accounts_) a->bridge.Close();
        accounts_.clear();
        ListView_DeleteAllItems(clientList_);

        const auto found = FindClients();
        for (const auto& game : found) {
            auto a = std::make_unique<Account>();
            a->game = game;
            std::wstring error;
            if (a->bridge.Attach(game, error)) {
                Response r{};
                if (a->bridge.Call(Command::ReadState, 0, 0, 0, r, error, 1200)) {
                    a->snapshot = r.snapshot;
                    a->snapshotValid = true;
                }
            }
            if (!a->snapshotValid) {
                a->snapshot = {};
                a->displayName = L"? • PID " + std::to_wstring(game.pid);
                Log(L"PID " + std::to_wstring(game.pid) + L": chưa đọc được identity: " + error);
            } else {
                a->displayName = DisplayName(a->snapshot, game.pid);
            }
            a->profile = LoadProfile(ProfileSection(a->snapshot, game.pid));
            if (a->profile.tradeRole >= 2) a->profile.enableSell = false;
            MigrateLegacySpot(a->profile);
            a->runtime.status = L"Đã dừng";
            accounts_.push_back(std::move(a));
        }

        for (std::size_t i = 0; i < accounts_.size(); ++i) InsertAccountRow(static_cast<int>(i), *accounts_[i]);
        RefreshSpotCombo();
        if (!accounts_.empty()) {
            ListView_SetItemState(clientList_, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(clientList_, 0, FALSE);
            LoadSelectedProfileToUi();
        } else {
            ClearEditor();
        }
        Log(L"Quét thấy " + std::to_wstring(accounts_.size()) + L" client GameAssembly.dll.");
    }

    void InsertAccountRow(int row, const Account& a) {
        LVITEMW item{};
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.iItem = row;
        item.iSubItem = 0;
        item.pszText = const_cast<wchar_t*>(a.displayName.c_str());
        item.lParam = static_cast<LPARAM>(a.game.pid);
        ListView_InsertItem(clientList_, &item);
        SetRowText(row, 2, std::to_wstring(a.game.pid));
        UpdateAccountRow(row, a);
    }

    void SetRowText(int row, int sub, const std::wstring& text) {
        ListView_SetItemText(clientList_, row, sub, const_cast<wchar_t*>(text.c_str()));
    }

    void UpdateAccountRow(int row, const Account& a) {
        SetRowText(row, 0, a.displayName);
        SetRowText(row, 1, TradeRoleLabel(a.profile.tradeRole));
        SetRowText(row, 2, std::to_wstring(a.game.pid));
        SetRowText(row, 3, (a.runtime.running ? L"RUN • " : L"STOP • ") + a.runtime.status);
        if (a.snapshotValid && (a.snapshot.validMask & (ValidMap | ValidPosition)) == (ValidMap | ValidPosition)) {
            std::wstring mapText = L"M" + std::to_wstring(a.snapshot.mapID) + L" • " +
                                   std::to_wstring(a.snapshot.x) + L"," + std::to_wstring(a.snapshot.y);
            if (a.snapshot.validMask & ValidBagSpace) mapText += L" • Trống " + std::to_wstring(a.snapshot.freeBagSpace);
            SetRowText(row, 4, mapText);
        } else {
            SetRowText(row, 4, L"?");
        }
        if (a.profile.target.valid) {
            SetRowText(row, 5, a.profile.target.name + L" • M" + std::to_wstring(a.profile.target.mapID) +
                             L" • " + std::to_wstring(a.profile.target.x) + L"," + std::to_wstring(a.profile.target.y) +
                             L" • vòng " + std::to_wstring(a.profile.rotationSpots.size()) + L" bãi");
        } else {
            SetRowText(row, 5, L"CHƯA CHỌN BÃI");
        }
    }


    void ResolveProfileTarget(AccountProfile& p) {
        const int index = FindSpotIndex(spots_, p.selectedSpot);
        if (index >= 0) {
            p.target = spots_[static_cast<std::size_t>(index)];
            p.target.valid = true;
        } else {
            p.target = {};
        }
    }

    bool RotationContains(const AccountProfile& p, const std::wstring& name) const {
        return std::any_of(p.rotationSpots.begin(), p.rotationSpots.end(), [&](const std::wstring& x){
            return _wcsicmp(x.c_str(), name.c_str()) == 0;
        });
    }

    void NormalizeRotationProfile(AccountProfile& p) {
        std::vector<std::wstring> clean;
        for (const auto& name : p.rotationSpots) {
            if (FindSpotIndex(spots_, name) < 0) continue;
            if (std::none_of(clean.begin(), clean.end(), [&](const std::wstring& x){ return _wcsicmp(x.c_str(), name.c_str()) == 0; })) {
                clean.push_back(name);
            }
        }
        p.rotationSpots = std::move(clean);
        if (p.rotationSpots.empty() && !p.selectedSpot.empty() && FindSpotIndex(spots_, p.selectedSpot) >= 0) {
            p.rotationSpots.push_back(p.selectedSpot);
        }
        if (!p.rotationSpots.empty() && (p.selectedSpot.empty() || !RotationContains(p, p.selectedSpot))) {
            p.selectedSpot = p.rotationSpots.front();
        }
        ResolveProfileTarget(p);
    }

    void RefreshRotationList() {
        if (!rotationList_) return;
        rotationUiLoading_ = true;
        ListView_DeleteAllItems(rotationList_);
        Account* a = SelectedAccount();
        for (std::size_t i = 0; i < spots_.size(); ++i) {
            const TargetProfile& spot = spots_[i];
            LVITEMW item{};
            item.mask = LVIF_TEXT;
            item.iItem = static_cast<int>(i);
            item.pszText = const_cast<wchar_t*>(spot.name.c_str());
            ListView_InsertItem(rotationList_, &item);
            const std::wstring map = L"M" + std::to_wstring(spot.mapID);
            const std::wstring xy = std::to_wstring(spot.x) + L"," + std::to_wstring(spot.y);
            ListView_SetItemText(rotationList_, static_cast<int>(i), 1, const_cast<wchar_t*>(map.c_str()));
            ListView_SetItemText(rotationList_, static_cast<int>(i), 2, const_cast<wchar_t*>(xy.c_str()));
            const std::wstring note = (a && _wcsicmp(a->profile.selectedSpot.c_str(), spot.name.c_str()) == 0) ? L"BÃI HIỆN TẠI" : L"";
            ListView_SetItemText(rotationList_, static_cast<int>(i), 3, const_cast<wchar_t*>(note.c_str()));
            if (a && RotationContains(a->profile, spot.name)) ListView_SetCheckState(rotationList_, static_cast<int>(i), TRUE);
        }
        rotationUiLoading_ = false;
    }

    void PersistRotationListFromUi(Account& a) {
        if (!rotationList_) return;
        std::vector<std::wstring> selected;
        const int count = std::min(ListView_GetItemCount(rotationList_), static_cast<int>(spots_.size()));
        for (int i = 0; i < count; ++i) {
            if (ListView_GetCheckState(rotationList_, i)) selected.push_back(spots_[static_cast<std::size_t>(i)].name);
        }
        if (selected.empty() && !a.profile.selectedSpot.empty() && FindSpotIndex(spots_, a.profile.selectedSpot) >= 0) {
            selected.push_back(a.profile.selectedSpot);
        }
        a.profile.rotationSpots = std::move(selected);
        NormalizeRotationProfile(a.profile);
    }

    void MigrateLegacySpot(AccountProfile& p) {
        if (p.selectedSpot.empty() && p.target.valid) p.selectedSpot = p.target.name;
        if (p.target.valid && !p.selectedSpot.empty()) {
            int index = FindSpotIndex(spots_, p.selectedSpot);
            if (index >= 0) {
                const TargetProfile& existing = spots_[static_cast<std::size_t>(index)];
                if (existing.mapID != p.target.mapID || existing.x != p.target.x || existing.y != p.target.y) {
                    p.selectedSpot += L" [M" + std::to_wstring(p.target.mapID) + L" " +
                                      std::to_wstring(p.target.x) + L"," + std::to_wstring(p.target.y) + L"]";
                    index = FindSpotIndex(spots_, p.selectedSpot);
                }
            }
            if (index < 0) {
                TargetProfile migrated = p.target;
                migrated.name = p.selectedSpot;
                migrated.valid = true;
                spots_.push_back(std::move(migrated));
                SaveSharedSpots(spots_);
            }
        }
        NormalizeRotationProfile(p);
    }

    void RefreshSpotCombo() {
        if (!spotCombo_) return;
        SendMessageW(spotCombo_, CB_RESETCONTENT, 0, 0);
        for (const auto& spot : spots_) {
            SendMessageW(spotCombo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(spot.name.c_str()));
        }
        Account* a = SelectedAccount();
        int select = -1;
        if (a) select = FindSpotIndex(spots_, a->profile.selectedSpot);
        SendMessageW(spotCombo_, CB_SETCURSEL, select, 0);
    }

    void SelectSharedSpotForAccount() {
        Account* a = SelectedAccount();
        if (!a) return;
        const LRESULT sel = SendMessageW(spotCombo_, CB_GETCURSEL, 0, 0);
        if (sel == CB_ERR || sel < 0 || static_cast<std::size_t>(sel) >= spots_.size()) return;
        const TargetProfile& spot = spots_[static_cast<std::size_t>(sel)];
        const std::wstring oldSpot = a->profile.selectedSpot;
        a->profile.selectedSpot = spot.name;
        a->profile.target = spot;
        // Selecting the top combobox defines the single default train spot. Rotation
        // becomes active only after the user explicitly checks at least one extra row below.
        a->profile.rotationSpots.clear();
        a->profile.rotationSpots.push_back(spot.name);
        NormalizeRotationProfile(a->profile);
        SetText(targetName_, spot.name);
        SaveProfile(a->profile);
        if (_wcsicmp(oldSpot.c_str(), spot.name.c_str()) != 0) {
            ResetRotationWindow(*a, GetTickCount());
            if (a->runtime.running) BeginTrainRecovery(*a, GetTickCount());
        }
        LoadSelectedProfileToUi();
        const int row = SelectedIndex();
        if (row >= 0) UpdateAccountRow(row, *a);
        LogAccount(*a, L"Đã chọn bãi chung: " + spot.name + L" • M" + std::to_wstring(spot.mapID) + L" • " +
                       std::to_wstring(spot.x) + L"," + std::to_wstring(spot.y));
    }

    void DeleteSelectedSharedSpot() {
        Account* a = SelectedAccount();
        if (!a) { Log(L"Chưa chọn acc"); return; }
        const LRESULT sel = SendMessageW(spotCombo_, CB_GETCURSEL, 0, 0);
        if (sel == CB_ERR || sel < 0 || static_cast<std::size_t>(sel) >= spots_.size()) {
            Log(L"Chưa chọn bãi chung để xóa");
            return;
        }
        const std::wstring name = spots_[static_cast<std::size_t>(sel)].name;
        spots_.erase(spots_.begin() + sel);
        SaveSharedSpots(spots_);
        for (auto& item : accounts_) {
            item->profile.rotationSpots.erase(std::remove_if(item->profile.rotationSpots.begin(), item->profile.rotationSpots.end(), [&](const std::wstring& x){
                return _wcsicmp(x.c_str(), name.c_str()) == 0;
            }), item->profile.rotationSpots.end());
            if (_wcsicmp(item->profile.selectedSpot.c_str(), name.c_str()) == 0) {
                item->profile.selectedSpot = item->profile.rotationSpots.empty() ? L"" : item->profile.rotationSpots.front();
            }
            NormalizeRotationProfile(item->profile);
            SaveProfile(item->profile);
        }
        RefreshSpotCombo();
        LoadSelectedProfileToUi();
        for (std::size_t i = 0; i < accounts_.size(); ++i) UpdateAccountRow(static_cast<int>(i), *accounts_[i]);
        Log(L"Đã xóa bãi chung: " + name);
    }

    int FocusedSelectedRow(HWND list) const {
        if (!list) return -1;
        const int focused = ListView_GetNextItem(list, -1, LVNI_FOCUSED);
        if (focused >= 0 && (ListView_GetItemState(list, focused, LVIS_SELECTED) & LVIS_SELECTED) != 0) return focused;
        return ListView_GetNextItem(list, -1, LVNI_SELECTED);
    }

    std::vector<int> SelectedRows(HWND list) const {
        std::vector<int> rows;
        if (!list) return rows;
        int row = -1;
        while ((row = ListView_GetNextItem(list, row, LVNI_SELECTED)) >= 0) rows.push_back(row);
        return rows;
    }

    void CopyClicksFromAnotherAccount() {
        Account* target = SelectedAccount();
        if (!target) { Log(L"LẤY 5 CLICK: chưa chọn acc đích."); return; }
        HMENU menu = CreatePopupMenu();
        if (!menu) return;
        std::vector<Account*> sources;
        for (auto& item : accounts_) {
            Account* source = item.get();
            if (!source || source->game.pid == target->game.pid) continue;
            int valid = 0;
            for (const ClickPoint& point : source->profile.points) if (point.valid) ++valid;
            if (valid == 0) continue;
            sources.push_back(source);
            const std::wstring label = AccountTag(*source) + L" • có " + std::to_wstring(valid) + L"/5 điểm";
            AppendMenuW(menu, MF_STRING, static_cast<UINT_PTR>(6000 + sources.size() - 1), label.c_str());
        }
        if (sources.empty()) {
            DestroyMenu(menu);
            LogAccount(*target, L"LẤY 5 CLICK: chưa có acc khác nào đã gán tọa độ.");
            return;
        }
        POINT screen{}; GetCursorPos(&screen);
        const int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
                                       screen.x, screen.y, 0, hwnd_, nullptr);
        DestroyMenu(menu);
        if (cmd < 6000 || static_cast<std::size_t>(cmd - 6000) >= sources.size()) return;
        Account* source = sources[static_cast<std::size_t>(cmd - 6000)];
        int copied = 0;
        for (std::size_t i = 0; i < target->profile.points.size(); ++i) {
            if (!source->profile.points[i].valid) continue; // Partial source never clears a valid target point.
            target->profile.points[i] = source->profile.points[i];
            ++copied;
        }
        SaveProfile(target->profile);
        LoadSelectedProfileToUi();
        LogAccount(*target, L"Đã lấy " + std::to_wstring(copied) + L"/5 CLICK từ " + AccountTag(*source) +
                           L" • điểm nguồn chưa gán không ghi đè điểm hiện tại.");
    }

    bool RecorderModeIsTrade(RecorderMode mode) const {
        return mode == RecorderMode::TradeMain || mode == RecorderMode::TradeChild;
    }

    int RecordedDelay(std::size_t index, int lastDefault) const {
        if (index + 1 >= recorderClicks_.size()) return lastDefault;
        const DWORD delta = recorderClicks_[index + 1].tick - recorderClicks_[index].tick;
        return std::clamp(static_cast<int>(delta), 50, 60000);
    }

    void UpdateRecorderUi(const std::wstring& status = L"") {
        const bool active = recorderMode_ != RecorderMode::None;
        if (sellRecordButton_) SetWindowTextW(sellRecordButton_, active && recorderMode_ == RecorderMode::Sell ? L"DỪNG REC" : L"REC");
        if (tradeRecordButton_) SetWindowTextW(tradeRecordButton_, active && RecorderModeIsTrade(recorderMode_) ? L"DỪNG REC" : L"REC");
        std::wstring text = status;
        if (text.empty()) text = active ? L"REC đang ghi thao tác tay..." : L"REC: sẵn sàng";
        if (sellRecordStatus_) SetWindowTextW(sellRecordStatus_, text.c_str());
        if (tradeRecordStatus_) SetWindowTextW(tradeRecordStatus_, text.c_str());
    }

    Account* RecorderAccountAtPoint(const POINT& screen) {
        HWND hit = WindowFromPoint(screen);
        HWND root = hit ? GetAncestor(hit, GA_ROOT) : nullptr;
        for (auto& item : accounts_) if (item && item->game.window == root) return item.get();
        return nullptr;
    }

    bool RecorderAllowsAccount(const Account& account) const {
        if (recorderMode_ == RecorderMode::Sell) return account.game.pid == recorderPrimaryPid_;
        if (recorderMode_ == RecorderMode::TradeMain) return account.profile.tradeRole == 1;
        if (recorderMode_ == RecorderMode::TradeChild) {
            return account.game.pid == recorderPrimaryPid_ || account.profile.tradeRole == 1;
        }
        return false;
    }

    void CaptureRecorderClick() {
        if (recorderMode_ == RecorderMode::None || recorderClicks_.size() >= 64) return;
        POINT screen{};
        if (!GetCursorPos(&screen)) return;
        Account* account = RecorderAccountAtPoint(screen);
        if (!account || !RecorderAllowsAccount(*account)) return; // Click on tool/other apps/other CON is intentionally ignored.
        POINT client = screen;
        if (!ScreenToClient(account->game.window, &client)) return;
        RECT rc{};
        if (!GetClientRect(account->game.window, &rc)) return;
        const int width = rc.right - rc.left, height = rc.bottom - rc.top;
        if (width <= 0 || height <= 0 || client.x < 0 || client.y < 0 || client.x >= width || client.y >= height) return;
        RecordedClick click{};
        click.pid = account->game.pid;
        click.point = ClickPoint{client.x, client.y, width, height, true};
        click.tick = GetTickCount();
        recorderClicks_.push_back(click);
        const std::wstring status = L"REC • " + std::to_wstring(recorderClicks_.size()) + L" click • vừa ghi " +
                                    AccountTag(*account) + L" @ " + PointDescription(click.point);
        UpdateRecorderUi(status);
        SetTradeStatus(L"RECORDING • FREEZE AUTO • " + status);
    }

    void PollRecorder() {
        if (recorderMode_ == RecorderMode::None) return;
        const bool down = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        if (down) {
            recorderMouseDown_ = true;
            return;
        }
        if (recorderMouseDown_) {
            recorderMouseDown_ = false;
            CaptureRecorderClick();
        }
    }

    int FindSharedMainStepByPoint(const ClickPoint& point) const {
        for (std::size_t i = 0; i < mainTradeSequence_.size(); ++i) {
            const ClickPoint& p = mainTradeSequence_[i].point;
            if (p.valid && p.x == point.x && p.y == point.y && p.baseW == point.baseW && p.baseH == point.baseH) return static_cast<int>(i);
        }
        return -1;
    }

    void CommitRecordedSell(DWORD pid) {
        Account* account = AccountByPid(pid);
        if (!account) return;
        const std::size_t room = account->profile.sellMacro.size() < 64 ? 64 - account->profile.sellMacro.size() : 0;
        const std::size_t count = std::min(room, recorderClicks_.size());
        const std::size_t first = account->profile.sellMacro.size();
        for (std::size_t i = 0; i < count; ++i) {
            SellMacroStep step{};
            step.description = L"REC bước " + std::to_wstring(first + i + 1);
            step.point = recorderClicks_[i].point;
            step.delayMs = RecordedDelay(i, 600);
            step.repeat = 1;
            account->profile.sellMacro.push_back(step);
        }
        SaveProfile(account->profile);
        RefreshSellMacroList();
        if (count > 0 && sellMacroList_) {
            const int row = static_cast<int>(first);
            ListView_SetItemState(sellMacroList_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(sellMacroList_, row, FALSE);
            LoadSelectedMacroEditor();
        }
        LogAccount(*account, L"REC BÁN ĐỒ → đã chuyển " + std::to_wstring(count) + L" click thành dòng tọa độ editable.");
    }

    void CommitRecordedTradeMain() {
        Account* main = AccountByTradeRole(1);
        if (!main) return;
        const std::size_t room = mainTradeSequence_.size() < 64 ? 64 - mainTradeSequence_.size() : 0;
        const std::size_t count = std::min(room, recorderClicks_.size());
        const std::size_t first = mainTradeSequence_.size();
        for (std::size_t i = 0; i < count; ++i) {
            if (recorderClicks_[i].pid != main->game.pid) continue;
            TradeSequenceStep step{};
            step.target = 1; step.mainRef = static_cast<int>(mainTradeSequence_.size()); step.kind = 0;
            step.description = L"REC MAIN bước " + std::to_wstring(mainTradeSequence_.size() + 1);
            step.point = recorderClicks_[i].point;
            step.delayMs = RecordedDelay(i, 500); step.repeat = 1;
            mainTradeSequence_.push_back(step);
        }
        for (std::size_t i = 0; i < mainTradeSequence_.size(); ++i) mainTradeSequence_[i].mainRef = static_cast<int>(i);
        SaveMainTradeSequence();
        RefreshTradeSequenceList();
        PopulateTradeTargetCombo();
        if (mainTradeSequence_.size() > first && tradeSeqList_) {
            const int row = static_cast<int>(first);
            ListView_SetItemState(tradeSeqList_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(tradeSeqList_, row, FALSE);
            LoadTradeSequenceRowToEditor(row);
        }
        LogAccount(*main, L"REC CHUỖI GD MAIN → click đã được chuyển thành thư viện MAIN editable dùng chung.");
    }

    void CommitRecordedTradeChild(DWORD childPid) {
        Account* child = AccountByPid(childPid);
        Account* main = AccountByTradeRole(1);
        if (!child || child->profile.tradeRole < 2 || !main) return;
        EnsureSharedChildTradeSequence();
        const std::size_t first = childTradeSequence_.size();
        for (std::size_t i = 0; i < recorderClicks_.size() && childTradeSequence_.size() < 64; ++i) {
            const RecordedClick& click = recorderClicks_[i];
            TradeSequenceStep row{};
            row.kind = 0; row.repeat = 1;
            if (click.pid == child->game.pid) {
                row.target = 0; row.mainRef = -1;
                row.description = L"REC ACC CON bước " + std::to_wstring(childTradeSequence_.size() + 1);
                row.point = click.point; row.delayMs = RecordedDelay(i, 500);
            } else if (click.pid == main->game.pid) {
                int ref = FindSharedMainStepByPoint(click.point);
                if (ref < 0) {
                    if (mainTradeSequence_.size() >= 64) {
                        LogAccount(*child, L"REC bỏ qua click MAIN mới vì CHUỖI GD MAIN đã đủ 64 dòng.");
                        continue;
                    }
                    TradeSequenceStep shared{};
                    shared.target = 1; shared.mainRef = static_cast<int>(mainTradeSequence_.size()); shared.kind = 0;
                    shared.description = L"REC MAIN bước " + std::to_wstring(mainTradeSequence_.size() + 1);
                    shared.point = click.point; shared.delayMs = RecordedDelay(i, 500); shared.repeat = 1;
                    mainTradeSequence_.push_back(shared);
                    ref = static_cast<int>(mainTradeSequence_.size() - 1);
                }
                row.target = 1; row.mainRef = ref; row.kind = 0;
            } else continue;
            childTradeSequence_.push_back(row);
        }
        for (std::size_t i = 0; i < mainTradeSequence_.size(); ++i) mainTradeSequence_[i].mainRef = static_cast<int>(i);
        SaveMainTradeSequence();
        SaveSharedChildTradeSequence();
        RefreshTradeSequenceList(); PopulateTradeTargetCombo();
        if (childTradeSequence_.size() > first && tradeSeqList_) {
            const int row = static_cast<int>(first);
            ListView_SetItemState(tradeSeqList_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(tradeSeqList_, row, FALSE); LoadTradeSequenceRowToEditor(row);
        }
        LogAccount(*child, L"REC CHUỖI GD ACC CON dùng chung → click trên " + TradeRoleLabel(child->profile.tradeRole) +
                           L" được lưu cho mọi CON; click MAIN vẫn tham chiếu CHUỖI GD MAIN.");
    }

    bool RecorderBlocksAccount(const Account& a) const {
        if (recorderMode_ == RecorderMode::None) return false;
        if (recorderMode_ == RecorderMode::Sell) return a.game.pid == recorderPrimaryPid_;
        if (recorderMode_ == RecorderMode::TradeMain) {
            const Account* main = const_cast<App*>(this)->AccountByTradeRole(1);
            return main && a.game.pid == main->game.pid;
        }
        if (recorderMode_ == RecorderMode::TradeChild) {
            const Account* main = const_cast<App*>(this)->AccountByTradeRole(1);
            return a.game.pid == recorderPrimaryPid_ || (main && a.game.pid == main->game.pid);
        }
        return false;
    }

    void StopRecorder(bool commit) {
        if (recorderMode_ == RecorderMode::None) return;
        const RecorderMode mode = recorderMode_;
        const DWORD primaryPid = recorderPrimaryPid_;
        KillTimer(hwnd_, kRecordTimer);
        recorderMode_ = RecorderMode::None;
        recorderMouseDown_ = false;
        if (commit && !recorderClicks_.empty()) {
            if (mode == RecorderMode::Sell) CommitRecordedSell(primaryPid);
            else if (mode == RecorderMode::TradeMain) CommitRecordedTradeMain();
            else if (mode == RecorderMode::TradeChild) CommitRecordedTradeChild(primaryPid);
        }
        const std::size_t count = recorderClicks_.size();
        recorderClicks_.clear(); recorderPrimaryPid_ = 0;
        const std::wstring status = commit ? L"REC xong • đã chuyển " + std::to_wstring(count) + L" click thành dòng tọa độ" : L"REC đã hủy";
        UpdateRecorderUi(status);
        SetTradeStatus(L"BĐPT thoát RECORDING CỤC BỘ • acc bị giữ tiếp tục auto");
    }

    void StartRecorder(RecorderMode mode) {
        if (recorderMode_ != RecorderMode::None) { StopRecorder(true); return; }
        // REC is now scoped to the window(s) being captured. It must never freeze
        // unrelated accounts merely because point capture uses the physical mouse.
        Account* primary = nullptr;
        if (mode == RecorderMode::Sell) {
            primary = SelectedAccount();
            if (!primary) { Log(L"REC BÁN ĐỒ: chưa chọn acc."); return; }
        } else if (mode == RecorderMode::TradeMain) {
            primary = AccountByTradeRole(1);
            if (!primary || tradeEditorMode_ != 1) { Log(L"REC MAIN: không có MAIN/editor MAIN."); return; }
        } else if (mode == RecorderMode::TradeChild) {
            primary = TradeEditorChild();
            if (!primary || !AccountByTradeRole(1)) { Log(L"REC CON cần cả MAIN và CON đang mở editor."); return; }
        } else return;
        recorderClicks_.clear(); recorderMode_ = mode; recorderPrimaryPid_ = primary->game.pid;
        recorderMouseDown_ = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        SetTimer(hwnd_, kRecordTimer, 10, nullptr);
        UpdateRecorderUi(L"REC ĐANG GHI • chỉ khóa acc/cặp acc đang capture • các acc khác tiếp tục auto");
        SetTradeStatus(L"RECORDING CỤC BỘ • chỉ giữ cửa sổ liên quan; scheduler acc khác tiếp tục");
        LogAccount(*primary, L"BĐPT vào RECORDING CỤC BỘ • chỉ acc/cặp acc capture bị giữ; acc khác vẫn auto độc lập.");
    }

    void ToggleSellRecorder() { StartRecorder(RecorderMode::Sell); }
    void ToggleTradeRecorder() { StartRecorder(tradeEditorMode_ == 1 ? RecorderMode::TradeMain : RecorderMode::TradeChild); }

    void CopySelectedSellRows() {
        Account* account = SelectedAccount();
        const std::vector<int> rows = SelectedRows(sellMacroList_);
        if (!account || rows.empty()) { Log(L"SAO CHÉP BÁN: hãy chọn một hoặc nhiều dòng."); return; }
        sellClipboard_.clear();
        for (int row : rows) if (row >= 0 && row < static_cast<int>(account->profile.sellMacro.size())) sellClipboard_.push_back(account->profile.sellMacro[static_cast<std::size_t>(row)]);
        UpdateRecorderUi(L"Đã sao chép " + std::to_wstring(sellClipboard_.size()) + L" dòng bán • bấm DÁN để thêm vào cuối chuỗi");
    }

    void PasteSellRows() {
        Account* account = SelectedAccount();
        if (!account || sellClipboard_.empty()) { Log(L"DÁN BÁN: clipboard dòng đang rỗng."); return; }
        const std::size_t first = account->profile.sellMacro.size();
        for (const SellMacroStep& step : sellClipboard_) {
            if (account->profile.sellMacro.size() >= 64) break;
            account->profile.sellMacro.push_back(step);
        }
        SaveProfile(account->profile); RefreshSellMacroList();
        if (account->profile.sellMacro.size() > first) {
            const int row = static_cast<int>(first); ListView_SetItemState(sellMacroList_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); ListView_EnsureVisible(sellMacroList_, row, FALSE);
        }
        UpdateRecorderUi(L"Đã DÁN " + std::to_wstring(account->profile.sellMacro.size() - first) + L" dòng vào cuối chuỗi bán");
    }

    void CopySelectedTradeRows() {
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        const std::vector<int> rows = SelectedRows(tradeSeqList_);
        if (!seq || rows.empty()) { Log(L"SAO CHÉP GD: hãy chọn một hoặc nhiều dòng."); return; }
        tradeClipboard_.clear(); tradeClipboardMode_ = tradeEditorMode_;
        for (int row : rows) if (row >= 0 && row < static_cast<int>(seq->size())) tradeClipboard_.push_back((*seq)[static_cast<std::size_t>(row)]);
        UpdateRecorderUi(L"Đã sao chép " + std::to_wstring(tradeClipboard_.size()) + L" dòng GD • bấm DÁN để thêm vào cuối chuỗi");
    }

    void PasteTradeRows() {
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        if (!seq || tradeClipboard_.empty() || tradeClipboardMode_ != tradeEditorMode_) {
            Log(L"DÁN GD: clipboard rỗng hoặc khác loại editor MAIN/CON."); return;
        }
        const std::size_t first = seq->size();
        int nextGroupId = MaxTradeGroupId(*seq) + 1;
        std::vector<std::pair<int, int>> groupMap;
        for (TradeSequenceStep step : tradeClipboard_) {
            if (seq->size() >= 64) break;
            if (tradeEditorMode_ == 1) {
                step.target = 1;
                step.mainRef = static_cast<int>(seq->size());
                step.kind = 0;
                step.groupId = 0;
                step.groupRepeat = 1;
            } else if (step.groupId > 0) {
                int mapped = 0;
                for (const auto& entry : groupMap) {
                    if (entry.first == step.groupId) { mapped = entry.second; break; }
                }
                if (mapped == 0) {
                    mapped = nextGroupId++;
                    groupMap.emplace_back(step.groupId, mapped);
                }
                step.groupId = mapped;
                step.groupRepeat = std::clamp(step.groupRepeat, 1, 999);
            }
            seq->push_back(step);
        }
        if (tradeEditorMode_ == 1) {
            for (std::size_t i = 0; i < seq->size(); ++i) (*seq)[i].mainRef = static_cast<int>(i);
        } else {
            NormalizeTradeGroups(*seq);
        }
        SaveEditorSequence(); RefreshTradeSequenceList(); PopulateTradeTargetCombo();
        if (seq->size() > first) {
            const int row = static_cast<int>(first);
            ListView_SetItemState(tradeSeqList_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(tradeSeqList_, row, FALSE);
            LoadTradeSequenceRowToEditor(row);
        }
        UpdateRecorderUi(L"Đã DÁN " + std::to_wstring(seq->size() - first) + L" dòng GD vào cuối chuỗi");
    }

    void GroupSelectedTradeRows() {
        if (tradeEditorMode_ != 2) { Log(L"GOM NHÓM chỉ dùng trong CHUỖI GD ACC CON."); return; }
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        std::vector<int> rows = SelectedRows(tradeSeqList_);
        if (!seq || rows.empty()) { Log(L"GOM NHÓM: chọn một hoặc nhiều dòng liên tiếp."); return; }
        std::sort(rows.begin(), rows.end());
        for (std::size_t i = 1; i < rows.size(); ++i) {
            if (rows[i] != rows[i - 1] + 1) {
                Log(L"GOM NHÓM: các dòng phải liên tiếp nhau."); return;
            }
        }
        const int repeat = tradeSeqGroupRepeat_
            ? std::clamp(_wtoi(GetText(tradeSeqGroupRepeat_).c_str()), 1, 999)
            : 1;
        const int id = MaxTradeGroupId(*seq) + 1;
        for (int row : rows) {
            if (row < 0 || row >= static_cast<int>(seq->size())) continue;
            TradeSequenceStep& step = (*seq)[static_cast<std::size_t>(row)];
            step.groupId = id;
            step.groupRepeat = repeat;
        }
        NormalizeTradeGroups(*seq);
        SaveEditorSequence();
        RefreshTradeSequenceList();
        for (int row : rows) {
            ListView_SetItemState(tradeSeqList_, row, LVIS_SELECTED, LVIS_SELECTED);
        }
        if (!rows.empty()) {
            ListView_SetItemState(tradeSeqList_, rows.front(), LVIS_FOCUSED, LVIS_FOCUSED);
            LoadTradeSequenceRowToEditor(rows.front());
        }
        Log(L"Đã GOM " + std::to_wstring(rows.size()) + L" dòng thành mini-sequence • lặp nhóm " + std::to_wstring(repeat) + L" lần.");
    }

    void UngroupSelectedTradeRows() {
        if (tradeEditorMode_ != 2) { Log(L"BỎ NHÓM chỉ dùng trong CHUỖI GD ACC CON."); return; }
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        const std::vector<int> rows = SelectedRows(tradeSeqList_);
        if (!seq || rows.empty()) { Log(L"BỎ NHÓM: chọn ít nhất một dòng thuộc nhóm."); return; }
        std::vector<int> groupIds;
        for (int row : rows) {
            if (row < 0 || row >= static_cast<int>(seq->size())) continue;
            const int id = (*seq)[static_cast<std::size_t>(row)].groupId;
            if (id > 0 && std::find(groupIds.begin(), groupIds.end(), id) == groupIds.end()) groupIds.push_back(id);
        }
        if (groupIds.empty()) { Log(L"BỎ NHÓM: các dòng đã chọn không thuộc nhóm nào."); return; }
        for (TradeSequenceStep& step : *seq) {
            if (std::find(groupIds.begin(), groupIds.end(), step.groupId) != groupIds.end()) {
                step.groupId = 0;
                step.groupRepeat = 1;
            }
        }
        NormalizeTradeGroups(*seq);
        SaveEditorSequence();
        RefreshTradeSequenceList();
        Log(L"Đã BỎ " + std::to_wstring(groupIds.size()) + L" nhóm khỏi chuỗi GD.");
    }

    int SelectedMacroIndex() const {
        return FocusedSelectedRow(sellMacroList_);
    }

    void RefreshSellMacroList() {
        if (!sellMacroList_) return;
        ListView_DeleteAllItems(sellMacroList_);
        Account* a = SelectedAccount();
        if (!a) return;
        for (std::size_t i = 0; i < a->profile.sellMacro.size(); ++i) {
            const SellMacroStep& step = a->profile.sellMacro[i];
            std::wstring no = std::to_wstring(i + 1);
            LVITEMW item{};
            item.mask = LVIF_TEXT;
            item.iItem = static_cast<int>(i);
            item.pszText = const_cast<wchar_t*>(no.c_str());
            ListView_InsertItem(sellMacroList_, &item);
            ListView_SetItemText(sellMacroList_, static_cast<int>(i), 1,
                const_cast<wchar_t*>(step.description.empty() ? L"(chưa mô tả)" : step.description.c_str()));
            std::wstring point = PointDescription(step.point);
            ListView_SetItemText(sellMacroList_, static_cast<int>(i), 2, const_cast<wchar_t*>(point.c_str()));
            std::wstring delay = std::to_wstring(step.delayMs);
            std::wstring repeat = std::to_wstring(step.repeat);
            ListView_SetItemText(sellMacroList_, static_cast<int>(i), 3, const_cast<wchar_t*>(delay.c_str()));
            ListView_SetItemText(sellMacroList_, static_cast<int>(i), 4, const_cast<wchar_t*>(repeat.c_str()));
        }
    }

    void ClearSellMacroEditor() {
        SetText(sellDesc_, L"");
        SetText(sellDelay_, L"600");
        SetText(sellRepeat_, L"1");
    }

    void LoadSelectedMacroEditor() {
        Account* a = SelectedAccount();
        const int index = SelectedMacroIndex();
        if (!a || index < 0 || index >= static_cast<int>(a->profile.sellMacro.size())) {
            ClearSellMacroEditor();
            return;
        }
        const SellMacroStep& step = a->profile.sellMacro[static_cast<std::size_t>(index)];
        SetText(sellDesc_, step.description);
        SetText(sellDelay_, std::to_wstring(step.delayMs));
        SetText(sellRepeat_, std::to_wstring(step.repeat));
    }

    void AddSellMacroRow() {
        Account* a = SelectedAccount();
        if (!a) { Log(L"Chưa chọn acc để thêm bước bán"); return; }
        if (a->profile.sellMacro.size() >= 64) { LogAccount(*a, L"Macro bán tối đa 64 dòng."); return; }
        SellMacroStep step{};
        step.description = L"Bước " + std::to_wstring(a->profile.sellMacro.size() + 1);
        a->profile.sellMacro.push_back(step);
        SaveProfile(a->profile);
        RefreshSellMacroList();
        const int row = static_cast<int>(a->profile.sellMacro.size() - 1);
        ListView_SetItemState(sellMacroList_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(sellMacroList_, row, FALSE);
        LoadSelectedMacroEditor();
    }

    void DeleteSellMacroRow() {
        Account* a = SelectedAccount();
        const int index = SelectedMacroIndex();
        if (!a || index < 0 || index >= static_cast<int>(a->profile.sellMacro.size())) {
            Log(L"Chưa chọn dòng macro để xóa"); return;
        }
        a->profile.sellMacro.erase(a->profile.sellMacro.begin() + index);
        SaveProfile(a->profile);
        RefreshSellMacroList();
        ClearSellMacroEditor();
    }

    void SaveSellMacroRow() {
        Account* a = SelectedAccount();
        const int index = SelectedMacroIndex();
        if (!a || index < 0 || index >= static_cast<int>(a->profile.sellMacro.size())) {
            Log(L"Chưa chọn dòng macro để lưu"); return;
        }
        SellMacroStep& step = a->profile.sellMacro[static_cast<std::size_t>(index)];
        step.description = GetText(sellDesc_);
        int delay = _wtoi(GetText(sellDelay_).c_str());
        int repeat = _wtoi(GetText(sellRepeat_).c_str());
        if (delay < 50) delay = 50;
        if (delay > 60000) delay = 60000;
        if (repeat < 1) repeat = 1;
        if (repeat > 999) repeat = 999;
        step.delayMs = delay;
        step.repeat = repeat;
        SaveProfile(a->profile);
        RefreshSellMacroList();
        ListView_SetItemState(sellMacroList_, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        LoadSelectedMacroEditor();
    }

    void BeginMacroCapture() {
        Account* a = SelectedAccount();
        const int index = SelectedMacroIndex();
        if (!a || index < 0 || index >= static_cast<int>(a->profile.sellMacro.size())) {
            Log(L"Chưa chọn dòng macro để lấy tọa độ"); return;
        }
        captureSlot_ = ClickSlot::None;
        captureMacroIndex_ = index;
        captureTradeSequenceIndex_ = -1;
        captureTradeSequenceMode_ = 0;
        captureTradeSequenceMainRef_ = -1;
        capturePid_ = a->game.pid;
        LogAccount(*a, L"Đang chờ F8 cho macro bán dòng " + std::to_wstring(index + 1));
        SetText(selected_, L"LẤY TỌA ĐỘ MACRO DÒNG " + std::to_wstring(index + 1) + L" • đưa chuột vào game rồi F8");
    }

    void TestSellMacroRow() {
        Account* a = SelectedAccount();
        const int index = SelectedMacroIndex();
        if (!a || index < 0 || index >= static_cast<int>(a->profile.sellMacro.size())) {
            Log(L"Chưa chọn dòng macro để TEST"); return;
        }
        const SellMacroStep& step = a->profile.sellMacro[static_cast<std::size_t>(index)];
        std::wstring error;
        if (!CoordinatorInternalPointAction(
                *a, step.point,
                L"TEST CHUỖI BÁN ĐỒ dòng " + std::to_wstring(index + 1),
                error)) {
            LogAccount(*a, L"TEST macro HIDDEN INPUT FAIL: " + error); return;
        }
        LogAccount(*a, L"TEST macro dòng " + std::to_wstring(index + 1) +
                       L" PASS • InputSync nội bộ • cursor không đổi");
    }

    void LoadSellNpcPositionToUi(const Account& a) {
        int index = a.profile.sellNpcPreset;
        if (index < 0 || index >= static_cast<int>(kSellNpcs.size())) index = 0;
        const SellNpcPosition& pos = sellNpcPositions_[static_cast<std::size_t>(index)];
        if (pos.valid) {
            SetText(sellNpcX_, std::to_wstring(pos.x));
            SetText(sellNpcY_, std::to_wstring(pos.y));
            SetText(sellNpcPosText_, L"M" + std::to_wstring(kSellNpcs[static_cast<std::size_t>(index)].mapID) + L" • " +
                                     std::to_wstring(pos.x) + L"," + std::to_wstring(pos.y));
        } else {
            SetText(sellNpcX_, L"");
            SetText(sellNpcY_, L"");
            SetText(sellNpcPosText_, L"CHƯA LẤY");
        }
    }

    void PersistSellNpcPositionEditor(Account& a) {
        int index = a.profile.sellNpcPreset;
        if (index < 0 || index >= static_cast<int>(kSellNpcs.size())) index = 0;
        const std::wstring xText = GetText(sellNpcX_);
        const std::wstring yText = GetText(sellNpcY_);
        SellNpcPosition& pos = sellNpcPositions_[static_cast<std::size_t>(index)];
        if (xText.empty() || yText.empty()) {
            pos = SellNpcPosition{};
            SaveSharedSellNpcPositions(sellNpcPositions_);
            return;
        }
        const int x = _wtoi(xText.c_str());
        const int y = _wtoi(yText.c_str());
        if (x < 0 || y < 0) {
            pos = SellNpcPosition{};
            SaveSharedSellNpcPositions(sellNpcPositions_);
            return;
        }
        pos.x = x;
        pos.y = y;
        pos.valid = true;
        SaveSharedSellNpcPositions(sellNpcPositions_);
    }

    void OnSellNpcSelectionChanged() {
        Account* a = SelectedAccount();
        if (!a) return;
        PersistSellNpcPositionEditor(*a);
        const LRESULT sellSel = SendMessageW(sellNpcCombo_, CB_GETCURSEL, 0, 0);
        if (sellSel != CB_ERR && sellSel >= 0 && sellSel < static_cast<LRESULT>(kSellNpcs.size())) {
            a->profile.sellNpcPreset = static_cast<int>(sellSel);
        }
        SaveProfile(a->profile);
        LoadSellNpcPositionToUi(*a);
    }

    void CaptureSellNpcPosition() {
        Account* a = SelectedAccount();
        if (!a) { Log(L"Chưa chọn acc để lấy tọa NPC"); return; }
        std::wstring error;
        if (!ReadSnapshot(*a, error, 1200)) {
            LogAccount(*a, L"Không đọc được state để lấy tọa NPC: " + error);
            return;
        }
        int index = a->profile.sellNpcPreset;
        if (index < 0 || index >= static_cast<int>(kSellNpcs.size())) index = 0;
        const SellNpcPreset& npc = kSellNpcs[static_cast<std::size_t>(index)];
        const Snapshot& snap = a->snapshot;
        if ((snap.validMask & (ValidMap | ValidPosition)) != (ValidMap | ValidPosition)) {
            LogAccount(*a, L"State chưa có Map/X/Y để lấy tọa NPC");
            return;
        }
        if (snap.mapID != npc.mapID) {
            LogAccount(*a, L"Không lưu: đang ở MapID " + std::to_wstring(snap.mapID) +
                           L" nhưng NPC đã chọn thuộc MapID " + std::to_wstring(npc.mapID));
            return;
        }
        SellNpcPosition& pos = sellNpcPositions_[static_cast<std::size_t>(index)];
        pos.x = snap.x;
        pos.y = snap.y;
        pos.valid = true;
        SaveSharedSellNpcPositions(sellNpcPositions_);
        LoadSellNpcPositionToUi(*a);
        LogAccount(*a, L"ĐÃ LẤY TỌA NPC • " + std::wstring(npc.name) + L" • " +
                       std::to_wstring(pos.x) + L"," + std::to_wstring(pos.y));
    }

    void LoadTradeSettings() {
        tradeEnabled_ = ReadIniInt(L"Global", L"TradeEnabled", 1) != 0;
        mainSellThreshold_ = ReadIniInt(L"Global", L"MainSellThreshold", 6);
        mainSellThreshold_ = std::clamp(mainSellThreshold_, 1, 90);

        tradeRendezvous_.name = L"TỌA GD";
        tradeRendezvous_.mapID = ReadIniInt(L"Global", L"TradeRendezvousMap", 0);
        tradeRendezvous_.x = ReadIniInt(L"Global", L"TradeRendezvousX", 0);
        tradeRendezvous_.y = ReadIniInt(L"Global", L"TradeRendezvousY", 0);
        tradeRendezvous_.valid = tradeRendezvous_.mapID > 0 && ReadIniInt(L"Global", L"TradeRendezvousValid", 0) != 0;
        tradeRendezvousTolerance_ = std::clamp(ReadIniInt(L"Global", L"TradeRendezvousTolerance", 120), 20, 500);
    }

    void LoadTradeSequence() {
        // v0.2.7: exactly two reusable trade definitions:
        // 1) MAIN shared coordinate library; 2) one shared ordered ACC CON workflow.
        mainTradeSequence_.clear();
        childTradeSequence_.clear();
        legacyChildTradeTemplate_.clear();
        sharedChildTradeMigrationDone_ = false;

        // Prefer the MAIN-shared section.
        int mainCount = std::clamp(ReadIniInt(L"MainTradeSequence", L"Count", 0), 0, 64);
        for (int i = 0; i < mainCount; ++i) {
            TradeSequenceStep step{};
            const std::wstring prefix = L"Step_" + std::to_wstring(i) + L"_";
            step.target = 1;
            step.mainRef = i;
            step.kind = 0;
            step.description = ReadIniText(L"MainTradeSequence", prefix + L"Desc");
            step.point.x = ReadIniInt(L"MainTradeSequence", prefix + L"X", -1);
            step.point.y = ReadIniInt(L"MainTradeSequence", prefix + L"Y", -1);
            step.point.baseW = ReadIniInt(L"MainTradeSequence", prefix + L"W", 0);
            step.point.baseH = ReadIniInt(L"MainTradeSequence", prefix + L"H", 0);
            step.point.valid = step.point.x >= 0 && step.point.y >= 0 && step.point.baseW > 0 && step.point.baseH > 0;
            step.delayMs = std::clamp(ReadIniInt(L"MainTradeSequence", prefix + L"Delay", 500), 50, 60000);
            step.repeat = std::clamp(ReadIniInt(L"MainTradeSequence", prefix + L"Repeat", 1), 1, 999);
            mainTradeSequence_.push_back(step);
        }

        // v0.2.7 shared ACC CON workflow. Count=-1 means the section does not exist yet,
        // allowing one-time migration from the old per-CON profiles.
        const int sharedChildCountRaw = ReadIniInt(L"ChildTradeSequence", L"Count", -1);
        if (sharedChildCountRaw >= 0) {
            sharedChildTradeMigrationDone_ = true;
            const int sharedChildCount = std::clamp(sharedChildCountRaw, 0, 64);
            for (int i = 0; i < sharedChildCount; ++i) {
                TradeSequenceStep step{};
                const std::wstring prefix = L"Step_" + std::to_wstring(i) + L"_";
                step.target = std::clamp(ReadIniInt(L"ChildTradeSequence", prefix + L"Target", 0), 0, 1);
                step.mainRef = ReadIniInt(L"ChildTradeSequence", prefix + L"MainRef", -1);
                step.kind = std::clamp(ReadIniInt(L"ChildTradeSequence", prefix + L"Kind", 0), 0, 1);
                step.description = ReadIniText(L"ChildTradeSequence", prefix + L"Desc");
                step.point.x = ReadIniInt(L"ChildTradeSequence", prefix + L"X", -1);
                step.point.y = ReadIniInt(L"ChildTradeSequence", prefix + L"Y", -1);
                step.point.baseW = ReadIniInt(L"ChildTradeSequence", prefix + L"W", 0);
                step.point.baseH = ReadIniInt(L"ChildTradeSequence", prefix + L"H", 0);
                step.point.valid = step.point.x >= 0 && step.point.y >= 0 && step.point.baseW > 0 && step.point.baseH > 0;
                step.delayMs = std::clamp(ReadIniInt(L"ChildTradeSequence", prefix + L"Delay", 500), 50, 60000);
                step.repeat = std::clamp(ReadIniInt(L"ChildTradeSequence", prefix + L"Repeat", 1), 1, 999);
                step.groupId = std::max(0, ReadIniInt(L"ChildTradeSequence", prefix + L"GroupId", 0));
                step.groupRepeat = std::clamp(ReadIniInt(L"ChildTradeSequence", prefix + L"GroupRepeat", 1), 1, 999);
                if (step.kind == 1) { step.target = 0; step.mainRef = -1; }
                childTradeSequence_.push_back(step);
            }
        }

        // One-time migration template from v0.2.3 combined global TradeSequence.
        int legacyCount = std::clamp(ReadIniInt(L"TradeSequence", L"Count", 0), 0, 64);
        std::vector<int> oldMainToNew(static_cast<std::size_t>(legacyCount), -1);
        if (mainTradeSequence_.empty()) {
            for (int i = 0; i < legacyCount; ++i) {
                const std::wstring prefix = L"Step_" + std::to_wstring(i) + L"_";
                if (std::clamp(ReadIniInt(L"TradeSequence", prefix + L"Target", 0), 0, 1) != 0) continue;
                TradeSequenceStep mainStep{};
                mainStep.target = 1; mainStep.mainRef = static_cast<int>(mainTradeSequence_.size());
                mainStep.description = ReadIniText(L"TradeSequence", prefix + L"Desc");
                mainStep.point.x = ReadIniInt(L"TradeSequence", prefix + L"X", -1);
                mainStep.point.y = ReadIniInt(L"TradeSequence", prefix + L"Y", -1);
                mainStep.point.baseW = ReadIniInt(L"TradeSequence", prefix + L"W", 0);
                mainStep.point.baseH = ReadIniInt(L"TradeSequence", prefix + L"H", 0);
                mainStep.point.valid = mainStep.point.x >= 0 && mainStep.point.y >= 0 && mainStep.point.baseW > 0 && mainStep.point.baseH > 0;
                mainStep.delayMs = std::clamp(ReadIniInt(L"TradeSequence", prefix + L"Delay", 500), 50, 60000);
                mainStep.repeat = std::clamp(ReadIniInt(L"TradeSequence", prefix + L"Repeat", 1), 1, 999);
                oldMainToNew[static_cast<std::size_t>(i)] = mainStep.mainRef;
                mainTradeSequence_.push_back(mainStep);
            }
            if (!mainTradeSequence_.empty()) SaveMainTradeSequence();
        } else {
            // Map legacy MAIN rows to new MAIN rows in original MAIN-order for child migration.
            int ref = 0;
            for (int i = 0; i < legacyCount; ++i) {
                const std::wstring prefix = L"Step_" + std::to_wstring(i) + L"_";
                if (std::clamp(ReadIniInt(L"TradeSequence", prefix + L"Target", 0), 0, 1) == 0 && ref < static_cast<int>(mainTradeSequence_.size())) {
                    oldMainToNew[static_cast<std::size_t>(i)] = ref++;
                }
            }
        }
        for (int i = 0; i < legacyCount; ++i) {
            TradeSequenceStep step{};
            const std::wstring prefix = L"Step_" + std::to_wstring(i) + L"_";
            const int oldTarget = std::clamp(ReadIniInt(L"TradeSequence", prefix + L"Target", 0), 0, 1);
            step.kind = std::clamp(ReadIniInt(L"TradeSequence", prefix + L"Kind", 0), 0, 1);
            step.description = ReadIniText(L"TradeSequence", prefix + L"Desc");
            step.delayMs = std::clamp(ReadIniInt(L"TradeSequence", prefix + L"Delay", 500), 50, 60000);
            step.repeat = std::clamp(ReadIniInt(L"TradeSequence", prefix + L"Repeat", 1), 1, 999);
            if (oldTarget == 0) {
                step.target = 1;
                step.mainRef = oldMainToNew[static_cast<std::size_t>(i)];
                step.kind = 0;
            } else {
                step.target = 0;
                step.mainRef = -1;
                step.point.x = ReadIniInt(L"TradeSequence", prefix + L"X", -1);
                step.point.y = ReadIniInt(L"TradeSequence", prefix + L"Y", -1);
                step.point.baseW = ReadIniInt(L"TradeSequence", prefix + L"W", 0);
                step.point.baseH = ReadIniInt(L"TradeSequence", prefix + L"H", 0);
                step.point.valid = step.point.x >= 0 && step.point.y >= 0 && step.point.baseW > 0 && step.point.baseH > 0;
            }
            if (step.target == 0 || step.mainRef >= 0) legacyChildTradeTemplate_.push_back(step);
        }
        NormalizeTradeGroups(childTradeSequence_);
        NormalizeTradeGroups(legacyChildTradeTemplate_);
    }

    void SaveMainTradeSequence() {
        EnsureUnicodeIni();
        WriteIniInt(L"MainTradeSequence", L"Count", static_cast<int>(mainTradeSequence_.size()));
        for (std::size_t i = 0; i < mainTradeSequence_.size(); ++i) {
            const TradeSequenceStep& step = mainTradeSequence_[i];
            const std::wstring prefix = L"Step_" + std::to_wstring(i) + L"_";
            WriteIniText(L"MainTradeSequence", prefix + L"Desc", step.description);
            WriteIniInt(L"MainTradeSequence", prefix + L"X", step.point.valid ? step.point.x : -1);
            WriteIniInt(L"MainTradeSequence", prefix + L"Y", step.point.valid ? step.point.y : -1);
            WriteIniInt(L"MainTradeSequence", prefix + L"W", step.point.valid ? step.point.baseW : 0);
            WriteIniInt(L"MainTradeSequence", prefix + L"H", step.point.valid ? step.point.baseH : 0);
            WriteIniInt(L"MainTradeSequence", prefix + L"Delay", step.delayMs);
            WriteIniInt(L"MainTradeSequence", prefix + L"Repeat", step.repeat);
        }
        FlushIni();
    }

    void SaveSharedChildTradeSequence() {
        EnsureUnicodeIni();
        NormalizeTradeGroups(childTradeSequence_);
        WriteIniInt(L"ChildTradeSequence", L"Count", static_cast<int>(childTradeSequence_.size()));
        for (std::size_t i = 0; i < childTradeSequence_.size(); ++i) {
            const TradeSequenceStep& step = childTradeSequence_[i];
            const std::wstring prefix = L"Step_" + std::to_wstring(i) + L"_";
            WriteIniInt(L"ChildTradeSequence", prefix + L"Target", step.target);
            WriteIniInt(L"ChildTradeSequence", prefix + L"MainRef", step.mainRef);
            WriteIniInt(L"ChildTradeSequence", prefix + L"Kind", step.kind);
            WriteIniText(L"ChildTradeSequence", prefix + L"Desc", step.description);
            WriteIniInt(L"ChildTradeSequence", prefix + L"X", step.point.valid ? step.point.x : -1);
            WriteIniInt(L"ChildTradeSequence", prefix + L"Y", step.point.valid ? step.point.y : -1);
            WriteIniInt(L"ChildTradeSequence", prefix + L"W", step.point.valid ? step.point.baseW : 0);
            WriteIniInt(L"ChildTradeSequence", prefix + L"H", step.point.valid ? step.point.baseH : 0);
            WriteIniInt(L"ChildTradeSequence", prefix + L"Delay", step.delayMs);
            WriteIniInt(L"ChildTradeSequence", prefix + L"Repeat", step.repeat);
            WriteIniInt(L"ChildTradeSequence", prefix + L"GroupId", step.groupId);
            WriteIniInt(L"ChildTradeSequence", prefix + L"GroupRepeat", step.groupRepeat);
        }
        sharedChildTradeMigrationDone_ = true;
        FlushIni();
    }

    void EnsureSharedChildTradeSequence() {
        if (sharedChildTradeMigrationDone_) return;
        sharedChildTradeMigrationDone_ = true;

        // Prefer the old sequence of the lowest CON slot so existing setups migrate deterministically.
        for (int slot = 1; slot <= 6; ++slot) {
            Account* child = AccountByTradeRole(slot + 1);
            if (!child || child->profile.childTradeSequence.empty()) continue;
            childTradeSequence_ = child->profile.childTradeSequence;
            SaveSharedChildTradeSequence();
            LogAccount(*child, L"v0.2.7 MIGRATE: lấy chuỗi GD cũ của " + TradeRoleLabel(child->profile.tradeRole) +
                               L" làm CHUỖI GD ACC CON dùng chung cho CON1→CON6.");
            return;
        }

        if (!legacyChildTradeTemplate_.empty()) {
            childTradeSequence_ = legacyChildTradeTemplate_;
            SaveSharedChildTradeSequence();
            Log(L"v0.2.7 MIGRATE: lấy template GD legacy làm CHUỖI GD ACC CON dùng chung.");
            return;
        }

        // Persist an intentional empty shared section so we do not repeatedly scan legacy profiles.
        SaveSharedChildTradeSequence();
    }

    std::vector<TradeSequenceStep>* EditorSequence() {
        if (tradeEditorMode_ == 1) return &mainTradeSequence_;
        if (tradeEditorMode_ == 2) {
            Account* child = TradeEditorChild();
            if (!child || child->profile.tradeRole < 2) return nullptr; // selected CON is only the capture/test donor.
            EnsureSharedChildTradeSequence();
            return &childTradeSequence_;
        }
        return nullptr;
    }

    Account* TradeEditorChild() {
        Account* child = AccountByPid(tradeEditorChildPid_);
        return child && child->profile.tradeRole >= 2 ? child : nullptr;
    }

    const TradeSequenceStep* ResolveMainReference(const TradeSequenceStep& step) const {
        if (step.target != 1 || step.mainRef < 0 || step.mainRef >= static_cast<int>(mainTradeSequence_.size())) return nullptr;
        return &mainTradeSequence_[static_cast<std::size_t>(step.mainRef)];
    }

    void NormalizeTradeGroups(std::vector<TradeSequenceStep>& seq) {
        int nextId = 1;
        int previousOldId = 0;
        int currentNewId = 0;
        int currentRepeat = 1;
        for (std::size_t i = 0; i < seq.size(); ++i) {
            TradeSequenceStep& step = seq[i];
            const int oldId = step.groupId;
            if (oldId <= 0) {
                step.groupId = 0;
                step.groupRepeat = 1;
                previousOldId = 0;
                currentNewId = 0;
                continue;
            }
            if (i == 0 || oldId != previousOldId || currentNewId == 0) {
                currentNewId = nextId++;
                currentRepeat = std::clamp(step.groupRepeat, 1, 999);
            }
            step.groupId = currentNewId;
            step.groupRepeat = currentRepeat;
            previousOldId = oldId;
        }
    }

    int MaxTradeGroupId(const std::vector<TradeSequenceStep>& seq) const {
        int maxId = 0;
        for (const TradeSequenceStep& step : seq) maxId = std::max(maxId, step.groupId);
        return maxId;
    }

    std::size_t TradeGroupStart(const std::vector<TradeSequenceStep>& seq, std::size_t index) const {
        if (index >= seq.size() || seq[index].groupId <= 0) return index;
        const int id = seq[index].groupId;
        while (index > 0 && seq[index - 1].groupId == id) --index;
        return index;
    }

    std::size_t TradeGroupEnd(const std::vector<TradeSequenceStep>& seq, std::size_t index) const {
        if (index >= seq.size() || seq[index].groupId <= 0) return index;
        const int id = seq[index].groupId;
        while (index + 1 < seq.size() && seq[index + 1].groupId == id) ++index;
        return index;
    }

    std::wstring TradeGroupLabel(const TradeSequenceStep& step) const {
        if (step.groupId <= 0) return L"-";
        return L"G" + std::to_wstring(step.groupId) + L" ×" + std::to_wstring(step.groupRepeat);
    }

    bool TradeSequenceReady(std::wstring& reason) {
        EnsureSharedChildTradeSequence();
        if (childTradeSequence_.empty()) { reason = L"chưa có CHUỖI GD ACC CON dùng chung"; return false; }
        for (std::size_t i = 0; i < childTradeSequence_.size(); ++i) {
            const TradeSequenceStep& step = childTradeSequence_[i];
            if (step.target == 1) {
                const TradeSequenceStep* shared = ResolveMainReference(step);
                if (!shared) { reason = L"bước " + std::to_wstring(i + 1) + L" tham chiếu MAIN không tồn tại"; return false; }
                if (!shared->point.valid) { reason = L"MAIN bước " + std::to_wstring(step.mainRef + 1) + L" chưa lấy tọa độ"; return false; }
            } else if (!step.point.valid) {
                reason = L"CON bước " + std::to_wstring(i + 1) + L" chưa lấy tọa độ"; return false;
            }
        }
        if (!HasChildTransferStep(childTradeSequence_)) {
            reason = L"chuỗi GD bắt buộc phải có ít nhất 1 bước CHUYỂN ĐỒ của ACC CON";
            return false;
        }
        return true;
    }

    std::wstring TradeStepTargetLabel(const TradeSequenceStep& step) {
        if (tradeEditorMode_ == 1) return L"MAIN";
        if (step.target == 1) return L"MAIN #" + std::to_wstring(step.mainRef + 1);
        return L"ACC CON ĐANG GD";
    }

    static std::wstring TradeStepKindLabel(const TradeSequenceStep& step) {
        return step.kind == 1 ? L"CHUYỂN ĐỒ" : L"CLICK";
    }

    const TradeSequenceStep* EffectiveEditorStep(const TradeSequenceStep& step) const {
        return tradeEditorMode_ == 2 && step.target == 1 ? ResolveMainReference(step) : &step;
    }

    void RefreshTradeSequenceList() {
        if (!tradeSeqList_) return;
        ListView_DeleteAllItems(tradeSeqList_);
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        if (!seq) return;
        for (std::size_t i = 0; i < seq->size(); ++i) {
            const TradeSequenceStep& step = (*seq)[i];
            const TradeSequenceStep* effective = EffectiveEditorStep(step);
            std::wstring idx = std::to_wstring(i + 1);
            LVITEMW item{}; item.mask = LVIF_TEXT; item.iItem = static_cast<int>(i); item.pszText = idx.data();
            ListView_InsertItem(tradeSeqList_, &item);
            const std::array<std::wstring, 7> cols = {{
                TradeStepTargetLabel(step), TradeStepKindLabel(step),
                (effective && !effective->description.empty()) ? effective->description : L"(không mô tả)",
                effective ? PointDescription(effective->point) : L"MAIN REF LỖI",
                effective ? std::to_wstring(effective->delayMs) : L"-",
                effective ? std::to_wstring(effective->repeat) : L"-",
                tradeEditorMode_ == 2 ? TradeGroupLabel(step) : L"-"
            }};
            for (int col = 0; col < 7; ++col) ListView_SetItemText(tradeSeqList_, static_cast<int>(i), col + 1, const_cast<wchar_t*>(cols[static_cast<std::size_t>(col)].c_str()));
        }
    }

    void SelectTradeSequenceDragRange(int startRow, int endRow) {
        if (!tradeSeqList_) return;
        const int count = ListView_GetItemCount(tradeSeqList_);
        if (count <= 0) return;
        startRow = std::clamp(startRow, 0, count - 1);
        endRow = std::clamp(endRow, 0, count - 1);
        const int first = std::min(startRow, endRow);
        const int last = std::max(startRow, endRow);
        tradeSeqDragUpdating_ = true;
        for (int i = 0; i < count; ++i) {
            const UINT state = (i >= first && i <= last) ? LVIS_SELECTED : 0;
            ListView_SetItemState(tradeSeqList_, i, state, LVIS_SELECTED);
        }
        ListView_SetItemState(tradeSeqList_, endRow, LVIS_FOCUSED, LVIS_FOCUSED);
        ListView_EnsureVisible(tradeSeqList_, endRow, FALSE);
        tradeSeqDragUpdating_ = false;
        LoadTradeSequenceRowToEditor(endRow);
        UpdateRecorderUi(L"Đã kéo chọn " + std::to_wstring(last - first + 1) + L" dòng GD");
    }

    int SelectedTradeSequenceIndex() const {
        return FocusedSelectedRow(tradeSeqList_);
    }

    void PopulateTradeTargetCombo(const TradeSequenceStep* step = nullptr) {
        if (!tradeSeqTarget_) return;
        SendMessageW(tradeSeqTarget_, CB_RESETCONTENT, 0, 0);
        if (tradeEditorMode_ == 1) {
            SendMessageW(tradeSeqTarget_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"MAIN (DÙNG CHUNG)"));
            SendMessageW(tradeSeqTarget_, CB_SETCURSEL, 0, 0);
            EnableWindow(tradeSeqTarget_, FALSE);
            return;
        }
        EnableWindow(tradeSeqTarget_, TRUE);
        const std::wstring childName = L"ACC CON ĐANG GD (DÙNG CHUNG)";
        SendMessageW(tradeSeqTarget_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(childName.c_str()));
        for (std::size_t i = 0; i < mainTradeSequence_.size(); ++i) {
            std::wstring label = L"MAIN #" + std::to_wstring(i + 1) + L" • " + (mainTradeSequence_[i].description.empty() ? L"(không mô tả)" : mainTradeSequence_[i].description);
            SendMessageW(tradeSeqTarget_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
        }
        int sel = 0;
        if (step && step->target == 1 && step->mainRef >= 0 && step->mainRef < static_cast<int>(mainTradeSequence_.size())) sel = step->mainRef + 1;
        SendMessageW(tradeSeqTarget_, CB_SETCURSEL, sel, 0);
    }

    void LoadTradeSequenceRowToEditor(int index) {
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        if (!seq || index < 0 || index >= static_cast<int>(seq->size())) return;
        TradeSequenceStep& step = (*seq)[static_cast<std::size_t>(index)];
        const TradeSequenceStep* effective = EffectiveEditorStep(step);
        PopulateTradeTargetCombo(&step);
        if (tradeSeqKind_) SendMessageW(tradeSeqKind_, CB_SETCURSEL, step.kind, 0);
        if (effective) {
            SetText(tradeSeqDesc_, effective->description);
            SetText(tradeSeqDelay_, std::to_wstring(effective->delayMs));
            SetText(tradeSeqRepeat_, std::to_wstring(effective->repeat));
        }
        if (tradeSeqGroupRepeat_) SetText(tradeSeqGroupRepeat_, std::to_wstring(step.groupId > 0 ? step.groupRepeat : 1));
        const bool sharedRef = tradeEditorMode_ == 2 && step.target == 1;
        EnableWindow(tradeSeqDesc_, !sharedRef);
        EnableWindow(tradeSeqDelay_, !sharedRef);
        EnableWindow(tradeSeqRepeat_, !sharedRef);
        if (tradeSeqKind_) EnableWindow(tradeSeqKind_, !sharedRef && tradeEditorMode_ == 2);
        if (tradeSeqGroupRepeat_) EnableWindow(tradeSeqGroupRepeat_, tradeEditorMode_ == 2);
    }

    void SaveEditorSequence() {
        if (tradeEditorMode_ == 1) SaveMainTradeSequence();
        else if (tradeEditorMode_ == 2) SaveSharedChildTradeSequence();
    }

    void AddTradeSequenceRow() {
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        if (!seq || seq->size() >= 64) return;
        TradeSequenceStep step{};
        if (tradeEditorMode_ == 1) {
            step.target = 1; step.mainRef = static_cast<int>(seq->size()); step.description = L"MAIN bước " + std::to_wstring(seq->size() + 1);
        } else {
            step.target = 0; step.mainRef = -1; step.description = L"CON bước " + std::to_wstring(seq->size() + 1);
        }
        seq->push_back(step); SaveEditorSequence(); RefreshTradeSequenceList();
        const int row = static_cast<int>(seq->size()) - 1;
        ListView_SetItemState(tradeSeqList_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(tradeSeqList_, row, FALSE); LoadTradeSequenceRowToEditor(row);
    }

    void DeleteTradeSequenceRow() {
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        std::vector<int> rows = SelectedRows(tradeSeqList_);
        if (!seq || rows.empty()) return;
        rows.erase(std::remove_if(rows.begin(), rows.end(), [&](int row) {
            return row < 0 || row >= static_cast<int>(seq->size());
        }), rows.end());
        if (rows.empty()) return;
        std::sort(rows.begin(), rows.end());

        if (tradeEditorMode_ == 1) {
            // Repair the one shared ACC CON workflow against all deleted MAIN refs before erasing rows.
            EnsureSharedChildTradeSequence();
            for (TradeSequenceStep& cs : childTradeSequence_) if (cs.target == 1) {
                if (std::binary_search(rows.begin(), rows.end(), cs.mainRef)) {
                    cs.mainRef = -1;
                } else {
                    cs.mainRef -= static_cast<int>(std::lower_bound(rows.begin(), rows.end(), cs.mainRef) - rows.begin());
                }
            }
            SaveSharedChildTradeSequence();
        }

        for (auto it = rows.rbegin(); it != rows.rend(); ++it) seq->erase(seq->begin() + *it);
        if (tradeEditorMode_ == 1) {
            for (std::size_t i = 0; i < mainTradeSequence_.size(); ++i) mainTradeSequence_[i].mainRef = static_cast<int>(i);
        } else if (tradeEditorMode_ == 2) {
            NormalizeTradeGroups(*seq);
        }
        SaveEditorSequence();
        RefreshTradeSequenceList();
    }

    void MoveTradeSequenceRow(int delta) {
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        const int row = SelectedTradeSequenceIndex();
        if (!seq || row < 0) return;
        const int next = row + delta;
        if (next < 0 || next >= static_cast<int>(seq->size())) return;
        if (tradeEditorMode_ == 1) {
            // Preserve shared ACC CON references by swapping ref IDs with MAIN content.
            std::swap((*seq)[static_cast<std::size_t>(row)], (*seq)[static_cast<std::size_t>(next)]);
            EnsureSharedChildTradeSequence();
            for (TradeSequenceStep& cs : childTradeSequence_) if (cs.target == 1) {
                if (cs.mainRef == row) cs.mainRef = next; else if (cs.mainRef == next) cs.mainRef = row;
            }
            for (std::size_t i = 0; i < seq->size(); ++i) (*seq)[i].mainRef = static_cast<int>(i);
            SaveSharedChildTradeSequence();
        } else {
            std::swap((*seq)[static_cast<std::size_t>(row)], (*seq)[static_cast<std::size_t>(next)]);
            NormalizeTradeGroups(*seq);
        }
        SaveEditorSequence(); RefreshTradeSequenceList();
        ListView_SetItemState(tradeSeqList_, next, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        LoadTradeSequenceRowToEditor(next);
    }

    void SaveTradeSequenceRowFromEditor(bool refreshUi = true, int forcedRow = -1) {
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        const int row = forcedRow >= 0 ? forcedRow : SelectedTradeSequenceIndex();
        if (!seq || row < 0 || row >= static_cast<int>(seq->size())) return;
        TradeSequenceStep& step = (*seq)[static_cast<std::size_t>(row)];
        if (tradeEditorMode_ == 1) {
            step.target = 1; step.mainRef = row; step.kind = 0;
            step.description = GetText(tradeSeqDesc_);
            step.delayMs = std::clamp(_wtoi(GetText(tradeSeqDelay_).c_str()), 50, 60000);
            step.repeat = std::clamp(_wtoi(GetText(tradeSeqRepeat_).c_str()), 1, 999);
        } else {
            const LRESULT targetSel = SendMessageW(tradeSeqTarget_, CB_GETCURSEL, 0, 0);
            if (targetSel > 0) {
                step.target = 1; step.mainRef = static_cast<int>(targetSel - 1); step.kind = 0;
            } else {
                step.target = 0; step.mainRef = -1;
                step.kind = SendMessageW(tradeSeqKind_, CB_GETCURSEL, 0, 0) == 1 ? 1 : 0;
                step.description = GetText(tradeSeqDesc_);
                step.delayMs = std::clamp(_wtoi(GetText(tradeSeqDelay_).c_str()), 50, 60000);
                step.repeat = std::clamp(_wtoi(GetText(tradeSeqRepeat_).c_str()), 1, 999);
            }
        }
        SaveEditorSequence();
        if (!refreshUi) return;
        RefreshTradeSequenceList();
        ListView_SetItemState(tradeSeqList_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        LoadTradeSequenceRowToEditor(row);
    }

    Account* TradeSequenceCaptureAccount(TradeSequenceStep& step, ClickPoint*& pointOut) {
        pointOut = nullptr;
        if (tradeEditorMode_ == 1) {
            Account* main = AccountByTradeRole(1); pointOut = &step.point; return main;
        }
        // A child-sequence row may reference MAIN. Do not require the donor CON to still
        // exist just to capture a MAIN coordinate. Resolve the actual target first.
        if (step.target == 1) {
            if (step.mainRef < 0 || step.mainRef >= static_cast<int>(mainTradeSequence_.size())) return nullptr;
            pointOut = &mainTradeSequence_[static_cast<std::size_t>(step.mainRef)].point;
            return AccountByTradeRole(1);
        }
        Account* child = TradeEditorChild();
        if (!child) return nullptr;
        pointOut = &step.point; return child;
    }

    void BeginTradeSequenceCapture() {
        // Freeze the exact row/target before arming F8. Saving the editor used to rebuild
        // the ListView first, so drag/multi-selection could change the row used by capture.
        const int row = SelectedTradeSequenceIndex();
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        if (!seq || row < 0 || row >= static_cast<int>(seq->size())) {
            Log(L"BĐPT: hãy chọn đúng một dòng đang focus để lấy tọa độ chuỗi GD.");
            return;
        }
        SaveTradeSequenceRowFromEditor(false, row);
        seq = EditorSequence();
        if (!seq || row >= static_cast<int>(seq->size())) return;

        TradeSequenceStep& step = (*seq)[static_cast<std::size_t>(row)];
        ClickPoint* point = nullptr;
        Account* target = TradeSequenceCaptureAccount(step, point);
        if (!target || !point) { Log(L"BĐPT: không xác định được cửa sổ để lấy tọa độ chuỗi GD."); return; }

        captureSlot_ = ClickSlot::None;
        captureMacroIndex_ = -1;
        captureTradeSequenceIndex_ = row;
        captureTradeSequenceMode_ = tradeEditorMode_;
        captureTradeSequenceMainRef_ = (tradeEditorMode_ == 2 && step.target == 1) ? step.mainRef : -1;
        capturePid_ = target->game.pid;
        LogAccount(*target, L"BĐPT yêu cầu lấy tọa chuỗi GD dòng " + std::to_wstring(row + 1) + L" → đưa chuột vào đúng vị trí và F8.");
    }

    void TestTradeSequenceRow() {
        SaveTradeSequenceRowFromEditor();
        std::vector<TradeSequenceStep>* seq = EditorSequence();
        const int row = SelectedTradeSequenceIndex();
        if (!seq || row < 0 || row >= static_cast<int>(seq->size())) return;
        TradeSequenceStep& stored = (*seq)[static_cast<std::size_t>(row)];
        const TradeSequenceStep* effective = EffectiveEditorStep(stored);
        Account* target = nullptr;
        if (tradeEditorMode_ == 1 || stored.target == 1) target = AccountByTradeRole(1); else target = TradeEditorChild();
        if (!target || !effective) { Log(L"BĐPT: TEST dòng không xác định được acc/MAIN reference."); return; }
        std::wstring error;
        if (!CoordinatorInternalPointAction(
                *target, effective->point,
                L"TEST CHUỖI GD dòng " + std::to_wstring(row + 1), error)) {
            LogAccount(*target, L"TEST chuỗi GD FAIL: " + error); return;
        }
        LogAccount(*target, L"TEST chuỗi GD dòng " + std::to_wstring(row + 1) +
                           L" PASS qua HIDDEN ACTION BĐPT.");
    }

    void BuildTradeEditorUi(HWND parent) {
        auto addColumn = [&](int index, int width, const wchar_t* text) {
            LVCOLUMNW c{}; c.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM; c.pszText = const_cast<wchar_t*>(text); c.cx = width; c.iSubItem = index;
            ListView_InsertColumn(tradeSeqList_, index, &c);
        };
        const wchar_t* heading = tradeEditorMode_ == 1
            ? L"CHUỖI GD MAIN — tọa MAIN dùng chung cho mọi giao dịch"
            : L"CHUỖI GD ACC CON (DÙNG CHUNG) — mọi CON1..CON6 dùng đúng một workflow này";
        MakeIn(parent, L"STATIC", heading, 0, 15, 10, 850, 23, 0);
        tradeSeqList_ = MakeIn(parent, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER, 15, 38, 850, 235, IDC_SEQ_LIST);
        ListView_SetExtendedListViewStyle(tradeSeqList_, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
        SetWindowSubclass(tradeSeqList_, TradeSequenceListSubclassProc, 1, reinterpret_cast<DWORD_PTR>(this));
        addColumn(0, 35, L"#"); addColumn(1, 100, L"ACC THỰC HIỆN"); addColumn(2, 85, L"Loại");
        addColumn(3, 180, L"Mô tả"); addColumn(4, 160, L"Tọa độ"); addColumn(5, 60, L"Delay"); addColumn(6, 50, L"Lặp"); addColumn(7, 105, L"Nhóm lặp");
        MakeIn(parent, L"STATIC", L"ACC:", 0, 15, 287, 38, 22, 0);
        tradeSeqTarget_ = MakeIn(parent, WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_VSCROLL, 55, 282, 210, 220, IDC_SEQ_TARGET);
        MakeIn(parent, L"STATIC", L"Loại:", 0, 275, 287, 40, 22, 0);
        tradeSeqKind_ = MakeIn(parent, WC_COMBOBOXW, L"", CBS_DROPDOWNLIST, 317, 282, 120, 150, IDC_SEQ_KIND);
        SendMessageW(tradeSeqKind_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"CLICK"));
        SendMessageW(tradeSeqKind_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"CHUYỂN ĐỒ"));
        SendMessageW(tradeSeqKind_, CB_SETCURSEL, 0, 0);
        MakeIn(parent, L"STATIC", L"Mô tả:", 0, 447, 287, 50, 22, 0);
        tradeSeqDesc_ = MakeIn(parent, L"EDIT", L"", WS_BORDER | ES_AUTOHSCROLL, 499, 282, 180, 27, IDC_SEQ_DESC);
        MakeIn(parent, L"STATIC", L"Delay:", 0, 687, 287, 42, 22, 0);
        tradeSeqDelay_ = MakeIn(parent, L"EDIT", L"500", WS_BORDER | ES_NUMBER | ES_CENTER, 731, 282, 55, 27, IDC_SEQ_DELAY);
        MakeIn(parent, L"STATIC", L"Lặp:", 0, 793, 287, 32, 22, 0);
        tradeSeqRepeat_ = MakeIn(parent, L"EDIT", L"1", WS_BORDER | ES_NUMBER | ES_CENTER, 827, 282, 38, 27, IDC_SEQ_REPEAT);
        MakeIn(parent, L"BUTTON", L"+ THÊM", BS_PUSHBUTTON, 15, 323, 90, 30, IDC_SEQ_ADD);
        MakeIn(parent, L"BUTTON", L"- XÓA", BS_PUSHBUTTON, 112, 323, 80, 30, IDC_SEQ_DELETE);
        MakeIn(parent, L"BUTTON", L"LÊN", BS_PUSHBUTTON, 199, 323, 70, 30, IDC_SEQ_UP);
        MakeIn(parent, L"BUTTON", L"XUỐNG", BS_PUSHBUTTON, 276, 323, 75, 30, IDC_SEQ_DOWN);
        MakeIn(parent, L"BUTTON", L"LƯU DÒNG", BS_PUSHBUTTON, 360, 323, 105, 30, IDC_SEQ_SAVE);
        MakeIn(parent, L"BUTTON", L"LẤY TỌA (F8)", BS_PUSHBUTTON, 474, 323, 125, 30, IDC_SEQ_CAPTURE);
        MakeIn(parent, L"BUTTON", L"TEST DÒNG", BS_PUSHBUTTON, 608, 323, 105, 30, IDC_SEQ_TEST);
        MakeIn(parent, L"BUTTON", L"ĐÓNG", BS_PUSHBUTTON, 775, 323, 90, 30, IDC_SEQ_CLOSE);
        tradeRecordButton_ = MakeIn(parent, L"BUTTON", L"REC", BS_PUSHBUTTON, 15, 360, 92, 30, IDC_SEQ_REC);
        MakeIn(parent, L"BUTTON", L"SAO CHÉP", BS_PUSHBUTTON, 115, 360, 105, 30, IDC_SEQ_COPY);
        MakeIn(parent, L"BUTTON", L"DÁN", BS_PUSHBUTTON, 228, 360, 80, 30, IDC_SEQ_PASTE);
        if (tradeEditorMode_ == 2) {
            MakeIn(parent, L"STATIC", L"Lặp nhóm:", 0, 320, 365, 68, 22, 0);
            tradeSeqGroupRepeat_ = MakeIn(parent, L"EDIT", L"2", WS_BORDER | ES_NUMBER | ES_CENTER, 390, 360, 45, 30, IDC_SEQ_GROUP_REPEAT);
            MakeIn(parent, L"BUTTON", L"GOM DÒNG ĐÃ CHỌN", BS_PUSHBUTTON, 443, 360, 170, 30, IDC_SEQ_GROUP_SELECTED);
            MakeIn(parent, L"BUTTON", L"BỎ NHÓM", BS_PUSHBUTTON, 621, 360, 110, 30, IDC_SEQ_UNGROUP);
        }
        tradeRecordStatus_ = MakeIn(parent, L"STATIC", L"REC: sẵn sàng • chọn nhiều dòng liên tiếp để GOM và lặp mini-sequence", SS_LEFT | SS_CENTERIMAGE, 15, 398, 850, 30, 0);
        MakeIn(parent, L"STATIC", tradeEditorMode_ == 1
            ? L"MAIN sequence là thư viện tọa dùng chung. ACC CON workflow tham chiếu MAIN #n; sửa MAIN một lần áp dụng mọi giao dịch."
            : L"GOM 1/2/3/... dòng liên tiếp thành một nhóm; nhóm chạy đủ số lần rồi chuỗi lớn mới đi tiếp.", 0, 15, 435, 850, 23, 0);
        PopulateTradeTargetCombo(); RefreshTradeSequenceList();
    }

    void OpenTradeSequenceEditor(int mode) {
        Account* selected = SelectedAccount();
        if (mode == 1 && (!selected || selected->profile.tradeRole != 1)) { Log(L"Chỉ acc MAIN mới mở CHUỖI GD MAIN."); return; }
        if (mode == 2 && (!selected || selected->profile.tradeRole < 2)) { Log(L"Chọn một CON bất kỳ để mở CHUỖI GD ACC CON dùng chung."); return; }
        if (tradeEditor_ && IsWindow(tradeEditor_)) DestroyWindow(tradeEditor_);
        tradeEditorMode_ = mode;
        tradeEditorChildPid_ = mode == 2 ? selected->game.pid : 0;
        if (mode == 2) EnsureSharedChildTradeSequence();
        WNDCLASSEXW wc{}; wc.cbSize = sizeof(wc); wc.lpfnWndProc = TradeEditorWndProc; wc.hInstance = instance_; wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1); wc.lpszClassName = L"ThanLongTradeSequenceEditorV03";
        if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) { Log(L"Không đăng ký được cửa sổ chuỗi GD."); return; }
        const wchar_t* title = mode == 1 ? L"Thần Long • CHUỖI GD MAIN (DÙNG CHUNG) v0.3 • REC"
                                         : L"Thần Long • CHUỖI GD ACC CON (DÙNG CHUNG) v0.3 • REC + NHÓM LẶP";
        tradeEditor_ = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                       CW_USEDEFAULT, CW_USEDEFAULT, 900, 530, hwnd_, nullptr, instance_, this);
        if (!tradeEditor_) { Log(L"Không mở được cửa sổ chuỗi GD."); return; }
        BuildTradeEditorUi(tradeEditor_); ShowWindow(tradeEditor_, SW_SHOW); UpdateWindow(tradeEditor_);
    }

    LRESULT HandleTradeEditor(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
            case WM_NOTIFY: {
                auto* hdr = reinterpret_cast<NMHDR*>(lp);
                if (hdr && hdr->idFrom == IDC_SEQ_LIST && hdr->code == LVN_ITEMCHANGED) {
                    const auto* n = reinterpret_cast<const NMLISTVIEW*>(hdr);
                    if (!tradeSeqDragUpdating_ && (n->uChanged & LVIF_STATE) != 0 && (n->uNewState & LVIS_SELECTED) != 0)
                        LoadTradeSequenceRowToEditor(n->iItem);
                }
                return 0;
            }
            case WM_COMMAND:
                switch (LOWORD(wp)) {
                    case IDC_SEQ_ADD: AddTradeSequenceRow(); return 0;
                    case IDC_SEQ_DELETE: DeleteTradeSequenceRow(); return 0;
                    case IDC_SEQ_UP: MoveTradeSequenceRow(-1); return 0;
                    case IDC_SEQ_DOWN: MoveTradeSequenceRow(1); return 0;
                    case IDC_SEQ_SAVE: SaveTradeSequenceRowFromEditor(); return 0;
                    case IDC_SEQ_CAPTURE: BeginTradeSequenceCapture(); return 0;
                    case IDC_SEQ_TEST: TestTradeSequenceRow(); return 0;
                    case IDC_SEQ_REC: ToggleTradeRecorder(); return 0;
                    case IDC_SEQ_COPY: CopySelectedTradeRows(); return 0;
                    case IDC_SEQ_PASTE: PasteTradeRows(); return 0;
                    case IDC_SEQ_GROUP_SELECTED: GroupSelectedTradeRows(); return 0;
                    case IDC_SEQ_UNGROUP: UngroupSelectedTradeRows(); return 0;
                    case IDC_SEQ_CLOSE: if (RecorderModeIsTrade(recorderMode_)) StopRecorder(true); DestroyWindow(hwnd); return 0;
                    case IDC_SEQ_TARGET: if (HIWORD(wp) == CBN_SELCHANGE) SaveTradeSequenceRowFromEditor(); return 0;
                    case IDC_SEQ_KIND:
                        if (HIWORD(wp) == CBN_SELCHANGE && tradeEditorMode_ == 2 && SendMessageW(tradeSeqKind_, CB_GETCURSEL, 0, 0) == 1) SendMessageW(tradeSeqTarget_, CB_SETCURSEL, 0, 0);
                        return 0;
                }
                break;
            case WM_CLOSE: if (RecorderModeIsTrade(recorderMode_)) StopRecorder(true); DestroyWindow(hwnd); return 0;
            case WM_NCDESTROY:
                tradeEditor_ = nullptr; tradeSeqList_ = nullptr; tradeSeqTarget_ = nullptr; tradeSeqKind_ = nullptr; tradeSeqDesc_ = nullptr; tradeSeqDelay_ = nullptr; tradeSeqRepeat_ = nullptr; tradeSeqGroupRepeat_ = nullptr; tradeRecordButton_ = nullptr; tradeRecordStatus_ = nullptr;
                captureTradeSequenceIndex_ = -1; captureTradeSequenceMode_ = 0; captureTradeSequenceMainRef_ = -1; tradeEditorMode_ = 0; tradeEditorChildPid_ = 0;
                return DefWindowProcW(hwnd, msg, wp, lp);
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

    void ShowSellMacroEditor(bool show) {
        sellMacroEditorVisible_ = show;
        for (HWND h : sellMacroControls_) if (h) ShowWindow(h, show ? SW_SHOW : SW_HIDE);
        if (logCaption_) SetWindowPos(logCaption_, nullptr, 18, show ? 882 : 742, 190, 20, SWP_NOZORDER);
        if (log_) SetWindowPos(log_, nullptr, 18, show ? 902 : 764, 1005, show ? 60 : 159, SWP_NOZORDER);
        if (show) { RefreshSellMacroList(); ClearSellMacroEditor(); }
    }

    void ToggleSellMacroEditor() {
        if (sellMacroEditorVisible_ && recorderMode_ == RecorderMode::Sell) StopRecorder(true);
        Account* a = SelectedAccount();
        if (!a) { Log(L"CHUỖI CLICK BÁN ĐỒ: chưa chọn acc."); return; }
        ShowSellMacroEditor(!sellMacroEditorVisible_);
    }

    void UpdateRoleActionButtons() {
        Account* a = SelectedAccount();
        const int role = a ? a->profile.tradeRole : 0;
        if (sellSequenceButton_) ShowWindow(sellSequenceButton_, a ? SW_SHOW : SW_HIDE);
        if (mainTradeSequenceButton_) ShowWindow(mainTradeSequenceButton_, role == 1 ? SW_SHOW : SW_HIDE);
        if (childTradeSequenceButton_) {
            ShowWindow(childTradeSequenceButton_, role >= 2 ? SW_SHOW : SW_HIDE);
            if (role >= 2) SetWindowTextW(childTradeSequenceButton_, L"CHUỖI GD ACC CON");
        }
        if (tradeRendezvousCaptureButton_) ShowWindow(tradeRendezvousCaptureButton_, SW_SHOW);
        if (tradeRendezvousLabel_) ShowWindow(tradeRendezvousLabel_, SW_SHOW);
        if (!a && sellMacroEditorVisible_) ShowSellMacroEditor(false);
    }

    void PersistGlobalTradeSettings() {
        if (mainSellThresholdEdit_) mainSellThreshold_ = std::clamp(_wtoi(GetText(mainSellThresholdEdit_).c_str()), 1, 90);
        WriteIniInt(L"Global", L"TradeEnabled", tradeEnabled_ ? 1 : 0);
        WriteIniInt(L"Global", L"ChildTriggerFreeSlots", 0);
        WriteIniInt(L"Global", L"MainSellThreshold", mainSellThreshold_);
        WriteIniInt(L"Global", L"TradeRendezvousMap", tradeRendezvous_.mapID);
        WriteIniInt(L"Global", L"TradeRendezvousX", tradeRendezvous_.x);
        WriteIniInt(L"Global", L"TradeRendezvousY", tradeRendezvous_.y);
        WriteIniInt(L"Global", L"TradeRendezvousValid", tradeRendezvous_.valid ? 1 : 0);
        WriteIniInt(L"Global", L"TradeRendezvousTolerance", tradeRendezvousTolerance_);
        if (mainSellThresholdEdit_) SetText(mainSellThresholdEdit_, std::to_wstring(mainSellThreshold_));
        FlushIni();
    }

    void UpdateConsolidationButton() {
        if (!tradeEnable_) return;
        SetWindowTextW(tradeEnable_, tradeEnabled_ ? L"DỒN ĐỒ: BẬT" : L"DỒN ĐỒ: TẮT");
    }

    void ToggleConsolidationMode() {
        tradeEnabled_ = !tradeEnabled_;
        if (!tradeEnabled_) {
            if (tradeTxn_.phase != TradePhase::Idle) {
                AbortTrade(L"người dùng TẮT DỒN ĐỒ", GetTickCount());
            }
            ReleaseTradeHolds(); // hard cleanup: no stale rendezvous HOLD may survive OFF.
            SetTradeStatus(L"DỒN ĐỒ TẮT • scheduler GD bị chặn thật • các acc auto-train/bán đồ độc lập");
            Log(L"DỒN ĐỒ: TẮT → vô hiệu hóa giao dịch MAIN↔CON; chỉ acc đã tick AUTO BÁN ĐỒ mới tự bán khi túi FULL.");
        } else {
            SetTradeStatus(L"DỒN ĐỒ BẬT • scheduler MAIN↔CON hoạt động");
            Log(L"DỒN ĐỒ: BẬT → khôi phục điều phối MAIN↔CON; MAIN bán ở ngưỡng riêng, CON FULL ưu tiên giao dịch.");
        }
        WriteIniInt(L"Global", L"TradeEnabled", tradeEnabled_ ? 1 : 0);
        FlushIni();
        UpdateConsolidationButton();
    }

    void CopySellSequenceFromAnotherAccount() {
        Account* target = SelectedAccount();
        if (!target) { Log(L"LẤY CHUỖI BÁN: chưa chọn acc đích."); return; }

        HMENU menu = CreatePopupMenu();
        if (!menu) return;
        constexpr UINT kBase = 7300;
        std::vector<Account*> sources;
        for (auto& item : accounts_) {
            Account* source = item.get();
            if (!source || source == target || source->profile.sellMacro.empty()) continue;
            sources.push_back(source);
            const std::wstring label = AccountTag(*source) + L" • " + std::to_wstring(source->profile.sellMacro.size()) + L" bước";
            AppendMenuW(menu, MF_STRING, kBase + static_cast<UINT>(sources.size() - 1), label.c_str());
        }
        if (sources.empty()) {
            AppendMenuW(menu, MF_STRING | MF_GRAYED, 1, L"Chưa có acc khác có chuỗi bán");
        }

        RECT rc{};
        if (sellMacroList_) GetWindowRect(sellMacroList_, &rc);
        else GetWindowRect(hwnd_, &rc);
        const UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN,
                                        rc.left + 12, rc.top + 12, 0, hwnd_, nullptr);
        DestroyMenu(menu);
        if (cmd < kBase || cmd >= kBase + sources.size()) return;

        Account* source = sources[cmd - kBase];
        if (!source) return;
        if (!target->profile.sellMacro.empty()) {
            const std::wstring q = L"Acc đích đang có " + std::to_wstring(target->profile.sellMacro.size()) +
                                   L" bước bán.\n\nThay toàn bộ bằng chuỗi của " + AccountTag(*source) + L"?";
            if (MessageBoxW(hwnd_, q.c_str(), L"LẤY CHUỖI CLICK BÁN ĐỒ", MB_YESNO | MB_ICONQUESTION) != IDYES) return;
        }
        target->profile.sellMacro = source->profile.sellMacro;
        SaveProfile(target->profile);
        RefreshSellMacroList();
        ClearSellMacroEditor();
        LogAccount(*target, L"Đã lấy CHUỖI CLICK BÁN ĐỒ từ " + AccountTag(*source) +
                            L" • " + std::to_wstring(target->profile.sellMacro.size()) + L" bước.");
    }

    void SetTradeStatus(const std::wstring& text) {
        if (!tradeStatus_) return;
        const std::wstring line = L"ĐIỀU PHỐI: " + text;
        SetWindowTextW(tradeStatus_, line.c_str());
    }

    void ApplySelectedTradeRole() {
        Account* selected = SelectedAccount();
        if (!selected || !tradeRoleCombo_) return;
        const LRESULT sel = SendMessageW(tradeRoleCombo_, CB_GETCURSEL, 0, 0);
        if (sel == CB_ERR || sel < 0 || sel > 7) return;
        const int newRole = static_cast<int>(sel);
        if (newRole != 0) {
            for (auto& item : accounts_) {
                Account& other = *item;
                if (&other == selected) continue;
                if (other.profile.tradeRole == newRole) {
                    other.profile.tradeRole = 0;
                    other.tradeHeld = false;
                    SaveProfile(other.profile);
                    LogAccount(other, L"Vai trò " + TradeRoleLabel(newRole) + L" được chuyển sang acc khác → trả về KHÔNG.");
                }
            }
        }
        selected->profile.tradeRole = newRole;
        if (newRole == 1) {
            selected->profile.enableSell = true; // MAIN must sell immediately at/below threshold.
            if (enableSell_) SendMessageW(enableSell_, BM_SETCHECK, BST_CHECKED, 0);
        }
        if (newRole >= 2) {
            selected->profile.enableSell = false; // hard rule: child never sells.
            selected->runtime.sellPhase = 0;
            if (enableSell_) SendMessageW(enableSell_, BM_SETCHECK, BST_UNCHECKED, 0);
        }
        SaveProfile(selected->profile);
        for (std::size_t i = 0; i < accounts_.size(); ++i) UpdateAccountRow(static_cast<int>(i), *accounts_[i]);
        LoadSelectedProfileToUi();
        LogAccount(*selected, L"Đặt vai trò giao dịch = " + TradeRoleLabel(newRole));
    }

    Account* AccountByTradeRole(int role) {
        for (auto& a : accounts_) if (a->profile.tradeRole == role) return a.get();
        return nullptr;
    }

    bool TradeStateReady(const Account& a) const {
        if (!a.runtime.running || !a.snapshotValid || !IsWindow(a.game.window)) return false;
        const Snapshot& s = a.snapshot;
        const std::uint32_t need = ValidLifeState | ValidBagSpace | ValidMap | ValidPosition;
        if ((s.validMask & need) != need) return false;
        if (s.dead || !s.mapReady || s.waitingChangeMap) return false;
        if (a.runtime.clientFreezeActive || a.runtime.revivePhase != 0 || a.runtime.sellPhase != 0 ||
            a.runtime.trainRecoveryPhase != 0 || a.runtime.routeOwnershipResetPending) return false;
        return true;
    }

    bool TradePairReadyForPreparation(const Account& main, const Account& child) const {
        if (!main.runtime.running || !child.runtime.running) return false;
        if (!main.snapshotValid || !child.snapshotValid) return false;
        const Snapshot& ms = main.snapshot;
        const Snapshot& cs = child.snapshot;
        const std::uint32_t need = ValidLifeState | ValidBagSpace | ValidMap | ValidPosition | ValidAutoFight | ValidAutoPath | ValidRiding;
        if ((ms.validMask & need) != need || (cs.validMask & need) != need) return false;
        if (ms.dead || cs.dead || !ms.mapReady || ms.waitingChangeMap || !cs.mapReady || cs.waitingChangeMap) return false;
        return IsWindow(main.game.window) && IsWindow(child.game.window);
    }

    void ResetTradeRendezvousTravel(Account& a) {
        RuntimeState& rt = a.runtime;
        rt.tradeTravelPhase = 0;
        rt.tradeTravelTick = 0;
        rt.tradeTravelReady = false;
        ResetRobustTravel(rt);
    }

    bool TradeAccountAtRendezvous(const Account& a) const {
        if (!tradeRendezvous_.valid || !a.snapshotValid) return false;
        const Snapshot& s = a.snapshot;
        const std::uint32_t need = ValidLifeState | ValidMap | ValidPosition | ValidAutoPath | ValidRiding;
        if ((s.validMask & need) != need || s.dead || !s.mapReady || s.waitingChangeMap) return false;
        State state{};
        state.valid = true; state.mapReady = true; state.waitingMap = false;
        state.mapID = s.mapID; state.x = s.x; state.y = s.y;
        state.autoPathing = s.autoPathing != 0; state.riding = s.riding != 0;
        Target target{tradeRendezvous_.mapID, tradeRendezvous_.x, tradeRendezvous_.y, tradeRendezvousTolerance_};
        return AtTarget(state, target) && !s.autoPathing && !s.riding;
    }

    void BeginTradeRendezvousTravel(Account& a, DWORD now, const wchar_t* who) {
        RuntimeState& rt = a.runtime;
        ResetTradeRendezvousTravel(a);
        rt.tradeTravelPhase = 4;
        rt.tradeTravelTick = now;
        rt.trainPositionMonitorArmed = false;
        rt.lastTrainPositionCheckTick = 0;
        rt.fightPhase = 0;
        rt.fightAttempts = 0;

        // The old train AutoPath belongs to the normal core and must not survive into
        // a consolidation rendezvous. This StopPath is internal and does not touch F4 state.
        if (a.bridge.Attached() && a.snapshotValid && (a.snapshot.validMask & ValidAutoPath) && a.snapshot.autoPathing) {
            Response r{}; std::wstring ignored;
            (void)a.bridge.Call(Command::StopPath, 0, 0, 0, r, ignored, 700);
        }
        LogAccount(a, L"GD TỌA: HOLD " + std::wstring(who ? who : L"ACC") +
                      L" • hủy AutoPath bãi cũ → v0.3 Travel Guard bắt buộc AutoFight OFF → cùng đi TỌA GD.");
    }

    bool HandleTradeRendezvousTravel(Account& a, DWORD now, const wchar_t* who) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        const std::wstring tag = who ? who : L"ACC";

        if (!tradeRendezvous_.valid || !a.runtime.running || !a.snapshotValid || !IsWindow(a.game.window)) return false;
        if (rt.autoPathFightConflictLatched) {
            rt.status = L"GD TỌA • chờ ROUTE/FIGHT INVARIANT recovery";
            return true;
        }
        const std::uint32_t need = ValidLifeState | ValidMap | ValidPosition | ValidAutoPath | ValidRiding;
        if ((s.validMask & need) != need || s.dead || !s.mapReady || s.waitingChangeMap) return false;

        // Once ready, keep the first-arriving account parked at TỌA GD. Any stale/automatic
        // path that reappears is stopped before the coordinator can advance the transaction.
        if (rt.tradeTravelReady) {
            if (s.autoPathing) {
                Response r{}; std::wstring ignored;
                if (a.bridge.Attached()) (void)a.bridge.Call(Command::StopPath, 0, 0, 0, r, ignored, 700);
                rt.status = L"GD HOLD • " + tag + L" đã tới TỌA GD • chặn AutoPath bãi cũ";
                return true;
            }
            if (s.riding) {
                (void)SendDecision(a, Action::Dismount, tradeRendezvous_, L"TỌA GD");
                rt.status = L"GD HOLD • " + tag + L" xuống ngựa tại TỌA GD";
                return true;
            }
            if (!TradeAccountAtRendezvous(a)) {
                rt.tradeTravelReady = false;
                rt.tradeTravelPhase = 4;
                rt.tradeTravelTick = now;
                ResetRobustTravel(rt);
                rt.status = L"GD RELOCK • " + tag + L" lệch TỌA GD → quay lại";
                return true;
            }
            rt.status = L"GD HOLD • " + tag + L" đã tới TỌA GD • chờ acc còn lại";
            return true;
        }

        if (rt.tradeTravelPhase == 0) {
            BeginTradeRendezvousTravel(a, now, who);
            return true;
        }

        if (rt.tradeTravelPhase == 4) {
            bool arrived = false;
            (void)HandleRobustTravel(a, now, tradeRendezvous_, L"TỌA GD", arrived, tradeRendezvousTolerance_);
            if (!arrived) {
                rt.status = L"GD TỌA • " + tag + L" đang đi M" + std::to_wstring(tradeRendezvous_.mapID) +
                            L" " + std::to_wstring(tradeRendezvous_.x) + L"," + std::to_wstring(tradeRendezvous_.y);
                return true;
            }
            rt.tradeTravelPhase = 5;
            rt.tradeTravelTick = now;
            rt.status = L"GD TỌA • " + tag + L" đã tới • khóa path và verify";
            return true;
        }

        if (rt.tradeTravelPhase == 5) {
            if (s.autoPathing) {
                Response r{}; std::wstring ignored;
                if (a.bridge.Attached()) (void)a.bridge.Call(Command::StopPath, 0, 0, 0, r, ignored, 700);
                rt.tradeTravelTick = now;
                rt.status = L"GD HOLD • " + tag + L" StopPath tại TỌA GD";
                return true;
            }
            if (s.riding) {
                (void)SendDecision(a, Action::Dismount, tradeRendezvous_, L"TỌA GD");
                rt.tradeTravelTick = now;
                return true;
            }
            if (!Elapsed(now, rt.tradeTravelTick, 450)) return true;

            // v0.3: reuse the same fail-closed guard at the rendezvous itself. No separate
            // trade stop-Auto state machine remains.
            if ((s.validMask & ValidAutoFight) == 0 || s.autoFight) {
                if (!EnsureAutoFightOffForTravel(a, now, L"TỌA GD")) {
                    rt.status = L"GD HOLD • " + tag + L" chờ Travel Guard xác nhận AutoFight OFF";
                    return true;
                }
            }
            if (!TradeAccountAtRendezvous(a)) {
                rt.tradeTravelPhase = 4; rt.tradeTravelTick = now; ResetRobustTravel(rt);
                return true;
            }
            rt.tradeTravelReady = true;
            rt.tradeTravelPhase = 0;
            rt.status = L"GD HOLD • " + tag + L" ĐÃ TỚI TỌA GD";
            LogAccount(a, L"GD TỌA PASS: " + tag + L" đã đứng tại TỌA GD; giữ HOLD chờ acc còn lại.");
            return true;
        }
        return true;
    }

    bool TradeQueueContains(DWORD pid) const {
        return std::find(tradeQueuePids_.begin(), tradeQueuePids_.end(), pid) != tradeQueuePids_.end();
    }

    Account* EarliestQueuedChild() {
        Account* best = nullptr;
        for (DWORD pid : tradeQueuePids_) {
            Account* child = AccountByPid(pid);
            if (!child || child->profile.tradeRole < 2 || child->profile.tradeRole > 7) continue;
            if (!best || itemtrade_coordinator::EarlierWorkflowEntry(
                    child->runtime.tradeWorkflowEntrySeq, child->profile.tradeRole - 1,
                    best->runtime.tradeWorkflowEntrySeq, best->profile.tradeRole - 1)) {
                best = child;
            }
        }
        return best;
    }

    void RelockRendezvousToEarliestQueued(DWORD now) {
        (void)now;
        if (tradeTxn_.phase != TradePhase::Rendezvous) return;
        Account* earliest = EarliestQueuedChild();
        if (!earliest || earliest->game.pid == tradeTxn_.childPid) return;
        Account* old = AccountByPid(tradeTxn_.childPid);
        const int oldSlot = tradeTxn_.childSlot;
        tradeTxn_.childPid = earliest->game.pid;
        tradeTxn_.childSlot = earliest->profile.tradeRole - 1;
        tradeTxn_.sequenceIndex = 0;
        tradeTxn_.sequenceRepeatDone = 0;
        tradeTxn_.sequenceGroupRepeatDone = 0;
        tradeTxn_.sequencePass = 1;
        tradeTxn_.sequenceDueTick = 0;
        if (old) LogAccount(*old, L"FIFO RELOCK: nhường active rendezvous cho CON" +
                                  std::to_wstring(tradeTxn_.childSlot) + L" vì CON này vào workflow trước.");
        LogAccount(*earliest, L"FIFO RELOCK PASS: giữ đúng vé workflow #" +
                              std::to_wstring(earliest->runtime.tradeWorkflowEntrySeq) +
                              L" • active trước CON" + std::to_wstring(oldSlot) + L".");
    }

    void ReleaseTradeHold(Account& a) {
        a.tradeHeld = false;
        a.runtime.tradeWorkflowEntrySeq = 0;
        ResetTradeRendezvousTravel(a);
    }

    void RemoveTradeQueuePid(DWORD pid) {
        tradeQueuePids_.erase(std::remove(tradeQueuePids_.begin(), tradeQueuePids_.end(), pid), tradeQueuePids_.end());
    }

    void ReleaseTradeHolds() {
        for (auto& item : accounts_) {
            Account& a = *item;
            if (!a.tradeHeld) continue;
            ReleaseTradeHold(a);
        }
        tradeQueuePids_.clear();
    }

    void ReleaseTradeWorkflowLock(const std::wstring& reason) {
        // Kept as a workflow hook for call-site stability. Trade ordering is owned by
        // tradeTxn_ itself; there is no global hidden-input lease in v0.6.1.6.
        ReleaseTradeWorkflowLockCore(reason);
    }

    void AbortTrade(const std::wstring& reason, DWORD now) {
        Account* main = AccountByPid(tradeTxn_.mainPid);
        Account* child = AccountByPid(tradeTxn_.childPid);
        if (main) LogAccount(*main, L"GD ABORT: " + reason);
        if (child) LogAccount(*child, L"GD ABORT: " + reason);
        ReleaseTradeWorkflowLock(L"giao dịch abort");
        ReleaseTradeHolds();
        tradeTxn_.phase = TradePhase::Idle;
        tradeTxn_.mainPid = 0;
        tradeTxn_.childPid = 0;
        tradeTxn_.childSlot = 0;
        tradeTxn_.sequenceIndex = 0; tradeTxn_.sequenceRepeatDone = 0; tradeTxn_.sequenceGroupRepeatDone = 0; tradeTxn_.sequencePass = 1; tradeTxn_.sequenceDueTick = 0; tradeTxn_.sequenceMainFreeBeforePass = -1; tradeTxn_.sequenceBagVerifyStartedTick = 0; tradeTxn_.sequenceBagStableSinceTick = 0; tradeTxn_.sequenceBagLastFree = -1;
        tradeTxn_.cooldownUntil = now + 2500;
        SetTradeStatus(L"HỦY • " + reason + L" • nhả hàng đợi + workflow lock");
    }

    void FinishTrade(DWORD now) {
        Account* main = AccountByPid(tradeTxn_.mainPid);
        Account* child = AccountByPid(tradeTxn_.childPid);
        const DWORD finishedChildPid = tradeTxn_.childPid;
        const int finishedSlot = tradeTxn_.childSlot;

        if (main) LogAccount(*main, L"GD xong với CON " + std::to_wstring(finishedSlot) +
                                 L" • pass cuối MAIN nhận ≤8 slot; CON đang xếp hàng vẫn giữ TỌA GD.");
        if (child) LogAccount(*child, L"GD ĐẠT ĐIỀU KIỆN MỚI • pass cuối làm MAIN nhận ≤8 slot"
                                  L" • nhả HOLD để core quay bãi; slot queue giải phóng ngay.");

        ReleaseTradeWorkflowLock(L"chuỗi click giao dịch hoàn tất");
        if (child) ReleaseTradeHold(*child);
        RemoveTradeQueuePid(finishedChildPid);

        // MAIN stays parked only while another queued CON still needs it. When the queue
        // becomes empty, release MAIN exactly like old behavior so normal core resumes.
        if (main && tradeQueuePids_.empty()) ReleaseTradeHold(*main);

        tradeTxn_.phase = TradePhase::Idle;
        tradeTxn_.mainPid = main ? main->game.pid : 0;
        tradeTxn_.childPid = 0;
        tradeTxn_.childSlot = 0;
        tradeTxn_.sequenceIndex = 0; tradeTxn_.sequenceRepeatDone = 0; tradeTxn_.sequenceGroupRepeatDone = 0; tradeTxn_.sequencePass = 1; tradeTxn_.sequenceDueTick = 0; tradeTxn_.sequenceMainFreeBeforePass = -1; tradeTxn_.sequenceBagVerifyStartedTick = 0; tradeTxn_.sequenceBagStableSinceTick = 0; tradeTxn_.sequenceBagLastFree = -1;
        tradeTxn_.cooldownUntil = now + 1500;
        SetTradeStatus(L"HOÀN TẤT CON" + std::to_wstring(finishedSlot) + L" • còn xếp hàng " +
                       std::to_wstring(tradeQueuePids_.size()) + L"/3");
    }

    bool ExecuteTradeSequenceTick(Account& main, Account& child, DWORD now) {
        EnsureSharedChildTradeSequence();
        std::vector<TradeSequenceStep>& seq = childTradeSequence_;
        if (tradeTxn_.sequenceIndex >= seq.size()) {
            // Honor the final configured click delay first. Then wait for MAIN bag space
            // to become stable before applying the R10 <=8-slot heuristic.
            if (tradeTxn_.sequenceDueTick != 0 && static_cast<LONG>(now - tradeTxn_.sequenceDueTick) < 0) return true;
            if (!main.snapshotValid || (main.snapshot.validMask & ValidBagSpace) == 0) {
                SetTradeStatus(L"TRADE WORKFLOW • chờ MAIN FreeBagSpace hợp lệ sau pass GD");
                return true;
            }

            const int observedFree = main.snapshot.freeBagSpace;
            if (tradeTxn_.sequenceBagVerifyStartedTick == 0) {
                tradeTxn_.sequenceBagVerifyStartedTick = now;
                tradeTxn_.sequenceBagStableSinceTick = now;
                tradeTxn_.sequenceBagLastFree = observedFree;
                SetTradeStatus(L"TRADE WORKFLOW • xác minh ổn định túi MAIN sau pass GD");
                return true;
            }
            if (observedFree != tradeTxn_.sequenceBagLastFree) {
                tradeTxn_.sequenceBagLastFree = observedFree;
                tradeTxn_.sequenceBagStableSinceTick = now;
                SetTradeStatus(L"TRADE WORKFLOW • MAIN FreeBagSpace vừa đổi → reset cửa sổ ổn định");
                return true;
            }
            const bool stableEnough = Elapsed(now, tradeTxn_.sequenceBagStableSinceTick, kTradeBagStableMs);
            const bool verifyTimedOut = Elapsed(now, tradeTxn_.sequenceBagVerifyStartedTick, kTradeBagVerifyMaxMs);
            if (!stableEnough && !verifyTimedOut) return true;

            const int beforeFree = tradeTxn_.sequenceMainFreeBeforePass;
            const int afterFree = tradeTxn_.sequenceBagLastFree;
            const int receivedSlots = ReceivedSlots(beforeFree, afterFree);
            if (DecidePass(beforeFree, afterFree) == PassDecision::RepeatSameChild) {
                ++tradeTxn_.sequencePass;
                tradeTxn_.sequenceIndex = 0;
                tradeTxn_.sequenceRepeatDone = 0;
                tradeTxn_.sequenceGroupRepeatDone = 0;
                tradeTxn_.sequenceDueTick = now + 250;
                tradeTxn_.sequenceMainFreeBeforePass = afterFree;
                tradeTxn_.sequenceBagVerifyStartedTick = 0;
                tradeTxn_.sequenceBagStableSinceTick = 0;
                tradeTxn_.sequenceBagLastFree = -1;
                SetTradeStatus(L"TRADE WORKFLOW • CON" + std::to_wstring(tradeTxn_.childSlot) +
                               L" • MAIN nhận " + std::to_wstring(receivedSlots) +
                               L" slot (>8) sau khi túi ổn định • lặp lại toàn chuỗi GD lần " +
                               std::to_wstring(tradeTxn_.sequencePass));
                LogAccount(child, L"GD CÒN ĐỒ • MAIN nhận " + std::to_wstring(receivedSlots) +
                                  L" slot/pass sau ổn định (>8) → GIỮ workflow/FIFO và chạy lại toàn bộ chuỗi GD; pass " +
                                  std::to_wstring(tradeTxn_.sequencePass) + L".");
                return true;
            }
            LogAccount(main, L"GD PASS CUỐI • MAIN nhận " + std::to_wstring(receivedSlots) +
                             L" slot sau khi FreeBagSpace ổn định (≤8) → coi CON hiện tại đã hết đồ, chuyển CON kế tiếp.");
            FinishTrade(now);
            return true;
        }
        TradeSequenceStep& stored = seq[tradeTxn_.sequenceIndex];
        const TradeSequenceStep* effective = &stored;
        Account* target = &child;
        if (stored.target == 1) {
            effective = ResolveMainReference(stored);
            target = &main;
            if (!effective) { AbortTrade(L"MAIN reference hỏng tại bước " + std::to_wstring(tradeTxn_.sequenceIndex + 1), now); return false; }
        }
        if (tradeTxn_.sequenceDueTick != 0 && static_cast<LONG>(now - tradeTxn_.sequenceDueTick) < 0) return true;
        if (stored.kind == 1 && stored.target == 0 && main.snapshot.freeBagSpace <= mainSellThreshold_) {
            AbortTrade(L"MAIN không còn sức chứa an toàn cho bước CHUYỂN ĐỒ", now); return false;
        }
        std::wstring error;
        const std::wstring who = stored.target == 1 ? L"MAIN" : (L"CON" + std::to_wstring(tradeTxn_.childSlot));
        SetTradeStatus(L"HIDDEN REQUEST → " + who + L" • bước " +
                       std::to_wstring(tradeTxn_.sequenceIndex + 1) + L"/" +
                       std::to_wstring(seq.size()) + L" • InputSync");
        if (!CoordinatorInternalPointAction(
                *target, effective->point,
                L"GD ACC CON dùng chung → " + TradeRoleLabel(child.profile.tradeRole) +
                    L" • bước " + std::to_wstring(tradeTxn_.sequenceIndex + 1),
                error)) {
            AbortTrade(L"BĐPT hidden action bước " +
                       std::to_wstring(tradeTxn_.sequenceIndex + 1) +
                       L" FAIL: " + error, now);
            return false;
        }
        ++tradeTxn_.sequenceRepeatDone;
        tradeTxn_.sequenceDueTick = GetTickCount() + static_cast<DWORD>(effective->delayMs);
        int repeatLimit = effective->repeat;
        if (stored.kind == 1 && stored.target == 0) {
            const int capByMain = std::max(0, main.snapshot.freeBagSpace - mainSellThreshold_);
            repeatLimit = std::min(repeatLimit, std::max(1, capByMain));
        }
        if (tradeTxn_.sequenceRepeatDone >= repeatLimit) {
            tradeTxn_.sequenceRepeatDone = 0;
            if (stored.groupId > 0) {
                const std::size_t groupStart = TradeGroupStart(seq, tradeTxn_.sequenceIndex);
                const std::size_t groupEnd = TradeGroupEnd(seq, tradeTxn_.sequenceIndex);
                if (tradeTxn_.sequenceIndex < groupEnd) {
                    ++tradeTxn_.sequenceIndex;
                } else {
                    ++tradeTxn_.sequenceGroupRepeatDone;
                    const int groupLimit = std::max(1, seq[groupStart].groupRepeat);
                    if (tradeTxn_.sequenceGroupRepeatDone < groupLimit) {
                        tradeTxn_.sequenceIndex = groupStart;
                        SetTradeStatus(L"NHÓM G" + std::to_wstring(stored.groupId) + L" • lặp " +
                                       std::to_wstring(tradeTxn_.sequenceGroupRepeatDone + 1) + L"/" + std::to_wstring(groupLimit));
                    } else {
                        tradeTxn_.sequenceIndex = groupEnd + 1;
                        tradeTxn_.sequenceGroupRepeatDone = 0;
                    }
                }
            } else {
                ++tradeTxn_.sequenceIndex;
                tradeTxn_.sequenceGroupRepeatDone = 0;
            }
        }
        return true;
    }

    void TickTradeCoordinator(DWORD now) {
        // v0.6.1.6: SELL on an unrelated PID never stalls the trade coordinator.
        // The active MAIN/CON pair is protected by tradeTxn_/tradeHeld only; SELL keeps
        // its own per-account macro ordering and has no cross-window input ownership.

        if (!tradeEnabled_) {
            if (tradeTxn_.phase != TradePhase::Idle || !tradeQueuePids_.empty())
                AbortTrade(L"DỒN ĐỒ đang TẮT", now);
            SetTradeStatus(L"DỒN ĐỒ TẮT • AUTO TRAIN/BÁN ĐỒ ĐỘC LẬP");
            return;
        }
        if (!tradeRendezvous_.valid) {
            if (!tradeQueuePids_.empty()) AbortTrade(L"mất TỌA GD đã lưu", now);
            SetTradeStatus(L"chưa GET TỌA GD • chọn acc đang đứng điểm GD rồi bấm TỌA GD • LẤY");
            return;
        }

        Account* main = AccountByTradeRole(1);
        if (!main) {
            if (!tradeQueuePids_.empty() || tradeTxn_.phase != TradePhase::Idle) AbortTrade(L"mất MAIN", now);
            SetTradeStatus(L"chưa chọn MAIN");
            return;
        }

        // If an active transaction exists, its MAIN identity must remain stable.
        if (tradeTxn_.phase != TradePhase::Idle && tradeTxn_.mainPid != main->game.pid) {
            AbortTrade(L"MAIN bị đổi giữa workflow", now);
            return;
        }

        // v0.5 World Flow recovery barrier: while traveling/rendezvous (not inside the
        // atomic trade click Sequence), a held account that dies keeps its FIFO/hold slot.
        // P2 revive and the life observer run outside this coordinator; when ALIVE returns,
        // RuntimeState travel phases restart cleanly and World Flow resumes toward TỌA GD.
        auto worldFlowLifeRecovery = [&](const Account* item) {
            if (!item || !item->tradeHeld || tradeTxn_.phase == TradePhase::Sequence) return false;
            const bool deadNow = item->snapshotValid && (item->snapshot.validMask & ValidLifeState) && item->snapshot.dead;
            return deadNow || item->deathSessionLatched || item->runtime.revivePhase != 0;
        };
        if (worldFlowLifeRecovery(main)) {
            SetTradeStatus(L"WORLD FLOW PAUSE • MAIN chết/đang Đầu thai • giữ FIFO/HOLD • LIFE P2 ưu tiên");
            return;
        }
        for (DWORD pid : tradeQueuePids_) {
            Account* queued = AccountByPid(pid);
            if (!worldFlowLifeRecovery(queued)) continue;
            const int slot = queued ? std::max(1, queued->profile.tradeRole - 1) : 0;
            SetTradeStatus(L"WORLD FLOW PAUSE • CON" + std::to_wstring(slot) + L" chết/đang Đầu thai • giữ FIFO/HOLD");
            return;
        }

        const bool mainBaseHealthy = main->runtime.running && main->snapshotValid && IsWindow(main->game.window) &&
            (main->snapshot.validMask & (ValidLifeState | ValidBagSpace | ValidMap | ValidPosition)) ==
                (ValidLifeState | ValidBagSpace | ValidMap | ValidPosition) &&
            !main->snapshot.dead;
        if (!mainBaseHealthy) {
            if (!tradeQueuePids_.empty() || tradeTxn_.phase != TradePhase::Idle)
                AbortTrade(L"MAIN dừng/mất state/chết trong workflow", now);
            else SetTradeStatus(L"MAIN chưa ở state rảnh/an toàn");
            return;
        }
        if (main->snapshot.freeBagSpace <= mainSellThreshold_) {
            if (!tradeQueuePids_.empty() || tradeTxn_.phase != TradePhase::Idle)
                AbortTrade(L"MAIN chạm ngưỡng bán ≤" + std::to_wstring(mainSellThreshold_) + L" ô", now);
            else SetTradeStatus(L"MAIN còn ≤" + std::to_wstring(mainSellThreshold_) + L" ô → ưu tiên BÁN ĐỒ");
            return;
        }
        // Preserve the old scheduler gate when there is no staged workflow yet: do not pull
        // MAIN away from sell/revive/recovery/route-reset work merely because a CON becomes FULL.
        if (tradeQueuePids_.empty() && tradeTxn_.phase == TradePhase::Idle && !main->tradeHeld && !TradeStateReady(*main)) {
            SetTradeStatus(L"MAIN chưa ở state rảnh/an toàn");
            return;
        }

        // Remove waiting children that can no longer participate. The active child remains
        // fail-closed and is handled below so a broken in-flight transaction aborts cleanly.
        for (std::size_t i = 0; i < tradeQueuePids_.size();) {
            const DWORD pid = tradeQueuePids_[i];
            if (pid == tradeTxn_.childPid && tradeTxn_.phase != TradePhase::Idle) { ++i; continue; }
            Account* child = AccountByPid(pid);
            const bool validRole = child && child->profile.tradeRole >= 2 && child->profile.tradeRole <= 7;
            const bool heldPriorityRecovery = child && child->tradeHeld &&
                (child->deathSessionLatched || child->runtime.revivePhase != 0 ||
                 (child->snapshotValid && (child->snapshot.validMask & ValidLifeState) && child->snapshot.dead));
            // A non-active queued traveler may die while another pair owns the atomic trade
            // Sequence. Keep its FIFO/HOLD slot; P2/life recovery runs globally and that CON
            // resumes rendezvous travel after ALIVE instead of being silently dropped.
            if (validRole && heldPriorityRecovery) { ++i; continue; }
            const bool alive = child && child->runtime.running && IsWindow(child->game.window) && child->snapshotValid &&
                ((child->snapshot.validMask & ValidLifeState) == 0 || !child->snapshot.dead);
            if (validRole && alive) { ++i; continue; }
            if (child) {
                LogAccount(*child, L"GD QUEUE: rời hàng đợi vì dừng/chết/đổi role • nhả HOLD cho core xử lý.");
                ReleaseTradeHold(*child);
            }
            tradeQueuePids_.erase(tradeQueuePids_.begin() + static_cast<std::ptrdiff_t>(i));
        }

        std::wstring sequenceReason;
        const bool sequenceReady = TradeSequenceReady(sequenceReason);

        // Fill up to three travel/wait slots. Admission scan is deliberately CON1→CON6, so
        // children that become FULL in the same scheduler tick enter by the smallest slot first.
        // Once staged, vector order is strict FIFO by workflow-entry time and is NEVER re-sorted:
        // e.g. an already-staged CON3 remains ahead of a CON1 that becomes FULL later. FULL is
        // only the entry gate; later bag deltas do not kick a staged child out.
        if (sequenceReady && tradeTxn_.phase != TradePhase::Sequence) {
            for (int slot = 1; slot <= 6 && tradeQueuePids_.size() < kMaxQueuedChildren; ++slot) {
                Account* child = AccountByTradeRole(slot + 1);
                if (!child || TradeQueueContains(child->game.pid)) continue;
                if (!ShouldAdmitFullChild(TradeStateReady(*child), child->snapshot.freeBagSpace,
                                          tradeQueuePids_.size())) continue;

                const bool firstQueued = tradeQueuePids_.empty();
                tradeQueuePids_.push_back(child->game.pid);
                child->runtime.tradeWorkflowEntrySeq = ++tradeWorkflowEntryCounter_;
                child->tradeHeld = true;
                const std::wstring childTag = L"CON" + std::to_wstring(slot);
                BeginTradeRendezvousTravel(*child, now, childTag.c_str());

                if (firstQueued) {
                    // Only the first queued child needs to pull MAIN away from train. Adding
                    // CON2/CON3 must never restart MAIN's rendezvous state.
                    main->tradeHeld = true;
                    BeginTradeRendezvousTravel(*main, now, L"MAIN");
                }

                LogAccount(*child, childTag + L" FULL → vào FIFO workflow vị trí " +
                           std::to_wstring(tradeQueuePids_.size()) + L"/3 • vé #" +
                           std::to_wstring(child->runtime.tradeWorkflowEntrySeq) +
                           L" • đồng thời FULL thì ưu tiên CON số nhỏ; đã có vé rồi thì CON vào sau không được chen.");
            }
        }

        if (!sequenceReady && tradeQueuePids_.empty() && tradeTxn_.phase == TradePhase::Idle) {
            // Only surface the missing-sequence error when a FULL child actually exists.
            for (int slot = 1; slot <= 6; ++slot) {
                Account* child = AccountByTradeRole(slot + 1);
                if (child && TradeStateReady(*child) && child->snapshot.freeBagSpace <= 0) {
                    SetTradeStatus(L"CON" + std::to_wstring(slot) + L" FULL nhưng " + sequenceReason + L" • mở CHUỖI GD ACC CON");
                    return;
                }
            }
        }

        // TRADE WORKFLOW LOCK protects the business ordering of the point-based
        // trade macro. Every dispatch is now a per-client Bridge action, so queued
        // travelers keep progressing without borrowing the Windows mouse.
        if (!tradeQueuePids_.empty()) {
            if (tradeTxn_.phase != TradePhase::Sequence) {
                main->tradeHeld = true;
                (void)HandleTradeRendezvousTravel(*main, now, L"MAIN");
                for (DWORD pid : tradeQueuePids_) {
                    Account* child = AccountByPid(pid);
                    if (!child) continue;
                    child->tradeHeld = true;
                    const int slot = std::max(1, child->profile.tradeRole - 1);
                    const std::wstring tag = L"CON" + std::to_wstring(slot);
                    (void)HandleTradeRendezvousTravel(*child, now, tag.c_str());
                }
            } else {
                for (DWORD pid : tradeQueuePids_) {
                    // The active trade pair must stay parked at TỌA GD for click safety.
                    // Only the other queued CON accounts are allowed to keep AutoPath moving.
                    if (pid == tradeTxn_.childPid) continue;
                    Account* child = AccountByPid(pid);
                    if (!child) continue;
                    child->tradeHeld = true;
                    const int slot = std::max(1, child->profile.tradeRole - 1);
                    const std::wstring tag = L"CON" + std::to_wstring(slot);
                    (void)HandleTradeRendezvousTravel(*child, now, tag.c_str());
                }
            }
        }

        // R7 FIFO invariant: while still in Rendezvous (before any trade click), the
        // active CON must be the account with the oldest immutable workflow-entry ticket.
        // This fixes cases where a later traveler could remain selected while an earlier CON
        // was already parked and waiting at the rendezvous.
        RelockRendezvousToEarliestQueued(now);

        Account* activeMain = tradeTxn_.phase == TradePhase::Idle ? nullptr : AccountByPid(tradeTxn_.mainPid);
        Account* activeChild = tradeTxn_.phase == TradePhase::Idle ? nullptr : AccountByPid(tradeTxn_.childPid);

        if (tradeTxn_.phase == TradePhase::Rendezvous) {
            if (!activeMain || !activeChild || !TradeQueueContains(activeChild->game.pid) ||
                !IsWindow(activeMain->game.window) || !IsWindow(activeChild->game.window)) {
                AbortTrade(L"mất cửa sổ/acc/hàng đợi khi đi TỌA GD", now); return;
            }
            if (!activeMain->runtime.running || !activeChild->runtime.running ||
                !activeMain->snapshotValid || !activeChild->snapshotValid) {
                AbortTrade(L"MAIN/CON dừng hoặc mất snapshot khi đi TỌA GD", now); return;
            }
            const Snapshot& ms = activeMain->snapshot;
            const Snapshot& cs = activeChild->snapshot;
            if (((ms.validMask & ValidLifeState) && ms.dead) || ((cs.validMask & ValidLifeState) && cs.dead)) {
                AbortTrade(L"MAIN/CON chết khi đi TỌA GD • nhả HOLD để core xử lý đầu thai", now); return;
            }

            const bool mainReady = activeMain->runtime.tradeTravelReady && TradeAccountAtRendezvous(*activeMain);
            const bool childReady = activeChild->runtime.tradeTravelReady && TradeAccountAtRendezvous(*activeChild);
            if (activeMain->runtime.autoPathFightConflictLatched ||
                activeChild->runtime.autoPathFightConflictLatched) {
                SetTradeStatus(L"GD TỌA • chờ MAIN/CON hoàn tất ROUTE/FIGHT INVARIANT recovery");
                return;
            }
            if (!mainReady || !childReady) {
                SetTradeStatus(L"QUEUE " + std::to_wstring(tradeQueuePids_.size()) + L"/3 • MAIN " +
                               std::wstring(mainReady ? L"ĐÃ TỚI" : L"ĐANG ĐI") + L" • CON" +
                               std::to_wstring(tradeTxn_.childSlot) + L" " +
                               std::wstring(childReady ? L"ĐÃ TỚI" : L"ĐANG ĐI") +
                               L" • XN Lâu Lan watchdog chỉ chạy khi M5 bị kẹt cổng");
                return;
            }

            if (!sequenceReady) { AbortTrade(L"chuỗi click GD chưa sẵn sàng: " + sequenceReason, now); return; }
            // Travel/queueing owns no sequence lease. The workflow lock begins only
            // when both accounts are parked and the first trade action is about to run.
            if (!AcquireTradeWorkflowLock(*activeMain, L"CHUỖI CLICK GIAO DỊCH MAIN↔CON")) {
                SetTradeStatus(L"ĐÃ TỚI TỌA GD • chờ tradeTxn_ bắt đầu chuỗi giao dịch");
                return;
            }
            tradeTxn_.phase = TradePhase::Sequence;
            tradeTxn_.sequenceIndex = 0;
            tradeTxn_.sequenceRepeatDone = 0;
            tradeTxn_.sequenceGroupRepeatDone = 0;
            tradeTxn_.sequencePass = 1;
            tradeTxn_.sequenceDueTick = 0;
            tradeTxn_.sequenceMainFreeBeforePass = activeMain->snapshot.freeBagSpace;
            tradeTxn_.sequenceBagVerifyStartedTick = 0;
            tradeTxn_.sequenceBagStableSinceTick = 0;
            tradeTxn_.sequenceBagLastFree = -1;
            SetTradeStatus(L"TRADE WORKFLOW • bắt đầu chuỗi GD CON" + std::to_wstring(tradeTxn_.childSlot) +
                           L" • đo slot MAIN nhận/pass; ≤8 thì đổi CON • queued " +
                           std::to_wstring(tradeQueuePids_.size()) + L"/3");
            LogAccount(*activeMain, L"BĐPT: MAIN + CON" + std::to_wstring(tradeTxn_.childSlot) +
                                    L" đã tới TỌA GD → BẮT ĐẦU TRADE WORKFLOW đúng tại chuỗi action.");
            return;
        }

        if (tradeTxn_.phase == TradePhase::Sequence) {
            if (!activeMain || !activeChild || !TradePairReadyForPreparation(*activeMain, *activeChild)) {
                AbortTrade(L"mất acc/state trong chuỗi giao dịch", now); return;
            }
            if (activeMain->runtime.autoPathFightConflictLatched ||
                activeChild->runtime.autoPathFightConflictLatched) {
                SetTradeStatus(L"TRADE WORKFLOW • pause action khi ROUTE/FIGHT INVARIANT đang recovery");
                return;
            }
            if (activeMain->snapshot.autoPathing || activeChild->snapshot.autoPathing) {
                for (Account* a : {activeMain, activeChild}) {
                    if (!a || !a->snapshot.autoPathing || !a->bridge.Attached()) continue;
                    Response r{}; std::wstring ignored;
                    (void)a->bridge.Call(Command::StopPath, 0, 0, 0, r, ignored, 700);
                }
                SetTradeStatus(L"TRADE WORKFLOW • phát hiện AutoPath bật lại tại TỌA GD → StopPath trước action tiếp");
                return;
            }
            if (!TradeAccountAtRendezvous(*activeMain) || !TradeAccountAtRendezvous(*activeChild)) {
                AbortTrade(L"MAIN/CON rời TỌA GD giữa chuỗi • fail-closed để tránh click nhầm", now); return;
            }
            (void)ExecuteTradeSequenceTick(*activeMain, *activeChild, now);
            return;
        }

        if (tradeTxn_.phase != TradePhase::Idle) {
            AbortTrade(L"state giao dịch không hợp lệ", now); return;
        }

        // No click transaction is active. A queue slot may be refilled immediately, even
        // during the existing post-trade cooldown, so the next FULL child can start travel
        // as soon as the previous child has been released to run back to train.
        if (tradeQueuePids_.empty()) {
            if (main->tradeHeld) ReleaseTradeHold(*main);
            SetTradeStatus(L"IDLE • chưa có CON FULL");
            return;
        }

        if (tradeTxn_.cooldownUntil != 0 && static_cast<LONG>(now - tradeTxn_.cooldownUntil) < 0) {
            SetTradeStatus(L"QUEUE " + std::to_wstring(tradeQueuePids_.size()) + L"/3 • chờ cooldown giữa 2 giao dịch");
            return;
        }

        Account* nextChild = EarliestQueuedChild();
        if (!nextChild || nextChild->profile.tradeRole < 2 || nextChild->profile.tradeRole > 7) {
            if (nextChild) {
                const DWORD badPid = nextChild->game.pid;
                ReleaseTradeHold(*nextChild);
                RemoveTradeQueuePid(badPid);
            } else if (!tradeQueuePids_.empty()) {
                tradeQueuePids_.erase(tradeQueuePids_.begin());
            }
            return;
        }

        tradeTxn_.mainPid = main->game.pid;
        tradeTxn_.childPid = nextChild->game.pid;
        tradeTxn_.childSlot = nextChild->profile.tradeRole - 1;
        tradeTxn_.cooldownUntil = 0;
        tradeTxn_.sequenceIndex = 0;
        tradeTxn_.sequenceRepeatDone = 0;
        tradeTxn_.sequenceGroupRepeatDone = 0;
        tradeTxn_.sequencePass = 1;
        tradeTxn_.sequenceDueTick = 0;
        tradeTxn_.phase = TradePhase::Rendezvous;

        SetTradeStatus(L"QUEUE " + std::to_wstring(tradeQueuePids_.size()) + L"/3 • tới lượt CON" +
                       std::to_wstring(tradeTxn_.childSlot) + L" • chờ MAIN+CON cùng tới TỌA GD");
    }

    void ClearEditor() {
        SetText(selected_, L"ACC ĐANG CHỈNH: chưa chọn");
        SetText(live_, L"STATE: chưa có");
        if (tradeRoleCombo_) SendMessageW(tradeRoleCombo_, CB_SETCURSEL, 0, 0);
        SetText(targetName_, L"");
        if (spotCombo_) SendMessageW(spotCombo_, CB_SETCURSEL, -1, 0);
        SetText(targetText_, L"CHƯA CHỌN");
        SetText(tolerance_, L"120");
        SendMessageW(enableRevive_, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(enableConfirm_, BM_SETCHECK, BST_UNCHECKED, 0);
        SetText(rotateDeathLimit_, std::to_wstring(kRotateDeathLimitDefault));
        SetText(rotateDeathWindow_, std::to_wstring(kRotateDeathWindowMinDefault));
        SetText(rotateNoFullBag_, std::to_wstring(kRotateNoFullBagMinDefault));
        if (rotationList_) ListView_DeleteAllItems(rotationList_);
        SendMessageW(enableFight_, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(enableSell_, BM_SETCHECK, BST_UNCHECKED, 0);
        if (sellNpcCombo_) SendMessageW(sellNpcCombo_, CB_SETCURSEL, 0, 0);
        SetText(sellNpcX_, L"");
        SetText(sellNpcY_, L"");
        SetText(sellNpcPosText_, L"CHƯA LẤY");
        for (HWND h : pointLabels_) SetText(h, L"CHƯA LẤY");
        RefreshSellMacroList();
        ClearSellMacroEditor();
        UpdateRoleActionButtons();
    }

    void LoadSelectedProfileToUi() {
        Account* a = SelectedAccount();
        if (!a) { ClearEditor(); return; }
        ResolveProfileTarget(a->profile);
        SetText(selected_, L"ACC ĐANG CHỈNH: " + AccountTag(*a));
        if (tradeRoleCombo_) SendMessageW(tradeRoleCombo_, CB_SETCURSEL, a->profile.tradeRole, 0);
        RefreshSpotCombo();
        RefreshRotationList();
        SetText(targetName_, a->profile.selectedSpot);
        SetText(tolerance_, std::to_wstring(a->profile.tolerance));
        SetText(rotateDeathLimit_, std::to_wstring(a->profile.rotateDeathLimit));
        SetText(rotateDeathWindow_, std::to_wstring(a->profile.rotateDeathWindowMin));
        SetText(rotateNoFullBag_, std::to_wstring(a->profile.rotateNoFullBagMin));
        SendMessageW(enableRevive_, BM_SETCHECK, a->profile.enableRevive ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessageW(enableConfirm_, BM_SETCHECK, a->profile.enableConfirm ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessageW(enableFight_, BM_SETCHECK, a->profile.enableFight ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessageW(enableSell_, BM_SETCHECK, a->profile.enableSell ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessageW(sellNpcCombo_, CB_SETCURSEL, a->profile.sellNpcPreset, 0);
        LoadSellNpcPositionToUi(*a);
        if (a->profile.target.valid) {
            SetText(targetText_, L"M" + std::to_wstring(a->profile.target.mapID) + L" • " +
                                std::to_wstring(a->profile.target.x) + L"," + std::to_wstring(a->profile.target.y));
        } else {
            SetText(targetText_, L"CHƯA CHỌN");
        }
        for (int i = 0; i < 5; ++i) {
            SetText(pointLabels_[static_cast<std::size_t>(i)], PointDescription(a->profile.points[static_cast<std::size_t>(i)]));
        }
        RefreshSellMacroList();
        ClearSellMacroEditor();
        UpdateRoleActionButtons();
        UpdateSelectedLive();
    }

    void PersistSelectedEditor() {
        Account* a = SelectedAccount();
        if (!a) return;
        int tol = _wtoi(GetText(tolerance_).c_str());
        if (tol < 20) tol = 20;
        if (tol > 2000) tol = 2000;
        a->profile.tolerance = tol;
        a->profile.enableRevive = SendMessageW(enableRevive_, BM_GETCHECK, 0, 0) == BST_CHECKED;
        a->profile.enableConfirm = SendMessageW(enableConfirm_, BM_GETCHECK, 0, 0) == BST_CHECKED;
        int deathLimit = _wtoi(GetText(rotateDeathLimit_).c_str());
        if (deathLimit < kRotateDeathLimitMin) deathLimit = kRotateDeathLimitMin;
        if (deathLimit > kRotateDeathLimitMax) deathLimit = kRotateDeathLimitMax;
        int deathWindow = _wtoi(GetText(rotateDeathWindow_).c_str());
        if (deathWindow < kRotateWindowMin) deathWindow = kRotateWindowMin;
        if (deathWindow > kRotateWindowMax) deathWindow = kRotateWindowMax;
        int noBagWindow = _wtoi(GetText(rotateNoFullBag_).c_str());
        if (noBagWindow < kRotateWindowMin) noBagWindow = kRotateWindowMin;
        if (noBagWindow > kRotateWindowMax) noBagWindow = kRotateWindowMax;
        a->profile.rotateDeathLimit = deathLimit;
        a->profile.rotateDeathWindowMin = deathWindow;
        a->profile.rotateNoFullBagMin = noBagWindow;
        SetText(rotateDeathLimit_, std::to_wstring(deathLimit));
        SetText(rotateDeathWindow_, std::to_wstring(deathWindow));
        SetText(rotateNoFullBag_, std::to_wstring(noBagWindow));
        PersistRotationListFromUi(*a);
        a->profile.enableFight = SendMessageW(enableFight_, BM_GETCHECK, 0, 0) == BST_CHECKED;
        a->profile.enableSell = SendMessageW(enableSell_, BM_GETCHECK, 0, 0) == BST_CHECKED;
        PersistSellNpcPositionEditor(*a);
        const LRESULT sellSel = SendMessageW(sellNpcCombo_, CB_GETCURSEL, 0, 0);
        if (sellSel != CB_ERR && sellSel >= 0 && sellSel < static_cast<LRESULT>(kSellNpcs.size())) a->profile.sellNpcPreset = static_cast<int>(sellSel);
        SaveProfile(a->profile);
        const int row = SelectedIndex();
        if (row >= 0) UpdateAccountRow(row, *a);
    }

    void SaveTargetForSelected() {
        Account* a = SelectedAccount();
        if (!a) { Log(L"Chưa chọn acc"); return; }
        PersistSelectedEditor();
        std::wstring error;
        if (!ReadSnapshot(*a, error, 1200)) { LogAccount(*a, L"Không đọc được state để lưu bãi: " + error); return; }
        const Snapshot& s = a->snapshot;
        if (!s.mapReady || s.waitingChangeMap ||
            (s.validMask & (ValidMap | ValidPosition)) != (ValidMap | ValidPosition)) {
            LogAccount(*a, L"State chưa ổn định, không lưu bãi.");
            return;
        }
        std::wstring name = GetText(targetName_);
        if (name.empty()) name = L"Bãi M" + std::to_wstring(s.mapID) + L" " + std::to_wstring(s.x) + L"," + std::to_wstring(s.y);
        TargetProfile spot{name, s.mapID, s.x, s.y, true};
        const int existing = FindSpotIndex(spots_, name);
        if (existing >= 0) spots_[static_cast<std::size_t>(existing)] = spot;
        else spots_.push_back(spot);
        SaveSharedSpots(spots_);
        a->profile.selectedSpot = name;
        a->profile.target = spot;
        a->profile.rotationSpots.clear();
        a->profile.rotationSpots.push_back(name);
        NormalizeRotationProfile(a->profile);
        SaveProfile(a->profile);
        RefreshSpotCombo();
        LoadSelectedProfileToUi();
        for (std::size_t i = 0; i < accounts_.size(); ++i) {
            if (_wcsicmp(accounts_[i]->profile.selectedSpot.c_str(), name.c_str()) == 0) {
                accounts_[i]->profile.target = spot;
                SaveProfile(accounts_[i]->profile);
                UpdateAccountRow(static_cast<int>(i), *accounts_[i]);
            }
        }
        LogAccount(*a, L"Đã lưu/cập nhật bãi CHUNG: " + name + L" • M" + std::to_wstring(s.mapID) + L" • " +
                       std::to_wstring(s.x) + L"," + std::to_wstring(s.y));
    }

    void UpdateTradeRendezvousLabel() {
        if (!tradeRendezvousLabel_) return;
        if (!tradeRendezvous_.valid) {
            SetText(tradeRendezvousLabel_, L"CHƯA LẤY TỌA GD");
            return;
        }
        SetText(tradeRendezvousLabel_, L"M" + std::to_wstring(tradeRendezvous_.mapID) + L" • " +
                                      std::to_wstring(tradeRendezvous_.x) + L"," + std::to_wstring(tradeRendezvous_.y));
    }

    void CaptureTradeRendezvous() {
        Account* source = SelectedAccount();
        if (!source) source = AccountByTradeRole(1);
        if (!source) { Log(L"TỌA GD: hãy chọn một acc hoặc gán MAIN trước."); return; }
        std::wstring error;
        if (!ReadSnapshot(*source, error, 1200)) { LogAccount(*source, L"TỌA GD: không đọc được state: " + error); return; }
        const Snapshot& state = source->snapshot;
        if (!state.mapReady || state.waitingChangeMap ||
            (state.validMask & (ValidMap | ValidPosition)) != (ValidMap | ValidPosition)) {
            LogAccount(*source, L"TỌA GD: state Map/X/Y chưa ổn định, không lưu.");
            return;
        }
        tradeRendezvous_.name = L"TỌA GD";
        tradeRendezvous_.mapID = state.mapID;
        tradeRendezvous_.x = state.x;
        tradeRendezvous_.y = state.y;
        tradeRendezvous_.valid = true;
        WriteIniInt(L"Global", L"TradeRendezvousMap", tradeRendezvous_.mapID);
        WriteIniInt(L"Global", L"TradeRendezvousX", tradeRendezvous_.x);
        WriteIniInt(L"Global", L"TradeRendezvousY", tradeRendezvous_.y);
        WriteIniInt(L"Global", L"TradeRendezvousValid", 1);
        WriteIniInt(L"Global", L"TradeRendezvousTolerance", tradeRendezvousTolerance_);
        FlushIni();
        UpdateTradeRendezvousLabel();
        LogAccount(*source, L"Đã GET TỌA GD = M" + std::to_wstring(tradeRendezvous_.mapID) + L" • " +
                            std::to_wstring(tradeRendezvous_.x) + L"," + std::to_wstring(tradeRendezvous_.y));
    }

    void BeginCapture(ClickSlot slot) {
        Account* a = SelectedAccount();
        if (!a) { Log(L"Chưa chọn acc để lấy tọa độ"); return; }
        captureSlot_ = slot;
        captureMacroIndex_ = -1;
        captureTradeSequenceIndex_ = -1;
        captureTradeSequenceMode_ = 0;
        captureTradeSequenceMainRef_ = -1;
        capturePid_ = a->game.pid;
        const int index = static_cast<int>(slot);
        LogAccount(*a, L"Đang chờ F8 để lấy điểm " + std::wstring(kClickLabels[static_cast<std::size_t>(index)]) + L".");
        SetText(selected_, L"LẤY TỌA ĐỘ CHO " + AccountTag(*a) + L" • đưa chuột vào nút rồi F8");
    }

    void CaptureHotkeyPoint() {
        const bool hasMode = captureSlot_ != ClickSlot::None || captureMacroIndex_ >= 0 ||
                             captureTradeSequenceIndex_ >= 0;
        if (!hasMode || capturePid_ == 0) return;
        Account* captureAccount = AccountByPid(capturePid_);
        if (!captureAccount || !IsWindow(captureAccount->game.window)) {
            Log(L"Lấy tọa độ thất bại: acc/cửa sổ đã mất.");
            captureSlot_ = ClickSlot::None; captureMacroIndex_ = -1; capturePid_ = 0;
            captureTradeSequenceIndex_ = -1; captureTradeSequenceMode_ = 0; captureTradeSequenceMainRef_ = -1;
            return;
        }
        POINT screen{};
        if (!GetCursorPos(&screen)) return;
        POINT client = screen;
        if (!ScreenToClient(captureAccount->game.window, &client)) return;
        RECT rc{};
        if (!GetClientRect(captureAccount->game.window, &rc)) return;
        const int width = rc.right - rc.left;
        const int height = rc.bottom - rc.top;
        if (client.x < 0 || client.y < 0 || client.x >= width || client.y >= height) {
            LogAccount(*captureAccount, L"F8 bỏ qua: con trỏ không nằm trong client game của acc đích.");
            return;
        }
        const ClickPoint captured{client.x, client.y, width, height, true};
        if (captureTradeSequenceIndex_ >= 0) {
            bool saved = false;
            if (captureTradeSequenceMode_ == 1) {
                if (captureTradeSequenceIndex_ < static_cast<int>(mainTradeSequence_.size())) {
                    mainTradeSequence_[static_cast<std::size_t>(captureTradeSequenceIndex_)].point = captured;
                    SaveMainTradeSequence();
                    saved = true;
                }
            } else if (captureTradeSequenceMode_ == 2) {
                if (captureTradeSequenceMainRef_ >= 0) {
                    if (captureTradeSequenceMainRef_ < static_cast<int>(mainTradeSequence_.size())) {
                        mainTradeSequence_[static_cast<std::size_t>(captureTradeSequenceMainRef_)].point = captured;
                        SaveMainTradeSequence();
                        saved = true;
                    }
                } else {
                    EnsureSharedChildTradeSequence();
                    if (captureTradeSequenceIndex_ < static_cast<int>(childTradeSequence_.size())) {
                        childTradeSequence_[static_cast<std::size_t>(captureTradeSequenceIndex_)].point = captured;
                        SaveSharedChildTradeSequence();
                        saved = true;
                    }
                }
            }

            if (!saved) {
                LogAccount(*captureAccount, L"F8 chuỗi GD thất bại: dòng/đích capture đã đổi hoặc không còn tồn tại.");
            } else {
                if (tradeEditor_ && IsWindow(tradeEditor_) && tradeEditorMode_ == captureTradeSequenceMode_) {
                    RefreshTradeSequenceList();
                    if (tradeSeqList_ && captureTradeSequenceIndex_ < ListView_GetItemCount(tradeSeqList_)) {
                        ListView_SetItemState(tradeSeqList_, captureTradeSequenceIndex_,
                                              LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
                        ListView_EnsureVisible(tradeSeqList_, captureTradeSequenceIndex_, FALSE);
                        LoadTradeSequenceRowToEditor(captureTradeSequenceIndex_);
                    }
                }
                LogAccount(*captureAccount, L"Đã lưu chuỗi GD dòng " + std::to_wstring(captureTradeSequenceIndex_ + 1) + L" qua F8 = " + PointDescription(captured));
            }
        } else if (captureMacroIndex_ >= 0) {
            if (captureMacroIndex_ >= static_cast<int>(captureAccount->profile.sellMacro.size())) {
                LogAccount(*captureAccount, L"F8 macro thất bại: dòng đã bị xóa.");
            } else {
                captureAccount->profile.sellMacro[static_cast<std::size_t>(captureMacroIndex_)].point = captured;
                SaveProfile(captureAccount->profile);
                LogAccount(*captureAccount, L"Đã lưu macro dòng " + std::to_wstring(captureMacroIndex_ + 1) + L" = " + PointDescription(captured));
            }
        } else {
            const int index = static_cast<int>(captureSlot_);
            if (index >= 0 && index < 5) {
                captureAccount->profile.points[static_cast<std::size_t>(index)] = captured;
                SaveProfile(captureAccount->profile);
                LogAccount(*captureAccount, L"Đã lưu " + std::wstring(kClickLabels[static_cast<std::size_t>(index)]) + L" = " + PointDescription(captured));
            }
        }
        LoadSelectedProfileToUi();
        captureSlot_ = ClickSlot::None; captureMacroIndex_ = -1; capturePid_ = 0;
        captureTradeSequenceIndex_ = -1; captureTradeSequenceMode_ = 0; captureTradeSequenceMainRef_ = -1;
    }

    bool DispatchInternalPointActionDirect(Account& a, const ClickPoint& savedPoint,
                                           const std::wstring& request,
                                           std::wstring& error) {
        if (a.runtime.clientFreezeActive) {
            error = L"client/map đang FREEZE; hidden action bị chặn";
            return false;
        }
        if (!IsWindow(a.game.window)) {
            error = L"Cửa sổ game không còn tồn tại";
            return false;
        }

        int normalizedX = -1;
        int normalizedY = -1;
        if (!NormalizeClickPointForBridge(a.game, savedPoint,
                                          normalizedX, normalizedY, error)) {
            return false;
        }

        std::wstring attachError;
        if (!EnsureAttach(a, attachError)) {
            error = L"Không attach được Bridge cho hidden action: " + attachError;
            return false;
        }

        Response response{};
        const bool ok = a.bridge.Call(Command::ClickInternalPoint,
                                      normalizedX, normalizedY, 0,
                                      response, error, 2200);
        const DWORD completedAt = GetTickCount();
        if (!ok) {
            if (BridgeLooksUnresponsive(error)) {
                EnterClientFreeze(a, L"Bridge timeout khi chạy hidden point action", completedAt);
            }
            return false;
        }

        LogAccount(a, L"HIDDEN ACTION DISPATCH PASS • " + request +
                      L" • normalized=" + std::to_wstring(normalizedX) + L"," +
                      std::to_wstring(normalizedY) + L" • " + response.detail);
        return true;
    }

    // v0.6.1.6: hidden InputSync actions do not share a Windows-input resource.
    // Business workflows serialize themselves (SELL per account, TRADE per active pair),
    // so there is no global input/owner/sequence lease between unrelated clients.
    bool AcquireTradeWorkflowLock(Account& owner, const std::wstring& reason) {
        (void)owner;
        (void)reason;
        // tradeTxn_ is the lock: only the active MAIN/CON pair advances this sequence.
        return true;
    }

    void ReleaseTradeWorkflowLockCore(const std::wstring& reason) {
        (void)reason;
    }

    bool CoordinatorInternalPointAction(Account& target, const ClickPoint& savedPoint,
                                        const std::wstring& request,
                                        std::wstring& error) {
        if (RecorderBlocksAccount(target)) {
            error = L"acc đang REC cấu hình; hidden action của chính acc này tạm giữ";
            return false;
        }
        return DispatchInternalPointActionDirect(target, savedPoint, request, error);
    }

    bool ClickSlotNow(Account& a, ClickSlot slot, const wchar_t* reason, bool verbose = true) {
        if (a.runtime.running && a.runtime.clientFreezeActive) {
            if (verbose) LogAccount(a, L"CLICK bị chặn: client/map đang FREEZE");
            return false;
        }
        const int index = static_cast<int>(slot);
        if (index < 0 || index >= static_cast<int>(a.profile.points.size())) return false;
        std::wstring error;
        const ClickPoint& savedPoint = a.profile.points[static_cast<std::size_t>(index)];
        if (!CoordinatorInternalPointAction(
                a, savedPoint,
                reason && *reason ? std::wstring(reason) : L"CLICK SLOT",
                error)) {
            if (verbose) LogAccount(a, L"HIDDEN CLICK " +
                                       std::wstring(kClickLabels[static_cast<std::size_t>(index)]) +
                                       L" FAIL: " + error);
            return false;
        }
        if (verbose) {
            std::wstring line = L"HIDDEN CLICK " +
                                std::wstring(kClickLabels[static_cast<std::size_t>(index)]) +
                                L" -> " + PointDescription(savedPoint) +
                                L" • TryClickUI→EndUIDrag • cursor không đổi";
            LogAccount(a, line);
        }
        return true;
    }

    bool QueuePriorityAutoClick(Account& a, ClickSlot slot, PriorityAutoOwner owner,
                                const std::wstring& reason) {
        RuntimeState& rt = a.runtime;
        if (slot != ClickSlot::AutoMenu && slot != ClickSlot::Attack && slot != ClickSlot::StopAuto2) return false;
        if (owner == PriorityAutoOwner::None) return false;
        if (!rt.running || rt.clientFreezeActive || !a.snapshotValid || !IsWindow(a.game.window)) return false;
        if (slot == ClickSlot::Attack &&
            !travel_fight_guard_logic::CanDispatchFightStart(
                (a.snapshot.validMask & ValidAutoPath) != 0,
                a.snapshot.autoPathing != 0)) {
            rt.status = L"PRIORITY #3 AUTO • chặn bật AutoFight vì AutoPath chưa authoritative OFF";
            return false;
        }
        if (rt.priorityAutoCompletedSlot != ClickSlot::None &&
            (rt.priorityAutoCompletedSlot != slot ||
             rt.priorityAutoCompletedOwner != owner)) {
            LogAccount(a, L"PRIORITY #3 AUTO: bỏ result cũ " + std::wstring(kClickLabels[static_cast<std::size_t>(rt.priorityAutoCompletedSlot)]) + L" do workflow đã đổi pha.");
            rt.priorityAutoCompletedSlot = ClickSlot::None;
            rt.priorityAutoCompletedOwner = PriorityAutoOwner::None;
            rt.priorityAutoCompletedOk = false;
            rt.priorityAutoCompletedTick = 0;
        }
        if (rt.priorityAutoRequestSlot == slot &&
            rt.priorityAutoRequestOwner == owner) return true;
        if (rt.priorityAutoRequestSlot != ClickSlot::None || rt.priorityAutoCompletedSlot != ClickSlot::None) return false;
        rt.priorityAutoRequestSlot = slot;
        rt.priorityAutoRequestOwner = owner;
        rt.priorityAutoReason = reason;
        rt.priorityAutoPointPhase = 0;
        rt.priorityAutoPointTick = 0;
        rt.status = L"PRIORITY #3 AUTO INPUTSYNC • đã xếp hàng " +
                    std::wstring(kClickLabels[static_cast<std::size_t>(slot)]);
        return true;
    }

    bool ConsumePriorityAutoResult(Account& a, ClickSlot slot, PriorityAutoOwner owner,
                                   bool& ok, DWORD& clickedAt) {
        RuntimeState& rt = a.runtime;
        if (rt.priorityAutoCompletedSlot != slot ||
            rt.priorityAutoCompletedOwner != owner) return false;
        ok = rt.priorityAutoCompletedOk;
        clickedAt = rt.priorityAutoCompletedTick;
        rt.priorityAutoCompletedSlot = ClickSlot::None;
        rt.priorityAutoCompletedOwner = PriorityAutoOwner::None;
        rt.priorityAutoCompletedOk = false;
        rt.priorityAutoCompletedTick = 0;
        return true;
    }

    bool PriorityAutoClick(Account& a) {
        RuntimeState& rt = a.runtime;
        const ClickSlot requestedSlot = rt.priorityAutoRequestSlot;
        const PriorityAutoOwner requestedOwner = rt.priorityAutoRequestOwner;
        if (requestedSlot == ClickSlot::None) return false;
        if (requestedOwner == PriorityAutoOwner::None) {
            rt.priorityAutoRequestSlot = ClickSlot::None;
            return false;
        }
        if (!rt.running || rt.clientFreezeActive || !a.snapshotValid || !IsWindow(a.game.window)) return false;
        const Snapshot& s = a.snapshot;

        auto complete = [&](bool ok, DWORD completedAt) {
            rt.priorityAutoRequestSlot = ClickSlot::None;
            rt.priorityAutoRequestOwner = PriorityAutoOwner::None;
            rt.priorityAutoCompletedSlot = requestedSlot;
            rt.priorityAutoCompletedOwner = requestedOwner;
            rt.priorityAutoCompletedOk = ok;
            rt.priorityAutoCompletedTick = completedAt;
            rt.priorityAutoReason.clear();
            rt.priorityAutoPointPhase = 0;
            rt.priorityAutoPointTick = 0;
        };
        const bool unsafeFightStart = requestedSlot == ClickSlot::Attack &&
            (!s.mapReady || s.waitingChangeMap ||
             ((s.validMask & ValidLifeState) && s.dead) ||
             !travel_fight_guard_logic::CanDispatchFightStart(
                 (s.validMask & ValidAutoPath) != 0, s.autoPathing != 0));
        const bool staleTrainStart = requestedSlot == ClickSlot::Attack &&
            requestedOwner == PriorityAutoOwner::Train &&
            AutoFightCheckBusy(a, GetTickCount());
        if (unsafeFightStart || staleTrainStart) {
            complete(false, GetTickCount());
            rt.status = L"PRIORITY #3 AUTO • hủy request cũ vì state không còn cho phép bật Fight";
            LogAccount(a, L"PRIORITY #3 AUTO SAFETY: hủy AUTO→ĐÁNH QUÁI trước dispatch vì "
                          L"AutoPath/map/workflow không còn ở state đã xếp hàng; không dùng request cũ.");
            return false;
        }

        if (!s.mapReady || s.waitingChangeMap ||
            ((s.validMask & ValidLifeState) && s.dead)) return false;

        constexpr auto autoStartPlan = internal_ui_click_logic::AutoStartPlan();
        ClickSlot pointSlot = requestedSlot;
        if (requestedSlot == ClickSlot::Attack) {
            if (rt.priorityAutoPointPhase < 0 ||
                rt.priorityAutoPointPhase >= static_cast<int>(autoStartPlan.size())) {
                rt.priorityAutoPointPhase = 0;
                rt.priorityAutoPointTick = 0;
            }
            const auto& step = autoStartPlan[static_cast<std::size_t>(rt.priorityAutoPointPhase)];
            if (step.waitBeforeMs > 0 &&
                !Elapsed(GetTickCount(), rt.priorityAutoPointTick,
                         static_cast<DWORD>(step.waitBeforeMs))) {
                return false;
            }
            pointSlot = step.point == internal_ui_click_logic::AutoStartPoint::AutoMenu
                ? ClickSlot::AutoMenu : ClickSlot::Attack;
        }

        const int pointIndex = static_cast<int>(pointSlot);
        std::wstring error;
        if (pointIndex < 0 || pointIndex >= static_cast<int>(a.profile.points.size())) {
            complete(false, GetTickCount());
            LogAccount(a, L"PRIORITY #3 AUTO INPUTSYNC FAIL: slot điểm không hợp lệ");
            return false;
        }

        int normalizedX = -1;
        int normalizedY = -1;
        if (!NormalizeClickPointForBridge(
                a.game, a.profile.points[static_cast<std::size_t>(pointIndex)],
                normalizedX, normalizedY, error)) {
            complete(false, GetTickCount());
            LogAccount(a, L"PRIORITY #3 AUTO INPUTSYNC FAIL tọa độ " +
                          std::wstring(kClickLabels[static_cast<std::size_t>(pointIndex)]) +
                          L": " + error);
            return false;
        }

        Response response{};
        const bool ok = a.bridge.Call(Command::ClickInternalPoint,
                                      normalizedX, normalizedY, 0,
                                      response, error, 2200);
        const DWORD clickedAt = GetTickCount();

        if (requestedSlot == ClickSlot::Attack && rt.priorityAutoPointPhase == 0 && ok) {
            rt.priorityAutoPointPhase = 1;
            rt.priorityAutoPointTick = clickedAt;
            rt.status = L"P3 AUTO INPUTSYNC • click 1/2 AUTO xong • chờ mở menu";
            LogAccount(a, L"PRIORITY #3 AUTO INPUTSYNC PASS click 1/2: AUTO • " +
                          std::wstring(response.detail));
            return true;
        }

        complete(ok, clickedAt);
        if (ok) {
            const std::wstring phase = requestedSlot == ClickSlot::Attack
                ? L"click 2/2: ĐÁNH QUÁI" :
                  std::wstring(kClickLabels[static_cast<std::size_t>(pointIndex)]);
            LogAccount(a, L"PRIORITY #3 AUTO INPUTSYNC PASS " + phase +
                          L" • TryClickUI→EndUIDrag • không chiếm chuột Windows.");
        } else {
            if (BridgeLooksUnresponsive(error)) EnterClientFreeze(a, L"Bridge timeout khi click AUTO InputSync", clickedAt);
            LogAccount(a, L"PRIORITY #3 AUTO INPUTSYNC FAIL tại " +
                          std::wstring(kClickLabels[static_cast<std::size_t>(pointIndex)]) +
                          L": " + error);
        }
        return ok;
    }

    bool RunPriorityAutoPass(DWORD now, const std::vector<bool>& snapshotReady) {
        (void)now;
        bool clicked = false;
        std::vector<DWORD> visited;
        auto runOne = [&](Account* a) {
            if (!a || !a->runtime.running || RecorderBlocksAccount(*a) || a->runtime.priorityAutoRequestSlot == ClickSlot::None) return;
            const auto it = std::find_if(accounts_.begin(), accounts_.end(), [&](const std::unique_ptr<Account>& x) { return x.get() == a; });
            if (it == accounts_.end()) return;
            const std::size_t index = static_cast<std::size_t>(std::distance(accounts_.begin(), it));
            if (index >= snapshotReady.size() || !snapshotReady[index]) return;
            if (std::find(visited.begin(), visited.end(), a->game.pid) != visited.end()) return;
            visited.push_back(a->game.pid);
            if (PriorityAutoClick(*a)) clicked = true;
        };
        // Legacy helper retained for deterministic diagnostics only; runtime Tick uses per-account priority.
        for (int role = 1; role <= 7; ++role) runOne(AccountByTradeRole(role));
        for (auto& item : accounts_) runOne(item.get());
        return clicked;
    }

    void ResetTravelFightGuard(RuntimeState& rt) {
        rt.travelFightGuardPhase = 0;
        rt.travelFightGuardTick = 0;
        rt.travelFightStopAttempts = 0;
    }

    bool EnsureAutoFightOffForTravel(Account& a, DWORD now, const wchar_t* context) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        const std::wstring where = context ? context : L"di chuyển";

        if ((s.validMask & ValidAutoFight) == 0) {
            rt.status = L"TRAVEL GUARD • chờ AutoFight authoritative trước StartPath tới " + where;
            return false;
        }
        if (travel_fight_guard_logic::CanDispatchMovement(true, s.autoFight != 0)) {
            if (rt.travelFightGuardPhase != 0 || rt.travelFightStopAttempts != 0) {
                LogAccount(a, L"TRAVEL GUARD PASS: AutoFight OFF authoritative → nhả StartPath tới " + where);
            }
            ResetTravelFightGuard(rt);
            return true;
        }

        bool ok = false;
        DWORD clickedAt = 0;
        switch (rt.travelFightGuardPhase) {
            case 0:
                if (QueuePriorityAutoClick(a, ClickSlot::StopAuto2,
                                           PriorityAutoOwner::TravelGuardStop,
                                           L"TRAVEL GUARD: click điểm DỪNG AUTO nội bộ trước " + where)) {
                    rt.travelFightGuardPhase = 1;
                    rt.status = L"TRAVEL GUARD • AutoFight ON → chờ click DỪNG InputSync";
                }
                return false;
            case 1:
                if (!ConsumePriorityAutoResult(a, ClickSlot::StopAuto2,
                                               PriorityAutoOwner::TravelGuardStop,
                                               ok, clickedAt)) return false;
                if (!ok) { rt.travelFightGuardPhase = 0; return false; }
                rt.travelFightGuardPhase = 2;
                rt.travelFightGuardTick = clickedAt;
                ++rt.travelFightStopAttempts;
                rt.status = L"TRAVEL GUARD • đã click DỪNG InputSync lần " +
                            std::to_wstring(rt.travelFightStopAttempts) + L" • verify OFF";
                return false;
            case 2:
                if (!Elapsed(now, rt.travelFightGuardTick, kPriorityAutoVerifyMs)) return false;
                if (travel_fight_guard_logic::NeedsAnotherStopBeforeReset(
                        rt.travelFightStopAttempts)) {
                    rt.travelFightGuardPhase = 0;
                    return false;
                }
                rt.travelFightGuardPhase = 3;
                rt.travelFightStopAttempts = 0;
                LogAccount(a, L"TRAVEL GUARD: click DỪNG 2 lần vẫn ON → chạy AUTO→ĐÁNH QUÁI InputSync reset rồi lặp DỪNG.");
                return false;
            case 3:
                if (QueuePriorityAutoClick(a, ClickSlot::Attack,
                                           PriorityAutoOwner::TravelGuardReset,
                                           L"TRAVEL GUARD RESET: chạy AUTO→ĐÁNH QUÁI InputSync")) {
                    rt.travelFightGuardPhase = 4;
                }
                return false;
            case 4:
                if (!ConsumePriorityAutoResult(a, ClickSlot::Attack,
                                               PriorityAutoOwner::TravelGuardReset,
                                               ok, clickedAt)) return false;
                if (!ok) { rt.travelFightGuardPhase = 3; return false; }
                rt.travelFightGuardPhase = 5;
                rt.travelFightGuardTick = clickedAt;
                return false;
            case 5:
                if (!Elapsed(now, rt.travelFightGuardTick, kPriorityAutoVerifyMs)) return false;
                rt.travelFightGuardPhase = 0;
                rt.travelFightStopAttempts = 0;
                rt.status = L"TRAVEL GUARD • reset AUTO→ĐÁNH QUÁI xong • lặp DỪNG nội bộ";
                return false;
            default:
                ResetTravelFightGuard(rt);
                return false;
        }
    }

    void ResetAutoPathFightConflict(RuntimeState& rt) {
        rt.autoPathFightConflictLatched = false;
        rt.autoPathFightConflictTick = 0;
        rt.autoPathFightConflictStopAttempts = 0;
    }

    bool HandleAutoPathFightInvariant(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        const std::uint32_t need = ValidAutoPath | ValidAutoFight;

        if ((s.validMask & need) != need) {
            if (rt.autoPathFightConflictLatched) {
                rt.status = L"ROUTE/FIGHT INVARIANT • chờ AutoPath + AutoFight authoritative";
                return true;
            }
            return false;
        }
        if ((s.validMask & ValidLifeState) && s.dead) return false;

        const bool conflict = travel_fight_guard_logic::HasAutoPathFightConflict(
            s.autoPathing != 0, s.autoFight != 0);
        if (conflict && !rt.autoPathFightConflictLatched) {
            rt.autoPathFightConflictLatched = true;
            rt.autoPathFightConflictTick = 0;
            rt.autoPathFightConflictStopAttempts = 0;
            ResetTravelFightGuard(rt);
            LogAccount(a, L"ROUTE/FIGHT INVARIANT VIOLATION: phát hiện AutoPath ON + AutoFight ON"
                          L" → StopPath fail-closed, sau đó DỪNG x2/reset cho tới khi cả hai OFF.");
        }

        if (!rt.autoPathFightConflictLatched) return false;

        if (travel_fight_guard_logic::ConflictRecoveryComplete(
                rt.autoPathFightConflictLatched,
                s.autoPathing != 0, s.autoFight != 0)) {
            ResetAutoPathFightConflict(rt);
            ResetTravelFightGuard(rt);
            rt.status = L"ROUTE/FIGHT INVARIANT PASS • AutoPath OFF + AutoFight OFF";
            LogAccount(a, L"ROUTE/FIGHT INVARIANT RECOVERED: cả AutoPath và AutoFight đều OFF"
                          L" → route kế tiếp phải đi lại qua Travel Guard.");
            return true; // one-cycle barrier before any new route decision
        }

        if (s.autoPathing) {
            if (rt.autoPathFightConflictTick == 0 ||
                Elapsed(now, rt.autoPathFightConflictTick,
                        kAutoPathFightConflictRetryMs)) {
                std::wstring attachError;
                Response response{};
                std::wstring error;
                bool ok = EnsureAttach(a, attachError);
                if (!ok) {
                    error = L"không attach được Bridge: " + attachError;
                } else {
                    ok = a.bridge.Call(Command::StopPath, 0, 0, 0,
                                       response, error, 900);
                }
                rt.autoPathFightConflictTick = now;
                ++rt.autoPathFightConflictStopAttempts;
                if (ok) {
                    LogAccount(a, L"ROUTE/FIGHT INVARIANT: đã gửi StopPath nội bộ lần " +
                                  std::to_wstring(rt.autoPathFightConflictStopAttempts) +
                                  L" • chờ AutoPath OFF authoritative.");
                } else {
                    if (BridgeLooksUnresponsive(error)) {
                        EnterClientFreeze(a, L"Bridge timeout khi dập AutoPath/Fight conflict", now);
                    }
                    LogAccount(a, L"ROUTE/FIGHT INVARIANT: StopPath fail-closed lần " +
                                  std::to_wstring(rt.autoPathFightConflictStopAttempts) +
                                  L" • " + error);
                }
            }
            rt.status = L"ROUTE/FIGHT INVARIANT • đang dập AutoPath trước khi tắt AutoFight";
            return true;
        }

        // AutoPath is now OFF. Reuse the proven stop-stop-reset loop and do not
        // release any route until AutoFight is authoritatively OFF as well.
        if (!EnsureAutoFightOffForTravel(a, now, L"khôi phục invariant AutoPath/AutoFight")) {
            rt.status = L"ROUTE/FIGHT INVARIANT • AutoPath OFF • đang DỪNG AutoFight x2/reset";
            return true;
        }
        return true;
    }

    void TestClick(ClickSlot slot) {
        Account* a = SelectedAccount();
        if (!a) { Log(L"TEST: chưa chọn acc"); return; }
        std::wstring attachError;
        if (!EnsureAttach(*a, attachError)) {
            LogAccount(*a, L"TEST nội bộ không attach được Bridge: " + attachError);
            return;
        }
        Command command = Command::None;
        int arg0 = 0;
        int arg1 = 0;
        std::wstring error;
        switch (slot) {
            case ClickSlot::Confirm: command = Command::ConfirmMap; break;
            case ClickSlot::Revive: command = Command::Revive; break;
            case ClickSlot::AutoMenu:
            case ClickSlot::Attack:
            case ClickSlot::StopAuto2: {
                const int index = static_cast<int>(slot);
                if (index < 0 || index >= static_cast<int>(a->profile.points.size()) ||
                    !NormalizeClickPointForBridge(
                        a->game, a->profile.points[static_cast<std::size_t>(index)],
                        arg0, arg1, error)) {
                    LogAccount(*a, L"TEST INPUTSYNC " +
                                   std::wstring(kClickLabels[static_cast<std::size_t>(index)]) +
                                   L" FAIL tọa độ: " + error);
                    return;
                }
                command = Command::ClickInternalPoint;
                break;
            }
            default: break;
        }
        if (command == Command::None) return;
        Response response{};
        const bool ok = a->bridge.Call(command, arg0, arg1, 0, response, error, 2200);
        const int index = static_cast<int>(slot);
        LogAccount(*a, L"TEST NỘI BỘ " + std::wstring(kClickLabels[static_cast<std::size_t>(index)]) +
                       (ok ? L" PASS • không chiếm chuột • " + std::wstring(response.detail)
                           : L" FAIL • " + error));
    }

    void StartChecked() {
        PersistSelectedEditor();
        int started = 0;
        const int count = ListView_GetItemCount(clientList_);
        for (int i = 0; i < count && i < static_cast<int>(accounts_.size()); ++i) {
            if (!ListView_GetCheckState(clientList_, i)) continue;
            Account& a = *accounts_[static_cast<std::size_t>(i)];
            if (!a.profile.target.valid) {
                LogAccount(a, L"Không start: acc chưa chọn bãi chung.");
                continue;
            }
            std::wstring error;
            if (!EnsureAttach(a, error)) {
                LogAccount(a, L"Không start: " + error);
                continue;
            }
            a.deathSessionLatched = false;
            a.rotationDeathTicks.clear();
            a.rotationMetricTick = GetTickCount();
            a.rotationActiveTrainMs = 0;
            a.rotationBagWasFull = false;
            a.runtime.running = true;
            ResetRuntime(a.runtime);
            a.runtime.running = true;
            a.runtime.routeOwnershipResetPending = true;
            a.runtime.status = L"Đang giám sát • chuẩn hóa ownership AutoPath";
            ++started;
            LogAccount(a, L"BẮT ĐẦU • bãi " + a.profile.target.name + L" • M" +
                           std::to_wstring(a.profile.target.mapID) + L" • " +
                           std::to_wstring(a.profile.target.x) + L"," + std::to_wstring(a.profile.target.y) +
                           L" • vòng " + std::to_wstring(a.profile.rotationSpots.size()) + L" bãi • chết quá " +
                           std::to_wstring(a.profile.rotateDeathLimit) + L"/" + std::to_wstring(a.profile.rotateDeathWindowMin) +
                           L" phút • chưa FULL túi " + std::to_wstring(a.profile.rotateNoFullBagMin) + L" phút");
            UpdateAccountRow(i, a);
        }
        if (started == 0) Log(L"Không có acc hợp lệ được start. Hãy tick checkbox và chọn bãi chung cho acc.");
    }

    void StopAccount(Account& a) {
        const bool wasFrozen = a.runtime.clientFreezeActive;
        a.deathSessionLatched = false;
        a.rotationDeathTicks.clear();
        a.rotationMetricTick = 0;
        a.rotationActiveTrainMs = 0;
        a.rotationBagWasFull = false;
        a.runtime.running = false;
        ResetRuntime(a.runtime);
        a.runtime.running = false;
        a.runtime.status = L"Đã dừng";
        if (a.bridge.Attached() && !wasFrozen) {
            Response r{};
            std::wstring ignored;
            (void)a.bridge.Call(Command::StopPath, 0, 0, 0, r, ignored, 700);
        }
        LogAccount(a, L"Đã dừng. Không tự đổi trạng thái ngựa.");
    }

    void StopChecked() {
        int stopped = 0;
        const int count = ListView_GetItemCount(clientList_);
        for (int i = 0; i < count && i < static_cast<int>(accounts_.size()); ++i) {
            if (!ListView_GetCheckState(clientList_, i)) continue;
            StopAccount(*accounts_[static_cast<std::size_t>(i)]);
            UpdateAccountRow(i, *accounts_[static_cast<std::size_t>(i)]);
            ++stopped;
        }
        if (tradeTxn_.phase != TradePhase::Idle) {
            Account* main = AccountByPid(tradeTxn_.mainPid);
            Account* child = AccountByPid(tradeTxn_.childPid);
            if ((main && !main->runtime.running) || (child && !child->runtime.running)) {
                AbortTrade(L"người dùng DỪNG AUTO acc thuộc workflow giao dịch", GetTickCount());
            }
        }
        if (stopped == 0) Log(L"Không có acc nào được tick để dừng.");
    }

    static bool BridgeLooksUnresponsive(const std::wstring& error) {
        return error.find(L"timeout") != std::wstring::npos ||
               error.find(L"Bridge còn bận") != std::wstring::npos ||
               error.find(L"Bridge busy") != std::wstring::npos;
    }

    bool WindowResponsive(const GameClient& game) const {
        if (!game.window || !IsWindow(game.window)) return false;
        DWORD_PTR ignored = 0;
        const LRESULT ok = SendMessageTimeoutW(game.window, WM_NULL, 0, 0,
                                               SMTO_ABORTIFHUNG | SMTO_BLOCK,
                                               kWindowResponsiveProbeMs, &ignored);
        return ok != 0;
    }

    void EnterClientFreeze(Account& a, const wchar_t* reason, DWORD now) {
        RuntimeState& rt = a.runtime;
        const bool first = !rt.clientFreezeActive;
        rt.clientFreezeActive = true;
        if (rt.clientFreezeSinceTick == 0) rt.clientFreezeSinceTick = now;
        rt.clientStableSinceTick = 0;
        rt.candidateCount = 0;
        rt.qualifiedMap = 0;
        rt.stallSinceTick = 0;
        rt.fightPhase = 0;
        if (first) {
            LogAccount(a, L"FREEZE ACTION: " + std::wstring(reason ? reason : L"client/map chưa ổn định"));
        }
    }

    void MarkReadStateFailure(Account& a, const std::wstring& error, DWORD now) {
        RuntimeState& rt = a.runtime;
        EnterClientFreeze(a, L"ReadState/Bridge không phản hồi", now);
        ++rt.readStateFailStreak;
        rt.clientStableSinceTick = 0;
        rt.status = L"CLIENT KHÔNG PHẢN HỒI • FREEZE ACTION";
        if (rt.lastReadFailureLogTick == 0 || now - rt.lastReadFailureLogTick >= kReadFailLogIntervalMs) {
            LogAccount(a, L"ReadState fail x" + std::to_wstring(rt.readStateFailStreak) + L": " + error +
                          L" • FREEZE, không gửi action mới");
            rt.lastReadFailureLogTick = now;
        }
    }

    bool HoldUntilClientStable(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;

        if (!s.mapReady || s.waitingChangeMap) {
            EnterClientFreeze(a, L"game đang chuyển map", now);
            rt.clientStableSinceTick = 0;
            rt.status = L"ĐANG CHUYỂN MAP • FREEZE ACTION";
            return true;
        }

        if (!rt.clientFreezeActive) {
            rt.readStateFailStreak = 0;
            rt.lastReadFailureLogTick = 0;
            return false;
        }

        if (!WindowResponsive(a.game)) {
            rt.clientStableSinceTick = 0;
            rt.status = L"CỬA SỔ GAME CHƯA PHẢN HỒI • FREEZE ACTION";
            return true;
        }

        if (rt.clientStableSinceTick == 0) {
            rt.clientStableSinceTick = now;
            rt.status = L"MAP/CLIENT ĐÃ PHẢN HỒI • chờ ổn định 2.0s";
            return true;
        }
        if (!Elapsed(now, rt.clientStableSinceTick, kClientStableResumeMs)) {
            const DWORD elapsed = now - rt.clientStableSinceTick;
            const DWORD remainMs = elapsed >= kClientStableResumeMs ? 0 : kClientStableResumeMs - elapsed;
            rt.status = L"CLIENT ĐANG ỔN ĐỊNH • chờ " + std::to_wstring((remainMs + 99) / 100) + L"00ms";
            return true;
        }

        rt.clientFreezeActive = false;
        rt.clientFreezeSinceTick = 0;
        rt.clientStableSinceTick = 0;
        rt.readStateFailStreak = 0;
        rt.lastReadFailureLogTick = 0;
        rt.lastActionTick = 0;
        rt.lastAction = Action::Wait;
        LogAccount(a, L"CLIENT ỔN ĐỊNH LIÊN TỤC 2s → mở khóa action, tiếp tục auto.");
        rt.status = L"Client ổn định 2s • tiếp tục auto";
        return false;
    }

    bool CooldownReady(RuntimeState& rt, Action a, DWORD now) {
        DWORD delay = 1500;
        if (a == Action::Mount || a == Action::Dismount) delay = 4000;
        if (a == Action::StartPath) delay = 5000;
        if (a != rt.lastAction) {
            rt.lastAction = a;
            rt.lastActionTick = 0;
        }
        return rt.lastActionTick == 0 || now - rt.lastActionTick >= delay;
    }

    bool SendDecision(Account& a, Action action, const TargetProfile& t, const wchar_t* context) {
        RuntimeState& rt = a.runtime;
        if (rt.clientFreezeActive) {
            rt.status = L"FREEZE ACTION • bỏ qua route/mount command";
            return false;
        }
        const DWORD now = GetTickCount();
        // Single authoritative movement gate: neither Mount nor StartPath may be
        // emitted while AutoFight is ON/unreadable. StartPath additionally waits for
        // the hard AutoPath+Fight conflict recovery to observe both states OFF.
        if (action == Action::Mount || action == Action::StartPath) {
            if (action == Action::StartPath && rt.autoPathFightConflictLatched) {
                rt.status = L"ROUTE/FIGHT INVARIANT • cấm StartPath khi recovery chưa hoàn tất";
                return false;
            }
            const wchar_t* movementContext = context ? context :
                (action == Action::Mount ? L"lên ngựa" : L"AutoPath");
            if (!EnsureAutoFightOffForTravel(a, now, movementContext)) return false;
        }
        if (!CooldownReady(rt, action, now)) return false;
        Response r{};
        std::wstring error;
        bool ok = false;
        const std::wstring where = context ? context : L"đích";
        switch (action) {
            case Action::Mount:
                ok = a.bridge.Call(Command::ToggleRide, 1, 0, 0, r, error, 1000);
                rt.status = L"Đang lên ngựa • " + where;
                break;
            case Action::Dismount:
                ok = a.bridge.Call(Command::ToggleRide, 0, 0, 0, r, error, 1000);
                rt.status = L"Tới " + where + L" • xuống ngựa";
                break;
            case Action::StartPath:
                ok = a.bridge.Call(Command::StartPath, t.mapID, t.x, t.y, r, error, 1300);
                rt.status = L"Đang AutoPath tới " + where;
                break;
            case Action::StopPath:
                ok = a.bridge.Call(Command::StopPath, 0, 0, 0, r, error, 900);
                rt.status = L"Tới " + where + L" • StopPath";
                break;
            default:
                return false;
        }
        rt.lastActionTick = now;
        if (!ok && BridgeLooksUnresponsive(error)) {
            EnterClientFreeze(a, L"Bridge action timeout/busy", now);
        }
        if (ok && action == Action::StartPath && t.mapID != a.snapshot.mapID) {
            // Arm cross-map confirmation from the command itself. Movement/autoPath
            // evidence is still required before any Confirm click is allowed.
            if (!rt.crossMapRouteArmed) rt.crossMapRouteMoved = false;
            rt.crossMapRouteArmed = true;
        }
        if (!ok) LogAccount(a, L"Route action fail-closed: " + error);
        return ok;
    }

    void ObserveMovement(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        if (rt.lastObservedMap != s.mapID) {
            const bool keepToolOwnedCrossMapRoute = rt.crossMapRouteArmed;
            rt.lastObservedMap = s.mapID;
            rt.lastObservedX = s.x;
            rt.lastObservedY = s.y;
            rt.lastMovementTick = now;
            // v0.5: a World Flow / robust travel route is still the SAME tool-owned route
            // after crossing an intermediate map. Do not lose the Lâu Lan gate watchdog
            // merely because MapID changed. Crossing a map is itself proof the route moved.
            if (keepToolOwnedCrossMapRoute) {
                rt.crossMapRouteMoved = true;
                if (s.autoPathing) rt.crossMapSeenAutoPath = true;
            } else {
                rt.crossMapSeenAutoPath = false;
                rt.crossMapRouteMoved = false;
            }
            rt.stallSinceTick = 0;
            rt.confirmAttempts = 0;
            rt.lastLauLanConfirmTick = 0;
            rt.fightPhase = 0;
            rt.fightAttempts = 0;
            rt.wasAtTarget = false;
            return;
        }
        if (rt.crossMapRouteArmed && s.autoPathing) rt.crossMapSeenAutoPath = true;
        const long long dx = static_cast<long long>(s.x) - rt.lastObservedX;
        const long long dy = static_cast<long long>(s.y) - rt.lastObservedY;
        if (dx * dx + dy * dy >= 25) {
            if (rt.crossMapRouteArmed) rt.crossMapRouteMoved = true;
            rt.lastMovementTick = now;
            rt.lastObservedX = s.x;
            rt.lastObservedY = s.y;
            rt.stallSinceTick = 0;
        }
    }

    void ResetRotationWindow(Account& a, DWORD now) {
        a.rotationDeathTicks.clear();
        a.rotationMetricTick = now;
        a.rotationActiveTrainMs = 0;
        a.rotationBagWasFull = false;
    }

    bool SwitchToNextRotationSpot(Account& a, DWORD now, const std::wstring& reason) {
        NormalizeRotationProfile(a.profile);
        const std::size_t count = a.profile.rotationSpots.size();
        if (count <= 1) {
            ResetRotationWindow(a, now);
            LogAccount(a, L"XOAY BÃI bỏ qua: chỉ tick 1 bãi • " + reason);
            SaveProfile(a.profile);
            return false;
        }
        std::size_t current = 0;
        for (std::size_t i = 0; i < count; ++i) {
            if (_wcsicmp(a.profile.rotationSpots[i].c_str(), a.profile.selectedSpot.c_str()) == 0) {
                current = i;
                break;
            }
        }
        const std::size_t next = NextRotationIndex(current, count);
        const std::wstring oldName = a.profile.selectedSpot;
        const std::wstring nextName = a.profile.rotationSpots[next];
        const int spotIndex = FindSpotIndex(spots_, nextName);
        if (spotIndex < 0) {
            a.profile.rotationSpots.erase(a.profile.rotationSpots.begin() + static_cast<std::ptrdiff_t>(next));
            NormalizeRotationProfile(a.profile);
            ResetRotationWindow(a, now);
            SaveProfile(a.profile);
            LogAccount(a, L"XOAY BÃI: bãi kế tiếp không còn trong data, đã loại khỏi vòng: " + nextName);
            return false;
        }
        a.profile.selectedSpot = nextName;
        a.profile.target = spots_[static_cast<std::size_t>(spotIndex)];
        a.profile.target.valid = true;
        ResetRotationWindow(a, now);
        SaveProfile(a.profile);
        if (SelectedAccount() == &a) LoadSelectedProfileToUi();
        LogAccount(a, L"XOAY BÃI: " + oldName + L" → " + nextName + L" • " + reason +
                      L" • M" + std::to_wstring(a.profile.target.mapID) + L" " +
                      std::to_wstring(a.profile.target.x) + L"," + std::to_wstring(a.profile.target.y));
        return true;
    }

    void RecordDeathForRotation(Account& a, DWORD now) {
        NormalizeRotationProfile(a.profile);
        if (a.profile.rotationSpots.size() <= 1) {
            ResetRotationWindow(a, now);
            return;
        }
        const DWORD windowMs = static_cast<DWORD>(a.profile.rotateDeathWindowMin) * 60u * 1000u;
        a.rotationDeathTicks.push_back(now);
        a.rotationDeathTicks.erase(std::remove_if(a.rotationDeathTicks.begin(), a.rotationDeathTicks.end(), [&](DWORD t){
            return static_cast<DWORD>(now - t) > windowMs;
        }), a.rotationDeathTicks.end());
        const std::size_t count = a.rotationDeathTicks.size();
        LogAccount(a, L"XOAY BÃI death-window: " + std::to_wstring(count) + L" chết / " +
                      std::to_wstring(a.profile.rotateDeathWindowMin) + L" phút");
        if (DeathLimitExceeded(count, a.profile.rotateDeathLimit)) {
            const std::wstring reason = L"chết quá " + std::to_wstring(a.profile.rotateDeathLimit) + L" lần / " +
                                        std::to_wstring(a.profile.rotateDeathWindowMin) + L" phút";
            (void)SwitchToNextRotationSpot(a, now, reason);
        }
    }

    bool UpdateRotationEfficiency(Account& a, DWORD now) {
        NormalizeRotationProfile(a.profile);
        if (a.profile.rotationSpots.size() <= 1) {
            if (a.rotationActiveTrainMs != 0 || !a.rotationDeathTicks.empty()) ResetRotationWindow(a, now);
            return false;
        }
        const Snapshot& s = a.snapshot;
        if (a.rotationMetricTick == 0) a.rotationMetricTick = now;
        DWORD delta = now - a.rotationMetricTick;
        a.rotationMetricTick = now;
        if (delta > 2000) delta = 2000;

        if (s.validMask & ValidBagSpace) {
            const bool full = s.freeBagSpace <= 0;
            if (full && !a.rotationBagWasFull) {
                a.rotationBagWasFull = true;
                a.rotationActiveTrainMs = 0;
                LogAccount(a, L"XOAY BÃI: ghi nhận 1 lần FULL túi → reset đồng hồ hiệu quả bãi.");
            } else if (!full) {
                a.rotationBagWasFull = false;
            }
        }

        bool activelyTraining = false;
        if (a.profile.target.valid &&
            (s.validMask & (ValidMap | ValidPosition | ValidAutoFight | ValidLifeState | ValidBagSpace)) ==
                (ValidMap | ValidPosition | ValidAutoFight | ValidLifeState | ValidBagSpace) &&
            !s.dead && s.autoFight) {
            State state{};
            state.valid = true; state.mapReady = true; state.waitingMap = false;
            state.mapID = s.mapID; state.x = s.x; state.y = s.y;
            Target target{a.profile.target.mapID, a.profile.target.x, a.profile.target.y, a.profile.tolerance};
            activelyTraining = AtTarget(state, target);
        }
        if (activelyTraining) a.rotationActiveTrainMs += delta;

        if (!NoFullBagWindowReached(a.rotationActiveTrainMs, a.profile.rotateNoFullBagMin)) return false;
        const std::wstring reason = L"train thực " + std::to_wstring(a.profile.rotateNoFullBagMin) + L" phút chưa FULL túi";
        if (!SwitchToNextRotationSpot(a, now, reason)) return false;
        BeginTrainRecovery(a, now);
        return true;
    }

    void ResetRuntimeForLifeBoundary(Account& a) {
        // World Flow/FIFO ownership lives partly outside RuntimeState (tradeHeld + queue).
        // Preserve only the immutable FIFO ticket across death/alive hard resets; all travel
        // phases restart cleanly so the same held account can AutoPath to TỌA GD again.
        const std::uint64_t workflowTicket = a.runtime.tradeWorkflowEntrySeq;
        const bool preserveWorkflowTicket = a.tradeHeld && workflowTicket != 0;
        ResetRuntime(a.runtime);
        if (preserveWorkflowTicket) a.runtime.tradeWorkflowEntrySeq = workflowTicket;
    }

    bool HandleDeath(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;

        // Life state is authoritative for the death-session boundary. If it becomes
        // temporarily unavailable while a death session is latched, fail closed and
        // preserve the latch/timers instead of silently returning to normal automation.
        if ((s.validMask & ValidLifeState) == 0) {
            if (a.deathSessionLatched) {
                rt.status = L"DEATH SESSION • chờ life-state authoritative";
                return true;
            }
            return false;
        }

        if (!s.dead) {
            if (!a.deathSessionLatched) return false;
            a.rotationMetricTick = now;

            // SECOND boundary reset: the character is alive again on a stable client
            // snapshot. Wipe every revive/travel/fight/sell/confirm/watchdog phase and
            // resume exactly like a fresh BẮT ĐẦU, while AccountProfile/settings and
            // the existing Bridge attachment remain intact.
            ResetRuntimeForLifeBoundary(a);
            a.deathSessionLatched = false;
            rt.routeOwnershipResetPending = true;
            rt.status = L"ALIVE • cold restart + chuẩn hóa ownership AutoPath";
            LogAccount(a, L"POST-REVIVE COLD START: ResetRuntime toàn bộ • giữ nguyên setting/bãi/click • phiên auto mới.");
            return true;
        }

        if (!a.deathSessionLatched) {
            a.rotationMetricTick = now;
            RecordDeathForRotation(a, now);
            // FIRST boundary reset: a new authoritative death is a hard session
            // boundary. Never carry ANY runtime state from the previous life. The
            // lifecycle latch is outside RuntimeState so this full reset cannot cause
            // a repeated-reset loop while the same dead snapshot remains true.
            ResetRuntimeForLifeBoundary(a);
            a.deathSessionLatched = true;
            rt.deadSinceTick = now;
            rt.status = L"DEAD • hard reset runtime đời trước";
            LogAccount(a, L"NEW DEATH SESSION: HARD ResetRuntime toàn bộ • coi như AUTO vừa được bật lại từ đầu.");
        }

        rt.status = L"Nhân vật đang chết";
        if (!a.profile.enableRevive) {
            rt.status = L"CHẾT • chờ Đầu thai thủ công";
            return true;
        }
        if (rt.revivePhase == 0 && Elapsed(now, rt.deadSinceTick, 500) &&
            (rt.lastReviveClickTick == 0 || Elapsed(now, rt.lastReviveClickTick, 5000))) {
            // The Revive callback is emitted by the per-account P2 priority pass. Keep this
            // path fail-closed for the same client without blocking unrelated windows.
            rt.status = L"ĐẦU THAI đến hạn • chờ P2 cục bộ của chính acc";
            return true;
        }
        if (rt.revivePhase == 1 && Elapsed(now, rt.revivePhaseTick, 900)) {
            // Map Confirm is not injected as a special revive action; global P1 Lâu Lan
            // watchdog owns its own internal MessageBox callback.
            rt.revivePhase = 2;
            rt.revivePhaseTick = now;
            rt.status = L"Đầu thai đã gửi • chờ sống lại; World Flow vẫn HOLD và sẽ resume";
            return true;
        }
        if (rt.revivePhase == 2 && Elapsed(now, rt.revivePhaseTick, 4500)) {
            rt.revivePhase = 0;
            rt.revivePhaseTick = now;
        }
        return true;
    }

    bool HandleRouteOwnershipReset(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        if (!rt.routeOwnershipResetPending) return false;

        // This is the missing game-side half of a true cold start. ResetRuntime()
        // clears controller ownership flags, but the client may preserve AutoPath=ON
        // across death/revive. If we accepted that stale path as our route, then
        // crossMapRouteArmed would stay false forever and Confirm would fail closed.
        if ((s.validMask & ValidAutoPath) == 0) {
            rt.status = L"SESSION ROUTE RESET • chờ AutoPath authoritative";
            return true;
        }

        if (!s.autoPathing) {
            rt.routeOwnershipResetPending = false;
            rt.routeOwnershipStopTick = 0;
            rt.routeOwnershipStopAttempts = 0;
            rt.crossMapRouteArmed = false;
            rt.crossMapRouteMoved = false;
            rt.crossMapSeenAutoPath = false;
            rt.confirmAttempts = 0;
            rt.suppressRouteSinceTick = 0;
            rt.status = L"SESSION ROUTE RESET • AutoPath OFF • ownership sạch";
            if (!rt.routeOwnershipResetLogged) {
                LogAccount(a, L"SESSION ROUTE RESET PASS: AutoPath OFF → route kế tiếp phải do tool StartPath mới để arm Confirm.");
                rt.routeOwnershipResetLogged = true;
            }
            return true; // one-cycle barrier before normal route logic
        }

        rt.routeOwnershipResetLogged = false;
        if (rt.routeOwnershipStopAttempts >= kRouteOwnershipStopMaxAttempts &&
            rt.routeOwnershipStopTick != 0 &&
            Elapsed(now, rt.routeOwnershipStopTick, kRouteOwnershipStopRetryMs)) {
            rt.status = L"SESSION ROUTE RESET • AutoPath cũ vẫn ON sau 3 StopPath • fail-closed";
            return true;
        }

        if (rt.routeOwnershipStopTick == 0 || Elapsed(now, rt.routeOwnershipStopTick, kRouteOwnershipStopRetryMs)) {
            if (SendDecision(a, Action::StopPath, a.profile.target, L"session route ownership reset")) {
                ++rt.routeOwnershipStopAttempts;
                rt.routeOwnershipStopTick = now;
                rt.status = L"SESSION ROUTE RESET • phát hiện AutoPath cũ ON → StopPath, chờ verify OFF";
                LogAccount(a, L"SESSION ROUTE RESET: AutoPath=ON nhưng controller vừa cold-reset → StopPath để xóa path đời trước trước khi route mới.");
            } else {
                rt.status = L"SESSION ROUTE RESET • chờ gửi StopPath fail-closed";
            }
        } else {
            rt.status = L"SESSION ROUTE RESET • đã StopPath → chờ snapshot AutoPath OFF";
        }
        return true;
    }

    bool HandleUnderworldAutoFightGuard(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        if (s.mapID != kUnderworldMapId) {
            rt.underworldGuardLogged = false;
            return false;
        }
        if ((s.validMask & ValidAutoFight) == 0) {
            rt.status = L"ĐỊA PHỦ M87 • chờ AutoFight authoritative • Travel Guard fail-closed";
            return true;
        }
        if (!s.autoFight) {
            if (!rt.underworldGuardLogged) {
                LogAccount(a, L"ĐỊA PHỦ M87: dùng Travel Guard chung • AutoFight OFF → route được phép tiếp tục.");
                rt.underworldGuardLogged = true;
            }
            ResetTravelFightGuard(rt);
            return false;
        }
        rt.underworldGuardLogged = false;
        if (!EnsureAutoFightOffForTravel(a, now, L"rời Địa Phủ M87")) {
            rt.status = L"ĐỊA PHỦ M87 • Travel Guard đang tắt AutoFight • CẤM route khi còn ON";
            return true;
        }
        return false;
    }

    bool HandleFightClicks(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        if (!a.profile.enableFight) {
            rt.fightPhase = 0;
            rt.fightAttempts = 0;
            return false;
        }
        if ((s.validMask & ValidAutoFight) == 0) {
            rt.status = L"Đúng bãi • chờ đọc trạng thái AutoFight";
            return true;
        }
        if (s.autoFight) {
            rt.fightPhase = 3;
            rt.fightAttempts = 0;
            if (!rt.trainPositionMonitorArmed) {
                rt.trainPositionMonitorArmed = true;
                rt.lastTrainPositionCheckTick = now;
                LogAccount(a, L"AutoFight ON • bắt đầu check tọa độ train 3 phút/lần.");
            }
            rt.lastAutoFightCheckTick = now;
            rt.status = L"Đúng bãi • AutoFight ON • check Auto mỗi 1 phút";
            return true;
        }
        if (rt.fightAttempts >= 2) {
            rt.status = L"P3 AUTO→Đánh quái thử 2 lần • chờ chu kỳ check Auto tiếp theo";
            rt.fightPhase = 3;
            rt.lastAutoFightCheckTick = now;
            return true;
        }
        if (rt.fightPhase == 3) rt.fightPhase = 0;

        bool ok = false;
        DWORD clickedAt = 0;
        if (rt.fightPhase == 0) {
            if (ConsumePriorityAutoResult(a, ClickSlot::Attack,
                                          PriorityAutoOwner::Train,
                                          ok, clickedAt)) {
                ++rt.fightAttempts;
                if (ok) {
                    rt.fightPhase = 2;
                    rt.fightPhaseTick = clickedAt;
                    rt.status = L"P3 AUTO INPUTSYNC • đủ 2 click • verify AutoFight";
                } else {
                    rt.status = L"P3 AUTO INPUTSYNC • sequence fail lần " +
                                std::to_wstring(rt.fightAttempts) + L"/2";
                }
                return true;
            }
            (void)QueuePriorityAutoClick(a, ClickSlot::Attack,
                                         PriorityAutoOwner::Train,
                                         L"TRAIN: InputSync AUTO→ĐÁNH QUÁI");
            rt.status = L"P3 AUTO INPUTSYNC • chờ Priority #3 chạy click 1→2";
            return true;
        }
        if (rt.fightPhase == 2 && Elapsed(now, rt.fightPhaseTick, 1500)) {
            if (s.autoFight) {
                rt.fightPhase = 3;
                rt.fightAttempts = 0;
                rt.lastAutoFightCheckTick = now;
                if (!rt.trainPositionMonitorArmed) {
                    rt.trainPositionMonitorArmed = true;
                    rt.lastTrainPositionCheckTick = now;
                }
                rt.status = L"AutoFight ON • P3 InputSync bật thành công";
                LogAccount(a, L"PRIORITY #3 AUTO→ĐÁNH QUÁI InputSync verify AutoFight ON.");
                return true;
            }
            if (rt.fightAttempts < 2) {
                rt.fightPhase = 0;
                rt.fightPhaseTick = now;
                return true;
            }
        }
        return true;
    }

    bool LauLanGateConfirmDue(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        if (!a.profile.enableConfirm) return false;
        if (!a.snapshotValid || rt.clientFreezeActive || globalPaused_ || RecorderBlocksAccount(a)) return false;
        if ((s.validMask & (ValidMap | ValidPosition | ValidAutoPath | ValidLifeState)) !=
            (ValidMap | ValidPosition | ValidAutoPath | ValidLifeState)) return false;
        if (!s.mapReady || s.waitingChangeMap || s.dead) return false;

        // Lâu Lan watchdog is completely dormant outside Map 5.
        if (s.mapID != kLauLanMapId) {
            rt.stallSinceTick = 0;
            rt.confirmAttempts = 0;
            rt.lastLauLanConfirmTick = 0;
            return false;
        }

        // Only a tool-owned cross-map route that actually moved can arm the gate watchdog.
        // Standing still in Lâu Lan without a live route can never cause a blind XN click.
        if (!rt.crossMapRouteArmed || !rt.crossMapRouteMoved || !rt.crossMapSeenAutoPath) return false;
        // The gate condition is specifically: AutoPath is still ON but position has stalled.
        // World Flow / SELL / GD UI sub-state must not suppress this P1 observer.
        if (!s.autoPathing) return false;
        if (rt.lastMovementTick == 0 || !Elapsed(now, rt.lastMovementTick, kLauLanGateStallMs)) {
            rt.stallSinceTick = 0;
            return false;
        }
        if (rt.stallSinceTick == 0) rt.stallSinceTick = rt.lastMovementTick;
        if (rt.lastLauLanConfirmTick != 0 && !Elapsed(now, rt.lastLauLanConfirmTick, kLauLanConfirmRetryMs)) return false;
        return true;
    }

    bool PriorityLauLanGateConfirmClick(Account& a, DWORD now) {
        if (!LauLanGateConfirmDue(a, now)) return false;
        std::wstring error;
        Response response{};
        const bool ok = a.bridge.Call(Command::ConfirmMap, 0, 0, 0, response, error, 2200);
        const DWORD clickedAt = GetTickCount();

        a.runtime.lastLauLanConfirmTick = clickedAt;
        if (ok) {
            ++a.runtime.confirmAttempts;
            a.runtime.lastMovementTick = clickedAt; // require a fresh full 3s stall before retry
            a.runtime.stallSinceTick = clickedAt;
            a.runtime.status = L"LÂU LAN M5 • AutoPath đứng ~3s → CALLBACK XN NỘI BỘ • lần " +
                               std::to_wstring(a.runtime.confirmAttempts);
            LogAccount(a, L"LÂU LAN GATE WATCHDOG P1: tìm MessageBox + gọi callback nút đồng ý nội bộ • KHÔNG foreground/chuột • lần " +
                          std::to_wstring(a.runtime.confirmAttempts));
            return true;
        }
        if (BridgeLooksUnresponsive(error)) EnterClientFreeze(a, L"Bridge timeout khi XN map nội bộ", clickedAt);
        LogAccount(a, L"LÂU LAN GATE XN NỘI BỘ FAIL: " + error);
        return false;
    }

    bool RunPriorityLauLanGateConfirmPass(DWORD now, const std::vector<bool>& snapshotReady) {
        bool clicked = false;
        for (std::size_t i = 0; i < accounts_.size(); ++i) {
            Account& a = *accounts_[i];
            if (!a.runtime.running || i >= snapshotReady.size() || !snapshotReady[i]) continue;
            if (PriorityLauLanGateConfirmClick(a, now)) clicked = true;
        }
        return clicked;
    }

    bool PrimeDeathSessionForPriorityRevive(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        if (!a.runtime.running || !a.snapshotValid || !IsWindow(a.game.window) || rt.clientFreezeActive) return false;
        if (!s.mapReady || s.waitingChangeMap) return false;
        if ((s.validMask & ValidLifeState) == 0 || !s.dead) return false;
        if (a.deathSessionLatched) return true;

        // Keep the existing FIRST-death boundary in the priority pre-pass so a click
        // sequence in another window cannot postpone detecting this account's death.
        // HandleDeath() sees deathSessionLatched and therefore does not reset twice.
        a.rotationMetricTick = now;
        RecordDeathForRotation(a, now);
        ResetRuntimeForLifeBoundary(a);
        a.deathSessionLatched = true;
        rt.deadSinceTick = now;
        rt.status = L"DEAD • hard reset runtime đời trước • chờ ĐẦU THAI P2";
        LogAccount(a, L"NEW DEATH SESSION: HARD ResetRuntime toàn bộ • P2 ĐẦU THAI đã nhận death trước auto click thường.");
        return true;
    }

    bool PriorityReviveDue(Account& a, DWORD now) {
        if (!PrimeDeathSessionForPriorityRevive(a, now)) return false;
        const RuntimeState& rt = a.runtime;
        if (!a.profile.enableRevive) return false;
        if (rt.revivePhase != 0 || rt.deadSinceTick == 0) return false;
        if (!Elapsed(now, rt.deadSinceTick, 500)) return false;
        return rt.lastReviveClickTick == 0 || Elapsed(now, rt.lastReviveClickTick, 5000);
    }

    bool PriorityReviveClick(Account& a, DWORD now) {
        if (!PriorityReviveDue(a, now)) return false;
        std::wstring error;
        Response response{};
        const bool ok = a.bridge.Call(Command::Revive, 0, 0, 0, response, error, 2200);
        const DWORD clickedAt = GetTickCount();

        if (ok) {
            a.runtime.lastReviveClickTick = clickedAt;
            a.runtime.revivePhase = 1;
            a.runtime.revivePhaseTick = clickedAt;
            a.runtime.status = L"ĐẦU THAI NỘI BỘ PASS • callback đúng acc chết • không chiếm chuột";
            LogAccount(a, L"ĐẦU THAI P2 PASS: Bridge xác minh IsDeath rồi gọi UIButton.HandleClickEvent nội bộ; chuỗi acc khác không mất index/repeat.");
            return true;
        }
        if (BridgeLooksUnresponsive(error)) EnterClientFreeze(a, L"Bridge timeout khi Đầu thai nội bộ", clickedAt);
        LogAccount(a, L"ĐẦU THAI NỘI BỘ FAIL: " + error);
        return false;
    }

    bool RunPriorityRevivePass(DWORD now, const std::vector<bool>& snapshotReady) {
        (void)now;
        bool clicked = false;
        std::vector<DWORD> visited;

        auto runOne = [&](Account* a) {
            if (!a || !a->runtime.running || RecorderBlocksAccount(*a)) return;
            const auto it = std::find_if(accounts_.begin(), accounts_.end(), [&](const std::unique_ptr<Account>& x) { return x.get() == a; });
            if (it == accounts_.end()) return;
            const std::size_t index = static_cast<std::size_t>(std::distance(accounts_.begin(), it));
            if (index >= snapshotReady.size() || !snapshotReady[index]) return;
            if (std::find(visited.begin(), visited.end(), a->game.pid) != visited.end()) return;
            visited.push_back(a->game.pid);
            if (PriorityReviveClick(*a, GetTickCount())) clicked = true;
        };

        // Legacy helper retained for diagnostics; runtime priority is scoped per account.
        for (int role = 1; role <= 7; ++role) runOne(AccountByTradeRole(role));
        for (auto& item : accounts_) runOne(item.get());
        return clicked;
    }

    bool AutoFightCheckBusy(const Account& a, DWORD now) const {
        const RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        // Hard exclusion gate: an AutoFight check/click sequence may run only when the
        // account is completely idle at the train spot. Do not interleave with any
        // route, mount, death, sell, recovery or another click operation.
        if (rt.sellPhase != 0 || rt.trainRecoveryPhase != 0 || rt.revivePhase != 0) return true;
        if (a.tradeHeld || rt.tradeTravelPhase != 0 || rt.tradeTravelReady) return true;
        if (rt.travelFightGuardPhase != 0 || rt.travelFightStopAttempts != 0) return true;
        if (rt.autoPathFightConflictLatched) return true;
        if (rt.travelMountAttempts != 0 || rt.travelFightBoostPhase != 0 ||
            rt.travelFootFallback) return true;
        if (rt.crossMapRouteArmed || rt.crossMapRouteMoved) return true;
        if (rt.suppressRouteSinceTick != 0 && !Elapsed(now, rt.suppressRouteSinceTick, 2500)) return true;
        if (s.riding || s.autoPathing || s.waitingChangeMap || !s.mapReady) return true;
        if ((s.validMask & ValidLifeState) && s.dead) return true;
        return false;
    }

    void ResetRobustTravel(RuntimeState& rt) {
        rt.travelMountAttempts = 0;
        rt.travelMountTick = 0;
        rt.travelMountCycle = 0;
        rt.travelFightBoostPhase = 0;
        rt.travelFightBoostTick = 0;
        rt.travelFootFallback = false;
        rt.travelFootTick = 0;
    }

    bool HandleMountFightBoost(Account& a, DWORD now, const wchar_t* context) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        const std::wstring where = context ? context : L"đích";
        if ((s.validMask & ValidAutoFight) == 0) {
            rt.status = L"MOUNT RECOVERY • chờ AutoFight authoritative trước boost 10s";
            return true;
        }

        bool ok = false;
        DWORD clickedAt = 0;
        if (rt.travelFightBoostPhase == 0) {
            if (s.autoFight) {
                rt.travelFightBoostPhase = 5;
                rt.travelFightBoostTick = now;
                rt.status = L"MOUNT RECOVERY • AutoFight đã ON → đánh thêm 10s";
                LogAccount(a, L"MOUNT RECOVERY: 2 lần lên ngựa fail; AutoFight đang ON → tính 10s đánh quái.");
                return true;
            }
            if (QueuePriorityAutoClick(a, ClickSlot::Attack,
                                       PriorityAutoOwner::MountRecovery,
                                       L"MOUNT RECOVERY: chạy AUTO→ĐÁNH QUÁI InputSync trước boost 10s")) {
                rt.travelFightBoostPhase = 1;
            }
            return true;
        }
        if (rt.travelFightBoostPhase == 1) {
            if (!ConsumePriorityAutoResult(a, ClickSlot::Attack,
                                           PriorityAutoOwner::MountRecovery,
                                           ok, clickedAt)) return true;
            if (!ok) { rt.travelFightBoostPhase = 0; return true; }
            rt.travelFightBoostPhase = 4;
            rt.travelFightBoostTick = clickedAt;
            rt.status = L"MOUNT RECOVERY • đã chạy 2 click AUTO nội bộ • verify ON";
            return true;
        }
        if (rt.travelFightBoostPhase == 4) {
            if (s.autoFight) {
                rt.travelFightBoostPhase = 5;
                rt.travelFightBoostTick = now;
                rt.status = L"MOUNT RECOVERY • AutoFight ON → đánh thêm 10s";
                LogAccount(a, L"MOUNT RECOVERY: AUTO→ĐÁNH QUÁI InputSync verify ON → đánh thêm 10s trước lần lên ngựa kế.");
                return true;
            }
            if (Elapsed(now, rt.travelFightBoostTick, 1500)) {
                rt.travelFightBoostPhase = 0;
                rt.status = L"MOUNT RECOVERY • chưa bật được Đánh quái → P3 retry";
            }
            return true;
        }
        if (rt.travelFightBoostPhase == 5) {
            if (!Elapsed(now, rt.travelFightBoostTick, kMountFightBoostMs)) {
                const DWORD sec = (now - rt.travelFightBoostTick) / 1000;
                rt.status = L"MOUNT RECOVERY • đánh quái " + std::to_wstring(sec) + L"/10s • " + where;
                return true;
            }
            // After the 10-second fight boost, use exactly the same fail-closed Travel Guard
            // to stop AutoFight before the second mount x2 cycle begins.
            if (!EnsureAutoFightOffForTravel(a, now, L"sau boost 10s trước lên ngựa lại")) {
                rt.status = L"MOUNT RECOVERY • đủ 10s → P3 DỪNG AUTO • chờ OFF";
                return true;
            }
            rt.travelFightBoostPhase = 0;
            rt.travelFightBoostTick = 0;
            rt.travelMountCycle = 1;
            rt.travelMountAttempts = 0;
            rt.travelMountTick = 0;
            rt.status = L"MOUNT RECOVERY • AutoFight OFF → lặp lại lên ngựa x2";
            LogAccount(a, L"MOUNT RECOVERY: đánh 10s xong + AutoFight OFF → bắt đầu chu kỳ lên ngựa x2 lần thứ hai.");
            return true;
        }
        rt.travelFightBoostPhase = 0;
        return true;
    }

    bool HandleRobustTravel(Account& a, DWORD now, const TargetProfile& targetProfile,
                            const wchar_t* context, bool& arrived, int toleranceOverride = 0) {
        arrived = false;
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        State logic{};
        logic.valid = true; logic.mapReady = true; logic.waitingMap = false;
        logic.mapID = s.mapID; logic.x = s.x; logic.y = s.y;
        logic.riding = s.riding != 0; logic.autoPathing = s.autoPathing != 0;
        const int travelTolerance = toleranceOverride > 0 ? toleranceOverride : a.profile.tolerance;
        Target target{targetProfile.mapID, targetProfile.x, targetProfile.y, travelTolerance};
        const std::wstring where = context ? context : L"đích";

        if (AtTarget(logic, target)) {
            if (s.autoPathing) {
                (void)SendDecision(a, Action::StopPath, targetProfile, context);
                return true;
            }
            if (s.riding) {
                (void)SendDecision(a, Action::Dismount, targetProfile, context);
                return true;
            }
            ResetRobustTravel(rt);
            ResetTravelFightGuard(rt);
            arrived = true;
            return true;
        }

        // Once mount #1/#2 have both timed out in the first cycle, do not immediately
        // walk. v0.3 performs P3 AUTO→Đánh quái for 10s, stops it via Travel Guard,
        // then grants a fresh second mount x2 cycle.
        if (rt.travelFightBoostPhase != 0) {
            return HandleMountFightBoost(a, now, context);
        }

        const DWORD phaseElapsed = rt.travelFootFallback
            ? (rt.travelFootTick == 0 ? 0 : now - rt.travelFootTick)
            : (rt.travelMountTick == 0 ? 0 : now - rt.travelMountTick);
        const MountAssistAction assist = DecideMountAssist(s.riding != 0, s.autoPathing != 0,
                                                           rt.travelMountAttempts, rt.travelFootFallback,
                                                           phaseElapsed, kMountRetryWaitMs, kFootWalkMaxMs);
        if (s.riding) {
            const int completedCycle = rt.travelMountCycle;
            ResetRobustTravel(rt);
            if (assist == MountAssistAction::StartPath) {
                (void)SendDecision(a, Action::StartPath, targetProfile, context);
            } else {
                rt.status = L"Đang cưỡi ngựa AutoPath tới " + where;
            }
            if (completedCycle == 1) LogAccount(a, L"MOUNT RECOVERY PASS: lên ngựa thành công sau boost 10s.");
            return true;
        }
        if (rt.travelFootFallback) {
            if (assist == MountAssistAction::FinishFootCycle) {
                if (s.autoPathing) (void)SendDecision(a, Action::StopPath, targetProfile, context);
                ResetRobustTravel(rt);
                ResetTravelFightGuard(rt);
                rt.status = L"Đã chạy bộ tối đa 15s • lặp lại toàn bộ Mount x2 → Fight10s → Mount x2";
                return true;
            }
            if (assist == MountAssistAction::StartPath) (void)SendDecision(a, Action::StartPath, targetProfile, context);
            const DWORD walked = rt.travelFootTick == 0 ? 0 : (now - rt.travelFootTick) / 1000;
            rt.status = L"Chạy bộ dự phòng tới " + where + L" • " + std::to_wstring(walked) + L"/15s";
            return true;
        }
        if (assist == MountAssistAction::Wait) {
            rt.status = rt.travelMountAttempts <= 1 ? L"Chờ lên ngựa lần 1 • tối đa 5s" : L"Chờ lên ngựa lần 2 • tối đa 5s";
            return true;
        }
        if (assist == MountAssistAction::Mount) {
            if (SendDecision(a, Action::Mount, targetProfile, context)) {
                ++rt.travelMountAttempts;
                if (rt.travelMountAttempts > 2) rt.travelMountAttempts = 2;
                rt.travelMountTick = now;
                rt.status = rt.travelMountAttempts == 1 ? L"Lên ngựa lần 1 • chờ 5s" : L"Lên ngựa lần 2 • chờ 5s";
            } else {
                rt.status = L"Chờ gửi lệnh lên ngựa • chưa tính lần thử";
            }
            return true;
        }

        // DecideMountAssist reaches MountCycleFailed here only after two mount attempts timed out.
        if (rt.travelMountCycle == 0) {
            rt.travelFightBoostPhase = 0;
            return HandleMountFightBoost(a, now, context);
        }

        // Second mount x2 cycle also failed: AutoFight must still be OFF before foot path.
        if (!EnsureAutoFightOffForTravel(a, now, L"chạy bộ dự phòng 15s")) {
            rt.status = L"MOUNT RECOVERY • lần 2 vẫn không lên ngựa • chờ AutoFight OFF trước chạy bộ";
            return true;
        }
        if (SendDecision(a, Action::StartPath, targetProfile, context)) {
            rt.travelFootFallback = true;
            rt.travelFootTick = now;
            rt.status = L"Mount x2 lần hai vẫn fail • AutoFight OFF • chạy bộ dự phòng tối đa 15s";
            LogAccount(a, L"MOUNT RECOVERY: Mount x2 → Fight10s → Mount x2 vẫn fail → chạy bộ AutoPath 15s rồi lặp toàn bộ.");
        } else {
            rt.status = L"Mount x2 lần hai vẫn fail • chờ gửi lệnh chạy bộ";
        }
        return true;
    }

    void BeginTrainRecovery(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        rt.trainPositionMonitorArmed = false;
        rt.lastTrainPositionCheckTick = 0;
        rt.trainRecoveryPhase = 4;
        rt.trainRecoveryTick = now;
        rt.fightPhase = 0;
        rt.fightAttempts = 0;
        ResetRobustTravel(rt);
        ResetTravelFightGuard(rt);
        LogAccount(a, L"CHECK 3 PHÚT: lệch bãi → v0.3 Travel Guard bắt buộc AutoFight OFF trước mọi StartPath → quay lại tọa train.");
    }

    bool HandleTrainRecovery(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        if (rt.trainRecoveryPhase == 0) return false;

        bool arrived = false;
        (void)HandleRobustTravel(a, now, a.profile.target, L"bãi train", arrived);
        if (arrived) {
            rt.trainRecoveryPhase = 0;
            rt.trainRecoveryTick = 0;
            rt.wasAtTarget = false;
            rt.fightPhase = 0;
            rt.fightAttempts = 0;
            rt.status = L"Đã về bãi • chuẩn bị bật lại Đánh quái";
            LogAccount(a, L"Đã quay lại bãi sau check lệch • Travel Guard đã bảo đảm path không chạy cùng AutoFight • chuẩn bị P3 AUTO→Đánh quái.");
        }
        return true;
    }

    TargetProfile SellNpcTarget(const Account& a) const {
        const int presetIndex = (a.profile.sellNpcPreset >= 0 && a.profile.sellNpcPreset < static_cast<int>(kSellNpcs.size()))
            ? a.profile.sellNpcPreset : 0;
        const SellNpcPreset& npc = kSellNpcs[static_cast<std::size_t>(presetIndex)];
        TargetProfile t{};
        t.name = npc.name;
        t.mapID = npc.mapID;
        const SellNpcPosition& pos = sellNpcPositions_[static_cast<std::size_t>(presetIndex)];
        t.x = pos.x;
        t.y = pos.y;
        t.valid = pos.valid;
        return t;
    }

    bool SellMacroConfigured(const Account& a, std::wstring& reason) const {
        const int row = fixed_slot_sell_logic::ConfigRowIndex(a.profile.sellMacro.size());
        if (row < 0) {
            reason = L"chưa có dòng tọa độ ô trang bị thứ 2";
            return false;
        }
        const SellMacroStep& step = a.profile.sellMacro[static_cast<std::size_t>(row)];
        if (!step.point.valid) {
            reason = L"dòng item " + std::to_wstring(row + 1) + L" chưa lấy tọa độ (F8)";
            return false;
        }
        reason.clear();
        return true;
    }

    void BeginAutoSell(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        rt.sellPhase = 4;
        rt.sellPhaseTick = now;
        rt.sellOpenAttempts = 0;
        rt.sellMacroIndex = 0;
        rt.sellMacroRepeatDone = 0;
        rt.sellMacroNextTick = 0;
        rt.sellMacroCompletionDueTick = 0;
        rt.sellMacroPass = 0;
        rt.sellLastFreeBag = a.snapshot.freeBagSpace;
        rt.sellBagStableSince = 0;
        rt.sellTriggeredByFullBag = true;
        rt.trainPositionMonitorArmed = false;
        rt.lastTrainPositionCheckTick = 0;
        rt.trainRecoveryPhase = 0;
        rt.fightPhase = 0;
        rt.fightAttempts = 0;
        rt.wasAtTarget = false;
        rt.crossMapSeenAutoPath = false;
        rt.stallSinceTick = 0;
        rt.confirmAttempts = 0;
        rt.crossMapRouteArmed = false;
        rt.crossMapRouteMoved = false;
        ResetRobustTravel(rt);
        if (a.bridge.Attached()) {
            Response r{}; std::wstring error;
            if (!a.bridge.Call(Command::StopPath, 0, 0, 0, r, error, 700) && BridgeLooksUnresponsive(error)) {
                EnterClientFreeze(a, L"Bridge timeout lúc bắt đầu Auto Sell", now);
            }
        }
        const SellNpcPreset& npc = kSellNpcs[static_cast<std::size_t>(a.profile.sellNpcPreset)];
        LogAccount(a, L"TÚI CHẠM NGƯỠNG → bắt đầu bán • " + std::wstring(npc.name));
    }

    bool RunSellMacroClick(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const int fixedRow = fixed_slot_sell_logic::ConfigRowIndex(a.profile.sellMacro.size());
        const SellMacroStep* fixedStep = fixedRow >= 0
            ? &a.profile.sellMacro[static_cast<std::size_t>(fixedRow)] : nullptr;
        const DWORD fixedDelay = fixedStep
            ? static_cast<DWORD>(std::clamp(fixedStep->delayMs, 50, 60000)) : 260u;
        const DWORD delay = rt.sellMacroIndex < 4 ? 350u : (rt.sellMacroIndex == 4 ? fixedDelay : 180u);
        if (rt.sellMacroNextTick != 0 && !Elapsed(now, rt.sellMacroNextTick, delay)) return true;

        Response response{};
        std::wstring error;
        if (rt.sellMacroIndex < 4) {
            if (!a.bridge.Call(Command::AdvanceBackgroundSell, 0, 0, 0, response, error, 2400)) {
                if (BridgeLooksUnresponsive(error)) EnterClientFreeze(a, L"Bridge timeout lúc mở UI bán nền", now);
                ++rt.sellOpenAttempts;
                rt.sellMacroNextTick = now;
                rt.status = L"BÁN NỀN • chờ đúng control UI • thử " +
                            std::to_wstring(rt.sellOpenAttempts) + L"/12";
                if (rt.sellOpenAttempts >= 12) {
                    rt.sellPhase = 10;
                    LogAccount(a, L"BÁN NỀN FAIL khi mở chuỗi shop: " + error + L" • dừng fail-closed");
                }
                return true;
            }
            rt.sellOpenAttempts = 0;
            rt.sellMacroIndex = std::clamp(response.value0, 0, 4);
            rt.sellMacroNextTick = now;
            rt.status = L"BÁN NỀN • UI semantic stage " + std::to_wstring(rt.sellMacroIndex) +
                        L"/4 • " + std::wstring(response.detail);
            return true;
        }

        if (rt.sellMacroIndex == 4) {
            if (!fixedStep || !fixedStep->point.valid) {
                rt.sellPhase = 10;
                rt.status = L"BÁN NỀN FAIL • mất tọa độ ô trang bị cố định";
                LogAccount(a, L"BÁN NỀN FAIL: cấu hình tọa độ item không còn hợp lệ • dừng fail-closed");
                return true;
            }
            const int clickTarget = fixed_slot_sell_logic::EffectiveClickCount(a.sellStep5LearnedRepeat);
            int normalizedX = -1, normalizedY = -1;
            if (!NormalizeClickPointForBridge(a.game, fixedStep->point,
                                              normalizedX, normalizedY, error)) {
                rt.sellPhase = 10;
                rt.status = L"BÁN NỀN FAIL • tọa độ ô trang bị không hợp lệ";
                LogAccount(a, L"BÁN NỀN FAIL tọa độ item: " + error + L" • dừng fail-closed");
                return true;
            }
            if (!a.bridge.Call(Command::SellNextBagItem, normalizedX, normalizedY, 0,
                               response, error, 2600)) {
                if (BridgeLooksUnresponsive(error)) EnterClientFreeze(a, L"Bridge timeout lúc bán ô trang bị", now);
                ++rt.sellOpenAttempts;
                rt.sellMacroNextTick = now;
                rt.status = L"BÁN NỀN • callback ô lỗi " + std::to_wstring(rt.sellOpenAttempts) + L"/6";
                if (rt.sellOpenAttempts >= 6) {
                    rt.sellPhase = 10;
                    LogAccount(a, L"BÁN NỀN FAIL callback item: " + error + L" • dừng fail-closed");
                }
                return true;
            }
            rt.sellOpenAttempts = 0;
            rt.sellMacroNextTick = now;
            rt.sellLastFreeBag = response.value0;
            ++rt.sellMacroRepeatDone;
            if (rt.sellMacroRepeatDone >= clickTarget) {
                rt.sellMacroIndex = 5;
                rt.sellMacroRepeatDone = 0;
                rt.sellMacroCompletionDueTick = now + fixedDelay;
                rt.status = L"BÁN NỀN • đủ " + std::to_wstring(clickTarget) +
                            L" callback ô cố định • bắt đầu đóng shop/tay nải";
            } else {
                rt.status = L"BÁN NỀN • ô cố định " + std::to_wstring(rt.sellMacroRepeatDone) +
                            L"/" + std::to_wstring(clickTarget) +
                            L" • FreeBag=" + std::to_wstring(response.value0);
            }
            return true;
        }

        if (rt.sellMacroIndex == 5) {
            if (rt.sellMacroCompletionDueTick != 0 &&
                static_cast<LONG>(now - rt.sellMacroCompletionDueTick) < 0) {
                rt.status = L"BÁN NỀN • callback cuối xong • chờ hết delay dòng item";
                return true;
            }
            rt.sellMacroCompletionDueTick = 0;
            if (rt.sellMacroRepeatDone >= 4) {
                rt.sellPhase = 7;
                rt.sellPhaseTick = now;
                rt.sellBagStableSince = 0;
                rt.status = L"BÁN NỀN xong • chờ FreeBagSpace xác nhận";
                return true;
            }
            if (!a.bridge.Call(Command::CloseBackgroundSell, 0, 0, 0, response, error, 2200)) {
                if (BridgeLooksUnresponsive(error)) EnterClientFreeze(a, L"Bridge timeout lúc đóng UI bán", now);
                ++rt.sellOpenAttempts;
                rt.sellMacroNextTick = now;
                if (rt.sellOpenAttempts >= 4) {
                    rt.sellPhase = 7;
                    rt.sellPhaseTick = now;
                    rt.sellBagStableSince = 0;
                    LogAccount(a, L"BÁN NỀN: không đóng hết UI sau 4 lần • vẫn chuyển sang verify túi");
                }
                return true;
            }
            rt.sellOpenAttempts = 0;
            rt.sellMacroNextTick = now;
            if (response.resultCode == static_cast<std::int32_t>(ActionResult::NothingToClose)) {
                rt.sellPhase = 7;
                rt.sellPhaseTick = now;
                rt.sellBagStableSince = 0;
                rt.status = L"BÁN NỀN xong • UI đã đóng • verify túi";
            } else {
                ++rt.sellMacroRepeatDone;
                rt.status = L"BÁN NỀN • đã đóng " + std::to_wstring(rt.sellMacroRepeatDone) + L" lớp UI";
            }
        }
        return true;
    }

    bool HandleAutoSell(Account& a, DWORD now) {
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        if (rt.sellPhase == 0) return false;

        if (rt.sellPhase == 4) {
            const TargetProfile npcTarget = SellNpcTarget(a);
            if (!npcTarget.valid) {
                rt.status = L"NPC bán chưa có tọa độ • nhập X/Y hoặc LẤY VỊ TRÍ";
                return true;
            }

            bool arrived = false;
            (void)HandleRobustTravel(a, now, npcTarget, L"NPC bán", arrived);
            if (arrived) {
                rt.lastAction = Action::Hold;
                rt.sellPhase = 5; rt.sellPhaseTick = now;
                rt.status = L"Đã tới NPC • chuẩn bị ClickNPC";
            }
            return true;
        }

        if (rt.sellPhase == 5) {
            if (!Elapsed(now, rt.sellPhaseTick, 500)) return true;
            const SellNpcPreset& npc = kSellNpcs[static_cast<std::size_t>(a.profile.sellNpcPreset)];
            Response r{}; std::wstring error;
            if (!a.bridge.Call(Command::BeginBackgroundSell, npc.npcID, 0, 0, r, error, 2200)) {
                if (BridgeLooksUnresponsive(error)) EnterClientFreeze(a, L"Bridge timeout/busy khi mở phiên bán nền", now);
                ++rt.sellOpenAttempts;
                LogAccount(a, L"BEGIN BÁN NỀN NPC " + std::to_wstring(npc.npcID) + L" FAIL: " + error);
                if (rt.sellOpenAttempts >= 2) { rt.sellPhase = 10; rt.status = L"Không mở được NPC bằng callback nội bộ • chờ thủ công"; }
                else rt.sellPhaseTick = now;
                return true;
            }
            ++rt.sellOpenAttempts;
            ++rt.sellMacroPass;
            rt.sellPhase = 6; rt.sellPhaseTick = now;
            rt.sellMacroIndex = 0; rt.sellMacroRepeatDone = 0; rt.sellMacroNextTick = 0; rt.sellMacroCompletionDueTick = 0;
            // SELL owns only this account's sellPhase/macro cursor. It never stalls
            // an unrelated trade workflow or another client's hidden action.
            rt.status = L"Đã ClickNPC nội bộ ID " + std::to_wstring(npc.npcID) +
                        L" • SELL riêng acc • cửa sổ khác tiếp tục độc lập";
            return true;
        }

        if (rt.sellPhase == 6) {
            if (!Elapsed(now, rt.sellPhaseTick, 1200)) return true;
            return RunSellMacroClick(a, now);
        }

        if (rt.sellPhase == 7) {
            if ((s.validMask & ValidBagSpace) == 0) {
                rt.status = L"Không đọc được FreeBagSpace • không tự kết luận bán xong";
                return true;
            }
            if (s.freeBagSpace > 0) {
                if (rt.sellLastFreeBag != s.freeBagSpace) {
                    rt.sellLastFreeBag = s.freeBagSpace;
                    rt.sellBagStableSince = now;
                } else if (rt.sellBagStableSince == 0) {
                    rt.sellBagStableSince = now;
                } else if (Elapsed(now, rt.sellBagStableSince, 1500)) {
                    a.sellStep5LearnedRepeat = s.freeBagSpace;
                    rt.sellPhase = 8; rt.sellPhaseTick = now;
                    rt.crossMapSeenAutoPath = false; rt.crossMapRouteArmed = false; rt.crossMapRouteMoved = false; rt.stallSinceTick = 0; rt.confirmAttempts = 0;
                    ResetRobustTravel(rt);
                    rt.status = L"Đã nhận diện bán xong • quay về bãi train";
                    LogAccount(a, L"BÁN NỀN XONG • FreeBagSpace=" + std::to_wstring(s.freeBagSpace) +
                                  L" ổn định 1.5s • lần bán tới callback ô cố định=" +
                                  std::to_wstring(fixed_slot_sell_logic::EffectiveClickCount(a.sellStep5LearnedRepeat)) +
                                  L" • quay bãi train");
                }
                return true;
            }
            if (Elapsed(now, rt.sellPhaseTick, 3500)) {
                if (rt.sellMacroPass < 2) {
                    rt.sellPhase = 5; rt.sellPhaseTick = now; rt.sellOpenAttempts = 0;
                    rt.status = L"Túi vẫn full • mở NPC + chạy macro lại lần 2";
                } else {
                    rt.sellPhase = 10;
                    rt.status = L"Macro bán 2 lần nhưng túi vẫn full • chờ thủ công";
                }
            }
            return true;
        }

        if (rt.sellPhase == 8) {
            const TargetProfile& trainTarget = a.profile.target;
            bool arrived = false;
            (void)HandleRobustTravel(a, now, trainTarget, L"bãi train", arrived);
            if (arrived) {
                rt.sellPhase = 0; rt.sellTriggeredByFullBag = false;
                rt.fightPhase = 0; rt.fightAttempts = 0; rt.wasAtTarget = false;
                rt.trainPositionMonitorArmed = false; rt.lastTrainPositionCheckTick = 0;
                rt.lastAction = Action::Hold;
                rt.status = L"Đã về bãi • tiếp tục AUTO train";
                LogAccount(a, L"Đã về bãi train sau bán đồ • tiếp tục chu trình.");
                return false;
            }
            return true;
        }

        if (rt.sellPhase == 10) {
            if ((s.validMask & ValidBagSpace) && s.freeBagSpace > 0) {
                rt.sellPhase = 8; rt.sellPhaseTick = now; ResetRobustTravel(rt);
                rt.status = L"Túi đã có ô trống • quay về bãi train";
            }
            return true;
        }
        return true;
    }

    void TickAccount(Account& a) {
        if (!a.runtime.running) return;
        RuntimeState& rt = a.runtime;
        const Snapshot& s = a.snapshot;
        const DWORD now = GetTickCount();

        if (!s.mapReady || s.waitingChangeMap) {
            rt.candidateCount = 0;
            rt.qualifiedMap = 0;
            rt.stallSinceTick = 0;
            rt.fightPhase = 0;
            rt.status = L"Đang chuyển map • chặn action/click";
            return;
        }
        const std::uint32_t need = ValidMap | ValidPosition | ValidRiding | ValidAutoPath;
        if ((s.validMask & need) != need) {
            rt.status = L"State chưa đủ";
            return;
        }

        // Movement observation is serviced globally before P1/P2/P3 so World Flow-held
        // accounts receive the exact same Lâu Lan stall detection as normal accounts.
        if (HandleDeath(a, now)) return;
        if (UpdateRotationEfficiency(a, now)) return;
        if (HandleRouteOwnershipReset(a, now)) return;
        if (HandleUnderworldAutoFightGuard(a, now)) return;
        // P1 XN is PER-ACCOUNT PRIORITY ONLY and now uses a per-client internal callback, so it
        // remains eligible during World Flow HOLD without touching the Windows cursor.

        if (rt.qualifiedMap != s.mapID) {
            if (rt.candidateMap == s.mapID) ++rt.candidateCount;
            else { rt.candidateMap = s.mapID; rt.candidateCount = 1; }
            if (rt.candidateCount < 2) {
                rt.status = L"Ổn định Map 1/2";
                return;
            }
            rt.qualifiedMap = s.mapID;
            rt.candidateCount = 0;
        }

        if (rt.sellPhase != 0) {
            if (HandleAutoSell(a, now)) return;
        } else if ((s.validMask & ValidBagSpace) &&
                   ShouldAutoSell(tradeEnabled_, a.profile.tradeRole, a.profile.enableSell,
                                  s.freeBagSpace, mainSellThreshold_)) {
            const TargetProfile npcTarget = SellNpcTarget(a);
            if (!npcTarget.valid) {
                rt.status = L"TÚI FULL nhưng NPC bán chưa có tọa độ • nhập X/Y hoặc LẤY VỊ TRÍ";
                return;
            }
            std::wstring sellReason;
            if (!SellMacroConfigured(a, sellReason)) {
                rt.status = L"TÚI FULL nhưng " + sellReason;
                return;
            }
            BeginAutoSell(a, now);
            if (HandleAutoSell(a, now)) return;
        }

        if (rt.trainRecoveryPhase != 0) {
            if (HandleTrainRecovery(a, now)) return;
        }

        // Semantic MessageBox Confirm remains disabled. Lâu Lan P1 XN is scheduled per account
        // globally before P2/P3 and is not part of this train FSM.
        // Steady training mode: AutoFight is checked once per minute, but ONLY when
        // no death/sell/recovery/confirm/path action is active. A busy state does not
        // advance the timer; the check is deferred until the account becomes idle.
        if (rt.trainPositionMonitorArmed) {
            // The exclusion gate also applies while an AUTO→Đánh quái sequence is
            // already in progress. If another operation starts between the two clicks,
            // freeze the sequence and resume only after the account is idle again.
            if (AutoFightCheckBusy(a, now)) {
                rt.status = L"Train • đang có thao tác khác → hoãn check/bật AutoFight";
                return;
            }
            if (rt.fightPhase != 3) {
                if (HandleFightClicks(a, now)) return;
            }

            const bool autoCheckDue = rt.lastAutoFightCheckTick == 0 ||
                                      Elapsed(now, rt.lastAutoFightCheckTick, kAutoFightRecheckMs);
            if (autoCheckDue && !AutoFightCheckBusy(a, now)) {
                if ((s.validMask & ValidAutoFight) == 0) {
                    rt.status = L"CHECK AUTO 1 PHÚT: getter chưa sẵn sàng • không click";
                    return;
                }
                rt.lastAutoFightCheckTick = now;
                if (!s.autoFight) {
                    rt.fightPhase = 0;
                    rt.fightAttempts = 0;
                    LogAccount(a, L"CHECK AUTO 1 PHÚT: AutoFight OFF → chạy AUTO→Đánh quái.");
                    if (HandleFightClicks(a, now)) return;
                } else {
                    LogAccount(a, L"CHECK AUTO 1 PHÚT: AutoFight vẫn ON • không click.");
                }
            }

            if (!Elapsed(now, rt.lastTrainPositionCheckTick, kTrainPositionCheckMs)) {
                const DWORD elapsedMs = rt.lastTrainPositionCheckTick == 0 ? 0 : now - rt.lastTrainPositionCheckTick;
                const DWORD remainSec = elapsedMs >= kTrainPositionCheckMs ? 0 : (kTrainPositionCheckMs - elapsedMs + 999) / 1000;
                rt.status = L"Train ổn định • Auto check 1 phút • tọa check sau " + std::to_wstring(remainSec) + L"s";
                return;
            }

            rt.lastTrainPositionCheckTick = now;
            State monitor{};
            monitor.valid = true; monitor.mapReady = true; monitor.waitingMap = false;
            monitor.mapID = s.mapID; monitor.x = s.x; monitor.y = s.y;
            monitor.riding = s.riding != 0; monitor.autoPathing = s.autoPathing != 0;
            Target monitorTarget{a.profile.target.mapID, a.profile.target.x, a.profile.target.y, a.profile.tolerance};
            if (AtTarget(monitor, monitorTarget)) {
                rt.status = L"CHECK 3 PHÚT: đúng tọa • tiếp tục đánh";
                LogAccount(a, L"CHECK 3 PHÚT: tọa train vẫn đúng.");
                return;
            }
            BeginTrainRecovery(a, now);
            if (HandleTrainRecovery(a, now)) return;
        }

        State logic{};
        logic.valid = true;
        logic.mapReady = true;
        logic.waitingMap = false;
        logic.mapID = s.mapID;
        logic.x = s.x;
        logic.y = s.y;
        logic.riding = s.riding != 0;
        logic.autoPathing = s.autoPathing != 0;
        Target target{a.profile.target.mapID, a.profile.target.x, a.profile.target.y, a.profile.tolerance};
        const bool atTarget = AtTarget(logic, target);
        if (!atTarget) {
            rt.trainPositionMonitorArmed = false;
            rt.lastTrainPositionCheckTick = 0;
            rt.lastAutoFightCheckTick = 0;
            if (rt.wasAtTarget) {
                rt.fightPhase = 0;
                rt.fightAttempts = 0;
            }
            rt.wasAtTarget = false;
        }

        const Action action = Decide(logic, target);
        if (action == Action::Hold) {
            rt.lastAction = Action::Hold;
            if (!rt.wasAtTarget) {
                rt.fightPhase = 0;
                rt.fightAttempts = 0;
                LogAccount(a, L"Đã tới bãi và ổn định.");
            }
            rt.wasAtTarget = true;
            if (HandleFightClicks(a, now)) return;
            rt.status = L"Đúng bãi • giám sát tọa độ";
            return;
        }
        if (action == Action::Wait) {
            if (s.autoPathing) rt.status = L"Đang AutoPath tới bãi";
            return;
        }
        SendDecision(a, action, a.profile.target, L"bãi train");
    }

    void RefreshAccountIdentityIfNeeded(Account& a) {
        if (!a.snapshotValid) return;
        const std::wstring newSection = ProfileSection(a.snapshot, a.game.pid);
        if (a.profile.section == newSection) return;
        // PID fallback is only temporary. Once RoleID is proven, switch to the persistent role profile.
        AccountProfile persistent = LoadProfile(newSection);
        const bool persistentHasData = persistent.tradeRole != 0 ||
            !persistent.selectedSpot.empty() || !persistent.rotationSpots.empty() || persistent.target.valid || persistent.enableSell ||
            !persistent.sellMacro.empty() ||
            std::any_of(persistent.points.begin(), persistent.points.end(), [](const ClickPoint& p){ return p.valid; });
        if (!persistentHasData) {
            persistent = a.profile;
            persistent.section = newSection;
        } else {
            // Merge data captured while identity was temporarily PID-based. The old
            // all-or-nothing switch could make newly captured clicks/macro appear lost.
            if (persistent.tradeRole == 0 && a.profile.tradeRole != 0) persistent.tradeRole = a.profile.tradeRole;
            if (persistent.selectedSpot.empty() && !a.profile.selectedSpot.empty()) persistent.selectedSpot = a.profile.selectedSpot;
            if (persistent.rotationSpots.empty() && !a.profile.rotationSpots.empty()) persistent.rotationSpots = a.profile.rotationSpots;
            if (!persistent.target.valid && a.profile.target.valid) persistent.target = a.profile.target;
            for (std::size_t i = 0; i < persistent.points.size(); ++i) {
                if (!persistent.points[i].valid && a.profile.points[i].valid) persistent.points[i] = a.profile.points[i];
            }
            if (persistent.sellMacro.empty() && !a.profile.sellMacro.empty()) persistent.sellMacro = a.profile.sellMacro;
        }
        persistent.section = newSection;
        SaveProfile(persistent);
        a.profile = persistent;
        if (a.profile.tradeRole >= 2) a.profile.enableSell = false;
        MigrateLegacySpot(a.profile);
        a.displayName = DisplayName(a.snapshot, a.game.pid);
    }

    void UpdateSelectedLive() {
        Account* a = SelectedAccount();
        if (!a) return;
        if (!a->snapshotValid) {
            SetText(live_, L"STATE: chưa đọc được snapshot");
            return;
        }
        const Snapshot& s = a->snapshot;
        std::wstring text = L"STATE " + AccountTag(*a) + L" • " + TradeRoleLabel(a->profile.tradeRole) + L" • M" + std::to_wstring(s.mapID) + L" • " +
                            std::to_wstring(s.x) + L"," + std::to_wstring(s.y) +
                            L" • Ngựa " + (s.riding ? L"ON" : L"OFF") +
                            L" • Path " + (s.autoPathing ? L"ON" : L"OFF");
        if (s.validMask & ValidLifeState) text += L" • " + std::wstring(s.dead ? L"CHẾT" : L"SỐNG");
        if (s.validMask & ValidAutoFight) text += L" • Đánh quái " + std::wstring(s.autoFight ? L"ON" : L"OFF");
        if (s.validMask & ValidBagSpace) text += L" • Túi trống " + std::to_wstring(s.freeBagSpace);
        if (a->profile.enableConfirm) text += (s.mapID == kLauLanMapId ? L" • XN LL watchdog ON" : L" • XN LL idle");
        if (globalPaused_) text += L" • F4 PAUSE";
        if (a->runtime.clientFreezeActive) text += L" • FREEZE ACTION";
        if (!s.mapReady || s.waitingChangeMap) text = L"STATE " + AccountTag(*a) + L" • ĐANG CHUYỂN MAP • FREEZE ACTION";
        SetText(live_, text);
    }

    void Tick() {

        // Snapshots + movement-observation run first, then v0.6.1 services semantic
        // background priorities before coordinate-based trade clicks.
        std::vector<bool> snapshotReady(accounts_.size(), false);
        for (std::size_t i = 0; i < accounts_.size(); ++i) {
            Account& a = *accounts_[i];
            const bool selected = static_cast<int>(i) == SelectedIndex();
            if (!a.runtime.running && !selected) continue;
            std::wstring error;
            const DWORD now = GetTickCount();
            if (!ReadSnapshot(a, error, a.runtime.running ? 700 : 900)) {
                if (a.runtime.running) MarkReadStateFailure(a, error, now);
                else a.runtime.status = L"Mất state/bridge";
                continue;
            }
            snapshotReady[i] = true;
            RefreshAccountIdentityIfNeeded(a);
            // Read-only movement observation MUST run even when BĐPT World Flow holds
            // this account. This feeds the Lâu Lan 3s stall watchdog before any priority click.
            const std::uint32_t observeNeed = ValidMap | ValidPosition | ValidAutoPath;
            if ((a.snapshot.validMask & observeNeed) == observeNeed && a.snapshot.mapReady && !a.snapshot.waitingChangeMap) {
                ObserveMovement(a, GetTickCount());
            }
        }

        // v0.6.1.6 priorities are scoped per account, not global input barriers.
        // For each PID preserve local safety order P1 XN -> P2 revive -> P3 AUTO, while
        // unrelated windows never wait merely because another PID has a higher-priority action.
        if (!globalPaused_) {
            for (std::size_t i = 0; i < accounts_.size(); ++i) {
                if (i >= snapshotReady.size() || !snapshotReady[i]) continue;
                Account& a = *accounts_[i];
                if (!a.runtime.running || RecorderBlocksAccount(a)) continue;
                const DWORD priorityNow = GetTickCount();
                if (PriorityLauLanGateConfirmClick(a, priorityNow)) continue;
                if (PriorityReviveClick(a, priorityNow)) continue;
                if (a.runtime.priorityAutoRequestSlot != ClickSlot::None) {
                    (void)PriorityAutoClick(a);
                }
            }
        }

        for (std::size_t i = 0; i < accounts_.size(); ++i) {
            Account& a = *accounts_[i];
            const bool selected = static_cast<int>(i) == SelectedIndex();
            if (!a.runtime.running && !selected) {
                UpdateAccountRow(static_cast<int>(i), a);
                continue;
            }
            if (!snapshotReady[i]) {
                UpdateAccountRow(static_cast<int>(i), a);
                continue;
            }
            const DWORD now = GetTickCount();
            if (a.runtime.running) {
                // Hidden actions are per-client. REC pauses only the window(s) being
                // captured; unrelated accounts keep their normal FSM ticks. F4 remains global.
                if (RecorderBlocksAccount(a)) {
                    a.runtime.status = L"BĐPT RECORDING CỤC BỘ • chỉ acc này tạm giữ để ghi thao tác tay";
                } else if (HoldUntilClientStable(a, now)) {
                    UpdateAccountRow(static_cast<int>(i), a);
                    continue;
                } else if (!globalPaused_) {
                    if (HandleAutoPathFightInvariant(a, now)) {
                        // Hard invariant owns this tick for both normal and held accounts.
                    } else if (a.tradeHeld) {
                        // World Flow HOLD never owns the life observer. P2 may have invoked
                        // Đầu thai above; keep advancing DEAD -> revive phases -> ALIVE cold restart
                        // here, without releasing FIFO/World Flow ownership.
                        if (!HandleDeath(a, now)) {
                            a.runtime.status = a.runtime.tradeTravelReady
                                ? L"BĐPT HOLD • đã tới TỌA GD • chờ đúng FIFO • LIFE/XN vẫn check"
                                : L"BĐPT WORLD FLOW • đang đi TỌA GD • LIFE/XN vẫn check ưu tiên";
                        }
                    } else TickAccount(a);
                } else a.runtime.status = L"TẠM DỪNG F4 • BĐPT không cấp tick cho acc";
            }
            UpdateAccountRow(static_cast<int>(i), a);
        }
        if (!globalPaused_) {
            Account* activeMain = AccountByPid(tradeTxn_.mainPid);
            Account* activeChild = AccountByPid(tradeTxn_.childPid);
            const bool tradeRecorderBlocked = (activeMain && RecorderBlocksAccount(*activeMain)) ||
                                              (activeChild && RecorderBlocksAccount(*activeChild));
            if (!tradeRecorderBlocked) TickTradeCoordinator(GetTickCount());
            else SetTradeStatus(L"RECORDING CỤC BỘ • giữ workflow GD liên quan; acc khác vẫn chạy");
        }
        UpdateSelectedLive();
    }

    void OnListNotification(const NMHDR* hdr) {
        if (!hdr) return;
        if (hdr->hwndFrom == mainTab_ && hdr->code == TCN_SELCHANGE) {
            const int index = TabCtrl_GetCurSel(mainTab_);
            SwitchMainTab(index);
            return;
        }
        if (hdr->hwndFrom == clientList_ && hdr->code == LVN_ITEMCHANGED) {
            const auto* n = reinterpret_cast<const NMLISTVIEW*>(hdr);
            if ((n->uChanged & LVIF_STATE) != 0 && (n->uNewState & LVIS_SELECTED) != 0) {
                PersistSelectedEditorSafeBeforeSwitch(n->iItem);
                LoadSelectedProfileToUi();
            }
            return;
        }
        if (hdr->hwndFrom == sellMacroList_ && hdr->code == LVN_ITEMCHANGED) {
            const auto* n = reinterpret_cast<const NMLISTVIEW*>(hdr);
            if ((n->uChanged & LVIF_STATE) != 0 && (n->uNewState & LVIS_SELECTED) != 0) {
                ListView_SetItemState(sellMacroList_, n->iItem, LVIS_FOCUSED, LVIS_FOCUSED);
                LoadSelectedMacroEditor();
            }
            return;
        }
        if (hdr->hwndFrom == rotationList_ && hdr->code == LVN_ITEMCHANGED) {
            if (rotationUiLoading_) return;
            const auto* n = reinterpret_cast<const NMLISTVIEW*>(hdr);
            if ((n->uChanged & LVIF_STATE) != 0 && ((n->uOldState ^ n->uNewState) & LVIS_STATEIMAGEMASK) != 0) {
                Account* a = SelectedAccount();
                if (a) {
                    const std::wstring oldSpot = a->profile.selectedSpot;
                    PersistRotationListFromUi(*a);
                    SaveProfile(a->profile);
                    RefreshRotationList();
                    if (_wcsicmp(oldSpot.c_str(), a->profile.selectedSpot.c_str()) != 0) {
                        const DWORD now = GetTickCount();
                        ResetRotationWindow(*a, now);
                        if (a->runtime.running) BeginTrainRecovery(*a, now);
                        LogAccount(*a, L"Đổi pool → bãi hiện tại chuyển sang " + a->profile.selectedSpot);
                    }
                    const int row = SelectedIndex();
                    if (row >= 0) UpdateAccountRow(row, *a);
                }
            }
            return;
        }
    }

    void ToggleGlobalPause() {
        globalPaused_ = !globalPaused_;
        if (globalPaused_) {
            for (auto& item : accounts_) {
                Account& a = *item;
                if (!a.runtime.running) continue;
                if (a.bridge.Attached() && !a.runtime.clientFreezeActive) {
                    Response r{}; std::wstring ignored;
                    (void)a.bridge.Call(Command::StopPath, 0, 0, 0, r, ignored, 700);
                }
                a.runtime.status = L"TẠM DỪNG F4";
            }
            Log(L"F4 → TẠM DỪNG toàn bộ acc đang RUN; StopPath đã gửi, không tự đổi combat.");
        } else {
            for (auto& item : accounts_) if (item->runtime.running) item->runtime.status = L"Tiếp tục sau F4";
            Log(L"F4 → TIẾP TỤC toàn bộ acc đang RUN.");
        }
    }

    void PersistSelectedEditorSafeBeforeSwitch(int newIndex) {
        // LVN_ITEMCHANGED arrives after selection state changes, so we cannot reliably know the old row here.
        // All meaningful editor mutations are persisted immediately on their own events/capture/save.
        (void)newIndex;
    }

    LRESULT Handle(UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
            case WM_CREATE:
                BuildUi();
                return 0;
            case WM_NOTIFY:
                OnListNotification(reinterpret_cast<const NMHDR*>(lp));
                return 0;
            case WM_COMMAND:
                switch (LOWORD(wp)) {
                    case IDC_SCAN:
                        ScanClients();
                        break;
                    case IDC_TRADE_ROLE:
                        if (HIWORD(wp) == CBN_SELCHANGE) ApplySelectedTradeRole();
                        break;
                    case IDC_CONSOLIDATE_TOGGLE:
                        if (HIWORD(wp) == BN_CLICKED) ToggleConsolidationMode();
                        break;
                    case IDC_MAIN_SELL_THRESHOLD:
                        if (HIWORD(wp) == EN_KILLFOCUS) PersistGlobalTradeSettings();
                        break;
                    case IDC_SELL_SEQUENCE:
                        ToggleSellMacroEditor();
                        break;
                    case IDC_MAIN_TRADE_SEQUENCE:
                        OpenTradeSequenceEditor(1);
                        break;
                    case IDC_CHILD_TRADE_SEQUENCE:
                        OpenTradeSequenceEditor(2);
                        break;
                    case IDC_TRADE_RENDEZVOUS_CAPTURE:
                        CaptureTradeRendezvous();
                        break;
                    case IDC_COPY_CLICKS:
                        CopyClicksFromAnotherAccount();
                        break;
                    case IDC_START_CHECKED:
                        StartChecked();
                        break;
                    case IDC_STOP_CHECKED:
                        StopChecked();
                        break;
                    case IDC_ROTATE_DEATH_LIMIT:
                    case IDC_ROTATE_DEATH_WINDOW:
                    case IDC_ROTATE_NO_BAG:
                        if (HIWORD(wp) == EN_KILLFOCUS) PersistSelectedEditor();
                        break;
                    case IDC_SAVE_TARGET:
                        SaveTargetForSelected();
                        break;
                    case IDC_DELETE_SPOT:
                        DeleteSelectedSharedSpot();
                        break;
                    case IDC_SPOT_COMBO:
                        if (HIWORD(wp) == CBN_SELCHANGE) SelectSharedSpotForAccount();
                        break;
                    case IDC_CAPTURE_CONFIRM:
                        BeginCapture(ClickSlot::Confirm);
                        break;
                    case IDC_CAPTURE_REVIVE:
                        BeginCapture(ClickSlot::Revive);
                        break;
                    case IDC_CAPTURE_AUTO:
                        BeginCapture(ClickSlot::AutoMenu);
                        break;
                    case IDC_CAPTURE_ATTACK:
                        BeginCapture(ClickSlot::Attack);
                        break;
                    case IDC_CAPTURE_STOP_AUTO_2:
                        BeginCapture(ClickSlot::StopAuto2);
                        break;
                    case IDC_TEST_CONFIRM:
                        TestClick(ClickSlot::Confirm);
                        break;
                    case IDC_TEST_REVIVE:
                        TestClick(ClickSlot::Revive);
                        break;
                    case IDC_TEST_AUTO:
                        TestClick(ClickSlot::AutoMenu);
                        break;
                    case IDC_TEST_ATTACK:
                        TestClick(ClickSlot::Attack);
                        break;
                    case IDC_TEST_STOP_AUTO_2:
                        TestClick(ClickSlot::StopAuto2);
                        break;
                    case IDC_SELL_ADD:
                        AddSellMacroRow();
                        break;
                    case IDC_SELL_DELETE:
                        DeleteSellMacroRow();
                        break;
                    case IDC_SELL_SAVE:
                        SaveSellMacroRow();
                        break;
                    case IDC_SELL_CAPTURE:
                        BeginMacroCapture();
                        break;
                    case IDC_SELL_TEST:
                        TestSellMacroRow();
                        break;
                    case IDC_SELL_REC:
                        ToggleSellRecorder();
                        break;
                    case IDC_SELL_COPY:
                        CopySelectedSellRows();
                        break;
                    case IDC_SELL_PASTE:
                        PasteSellRows();
                        break;
                    case IDC_SELL_COPY_ACCOUNT:
                        CopySellSequenceFromAnotherAccount();
                        break;
                    case IDC_ENABLE_REVIVE:
                    case IDC_ENABLE_CONFIRM:
                    case IDC_ENABLE_FIGHT:
                    case IDC_ENABLE_SELL:
                        if (HIWORD(wp) == BN_CLICKED) PersistSelectedEditor();
                        break;
                    case IDC_SELL_NPC:
                        if (HIWORD(wp) == CBN_SELCHANGE) OnSellNpcSelectionChanged();
                        break;
                    case IDC_SELL_NPC_CAPTURE:
                        if (HIWORD(wp) == BN_CLICKED) CaptureSellNpcPosition();
                        break;
                    case IDC_SELL_NPC_X:
                    case IDC_SELL_NPC_Y:
                    case IDC_TOLERANCE:
                        if (HIWORD(wp) == EN_KILLFOCUS) PersistSelectedEditor();
                        break;
                }
                return 0;
            case WM_HOTKEY:
                if (static_cast<int>(wp) == kCaptureHotkeyId) {
                    CaptureHotkeyPoint();
                    return 0;
                }
                if (static_cast<int>(wp) == kPauseHotkeyId) {
                    ToggleGlobalPause();
                    return 0;
                }
                break;
            case WM_TIMER:
                if (wp == kRecordTimer) { PollRecorder(); return 0; }
                if (wp == kTimer) Tick();
                return 0;
            case WM_CLOSE:
                if (recorderMode_ != RecorderMode::None) StopRecorder(true);
                DestroyWindow(hwnd_);
                return 0;
            case WM_DESTROY:
                if (recorderMode_ != RecorderMode::None) StopRecorder(false);
                // Auto-save every persistent input before exit. Captures already save
                // immediately; this final pass also commits the currently edited macro row.
                SaveSellMacroRow();
                PersistSelectedEditor();
                SaveSharedSellNpcPositions(sellNpcPositions_);
                for (auto& a : accounts_) SaveProfile(a->profile);
                FlushIni();
                UnregisterHotKey(hwnd_, kCaptureHotkeyId);
                UnregisterHotKey(hwnd_, kPauseHotkeyId);
                for (auto& a : accounts_) a->bridge.Close();
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProcW(hwnd_, msg, wp, lp);
    }

    HINSTANCE instance_ = nullptr;
    HWND hwnd_ = nullptr;
    HWND clientList_ = nullptr;
    HWND selected_ = nullptr;
    HWND live_ = nullptr;
    HWND tradeRoleCombo_ = nullptr;
    HWND tradeEnable_ = nullptr;
    HWND mainSellThresholdEdit_ = nullptr;
    HWND tradeStatus_ = nullptr;
    HWND tradeEditor_ = nullptr;
    HWND tradeSeqList_ = nullptr;
    HWND tradeSeqTarget_ = nullptr;
    HWND tradeSeqKind_ = nullptr;
    HWND tradeSeqDesc_ = nullptr;
    HWND tradeSeqDelay_ = nullptr;
    HWND tradeSeqRepeat_ = nullptr;
    HWND tradeSeqGroupRepeat_ = nullptr;
    HWND tradeRecordButton_ = nullptr;
    HWND tradeRecordStatus_ = nullptr;
    HWND targetName_ = nullptr;
    HWND spotCombo_ = nullptr;
    HWND targetText_ = nullptr;
    HWND tolerance_ = nullptr;
    HWND enableRevive_ = nullptr;
    HWND enableConfirm_ = nullptr;
    HWND rotationList_ = nullptr;
    HWND rotateDeathLimit_ = nullptr;
    HWND rotateDeathWindow_ = nullptr;
    HWND rotateNoFullBag_ = nullptr;
    HWND enableFight_ = nullptr;
    HWND enableSell_ = nullptr;
    HWND sellNpcCombo_ = nullptr;
    HWND sellNpcX_ = nullptr;
    HWND sellNpcY_ = nullptr;
    HWND sellNpcPosText_ = nullptr;
    HWND sellMacroList_ = nullptr;
    HWND sellDesc_ = nullptr;
    HWND sellDelay_ = nullptr;
    HWND sellRepeat_ = nullptr;
    HWND sellRecordButton_ = nullptr;
    HWND sellRecordStatus_ = nullptr;
    std::vector<HWND> sellMacroControls_{};
    bool sellMacroEditorVisible_ = false;
    HWND logCaption_ = nullptr;
    HWND sellSequenceButton_ = nullptr;
    HWND mainTradeSequenceButton_ = nullptr;
    HWND childTradeSequenceButton_ = nullptr;
    HWND tradeRendezvousCaptureButton_ = nullptr;
    HWND tradeRendezvousLabel_ = nullptr;
    std::array<HWND, 5> pointLabels_{};
    HWND log_ = nullptr;
    HWND mainTab_ = nullptr;
    int mainTabIndex_ = 0;
    std::vector<HWND> aboutControls_{};
    std::vector<std::pair<HWND, bool>> autoTabVisibility_{};

    std::vector<std::unique_ptr<Account>> accounts_;
    std::vector<TargetProfile> spots_;
    std::array<SellNpcPosition, kSellNpcs.size()> sellNpcPositions_ = LoadSharedSellNpcPositions();
    ClickSlot captureSlot_ = ClickSlot::None;
    int captureMacroIndex_ = -1;
    DWORD capturePid_ = 0;
    int captureTradeSequenceIndex_ = -1;
    int captureTradeSequenceMode_ = 0;
    int captureTradeSequenceMainRef_ = -1;
    bool globalPaused_ = false;
    bool rotationUiLoading_ = false;

    std::vector<TradeSequenceStep> mainTradeSequence_{};
    std::vector<TradeSequenceStep> childTradeSequence_{}; // v0.2.7 one GLOBAL workflow used by whichever CON is active.
    std::vector<DWORD> tradeQueuePids_{}; // R7: max 3; immutable entry tickets are authoritative FIFO.
    std::uint64_t tradeWorkflowEntryCounter_ = 0;
    std::vector<TradeSequenceStep> legacyChildTradeTemplate_{};
    bool sharedChildTradeMigrationDone_ = false;
    int tradeEditorMode_ = 0; // 1=MAIN shared sequence, 2=GLOBAL ACC CON workflow; selected CON is capture/test donor only.
    DWORD tradeEditorChildPid_ = 0;
    struct TradeTxn {
        TradePhase phase = TradePhase::Idle;
        DWORD mainPid = 0; DWORD childPid = 0; int childSlot = 0;
        DWORD cooldownUntil = 0;
        std::size_t sequenceIndex = 0;
        int sequenceRepeatDone = 0;
        int sequenceGroupRepeatDone = 0;
        int sequencePass = 1;
        DWORD sequenceDueTick = 0;
        int sequenceMainFreeBeforePass = -1;
        DWORD sequenceBagVerifyStartedTick = 0;
        DWORD sequenceBagStableSinceTick = 0;
        int sequenceBagLastFree = -1;
    } tradeTxn_{};
    bool tradeEnabled_ = true;
    TargetProfile tradeRendezvous_{};
    int tradeRendezvousTolerance_ = 120;
    int mainSellThreshold_ = 6;
    RecorderMode recorderMode_ = RecorderMode::None;
    DWORD recorderPrimaryPid_ = 0;
    bool recorderMouseDown_ = false;
    std::vector<RecordedClick> recorderClicks_{};
    std::vector<SellMacroStep> sellClipboard_{};
    std::vector<TradeSequenceStep> tradeClipboard_{};
    int tradeClipboardMode_ = 0;
    bool tradeSeqDragSelecting_ = false;
    bool tradeSeqDragUpdating_ = false;
    int tradeSeqDragStartRow_ = -1;

};

} // namespace

void EnableDpiAwareness() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    using SetContextFn = BOOL (WINAPI*)(HANDLE);
    SetContextFn setContext = nullptr;
    if (user32) ResolveProc(user32, "SetProcessDpiAwarenessContext", setContext);
    if (setContext) {
        // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == (HANDLE)-4. Dynamic lookup keeps old SDKs buildable.
        (void)setContext(reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4)));
    } else {
        (void)SetProcessDPIAware();
    }
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    // Prevent DPI virtualization from corrupting cursor->client coordinate capture on scaled displays.
    EnableDpiAwareness();
    App app;
    if (!app.Create(instance)) return 2;
    app.Show(show);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
