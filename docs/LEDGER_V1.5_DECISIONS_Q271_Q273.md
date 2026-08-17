# nightfall!punk — v1.5 Decisions Q271–Q273

Status: **LOCKED DESIGN CONTRACTS — ACTIVE BUILD**
Branch: `build/v1.5-combat-ai-communications`

## Q271 — KEEP: Causal Evidence Channels

Use familiar perception/stimulus categories as implementation primitives, but preserve their distinct epistemic meaning rather than flattening all combat information into a generic awareness event.

Canonical bounded evidence channels:

- `DIRECT_VISUAL`
- `AUDIBLE_GUNFIRE`
- `IMPACT`
- `DAMAGE_RECEIVED`
- `TRACE`
- `REPORT`

Each channel carries different legitimate precision, confidence, source identity, directionality, age, and provenance.

Examples:

- direct visual may justify fine positional precision and possibly actor identity;
- audible gunfire may justify only direction/region and uncertain source identity;
- received damage guarantees harm occurred but need not identify the attacker;
- report knowledge is bounded by its source provenance and may degrade through transmission.

Implementation doctrine:

`Industry stimulus primitive -> channel-specific epistemic contract -> actor belief`

## Q272 — KEEP: Meaningful Information Delta

AI communication is event-driven but not automatic blackboard replication. An actor reports when it believes new information materially changes another actor's possible decision.

Conceptual trigger:

`ReportValue = InformationChange * Relevance * Confidence`

Publish only when bounded report value crosses threshold, with cooldown/hysteresis to prevent redundant chatter.

Typical report triggers:

- first legitimate detection;
- meaningful position-region change;
- route/object revision;
- substantial confidence change;
- target entering/leaving an important tactical relation;
- materially important resource-state change.

The purpose of communication is to alter another actor's legitimate decision space, not to keep a shared database perfectly synchronized.

## Q273 — KEEP + AMEND: Ecological Relational Interaction Game

Replace a conventional kill-priority target table with a bounded ecological/game-theoretical interaction-selection architecture.

Canonical rule:

> **An actor does not ask merely “Which enemy should I attack?” It asks “Given what I legitimately know, which available interaction best reproduces viable future possibilities across the nested relations that constitute my lifeworld?”**

### General interaction set

`InteractionTarget in { Actor, Resource, Route, Object, Locality, InformationSource }`

Combat is one possible interaction class, not the universal organizing objective.

### Nested-account derivation

For actor `i`, candidate interaction target `j`, and action `a`:

`V_i(a,j) = E_Bi[ DeltaActor + DeltaRelation + DeltaCrew + DeltaLocality + DeltaFutureOptions ] - ActionCost`

where expectation is conditioned on `Belief_i`, not server omniscience.

Action cost includes energy burn, capacity risk, exposure, time, uncertainty, opportunity cost, and escalation cost.

### Best-response comparison

For candidate `j`:

`T_i(j) = max_a V_i(a,j) - max_b V_i(b where b does not interact with j)`

Only select an interaction target when `T_i(j)` exceeds the actor's bounded threshold/hysteresis. Otherwise `Target = NONE` is legitimate.

This prevents forced combat when withdrawal, repositioning, guarding, repair, information acquisition, resource control, or detente better reproduces viable future options.

### Ecological abstractions

The implementation should borrow mathematically from:

- optimal-foraging style profitability: expected return relative to energy/time/handling cost;
- contest theory: escalation depends on resource value versus expected contest cost;
- assessment under uncertainty: beliefs about opponent/state alter persistence and withdrawal;
- state-dependent ecological preference: interaction priority changes as densities, routes, resources, and conditions change.

These are abstractions, not literal organism simulation.

### Specialized combat appraisal

`RelationalTargetAppraisal subset_of EcologicalRelationalInteractionGame`

A bounded combat score may use familiar threat-table machinery, but its terms must be generated from nested relational accounts rather than a standalone kill-priority subsystem.

Compact form:

`U_i(j,a) = P_success * (ObjectiveGain + ResourceGain + CrewGain + RelationGain + FutureOptionGain) - (EnergyCost + DamageRisk + ExposureCost + TimeCost + UncertaintyCost + OpportunityCost)`

all terms conditioned by actor-legitimate evidence and belief.

### Governing pipeline

`World/Relation State -> Legitimate Evidence -> Belief -> Nested Account Projection -> Interaction Candidate Set -> Reproductive Value -> Commitment -> Combat/Movement/Communication -> Physical Consequence -> Account Revision`

This preserves standard bounded utility machinery while making the causal interpretation explicitly nightfall!punk.
