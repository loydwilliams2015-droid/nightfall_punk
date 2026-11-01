# H72 empirical integration gate — actual two-actor A6 bind BLOCKED

**Baseline:** H71 `d33bd367dd5141d1867a3f7fc8e2f18f6d00dede` passed GCC full-game and GCC/Clang cumulative tests.
**Candidate:** H72 `90d761135d9119472b785cbdeb1a914621dfb08f` adds a real second actor to the existing source-exact A6/B2/B3 joint two-tick fixture.
**Executed:** GitHub run https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/41328282613. Independent Clang RET-C rejected real A6 bind at `test_v18c2_retc_real_twotick.c` line 83. This is a **valid architectural refusal**, not evidence of functioning two-actor authority. GCC job is independently evaluated.

## Precise causal root
`src/shared/nf_embody18a6.c` `nf18a6_bind_world_tick` explicitly rejects `nf_world_active_actor_count(world)!=1u` before binding the dispatcher. `nf18a6_tick_dispatch` separately rejects whenever the world actor count is not exactly one, documenting that it will not silently starve a remote actor. Therefore simply deleting the bind-time guard would **not** produce a legitimate multi-actor transaction. The current A6 owner step, per-actor input, B2 registry witness and B3 joint envelope are still one-authorized-actor shaped.

## Exact recovery recommendation (not yet implemented)
1. Preserve all single-owner gates and previous PASS proofs as baseline.
2. Introduce source-owned **multi-actor tick participants** in A6; enumerate active actor IDs deterministically and bind input/eligibility to a single source snapshot, not arbitrary callbacks.
3. Add deterministic actor–actor and actor–object broad/narrow contact and tie-break arbitration using existing MA03 / A4 solver. Account for impulses of all active participants and material witnesses, not only actor 10.
4. Stage the multi-actor A4/A5/B2/B3 consequences together in ONE immutable frame and enforce all-or-nothing refusal on partial eligibility or unsupported actions.
5. Ensure publisher, network serialization and AI/camera readers use accepted snapshots. Test two simultaneous real actor inputs in canonical, randomized, pathological, held-out, boundary and raced-shutdown cases.
6. Run GCC full/Clang RET-C, ASan/UBSan, TSan as feasible and a representative actor-scale frame-time test.
7. Only then attempt durability C3 cutover. Do not modify `NF_RETC_HISTORY` or native journal format to conceal a multi-actor authority gap.

**Gate status:** live multi-actor A6/B2/A5/B3 interoperability **FAIL/HOLD**. This fixture intentionally remains an executable failing acceptance test in the draft branch until a real multi-actor owner is developed. Do not merge it into the passing incumbent as if it were accepted.
