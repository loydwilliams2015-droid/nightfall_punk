# nightfall!punk 1.8A.5 — Canonical Grid / Condensed History H1 handoff

## Inherited lock
**LOCKED:** 1.8A.4 Model E — motor-driven dynamic interaction with reciprocal angular/friction impulses and bounded adaptive solver — accepted as H1 laboratory exit candidate, NOT production physics. 1.8A.3 Model C remains the previous H1 capsule, flat-foot support, dual geometry/traversal jurisdiction candidate.

## H1 selected conditional exit candidates
- GRID: authoritative 1 m canonical **3D** material voxels, with 4 m chunks; 0.5 m / 0.25 m fine voxel witnesses loaded when needed. Canonical FREE/SOLID must agree with finer representations; MIXED cannot be called FREE without current valid fine data. Missing fine = PENDING, not false clearance. The H1 cache candidate is *hysteretic selective refinement* with material revisions independent of cache revisions, optional pinned chunks and explicit capacity failures. Retain eager fine as comparator when query responsiveness outweighs fine residency cost.
- HISTORY: each applied and committed contact sample contributes to a 15-tick (250 ms @60 Hz, provisional) window summary; after a summary closes it is eligible for peak impulse, cumulative impulse, cumulative duration and integrated resting-force triggers, separately. Promoted consequential events enter a bounded outbox, then append to a POSIX laboratory journal with payload/checksum, fflush/fsync and contiguous ACK. Outbox full = reject/rollback rather than overwrite; journal corruption = fail closed. Keep raw-all, peak-only, isolated-window and overwrite controls in tests.
- LOCAL TRANSACTION: one canonical material edit plus its committed contact samples can be staged as one copy-on-write world-version operation. This is not the entire production world physics transaction, durable world WAL, or synchronized network player body.
- LOGICAL LOCK: geometry material truth `G` AND valid traversal jurisdiction `T` precede any physically realized proposed action; cache/historical evidence cannot generate a false `G` or retroactively authorize `T`.

## Native lab comparison (local source package, not yet grafted into this GitHub branch)
- Strict C11 83/83 contact/grid/history fixtures PASS; 9/9 POSIX sink assertions PASS, normal production unsafe-grid policy disabled.
- Offline CMake/CTest 3/3 PASS. Inherited contact A1–A4 strict-C regression PASS. ASan/UBSan 83/83 PASS.
- 10,000 paired model evaluations, including 5 grid policies ×1,000 scenarios ×12 queries = 60,000 grid query observations, and 5 history policies ×1,000 contact episodes; repeated CSV SHA256 identical on the same host/compiler.
- Grid accurate / 12,000 queries: coarse 8,000 (4,000 false-blocks), eager 12,000 (0 pending, mean 4,160 voxel bytes), selective 5,000 (7,000 pending, mean 746 bytes), hysteretic 9,000 (3,000 pending, mean 2,794 bytes), unsafe missing-fine 8,000 (4,000 **false frees**; reject).
- History correct /1,000 episodes: raw all 1,000; peak only 400; isolated windows 400; cumulative+ACK 1,000; overwrite control 1,000 below queue capacity but fails separate overflow hard gate.
- POSIX sink confirms replay after simulated append-before-ACK does not duplicate event, and torn record rejects load; it is NOT a multiwriter, replicated, fully crash-consistent world WAL.
- Repeated scenarios represent parametrized fixtures, NOT random multiplayer game worlds. Nonzero 3,000/12,000 H1 hysteretic PENDING is admissible only provisionally and demands active on-demand refinement and latency measurement before H3 promotion.

## Current source provenance and exit gate
**The tested source is delivered as `nightfall-v18a5-canonical-history-source.tar.gz` in the conversation. This GitHub branch records this ledger and policy lock; do not claim source graft, full project CI, playable 1.8A.5, network H3 or human H4 until separately proved.**

## Next severe experiments
1. Tie actual authoritative 1.8A.3 swept capsule/1.8A.4 applied motor impulses to canonical materials and support contracts. Test walls, mixed cells, moving supports, rotated boxes, stale fine data, and simultaneous ownership claims.
2. Implement bounded spatial indexing and production chunk streaming with on-demand escalation for exact queries before committing movement; test latency and cache-thrash tails, not only memory.
3. Integrate a durable whole-world WAL and atomic contact outbox recovery; test power-loss insertion and multiwriter ordering.
4. Add 1.7E observer WORLD/ACTOR/CAUSAL/NETWORK overlays for contact/material/history with zero authority effect.
5. Run actual native H3 desktop/server prediction and H4 user-feel trials; preserve H1 incumbent plus eager-fine and raw-all controls.

## Industry references
Box2D separates contact begin/end/hit events from physical stepping. Rapier separates broadphase and narrowphase, and thresholds contact forces before event surfacing. These justify separating physics contact fidelity, material truth and long-term consequential event selection, not claiming algorithmic equivalence.
