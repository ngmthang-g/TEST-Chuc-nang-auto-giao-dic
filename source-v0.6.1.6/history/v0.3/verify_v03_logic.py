from pathlib import Path
import re
ROOT=Path(__file__).resolve().parents[1]
controller=(ROOT/'src/controller.cpp').read_text(encoding='utf-8')
assert (ROOT/'VERSION.txt').read_text(encoding='utf-8').strip() == 'v0.3'
assert 'Thần Long Item Consolidator v0.3' in controller
assert controller.count('Command::StartPath') == 1, 'StartPath must have one dispatch site'
assert 'action == Action::StartPath && !EnsureAutoFightOffForTravel' in controller
assert 'RunPriorityConfirmPass(GetTickCount(), snapshotReady);' in controller
assert 'RunPriorityRevivePass(GetTickCount(), snapshotReady);' in controller
assert 'RunPriorityAutoPass(GetTickCount(), snapshotReady);' in controller
assert controller.index('RunPriorityConfirmPass(GetTickCount(), snapshotReady);') < controller.index('RunPriorityRevivePass(GetTickCount(), snapshotReady);') < controller.index('RunPriorityAutoPass(GetTickCount(), snapshotReady);')
for bad in [
    'AutoFight vẫn ON sau 2 lần → cho phép di chuyển',
    'tắt AutoFight 2 lần chưa được → vẫn di chuyển',
    'AutoFight vẫn ON sau 3 lần → cho phép route',
    'thiếu AUTO/DỪNG AUTO 2 → vẫn đi NPC',
    'thiếu AUTO/DỪNG AUTO 2 → vẫn quay về bãi',
]:
    assert bad not in controller, bad
# Runtime AUTO menu clicks must be routed through P3; manual TEST is the only ClickSlotNow use allowed.
assert not re.search(r'ClickSlotNow\(a, ClickSlot::(?:AutoMenu|Attack|StopAuto2)', controller)
for field in ['sellFallbackTravel','sellStopAttempts','trainRecoveryFallbackTravel','trainRecoveryStopAttempts','tradeTravelFallback','tradeTravelStopAttempts','underworldStopPhase','underworldFallbackRoute']:
    assert field not in controller, field
assert 'kMountFightBoostMs = 10000' in controller
assert 'kFootWalkMaxMs = 15000' in controller
assert (ROOT/'TOOL_LOGIC_TABLE.html').exists()
print('V0.3 LOGIC AUDIT PASS')
