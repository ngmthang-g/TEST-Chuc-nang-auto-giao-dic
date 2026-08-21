#!/usr/bin/env python3
"""Fail-closed source audit for v0.6.1.5 hidden actions and route safety."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(relative: str) -> str:
    return (ROOT / relative).read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL: {message}")


def main() -> None:
    controller = read("src/controller.cpp")
    bridge = read("src/bridge.cpp")
    protocol = read("src/protocol.h")
    guard = read("src/travel_fight_guard_logic.h")
    guard_test = read("src/travel_fight_guard_logic_test.cpp")
    cmake = read("CMakeLists.txt")
    workflow = read(".github/workflows/build.yml")
    version = read("VERSION.txt").strip()

    require(version == "v0.6.1.5", "VERSION.txt is not v0.6.1.5")
    require("Thần Long Item Consolidator v0.6.1.5" in controller,
            "controller title is not v0.6.1.5")
    require("ThanLongItemConsolidator-v0.6.1.5-win-x64" in workflow,
            "Actions artifact is not versioned v0.6.1.5")

    forbidden_physical_injection = (
        "SetCursorPos",
        "SendInput",
        "MOUSEEVENTF_",
        "SetForegroundWindow",
        "BringWindowToTop",
        "AttachThreadInput",
        "WH_MOUSE_LL",
        "PerformRealInputClickDirect",
    )
    for token in forbidden_physical_injection:
        require(token not in controller,
                f"physical mouse injection token remains in controller: {token}")

    require("CoordinatorInternalPointAction" in controller,
            "generic hidden coordinator is missing")
    require("DispatchInternalPointActionDirect" in controller,
            "direct hidden point dispatcher is missing")
    require(controller.count("Command::ClickInternalPoint") >= 2,
            "AUTO and trade must both dispatch the generic internal point command")
    require("TryClickUI" in bridge and "EndUIDrag" in bridge,
            "Bridge no longer contains the proven internal UI press/release path")
    require("ClickInternalPoint = 14" in protocol,
            "generic internal point protocol command changed unexpectedly")
    require("kProtocolVersion = 0x00010615u" in protocol,
            "EXE/DLL protocol was not bumped for v0.6.1.5")

    require("kStopAttemptsBeforeReset = 2" in guard,
            "travel guard must perform two stop attempts before reset")
    require("NeedsAnotherStopBeforeReset" in controller,
            "controller is not using the tested two-stop policy")
    require("ClickSlot::Attack" in controller and
            "TRAVEL GUARD RESET" in controller,
            "AUTO/Attack reset stage is missing from travel guard")
    require("HandleAutoPathFightInvariant" in controller,
            "hard AutoPath/AutoFight conflict recovery is missing")
    require(controller.count("CanDispatchFightStart") >= 2,
            "queued fight-start requests are not rechecked at queue and dispatch time")
    require("PriorityAutoOwner" in controller and
            "priorityAutoCompletedOwner" in controller,
            "P3 request/result mailbox is not isolated by workflow owner")
    require("autoPathFightConflictLatched" in controller,
            "conflict latch is missing")
    require("cấm StartPath khi recovery chưa hoàn tất" in controller,
            "StartPath is not fail-closed during conflict recovery")
    require("EnsureAutoFightOffForTravel(a, now, context)" in controller,
            "StartPath no longer passes through the authoritative AutoFight guard")

    require("travel_fight_guard_logic_tests" in cmake,
            "travel guard self-test target is missing")
    require("travel_fight_guard_logic_tests.exe" in workflow,
            "Windows Actions does not run the travel guard self-test")
    require("HasAutoPathFightConflict(true, true)" in guard_test,
            "conflict truth-table test is missing")
    require("CanDispatchFightStart(true, false)" in guard_test and
            "CanDispatchFightStart(true, true)" in guard_test,
            "fight-start dispatch gate truth-table test is missing")
    require("ConflictRecoveryComplete(true, false, false)" in guard_test,
            "conflict recovery completion test is missing")

    print("PASS: v0.6.1.5 hidden-action and AutoPath/AutoFight source audit")


if __name__ == "__main__":
    main()
