from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


version = read("VERSION.txt").strip()
controller = read("src/controller.cpp")
bridge = read("src/bridge.cpp")
protocol = read("src/protocol.h")
fixed_logic = read("src/fixed_slot_sell_logic.h")
geometry_logic = read("src/unity_geometry_logic.h")
cmake = read("CMakeLists.txt")
workflow = read(".github/workflows/build.yml")

assert version == "v0.6.1.2"
assert "Thần Long Item Consolidator v0.6.1.2" in controller
assert "project(ThanLongItemConsolidator VERSION 0.6.1.2" in cmake
assert "verify_v0612_logic.py" in workflow
assert "ThanLongItemConsolidator_v0.6.1.2.exe" in workflow
assert "ThanLongItemConsolidator-v0.6.1.2-win-x64" in workflow
assert "kProtocolVersion = 0x00010612u" in protocol

# Preserve the existing action transport and non-sell action surface.
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

# The user-confirmed semantic chain and fixed row-5 item stage remain intact.
advance_start = bridge.index("bool AdvanceBackgroundSell")
advance_end = bridge.index("bool SellFixedBagSlot", advance_start)
advance = bridge[advance_start:advance_end]
for role in ("ShopEntry", "SellTab", "QuickSell", "EquipmentTab"):
    assert f"Role::{role}" in advance

run_start = controller.index("bool RunSellMacroClick")
run_end = controller.index("bool HandleAutoSell", run_start)
run_sell = controller[run_start:run_end]
assert "NormalizeClickPointForBridge" in run_sell
assert "Command::SellNextBagItem, normalizedX, normalizedY" in run_sell
assert "CoordinatorClick" not in run_sell

fixed_start = bridge.index("bool SellFixedBagSlot")
fixed_end = bridge.index("bool CloseBackgroundSell", fixed_start)
fixed_sell = bridge[fixed_start:fixed_end]
assert "FindControlAtNormalizedPoint" in fixed_sell
assert fixed_sell.count("InvokeControl(item") == 1
assert "SetCursorPos" not in bridge
assert "SendInput" not in bridge
assert "kInitialClickCount = 90" in fixed_logic
assert "ConfigRowIndex" in fixed_logic
assert "a.sellStep5LearnedRepeat = s.freeBagSpace" in controller

# Runtime EVID-004 proved CoreModule opened but at least one geometry class was
# absent there. Resolve each class over split-module + legacy layouts.
ensure_start = bridge.index("bool EnsureUiGeometry")
ensure_end = bridge.index("bool StartsWith", ensure_start)
ensure = bridge[ensure_start:ensure_end]
for assembly in (
    'ImageForAssembly("UnityEngine.CoreModule", "UnityEngine.CoreModule.dll")',
    'ImageForAssembly("UnityEngine.UIModule", "UnityEngine.UIModule.dll")',
    'ImageForAssembly("UnityEngine", "UnityEngine.dll")',
):
    assert assembly in ensure
for role in (
    "RectTransform",
    "Transform",
    "GameObject",
    "RectTransformUtility",
    "Screen",
):
    assert f"GeometryClass::{role}" in ensure
assert "Geometry thiếu class:" in ensure
assert "Thiếu RectTransform/Utility/Screen" not in bridge

# Search policy is explicit and independently testable: UI module first for
# RectTransformUtility; Core/legacy first for the core geometry classes.
assert "GeometryClass::RectTransformUtility" in geometry_logic
assert "ImageSlot::UiModule, ImageSlot::CoreModule, ImageSlot::LegacyUnity" in geometry_logic
assert "ImageSlot::CoreModule, ImageSlot::LegacyUnity, ImageSlot::UiModule" in geometry_logic
assert "unity_geometry_logic_tests" in cmake
assert "unity_geometry_logic_tests.exe" in workflow

# F4/F8 and physical trade input remain protected.
assert controller.count("RegisterHotKey(hwnd_, kPauseHotkeyId, MOD_NOREPEAT, VK_F4)") == 1
assert controller.count("if (static_cast<int>(wp) == kPauseHotkeyId)") == 1
assert "ToggleGlobalPause();" in controller
assert controller.count("RegisterHotKey(hwnd_, kCaptureHotkeyId, MOD_NOREPEAT, VK_F8)") == 1
assert "PerformRealInputClickDirect" in controller

for path in (
    "src/unity_geometry_logic_test.cpp",
    "docs/bugs/BUG_REGISTRY.md",
    "docs/evidence/EVIDENCE_REGISTRY.md",
    "docs/decisions/DECISIONS.md",
    "docs/features/BACKGROUND_UI_ACTIONS.md",
    "docs/history/VERSION_v0.6.1.2.md",
):
    assert (ROOT / path).is_file(), path

print("v0.6.1.2 multi-assembly Unity geometry resolver audit PASS")
