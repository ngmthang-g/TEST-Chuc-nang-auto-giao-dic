from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


version = read("VERSION.txt").strip()
controller = read("src/controller.cpp")
bridge = read("src/bridge.cpp")
protocol = read("src/protocol.h")
fixed_logic = read("src/fixed_slot_sell_logic.h")
cmake = read("CMakeLists.txt")
workflow = read(".github/workflows/build.yml")

assert version == "v0.6.1.1"
assert "Thần Long Item Consolidator v0.6.1.1" in controller
assert "project(ThanLongItemConsolidator VERSION 0.6.1.1" in cmake
assert "verify_v0611_logic.py" in workflow
assert "ThanLongItemConsolidator_v0.6.1.1.exe" in workflow
assert "ThanLongItemConsolidator-v0.6.1.1-win-x64" in workflow
assert "kProtocolVersion = 0x00010611u" in protocol

# Preserve the v0.6.1 transport/action boundary and every non-sell action.
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

# The working shop chain remains semantic through EquipmentTab.
advance_start = bridge.index("bool AdvanceBackgroundSell")
advance_end = bridge.index("bool SellFixedBagSlot", advance_start)
advance = bridge[advance_start:advance_end]
for role in ("ShopEntry", "SellTab", "QuickSell", "EquipmentTab"):
    assert f"Role::{role}" in advance

# Only the broken item stage changes: a manually captured point is normalized,
# hit-tested against current live UI, and exactly one callback is invoked/request.
run_start = controller.index("bool RunSellMacroClick")
run_end = controller.index("bool HandleAutoSell", run_start)
run_sell = controller[run_start:run_end]
assert "NormalizeClickPointForBridge" in run_sell
assert "Command::SellNextBagItem, normalizedX, normalizedY" in run_sell
assert "CoordinatorClick" not in run_sell
assert "PerformRealInputClickDirect" not in run_sell

fixed_start = bridge.index("bool SellFixedBagSlot")
fixed_end = bridge.index("bool CloseBackgroundSell", fixed_start)
fixed_sell = bridge[fixed_start:fixed_end]
assert "FindControlAtNormalizedPoint" in fixed_sell
assert fixed_sell.count("InvokeControl(item") == 1
assert "CollectSafeBagItems" not in bridge
assert "SetCursorPos" not in bridge
assert "SendInput" not in bridge
assert "RectangleContainsScreenPoint" in bridge
assert "UnityEngine.CoreModule" in bridge
assert "ResolveRectTransform" in bridge

# v0.5 adaptive Step 5 semantics requested by the user: first 90, then the
# stable FreeBagSpace learned after a completed sale, capped at 90.
assert "kInitialClickCount = 90" in fixed_logic
assert "learnedFreeBagSpace > 0" in fixed_logic
assert "ConfigRowIndex" in fixed_logic
assert "a.sellStep5LearnedRepeat = s.freeBagSpace" in controller
assert "EffectiveClickCount(a.sellStep5LearnedRepeat)" in controller
assert "rt.sellMacroCompletionDueTick = now + fixedDelay" in run_sell

# F4/F8 and physical trade input are protected and remain present once each.
assert controller.count("RegisterHotKey(hwnd_, kPauseHotkeyId, MOD_NOREPEAT, VK_F4)") == 1
assert controller.count("if (static_cast<int>(wp) == kPauseHotkeyId)") == 1
assert "ToggleGlobalPause();" in controller
assert controller.count("RegisterHotKey(hwnd_, kCaptureHotkeyId, MOD_NOREPEAT, VK_F8)") == 1
assert "PerformRealInputClickDirect" in controller

# Test/build/document handoff must ship with the new hotfix identity.
assert "fixed_slot_sell_logic_tests" in cmake
assert "fixed_slot_sell_logic_tests.exe" in workflow
for path in (
    "docs/bugs/BUG_REGISTRY.md",
    "docs/evidence/EVIDENCE_REGISTRY.md",
    "docs/decisions/DECISIONS.md",
    "docs/features/BACKGROUND_UI_ACTIONS.md",
    "docs/history/VERSION_v0.6.1.1.md",
):
    assert (ROOT / path).is_file(), path

print("v0.6.1.1 fixed-slot background-sell audit PASS")
