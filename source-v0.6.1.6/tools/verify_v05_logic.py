from pathlib import Path

root = Path(__file__).resolve().parents[1]
controller = (root / "src/controller.cpp").read_text(encoding="utf-8")
version = (root / "VERSION.txt").read_text(encoding="utf-8").strip()
cmake = (root / "CMakeLists.txt").read_text(encoding="utf-8")
workflow = (root / ".github/workflows/build.yml").read_text(encoding="utf-8")

assert version == "v0.5"
assert "Thần Long Item Consolidator v0.5" in controller
assert "project(ThanLongItemConsolidator VERSION 0.5.0" in cmake
assert "Verify v0.5 logic scope" in workflow and "verify_v05_logic.py" in workflow

# P1 Lâu Lan watchdog: Map 5 only, live tool-owned cross-map AutoPath ON, stall >=3s.
assert "constexpr int kLauLanMapId = 5;" in controller
assert "constexpr DWORD kLauLanGateStallMs = 3000;" in controller
assert "constexpr DWORD kLauLanConfirmRetryMs = 3000;" in controller
assert "RunPriorityLauLanGateConfirmPass" in controller
assert "LauLanGateConfirmDue" in controller
assert "if (!s.autoPathing) return false;" in controller
assert "crossMapRouteArmed" in controller and "crossMapRouteMoved" in controller and "crossMapSeenAutoPath" in controller
assert "keepToolOwnedCrossMapRoute" in controller

# v0.5 explicitly removes the v0.4 background PostMessage path from active runtime.
assert "PerformBackgroundWindowClick" not in controller
assert "PostMessageW" not in controller
p1_beg = controller.index("bool PriorityLauLanGateConfirmClick")
p1_end = controller.index("bool RunPriorityLauLanGateConfirmPass", p1_beg)
p1 = controller[p1_beg:p1_end]
assert "PerformRealInputClickDirect" in p1
assert "coordinatorInputBusy_ = true" in p1
assert "hadSequenceLease" in p1 and "savedSequenceOwner" in p1
assert "foreground + SetCursorPos + SendInput XN" in p1
assert "AccountBusyWithOwnAutoClickUi" not in controller

# Old periodic XN remains fully removed; Confirm coordinate/capture still exists.
assert "confirmIntervalSec" not in controller
assert "IDC_CONFIRM_INTERVAL" not in controller
assert "PeriodicConfirm" not in controller
assert "RunPriorityConfirmPass" not in controller
assert "lastPeriodicConfirmTick" not in controller
assert "ClickSlot::Confirm" in controller

# World Flow held accounts MUST still receive movement observation before P1/P2/P3.
tick_beg = controller.index("void Tick()")
tick_end = controller.index("void OnListNotification", tick_beg)
tick = controller[tick_beg:tick_end]
assert "ObserveMovement(a, GetTickCount())" in tick
pos_observe = tick.index("ObserveMovement(a, GetTickCount())")
pos_ll = tick.index("RunPriorityLauLanGateConfirmPass")
pos_revive = tick.index("RunPriorityRevivePass")
pos_auto = tick.index("RunPriorityAutoPass")
assert pos_observe < pos_ll < pos_revive < pos_auto
assert "if (!HandleDeath(a, now))" in tick
assert "BĐPT WORLD FLOW • đang đi TỌA GD • LIFE/XN vẫn check ưu tiên" in tick

# TickAccount no longer owns ObserveMovement; it is globally serviced for held accounts too.
tickacc_beg = controller.index("void TickAccount(Account& a)")
tickacc_end = controller.index("void UpdateSelectedLive", tickacc_beg)
tickacc = controller[tickacc_beg:tickacc_end]
assert "ObserveMovement(a, now)" not in tickacc

# Death hard reset preserves only the immutable FIFO ticket while tradeHeld, then travel restarts cleanly.
assert "ResetRuntimeForLifeBoundary" in controller
life_beg = controller.index("void ResetRuntimeForLifeBoundary")
life_end = controller.index("bool HandleDeath", life_beg)
life = controller[life_beg:life_end]
assert "tradeWorkflowEntrySeq" in life and "a.tradeHeld" in life
assert "ResetRuntime(a.runtime)" in life
assert controller.count("ResetRuntimeForLifeBoundary(a)") >= 3

# World Flow coordinator pauses travel/rendezvous on death/revive instead of dropping FIFO/hold.
coord_beg = controller.index("void TickTradeCoordinator")
coord_end = controller.index("void TickAccount", coord_beg)
coord = controller[coord_beg:coord_end]
assert "worldFlowLifeRecovery" in coord
assert "tradeTxn_.phase == TradePhase::Sequence" in coord
assert "WORLD FLOW PAUSE • MAIN chết/đang Đầu thai • giữ FIFO/HOLD" in coord
assert "WORLD FLOW PAUSE • CON" in coord
assert "heldPriorityRecovery" in coord
assert "Keep its FIFO/HOLD slot" in coord

# Physical user mouse still pauses all automatic clicks only; route/scanner continue.
assert "constexpr DWORD kUserMouseAutoClickPauseMs = 5000;" in controller
assert "WH_MOUSE_LL" in controller and "LLMHF_INJECTED" in controller
assert "UserMouseAutoClicksPaused" in controller
assert "CHUỘT TAY • pause auto-click ALL 5s" in controller

# Physical click lease remains a click resource only; no scheduler-wide freeze resurrected.
assert "coordinatorInputFreeze_" not in controller
assert "coordinatorSequenceLease_" in controller
assert "CLICK SEQUENCE LEASE protects physical click ownership only" in controller
assert "FREEZE ALL • SEQUENCE" not in controller

# Every StartPath remains behind v0.3 fail-closed AutoFight Travel Guard.
assert controller.count("a.bridge.Call(Command::StartPath") == 1
assert "if (action == Action::StartPath && !EnsureAutoFightOffForTravel" in controller
assert "kTravelStopAttemptsBeforeReset = 2" in controller
assert "kMountFightBoostMs = 10000" in controller
assert "kFootWalkMaxMs = 15000" in controller

# Protected adaptive sell behavior remains present.
assert "sellStep5LearnedRepeat" in controller
assert "rt.sellMacroIndex == 4" in controller

print("v0.5 logic audit PASS")
