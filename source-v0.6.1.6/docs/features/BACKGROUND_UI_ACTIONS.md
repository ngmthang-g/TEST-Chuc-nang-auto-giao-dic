# FEATURE: Background UI actions

## Purpose

Perform Confirm, Revive, AUTO, Auto Sell and MAIN/CON trade actions without moving the physical mouse.

## Current implementation

The controller retains the v0.5 state machines. The per-PID Bridge resolves live managed UI objects at action time and invokes semantic callbacks or the client's own InputSync EventSystem point dispatcher. It does not cache child controls across UI transitions.

## Current runtime status

- v0.6 Confirm: RUNTIME FAIL, BUG-001.
- v0.6 Auto Sell: RUNTIME FAIL, BUG-001.
- v0.6 Revive: UNKNOWN / not reported.
- v0.6–v0.6.1.3 AUTO named Lua action: RUNTIME FAIL, BUG-005.
- v0.6.1: BUILD PASS (Windows CI 274), RUNTIME UNTESTED.
- v0.6.1.1: BUILD PASS; RUNTIME FAIL at Unity geometry class gate before item hit-test.
- v0.6.1.2: BUILD PASS; RUNTIME FAIL at custom callback ownership after geometry resolved (`geometry=149`).
- v0.6.1.3: Windows CI PASS; user-confirmed RUNTIME PASS for the full fixed-item 90-click flow.
- v0.6.1.4: generic InputSync AUTO point sequence in source; build/runtime pending.
- v0.6.1.5: every configured trade click uses the same generic InputSync path; source audit, eight pure tests, local Windows x64 cross-build and GitHub MSVC run 307 pass; live runtime pending.

## Version timeline

### v0.6

- Added semantic callbacks learned from donor v0.8.4.
- Build/CI passed.
- Runtime exposed an aggregate resolver gate before control selection.

### v0.6.1

- Split basic UI and Lua capabilities.
- Added metadata fallback and exact diagnostics.
- Kept state machines, retry timing and mouse-free policy unchanged.

### v0.6.1.1

- Preserved the working semantic shop chain through Equipment tab.
- Replaced only automatic bag-item name/ancestor enumeration with one manually captured fixed-cell hit-test.
- Invokes one current live callback per scheduler tick; no physical mouse input.
- Restored adaptive Step 5 lifetime: 90 callbacks initially, then stable post-sale `FreeBagSpace` for later sessions, capped at 90.

### v0.6.1.2

- Preserved the entire v0.6.1.1 item path.
- Resolved geometry classes independently across CoreModule, UIModule and legacy UnityEngine.dll.
- Added exact missing-class and assembly-availability diagnostics.

### v0.6.1.3

- Preserved the working semantic stages through Equipment and the close stage.
- Replaced only the item-cell custom-control hit-test with the exact current-client `InputSyncManager` EventSystem path.
- Each item tick performs left press plus release at the manually assigned point and verifies the internal drag lifecycle.
- Retained 90 first, learned stable `FreeBagSpace` later, and the 90 ceiling.
- User later confirmed the full 90-click flow succeeds on the live client.

### v0.6.1.4

- Exposes one generic `ClickInternalPoint` action instead of duplicating the item dispatcher.
- P3 Start performs configured `AUTO`, waits 500 ms through controller state, then configured `ĐÁNH QUÁI`.
- P3 Stop performs configured `DỪNG AUTO 2`.
- Start/Stop acceptance still comes from a fresh authoritative `AutoFight` snapshot, not the click return value.

### v0.6.1.5

- Removes the controller's foreground/cursor/Windows-input path and low-level mouse guard.
- Routes trade tests and runtime MAIN/CON sequence rows through one generic hidden point primitive.
- Keeps one request per scheduler action; delay, repeat, group and sequence business verification stay in the controller.
- Adds a hard `AutoPath && AutoFight` conflict latch. Recovery stops AutoPath first, then reuses Stop #1 → Stop #2 → AUTO/Attack reset → repeat until both states are OFF.
- Rechecks AutoPath when AUTO/Attack is queued and immediately before dispatch, closing the stale-request race.
- Tags P3 request/result ownership, preventing Train, Travel Guard and Mount Recovery from consuming one another's same-slot completion.

## Do-not-break rules

- Resolve current controls for every action; never retain stale UI pointers.
- Ambiguous equal-score candidates must fail closed.
- Confirm must remain MessageBox-scoped.
- Revive must recheck `IsDeath` immediately.
- Sell completion must still be verified using stable fresh `FreeBagSpace`; the fixed-slot hotfix retains the 90-callback ceiling.
- Button/Toggle paths must not require Lua Executor.
- Every successful internal point press must receive its matching release; failure cleanup must not cancel a drag that existed before the tool action.
- An AUTO start request may publish completion only after both configured points finish.
- An AUTO start request may dispatch only while AutoPath is authoritatively OFF, including after it has waited in the queue.
- No automatic action may fall back to Windows mouse injection.
- No `StartPath` may pass while AutoFight is ON/unreadable or while route/fight conflict recovery is latched.

## Open architectural item

The frozen-client KB recommends an asynchronous legitimate `System.Action -> MainThread.Execute` boundary. This tool's current direct hook mutation is not promoted to production-verified until the separate CTS proof succeeds.
