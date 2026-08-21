from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


version = read("VERSION.txt").strip()
controller = read("src/controller.cpp")
bridge = read("src/bridge.cpp")
protocol = read("src/protocol.h")
fixed_logic = read("src/fixed_slot_sell_logic.h")
click_logic = read("src/internal_ui_click_logic.h")
cmake = read("CMakeLists.txt")
workflow = read(".github/workflows/build.yml")

assert version == "v0.6.1.4"
assert "Thần Long Item Consolidator v0.6.1.4" in controller
assert "project(ThanLongItemConsolidator VERSION 0.6.1.4" in cmake
assert "verify_v0614_logic.py" in workflow
assert "ThanLongItemConsolidator_v0.6.1.4.exe" in workflow
assert "ThanLongItemConsolidator-v0.6.1.4-win-x64" in workflow
assert "kProtocolVersion = 0x00010614u" in protocol

# Preserve the existing WH_GETMESSAGE transport and one-in-flight invariant.
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
    "ClickInternalPoint",
):
    assert f"{command} =" in protocol
    assert f"Command::{command}" in bridge

# Generic configured-point command must use the exact runtime-proven complete
# InputSync press/release path and the normal bridge safety gate.
point_start = bridge.index("bool ClickInternalPoint")
point_end = bridge.index("bool ReadRectArea", point_start)
point_action = bridge[point_start:point_end]
assert "ResolveClasses" in point_action
assert "SafeForAction" in point_action
assert "InvokeInternalPointClick(normalizedX, normalizedY" in point_action
assert "ActionResult::ActionInvoked" in point_action

dispatch_start = bridge.index("bool InvokeInternalPointClick")
dispatch_end = bridge.index("bool ClickInternalPoint", dispatch_start)
dispatch = bridge[dispatch_start:dispatch_end]
assert dispatch.index("InvokeVoid(g_ui.inputSyncPress") < dispatch.index("InvokeVoid(g_ui.inputSyncRelease")
assert "InputSyncManager đang giữ UI drag; không chồng click nội bộ" in dispatch
assert "InputSyncManager raycast không bắt được UI tại tọa độ đã gán" in dispatch
assert dispatch.count("CancelOwnedInputSyncDrag") >= 2
assert "SetCursorPos" not in bridge
assert "SendInput" not in bridge

# Start AUTO is one logical P3 request but exactly two configured point actions:
# AUTO first, wait 500 ms, then ĐÁNH QUÁI. Only click 2 publishes completion.
assert "AutoStartPlan" in click_logic
assert "AutoStartPoint::AutoMenu, 0" in click_logic
assert "AutoStartPoint::Attack, 500" in click_logic
priority_start = controller.index("bool PriorityAutoClick")
priority_end = controller.index("bool RunPriorityAutoPass", priority_start)
priority = controller[priority_start:priority_end]
assert "AutoStartPlan" in priority
assert "ClickSlot::AutoMenu : ClickSlot::Attack" in priority
assert "NormalizeClickPointForBridge" in priority
assert "Command::ClickInternalPoint" in priority
assert "priorityAutoPointPhase = 1" in priority
assert "click 1/2: AUTO" in priority
assert "click 2/2: ĐÁNH QUÁI" in priority
assert "Command::StartAutoFight" not in priority
assert "Command::StopAutoFight" not in priority
assert "CoordinatorClick" not in priority

fight_start = controller.index("bool HandleFightClicks")
fight_end = controller.index("bool UserMouseAutoClicksPaused", fight_start)
fight = controller[fight_start:fight_end]
assert "ConsumePriorityAutoResult(a, ClickSlot::Attack" in fight
assert "QueuePriorityAutoClick(a, ClickSlot::Attack" in fight
assert "s.autoFight" in fight
assert "AUTO→ĐÁNH QUÁI InputSync verify AutoFight ON" in fight

travel_start = controller.index("bool EnsureAutoFightOffForTravel")
travel_end = controller.index("void TestClick", travel_start)
travel = controller[travel_start:travel_end]
assert "QueuePriorityAutoClick(a, ClickSlot::StopAuto2" in travel
assert "QueuePriorityAutoClick(a, ClickSlot::Attack" in travel

# Manual TEST for the three AUTO-related rows uses each F8 point independently.
test_start = controller.index("void TestClick")
test_end = controller.index("void StartChecked", test_start)
test_click = controller[test_start:test_end]
assert "ClickSlot::AutoMenu" in test_click
assert "ClickSlot::Attack" in test_click
assert "ClickSlot::StopAuto2" in test_click
assert "NormalizeClickPointForBridge" in test_click
assert "Command::ClickInternalPoint" in test_click

# v0.6.1.3 runtime-passed fixed-slot sell path remains intact.
run_start = controller.index("bool RunSellMacroClick")
run_end = controller.index("bool HandleAutoSell", run_start)
run_sell = controller[run_start:run_end]
assert "ConfigRowIndex" in run_sell
assert "Command::SellNextBagItem, normalizedX, normalizedY" in run_sell
assert "CoordinatorClick" not in run_sell
assert "kInitialClickCount = 90" in fixed_logic
assert "EffectiveClickCount(a.sellStep5LearnedRepeat)" in controller
assert "a.sellStep5LearnedRepeat = s.freeBagSpace" in controller

# Protected hotkeys and physical MAIN/CON trade layer are unchanged.
assert controller.count("RegisterHotKey(hwnd_, kPauseHotkeyId, MOD_NOREPEAT, VK_F4)") == 1
assert controller.count("if (static_cast<int>(wp) == kPauseHotkeyId)") == 1
assert "ToggleGlobalPause();" in controller
assert controller.count("RegisterHotKey(hwnd_, kCaptureHotkeyId, MOD_NOREPEAT, VK_F8)") == 1
assert "PerformRealInputClickDirect" in controller

for path in (
    "src/internal_ui_click_logic.h",
    "src/internal_ui_click_logic_test.cpp",
    "docs/bugs/BUG_REGISTRY.md",
    "docs/evidence/EVIDENCE_REGISTRY.md",
    "docs/decisions/DECISIONS.md",
    "docs/features/BACKGROUND_UI_ACTIONS.md",
    "docs/history/VERSION_v0.6.1.4.md",
):
    assert (ROOT / path).is_file(), path

print("v0.6.1.4 generic InputSync AUTO point-sequence scope audit PASS")
