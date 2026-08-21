# EVIDENCE REGISTRY

## EVID-012 — User requires strict AutoFight-OFF-before-AutoPath recovery

- Type: USER OBSERVATION + EXPLICIT REQUIREMENT
- Date/version: 2026-08-21 / v0.6.1.4 → v0.6.1.5
- Confidence: High for the required invariant; exact original race timing not captured in a raw log

### Observation

The user reports seeing an incorrect state around AutoPath and requires that AutoFight always be stopped first. The requested recovery is explicit: stop once, stop a second time, and if still ON, perform one AUTO/Attack reset before repeating the stop process.

### Supports

- Preserve and test the existing two-stop/reset Travel Guard.
- Recheck queued fight-start work at dispatch time, covering a scheduler race that a one-time busy check cannot exclude.
- Add a post-condition invariant that detects `AutoPath ON + AutoFight ON`, rather than relying only on pre-dispatch intent.
- Block route and trade movement until authoritative snapshots prove recovery.

### Does not prove

- Which exact scheduler interleaving or external client action produced the observed state.
- Live success of the v0.6.1.5 recovery before the matching binaries are tested.

## EVID-011 — v0.6.1.5 removes automatic Windows mouse injection and passes Windows CI

- Type: SOURCE AUDIT + PURE TESTS + LOCAL CROSS-BUILD + GITHUB ACTIONS
- Date/version: 2026-08-21 / v0.6.1.5
- Confidence: High for source/build properties; no live-client claim

### Observation

`tools/verify_v0615_logic.py` finds no foreground, cursor-warp, Windows mouse injection or low-level mouse-hook token in the controller. Trade test/runtime call sites route through `CoordinatorInternalPointAction` and `ClickInternalPoint`. Eight native logic groups pass, `git diff --check` passes and LLVM-MinGW produces PE32+ x64 EXE/DLL locally. GitHub Actions Windows MSVC x64 run 307 (`32427510483`) then passes the verifier, build and all eight self-tests; artifact `9427940998` digest is `sha256:dd21bf0d3f96a4e8b37d623cf5c54beb5a4e8f067033f5fb42f4261596e7988a`.

### Supports

- The checked source contains no automatic physical-mouse fallback.
- Trade and AUTO share the generic internal point transport.
- The two-stop/reset and conflict truth table have pure logic coverage.

### Does not prove

- Live raycast correctness of every saved trade point.
- Live multi-client behavior, correct raycast of the user's configured points or recovery timing inside the game client.

## EVID-010 — AUTO named Lua path fails; user requests proven point sequence

- Type: USER RUNTIME REPORT + CURRENT REQUIREMENT
- Date/version: 2026-08-21 / v0.6.1.3 → v0.6.1.4
- Confidence: High for the reported failure boundary and requested behavior

### Observation

The carried runtime log states `AUTO NỘI BỘ FAIL: Không tìm thấy Lua UI theo tên`. The user requests the same mechanism as the successful 90-click path, specifically `AUTO (click 1) → ĐÁNH QUÁI (click 2)` followed by an AutoFight condition check, with room for more configured internal actions later.

### Supports

- Active P3 must not continue depending solely on the current named Lua resolver.
- Existing F8 `AUTO`, `ĐÁNH QUÁI` and `DỪNG AUTO 2` points are the intended configuration source.
- The outer snapshot proof must remain; point dispatch alone is not start/stop success.

### Does not prove

- That the three saved points are currently correct for every account/resolution.
- Live success of v0.6.1.4 before the matching EXE/DLL is tested.

## EVID-009 — v0.6.1.3 completes the 90-click fixed-item flow

- Type: USER RUNTIME REPORT
- Date/version: 2026-08-21 / v0.6.1.3
- Confidence: High for the reported end-to-end result; exact raw log not attached in this turn

### Observation

The user states that the final file was tested and worked extremely successfully for all 90 clicks.

### Supports

- `TryClickUI(0, point) → EndUIDrag(point)` works live on the user's client/profile for the configured item point.
- BUG-004 is runtime-fixed for that path.
- Reusing the already implemented dispatcher is a narrower change than introducing another remote action engine.

### Does not prove

- AUTO point coordinates or AUTO state transitions.
- Every intermediate counter/detail string without the raw 1/90…90/90 log.

## EVID-008 — v0.6.1.3 build acceptance

- Type: CI / STATIC + PURE LOGIC TESTS
- Date/version: 2026-08-20 / v0.6.1.3
- Confidence: High for source integrity and build acceptance

### Observation

GitHub Actions run 306 (`32418075439`) built source commit `cfaafc210aede366577da4af49f161545824fea3` with MSVC x64. The scope verifier and all seven self-test groups passed. Artifact `9424653318` has GitHub digest `sha256:0f53a9d8860bcddd17e2429349a3829dd20d7c0573b67ca8496a072bc27c9f15`; ZIP integrity passed and both binaries were identified as PE32+ x86-64.

### Binary hashes

- EXE: `5a865e4a5e554b68abf1b2d3a2563c9698d6efbd5299377982b8981ef987a91c`.
- Bridge DLL: `bb2396b879aaf5e014c13b2d5ec9870aade9b1a98d996b4a2213219454d10ef5`.

## EVID-007 — Exact client has a complete internal EventSystem point-click sequence

- Type: CLIENT METADATA + NATIVE CALL-SITE INSPECTION
- Date/version: 2026-08-20 / client-data commit `f0c37b7745be47e185376358c1a51ebaa376475a`
- Confidence: High for the cited client snapshot; live item behavior later confirmed by EVID-009

### Observation

`Assembly-CSharp` metadata exposes `InputSyncManager.get_Instance`, private `TryClickUI(Int32, Vector2)`, `EndUIDrag(Vector2)`, `CancelUIDragState()` and Boolean `_uiDragging`. The native input parser calls `TryClickUI` for the UI-down event and `EndUIDrag` for the matching UI-up event. `TryClickUI` raycasts through Unity `EventSystem`; `EndUIDrag` dispatches the release/click path and clears the stored drag state.

### Supports

- A configured screen point can be dispatched through the client UI system without a Windows cursor event.
- A complete logical click requires press plus release; `TryClickUI` alone is insufficient.
- `_uiDragging` provides a fail-closed proof that the press raycast acquired a UI target and that release cleaned up.

### Does not prove

- That the user's saved coordinate is centered on the intended second equipment cell.
- AUTO point behavior; EVID-009 only promotes the fixed-item route.

## EVID-006 — v0.6.1.2 resolves geometry but finds no custom callback at the item point

- Type: USER_RUNTIME / LOG
- Date/version: 2026-08-20 / v0.6.1.2
- Character/PID: Adonis / RoleID ending 013169 / PID 2804
- Confidence: High for the failure boundary

### Observation

`BÁN NỀN FAIL callback item: Không có UIButton/UIRect callback tại tọa độ đã gán • geometry=149 • dừng fail-closed`

### Supports

- The working semantic sequence reached the item command after Equipment.
- v0.6.1.2 successfully resolved its Unity geometry classes and usable geometry for 149 controls.
- No enumerated custom `UIButton/UIRect` callback owns the configured point; the active callback-object assumption is wrong for this bag cell.
- Row selection and the 90/adaptive count are not implicated because no item callback began.

### Does not prove

- Whether the configured point is correct.
- Runtime success of the replacement EventSystem dispatcher before live retest.

## EVID-005 — v0.6.1.2 build acceptance

- Type: CI / STATIC + PURE LOGIC TESTS
- Date/version: 2026-08-20 / v0.6.1.2
- Confidence: High for source integrity and build acceptance; no live-client claim

### Observation

GitHub Actions run 302 (`32414440608`) built source tree `1c91bcbdba96e85f647b52f3aa21abde86fda1a4` with MSVC x64. Scope audit and all six self-test groups passed. Artifact `9423369955` was ZIP-tested and both binaries were verified as PE32+ x86-64.

### Does not prove

- Which assembly/class layout the live client exposes.
- That a live item control supplies usable RectTransform geometry.
- Runtime success before the user retests the matching EXE/DLL pair.

## EVID-004 — v0.6.1.1 fixed-slot geometry gate fails

- Type: USER_RUNTIME / LOG
- Date/version: 2026-08-20 / v0.6.1.1
- Character/PID: RoleID ending 013169 / PID 2804
- Confidence: High for failure boundary; exact missing Unity class remains unknown

### Observation

`BÁN NỀN FAIL callback item: Thiếu RectTransform/Utility/Screen để hit-test ô cố định • dừng fail-closed`

### Supports

- The semantic sequence already reached Equipment and issued the item command.
- The Bridge opened CoreModule but failed its all-classes-in-CoreModule gate.
- No coordinate hit-test and no item callback occurred; `6/6` is retry count, not macro row 6.

### Does not prove

- Which exact geometry class is absent from CoreModule.
- That the item control exposes a usable RectTransform after multi-assembly resolution.
- Runtime success of v0.6.1.2 before live retest.

## EVID-003 — v0.6.1.1 build acceptance

- Type: CI / STATIC + PURE LOGIC TESTS
- Date/version: 2026-08-20 / v0.6.1.1
- Confidence: High for source integrity and build acceptance; no live-client claim

### Observation

GitHub Actions run 295 (`32402322381`) built the exact source tree `9f09f444df035f41f6700774e47e6572a8ae8aeb` with MSVC x64. Scope audit, route, rotation, shared trade coordinator, background UI scoring and fixed-slot sell self-tests all passed. Artifact `9418953183` was downloaded, ZIP-tested and verified as PE32+ x86-64 for both EXE and Bridge DLL.

### Does not prove

- That the user's live client exposes the item control geometry expected by the Bridge.
- That 90 live callback invocations sell 90 items on that exact client.
- Runtime success before the user retests the matching EXE/DLL pair.

## EVID-002 — v0.6.1 reaches Equipment tab but item stage is wrong

- Type: USER_RUNTIME / DESCRIPTION + SCREENSHOTS
- Date/version: 2026-08-20 / v0.6.1
- Character/PID: MộcBăngMi / PID 12252
- Confidence: High for the stage boundary; no claim that a specific UI class/name is absent

### Observation

The user reports that NPC/shop/Sell/Quick Sell/Equipment tab actions are correct, but the following item-cell stage does not repeat one fixed cell as v0.5 did. A partial sale can leave the bag full while the flow closes UI and returns toward training. The user clarifies that Quick Sell merely makes each item-cell click sell immediately and remaining equipment collapses into the same cell.

### Source corroboration

v0.6.1's `SellNextBagItem` searches control names/ancestors and stops on `NoCandidate`; it does not read the recorded Step 5 coordinate and does not assign the stable post-sale `FreeBagSpace` back to `sellStep5LearnedRepeat`.

### Does not prove

- The exact live name/handler of the second item cell.
- That Windows CI can prove the Unity hit-test on the user's client.
- Runtime success of v0.6.1.1 before live retest.

## EVID-001 — v0.6 user runtime resolver failure

- Type: USER_RUNTIME / LOG
- Date/version: 2026-08-20 / v0.6
- Character/PID: MộcBăngMi, RoleID 4024949, PID 12252
- Confidence: High for observed failure; insufficient to name the null dependency

### Observation

```text
[22:24:26] SESSION ROUTE RESET PASS: AutoPath OFF → route kế tiếp phải do tool StartPath mới để arm Confirm.
[22:24:31] LÂU LAN GATE XN NỘI BỘ FAIL: Không resolve đủ UIObject/UIButton/UIToggle/UIRect/Executor
[22:24:57] TÚI CHẠM NGƯỠNG → bắt đầu bán • Mã Kiêu Minh • M5 • ID 373
[22:25:05] BÁN NỀN FAIL khi mở chuỗi shop: Không resolve đủ UIObject/UIButton/UIToggle/UIRect/Executor • probe trước: Không resolve đủ UIObject/UIButton/UIToggle/UIRect/Executor • dừng fail-closed
```

### Supports

- Route/FSM progressed far enough to invoke both semantic UI flows.
- Both flows failed at their shared UI readiness gate before role scoring/callback.
- v0.6 cannot be called runtime-pass for Confirm or Auto Sell.

### Does not prove

- Which of the aggregated dependencies was null.
- Whether UIButton discovery itself is valid once optional dependencies are removed.
- Whether Revive or AUTO succeeds/fails.
- Whether the v0.6.1 hotfix works at runtime.
