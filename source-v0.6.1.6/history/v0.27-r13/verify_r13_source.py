from pathlib import Path
import hashlib

ROOT = Path(__file__).resolve().parents[1]
ACTIVE = [ROOT/'src', ROOT/'resources']
FORBIDDEN = [
    'ChildTradeTargetFreeSlots','childTradeTargetFreeSlots',
    'ShutdownEnabled','LoadShutdownSettings','CheckScheduledShutdown','shutdownEnable_',
    'shutdownHourEdit_','shutdownMinuteEdit_','shutdownStatus_',
    'TradeMacroPairExists','BeginTradeMacro','AdvanceTradeMacro','EffectiveTradeStepRepeat',
    'MacroLibrary','MacroRunner','BackgroundClicker','childTriggerEdit_','IDC_CHILD_TRIGGER',
]
for base in ACTIVE:
    for p in base.rglob('*'):
        if not p.is_file() or p.suffix.lower() not in {'.cpp','.h','.rc','.manifest'}:
            continue
        text = p.read_text(encoding='utf-8', errors='ignore')
        for token in FORBIDDEN:
            assert token not in text, f'{token} remains in {p.relative_to(ROOT)}'

controllers = list(ROOT.rglob('controller*.cpp'))
assert controllers == [ROOT/'src/controller.cpp'], controllers
assert not list(ROOT.rglob('*.macro'))
assert not list(ROOT.rglob('trade_macro.h'))
assert (ROOT/'VERSION.txt').read_text(encoding='utf-8').strip() == 'v0.2.7-R13'

controller = (ROOT/'src/controller.cpp').read_text(encoding='utf-8')
assert controller.count('SendInput(') == 2
for token in [
    'RunPriorityConfirmPass(GetTickCount(), snapshotReady)',
    'RunPriorityRevivePass(GetTickCount(), snapshotReady)',
    'mainSellThreshold_ = 6',
    'DecidePass(beforeFree, afterFree)',
    'HasChildTransferStep(childTradeSequence_)',
    'ShouldAdmitFullChild(',
    'AutoPath không bị khóa',
]:
    assert token in controller, token

helper = (ROOT/'src/trade_coordinator_logic.h').read_text(encoding='utf-8')
assert 'if (freeBagSpace < 0 || !enableSell) return false;' in helper
assert 'if (tradeRole >= 2) return false;' in helper

print('R13 SOURCE AUDIT PASS')
for rel in ['src/controller.cpp','src/bridge.cpp','src/protocol.h','src/trade_coordinator_logic.h','resources/app.rc']:
    b=(ROOT/rel).read_bytes().replace(b'\r\n',b'\n')
    print(rel, hashlib.sha256(b).hexdigest())
