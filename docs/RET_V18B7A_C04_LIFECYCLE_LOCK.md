# C04LINE — Interaction Lifecycle Contract Lock
Date: 2026-10-18. PRAXIS / SIGNAL / STEWARD. Review branch audit/v18b7a-g1-authority-contracts. **Normative contract proposal accepted for research; empirical/full release HOLD.**

## Public governing principle
**Actor intention and even successful eligibility never establish an applied consequence. Only the authoritative A6 owner may commit a state change, and only actually executed A4/A5 material changes may be reported as physical outcomes.** B5 records must be consistent with committed consequences; UI/observer status cannot confer authority. This document locks semantics for the 1.8B.7A programme; it is not a production integration claim.

## Three domains and ten requirements
### A — Authorization and commitment (options 1–5)
C04-A01 A request has a typed actor/action/object identity, scope, timestamp and immutable reference to authoritative snapshot.
C04-A02 Proposal, eligibility and permission are separate from reservation, commitment and physical execution.
C04-A03 Stale world/material/contract epochs must be revalidated by A6 immediately before authoritative publication.
C04-A04 Reservations are scoped, finite and exclusive/shared according to contracts; a reservation is not a completed debit or grant.
C04-A05 Publication is failure-atomic: do not partially expose object, resource, account, body or receipt effects. Durable guarantees require actual B5 implementation, not a successful mock callback.
### B — Realization and history (options 6–8)
C04-B01 Track execution start, real progress and observed physical completion separately, with typed outcomes.
C04-B02 Cancellation before threshold can release reservations; interruption after irreversible work preserves history and uses *new compensating transactions*, never retroactive erasure.
C04-B03 A4 impulses, A5 contact/material history, inventory and resource changes have authoritative receipts only after actual respective operations; retain causal provenance.
### C — Observation and falsification (options 9–10)
C04-C01 Client feedback distinguishes requested, waiting, contested, refused, committed, executing, completed, interrupted and failed. Projection is read-only, sequence/epoch bound and monotonic with historical truth.
C04-C02 Test all legal and illegal stage transitions against canonical, randomized, pathological, boundary and held-out cases, including races, retries, failures, disconnect and crash recovery. Independent oracle necessary.

## Transition contract
PROPOSED -> ELIGIBLE | PENDING | BLOCKED | STALE
ELIGIBLE -> RESERVED | REFUSED | STALE (after revalidation)
RESERVED -> COMMITTED | RELEASED | STALE | FAILED
COMMITTED -> EXECUTING | FAILED (committed changes remain in history)
EXECUTING -> COMPLETED | INTERRUPTED | FAILED
COMPLETED -> (historical, immutable); any restitution is separately committed.
These states are semantic categories and do NOT assert that existing B3/B4/B5 code implements a single unified state machine. A state transition must state owner, preconditions, actual effects, and failure rollback/compensation.

## Evidence and release gates
Inherited B2 checked witness and opt-in A6 single-actor proofs are narrower than live multi-actor. B3 typed realizer H0 uses a trusted callback, not real mechanics. B4 client HF-01 only projects status; B5 H0 mock persistence is not a durable disk journal. C01 source H2 is a standalone new-source reference; C02 contract map and C03 tripartite M3/M4 protocol are preserved unchanged.
RET: a priori authority != material outcome; formal = defined transition invariants, NOT formal machine proof; empirical = none freshly executed by this document; highly predictable = better status distinction, not observed gameplay evidence; uncertain/open = A6 live transition adapter, B3 actual physics, B4 arbitration, B5 crash recovery, exact test coverage and independent oracle.
**C04 specification LOCKED, executable whole-lifecycle acceptance OPEN.** No authority transfer, merge or release promotion. All C01–C03 and historical hold statuses retained.

## Reusable command
[C0xLINE] = review evidence for C0x; select recommended approach, implement/test when justified, distinguish executed evidence from predictions, publish a versioned public contract to Group 1 PR, preserve holds and present 5–10 brief plain-English options for the next unresolved determination. A documentary lock is not an empirical PASS.
