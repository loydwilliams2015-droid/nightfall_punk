# nightfall!punk 1.8C.1 — Multi-Actor World Authority (H1 staged A4 island)
Date 2026-10-20. PRAXIS / SIGNAL / STEWARD. Source parent PR #68 at `1773f4a0f6116705b8e86cdb0afb42ad8dd91de6`. Research-only; production HOLD.

## Actual engineering changes
- `src/shared/nf_multi18c1.h/.c` introduces a bounded **single-owner immutable snapshot** with up to eight true dynamic A4 bodies, deterministic actor/action-key ordering of independently proposed motor intents, identity/revision validation, single-actor single-intent arbitration, and staged authoritative contact solution through the existing `nf18a4_solve_island`.
- A4's actual reciprocal velocity impulses are realized on staged body copies, not estimates. A later explicit `nf_multi18c1_publish` replaces the bounded whole snapshot with its successor. Invalid/stale/competing proposals or unresolved contact islands leave input world unmodified.
- CMake `NF_CONTACT_LAB_ONLY` receives the new challenger executable; offline GCC/Clang CTest and standalone ASan/UBSan. Incumbent native A6 one-actor scheduler remains unchanged and all inherited B6/UCW physics+contract/journal regressions run.

## Scientific comparison
**Incumbent:** `nf18a6_step` / `nf18a5_integrated_pair_step_checked` owns a real *single* actor plus dynamic crate and refuses nearby additional actors. Retain as the H1 physical regression reference.
**Challenger:** `nf_multi18c1_stage` uses the real A4 multi-contact solver for genuine actor–actor momentum exchange across an 8-body bounded world, multiple separate actor motors and one authoritative staged world snapshot. Compared arrival order permutations, conflict/stale/bad-geometry refusals, staged-vs-committed invariants. **Not a like-for-like throughput comparison**: A6 includes A5 material witnesses, B2 gate, body and crate world mirrors, and contact history; new challenger does not yet include those.

## RET
- **A priori:** conflicting proposals cannot independently own a single material state; an unresolved physical contact cannot be represented as completed; the existing A6 owner cannot be bypassed for genuine multi-actor gameplay.
- **Formally specified:** fixed stable ordering, one intent per actor per tick, immutable input, atomic local staged output, unchanged world on refusal, no second A6 world-tick owner.
- **Empirically validated:** only actual exact-source CI results; 5 strata × 256 = 1,280 tests, reciprocal A4 contact + deterministic input permutation with sanitized compiler coverage. Tests do not verify fair arbitration across long histories, realistic simultaneous CCD, engine-wide causal completeness or network play.
- **Highly predictable:** real A4 island reuse avoids introducing a second solver model; staged world copy makes publication simpler; copies and canonical sorting have finite costs.
- **Uncertain:** actual multi-actor A3/A5 clearance, contacts spanning chunks, fair competing intents, interaction with B2 contract mutations, collision CCD for many moving actors, 60 Hz p95/p99 and integration performance.
- **Unresolved production gates:** actual `NfWorld` A6 owner handoff from one-actor to shared multi-actor, contact samples and A5 history, B2/M3 independent oracle, durable C3/A5+B3 journal, cross-worker determinism and acceptance, hardware power-cut, licensing/rights and main ancestry. H3 and release HOLD.

## Next engineering action
Preserve the one-owner shared snapshot; add A3/A5 canonical and refined material clearance for **every** real actor and inter-actor broadphase/CCD, then replace (not add alongside) the single-actor A6 dispatcher. Couple contact receipts into canonical A5 history and B3 contract consequences at the same complete durable B5 publication boundary. Test independent multi-actor physics truth and 1/2/4/8 worker configurations only after concurrency exists; do not fabricate parallel speedup from this serial deterministic H1.
