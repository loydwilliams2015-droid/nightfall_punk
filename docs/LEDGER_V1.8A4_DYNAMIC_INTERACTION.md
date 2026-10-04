# nightfall!punk 1.8A.4 — Dynamic Interaction H1 handoff

**Source boundary:** This GitHub branch holds the authoritative architectural and evidence handoff only. The complete tested C implementation is in the downloadable 1.8A.4 source archive. Do **not** claim this GitHub branch is CI-certified or that it already runs the new physical solver.

## Preserved lock: 1.8A.3 Model C

Accept swept upright capsule + independently validated rounded-box-like foot support + simultaneous geometry/traversal jurisdiction as the **H1 laboratory exit candidate, not production player physics**. Both jurisdictions must approve and an external world transaction must commit. Authorizing a trajectory never teleports the actor.

## 1.8A.4 selection

Select **motor-driven reciprocal linear-and-angular contact with Coulomb-bounded friction and a 2→4→6 adaptive constraint budget** as the **H1 laboratory exit candidate**. The motor is a bounded, accountable external source of work. A contact cannot be called dynamic because it merely reports an estimated impulse: velocity/angular velocity must actually update both applicable bodies. Reject invalid input and duplicate contact IDs; retain explicit PENDING and rollback on contact-island budget exhaustion.

The 1.8A.3 trusted snapshot is re-adjudicated before staging a motor impulse. Verify actor ID, material geometry, world revision, authoritative pose/shape, feature permission, step/ladder authority and single-stage-per-tick. No direct world-state mutation is authorized by a mere candidate.

## H1 local evidence (not GitHub CI evidence)

- 56/56 strict-C assertions; AddressSanitizer/UndefinedBehaviorSanitizer pass.
- Inherited local suites: 1.8A.1 27/27, 1.8A.2 46/46, 1.8A.3 38/38 + production negative-control guard.
- 8,000 model evaluations on shared deterministic parameters: 1,000 two-body pair fixture sets; 1,000 matching island evaluations, plus 500 held-out three-body chain sets.
- Direct pair contact momentum residual <=0.00024414 kg*m/s, zero within-process replay mismatches in declared corpus.
- 1,000 simple island fixtures: fixed six constraint evaluations vs adaptive one; 500 chain fixtures: fixed 12 vs adaptive 11.272 average. The selected mixture saves 44.7% of constraint evaluations, **not measured CPU time**. Max fixed/adaptive crate speed difference in chain roughly 0.001245 m/s.
- Dense eight-body chain explicitly PENDING with caller-state rollback; 0.005 m/s provisional normal-speed convergence criterion.
- Actual applied contact receipt may be passed into 1.8A.2's condensed history, but only after separate world transaction commit; durable event persistence remains future work.

## What remains unbuilt

Continuous stair/ladder motor path realization, rotating collider geometry/angular CCD, orientation integration, warm-started persistent friction, full 3D stacked rigid-body solver, dynamic support contracts, fine chunk material authority, faithful native server/client reconciliation, verified cross-platform fixed-point replay, game camera/weapon integration, H3 hardware benchmarks, H4 human trials, durable contact-event store.

The full project's global CMake configure could not complete in the isolated container because ENet's external dependency could not be fetched; it must not be recorded as PASS or as a code failure.

## Method

Conditional a priori hard core: genuine material contact, geometry AND traversal, reciprocal impulses, bounded conserved contact constraints, explicit pending without partial commit. H1 compiled tests support this scope, while relative efficiency, human feel and production suitability remain contingent and defeasible. Keep fixed six and angular-normal comparators, estimate-only negative control, and severe future falsification for low ceilings, rotated crates, deep stacks, high mass ratios and same-tick multiplayer.
