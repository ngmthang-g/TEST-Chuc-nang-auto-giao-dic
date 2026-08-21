from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


version = read("VERSION.txt").strip()
controller = read("src/controller.cpp")
bridge = read("src/bridge.cpp")
protocol = read("src/protocol.h")
cmake = read("CMakeLists.txt")
workflow = read(".github/workflows/build.yml")
scoring = read("src/background_ui_logic.h")

assert version == "v0.6"
assert "Thần Long Item Consolidator v0.6" in controller
assert "project(ThanLongItemConsolidator VERSION 0.6.0" in cmake
assert "verify_v06_logic.py" in workflow
assert "ThanLongItemConsolidator_v0.6.exe" in workflow
assert "background_ui_logic_tests" in cmake and "background_ui_logic_tests" in workflow

# The donor's persistent remote thread is deliberately not transplanted. v0.6 extends
# the existing per-client UI-thread Bridge and keeps one request in flight per mapping.
assert "CreateRemoteThread" not in controller
assert "CreateRemoteThread" not in bridge
assert "WH_GETMESSAGE" in controller
assert "callbackThreadId != g_shared->targetWindowThreadId" in bridge
assert "Bridge còn bận sau timeout; không gửi chồng request" in controller

for command in (
    "ConfirmMap",
    "Revive",
    "StartAutoFight",
    "StopAutoFight",
    "BeginBackgroundSell",
    "AdvanceBackgroundSell",
    "SellNextBagItem",
    "CloseBackgroundSell",
):
    assert f"{command} =" in protocol
    assert f"Command::{command}" in bridge

# Map confirm must be semantic, MessageBox-scoped and fail closed on ambiguous controls.
confirm_start = controller.index("bool PriorityLauLanGateConfirmClick")
confirm_end = controller.index("bool RunPriorityLauLanGateConfirmPass", confirm_start)
confirm = controller[confirm_start:confirm_end]
assert "Command::ConfirmMap" in confirm
assert "PerformRealInputClickDirect" not in confirm
assert "SetCursorPos" not in confirm and "SendInput" not in confirm
assert "Role::ConfirmMap" in bridge
assert "messagebox" in scoring.lower()
assert "Có nhiều control cùng điểm; fail-closed" in bridge
assert "CollectDescendantLabels" in bridge
assert "if (candidates.empty()) scoreControls(true)" in bridge

# Revive is guarded twice: controller life state plus Bridge IsDeath immediately before callback.
revive_start = controller.index("bool PriorityReviveClick")
revive_end = controller.index("bool RunPriorityRevivePass", revive_start)
revive = controller[revive_start:revive_end]
assert "Command::Revive" in revive
assert "PerformRealInputClickDirect" not in revive
assert 'ScalarGetter(leaderClass, "get_IsDeath"' in bridge
assert 'ExactMethod(control.klass, "HandleClickEvent", 0, false)' in bridge

# AUTO start/stop invoke the exact client Lua quick actions and are snapshot-verified by
# the retained authoritative AutoFight state machines.
assert '"TopIcon", start ? "AutoTrainClick" : "AutoStopClick"' in bridge
priority_start = controller.index("bool PriorityAutoClick")
priority_end = controller.index("bool RunPriorityAutoPass", priority_start)
priority = controller[priority_start:priority_end]
assert "Command::StartAutoFight" in priority and "Command::StopAutoFight" in priority
assert "PerformRealInputClickDirect" not in priority
assert "if (!s.autoFight)" in controller[controller.index("bool EnsureAutoFightOffForTravel"):controller.index("void TestClick")]

# Background sell is semantic and feedback-driven. The old recorded macro remains only
# as legacy config/editor data and cannot drive the active automatic sell phase.
sell_start = controller.index("bool RunSellMacroClick")
sell_end = controller.index("bool HandleAutoSell", sell_start)
sell = controller[sell_start:sell_end]
for command in ("AdvanceBackgroundSell", "SellNextBagItem", "CloseBackgroundSell"):
    assert f"Command::{command}" in sell
assert "CoordinatorClick" not in sell
assert "ScaleClickPoint" not in sell
assert "GetFreeBagSpace" in bridge
assert "failed.failures >= 3" in bridge
assert "g_sell.callbacks >= 90" in bridge
assert "SafeBagItem" in bridge

# UI dictionary iteration covers the entries capacity so deleted hash buckets cannot
# hide later live controls; hard caps remain to avoid runaway scans.
assert "for (std::uintptr_t i = 0; i < capacity; ++i)" in bridge
assert "capacity > 32768" in bridge

# Only the five semantic actions and auto-sell were replaced. Physical SendInput remains
# intentionally available to the unrelated, user-configured trade macro.
assert "PerformRealInputClickDirect" in controller
assert "SendInput" in controller

print("v0.6 background-action audit PASS")
