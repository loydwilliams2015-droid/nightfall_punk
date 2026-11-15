# RET-C H307 — cross-source physical/contract binding sweep, failure ledger and scoped certificate

Date: 2026-11-15. Base H306 source `3ec96b8047df08a6a499cd1ede9e4fef902f05a8`.

## Previous checkpoint and failures

H306 exact-SHA GitHub PR workflows: **59/59 successful**, zero failed/pending, both GCC and Clang successful. This certifies bounded A6-owned A4 source integrity; does not constitute M18C.2 release certification. The only previously clarified H302 red was an incorrect negative-test fixture (a purported invalid coordinate equaled valid source) subsequently corrected and verified H302 55/55. No active H306 failures.

## H307 problem: tempting but incorrect provenance join

The real two-B2 proposal in the accepted A6 encounter fixture involves actors **10 and 20**, whereas the independently real A6-world-derived two-body A4/A5 contact fixture involves actors **30 and 40**. Even if both independently pass, combining these two pieces of evidence into one shared committed frame would be an authorization/provenance error. The engine must refuse this join, not claim one source exactness from two independently valid but incompatible sources.

`nf_retc_h307_structural_pair_match()` is a **necessary, never sufficient, structural join predicate**, comparing actor identities, ordering, object identity, two-event tick/epoch/material lineage, original A4 roster/solved bodies, same A5 handoff, full material FREE for both actors, proposed B3 phase and no applied/authorized flags. It does not grant authorization or publish a frame, and its caller-supplied inputs are not independently trusted.

### New test matrix (40 cases)

- One negative assertion using the **two authentic but disparate** contexts (B2 10/20 vs A4/A5 30/40) must fail.
- One explicitly **synthetic**, separately labeled canonical event pair aligned to the real A4/A5 source, used only as a positive control for *the structural predicate*, not as a B2 grant.
- Forty adversarial mutations: 10 actor/epoch/tick/material lineage fields × four nonzero deltas; each must deny and preserve the true source, accepted publisher, world tick and existing B3 count.
- Exact SHA GCC and Clang workflow runs the full genuine A6 real-two-tick fixture with inheritance. No scoped H307 certification until both jobs successfully complete.

## Five M2 workstreams and 25 release criteria

M2.17: two-authoritative-actor A4/A5 contacts, swept material, chain oracle, source coherence and production commit. Laboratory real solver and bounded clearance exist, **production joint commit remains open**.

M2.18: both independent B2 grants, current issuer provenance, revocation, arbitration and fairness under active contention. Two source-authentic proposals exist, **same-source active physics authorization remains open**.

M2.19: typed B3 consequences for each actor, realization, compensation, determinism and idempotent replay. Two proposed identities exist, **joint typed applied consequences remain open**.

M2.20: one unified A6 envelope, exclusive tick ownership, one transaction commit, failure atomicity and durability boundary. **Not implemented for both active actors**.

M2.21: source-exact GCC/Clang, sanitizers, held-out tests, independent readers and native gameplay acceptance. **Partial tests only**, P* HOLD.

The remaining 25 C3 objectives cover journal/checkpoint, power loss, fresh-owner recovery, native map/audio/render, constitutional and release certification. H307's 40 cases are a test increment, not 40 finished implementation tasks.

## Failure taxonomy

- Compile error / runtime assertion: source or test issue until logs classify it.
- False-negative test fixture: must prove expected source behavior, repair fixture, rerun at new SHA.
- Unauthorized cross-source admission: substantive security/contract defect; C2 P* must be held.
- CI runner/checkout failure: infrastructure; rerun without claiming PASS.
- Queued/in-progress: no verdict.

## Litmus and distance

C2 P*: TWO independently owner-issued B2 grants tied to SAME real active actor IDs, contract and A4/A5 epoch; two typed B3 realized effects; single A6 immutable atomic commit; no visible partial state after failed stage. **HOLD**.

C3 P-recovery: a fresh independent A6 process restores or rejects entire durable transaction from versioned WAL/checkpoint, no duplicate effect. **HOLD**.

Cross-engine acceptance: native map, render, audio observe same committed frame without ghost effects. **HOLD**.

P1 proposed playable: three native Linux 1080p sessions >=10 minutes; p95 frame <=33.3 ms, p99 <=50 ms, p95 input-visible <=150 ms, no critical faults. **NOT MEASURED**.

P2 proposed alpha: eight active actor 15–30 minute sessions; p95 frame <=16.7 ms, p99 <=33.3 ms, p95 input-visible <=100 ms and no authoritative divergence. **NOT MEASURED**.

Constitutional ratification and independent signoff require accountable human governance, not software provenance alone. **HOLD**.

Prior high-uncertainty planning estimate remains 7–14 substantive five-objective C2 sweeps plus 4–10 potentially overlapping C3 sweeps. Not an earned percentage or release date.

## Next milestone H308

Construct the first **positive** two-live-B2-authorized, same-source A4/A5 contact fixture for two active actors with current issuer provenance; privately stage two typed B3 consequences. Use the structural predicate only alongside true A6 owner authorization. Apply each failed grant, geometry, commit and recovery injection before any A6 publication. Preserve draft until dual-effect positive and complete rollback both pass.
