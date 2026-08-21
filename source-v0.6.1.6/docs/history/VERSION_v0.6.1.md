# VERSION v0.6.1

## A. Identity / lineage

- Version: v0.6.1
- Date: 2026-08-20
- Based on: v0.6
- Reason created: runtime resolver failure in Confirm and Auto Sell
- Last known-good: none for the v0.6 internal UI callback subsystem
- Regression from: initial v0.6 implementation defect, not a regression of a runtime-confirmed callback version
- Related bug/evidence/decision: BUG-001 / EVID-001 / DEC-001

## B. User runtime report

The user's PID 12252 reached the Lâu Lan confirm and Mã Kiêu Minh sell flows, but both stopped with the same aggregate missing-component error.

## C. State before modification

- Windows build and self-tests: PASS.
- Real Confirm and Auto Sell: FAIL before role scoring.
- Exact missing managed component: UNKNOWN because the error grouped seven dependencies.

## D. Investigation / root cause

- CONFIRMED: `EnsureUiRuntime` required Executor/GUI/System.Object even for UIButton/UIToggle discovery.
- LIKELY: Executor namespace assumption was wrong; donor v0.8.4 resolved native RVAs and did not establish that managed namespace.
- Source/client KB consulted: donor source, `AI_BOOTSTRAP`, UI API analysis and MainThread bridge contract.

## E. Changes made

- `src/bridge.cpp`: capability split, optional control classes, bounded executor metadata scan, method-surface validation, exact error text.
- `src/protocol.h`: protocol `0x00010601`.
- Version/build/resource files: v0.6.1.
- Documentation/registries/verifier updated.

## F. Protected behavior

No controller FSM, route gate, death timing, sell phase, trade macro, F4/F8 or physical MAIN/CON trade behavior was redesigned.

## G. Build / CI

- Initial local capability audit and pure logic self-tests: PASS.
- Windows MSVC x64 CI run 274 (`32387856270`), source commit `e90e08c`: PASS.
- Release artifact: CI run 276 (`32388213200`), source commit `3cfa9347`, artifact `9413784891`.
- ZIP digest matches GitHub; EXE and DLL both inspect as PE32+ x86-64. Exact hashes are in `release/SHA256SUMS_v0.6.1.txt`.

## H. Runtime result

- Status: RUNTIME UNTESTED.
- Awaiting: one-account Confirm, Mã Kiêu Minh stages and separate AUTO test.

## I. Handoff

If v0.6.1 still fails, use its exact component/capability log. Do not infer another namespace or restore coordinate clicks without that new evidence.
