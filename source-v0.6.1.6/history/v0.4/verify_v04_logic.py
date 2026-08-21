from pathlib import Path

root = Path(__file__).resolve().parents[1]
controller = (root / "src/controller.cpp").read_text(encoding="utf-8")
route = (root / "src/route_logic.h").read_text(encoding="utf-8")
version = (root / "VERSION.txt").read_text(encoding="utf-8").strip()
workflow = (root / ".github/workflows/build.yml").read_text(encoding="utf-8")

assert version == "v0.4"
assert "Thần Long Item Consolidator v0.4" in controller
assert "project(ThanLongItemConsolidator VERSION 0.4.0" in (root / "CMakeLists.txt").read_text(encoding="utf-8")
assert "Verify v0.4 logic scope" in workflow and "verify_v04_logic.py" in workflow

# Lâu Lan watchdog replaces old timer-driven Confirm runtime.
assert "constexpr int kLauLanMapId = 5;" in controller
assert "constexpr DWORD kLauLanGateStallMs = 3000;" in controller
assert "constexpr DWORD kLauLanConfirmRetryMs = 3000;" in controller
assert "RunPriorityLauLanGateConfirmPass" in controller
assert "LauLanGateConfirmDue" in controller
assert "PerformBackgroundWindowClick" in controller
assert "PostMessageW(target, WM_MOUSEMOVE" in controller
assert "PostMessageW(target, WM_LBUTTONDOWN" in controller
assert "PostMessageW(target, WM_LBUTTONUP" in controller
assert "s.mapID != kLauLanMapId" in controller
assert "crossMapRouteArmed" in controller and "crossMapRouteMoved" in controller and "crossMapSeenAutoPath" in controller
assert "confirmIntervalSec" not in controller
assert "IDC_CONFIRM_INTERVAL" not in controller
assert "PeriodicConfirm" not in controller
assert "RunPriorityConfirmPass" not in controller
assert "lastPeriodicConfirmTick" not in controller

# Background Lâu Lan click must not take foreground or move real cursor.
beg = controller.index("bool PerformBackgroundWindowClick")
end = controller.index("bool PriorityLauLanGateConfirmClick", beg)
bg = controller[beg:end]
assert "SetForegroundWindow" not in bg
assert "BringWindowToTop" not in bg
assert "SetCursorPos" not in bg
assert "SendInput" not in bg
assert "PostMessageW" in bg

# Physical user mouse pauses auto-clicks only.
assert "constexpr DWORD kUserMouseAutoClickPauseMs = 5000;" in controller
assert "WH_MOUSE_LL" in controller
assert "LowLevelMouseProc" in controller
assert "LLMHF_INJECTED" in controller
assert "UserMouseAutoClicksPaused" in controller
assert "NotePhysicalUserMouseInput" in controller
assert "CHUỘT TAY • pause auto-click ALL 5s" in controller
# Sell and Trade do not abort when user moves during click preparation.
assert controller.count('error.rfind(L"CHUỘT TAY", 0) == 0') >= 3

# v0.4: obsolete scheduler-wide click freeze state is gone; physical clicks keep a lease only.
assert "coordinatorInputFreeze_" not in controller
assert "coordinatorSequenceLease_" in controller
assert "AcquireCoordinatorSequenceLease" in controller
assert "ReleaseCoordinatorSequenceLease" in controller
assert "FREEZE ALL • SEQUENCE" not in controller
assert "CLICK SEQUENCE LEASE protects physical click ownership only" in controller
# The account scheduler must not gate TickAccount on a click-lease/freeze flag.
tick_beg = controller.index("void Tick()")
tick_end = controller.index("void OnListNotification", tick_beg)
tick = controller[tick_beg:tick_end]
assert "coordinatorSequenceLease_" not in tick
assert "coordinatorInputBusy_" not in tick
assert "TickAccount(a)" in tick

# Priority order is conditional Lâu Lan XN, revive, then P3 AUTO.
pos_ll = tick.index("RunPriorityLauLanGateConfirmPass")
pos_revive = tick.index("RunPriorityRevivePass")
pos_auto = tick.index("RunPriorityAutoPass")
assert pos_ll < pos_revive < pos_auto

# Every route StartPath still goes through the v0.3 fail-closed AutoFight Travel Guard.
assert controller.count("a.bridge.Call(Command::StartPath") == 1
assert "if (action == Action::StartPath && !EnsureAutoFightOffForTravel" in controller
assert "kTravelStopAttemptsBeforeReset = 2" in controller
assert "kMountFightBoostMs = 10000" in controller
assert "kFootWalkMaxMs = 15000" in controller
assert "MountCycleFailed" in route
assert "rt.travelMountCycle = 1" in controller
assert "rt.travelMountCycle == 0" in controller

# Adaptive Sell Step 5 remains present.
assert "sellStep5LearnedRepeat" in controller
assert "rt.sellMacroIndex == 4" in controller

print("v0.4 logic audit PASS")
