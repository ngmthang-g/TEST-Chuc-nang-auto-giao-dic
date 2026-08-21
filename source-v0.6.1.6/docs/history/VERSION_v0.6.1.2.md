# VERSION v0.6.1.2

## Identity and scope

- Direct base: final v0.6.1.1 source tree.
- Requested change: fix only the Unity geometry resolver exposed by the user's `callback ô lỗi 6/6` log.
- Runtime status at creation: RUNTIME UNTESTED.

## Evidence and boundary

v0.6.1.1 opened the semantic shop chain through Equipment, then returned:

`Thiếu RectTransform/Utility/Screen để hit-test ô cố định`

The code emits a different error when CoreModule itself cannot open. Therefore the image opened, but one or more of five classes was absent from that image. The failure occurs before coordinate hit-test and before `InvokeControl`; it does not implicate macro row 5 or the captured coordinate.

## Change

- Open `UnityEngine.CoreModule`, `UnityEngine.UIModule` and legacy `UnityEngine.dll` independently.
- Resolve RectTransform, Transform, GameObject, RectTransformUtility and Screen with class-specific fallback order.
- Search UIModule first for RectTransformUtility.
- Name every missing class and show Core/UI/Legacy assembly availability in the error.
- Add a pure search-policy test and static v0.6.1.2 scope verifier.
- Bump matching EXE/DLL protocol to `0x00010612`.

## Protected

- Six-row v0.5 profiles still use row 5 (zero-based index 4) for the item coordinate.
- First sale remains 90 callbacks; later sessions use stable learned FreeBagSpace capped at 90.
- Semantic stages, close UI, F4/F8, AUTO, Revive, Confirm, travel, train, rotation and trade are unchanged.
- No physical mouse fallback and no `CreateRemoteThread`.

## Remaining live proof

Passing the assembly gate does not prove that the live item control exposes a RectTransform getter. The next log must distinguish:

1. exact remaining missing class, or
2. geometry count / no control under coordinate, or
3. successful callback progress `1/90`.

## Build verification

- Windows x64 CI run 302 (`32414440608`): PASS, including MSVC Release build, scope audit, six self-test groups and artifact staging.
- Binary artifact `9423369955` came from source commit `71ef67610e1b8cb0b14fa357ff50f65ce98adff1`, tree `1c91bcbdba96e85f647b52f3aa21abde86fda1a4`.
- Live client: RUNTIME UNTESTED.
