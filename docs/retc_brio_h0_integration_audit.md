# RET-C / BRIO H0 — deterministic traversal prefilter research
Date: 2026-11-16. Source baseline H318 a465670e612b61a82b073acdbc68c758e1528bf0. This is an isolated experiment, not production M18C.2 certification.

## Scope and observations
- Existing H318: 68/68 source-exact GitHub workflows succeeded, with 10 genuine reverse-pose A4 contacts absent and actor20 B2 material PENDING. H319–H327 remain open.
- New BRIO prefilter in tests/brio_snap_contract.[ch]: purely functional, fixed-unit (mm, mm/s), deterministic, nearest-qualified-surface selection, stable surface-ID tie breaking and strict fail-closed gating.
- No A6 tick or publisher, A4/A5 geometry, B2 authorization issue/consumption, or B3 effects are modified by the prefilter. Its three gate fields are research inputs, NOT verifiable production B2/A5 credentials. A return of ALLOW is a **candidate only**, not authorization.
- Prototype tests: 23,527 check assertions per compiler, seed 0x15c2b319; GCC and Clang each completed locally under ASan+UBSan with zero failures (two executions). Re-run on source-exact GitHub SHA before certifying the uploaded branch. TSAN not executed. No human play.
- BRIO comparisons: conventional ragdolls require actual body integration; assisted Sunset Overdrive / Jet Set Radio-like motion requires validated acquisition; ONI / Dwarf Fortress-like consequences require material effects and persistence. None is proved by this prefilter.

## X = 26 engineering recommendations/test-run planning accounting
X is 5 reserve + 9 outstanding RET-C hypotheses + 10 BRIO experiment recommendations + 2 executed compiler test runs = 26 scoped items. Reserve is planning capacity, not five completed changes.

### RET-C production H319–H327 (all OPEN)
1. H319 validate actual reverse/arc A4 impacts against independent oracle.
2. H320 obtain same-owner dual B2 eligibility with A5 FREE for both.
3. H321 consume two true grants and stage two real B3 typed effects.
4. H322 one A6 atomic immutable publication and failure rollback.
5. H323 versioned transaction journal/checkpoint and torn-write rejection.
6. H324 new-owner independent whole-transaction restoration exactly once.
7. H325 accepted-epoch-only native consumers.
8. H326 independent sanitizers, fairness, held-out and playable trials.
9. H327 independent ratification and whole-engine acceptance.

### BRIO candidate hypotheses
10. B-PHY-01 deterministic capsule impact and bounded ragdoll impulse — OPEN.
11. B-PHY-02 partial ragdoll and recovery — OPEN.
12. B-TRV-01 deterministic fail-closed traversal candidate prefilter — prototype implemented; local GCC/Clang PASS, repository CI pending.
13. B-TRV-02 A4/A5/B2-backed live rail acquisition — OPEN.
14. B-TRV-03 wall ride with tangential momentum, no penetration — OPEN.
15. B-TRV-04 chained rail/wall/jump movement — OPEN.
16. B-MAT-01 material-specific friction response — OPEN.
17. B-MAT-02 mass and impact sensitivity — OPEN.
18. B-MAT-03 contested dual-actor real effects — OPEN, same dependency as H321/H322.
19. B-SYS-01 durable material effects across fresh-owner recovery — OPEN, same dependency as H323/H324.

### Test executions and reserve
20. Local GCC C11 -O2 -Wall -Wextra -Werror ASan+UBSan: 23,527 PASS / 0 FAIL.
21. Local Clang same flags: 23,527 PASS / 0 FAIL.
22–26. Reserve for evidence-driven corrective recommendations after H319 and CI, not counted as runs.

## Release boundaries (unchanged)
**C2 P*** requires physical same-owner A4 contact, both A5 paths FREE, two independently current owner-issued B2 grants, two applied typed B3 outcomes, one atomic A6 immutable publication, negative fault tests, canonical/random/pathological/boundary/held-out, sanitizer/TSAN and lifecycle evidence. HOLD.
**C3 P-recovery** requires complete, versioned WAL/checkpoint and whole-effect recovery or rejection by genuinely new A6 owner with no ghost/double receipts. HOLD.
**P0/P1/P2 playable** require native Linux human sessions and measured p95/p99 frame and input latency; all UNMEASURED.

## Failure interpretation
A4 negative contact, B2 A5 PENDING, denied/stale gate or out-of-radius rail are expected safe refusals. A traversal candidate returned with any pending/denied gate, source mismatch, collision penetration, or mutated owner epoch would be a genuine defect. Local success on isolated prototype must not be presented as H319, C2 or C3 progress.
## Estimated distance
High-uncertainty existing ledger: 7–14 substantive C2 five-objective sweeps plus 4–10 potentially overlapping C3 sweeps. BRIO adds a distinct non-blocking playable prototype track; not counted in C2 closure.

## H0-S/P serial and parallel observation continuation (2026-11-17)
New source test: `tests/brio_snap_parallel_test.c`. Same source and data generation are evaluated serially forward, serially reverse, and via four pthread workers with disjoint output regions. Eight rounds × 4096 cases = **32,768 test cases per compiler**, **65,536 pairwise comparisons per compiler**, with seeded deterministic candidate geometry, mixed denied/pending gates, and an order-stable result signature. Positive candidate selection is a *research observation only*. No accepted-world concurrency, live B2 grant, A6 ownership, B3 journal or production traversal is claimed.
- CI: GCC and Clang ASan/UBSan exact-SHA workflow has been added; completion must be checked at head SHA before certifying. No ThreadSanitizer or human measurement run.
- Null/denied/pending gates still fail closed; nearest eligible selection remains order-independent. The experiment directly falsifies schedule-dependent pure-prefilter outcomes but cannot establish absence of A6 data races.
- Total next-sweep accounting **X=28**: 9 production H319–H327 recommendations + 10 BRIO research recommendations + 4 compiler/test execution slots (existing GCC/Clang contract and new GCC/Clang S/P comparison) + 5 reserve recommendations. The reserve items are unimplemented. This is a scope ledger, not 28 completed implementations.
- Parallel hypothesis **SP01**: independently generated per-case candidates produce identical semantic output in forward/reverse serial and four-worker parallel schedules at the exact same source revision.
- Serial hypothesis **SP02**: repeating deterministic seeds across GCC and Clang yields equal golden signatures and failure counts.
- Integration hypothesis **SP03** (OPEN): same A6 source snapshot, live A4/A5/B2 admission, and authoritative A6 publication produce the same accepted epoch across serial and parallel world-schedules, including denied/stale cases.
- Performance hypothesis **SP04** (OPEN): representative 1080p p95/p99 and response latency from native Linux human play.
- C2 P*, C3 recovery and P0/P1/P2 remain HOLD without new positive source-backed experiments.
