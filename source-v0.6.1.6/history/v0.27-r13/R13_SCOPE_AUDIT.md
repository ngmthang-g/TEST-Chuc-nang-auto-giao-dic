# R13 scope audit

R13 changes only:
1. Auto Sell checkbox is a mandatory master gate.
2. Visible/current release identity becomes R13.
3. Distributable source cleanup for the user-requested dead systems and duplicate historical source.

Runtime controller behavior is byte-identical to R12 after reversing only the R13 visible version strings. `bridge.cpp`, `protocol.h`, route/rotation logic and `CMakeLists.txt` are byte-identical to R12.

Expected active-source absences:
- `ChildTradeTargetFreeSlots`
- `ShutdownEnabled` / shutdown scheduler functions or HWNDs
- `TradeMacroPairExists`, `BeginTradeMacro`, `AdvanceTradeMacro`, `EffectiveTradeStepRepeat`
- `MacroLibrary`, `MacroRunner`, `BackgroundClicker`
- `childTriggerEdit_`, `IDC_CHILD_TRIGGER`
- legacy `.macro` files / `trade_macro.h`
- duplicate `controller*.cpp`
