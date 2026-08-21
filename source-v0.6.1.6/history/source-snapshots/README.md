# Historical source snapshots are not shipped in R13

R13 intentionally contains exactly one active implementation of the controller: `src/controller.cpp`.
Old `controller_*.cpp`, pre-R12 trade-logic copies, and other duplicate source snapshots were removed from the distributable source package because they caused grep/AI/editor ambiguity.

Historical reconstruction remains available through:
- `CHANGELOG.md`,
- the versioned patch lineage under `hotfix/` / `patches/`,
- GitHub Draft PR history in `ngmthang-g/Auto-than-long-phien-ban-cu`.

Nothing under this directory is compiled.
