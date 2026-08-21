# DECISION REGISTRY

## DEC-006 — Use one hidden point transport and one fail-closed route invariant

- Date/version: 2026-08-21 / v0.6.1.5
- Status: ACTIVE / RUNTIME UNTESTED
- Decision: all automatic configured-point actions use the generic InputSync Bridge command; no Windows-mouse fallback exists. Every StartPath is released only by authoritative AutoFight-OFF proof, with a separate conflict recovery boundary if path and fight are ever observed together.

### Why

The exact same `TryClickUI → EndUIDrag` lifecycle already completed the live 90-click item flow. Keeping a second foreground/cursor/SendInput transport for trade adds cursor theft, multi-window coupling and a second failure model without adding business semantics. Separately, a pre-dispatch guard cannot alone contain a stale/external/race violation after the fact, so a snapshot invariant must detect and recover it.

### Constraints

- One Bridge request performs one complete internal point action; repeats remain scheduler state.
- Dispatch PASS is not trade PASS; stable MAIN `FreeBagSpace` remains the sequence-level proof.
- Stop AutoFight twice, reset once through AUTO/Attack if needed, then repeat until OFF.
- Recheck AutoPath at both queue and dispatch boundaries for every AUTO/Attack request; unknown or ON is fail-closed.
- Tag request and completion with a workflow owner; a same-slot result from another FSM is never consumable.
- An observed path+fight conflict stops path first and requires both states OFF before clearing.
- F8/REC remain configuration input only; they never authorize automatic Windows mouse injection.
- Live runtime must be tested with cursor outside the game, multiple PIDs and popup/overlay cases.

## DEC-005 — Reuse the proven EventSystem point dispatcher for active P3 AUTO

- Date/version: 2026-08-21 / v0.6.1.4
- Status: ACTIVE SCOPED RUNTIME FALLBACK
- Decision: active P3 uses configured InputSync points for `AUTO → ĐÁNH QUÁI` and `DỪNG AUTO 2`; business success is still proven by the authoritative AutoFight snapshot.

### Why

The exact semantic `TopIcon.AutoTrainClick/AutoStopClick` contract remains verified in client Lua, but the current Bridge's named Lua UI resolution fails at runtime. Separately, the same client/profile has now runtime-proven the complete InputSync press/release path through a 90-click sale. The user explicitly requests applying that working mechanism to AUTO and future configured hidden actions.

### Constraints

- Keep one generic point command; do not duplicate InputSync resolution per feature.
- One Attack request owns both points in order and does not publish completion after click 1.
- Wait for the menu through scheduler state, never `Sleep` on the game hook thread.
- Start/Stop remain fail-closed on snapshot proof.
- F8 coordinates are layout-dependent and must be tested per profile/resolution.
- Do not claim the coordinate fallback supersedes the longer-term semantic/MainThread architecture for new features where an exact reliable action is available.

## DEC-004 — Use the exact client EventSystem point dispatcher for Step 5

- Date/version: 2026-08-20 / v0.6.1.3
- Status: ACTIVE
- Decision: dispatch one fixed-cell left click as `InputSyncManager.TryClickUI(0, point)` followed by `EndUIDrag(point)` on the existing game window thread.

### Why

Runtime proves 149 custom UI controls expose geometry but none owns the bag-cell point. Exact client metadata and native call sites instead prove that `InputSyncManager` already maps a screen point through Unity EventSystem and performs separate press/release phases.

### Constraints

- Resolve by class/method/field metadata; do not hardcode native RVAs.
- Never call `TryClickUI` without the matching `EndUIDrag` for a successful press.
- Do not overlap an existing InputSync drag.
- Cancel only cleanup state created by this action when release fails.
- Keep the coordinate, row 5, 90/adaptive count and fresh `FreeBagSpace` proof.
- Do not change F4 or any non-item sell stage.

## DEC-003 — Resolve Unity geometry per class across assembly layouts

- Date/version: 2026-08-20 / v0.6.1.2
- Status: RETAINED FOR DIAGNOSTICS / SUPERSEDED FOR ACTIVE ITEM-CELL DISPATCH BY DEC-004
- Decision: search CoreModule, UIModule and legacy UnityEngine.dll with an explicit class-specific order.

### Why

Runtime proves CoreModule exists but the v0.6.1.1 all-in-one class gate fails. `RectTransformUtility` is UI-related and may be split from core classes; older clients may instead use monolithic UnityEngine.dll.

### Constraints

- Do not change the coordinate, row, count or callback transport.
- Missing classes remain fail-closed.
- Diagnostics must name each missing class and available assembly.
- Passing this gate is not yet proof that a live item control exposes RectTransform.

## DEC-002 — Scoped fixed-slot internal callback for v0.6.1.1

- Date/version: 2026-08-20 / v0.6.1.1
- Status: SUPERSEDED BY DEC-004
- Decision: keep semantic shop stages, but let one captured coordinate select the current live item-cell callback at every Step 5 tick.

### Why this exception exists

The client knowledge base recommends instance-ID sale packets and normally rejects blind coordinate fallback. The user explicitly requested a one-fix v0.6.1 hotfix that restores the already understood v0.5 collapsing-cell behavior, without introducing item policy or packet-sale changes. The coordinate therefore selects a managed UI callback; it never produces Windows mouse input.

### Constraints

- Resolve fresh on every click.
- One callback per Bridge request.
- 90 first, then learned stable free space capped at 90.
- Ambiguous or missing hit fails closed.
- Do not change F4 or any non-item sell stage.

## DEC-001 — Resolve UI capabilities lazily

- Date/version: 2026-08-20 / v0.6.1
- Status: ACTIVE
- Decision: Separate `UIObject.instances` discovery from UIRect/Lua execution dependencies.

### Context

v0.6 treated seven managed components as one readiness condition. Runtime showed that one absent optional component disabled both Confirm and Auto Sell before any control could be inspected.

### Consequences

- UIButton and UIToggle remain usable without MonoBehaviourExecutor.
- UIRect failures cannot disable Button/Toggle actions globally.
- AUTO remains independently dependent on LuaSystemAPI_GUI, System.Object and Executor.
- Diagnostics expose capability boundaries and preserve fail-closed behavior.

### Alternatives rejected

- Hardcode only the donor RVA: build-specific and contrary to semantic resolution.
- Restore physical clicks automatically: hides the defect and retakes the mouse.
- Replace the whole action dispatcher in the same hotfix: too broad without the required CTS MainThread live proof.
