# RET-C H305 | M18C.2 A4/A5 pair swept-source acceptance report

Date: 2026-11-14. Base H304 head `46e06615fd7047e4cf6253508bf85d8b6bfe2d46`.

## Previous checkpoint: certified, no outstanding failure
H304 completed **57/57 configured GitHub PR workflow successes**, including dedicated Clang and GCC. Zero failures at the final observed checkpoint. Earlier H302's malformed finite-position test failure (a supposed invalid coordinate matched the legitimate coactor coordinate) was corrected at `ef359fdd53fccaa4ee34c5cc5c9023ef4fcf7dac`; subsequent H302 matrix passed 55/55. This was a **test oracle/fixture defect**, not a demonstrated production bypass. Do not conflate historical failed PR head with the later corrected green SHA.

## H305 implementation and tests
New `nf_retc_ma03_pair_swept_clearance` in `src/shared/nf_retc_ma03_a5_sweep_pair.h` binds **two source-owned A4 solved actor paths** to the **same A5 material epoch**. Both must return actual `NF18A5_Q_FREE` via `nf18a5_sweep_query`. Static terrain cache operations occur only on privately copied grids, never the authoritative A6 world. No frame is published.

The actual two-owner A4/A5 research harness performs forty cases:
- 20 accepted paths with distinct cache revision generations that must not change physical or material authority.
- 20 refused paths with a resident static chunk switched to SOLID or unresolved MIXED; no stale or missing fine-material witness may confer clearance.
Every case restores its test source, rechecks the valid original, and asserts unchanged publisher pointer, actual tick, A5 history and B3 receipt count.

A dedicated GCC/Clang source-exact workflow and all inherited workflows must PASS on the H305 head before any scoped acceptance. These forty cases test one bounded M2 physical-material interface; they do not correspond to forty closed release milestones.

## M2.17–M2.21 engineering acceptance summary
| Area | Supported | Still required |
| --- | --- | --- |
| M2.17 physics/material | Two real A4 source bodies, one private A5 contact, same-epoch double swept static checks | Production simultaneous multi-actor material/physics commit |
| M2.18 independent B2 | Two read-only B2 proposals revalidated against source | Both active grants bound to same physical candidate at commit tick |
| M2.19 typed B3 | Two authentic proposed B3 event identities | Actual separate committed consequences and no double side effects |
| M2.20 atomic A6 | One immutable single-event accepted research path | Unified two-actor multi-effect A6 atomic publication with failure rollback |
| M2.21 independent tests | Configured source-exact GCC/Clang and bounded negative/positive controls | Held-out end-to-end producer/consumer, sanitizer and native acceptance |

## Failure taxonomy
- Compile/source failure: compiler error on exact SHA; must fix source.
- Test oracle failure: incorrect expected value or fixture; must show the source accepted behavior is lawful, then correct and re-run on a new SHA.
- Semantic/authority failure: source accepts invalid grant/geometry or leaks partial transaction; production blocker; no release certificate.
- CI infrastructure failure: runner/checkout/artifact unavailable; rerun with provenance, never silently waive.
- Pending job: not a pass and not a fail.

## Final P* and playability gates
C2 P*: two independent live B2-authorized actors + A4/A5 source-exact physical effects + two typed B3 consequences + one atomic immutable A6 commit, with complete rollback and independent acceptance. **HOLD**.
C3 P-recovery: a new A6 owner process reconstructs the *whole* durable transaction from portable WAL/checkpoint and never duplicates effects. **HOLD**.
Cross-engine P*: real native MAP/AUDIO/RENDER observe same committed transaction; no ghost consequence. **HOLD**.
P1 proposed: three >=10-minute native Linux 1080p sessions, p95 frame <=33.3ms, p99 <=50ms; input-to-visible p95 <=150ms, zero critical faults. **UNMEASURED**.
P2 proposed: eight concurrent actors, 15–30 min, p95 frame <=16.7ms, p99 <=33.3ms, input-to-visible p95 <=100ms. **UNMEASURED**.
Constitutional ratification/revision authority: human governance criterion, not proven by A6 writer seriality; **HOLD**.

## Planning distance and decision
Prior high-uncertainty estimate retained: **7–14 substantial five-objective M18C.2 sweeps** and **4–10 potentially overlapping M18C.3 sweeps**. No unsupported completion percentage or calendar date. H305 only closes if its tests finish successfully.

Next: source-bind the two B2 grants and proposed B3 identities to the *same* accepted physical/contract actor/object epoch in an A6-private staging envelope, then prove two realized consequences and all-or-none single publication with failure injection. Maintain draft until then.
