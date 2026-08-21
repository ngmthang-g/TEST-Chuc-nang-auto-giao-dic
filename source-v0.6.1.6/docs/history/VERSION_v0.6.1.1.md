# VERSION v0.6.1.1

## Identity and scope

- Direct base: exact v0.6.1 release source/tree.
- Requested change: replace only the failing item-cell enumeration stage of Auto Sell.
- Runtime status at creation: user-confirmed shop chain through `Trang bị`; fixed-slot callback remains RUNTIME UNTESTED until the new EXE/DLL pair is tested in game.

## Confirmed v0.5/v0.6.1 difference

v0.5 Step 5 clicked its configured coordinate repeatedly. Its first effective repeat was the configured row repeat (normally 90); after a completed sale and stable `FreeBagSpace`, the next sale used that learned value, capped by the configured repeat.

v0.6.1 did not keep that active behavior. It retained `sellStep5LearnedRepeat` only as an unused compatibility field and replaced Step 5 with automatic control-name/ancestor enumeration.

## v0.6.1.1 change

- Stages `ClickNPC → shop → Bán vật phẩm → Bán nhanh → Trang bị` are unchanged.
- The item stage uses one manually captured coordinate: legacy profiles use row 5; profiles with fewer than five rows use their last row, so a one-row profile works directly.
- Controller converts the captured client point to normalized coordinates.
- Bridge resolves the current live `UIButton`/Lua-backed `UIRect` under that point using Unity `RectTransformUtility.RectangleContainsScreenPoint`, then invokes exactly one internal callback per request.
- The control and its RectTransform are resolved again for every click; no transient UI pointer is retained after the inventory collapses.
- First sale session uses 90 callbacks. After stable completion, later sessions use the learned `FreeBagSpace`, capped at 90. The learned count is runtime-only, matching v0.5 lifetime behavior.
- The configured row delay remains authoritative between callbacks.

## Protected behavior

No route, shop-opening stage, close-UI stage, AUTO, Revive, Confirm, F4, F8, train, rotation, trade coordinator or physical MAIN/CON trade logic was redesigned.

## Safety boundary

- No `SetCursorPos`, `SendInput`, foreground switch or physical mouse lease is used by the fixed-slot stage.
- A missing coordinate, missing Unity geometry surface, no clickable control at the point or an equal-rank ambiguity stops fail-closed.
- The controller still verifies completed selling with stable `FreeBagSpace` before returning to train.
- This is an explicitly requested legacy fixed-slot fallback. The client knowledge base's instance-ID packet path remains the preferred future semantic design, but is outside this one-fix hotfix.

## Verification

- Pure fixed-slot selection/count tests: added.
- Static scope verifier: added.
- Windows x64 CI run 295 (`32402322381`): PASS, including MSVC Release build, scope audit, five self-test groups and artifact staging.
- Binary artifact `9418953183` came from source commit `930ceee7d857254770da32b74aff658c42047237`, tree `9f09f444df035f41f6700774e47e6572a8ae8aeb`.
- Live client: RUNTIME UNTESTED.
