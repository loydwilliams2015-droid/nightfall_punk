# v1.8A.6 — GitHub reconciliation incident, 2026-10-04

## Summary
The A6 local laboratory used `nf18a5_integrated_pair_step_checked()` for a prepublication geometry and world-state gate. The initial source publish at commit `40574040812e6c525790d60744d6f877067eedc3` omitted its newer A5 dependency files. As a result the strict GCC/Clang GitHub workflow failed to compile. Local A6 tests previously passed only because the local complete source package contained both dependency files.

### Failed remote evidence (retained)
- Workflow https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/37236430352
- GCC and Clang: `nf18a5_integrated_pair_step_checked` implicitly declared in `src/shared/nf_embody18a6.c:219`; the older published header only declared `nf18a5_integrated_pair_step`.
- This was a **Git source-provenance/dependency omission**, not a measured failure of collision physics. A green local build was insufficient evidence that the published source would compile.

### Correction
- Commit `fd3e3862975bfad41561c0f9f554b04eefae055e` adds the **tested local** `src/shared/nf_contact18a5_close.c/.h` files, with SHA-256 recorded in `evidence/v18a6/TICK_AUTHORITY_SHA256SUMS.txt`.
- Corrected push workflow https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/37236539427 — successful GCC and Clang matrix.
- PR workflows https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/37236559013 and https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/37236559098 — successful at the corrected source revision before this documentation-only follow-up.
- Local GCC/Clang offline CTest 7/7, GCC ASan/UBSan offline CTest 7/7 and new tick-owner test 32/32. The six older A6 model strata are **not** new multiplayer scenario evidence.

### Prevention / scientific norm
A release claim requires source completeness at an exact commit (including all dependent declarations), compiler-matrix results for that commit, explicit API provenance, retained failed CI, and objective scope labeling. A successful local archive does not certify GitHub source until the archive's dependent implementation is reconciled. This is an H1 single grounded actor/one crate laboratory, NOT full player+AI+combat+network gameplay. Retain open issue #31 and draft PR #32 pending production integration.

### Future conditions
- Full `nf_world_step` replacement must schedule **all** active actors and combat/energy/contamination/AI stages in one authoritative transaction or explicitly reject unsupported worlds.
- Extend Red/Blue/Purple beyond currently committed pair contacts only through physical causal dependency and same-tick target-local SCC evidence; no inferred Purple from semantic connectivity.
- Full client/server (ENet/raylib) build, native hardware timing and H4 human trials remain untested.
