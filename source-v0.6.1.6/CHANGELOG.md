# CHANGELOG

## v0.6.1.6 — 2026-08-21

### Clean architecture
- Remove global hidden-input busy/owner/sequence lease state inherited from physical mouse serialization.
- Trade ordering is owned only by `tradeTxn_`/`tradeHeld`; unrelated SELL no longer stalls trade.
- REC is scoped to the captured PID or MAIN/CON pair instead of freezing all automation.
- Runtime priority is per account: P1 XN → P2 Revive → P3 AUTO.

### Movement safety
- `Mount` and `StartPath` share the same authoritative AutoFight-OFF precondition.
- Reuse existing Travel Guard: Stop #1 → Stop #2 → AUTO/Attack reset → repeat until OFF.
- Preserve Mount Recovery fight-10s stage, but every actual Mount dispatch is guarded OFF.

### Versioning / audit
- Protocol bump to `0x00010616`.
- Add `CanDispatchMovement()` pure guard test.
- Add `tools/verify_v0616_logic.py`.


## v0.6.1.5 — 2026-08-21

### Requested

- Remove every automatic Windows-mouse click that foregrounds a client, moves the cursor or consumes the user's mouse.
- Migrate the configurable MAIN/CON trade sequence to the same internal point mechanism proven by the 90-click item flow.
- Enforce the invariant that AutoFight must be authoritatively OFF before any AutoPath begins. Stop twice; if still ON, run one AUTO/Attack reset and repeat the stop cycle.

### Changed — hidden trade actions

- Delete the `SetForegroundWindow`, cursor-warp and `SendInput` path and its low-level user-mouse guard.
- Route sell-editor tests, trade-editor tests and every runtime trade row through one `CoordinatorInternalPointAction` primitive.
- Scale and normalize each saved F8 point, send `ClickInternalPoint`, and let `InputSyncManager.TryClickUI → EndUIDrag` perform the UI raycast/press/release inside the target client.
- Retain trade delays, repeats, grouped repeats, FIFO, sequence atomicity and stable `FreeBagSpace` business verification. The lease now protects business ordering only and never owns the Windows cursor.

### Fixed — AutoPath/AutoFight race containment

- Keep the existing Travel Guard sequence: Stop #1, Stop #2, AUTO/Attack reset once, then repeat Stop until a fresh snapshot proves OFF.
- Add a runtime conflict latch for any observed `AutoPath=ON && AutoFight=ON`. It stops AutoPath first and blocks normal/trade routing until both AutoPath and AutoFight have been observed OFF.
- Recheck authoritative AutoPath state both when an AUTO/Attack sequence is queued and again immediately before dispatch. Cancel a stale queued start if AutoPath became ON or unreadable.
- Tag P3 requests/results by workflow owner (Train, Travel Guard or Mount Recovery), preventing same-slot results from crossing state-machine ownership.
- Add the conflict latch to the central `StartPath` gate, trade rendezvous/sequence gates and steady-train busy policy.
- Bump matching EXE/DLL protocol to `0x00010615`.

### Verification status

- `tools/verify_v0615_logic.py`: PASS.
- Eight native C++ logic test groups: PASS locally.
- Windows x64 local cross-compile: PASS (PE32+ EXE/DLL).
- GitHub Actions MSVC x64 run 307 (`32427510483`): PASS, including verifier, build and all eight self-test groups.
- Artifact `9427940998`: `ThanLongItemConsolidator-v0.6.1.5-win-x64`, digest `sha256:dd21bf0d3f96a4e8b37d623cf5c54beb5a4e8f067033f5fb42f4261596e7988a`.
- Live-client trade/AUTO/conflict-recovery runtime remains unproven until the matching pair is tested in game.

## v0.6.1.4 — 2026-08-21

### Requested / runtime evidence

- User confirms the final v0.6.1.3 item path completed the full 90-click run extremely successfully.
- Separate AUTO runtime still fails while resolving the named Lua UI/action. The user requests reusing the proven point-dispatch mechanism for `AUTO (click 1) → ĐÁNH QUÁI (click 2)` and checking authoritative `AutoFight` afterward.

### Changed — P3 AUTO point action only

- Add generic Bridge command `ClickInternalPoint(x,y)` over the same complete `TryClickUI(0, point) → EndUIDrag(point)` lifecycle used by the runtime-passed item action.
- One queued Attack request now owns two ordered phases: configured `AUTO`, wait 500 ms, configured `ĐÁNH QUÁI`; completion is published only after click 2.
- `DỪNG AUTO 2` uses the same generic point command. Existing Travel Guard still blocks movement until a fresh snapshot proves `AutoFight OFF`.
- TEST buttons dispatch each of the three F8 points independently, so coordinates can be verified before enabling the scheduler.
- Bump matching EXE/DLL protocol to `0x00010614`.

### Protected / status

- v0.6.1.3 fixed-slot sell path, 90/adaptive count, F4/F8, XN, Revive, route, mount, rotation and MAIN/CON trade are unchanged.
- v0.6.1.4 scope audit and all seven pure test groups: PASS locally.
- Windows MSVC x64 build and live AUTO runtime are **UNTESTED** until their respective evidence exists.

## v0.6.1.3 — 2026-08-20

### Runtime evidence

- v0.6.1.2 reaches the fixed item stage and reports `Không có UIButton/UIRect callback tại tọa độ đã gán • geometry=149`.
- This proves the geometry resolver now passes, but the bag cell is not owned by an enumerated custom callback control.

### Fixed — item dispatch only

- Replace the active `FindControlAtNormalizedPoint → InvokeControl` item path with the exact current-client `InputSyncManager` EventSystem path.
- Perform a complete left click as `TryClickUI(0, point)` then `EndUIDrag(point)`; the native client uses the same down/up split.
- Reject a pre-existing InputSync drag, require the press to acquire a target, and verify release clears the drag state. Cancel only cleanup state created by the failed tool action.
- Keep the manually assigned coordinate and existing top-left-to-Unity coordinate conversion.
- Bump the EXE/DLL protocol to `0x00010613`.

### Protected

- First count remains 90; later count remains stable learned `FreeBagSpace`, capped at 90.
- Row selection remains v0.5 row 5 for profiles with at least five rows.
- Semantic shop stages, close UI, F4/F8, AUTO, Revive, Confirm, travel, train, rotation and trade logic are unchanged.
- No foreground switch, cursor movement, `SendInput` or physical fallback in the item stage.

### Build / runtime

- Exact-client metadata/native dispatcher analysis: PASS against client-data commit `f0c37b7745be47e185376358c1a51ebaa376475a`.
- Windows MSVC x64 CI run 306 (`32418075439`) on source commit `cfaafc210aede366577da4af49f161545824fea3`: PASS with scope audit and seven self-test groups.
- User live report 2026-08-21: full 90-click item run is **RUNTIME PASS**. Exact per-click log was not attached, so this evidence proves the reported end-to-end run, not every intermediate counter string.

## v0.6.1.2 — 2026-08-20

### Runtime evidence

- v0.6.1.1 reaches the Equipment tab and then reports `callback ô lỗi 6/6`.
- Final detail is `Thiếu RectTransform/Utility/Screen để hit-test ô cố định`.
- Source proves this failure occurs before coordinate hit-testing and before any item callback. Row 5 selection, click count and learned repeat are not implicated.

### Fixed — geometry resolver only

- Stop requiring every Unity geometry class to exist in `UnityEngine.CoreModule`.
- Open `UnityEngine.CoreModule`, `UnityEngine.UIModule` and legacy `UnityEngine.dll` independently.
- Resolve each required class through an explicit ordered search. `RectTransformUtility` checks UIModule first; core classes check CoreModule first; every class has legacy fallback.
- Replace the aggregate message with the exact missing class list plus assembly availability.
- Bump the EXE/DLL protocol to `0x00010612`.

### Protected

- Fixed item coordinate remains v0.5 row 5 for every profile with at least five rows.
- First count remains 90; later count remains stable learned `FreeBagSpace`, capped at 90.
- Semantic shop stages, close UI, F4/F8, AUTO, Revive, Confirm, travel, train, rotation and trade logic are unchanged.
- Runtime remains **UNTESTED** until the user tests the matching v0.6.1.2 EXE/DLL pair.

### Build / runtime

- Windows MSVC x64 CI run 302 (`32414440608`) on source commit `71ef6761`: PASS.
- Scope audit plus route, rotation, trade coordinator, background UI scoring, fixed-slot sell and Unity geometry resolver self-tests: PASS.
- Release ZIP is the unmodified CI artifact `9423369955`; hashes are recorded in `release/SHA256SUMS_v0.6.1.2.txt`.
- Runtime: **RUNTIME UNTESTED** until the user tests the matching v0.6.1.2 EXE/DLL pair.

## v0.6.1.1 — 2026-08-20

### Requested / confirmed

- User runtime confirms the internal shop chain reaches and selects `Trang bị`; only the following item-cell stage is wrong.
- Source comparison confirms v0.6.1 did **not** retain active v0.5 adaptive Step 5. It kept only an unused learned-repeat field while replacing coordinate Step 5 with item-control name/ancestor enumeration.

### Fixed — item stage only

- Keep `ClickNPC → shop → Bán vật phẩm → Bán nhanh → Trang bị` and close-UI actions unchanged.
- Use one captured item coordinate: v0.5 row 5 when present, otherwise the last configured row (including a one-row profile).
- Normalize the client point, resolve the current live control under it via Unity RectTransform hit-testing, and invoke exactly one `UIButton`/`UIRect` callback per Bridge request.
- Resolve again after every sale so the collapsing inventory never reuses a cached transient pointer.
- First session uses 90 callbacks; later sessions use stable post-sale `FreeBagSpace`, capped at 90 and retained for the current tool process as in v0.5.
- Keep the configured row delay between callbacks. No foreground switch, cursor movement or Windows mouse injection occurs in this stage.
- Bump the EXE/DLL protocol to `0x00010611`.

### Protected

- F4/F8, AUTO, Revive, Confirm, travel, train, rotation, trade coordinator and physical MAIN/CON trade code are unchanged except version text/build wiring.
- Runtime remains **UNTESTED** until the user tests the matching v0.6.1.1 EXE/DLL pair.

### Build / runtime

- Windows MSVC x64 CI run 295 (`32402322381`) on source commit `930ceee7`: PASS.
- Scope audit plus route, rotation, trade coordinator, background UI scoring and fixed-slot sell self-tests: PASS.
- Release ZIP is the unmodified CI artifact `9418953183`; hashes are recorded in `release/SHA256SUMS_v0.6.1.1.txt`.
- Runtime: **RUNTIME UNTESTED** until the user tests the matching v0.6.1.1 EXE/DLL pair.

## v0.6.1 — 2026-08-20

### Requested / runtime evidence
- User runtime log reported both Lâu Lan internal confirm and background-sell UI opening failing with `Không resolve đủ UIObject/UIButton/UIToggle/UIRect/Executor`.
- v0.6 is therefore `RUNTIME FAIL` for those two paths; AUTO/Revive remain unproven, not implicitly failed or passed.

### Root cause
- CONFIRMED: v0.6 basic UI enumeration required every Lua/Executor dependency even though UIButton and UIToggle callbacks do not use them.
- UNKNOWN: the old aggregate message cannot identify which exact dependency was null on the live client.
- LIKELY: the guessed `FGStudio.LuaSystem.MonoBehaviourExecutor` namespace was the null dependency. The donor proved a native RVA, not that managed namespace; the old aggregate error hid the exact member.

### Fixed
- Split Bridge readiness into basic UI discovery and lazy Lua/Executor capabilities.
- Basic scanning now requires `UIObject.instances` plus at least one available control class, not Button+Toggle+Rect+Executor+GUI+System.Object simultaneously.
- Known namespaces remain the fast path, but UIObject/Button/Toggle/Rect also get a bounded simple-name metadata fallback and are accepted only after their required field/method surface validates.
- UIButton/UIToggle callback paths never load Executor.
- UIRect and TopIcon Lua actions load their extra dependencies only when selected.
- Resolve `MonoBehaviourExecutor` through namespace candidates, then bounded Assembly-CSharp metadata enumeration; accept only a class with static `get_Instance()` and instance `ExecuteScriptFunction(3)`.
- Replace the aggregate resolver error with exact missing-component diagnostics and report available B/T/R control capabilities when no semantic candidate is found.
- Bump controller/Bridge protocol to `0x00010601` so v0.6 DLL mismatch fails closed.

### Preserved
- v0.5 FSM/business logic, Lâu Lan route gate, death flow, sell phases, FreeBag verification, trade macro, F4/F8, timing and unrelated behavior are unchanged.
- No `CreateRemoteThread`; no fallback to coordinate clicks.

### Build / runtime
- Local capability audit and all four pure logic self-test groups: PASS.
- Windows MSVC x64 CI run 274 (`32387856270`) on source commit `e90e08c`: PASS, including artifact staging.
- Release ZIP is the verified artifact from CI run 276 (`32388213200`), source commit `3cfa9347`, artifact `9413784891`; ZIP/EXE/DLL hashes are recorded in `release/SHA256SUMS_v0.6.1.txt`.
- Runtime: **RUNTIME UNTESTED** until the user retests with the v0.6.1 EXE/DLL pair.

### Next version notes
- Test XN and Mã Kiêu Minh first. Test AUTO separately because it legitimately needs Lua Executor even if UIButton-based actions pass.
- Treat `System.Action -> MainThread.Execute` CTS live proof as separate architectural work; do not silently combine it with this narrow resolver hotfix.

## v0.6 — 2026-08-20
- Replaced active XN Lâu Lan physical click with a MessageBox-scoped internal callback; preserved the exact v0.5 route-ownership and 3-second stall gate.
- Replaced active Đầu thai physical click with a Bridge action that rechecks `IsDeath` immediately and invokes the unique revive control.
- Replaced AUTO menu/Attack/StopAuto2 coordinate chains with exact `TopIcon.AutoTrainClick` / `TopIcon.AutoStopClick` Lua actions; retained authoritative snapshot verification and Travel Guard.
- Replaced active recorded auto-sell macro with a non-blocking semantic Bridge sequence and per-item `FreeBagSpace` feedback. Three no-progress attempts skip an item; 90 callbacks is the hard ceiling.
- Kept the v0.5 Bridge architecture and moved semantic actions onto the target game message thread; did not transplant donor v0.8.4's persistent remote thread.
- Preserved coordinate-based MAIN/CON trade sequences, World Flow/FIFO, route/mount/rotation, F4/F8 and unrelated profile behavior.
- Added donor analysis, v0.6 verifier and pure UI scoring self-test.

## v0.5 — 2026-08-20
- Fixed World Flow/BĐPT HOLD so held MAIN/CON accounts are movement-observed before P1/P2/P3 instead of losing the Lâu Lan stall watchdog.
- Fixed held-account death lifecycle: P2 Đầu thai preempts World Flow; travel/rendezvous coordinator pauses without dropping FIFO/HOLD, then held travel restarts cleanly after ALIVE. Atomic trade click Sequence safety is unchanged.
- Preserved tool-owned cross-map route ownership/evidence across intermediate MapID changes, so entering Lâu Lan no longer disarms the gate watchdog.
- Tightened Lâu Lan condition to require current AutoPath=ON plus >=3s position stall.
- Removed the v0.4 background `PostMessageW` XN implementation from active runtime. P1 XN now foregrounds the target game, moves the real cursor and clicks through `SendInput`, borrowing/restoring any existing click-sequence lease for exactly one click.
- User Mouse Guard 5s, periodic-XN removal, route/click separation, Travel Guard, mount recovery, Adaptive Sell Step 5 and protected Sell/Trade/Rotation rules retained.

## v0.4 — 2026-08-20
- Removed scheduler-wide click freeze: physical click sequences keep only a click lease; bridge AutoPath/Mount/Dismount continue in parallel between clicks.
- Removed periodic `Xác nhận ra Map mỗi N giây` runtime/config/UI while preserving Confirm coordinate/capture/manual test.
- Added Lâu Lan-only (`MapID=5`) cross-map stall watchdog: after observed AutoPath movement, >=3s position stall triggers a hidden/background Confirm `PostMessageW` click.
- Added global User Mouse Guard: physical mouse activity pauses all automatic clicks for 5s from the latest input without pausing scanner/FSM/route/mount.
- Sell/Trade/P3 retain exact click step/request if mouse activity is detected during click preparation.
- v0.3 AutoFight Travel Guard, mount recovery, Adaptive Sell Step 5, Sell/Trade/Rotation business rules retained.

## v0.3 — 2026-08-20
- Added one fail-closed AutoFight Travel Guard before every StartPath.
- Removed duplicated/fail-open stop-Auto movement logic from Sell, Train Recovery, Trade Rendezvous and M87.
- Added global P3 AUTO UI barrier below XN and Đầu thai; P3 preserves SELL/GD sequence lease state.
- Restored requested mount recovery: Mount x2 -> fight 10s -> stop -> Mount x2 -> foot 15s -> repeat.
- Kept Adaptive Sell Step 5 and unrelated business logic unchanged.
- Added living `TOOL_LOGIC_TABLE.html`.

## Historical changelog from pre-v0.3 source
 — HISTORICAL RECORD

> This file intentionally mentions old behavior from old versions. It is **not** the current runtime specification. For current R13 behavior, read `PROJECT_KNOWLEDGE.md` and `README.md`.

## v0.2.7-R6
- SELL click sequence is atomic relative to trade workflow: trade coordinator yields for the entire sell macro and final delay.
- SELL acquires its sequence lease immediately after successful ClickNPC.
- Added `AUTO` / `GIỚI THIỆU` tab navigation.
- About text: `Thiết kế và phát triển bởi Thắng Nguyễn - ĐỒ LONG`.
- R5 trade target/FIFO and all protected behaviors retained.

## v0.2.7-R6 — 2026-08-19

### Requested / changed
- Each CON now has a per-account `GD đến trống ≥ N` target, default 30 and persisted by profile.
- A FULL CON still enters the trade workflow only at 0 free slots, but after each complete shared trade-click sequence the same CON remains active and repeats the entire sequence while `FreeBagSpace < N`. It is released back to normal train only when `FreeBagSpace >= N`.
- The final click delay is honored before evaluating the bag snapshot, reducing stale post-confirm reads.
- Simultaneous FULL admissions are deterministic by child number (CON1 before CON2 ...); after admission the queue remains strict FIFO by workflow-entry time, so an already-staged CON3 stays ahead of a later CON1.

### Protected
- R4 queue max 3, one trade sequence at a time, AutoPath freeze exemption, F8 capture fix, F4, two raw SendInput sites, MAIN sell threshold 6, FULL-only entry threshold 0, drag/group/rotation/train/sell/revive/route logic remain unchanged outside the requested integration points.

### Verification
- Local R5 static scope audit: PASS.
- Windows GitHub CI: pending at package-prep stage.
- Runtime: **RUNTIME UNTESTED** until live trade test.

## v0.2.7-R4 — 2026-08-19

### Requested / changed
- Stage up to 3 FULL children at TỌA GD while keeping the actual trade click sequence strictly sequential.
- Long Freeze All now begins only when MAIN + active child are at TỌA GD and the trade click sequence starts; periodic Auto XN map remains available during travel/wait.
- Mouse-drag selects contiguous trade-sequence rows.
- Rotation is disabled with only the combo-selected map and activates only when a second map is manually checked; combo selection resets the checked pool.

### Protected
- F4 byte-identical to R3, one F4 registration, two raw SendInput call sites, MAIN threshold 6, child FULL threshold 0.
- Existing DỒN ĐỒ, R3 multi-delete, group repeat, REC/sell/train/route/revive and unrelated core behavior remain outside scope.
- No v0.2.8/v0.2.9 code imported.

### Verification
- Local exact patch reproduction + static audit: PASS.
- GitHub Windows CI workflow committed, but completed CI run not observable when archive finalized; CI PASS not claimed.
- Runtime: **RUNTIME UNTESTED**.

## v0.2.7-R3 — 2026-08-17

### Requested
- Only change the trade click-sequence editor so `- XÓA` deletes all currently selected rows instead of only one focused row.
- Keep every other runtime feature/code path unchanged.

### Changed
- `DeleteTradeSequenceRow()` now uses the editor's existing multi-selection (`SelectedRows`).
- Selected rows are deleted from highest index to lowest index.
- When deleting multiple rows from `CHUỖI GD MAIN`, shared ACC CON `MAIN #n` references are repaired across all removed MAIN rows.
- Single-row delete remains supported by the same code path.

### Protected / Unchanged
- F4, DỒN ĐỒ, TỌA GD rendezvous, group repeat, AutoFight fallback, five-click model, BĐPT/REAL INPUT, sell/REC/train/route/revive logic.
- No v0.2.8/v0.2.9 code imported.

### Source
- R2 base controller: `de141e34f07903c3e490d9684410309f4e0d3a49d7e36438b76a9e941e8cd6e2`.
- R3 controller: `a69fa0df4932e4020aed6e61b4109bd2c558db5c407afedb46c03456fb575abf`.
- R3 patch: `3b013821934c882cce8dc755894f66ab835feec394d3433015127a8792fc2136`.
- CI #60 / run `32048547405`: FAILED before compile because Windows checkout changed patch transport EOL; runtime source not implicated.
- CI #61 / run `32048648947`: **CI PASS / BUILD PASS** including rehydrate, x64 build, route/rotation/trade tests, static multi-delete-only audit and artifact upload.
- Runtime status: **RUNTIME UNTESTED** until live multi-delete is tested.

## v0.2.7-R2 — 2026-08-17

### Requested
- Verify `DỒN ĐỒ: BẬT/TẮT` actually gates consolidation.
- FULL CON immediately holds MAIN+CON and sends both to one user-captured TỌA GD.
- First arrival waits; both must be confirmed at TỌA GD before existing trade clicks.
- Add grouped mini-sequence repetition for one or more consecutive trade rows.
- Remove all per-CON selector coordinates.
- AutoFight-stop failure must not deadlock movement; retry after map transition.
- Merge old AUTO + DỪNG AUTO 1 into one `AUTO` point.
- Preserve F4 exactly.

### Added / Changed / Fixed
- Added global TỌA GD capture/persistence and dedicated `tradeTravel*` rendezvous state.
- Both transaction participants are `tradeHeld` immediately on FULL CON selection.
- Added first-arrival hold/StopPath behavior and both-arrived readiness gate.
- Added group metadata/editor/runtime loop while preserving row repeat.
- Removed `tradeSelectPoint`, `TradeSelect*` persistence/UI/runtime.
- Per-account click array changed 6→5; `AUTO` replaces old duplicated StopAuto1 point.
- Train/sell/M87/trade movement can continue after bounded AutoFight-stop retries and retry after map change.
- `ReleaseTradeHolds()` now cleans all rendezvous holds/state.
- Restored untouched clean-v0.2.7 `PeriodicConfirmBusy()` and `HandleFightClicks()` after CI detected they had been accidentally deleted during the first edit pass.

### Build / CI
- Local clean-base patch chain: PASS, final controller `de141e34f07903c3e490d9684410309f4e0d3a49d7e36438b76a9e941e8cd6e2`.
- CI #55: FAILED before build due Windows EOL checksum mismatch; corrected by LF normalization.
- CI #56: rehydrate/configure PASS, BUILD FAILED due accidental removal of two untouched clean helpers; corrected by byte-exact restoration.
- CI #57 / run `32043612053`, job `95427111307`: **CI PASS**, including Windows x64 Build Release + route/rotation/trade tests + static requested-only audit + artifact upload.
- Runtime remains **RUNTIME UNTESTED**; CI does not upgrade it.

### Runtime
- Status: **RUNTIME UNTESTED**.
- Protected evidence: F4 behavior from clean v0.2.7 is user-confirmed useful; exact pause block is byte-protected.

### Next Version Notes
- First collect live evidence for rendezvous/map transitions/group repeat/DỒN OFF/F4 before redesigning any state machine.

## v0.2.7
- Replaced per-CON trade workflows with one global `CHUỖI GD ACC CON` shared by CON1..CON6.
- Runtime binds CON-targeted rows to active transaction child and retains shared MAIN references.
- DỒN ĐỒ ON/OFF, BĐPT, REC, sell FREEZE and donor core were preserved.

## v0.2.6
- Added `DỒN ĐỒ: BẬT/TẮT` and independent auto-train/sell while OFF.
- Added whole sell-sequence cloning.

## v0.2.7-R7
- AUTO XN MAP becomes global highest-priority automatic physical click via scheduler pre-pass; interrupted SELL/TRADE/AUTO click state resumes immediately after the XN click with no XN post-delay.
- Added immutable workflow-entry tickets and Rendezvous FIFO relock so an earlier workflow child cannot be overtaken by a later traveler; same-batch ties preserve lower CON number first.
- Preserved R6/R5 behavior outside this scope.

## v0.2.7-R8
- XN MAP is now global-pass-only and acts as an all-window auto-click barrier.
- Only the due/eligible target game window receives its own Confirm coordinate.
- A window currently inside SELL/TRADE/AUTO UI click sequence defers its own XN; no Confirm injection into that busy UI.
- Other windows resume their existing sequence state immediately after the XN click.

## v0.2.7-R9 — Đầu thai global barrier cấp 2
- Giữ nguyên toàn bộ R8 ngoài đúng click Đầu thai.
- XN map vẫn ưu tiên cấp 1.
- Đầu thai trở thành ưu tiên cấp 2 toàn cục: pause mọi auto click cửa sổ khác, click đúng cửa sổ acc chết, rồi resume ngay.
- Death-session được prime trong global pre-pass để không bị sequence lease của cửa sổ khác làm chậm phát hiện chết.
- Giữ nguyên 500ms / 5000ms / 900ms / 4500ms của flow Đầu thai cũ.

## v0.2.7-R11
- Removed hidden MaxTransferClicks INI cap from active trade runtime; legacy key is normalized to 90 only for compatibility.
- Disabled hidden scheduled shutdown and clear stale ShutdownEnabled at startup.
- Added bounded MAIN FreeBagSpace stabilization after each complete trade pass before applying R10 <=8 received-slot heuristic.
- Trade sequence now requires at least one real CON CHUYỂN ĐỒ row to be READY.
- Protected R10/R9/R8/R7/R6 behavior unchanged outside these four runtime-hardening points.

## v0.2.7-R12
- Removed dead `CON GD đến trống ≥ N` UI/profile setting.
- DỒN ĐỒ OFF now respects each account's Auto Sell checkbox; DỒN ĐỒ ON MAIN/CON sell semantics remain coordinator-owned.
- Removed legacy trade macro subsystem, dead shutdown/confirm/transfer-cap state and obsolete TradeTxn fields.
- Cleaned bridge protocol baggage and bumped protocol to 0x00010503.
- Moved historical source snapshots and macro files out of active `src/`.
- Replaced detached toy trade test with shared runtime coordinator decision helpers + real unit coverage.
## v0.2.7-R13 — strict source cleanup + Auto Sell master switch
- Auto Sell checkbox is authoritative in every mode: unchecked blocks automatic selling; checked permits existing thresholds/role rules.
- Removed the remaining legacy `.macro` archive and legacy trade-macro header from the distributable source package.
- Removed duplicate historical controller/base-source implementations from the distributable package; Git/patch lineage remains the history source.
- Old version notes/manifests moved under `history/version-notes/`; current root identity is R13 only.
- Re-audited absence of dead shutdown, fixed-CON-target, legacy trade-macro runtime and childTrigger UI symbols.
