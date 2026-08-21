# BUG REGISTRY

## BUG-007 — AutoPath can be observed together with AutoFight

- Status: FIXED-IN-SOURCE / RUNTIME UNTESTED
- Severity: Critical movement invariant
- First observed: user report after v0.6.1.4 handoff
- Related feature: every route, including trade rendezvous and recovery paths
- Evidence: EVID-012

### Required behavior

Before any StartPath, AutoFight must be authoritatively OFF. Stop is attempted twice; if AutoFight remains ON, one AUTO/Attack reset is dispatched and the two-stop process repeats.

### Fix in v0.6.1.5

- Preserve the existing central `EnsureAutoFightOffForTravel` loop and expose its two-stop rule in a pure tested helper.
- Add a hard conflict latch for any snapshot with `AutoPath=ON && AutoFight=ON`.
- Revalidate AutoPath both when queueing and immediately before dispatching AUTO/Attack, so a request queued by an older idle snapshot cannot race with a new route.
- Match every P3 completion to its workflow owner, not only its click slot, and cancel stale Train work after the account becomes busy.
- Stop AutoPath first, run the same two-stop/reset guard, and require an observed `AutoPath=OFF && AutoFight=OFF` boundary before route recovery can clear.
- Block central StartPath, trade rendezvous and trade sequence while the latch is active.

## BUG-006 — Coordinate trade macro owns the Windows cursor

- Status: FIXED-IN-SOURCE / RUNTIME UNTESTED
- Severity: High usability and multi-client isolation
- First observed: inherited v0.5–v0.6.1.4 trade transport
- Related feature: MAIN/CON trade editor tests and runtime sequence
- Evidence: EVID-011

### Root cause

The trade coordinator converted a saved client point to screen coordinates, foregrounded the target window, moved the cursor and emitted Windows mouse down/up input. The sequence lease therefore serialized ownership of the user's physical mouse.

### Fix in v0.6.1.5

- Delete the physical input function and low-level mouse guard.
- Route every trade point through the existing generic `ClickInternalPoint` Bridge command.
- Keep sequence delay/repeat/group/FIFO and final stable `FreeBagSpace` verification in the controller.
- Keep F8/REC only as manual coordinate capture; no automatic physical fallback remains.

## BUG-005 — P3 AUTO named Lua action is not resolved on the live client

- Status: FIXED-IN-SOURCE / RUNTIME UNTESTED
- Severity: High
- First observed: v0.6.x live AUTO test, reiterated with the v0.6.1.3 handoff
- Related feature: P3 AUTO start/stop only
- Evidence: EVID-009, EVID-010

### Runtime evidence

The user reports `AUTO NỘI BỘ FAIL: Không tìm thấy Lua UI theo tên` while the separate v0.6.1.3 fixed-item InputSync path completes all 90 clicks successfully. Therefore a sell pass must not be treated as proof that `InvokeLuaAction("TopIcon", ...)` works.

### Fix in v0.6.1.4

- Promote the proven InputSync point dispatcher to generic command `ClickInternalPoint`.
- Start uses the configured `AUTO` point, waits 500 ms, then uses the configured `ĐÁNH QUÁI` point.
- Stop uses the configured `DỪNG AUTO 2` point.
- Preserve snapshot proof: start requires `AutoFight ON`; stop/travel requires `AutoFight OFF`.
- Preserve the old semantic commands for history/diagnostics; active P3 no longer depends on their runtime resolver.

### Runtime verification required

Test the three F8 points individually, then prove one full OFF→ON start and one ON→OFF stop on a single PID before enabling multiple accounts.

## BUG-004 — v0.6.1.2 assumes the bag cell is a UIButton/UIRect callback object

- Status: FIXED / RUNTIME PASS IN v0.6.1.3
- Severity: High
- First observed: v0.6.1.2, user runtime 2026-08-20
- Related feature: Auto Sell fixed item-cell action only
- Evidence: EVID-006, EVID-007

### Runtime evidence

`BÁN NỀN FAIL callback item: Không có UIButton/UIRect callback tại tọa độ đã gán • geometry=149 • dừng fail-closed`

The resolver and geometry enumeration succeeded for 149 controls. The active v0.6.1.2 selection model failed because the bag-grid cell is not represented by one of the enumerated custom `UIButton/UIRect` callback objects under that point.

### Fix in v0.6.1.3

- Keep the same manually assigned coordinate.
- Replace only the active item-cell hit-test/callback with the client's own EventSystem point dispatcher: `TryClickUI(0, point)` followed by `EndUIDrag(point)`.
- Verify the dispatch lifecycle with `_uiDragging`; cancel only a drag started by this action if release fails.
- Preserve row 5, 90/adaptive count, delays, close UI, F4 and all other workflows.

### Runtime result

On 2026-08-21 the user confirmed the final v0.6.1.3 build completed the 90-click item flow extremely successfully. This promotes the InputSync item route from runtime-untested to runtime-pass for the reported client/profile.

## BUG-003 — v0.6.1.1 geometry resolver assumes one Unity assembly

- Status: FIXED / RUNTIME PASS FOR THE RESOLVER GATE
- Severity: High
- First observed: v0.6.1.1, user runtime 2026-08-20
- Related feature: Auto Sell fixed item-cell hit-test only
- Evidence: EVID-004

### Runtime evidence

`BÁN NỀN FAIL callback item: Thiếu RectTransform/Utility/Screen để hit-test ô cố định • dừng fail-closed`

The separate CoreModule-open error did not fire. Therefore CoreModule opened, but at least one of RectTransform, Transform, GameObject, RectTransformUtility or Screen was not found in that one image. The old aggregate detail cannot identify the exact class.

### Root cause

CONFIRMED: v0.6.1.1 resolves all five classes only from `UnityEngine.CoreModule`. Unity clients may split UI types into `UnityEngine.UIModule` or use legacy monolithic `UnityEngine.dll`.

### Fix in v0.6.1.2

- Open CoreModule, UIModule and legacy UnityEngine independently.
- Resolve each class through an explicit ordered fallback.
- Report the exact missing class list and assembly availability.
- Preserve row 5, 90/adaptive count and every non-resolver action.

### Runtime result

The later v0.6.1.2 log reached `geometry=149`, proving this class/assembly resolver gate passed. The sale still failed for the separate BUG-004 control-ownership assumption.

## BUG-002 — v0.6.1 item-cell auto-enumeration does not complete the bag sale

- Status: FIXED-IN-SOURCE / RUNTIME UNTESTED
- Severity: High
- First observed: v0.6.1, user runtime 2026-08-20
- Last known-good behavior: v0.5 coordinate Step 5 (physical mouse)
- Related feature: Auto Sell item stage only
- Evidence: EVID-002

### Confirmed scope

The user reports that `ClickNPC → shop → Bán vật phẩm → Bán nhanh → Trang bị` succeeds. The failure begins only when v0.6.1 attempts to discover/callback bag-item controls. v0.6.1 source confirms that active Step 5 no longer consumes the recorded item coordinate or learned repeat.

### Fix in v0.6.1.1

Keep all working semantic stages. Replace only `CollectSafeBagItems` selection with a fresh coordinate-to-live-control hit-test and internal callback. Restore the v0.5 adaptive count lifetime: 90 first, then stable `FreeBagSpace`, capped at 90.

### Runtime verification required

1. Capture the center of equipment cell 2 in the active item row.
2. Confirm cursor and foreground never move during all callbacks.
3. Confirm the first session reaches 90 callbacks, closes UI and learns stable free space.
4. Confirm the next session logs and uses the learned count.

## BUG-001 — Aggregate UI readiness blocks every internal control action

- Status: FIXED-BUILD-PASS / RUNTIME UNTESTED
- Severity: High
- First observed: v0.6, user runtime 2026-08-20 22:24–22:25 local time
- Last known-good: none for this new internal-callback implementation
- First confirmed-bad: v0.6
- Related features: Lâu Lan Confirm, Auto Sell, Revive, AUTO
- Evidence: EVID-001

### Known evidence

Both Confirm and Auto Sell reached `EnsureUiRuntime` and returned the identical aggregate error:

`Không resolve đủ UIObject/UIButton/UIToggle/UIRect/Executor`

Source inspection proves that v0.6 required UIObject, UIButton, UIToggle, UIRectTransform, MonoBehaviourExecutor, LuaSystemAPI_GUI, System.Object and UIObject.instances as one indivisible gate.

### Root cause

- CONFIRMED: dependency scope was wrong. UIButton/UIToggle discovery was unnecessarily blocked by Lua/Executor dependencies.
- UNKNOWN: which exact pointer was null on the user's client, because v0.6 erased that information.
- LIKELY: MonoBehaviourExecutor, because its namespace was guessed while the donor only proved its native RVA.

### Fix in v0.6.1

- Basic discovery and Lua execution are separate capabilities.
- Button/Toggle do not resolve Executor.
- Executor resolution uses candidate namespaces and bounded metadata enumeration with method-surface validation.
- Error output names the exact missing component.

### Runtime verification required

1. Confirm at the Lâu Lan gate.
2. Mã Kiêu Minh semantic shop stages.
3. AUTO start/stop as a separate Lua-dependent test.

### Do not do

- Do not reintroduce coordinate fallback merely because a semantic dependency is absent.
- Do not mark fixed-runtime from CI or static tests.
- Do not conclude all UI class names are wrong from the old aggregate message.
