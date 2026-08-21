# BUILD NOTE v0.2.7-R13

Direct base: **v0.2.7-R12**.

## Requested scope completed
- Removed dead `CON GD đến trống >= N` UI/profile/runtime remnants from active source.
- Removed hidden scheduled-shutdown runtime/UI remnants.
- Removed legacy trade-file macro subsystem and old `.macro` files from the distributable source.
- Removed dead child-trigger UI remnants.
- Distributable source contains exactly one `controller.cpp`; historical implementation copies are not shipped.
- Current visible/internal version identity is consistently **v0.2.7-R13**.
- Auto Sell checkbox is the master switch: unchecked means no automatic sell; checked permits the existing mode/role threshold. CON roles remain no-sell under DỒN ĐỒ.

## Protected behavior unchanged
XN MAP barrier, Đầu thai barrier, SELL FSM/atomic sequence, FIFO/relock, queue max 3, AutoPath, F4/F8, R10 received-slot <=8 rule, R11 stable MAIN bag verification behavior, mainSellThreshold=6, route/train/rotation, and two raw SendInput sites.

## GitHub CI evidence
- Branch: `agent/item-consolidator-v0.2.7-r13-strict-cleanup-autosell`
- Draft PR: #18
- Actions run: `32258859028`
- Result: `success`
- Artifact ID: `9367437964`
- Artifact: `ThanLongItemConsolidator-v0.2.7-R13-strict-cleanup-win-x64`

Runtime live-game behavior remains untested until user verification.
