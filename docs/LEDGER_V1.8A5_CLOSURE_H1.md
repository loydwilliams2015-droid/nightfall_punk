# nightfall!punk v1.8A.5 — Integrated Physical World: H1 closure decision

## Locked decisions
- Preserve v1.8A.3 Model C: swept capsule + independently validated flat foot support + geometry AND traversal simultaneous jurisdiction; laboratory-only.
- Preserve v1.8A.4 Model E: bounded motor-driven dynamic bodies + reciprocal angular/friction impulses + adaptive 2→4→6 contact solve; laboratory-only.
- **CLOSE v1.8A.5 at H1 integrated laboratory evidence.** Selected grid D = canonical 1 m three-dimensional cells with actual selectively loaded finer voxels and safe pending; selected history D = consecutive applied receipts → summaries → threshold events → bounded acknowledged outbox.

## Locally implemented closure (SOURCE IN DOWNLOADABLE ARCHIVE ONLY)
- `nf_contact18a5_close.h/.c`: valid fine-data callback with material/cache revision separation. Check *entire proposed and realized upright capsule sweeps* against canonical and fine voxel solids, not simply a clear point. Missing/stale/malformed fine returns PENDING; material obstruction returns BLOCKED. Actual 1.8A.4 translating capsule↔box contact changes BOTH velocities before the same copy-on-write world commit ingests the applied contact receipt.
- Continuous support: apply the authoritative gravitational acceleration to dynamic actor, resolve physical upward contact with 1.8A.4 island solver, verify actor/support ID, support top, contact position, grounded/material/stable contract, foot patch, tick/world version; summarize applied support impulse/dt as force over time, not a fake collision impact.
- `nf_contact18a5_wal.c`: local one-process, same-ABI binary snapshot WAL for paired dynamic bodies + canonical/cache grid + contact history/outbox; fsync snapshot and containing directory BEFORE memory publication, local file lock, monotone revision and checksum, fail-closed torn/corrupt/duplicate-conflicting replay. Existing event sink survives append-before-ACK replay and refuses outbox overwrite.
- No authority granted by a pending cell, no lost committed body/history state on disk or queue failure, no false resting support.

## Local evidence (not GitHub CI)
- 46/46 strict C11 integration gates PASS; 83/83 inherited A5 core assertions and 9/9 sink assertions; prior A1–A4 regressions PASS.
- 4/4 offline CMake/CTest targets PASS; ASan/UBSan integration PASS.
- 1,400 paired cases (7 deliberately adversarial strata × 200 nearby parameter variations); 400/400 false point-only FREE classifications against known adjacent obstructed capsule sweeps prevented by full volume; 400/400 valid loaded/preloaded geometry leads to committed real two-body impulse; missing/malformed returns PENDING 400/400; stale version returns STALE 200/200; zero sampled cache-only material epoch changes. CSV byte-repeat SHA256 on same host.
- Archive independently extracted and 46/46 closure assertions reproduced.

## Critical scope boundaries
The GitHub branch is a HANDOFF/LEDGER branch, not the compiled source graft. Tested code and PAR were generated in the conversation as `nightfall-v18a5-closure-source.tar.gz`. The current GitHub repository source tree still needs that graft and GitHub CI before calling it repository-certified. H1 only: upright capsule vs translating AABB; static voxel sweep is conservative and uses bounded scans, not performance-qualified spatial acceleration. Absent *canonical* chunks still return PENDING (the new fine loader only operates under a present authoritative canonical parent). It does not prove a live player controller, rotating geometry, full Cell/Nexus semantic commit, multiwriter portable WAL, hardware power-loss certification, client/server reconciliation, or H4 feel.

## 1.8A.6 inheritance
Promote only after graft, authoritative world tick/player controller integration, native hardware tests of load latency/P95/P99, optimized chunk broadphase, arbitrary moving supports, persistent contact manifolds, portable authoritative WAL and network reconciliation. Do not portray H1 fixture prevalence as actual game-world failure rates.

## Industry comparison
Box2D surfaces post-step touch/hit events; Rapier computes thresholdable contact force events following physics solve. Our project-specific stricter law is material-and-traversal dual jurisdiction with applied-contact provenance before long-term history promotion.
