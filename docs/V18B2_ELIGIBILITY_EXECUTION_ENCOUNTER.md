# 1.8B.2 — Eligibility–Execution Encounter Contract (EEC-01)

Status: proposed binding engineering contract, NOT executed proof. 2026-10-10.
Source programme: RP-01 / PR #42, H1/H2 B1 historical distinction, unified Codex handoff PR #43.

## Philosophical-to-operational translation
Eligibility and execution form an **eventual encounter**, not a timeless identity or a single simultaneous boolean. Eligibility specifies *necessary* physical and contractual conditions at an authoritative tick snapshot. Execution is a contingent, temporally subsequent or explicitly same-tick-ordered physical transaction. Contingency includes changing geometry, contested access, actor intent reversal, force/collision outcomes, streaming data and resource exhaustion. A correct eligible decision does not ensure the attempted action succeeds.

An **encounter event** binds actor, object/feature, action, tick, authoritative world/material/contract epoch, jurisdictional reasons, reserved resources (if any), causal provenance, and subsequent disposition. There can be zero or more execution attempts per eligibility assessment only where the contract explicitly allows; all attempts require fresh validation or a safe authoritative reservation. The encounter is the junction of determination and contingent realization; not a permission to invent physics.

## Normative state progression
PROPOSED -> EVALUATING -> {BLOCKED | PENDING | ELIGIBLE}
ELIGIBLE -> {EXPIRED | RESERVED | REVALIDATING}
RESERVED -> {CANCELLED | EXPIRED | COMMIT_PENDING}
REVALIDATING -> {BLOCKED | PENDING | RESERVED | EXPIRED}
COMMIT_PENDING -> {COMMITTED | FAILED_NO_EFFECT}
COMMITTED -> {EXECUTING | COMPLETED | COMPENSATION_REQUIRED}
EXECUTING -> {COMPLETED | INTERRUPTED | COMPENSATION_REQUIRED}

Only the authoritative tick owner may perform transitions that change world state. An eligibility decision does not itself change bodies, history, capacity, balances or ownership. A committed event may become physically interrupted; record history rather than retroactively pretending the earlier commitment never happened. Pre-commit cancellation produces no persistent consequence unless reservation rules explicitly require it; post-commit reversal uses new compensating events, not erasure.

## Local invariants
E1: A permit is necessary but never substitutes for material feasibility.
E2: Body clearance/support/LOS use authoritative geometry at the declared scope; unresolved material is PENDING, not GRANTED.
E3: Evaluation and publication snapshots bind actor+object+material identity+state revision+contract epoch+tick; changed witnesses demand revalidation.
E4: Equal action ID and actor/target/tick cannot publish twice, including after rollback/reboot at the durability boundary.
E5: Physical receipts derive from actual A4 applied impulse/contact; A5 history is never fabricated from eligibility.
E6: Reservations and resource debits publish atomically or not at all. Failed journal/capacity commits leave world unchanged.
E7: Contested claims are settled at the authoritative same-tick arbitration barrier; actor-ID ordering may be deterministic but is not evidence of fairness across time.
E8: No time-travel: an executed interrupted action remains an historical fact, with compensating transaction on cancellation if needed.
E9: Read-only observers never alter the authority state; Purple only on evidenced same-target/same-tick reciprocal material dependence.
E10: B2 owns authorization and handoff; B3 owns realized multi-stage action; B4 owns sustained strategic contention/fairness; B5 owns distributed persistence and recovery. B2 must specify compatible interfaces, not independently complete future builds.

## Boundary questions and acceptance fixtures
1. ELIGIBLE at tick t, obstacle closes before commit: STALE/BLOCKED/PENDING, no movement or debit.
2. ELIGIBLE under contract, actor withdraws before commit: CANCELLED, no physical receipt.
3. Shared capacity=2, three valid simultaneous proposals: exactly two unique grants in deterministic order; one explicit refusal; no double grant.
4. Valid ladder contract without support/clearance: BLOCKED and no synthesized climb.
5. Applied A4 contact followed by interruption: physical history persists and compensation is new event.
6. Material ID same but state revision changes: old grant invalid, contact history material identity preserved.
7. Journal full or torn at publication: no visible grant, no partial balance/body mutation; restart replay tests belong to B5 but B2 gate must not misrepresent them.
8. Eligibility expires while player view predicts success: authoritative rejection and visual correction; observer does not grant authority.
9. A4 moving support changes velocity while B2 eligibility rests on static assumption: PENDING/OUT_OF_SCOPE; B3 proves movement.
10. Retry same action after rejected attempt: bounded retry with new, version-bound witness; no silent carryover.

For every fixture record: source SHA, seed, actor/target/action, S_t, contract and material epochs, eligibility result/reason, arbitration order, publication/WAL result, S_t+1, physical receipt (if any), contact history evidence, visual projection, latency and whether case belongs to B2 or a later milestone.

## Time and scope budget policy
*Build closure first:* real A6 owner integration; revision-safe eligibility; no side-effect failures; local atomic handoff; representative canonical/held-out fixtures. The correct exit is a verified B2 interface and protected downstream contracts, **not** full B3 physical action or B5 multiplayer rollback.
*Research only unless demonstrated necessary:* exotic temporal logics, a universal encounter ontology for all subsystems, distributed multi-world WAL, optimal fairness game theory, fine-grained physics scheduler rearchitecture, additional microbenchmark specialization.
*Promote refinements only with observed regression or downstream blocker:* M4 vs M3 frame-time cost, contention/backlog evidence, material cache invalidation risk. Make each proposal show impact, uncertainty, counterevidence and optional rollback plan.

## RET classifications
A priori under adopted specification: eligibility is not realization; conjunction of necessary physical and contractual checks.
Formally testable: deterministic event transition validity, unique grant identity, snapshot epoch preconditions, conservation under atomic publication.
Empirically pending: actual A6 entry-point trace from intent through disposition, crash/no-effect and partial-state gates, frame and UX responsiveness.
Highly predictable: expired/stale grants rejected before costly realization; early physical rejection economizes work when negative cases dominate.
Uncertain: frequency of interruption, impact of extra revalidation, fairness over extended contention and cache invalidation overhead.
Unresolved: original B1 H1 source (#37), safe B1 H2 promotion, B2 live owner integration, end-to-end acceptance.

## Codex next action
Implement ONLY the narrow encounter transition tracer and assertions around the already-existing PR #42 evaluator/resolver and the actual A6 server owner when integration lands. Make it an opt-in scoped adapter and avoid an independent global world state machine. Add the ten above fixtures as test cases. Run M3/M4 with identical snapshots and report **execution evidence separately from authorization evidence**. Mark PR #42 HOLD until live binding, publication and corresponding mandatory B2 gates pass.
