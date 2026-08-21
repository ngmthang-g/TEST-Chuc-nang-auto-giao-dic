# VERSION v0.6.1.4

## Lineage

- Based On: source commit `cfaafc210aede366577da4af49f161545824fea3` plus the final v0.6.1.3 CI/runtime documentation supplied by the user.
- Reason Created: replace the runtime-failing named Lua AUTO action with the already runtime-passed InputSync point dispatcher.
- Previous Runtime State: v0.6.1.3 fixed-item 90-click run is user-confirmed RUNTIME PASS; P3 AUTO Lua start/stop remains RUNTIME FAIL.
- Last Known-Good: v0.6.1.3 for fixed-slot sale; v0.5 coordinate behavior for the visible two-point AUTO sequence.
- Related Requirement: configured `AUTO (click 1) → ĐÁNH QUÁI (click 2) → verify AutoFight`, with an extensible internal point action for future hidden operations.
- Related Bug/Decision/Evidence: BUG-005, DEC-005, EVID-009/EVID-010.

## Mechanism

F8 is capture only. It records client-relative coordinates plus the capture client size. At action time the controller rescales and normalizes the saved point. The Bridge converts top-left client coordinates to Unity screen coordinates and invokes:

```text
InputSyncManager.TryClickUI(0, point)
 -> require _uiDragging=true
 -> InputSyncManager.EndUIDrag(point)
 -> require _uiDragging=false
```

No Windows mouse event, foreground switch or cursor move is created.

## Implementation

- Add protocol command `ClickInternalPoint = 14` and bump protocol to `0x00010614`.
- Keep semantic Confirm/Revive and fixed-slot sale unchanged.
- Route the three configured P3 points through `ClickInternalPoint`.
- Treat an Attack request as one two-step transaction: click `AUTO`, wait 500 ms without blocking the game thread, click `ĐÁNH QUÁI`, then publish the request result.
- Preserve the existing outer success proof: fresh snapshot must show `AutoFight ON`. Stop remains fail-closed until a fresh snapshot shows `AutoFight OFF`.
- Keep the old named Lua Start/Stop commands in the protocol/Bridge for lineage and diagnostics, but active controller P3 does not call them.

## Safety / failure behavior

- Missing/out-of-window F8 points fail before Bridge mutation.
- Existing InputSync drag blocks a new point action.
- Failed release cancels only the drag owned by that request.
- Map transition, death, freeze or busy Bridge prevents the P3 action from advancing.
- A successful point dispatch is not business success; AutoFight state remains authoritative.

## Protected

- Runtime-passed v0.6.1.3 item click and 90/adaptive sale count.
- F4 global pause and F8 capture.
- P1 Confirm, P2 Revive, World Flow, Travel Guard, mount recovery, rotation, FIFO/relock/trade and physical MAIN/CON sequences.

## Verification status

- Static scope verifier: PASS.
- Seven pure logic test groups: PASS with C++17 warnings-as-errors locally.
- Windows MSVC x64 build/CI: pending.
- Live AUTO start/stop: RUNTIME UNTESTED until one PID proves both ON and OFF transitions.
