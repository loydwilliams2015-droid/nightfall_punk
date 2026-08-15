# nightfall!punk — v1.5 Decisions Q265–Q266

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

Q267 remains open for disposition.
