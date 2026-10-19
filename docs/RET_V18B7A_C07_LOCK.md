# C07LINE — Single writer and immutable snapshot
Date: 2026-10-18. nightfall!punk 1.8B.7A Group 1. PRAXIS / SIGNAL / STEWARD.
**Public governing commitment:** One existing A6 authoritative world-tick owner receives actor, material, physics and contract proposals, validates a shared S_t snapshot and alone publishes S_(t+1). Specialized solvers are workers, not another authority.

## Source-level demonstration (strictly scoped)
nf7_batch accepts requests and a world snapshot, stages any changes, and calls a trusted persistence callback before replacing the local world. This is a standalone single-writer API contract, NOT a live A6 owner hookup.

## Acceptance still open
Prove actual A6 locking and owner identity, reentrancy, tick/snapshot synchronization across movement/physical/material/network paths, fail closed on client-supplied authority.

## RET classification
- **A priori:** authorized proposal is distinguishable from actual material realization, and consistency constraints apply to all observed state.
- **Formally specified:** finite-state and resource invariants in the source API; not a formal theorem of a live game engine.
- **Empirically observed:** only the authored local authority lab and inherited historical evidence explicitly cited in the Group 1 suite; no release-grade integrated acceptance.
- **Highly predictable:** explicit staging, typed results, deterministic tie breaks and stable action identity can reduce classes of errors when bound correctly to production authority; unproven outside the declared scope.
- **Uncertain/unresolved:** items above, plus linked B1 provenance #37, legal #36, canonical branch #30.

## Public disposition
**C07 semantic design and limited lab implementation COMMITTED. Whole-engine test and acceptance HOLD.** Existing C01–C06 commitments remain intact; no silent release promotion. M4 remains provisional eligibility incumbent and M3 active challenger under C03 tripartite rules. This laboratory's two arbitration ordering policies are NOT M3/M4, and cannot substitute for the independent comparison.
