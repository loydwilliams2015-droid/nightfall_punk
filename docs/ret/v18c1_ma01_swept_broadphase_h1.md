# 1.8C.1 MA01 — Swept candidate discovery and industry standard evidence
PRAXIS / SIGNAL / STEWARD. 2026-10-20. Source parent PR #69 at `71c9751fa0ce82449aaba28ab27264a67806265f`. Experimental H1, release HOLD.

## Implemented
`nf_multi_broad18c1.{h,c}`: a bounded eight-body shared-snapshot sweep-AABB broadphase using `Nf18a4Body` and source tick duration. Each body contributes a conservative union of beginning and ending boxes (capsule bounding proxy or crate half-extents); all candidates sorted canonically by pair IDs; no world mutation; reject duplicate IDs, invalid body geometry and impossible resulting coordinates. Sweep handles fast crossing paths rather than only evaluating end-position proximity.

This is an **O(N²) small-island reference candidate filter** (max eight bodies) and not yet a spatial-hash/grid or dynamic-tree acceleration. Its primary use is to prove candidate completeness and isolate the next narrowphase. Neither the A4 contact solver nor the A3 capsule CCD is presently invoked here; overlapping swept AABBs are *potential* contacts, never verified impulses.

## Applicable established industry practices
- Box2D authoritative collision documentation distinguishes a broadphase (conservative potential overlap search) from narrowphase (geometric contacts); its dynamic AABB tree is a scalable challenger: https://box2d.org/documentation/md_collision.html and https://box2d.org/documentation/group__tree.html .
- Box2D continuous collision documentation uses shape sweeps/time of impact (TOI) for tunneling prevention and notes expensive dynamic–dynamic CCD may be selectively enabled: https://github.com/erincatto/box2d/blob/main/docs/simulation.md .
- Existing nightfall engine has `nf18a3_capsule_sweep` with translating capsule/AABB and `nf18a4_solve_island` for true reciprocal contacts. Those are incumbents, not reimplemented by this filter.
- Within the eight-body corpus brute-force is the simpler independent oracle, and a spatial index should only be promoted once larger benchmarks demonstrate cost savings with no missing pairs.

## Test and evidence
`test_multi_broad18c1.c` contains 5 strata × 256 = 1,280 scenarios, including rapid crossing, overlap, extreme velocity, capsule proxy and input-order permutation. Deterministic pair IDs are invariant to body-array reversal. The test checks fast moving pairs are not omitted and the incoming authority snapshot is unmodified. GCC/Clang headless regressions and ASan/UBSan in `.github/workflows/v18c1-ma01-broad.yml`.

**Limitations:** The current authored tests are *not* an independently computed exhaustive swept-geometry oracle over arbitrary positions/3D orientations; they only establish fixture-specific broadphase properties. Valid CCD and A5 material witness integration remain required. A false-positive candidate is acceptable, a missed physically possible pair is not.

## RET
**A priori:** broadphase overlap is a necessary candidate filter, not a sufficient contact certificate; proposed physical consequences must not commit on pending or unknown material data.
**Formally specified:** immutable input, bounded geometry, conservative swept limits and stable pair ordering.
**Empirical:** executed CI only on exact verified commits; do not infer physics/full-world fidelity from the narrow candidate fixture.
**Highly predictable:** O(N²) is affordable for a very small bounded island; tree or chunk-grid performance gains depend on sparsity, distribution and update frequency.
**Uncertain:** true capsule-capsule TOI, rotating body bounds, cross-chunk contacts, adaptive refinement, real multi-actor A6 tick ownership, long-running fairness, 1/2/4/8 worker determinism, 60Hz p95/p99, authenticated B2 and crash atomic A5/B3/B5.
**Unresolved:** all production source/rights, B2/A5/B3/B5 integrated durability, network and H3 acceptance holds.

## Next candidate
Implement a narrowphase challenger which takes these pairs, computes proven contacts/TOI against real A3 shapes, handles true dynamic actor–actor cases and only then supplies constraints to the canonical A4 island solver. Parallelize per independent island after comparing worker-order outcomes against the serial incumbent. Couple to canonical A5 material witnesses and B2/B3 authority before replacing the live A6 single-actor dispatcher. No production promotion.
