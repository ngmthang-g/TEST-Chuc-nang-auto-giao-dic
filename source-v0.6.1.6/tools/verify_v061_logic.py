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
knowledge = read("PROJECT_KNOWLEDGE.md")
changelog = read("CHANGELOG.md")
readme = read("README.md")

assert version == "v0.6.1"
assert "Thần Long Item Consolidator v0.6.1" in controller
assert "project(ThanLongItemConsolidator VERSION 0.6.1" in cmake
assert "verify_v061_logic.py" in workflow
assert "ThanLongItemConsolidator_v0.6.1.exe" in workflow
assert "ThanLongItemConsolidator-v0.6.1-win-x64" in workflow
assert "kProtocolVersion = 0x00010601u" in protocol

# Preserve the v0.6 requested action scope and its no-remote-worker boundary.
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

# BUG-001: basic discovery may not depend on Lua/Executor. Only a selected Rect or
# TopIcon Lua action may escalate into EnsureUiLua.
discovery_start = bridge.index("bool EnsureUiDiscovery")
discovery_end = bridge.index("bool EnsureUiLua", discovery_start)
discovery = bridge[discovery_start:discovery_end]
assert 'FindField(g_ui.uiObject, "instances")' in discovery
assert "g_ui.executor" not in discovery
assert "g_ui.guiApi" not in discovery
assert "g_ui.systemObject" not in discovery
assert "const bool anyControlClass" in discovery
assert "g_ui.button || g_ui.toggle || g_ui.rect" in discovery

enumerate_start = bridge.index("bool EnumerateControls")
enumerate_end = bridge.index("void AppendLabel", enumerate_start)
enumerate = bridge[enumerate_start:enumerate_end]
assert "EnsureUiDiscovery" in enumerate
assert "EnsureUiLua" not in enumerate

invoke_start = bridge.index("bool InvokeControl")
invoke_end = bridge.index("bool FindUiByName", invoke_start)
invoke = bridge[invoke_start:invoke_end]
assert invoke.index("UiKind::Button") < invoke.index("EnsureUiLua(false")
assert invoke.index("UiKind::Toggle") < invoke.index("EnsureUiLua(false")
assert 'ExactMethod(control.klass, "HandleClickEvent", 0, false)' in invoke

# Executor's namespace was not proven by the donor. Resolve candidates, then bounded
# metadata enumeration, and accept only the correct method surface.
assert "il2cpp_image_get_class_count" in bridge
assert "il2cpp_image_get_class" in bridge
assert "il2cpp_class_get_name" in bridge
assert "count > 65536" in bridge
assert "IsExecutorClass" in bridge
assert "FindUiClassesByMetadata" in bridge
assert "IsUiObjectClass" in bridge
assert "IsButtonClass" in bridge
assert "IsToggleClass" in bridge
assert "IsRectClass" in bridge
assert 'ExactMethod(klass, "get_Instance", 0, true)' in bridge
assert 'ExactMethod(klass, "ExecuteScriptFunction", 3, false)' in bridge
assert "MonoBehaviourExecutor(any namespace)" in bridge
assert "Không resolve đủ UIObject/UIButton/UIToggle/UIRect/Executor" not in bridge
assert "types B/T/R=" in bridge

# Original semantic safety gates remain.
assert "Role::ConfirmMap" in bridge and "messagebox" in scoring.lower()
assert "Có nhiều control cùng điểm; fail-closed" in bridge
assert 'ScalarGetter(leaderClass, "get_IsDeath"' in bridge
assert '"TopIcon", start ? "AutoTrainClick" : "AutoStopClick"' in bridge
assert "GetFreeBagSpace" in bridge
assert "failed.failures >= 3" in bridge
assert "g_sell.callbacks >= 90" in bridge
assert "SafeBagItem" in bridge
assert "for (std::uintptr_t i = 0; i < capacity; ++i)" in bridge

# User runtime failure and uncertainty must be durable project knowledge.
for path in (
    "docs/bugs/BUG_REGISTRY.md",
    "docs/evidence/EVIDENCE_REGISTRY.md",
    "docs/decisions/DECISIONS.md",
    "docs/features/BACKGROUND_UI_ACTIONS.md",
    "docs/history/VERSION_v0.6.1.md",
):
    assert (ROOT / path).is_file(), path
assert "v0.6: **RUNTIME FAIL**" in knowledge
assert "v0.6.1:" in knowledge and "**BUILD PASS**" in knowledge
assert "**RUNTIME UNTESTED**" in knowledge
assert "UNKNOWN" in changelog and "LIKELY" in changelog

# Release package is traceable to the exact CI-produced source artifact. Runtime status
# must remain untested until live-client evidence arrives.
release_zip = ROOT / "release/ThanLongItemConsolidator-v0.6.1-win-x64.zip"
release_hashes = read("release/SHA256SUMS_v0.6.1.txt")
assert release_zip.is_file()
assert "ThanLongItemConsolidator-v0.6.1-win-x64.zip" in readme
assert "862b3690bb37d186462fbb3ea1d96308ed4e092d60f0d08586675c5441114949" in release_hashes
assert "Runtime status: RUNTIME UNTESTED" in release_hashes

print("v0.6.1 capability-lazy UI resolver audit PASS")
