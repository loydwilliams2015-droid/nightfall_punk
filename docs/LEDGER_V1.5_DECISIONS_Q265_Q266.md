# nightfall!punk — v1.5 Decisions Q265–Q267

Status: **LOCKED DESIGN CONTRACTS — PREBUILD**
Branch: `design/v1.5-frontier-contracts`

## Q265 — KEEP: Legible Strategic Accounting

Use conventional player-readable HUD/objective grammar, but expose strategic tendencies and consequences rather than optimizer internals.

Normal play may communicate bounded states such as:

- energy flow: stable / falling / recovering;
- Crew viability: strong / pressured / critical;
- route security: improving / contested / lost;
- resource flow: surplus / balanced / deficit;
- future options: expanding / narrowing.

Diagnostic modes may expose DeltaE, capacity, opportunity cost, damage efficiency, and option value.

Governing rule:

**The mathematics remains authoritative without requiring the player to play the spreadsheet.**

## Q266 — KEEP + AMEND: Nested-Account-Derived Individual Tactical AI

Use synthesis E, but heavily amend the hierarchy: individual tactical AI is not an isolated optimizer with collective factors added afterward. Its tactical utility is derived through nested accounts from the outset.

Canonical account set:

`Accounts_i = { actor_i, relation_i-j, Crew_i, locality_i, slower ecological/world accounts where relevant }`

Therefore individual action evaluation is structurally relational:

`U_i(action) = F(ActorAccount_i, RelationAccounts_i, CrewAccount_i, LocalityAccount_i, Belief_i, Memory_i, CurrentState_i)`

The actor remains an individual with bounded perception, biography, disposition, energy, and immediate needs, but those needs are evaluated inside the reproduced relations that make action possible.

Examples:

- an individually costly action can be rational because it preserves a teammate, Crew route, or critical shared resource;
- an individually profitable action can be rejected because it collapses Crew viability or destroys the locality that reproduces future options;
- helping the Crew is not hive-mind obedience: the actor's relation to the Crew has its own weight, history, trust, obligation, uncertainty, and opportunity cost;
- locality/ecological accounts act as slower material constraints, not moral score multipliers.

### Strong derivation rule

**Individual tactical AI is a local expression of nested accounts, not an atomistic utility function plus squad bonuses.**

Implementation should still use bounded, efficient utility machinery, but its terms should be generated through account projections rather than hard-coded `self_score + squad_bonus` arithmetic.

Suggested pipeline:

`World/Relation State -> Actor-Legitimate Evidence -> Nested Account Projection -> Situated Utility -> Commitment -> ControlIntent -> Snap Locomotion / Combat -> Physical Consequence -> Account Revision`

This preserves individuality while making interdependence causal rather than decorative.

## Q267 — KEEP: Bounded Reproductive Conflict Appraisal

Use bounded utility machinery for continue / withdraw / reposition / exchange / detour / detente decisions, but derive each appraisal from the same nested-account and actor-knowledge architecture fixed in Q266.

A compact conceptual form is:

`U_continue = ExpectedObjectiveGain + ExpectedResourceGain + FutureOptionGain - EnergyBurn - CapacityRisk - Exposure - Uncertainty`

compared against the best currently believed alternative:

`U_alternative = max(Withdraw, Reposition, Exchange, Detour, Detente)`

The terms are not evaluated from server omniscience. They are conditioned by legitimate actor knowledge:

`Decision_i = F(Belief_i, Memory_i, NestedAccounts_i, Energy_i, Relation_i, CurrentState_i)`

### Strong continuation rule

**An actor continues conflict only insofar as, from its situated and bounded perspective, continued conflict still reproduces sufficient future possibility across the nested accounts that constitute its tactical world.**

Consequences:

- low health alone does not force retreat;
- high health alone does not justify pursuit;
- poor information can rationally increase withdrawal pressure;
- a mistaken but legitimate belief can sustain a fight longer than an omniscient optimizer would;
- Crew viability, relation-specific obligations, resource flows, locality conditions, and future-option value can outweigh immediate damage opportunities;
- retreat is not failure by definition: under Sustainable Passage / Reproductive Victory it can be the winning move when it preserves or improves long-horizon viability.

Implementation should remain bounded and efficient: use existing utility evaluation, hysteresis, memory, information topography, claims, and Snap Locomotion rather than adding a general planner.

Suggested conflict loop:

`Evidence -> Belief -> Nested Account Projection -> Continue-vs-Alternative Appraisal -> Commitment -> Embodied Action -> Consequence -> New Evidence / Account Revision`
