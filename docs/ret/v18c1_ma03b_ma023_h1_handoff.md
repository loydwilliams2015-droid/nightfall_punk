# nightfall!punk 1.8C.1 — MA03B and MA02-3 H1 evidence and handoff

PRAXIS / SIGNAL / STEWARD · October 20, 2026 (America/Chicago). Parent PR #74 tested source `f778ee5511faf9bed707ed89eca5b10f3d475c5e`. Draft laboratory only. **Production/release HOLD**.

## MA03B — independent unequal-mass three-body scientific reference
New `src/shared/nf_multi_ma03b.{h,c}` independently calculates chronological collisions among three collinear radius-matched bodies using closed-form *perfectly elastic*, mass-sensitive two-body formulas. It does not call A4. `src/tests/test_multi_ma03b.c` separately advances three A4 bodies over the corresponding horizon, computes actual `nf18a4_apply_contact` reciprocal impulses, and compares final positions, velocities, momentum and impact count against the independent oracle (5 strata × 64 = 320 cases; one intentional initial overlap group does not claim completion).

Important scientific failure record: an earlier *perfectly inelastic* oracle exhausted finite event budgets under collision clustering; retained as a nonaccepted uncertainty, rather than silently declaring completion. First elastic tests disagreed on contact counts with a short .25-second horizon at event boundaries; the benchmark horizon was widened to .6 seconds, and the same impulse/count/position/momentum assertions passed. **This is a research-horizon physics reference, not a 60 Hz game tick benchmark** and does not validate general 3D, off-axis, frictional or simultaneous-contact manifolds. The code under test is an explicit A4 event-driven comparison harness, not the entire real A6 actor scheduler and not `nf_ma01c2_integrate` as a general three-body solver.

## MA02-3 — positive and negative actual A6/B2 material adjudication
New `nf_ma023_recheck` reevaluates `nf_ma03_b2_owner_evaluate` twice, under real A6 `NfWorld` owner and `Nf18a6Runtime` identity/pose/material-pointer checks. It rejects missing/mismatched owner and any denied/pending/non-eligible decision, and checks consistency of the returned status, contract and physical classification. `src/tests/test_multi_ma023_bound.c` constructs an **actual A6-owned world**, an embodied actor and A5 canonical material cache, with server-side B2 scene fixture. Verifies eligible, credential revoked (BLOCKED), mismatched material/world revision (INVALID), unavailable provider (INVALID), revocation on recheck (BLOCKED), and unbinding the A6 tick owner (INVALID).

**Essential limitation:** B2 scene credentials, inventory and group policy still originate in an injected provider. Possession of the provider callback does **not** authenticate its origin, and equality of two classifications is weaker than an atomic, signed/versioned comparison of all provider data and dependencies. The new function is research-only, not a replacement for the existing B6 live tick prepublish gate and not a final durable publisher. The true multi-actor A6 owner is still unimplemented.

## CI and audit
GCC and Clang strict full offline CMake/CTest, plus full-source GCC ASan+UBSan headless CTest. Expected final inherited suite **23 tests**; exact run/commit should be verified separately. Preceding failed CI runs and changed model assumptions remain in GitHub source history (do not erase negative results).

## RET
A priori: material unknown cannot count as eligible; same-tick physical state and contractual state require one owner; a positive eligibility evaluation is not a committed consequence.
Formally specified: narrow collinear unequal-mass elastic oracle, independent from actual A4 impulses; owner-identity and physical-frame checks; double B2 evaluation.
Empirically validated: only source-matched exact CI fixtures. No network clients, long-running fairness proof, hardware power-loss or complex 3D independent truth.
Highly predictable: closed-form momentum/energy oracle is useful to isolate errors in A4 impulse algebra; runtime-bound material references reduce authority drift.
Uncertain: simultaneous contacts, inelastic collapse, finite-float end-frame TOI, same-tick provider changes between prepublication and durable ACK, long contention and 60Hz p99 performance.
Unresolved P0: complete multi-body A6 dispatcher, authentic credential source, independent 3D/whole-world oracle, portable whole-world A5/B3 codecs, one B5 durable envelope, source/rights #30/#36/#37, networking, concurrency and production release.

## Next phase
1. Make a general staged MA01C2 multi-body scheduler run directly against unequal-mass/orbiting and simultaneous-contact reference worlds (not just independent harness).
2. Bind B2 credential authority to a verifiable server-owned service with immutable tick-scoped generation, compare full witness hashes (not status alone), and connect validation to A6 prepublication; preserve an explicit pending branch when revisions change.
3. Put material/physics/contact-history/B3 deltas into one portable durable B5 transaction, with independent crash/recovery oracle; then consider H3 promotion.
