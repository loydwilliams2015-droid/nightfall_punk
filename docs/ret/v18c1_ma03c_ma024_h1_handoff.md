# nightfall!punk 1.8C.1 MA03C / MA02-4 H1 handoff
PRAXIS / SIGNAL / STEWARD. Date 2026-10-20. Ancestor PR #75 at `9e2eb79c53001b8158940c72cd724338a42fdfac`. Research-only, all production holds preserved.

## Implemented, exact scope
`src/shared/nf_multi_ma03c.{h,c}`:
- MA03C calls the **actual** `nf_ma01c2_integrate` scheduler and compares its two-body, horizontal, zero-restitution, unequal-mass outcome to an independently written closed-form analytical momentum reference, comparing both end-of-frame positions and velocities.
- Three-body and unsupported/off-axis/overlap configurations are explicitly classified **unresolved**; this is NOT a solved general 3-body, off-axis, or simultaneous-contact scheduler. The independent MA03/MA03B three-body references remain in inherited regressions, not yet run against the full scheduler.
- MA02-4 records identity, action, tick, world and material revisions plus a bounded deterministic FNV-derived fingerprint of selected B2 scene actor/object contract fields. This first invokes the existing A6-owner-bound B2 evaluator with A5 material checks. Recheck repeats evaluation and requires identity/revision/fingerprint equality. Typed fields are explicit, but the fingerprint is neither cryptographic authentication nor a complete serialization of every scene dependency.
- A bound A6-world fixture verifies a positive record and identical recheck, denial after changing server-fixture credentials, changed action refusal, detached owner rejection; a separate suite tests missing server provider denial.

## Independent evidence and build scope
Five families ×128 = 640 MA03C scheduler comparisons: positive two-body, no hit, overlap, unsupported vertical motion, 3-body unresolved. Full strict GCC/Clang and ASan/UBSan headless inherited CTest; live A6 snapshot fixture exercises the real runtime, B2 and A5. The 3-body full scheduler is *not* independently validated. Be explicit that the retained numerical checks are only for a bounded idealized model.

## RET
A priori: positive eligibility is not authorization to publish; neither noncryptographic hash nor two repeated calls prove provenance; non-resolved collision frontier cannot become success.
Formally specified: real A4/A6 scheduler comparison using idealized momentum oracle, fail-closed source-bound record generation and revision/fingerprint matching.
Empirical: source-matched CI; no performed real network, power-cut, multi-worker, 60Hz latency or security penetration tests.
Highly predictable: provider revision/fingerprint comparison catches selected accidental state changes; authority provenance remains separate.
Uncertain: simultaneous contact, 3-body event schedule, inelastic clusters, height-varying capsules and orientations, completeness of the witness fingerprint, provider modification between recheck and actual journal publication.
Unresolved gates: authenticated immutable server credential generation; check/publish atomicity within A6 owner lock; complete physical A5/B3/B5 portable durable transaction; independent 3D whole-engine acceptance; provenance/license #30/#36/#37, multiplayer scaling and p95/p99.

## Next decisive tasks
(1) extend the actual MA01C2 multi-actor scheduler and show 3-body and off-axis cases against independently derived oracle, not separate contact harness; (2) replace the injected provider with a server-issued immutable tick-scoped full dependency record including material witness hashes and rights, ensure owner-locked prepublication verification; (3) single portable crash-atomic A5 physical + B2/B3 contract and B5 transaction; (4) independently replay across crash points. No production promotion. All draft PRs remain unmerged.
