# nightfall!punk — v1.5 Nested-Account Tactical Decisions Q1019–Q1023

Status: **LOCKED DESIGN CONTRACTS — ACTIVE BUILD**
Branch: `build/v1.5-combat-ai-communications`

## Q1019 — KEEP: Constitutive Nested Viability

Use a hard actor-viability floor. Above that floor, Crew continuity competes strongly with actor continuity but does not automatically dominate it. Role, relation history, current obligation, objective stakes, and future options may shift the balance.

`CatastrophicSelfRisk -> strong rejection`

Above the floor:

`ActorContinuity <-> CrewContinuity`

The actor is neither a disposable squad token nor an isolated self-maximizer.

## Q1020 — KEEP: Metastable Option Value

Future-option diversity/reversibility is a meaningful secondary strategic term whose weight rises with uncertainty and irreversibility, and falls when immediate objective pressure becomes decisive.

Conceptual relation:

`OptionWeight = F(Uncertainty, Irreversibility, EnergyMargin, ObjectiveUrgency)`

Preserve enough alternative possibility to remain adaptive without rewarding endless indecision.

## Q1021 — KEEP: Bounded Relational Obligation

Implement a very small reciprocity/history component based only on concrete recent interactions such as:

- assistance;
- revival;
- yielding;
- resource support;
- support for a claim/commitment;
- failed or abandoned coordination.

These feed bounded relation terms with decay. Do not introduce a universal morality or scalar trust system.

## Q1022 — KEEP: 12-Candidate Hotspot-Pruned Tactical Cap

Use a default fixed tactical candidate cap of **12 per actor after Priority Hotspot / relevance pruning**.

The cap remains an implementation constant so 8/12/16 can be compared later, but 12 is the canonical first v1.5 baseline.

The candidate list may include a bounded mix of attack, cover/reposition, guard, withdraw, investigate, assist, and route/resource interactions; no action class is guaranteed slots when irrelevant.

## Q1023 — KEEP: Causal Account Trace for F4

Use compact ranked candidate rows plus a small account-contribution strip for the selected action.

Example diagnostic shape:

```text
1 GUARD ROUTE_12      +0.67
  ACTOR    +0.10
  RELATION +0.08
  CREW     +0.22
  LOCALITY +0.31
  OPTIONS  +0.18
  COST     -0.22

2 ATTACK PLAYER       +0.51
3 INVESTIGATE WEST    +0.46

SELECTED: GUARD
WHY: route + Crew continuity > pursuit gain
COMMIT: retained, margin +0.16
```

The diagnostic must communicate causal contribution rather than suggesting that one magical scalar is the AI's complete reasoning.

## Closure consequence

Q1019–Q1023 close the remaining nested-account tactical uncertainties for this sweep.

The implementation target is now:

`Actor-local belief`
`-> nested tactical snapshot`
`-> <=12 hotspot-pruned feasible candidates`
`-> hard feasibility/viability gates`
`-> normalized account deltas + uncertainty + opportunity cost`
`-> commitment/hysteresis`
`-> intent`
`-> snap_locomotion / combat / interaction`
`-> embodied consequence`

The main remaining work in this area is implementation and tuning, not architecture.
