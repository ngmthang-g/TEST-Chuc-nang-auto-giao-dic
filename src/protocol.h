#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>

namespace autotradetest {

constexpr std::uint32_t kMagic = 0x44525454u; // TTRD
constexpr std::uint32_t kProtocolVersion = 0x00010100u;
constexpr UINT kWakeMessage = WM_APP + 0x532;
constexpr wchar_t kMappingPrefix[] = L"Local\\ThanLongAutoTradeTest_";

enum class Command : std::uint32_t {
    None = 0,
    Probe = 1,
    QueryTeam = 2,
    SelectTarget = 3,
    SelectAndTrade = 4,
    QueryTradeUi = 5,
};

enum class BridgeStage : LONG {
    Idle = 0,
    HookEntered = 1,
    RequestAccepted = 2,
    Il2CppReady = 10,
    MainThreadProof = 20,
    SemanticCall = 30,
    ScanPlayers = 40,
    SelectTarget = 50,
    ReadTradeConstants = 60,
    SendTradePacket = 70,
    QueryTradeUi = 80,
    ResponseReady = 90,
};

struct Request {
    std::uint32_t command = 0;
    std::uint64_t roleId = 0;
};

struct Response {
    std::int32_t ok = 0;
    std::int32_t errorCode = 0;
    std::uint32_t callbackThreadId = 0;
    std::uint64_t selectedRoleId = 0;
    std::int32_t tradeUiVisible = 0;
    wchar_t detail[512]{};
    wchar_t data[4096]{};
};

struct SharedBlock {
    std::uint32_t magic = kMagic;
    std::uint32_t protocolVersion = kProtocolVersion;
    std::uint32_t targetPid = 0;
    std::uint32_t targetWindowThreadId = 0;
    volatile LONG requestSeq = 0;
    volatile LONG completedSeq = 0;
    volatile LONG bridgeLoaded = 0;
    volatile LONG bridgeBusy = 0;
    volatile LONG callbackSeq = 0;
    volatile LONG stage = static_cast<LONG>(BridgeStage::Idle);
    Request request{};
    Response response{};
};

inline void MappingName(DWORD pid, wchar_t* output, std::size_t count) {
    if (!output || count == 0) return;
    wsprintfW(output, L"%s%lu", kMappingPrefix, static_cast<unsigned long>(pid));
}

} // namespace autotradetest
