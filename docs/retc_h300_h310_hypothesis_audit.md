# RET-C H300–H310 hypothesis audit and H308 source-exact certificate
Date: November 15, 2026. H307 predecessor source: `4be2984ac03b8be93d0c8cec37f60f286109025e`.

## Evidence baseline
H300 53/53, H301 52/52, H302 corrected 55/55, H303 56/56, H304 57/57, H305 58/58, H306 59/59 and H307 60/60 configured GitHub Actions PR workflows passed at their respective exact head SHAs. These are source-exact research checkpoint results, **not production multi-actor release certificates**. Earlier H302 first attempt failed from an incorrectly constructed negative test whose X coordinate was actually valid. The corrected test later passed 55/55. As of H307, no outstanding failed workflow.

## H300–H310 research hypotheses and implementation status
| H | Falsifiable hypothesis / recommendation | Evidence |
|---|---|---|
| 300 | A5 static clearance must accept FREE only and reject PENDING | Implemented, verified H300 |
| 301 | Real A6 map witness matches direct A5 swept query across 40 paths | Implemented, verified H301 |
| 302 | Nonfinite or unauthorized coactor source states fail preflight | Implemented, verified H302 |
| 303 | Both original B2 proposals must be recomputed from current issuer registry | Implemented, verified H303 proposal scope |
| 304 | Real-world two-body A4 stage refuses nonfinite roster input | Implemented, verified H304 |
| 305 | Both actor paths pass swept geometry from the same material epoch | Implemented, verified H305 |
| 306 | Cached A6 A4 owner has complete finite XYZ velocity, capsule and source shape | Implemented, verified H306 |
| 307 | Physically incompatible B2 and A4/A5 scenes must not be combined | Implemented, verified H307 |
| 308 | Actual A4 reciprocal contact receipt must share tick/revision and finite accepted impulse with the physical proposal | Implemented, exact H308 CI pending |
| 309 | Two current owner-issued B2 grants in a single physically active two-actor A6 scene are actually authorized | Not demonstrated; next positive hypothesis |
| 310 | Both typed B3 effects publish in one atomic A6 frame and restart without duplicates | Not demonstrated; C2/C3 transition |

## H308 new source and testing
`src/shared/nf_retc_h307_cross_source_binding.h`: extend the **necessary, not sufficient** structural pair check to validate the actual A4 reciprocal contact receipt. Require identical tick, A5 source world version, accepted status, applied impulse, nonzero contact ID, finite impulse components and finite energy delta. This function is **not** an authorization or commit API.

`src/tests/test_v18c2_retc_real_twotick.c`: 24 mutated receipt tests = 12 independent fields × 2 alterations. Each rejects, rechecks the original structurally aligned positive control, refuses the authentic mismatched B2 10/20 vs A4 30/40 scene, and confirms no accepted A6 publisher/tick/B3 count changes. Original aligned B3 control remains explicitly **synthetic**; it proves only structural predicate falsifiability, not B2 authorization.

GitHub Actions dedicated source-exact GCC and Clang tests validate these receipt trials plus H307 inherited tests. No H308 scoped PASS until both compiler jobs complete on the exact SHA; no complete cumulative PASS until every inherited workflow finishes.

## Consolidated X=11 recommendations and independent acceptance observations
1. H300 FREE-only clearance: green predecessor.
2. H301 real-source A5 parity: green predecessor.
3. H302 refuse nonfinite coactor: green corrected source.
4. H303 recompute independent B2 source proposals: green predecessor.
5. H304 finite real two-body roster: green predecessor.
6. H305 both actor swept paths share a material epoch: green predecessor.
7. H306 full XYZ owner capsule source: green predecessor.
8. H307 reject mismatched B2 and A4 actor IDs: green predecessor.
9. H308 enforce actual A4 receipt ancestry: pending same-SHA CI.
10. H309 create **positive same-scene**, active two-grant authorization with issuer freshness: OPEN.
11. H310 privately stage both typed B3 effects, atomic publication and durable crash recovery: OPEN.

The 24 H308 source mutation runs are test cases in compiler runs, not 24 independently finished production milestones. Cumulative workflows are overlapping, not 60 unique capabilities.

## M18C.2/M18C.3 acceptance and remaining distance
C2 P*: both independently B2-authorized *active* actors from one authentic A6 world, real A4/A5 contacts, two typed B3 realized effects, one immutable atomic publication; rejected attempt exposes no partial effects. HOLD.
C3 P*: a newly started owner reconstructs or rejects the whole portable WAL/checkpoint transaction without duplicated effects or incomplete consequences. HOLD.
Map/audio/render P*: actual native consumers agree on accepted A6 epoch, never speculative effects. HOLD.
Constitutional acceptance: independent ratification, appeal and versioned owner authority (human review required). HOLD.

Estimated remaining 7–14 substantial five-objective C2 sweeps, plus 4–10 potentially overlapping C3 sweeps, high-uncertainty planning ranges. H308's bounded verifier does not justify shortening them.

## Human playability survey litmus
P0 basic smoke: native Linux window launch, working WASD/third-person camera, score/objective, pause and safe exit; an uninterrupted 5-minute session with a reproducible build/seed, no critical fault.
P1 human-usable: 3 human sessions ≥10 minutes at 1080p, p95 frame ≤33.3 ms, p99 ≤50 ms, input-to-visible p95 ≤150 ms, zero critical faults; players report control responsiveness and task completion.
P2 alpha: 8 active actors, sessions 15–30 min; p95 frame ≤16.7 ms, p99 ≤33.3 ms, input-visible p95 ≤100 ms, no authoritative desync.
None of these are established by source-only contact CI. Run real native telemetry; survey human players separately on movement, readability, orientation, challenge, responsiveness (1–5) and freeform defects. Record hardware, version, median score, critical bugs and task completion. P0/P1/P2 remain UNMEASURED.

## Failure taxonomy
Incorrect negative test expected a legitimate coordinate to fail: historical H302 fixture defect, corrected.
A test accepting an invalid A4 contact receipt: H308 real semantic defect; fix source, new SHA and rerun.
Compile/runtime job failure: inspect exact job logs before attribution.
Checkout/runner/artifact failure: infrastructure; not source correctness evidence.
Pending: no verdict. Never turn 11 recommendations into claimed 11 successful new CI runs.
