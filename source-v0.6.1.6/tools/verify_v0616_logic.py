#!/usr/bin/env python3
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def r(x): return (ROOT/x).read_text(encoding='utf-8')
def req(c,m):
    if not c: raise SystemExit('FAIL: '+m)
def main():
    c=r('src/controller.cpp'); g=r('src/travel_fight_guard_logic.h'); gt=r('src/travel_fight_guard_logic_test.cpp'); p=r('src/protocol.h'); w=r('.github/workflows/build.yml')
    req(r('VERSION.txt').strip()=='v0.6.1.6','version')
    for t in ('SetCursorPos','SendInput','WH_MOUSE_LL','coordinatorInputBusy_','coordinatorOwnerPid_','coordinatorSequenceLease_','coordinatorSequenceOwnerPid_','coordinatorRecording_'):
        req(t not in c,'legacy global input token remains: '+t)
    req('RecorderBlocksAccount' in c and 'RECORDING CỤC BỘ' in c,'REC not scoped locally')
    req('action == Action::Mount || action == Action::StartPath' in c,'Mount+StartPath shared movement guard missing')
    req('EnsureAutoFightOffForTravel(a, now, movementContext)' in c,'movement guard does not reuse stop-stop-reset FSM')
    req('CanDispatchMovement' in g and 'CanDispatchMovement(true, false)' in gt,'movement invariant self-test missing')
    req('kStopAttemptsBeforeReset = 2' in g and 'TRAVEL GUARD RESET' in c,'two-stop then reset policy missing')
    req('SELL on an unrelated PID never stalls the trade coordinator' in c,'SELL/trade cross-window lock cleanup missing')
    req('priorities are scoped per account' in c,'per-account priority scheduling missing')
    req('kProtocolVersion = 0x00010616u' in p,'protocol not bumped')
    req('ThanLongItemConsolidator-v0.6.1.6-win-x64' in w,'artifact version')
    print('PASS: v0.6.1.6 per-client cleanup + Mount/StartPath AutoFight guard audit')
if __name__=='__main__': main()
