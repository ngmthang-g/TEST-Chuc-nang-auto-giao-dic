# VERSION v0.6.1.5

Date: 2026-08-21

## Scope

Remove the remaining automatic Windows-mouse transport from configurable trade actions and harden the invariant that AutoFight must be OFF before AutoPath.

## Input research

- User-supplied `hướng dẫn làm click ẩn thần long(1).md`.
- Runtime-passed v0.6.1.3 fixed-item 90-click path.
- Existing generic `ClickInternalPoint` command introduced in v0.6.1.4.
- Existing authoritative Travel Guard in `EnsureAutoFightOffForTravel`.

## Hidden-action transport

- Deleted the controller path that foregrounded a game window, moved the cursor and emitted Windows mouse input.
- Deleted the low-level user-mouse guard that existed only to coordinate that physical transport.
- Added `DispatchInternalPointActionDirect` and `CoordinatorInternalPointAction`.
- Sell/trade editor tests and every runtime MAIN/CON sequence row now scale + normalize the saved F8 point and call `ClickInternalPoint`.
- Bridge retains the proven fail-closed lifecycle: no pre-existing UI drag, `TryClickUI` must acquire a UI target, `EndUIDrag` must release it, and cleanup is bounded.
- Delay, repeat, group repeat, FIFO, sequence pass and stable MAIN `FreeBagSpace` verification remain controller business state.

## AutoPath/AutoFight invariant

- Travel Guard still performs Stop #1, Stop #2, then one AUTO/Attack reset if AutoFight remains ON, and repeats until an authoritative OFF snapshot.
- `travel_fight_guard_logic.h` exposes the two-stop policy and conflict truth table for pure testing.
- A new runtime latch detects any `AutoPath=ON && AutoFight=ON` snapshot.
- Queued AUTO/Attack work rechecks authoritative AutoPath at queue time and dispatch time; a stale request cannot turn fight on after travel starts.
- The P3 mailbox carries a workflow owner on both request and completion, so Train cannot consume a Travel Guard or Mount Recovery result with the same click slot.
- Conflict recovery stops AutoPath first, then reuses Travel Guard, and clears only after both AutoPath and AutoFight are observed OFF.
- Central StartPath, trade rendezvous and active trade sequence are fail-closed while recovery is latched.

## Version and compatibility

- Application version: `0.6.1.5`.
- Protocol: `0x00010615`.
- Artifact: `ThanLongItemConsolidator-v0.6.1.5-win-x64`.
- v0.6.1.5 EXE and DLL must stay together; older pairs are rejected.

## Local verification

- `python3 tools/verify_v0615_logic.py`: PASS.
- Eight native C++ logic groups with strict warnings: PASS.
- `git diff --check`: PASS.
- LLVM-MinGW Windows x64 cross-build: PASS; EXE and Bridge are PE32+ x86-64.
- GitHub Actions Windows MSVC x64 run 307 (`32427510483`): PASS for verifier, build and all eight self-tests.
- Artifact `9427940998`, digest `sha256:dd21bf0d3f96a4e8b37d623cf5c54beb5a4e8f067033f5fb42f4261596e7988a`.

## Runtime status

- Hidden item path: inherited RUNTIME PASS evidence from v0.6.1.3.
- Hidden AUTO path: RUNTIME UNTESTED for this exact version.
- Hidden MAIN/CON trade: RUNTIME UNTESTED.
- AutoPath/AutoFight conflict recovery: RUNTIME UNTESTED.
- GitHub Actions MSVC artifact: BUILD PASS; this still does not prove live game behavior.
