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

assert version == "v0.6.1.3"
assert "Thần Long Item Consolidator v0.6.1.3" in controller
assert "project(ThanLongItemConsolidator VERSION 0.6.1.3" in cmake
assert "verify_v0613_logic.py" in workflow
assert "ThanLongItemConsolidator_v0.6.1.3.exe" in workflow
assert "ThanLongItemConsolidator-v0.6.1.3-win-x64" in workflow
assert "kProtocolVersion = 0x00010613u" in protocol

# Preserve the existing WH_GETMESSAGE transport and all action commands.
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

# The four working semantic stages and close stage remain unchanged in shape.
advance_start = bridge.index("bool AdvanceBackgroundSell")
advance_end = bridge.index("bool SellFixedBagSlot", advance_start)
advance = bridge[advance_start:advance_end]
for role in ("ShopEntry", "SellTab", "QuickSell", "EquipmentTab"):
    assert f"Role::{role}" in advance
assert "FindRoleControl(Role::CloseTradeOrBag" in bridge

# Controller still takes exactly the configured v0.5 Step-5 row, normalizes it,
# and sends it to the Bridge. No physical click is allowed in this sell stage.
run_start = controller.index("bool RunSellMacroClick")
run_end = controller.index("bool HandleAutoSell", run_start)
run_sell = controller[run_start:run_end]
assert "ConfigRowIndex" in run_sell
assert "NormalizeClickPointForBridge" in run_sell
assert "Command::SellNextBagItem, normalizedX, normalizedY" in run_sell
assert "CoordinatorClick" not in run_sell

fixed_start = bridge.index("bool SellFixedBagSlot")
fixed_end = bridge.index("bool CloseBackgroundSell", fixed_start)
fixed_sell = bridge[fixed_start:fixed_end]
assert fixed_sell.count("InvokeInternalPointClick(normalizedX, normalizedY") == 1
assert "FindControlAtNormalizedPoint" not in fixed_sell
assert "InvokeControl(item" not in fixed_sell
assert "SetCursorPos" not in bridge
assert "SendInput" not in bridge

# Exact current-client dispatcher: left-button press and release are both
# required. TryClickUI alone is not a complete click.
assert 'class_from_name(g_ui.image, "", "InputSyncManager")' in bridge
assert '"System.Int32", "UnityEngine.Vector2"' in bridge
assert '"CancelUIDragState"' in bridge
assert 'FindField(g_ui.inputSyncManager, "_uiDragging")' in bridge
assert 'FieldType(g_ui.inputSyncDragging, "System.Boolean")' in bridge
dispatch_start = bridge.index("bool InvokeInternalPointClick")
dispatch_end = bridge.index("bool ReadRectArea", dispatch_start)
dispatch = bridge[dispatch_start:dispatch_end]
press_call = dispatch.index("InvokeVoid(g_ui.inputSyncPress")
release_call = dispatch.index("InvokeVoid(g_ui.inputSyncRelease")
assert press_call < release_call
assert "InputSyncManager đang giữ UI drag; không chồng callback item" in dispatch
assert "InputSyncManager raycast không bắt được UI tại tọa độ đã gán" in dispatch
assert dispatch.count("CancelOwnedInputSyncDrag") >= 2
assert '"TryClickUI"' in click_logic
assert '"EndUIDrag"' in click_logic
assert "kLeftButton = 0" in click_logic

# v0.5 Step-5 count lifetime remains untouched: row 5, 90 first, learned
# stable FreeBagSpace later, capped at 90.
assert "kInitialClickCount = 90" in fixed_logic
assert "ConfigRowIndex" in fixed_logic
assert "learnedFreeBagSpace > 0" in fixed_logic
assert "EffectiveClickCount(a.sellStep5LearnedRepeat)" in controller
assert "a.sellStep5LearnedRepeat = s.freeBagSpace" in controller

# F4/F8 and physical MAIN/CON trade input are protected.
assert controller.count("RegisterHotKey(hwnd_, kPauseHotkeyId, MOD_NOREPEAT, VK_F4)") == 1
assert controller.count("if (static_cast<int>(wp) == kPauseHotkeyId)") == 1
assert "ToggleGlobalPause();" in controller
assert controller.count("RegisterHotKey(hwnd_, kCaptureHotkeyId, MOD_NOREPEAT, VK_F8)") == 1
assert "PerformRealInputClickDirect" in controller

assert "internal_ui_click_logic_tests" in cmake
assert "internal_ui_click_logic_tests.exe" in workflow
for path in (
    "src/internal_ui_click_logic.h",
    "src/internal_ui_click_logic_test.cpp",
    "docs/bugs/BUG_REGISTRY.md",
    "docs/evidence/EVIDENCE_REGISTRY.md",
    "docs/decisions/DECISIONS.md",
    "docs/features/BACKGROUND_UI_ACTIONS.md",
    "docs/history/VERSION_v0.6.1.3.md",
):
    assert (ROOT / path).is_file(), path

print("v0.6.1.3 exact InputSyncManager point-click scope audit PASS")
