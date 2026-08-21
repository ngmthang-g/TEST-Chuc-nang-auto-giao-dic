# SOURCE PROVENANCE — v0.6.1.6

- Direct base: user-provided `TEST-chuc-nang-than-long-2-codex-v0.6.1.5-hidden-trade-autopath-guard.zip`, corresponding to GitHub branch `codex/v0.6.1.5-hidden-trade-autopath-guard`.
- Scope: architecture cleanup approved by user + new invariant requiring AutoFight OFF before both Mount and StartPath.
- No donor/client binary or new RVA introduced. Bridge InputSync transport is unchanged; controller scheduling/guard logic is cleaned.
- Target branch: `codex/v0.6.1.6-per-client-clean-movement-guard`.


- Direct v0.6.1.5 base: local commit `5404f5ddbeedf47e8acec58f834b71316119dfcf` on `codex/v0.6.1.4-internal-auto-points`.
- Requirement input: attached `hướng dẫn làm click ẩn thần long(1).md`, derived from the supplied v0.6.1.3 source, runtime-successful 90-click behavior and the exact-client `InputSyncManager` evidence already recorded in this repository.
- v0.6.1.5 copies no client binary, metadata blob, native RVA or donor worker. It reuses the existing metadata-resolved generic point command, removes Windows mouse injection from the controller, and adds controller-only route/fight safety state.
- Local acceptance before GitHub publish: hidden-action source audit PASS, eight native pure-test groups PASS, `git diff --check` PASS and LLVM-MinGW Windows x64 PE cross-build PASS. These results do not claim live-client success.
- GitHub publish: branch `codex/v0.6.1.5-hidden-trade-autopath-guard`, source commit `ffc376559dd0aee6ce3966f7f7c93a828c33e465`, exact local/remote tree `b350db03588e2317a2e03e5b706b35ac9c695304`.
- Windows MSVC x64 run 307 (`32427510483`) passed verifier, build and all eight self-tests. Artifact `9427940998` has digest `sha256:dd21bf0d3f96a4e8b37d623cf5c54beb5a4e8f067033f5fb42f4261596e7988a`; extracted EXE hash is `6d2bdfd6015ab6a6b9c503a8f3329228d73d556a31f4059a54b3927d39046c51`, DLL hash is `da1febd514af0951ebcab56de575dfa794f00ee99614157684a456d65faf554c`.

- Direct v0.6.1.4 code base: GitHub source commit `cfaafc210aede366577da4af49f161545824fea3` on `codex/v0.6.1.3-internal-ui-point-click`. The user-supplied final v0.6.1.3 source archive has identical code files and adds the completed CI documentation from run 306.
- Runtime input for v0.6.1.4: the user confirms the v0.6.1.3 fixed-item InputSync path completes all 90 clicks successfully, while carried AUTO runtime reports named Lua UI/action resolution failure.

- Direct v0.6.1.3 hotfix base: final packaged v0.6.1.2 commit `674cecca39fc92f776f8184c509a85c34fd80d31`, tree `132b817d1a1da3366a54fed9f6bd63891a2511b8`.
- Direct v0.6.1.2 hotfix base: final v0.6.1.1 tree `98b4b1e3302da628df3e3d949faebe297223d3d4` / GitHub commit `c4433ed063392bcacdbb0344599bde594403f6dc`.
- Direct hotfix base: exact v0.6.1 release tree `fd55f16c6415e56469867ff55902c7e73756c4e2` / GitHub commit `1048c2064ec6be05756e25aa8a0adc7893df3f38`.

- Direct base: attached `ThanLongItemConsolidator_Source_v0.5(2).zip`, SHA-256 `d84b0f57a7da17e549c85e793232b37b5f8a518fe58f494d97fea4fd7393126d`.
- Base source passed its supplied `tools/verify_v05_logic.py`; supplied EXE/DLL matched `BUILD_HASHES_v0.5.txt`.
- Donor reference: attached `ThanLongAutoTrain_Source_v0.8.4 hoan thanh chu trinh ban do.zip`, SHA-256 `fdc93bb6653418705b058cd2c2a2b56a820351a2d9bd38c85ce062a51fabac86`.
- Donor code was analyzed for UI discovery, callback invocation, treatment, selling, MessageBox confirmation, revive and TopIcon AUTO actions.
- v0.6 did not copy donor `remote_worker.S` or `CreateRemoteThread` transport. It implemented the learned semantic-action model inside the existing v0.5 Bridge.
- v0.6.1 is a direct hotfix on v0.6 using user runtime evidence EVID-001. It does not import new donor code or alter v0.5 business-state logic.
- The donor established the Executor's native action surface/RVA, not the managed namespace assumed by v0.6. v0.6.1 therefore uses validated metadata discovery and records uncertainty explicitly.
- GitHub Actions Windows x64 plus `tools/verify_v061_logic.py` and all self-tests are the build acceptance gate. Only user live-client evidence may promote runtime status.
- The checked-in v0.6.1 ZIP is the unmodified artifact from source commit `3cfa9347`, CI run `32388213200`, artifact `9413784891`; its GitHub digest and extracted binary hashes were independently rechecked before packaging.
- v0.6.1.1 does not import v0.6.2 changes. It changes only the v0.6.1 item-cell stage, protocol/version/test/document surfaces required to ship the matching EXE/DLL pair.
- The knowledge repository `ngmthang-g/clinent-game-than-long-DATA-2222` was consulted for Quick Sell and bag-grid semantics. It confirms Quick Sell redirects each bag-cell click to the normal one-item sell request and that inventory UI refreshes after removal. Its recommended instance-ID packet seller was deliberately not introduced in this scoped hotfix.
- The checked-in v0.6.1.1 binary ZIP is the unmodified artifact from source commit `930ceee7d857254770da32b74aff658c42047237`, source tree `9f09f444df035f41f6700774e47e6572a8ae8aeb`, CI run `32402322381`, artifact `9418953183`. GitHub digest and extracted PE32+ x64 EXE/DLL hashes were independently rechecked before packaging.
- v0.6.1.2 is a direct narrow hotfix on the final v0.6.1.1 tree. It changes only Unity geometry assembly/class resolution, exact diagnostics, protocol/version and the required test/document/build surfaces. It does not import donor code or alter the fixed-slot/90/adaptive business logic.
- The checked-in v0.6.1.2 binary ZIP is the unmodified artifact from source commit `71ef67610e1b8cb0b14fa357ff50f65ce98adff1`, source tree `1c91bcbdba96e85f647b52f3aa21abde86fda1a4`, CI run `32414440608`, artifact `9423369955`. GitHub digest and extracted PE32+ x64 EXE/DLL hashes were independently rechecked before packaging.
- v0.6.1.3 uses the public client-data repository only as research evidence, at commit `f0c37b7745be47e185376358c1a51ebaa376475a`. Metadata and native call sites establish the `InputSyncManager` press/release surface. No client binary, metadata blob, hardcoded RVA or donor worker is copied into this repository.
- v0.6.1.3 changed only the item-cell dispatcher plus matching protocol/version, tests, documentation and release wiring. Fixed row/count/adaptive logic and all unrelated action FSMs remained from v0.6.1.2.
- v0.6.1.4 adds no new client binary research and copies no binary/code from the data repository. It promotes the existing metadata-resolved InputSync dispatcher to one generic point command and changes only active P3 AUTO point sequencing, matching tests/version/docs/build wiring.
