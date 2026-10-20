# 1.8C.1 MA01C + MA02 — bounded CCD and material-contract witness research
PRAXIS / SIGNAL / STEWARD · 2026-10-20 · Review-only H1, production and release HOLD.
Base PR #71 at `62cfe4935a9d54d893abad0bedff98dcc2bb03bd`.

## Code delivered
`src/shared/nf_multi_c1_next.{h,c}`: analytic quadratic time of impact (TOI) for **two upright capsules with the same vertical center and zero vertical velocity**, using horizontal center separation and radii. Validated collision supplies an A4 normal/contact point and can produce actual reciprocal A4 impulses. Initial overlaps, unsupported vertical relative dynamics and invalid configuration are PENDING rather than fabricated resolved contacts.

`nf_ma02_eligibility` is a separately testable, restrictive **witness adapter** requiring actor ID, revision, contract identity, authenticated flag, known physical material and contract grant. Missing material yields PENDING; missing/stale/unauthenticated contract or known blocked material yields DENIED. These explicit fixture flags are not the real authenticated B2 provider nor an actual A5 sweep, and MUST NOT be accepted as production authority. Actual B2/M3 and A5 canonical/fine material corroboration remains to be wired and independently checked.

## Executed tests (only exact-source CI is evidence)
`src/tests/test_multi_c1_next.c` exercises 5 families × 128 = **640** scenarios: positive high-speed collision with actual A4 impulse, no reach, initial penetration, unsupported vertical dynamics, lateral separation, and allowed/pending/denied witness combinations. Existing CMake headless entire suite and GCC/Clang/ASan+UBSan CI. Numeric solution is analytically exact only for the restricted horizontal circular swept profiles in ideal arithmetic, not general 3D capsule CCD.

## RET
**A priori:** eligibility is not execution; an unknown material witness cannot grant access; an initial overlap needs an explicit recovery policy.
**Formally specified:** narrow supported domain, TOI in [0,1], no mutation of the source physical body, denied/pending statuses.
**Empirical:** bounded 640 fixtures including A4 result and compiler/CTests once CI passes; the test is not an independent exact 3D physical oracle.
**Highly predictable:** quadratic 2D center-sweep is inexpensive and useful for supported horizontal human actor contacts.
**Uncertain:** arbitrary-height capsules, moving vertical geometry, multiple chronological TOIs with remaining-time solving, numerical edge cases, authentic A5 material and B2 credential witness verification, A6 real world owner, latency tails.
**Unresolved:** full MA02 physical/contract integration, genuine multi-actor A6 replacement, joint B3/B5 durability, independent full-world correctness, source/rights and networking.

## Next engineering handoff
Connect the narrow capsule/capsule result with MA01B box contacts in a single ordered TOI scheduler that accounts for remaining tick time, and compare with an independent analytical reference. Then have each actor acquire actual A5 geometry/material clearance and B2 authenticated contractual authority rather than trusting caller booleans. Only one A6 owner may publish a verified and durable transaction. No production promotion.
