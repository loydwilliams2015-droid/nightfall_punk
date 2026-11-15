# RET-C H306 — A6-owned A4 full-3D source-seal certification and failure audit

Date: 2026-11-15. Base H305 commit: `a96aceb4b3831f910527492c91bfa920d320b2ae`.

## Verified inherited status

H305: **58/58 GitHub pull-request workflows successful** at exact source SHA, including dedicated GCC and Clang pair-swept A4/A5 tests. Zero failed/cancelled/pending at the certified checkpoint.

Earlier H302 initial source run failed because a test fixture labeled an actually valid coactor coordinate as "invalid"; the corrected SHA later passed 55/55. This was an erroneous negative-test oracle, not demonstrated production invalid authorization. There are no unresolved H305 failures.

## H306 diagnosed actual source gap

`nf_retc_ma03_stage_physics()` checked source-coherent A6-owned A4 capsule X/Z velocity and 3D position with tolerances but did **not** check owner Y velocity against the world roster. It also did not explicitly reject NaN/Infinity in the owner A4 capsule fields before `fabsf` comparisons; a NaN may cause a comparison of the form `fabsf(NaN)>tolerance` to evaluate false. Separate H304 input-roster rejection does not establish the validity of the A6-cached physical owner.

H306 implements finite checks for owner center XYZ, velocity XYZ, angular velocity XYZ, capsule radius/height, inverse mass and inverse inertia XYZ; positive physical shape; owner height matching source actor body height; and owner velocity Y tied to the actual A6 world roster.

A real A6-owner, two-actor A4 contact harness exercises **40 source mutations**: 10 owner fields × 4 special values (NaN, negative NaN, +infinity, -infinity), every one followed by restoring the true owner source, a positive physics-candidate recheck, and a verification that the actual A6 world tick, both actor positions, immutable publisher pointer and existing B3 receipt count remained unchanged. No private or public authority is issued by the test.

Dedicated same-SHA GCC and Clang plus all inherited PR workflows must pass for a complete scoped H306 checkpoint. A queued/unfinished CI job is not certified.

## 50-objective programme, X=50 (5 objectives per workstream)

| Group | Targets | Evidence and status |
| --- | --- | --- |
| M2.17 | A4/A5 two-body, swept shape, material lineage, collision-chain, coherent owner state | Bounded private stage and source checks; production two-active-actor realization OPEN |
| M2.18 | Two live B2 grants, issuer provenance, revocation, arbitration, fairness | Independent proposals and source seals; same physical commit coupling OPEN |
| M2.19 | Typed B3 effects, independent entitlements, order, compensation, replay | Two proposals only; two realized effects OPEN |
| M2.20 | Exclusive A6 owner, unified commit envelope, all-or-none publish, epoch coherence, WAL | Single-actor research commit; atomic double-actor publication OPEN |
| M2.21 | GCC/Clang, sanitizers, held-out oracle, lifecycle races, native acceptance | Extensive CI but unverified whole-engine production acceptance |
| M3.1 | Versioned portable WAL/checkpoint and identity schema | OPEN |
| M3.2 | Torn write, power loss, durable replay, idempotency, failure injection | PARTIAL laboratory evidence |
| M3.3 | Fresh-owner process recovery, ownership transfer, exact reconstruct/reject, revision seal | OPEN |
| M3.4 | Real map/audio/render consumers, coherent frame timing, active-player input, graceful degradation | PARTIAL read-only map tests |
| M3.5 | Independent full-source audit, constitutional governance, deployable release, accessible docs, human signoff | OPEN |

**One 40-case safety test increment is not 40 completed features**; X=50 is an acceptance planning ledger, not closed item count.

## Failure taxonomy

1. **Source compilation/runtime defect:** reproducible failure against declared exact SHA, stop publication, fix source, retest.
2. **Fixture/oracle defect:** test incorrectly defines expected behavior; document evidence showing correct source behavior, repair test and rerun all relevant gates on new SHA.
3. **Semantic/authority defect:** stale capability, false clearance, ghost B3 effect, bypass of A6 single-owner seriality; unconditional C2 P* HOLD until corrected.
4. **CI infrastructure:** checkout, runner, artifact or timeout; not interpreted as source PASS or code FAIL. Rerun with logs.
5. **Queued/in progress:** no verdict; do not certify unexecuted cases.

## Final acceptance litmus

- C2 P*: **two independently B2-authorized active actors + real A4/A5 effects + two typed B3 consequences + one atomic immutable A6 frame**, with complete denial/rollback and source-exact independent verification. HOLD.
- C3 P-recovery: a **new independently started A6 owner** reconstructs or rejects the entire durable transaction using a portable versioned WAL/checkpoint, without duplicating B3 or material effects. HOLD.
- Cross-engine: map, audio and render observe the same accepted frame and do not emit speculative side effects. HOLD.
- P1 native playable: three Linux 1080p sessions >=10 minutes; p95 frame <=33.3ms, p99 <=50ms; input-to-visible latency p95 <=150ms, safe exit and no critical faults. NOT MEASURED.
- P2 native alpha: eight active actor sessions 15–30 minutes; p95 frame <=16.7ms, p99 <=33.3ms; input-to-visible p95 <=100ms; no divergence. NOT MEASURED.
- Governance acceptance: real accountable ratification and independent review, not A6 writer seriality. HOLD.

## Remaining distance (high-uncertainty estimate)

Retain **7–14 substantive five-objective sweeps** for M18C.2 and **4–10 potentially overlapping** for M18C.3. Not certified completion percentage, time promise, or release schedule.

## Next source-convergent milestone

H307 should couple **both current B2 grants and their typed proposed B3 consequences** to the **same A6-world actor IDs, object ID, accepted epoch and real A4/A5 swept physical candidate**. It must stage privately and prove all-or-nothing effects under injected failures. H307 must not be promoted as production until a true two-actor A6 commit and durable recovery are tested.
