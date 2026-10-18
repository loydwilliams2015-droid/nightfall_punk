# C06LINE — Relational Identity and Same-Tick Coherence: Public Commitment
Date: 2026-10-18. PRAXIS / SIGNAL / STEWARD. Group 1, 1.8B.7A. This is a review-branch contract, not a production integration or empirical test result.

## Locked decision and emphasized priorities
**Priority 1 — Option 5, relationships:** Maintain typed and versioned relations between actors, objects, canonical cells, local refinement cells, nexuses, inventory, ownership contracts, attachments and material identity. Identity is not location or ownership; moving, repairing or reparenting an object changes relations and revisions, not necessarily the object's persistent identity. A relation has a source, target, type, scope, effective tick, authority and revision/provenance. Track dependencies so invalidated relation witnesses cannot survive changes.
**Priority 2 — Option 9, simultaneous changes:** Resolve competing ownership, containment, attachment, refinement and material transitions through *one A6 authoritative tick transaction*. Read from S_t, validate proposal/read-write dependencies, arbitrate conflicts deterministically and fairly within applicable classes, publish committed changes as S_(t+1). No intermediate partial world, duplicate resources or separate competing tick owner. Fairness requires cross-tick measurement, not ID-order alone.

## Supporting commitments (Options 1–4, 6–8, 10)
- Typed stable IDs with generation numbers for any reused slot; all zero, unknown, stale or ambiguous references fail closed.
- Material identity epoch and mutable material/object state revision are distinct; object transfer, damage, open/closed state do not accidentally reset A5 persistent contact identity.
- Explicit relation edges include membership, support, containment, ownership, inventory, attachment and contract authority. Maintain their inverse/uniqueness constraints where semantics require; shared contractual membership is not silently forced into exclusive ownership.
- Scoped revision stamps and invalidation dependencies for geometry, support, occupancy, material, contract, navigation/perception and cached eligibility. Old versions never authorize after a relevant mutation.
- Refinement maps to canonical parent cell and common coordinates; finer geometry cannot multiply material or ownership.
- Detect ID generation/epoch exhaustion and reject instead of wrapping or silently reusing a live identity.
- Exercise transfer, concurrent read/write races, disconnect, repair, damage, destroy/recreate, multi-actor contested attachment, rollback/compensation and replay; paired canonical/randomized/pathological/boundary/held-out tests, independent oracle.
- Favor simple authoritative relationship and revision contracts with selective fine-grained validation (C05); do not introduce a generalized graph engine without measured need.

## RET / evidence classification
A priori: relation changes and identity changes are distinct concepts; simultaneous incompatible claims require a resolution rule. Formally specified: the above invariants and S_t→S_(t+1) arbitration contract, not a machine-checked proof. Empirical: **no new C06 source or test executed in this document step**; earlier B1 H2 overflow, B2 EEC and B4/B5 laboratory results remain scope-limited inherited evidence. Highly predictable: relation/dependency stamping can prevent stale references if implemented and exercised; cost and efficacy remain to measure. Uncertain: efficient dependency invalidation granularity, concurrent queue ordering fairness, alias detection performance. Unresolved: actual fully integrated A6 actor/physics/contract atomic publication, dynamic multi-actor ownership, durability.

## Public acceptance/hold
**C06 SEMANTIC AND ARCHITECTURAL COMMITMENT LOCKED** with *Option 5 relational coherence* and *Option 9 same-tick authoritative concurrency* the leading acceptance requirements. **C06 IMPLEMENTATION AND INTEGRATED EMPIRICAL ACCEPTANCE OPEN**. Preserve C01 H2 conditional adoption, C02 contract map hold, C03 M4/M3 protocol lock, C04 lifecycle, C05 selective refinement, historical H1 #37, licensing #36, ancestry #30 and overall release HOLD.

## Next C07
Prove exactly one A6 authoritative world-tick owner across actor interactions, physics, contract publication, material causality and journal consequences; no separate hidden owners.
