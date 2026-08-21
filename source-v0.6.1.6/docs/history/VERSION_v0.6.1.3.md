# VERSION v0.6.1.3

## Scope

- Direct base: final v0.6.1.2 tree.
- Runtime report: `Không có UIButton/UIRect callback tại tọa độ đã gán • geometry=149`.
- Requested change: fix only the fixed item-cell action after the Equipment tab; preserve every other feature.

## Confirmed failure boundary

The `geometry=149` detail proves the v0.6.1.2 Unity class resolver passed and 149 live UI controls exposed usable geometry. None of those `UIButton/UIRect` callback objects owned the configured bag-cell point. The selected v0.5 row and 90/adaptive count were not reached and are not the cause.

## Exact-client research

The current client-data snapshot `f0c37b7745be47e185376358c1a51ebaa376475a` contains `InputSyncManager` in `Assembly-CSharp` with this native UI-input surface:

- `get_Instance()`;
- `TryClickUI(Int32 btn, UnityEngine.Vector2 screenPos)`;
- `EndUIDrag(UnityEngine.Vector2 screenPos)`;
- `CancelUIDragState()` and Boolean `_uiDragging` state.

Native call-site inspection proves a complete left click is two phases: `TryClickUI(0, point)` on down, then `EndUIDrag(point)` on up. Calling `TryClickUI` alone would leave only the press/drag phase.

## Fix

- Keep the configured coordinate and top-left-to-Unity screen conversion.
- For exactly one Step-5 tick, call the client's EventSystem dispatcher as press then release on the game window thread.
- Require `_uiDragging=false` before press, require it to become true after a successful raycast, and require it to return false after release.
- If release throws or leaves drag state active, call `CancelUIDragState()` and stop fail-closed.
- Do not move the Windows cursor, foreground the game, or use `SendInput`.
- Bump the matching EXE/DLL protocol to `0x00010613`.

## Protected

- `ClickNPC → shop → Bán vật phẩm → Bán nhanh → Trang bị` and close-UI callbacks are unchanged.
- Profiles with at least five sell rows still use row 5 (zero-based index 4); shorter profiles still use the last available row.
- First sale remains 90 item clicks. Later sales use stable learned `FreeBagSpace`, capped at 90.
- F4/F8, AUTO, Revive, Confirm, route, train, rotation, trade and physical MAIN/CON trade code are unchanged.

## Verification status

- Exact client metadata/native call sequence: VERIFIED against the cited data snapshot.
- Static scope verifier and all seven test groups: PASS.
- Windows MSVC x64 CI run 306 (`32418075439`), source commit `cfaafc210aede366577da4af49f161545824fea3`: PASS.
- Live client item sale: user-confirmed RUNTIME PASS for the full 90-click run on 2026-08-21 (EVID-009).
