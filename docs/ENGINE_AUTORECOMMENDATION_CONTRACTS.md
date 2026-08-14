# nightfall!punk — Engine Autorecommendation Contracts

Date: 2026-08-13
Base: v1.2 human-video candidate `e65e19e931794fbe2f7e7f359b982df6cc10d4de`
Scope: engine/design policy only; this branch does not alter the frozen v1.2 video candidate.

## Purpose

Future engine work should not repeatedly reopen choices that are already deductively implied by accepted contracts or inductively very likely from successful tests, project constraints, and established game-development practice.

Decision rule:

1. **DEDUCTIVE DEFAULT — AUTO KEEP** when a choice follows directly from already accepted architecture and does not conflict with a stronger contract.
2. **HIGH-LIKELIHOOD DEFAULT — AUTO KEEP + MEASURE** when a choice is strongly supported by the architecture, existing implementation, comparative evidence, and computational economy. Implement the smallest compatible form and verify it.
3. **MID/HIGH UNCERTAINTY — QUESTION SUBROUTINE** when two or more materially different choices remain plausible and would significantly alter feel, performance, architecture, player agency, or future extensibility.
4. **LOW-VALUE UNCERTAINTY — DEFER** when the choice does not affect the current causal proof or next useful build.

Default implementation grammar:

`ExistingSystem -> CompatibleExtension -> MeasuredImprovement`

and:

`Add consequence before adding machinery.`

## Canonical movement/control stack

Reynolds's useful three-way separation is retained, but translated into nightfall!punk's architecture:

`Relational Action Selection -> Semantic Steering -> snap_locomotion -> Embodied Arcade Locomotion -> Collider/Material/Physics Truth -> World Trace -> New Evidence`

Definitions:

- **Relational Action Selection** decides what relation an actor is trying to produce from legitimate evidence, bounded belief, biography/personality, relationships, Crew convention, energy, utility, commitments, gradients, and Priority Hotspots.
- **Semantic Steering** converts action commitment into situated spatial intention without becoming physics or pathfinding truth.
- **snap_locomotion** is the canonical replacement for the retired term **Fuzzy Rail**. It is the high-authority, smoothed affordance-capture regime inside the broader Embodied Arcade Locomotion contract.
- **Embodied Arcade Locomotion** realizes steering through capability, energy, contamination, momentum, posture, current movement state, and existing movement machinery.
- **Physics Truth** resolves actual contact, obstruction, material interaction, and resulting motion/consequence.

Canonical maxim:

> Intent selects; steering directs; `snap_locomotion` captures; physics resolves.

Secondary maxim:

> Arcade intention, physical negotiation, systemic consequence.

## `snap_locomotion` contract — AUTO KEEP

`Snap locomotion` must never mean teleporting intention directly into success.

Positive chain:

`MovementIntent -> LegitimateAffordance -> CaptureCandidate -> SmoothedCapture -> EmbodiedCapability -> Energy/Contamination/Momentum -> ExistingMovement -> PhysicsResolution -> Outcome`

Hard guards:

- No capture without a legitimate affordance contract.
- No affordance annotation can manufacture physical possibility.
- No snap operation can bypass authoritative colliders/material truth.
- Material impossibility immediately overrides stale commitment or remembered possibility.
- Success may snap; failure must remain legible and flow into micro-adjustment, local reroute, broader reroute, reassessment, or abandonment.
- Capture should preserve movement purpose and momentum where compatible rather than forcing rigid authored animation rails.
- The system remains actor-capability-relative: the same geometry may afford different realizations to Player Crew, Human Rivals, Dream Cattlers, or future bodies.

Retired terminology:

- `Fuzzy Rail` — RETIRED.
- Canonical prose: **Snap Locomotion**.
- Canonical code-facing name where a dedicated identifier is needed: `snap_locomotion` / `NF_SNAP_*`.

Do not rename unrelated existing movement identifiers merely to satisfy terminology. Rename only when the concept becomes a real code-facing contract.

## Relational Action Selection — AUTO KEEP

Do not create an omniscient master planner.

Required causal interface:

`LegitimateEvidence -> SituatedBelief -> Biography/Personality/Relationship -> Need/Commitment/CrewConvention -> PossibilitySpace -> PriorityHotspotFilter -> GameTheoreticUtility -> ActionCommitment -> MovementIntent/InteractionIntent`

Rules:

- World truth is not automatically actor knowledge.
- Shared information is not shared interpretation.
- Existing pathfinding/navigation supplies viable traversal candidates; action selection evaluates situated meaning.
- Priority Hotspots regulate attention before expensive global comparison.
- Commitment supplies temporal continuity but does not override material impossibility.
- Utility is plural and situated, not one permanent scalar of goodness.
- Strategy is relational: `Strategy_i->j = F(Disposition_i, Relationship_ij, Evidence_i, Context)`.
- No protected demographic metadata may be a behavioral shortcut.

Likely future interface, implement only when a concrete consumer exists:

`NfActionIntent { type, subject, spatial_relation, urgency, commitment, evidence_tick }`

Do not add a large planner merely because an enum exists.

## Steering — AUTO KEEP

Existing pathfinding and locomotion remain authoritative in their own domains.

`Pathfinding selects viable traversal representation; steering realizes local spatial purpose; physics resolves embodied motion.`

Use industry-standard geometric machinery first:

- navmesh / graph search for geometric reachability;
- path corridors / funnel-like refinement for local path realization;
- bounded local avoidance / occupancy coordination;
- hierarchical search only when map scale makes it measurably useful;
- dynamic/replanning only when changed traversability or material state invalidates current assumptions.

Spend novelty budget on:

- actor-relative evidence;
- relational route evaluation;
- Priority-Hotspot Gradients;
- bounded memory;
- energy accounting;
- Crew negotiation;
- semantic affordance capture;
- world traces and consequence.

Never add a second navigation authority to create novelty.

## Embodied Arcade Locomotion — AUTO KEEP

High-authority arcade control is a design requirement, not an excuse to ignore material relations.

Conceptual realization:

`DesiredMotion x Capability x Energy x Contamination x SurfaceRelation x MomentumCondition x Affordance -> RealizedLocomotion`

Implementation should preserve the existing responsive movement controller and add constraints as compatible modifiers or affordance contracts rather than replacing the controller with universal force-driven character simulation.

Automatic defaults:

- Player/AI locomotion should share fundamental body/movement contracts where practical.
- Different bodies may realize the same abstract steering vocabulary differently.
- Movement degradations should alter capability relations rather than simply subtract generic speed.
- Energy costs should participate through concrete affordances/actions instead of becoming a decorative stamina meter everywhere.
- Momentum should matter where it changes the legitimacy, cost, or quality of traversal, not as compulsory realism on every frame.

## Physics architecture — HIGH-LIKELIHOOD DEFAULTS

No single comparison game is the target. The intended synthesis is selective:

- **Sunset Overdrive / Titanfall / Dying Light lineage:** movement authority and contextual traversal capture.
- **Death Stranding lineage:** terrain negotiation and embodied traversal consequence.
- **Prey / Tears of the Kingdom lineage:** systemic affordances and world transformation.
- **Exanima lineage:** meaningful embodied/contact physics.
- **Oxygen Not Included lineage:** material-state and energy consequence.

The engine should combine the useful relation, not reproduce each simulation wholesale.

### Contact/body physics

Default:

- retain conventional collision broadphase/narrowphase/contact resolution;
- use responsive character bodies for Player Crew/Human Rivals;
- use rigid-body or stronger dynamic simulation selectively for objects where momentum/contact materially matters;
- reserve active-ragdoll/full physical embodiment for cases where it adds reliable gameplay consequence rather than spectacle alone;
- collision/body truth remains authoritative over snap/steering intention.

### Material/world simulation

Default:

Use **bounded relational fields and object-local state**, not universal per-voxel simulation.

Prefer:

`Object/Surface/Volume LocalState + SparseFields + Events + History`

before:

`FullContinuousWorldSimulation`.

Candidate materially consequential domains:

- contamination transport;
- water/puddles/drainage;
- heat/temperature where gameplay-relevant;
- electrical/mechanical state where gameplay-relevant;
- breakage/deformation where it changes affordances;
- infestation/ecological state;
- acoustic/visibility/lighting fields where they materially affect perception.

Every simulated quantity should have a causal consumer. If removing a simulation variable does not change legitimate future possibilities, it should probably not exist yet.

### Physics novelty budget

Automatic policy:

> Extend proven machinery; do not compete with it unnecessarily.

Use conventional physics for geometric/contact problems. Spend project-specific machinery on relational interpretation and persistent consequence.

## World transformation — AUTO KEEP

General law:

`Topography -> Affordance -> Action -> TopographicTransformation -> NewAffordance`

`ALTER_ROUTE` is the first bounded proof, not a one-off mechanic.

Future transformable objects should satisfy the same affordance contract:

`ActorCapability + ObjectState + SpatialRelation + Energy -> Affordance -> EmbodiedAction -> AuthoritativeWorldChange`.

Do not generalize into unrestricted destruction until a concrete gameplay relation requires it.

## Memory and causality — AUTO KEEP

Actor cognition uses bounded, provenance-bearing memory.

`ObjectiveEvent != ExperiencedEvent != RememberedEvent`.

Working record remains small:

`subject, location, observed_state, source, confidence, observed_tick, freshness, consequence`.

Memory weight:

`MemoryWeight = F(Recency, Consequence, Relevance, Repetition, Contradiction, UnresolvedCommitment)`.

Default flow:

`Event -> RecentPriorityMemory -> Compress/Consolidate/RemainUnresolved/Decay`.

Persist consequences, not decorative micro-history.

## Game-theoretic C-programming method — AUTO KEEP

Prefer explicit, bounded state and ordinary C data structures over opaque generalized frameworks.

Default implementation style:

- small enums for meaningful mutually exclusive modes;
- structs for situated state, evidence, memory, claims, and contracts;
- bounded arrays/rings instead of unbounded allocation where scale is known;
- deterministic functions where possible;
- fixed or measured update budgets by subsystem;
- event-driven updates for sparse consequences;
- explicit provenance/confidence when information is epistemic;
- server-authoritative material/world truth;
- client prediction only where responsiveness requires it;
- separate physical truth, predicted claims, social resolution, and actor belief;
- assertions/tests for negative controls, not only successful reactions;
- diagnostic traces should explain causal decisions rather than dump every variable.

Game-theoretic evaluation should usually take the form:

`CandidateStrategy -> ExpectedSituatedCost/Benefit -> OtherActor/WorldResponse -> Persistence/Revision`

not a universal minimax tree.

Use frequency dependence, resource pressure, relationships, commitments, evidence, and ecological state only where they have actual causal relevance.

## Performance / complexity law — AUTO KEEP

> Simple complexity, not over-complex simplicity.

Implementation order:

1. reuse existing machinery;
2. add the smallest state needed to express the missing relation;
3. establish a causal proof;
4. measure cost and failure modes;
5. generalize only after repeated consumers appear.

Hierarchy supplies alternatives; gradient determines pressure; memory preserves consequence.

Priority Hotspots regulate where computation and attention should concentrate.

Avoid full-world expensive recomputation for local changes when a smallest-sufficient update is possible.

## Automatically recommended future directions

Unless contradicted by testing, automatically prefer these when their dependencies become relevant:

- shared abstract action/steering vocabulary with body-specific locomotion realization;
- bounded `snap_locomotion` capture windows rather than authored hard rails;
- path-corridor/local-steering compatibility rather than exact path following;
- material impossibility breaking commitment before high-level replanning;
- local reroute before wider reroute;
- sparse/event-driven world fields;
- object-local material histories where persistent consequence matters;
- actor-relative evidence instead of global AI flags;
- compressed persistent expectations instead of giant episodic logs;
- conventional collision/navigation foundations plus project-specific relational layers;
- negative-control tests for non-omniscience;
- deterministic/reproducible seeds for causal prototype slices;
- instrumentation that exposes `CAUSE -> INTERPRETATION -> ACTION -> CONSEQUENCE`.

## Question-subroutine trigger

Ask exactly three questions when unresolved choices are mid-to-high uncertainty and materially important. Where useful, each question should provide up to five positions:

1. Thesis
2. Antithesis
3. Original nightfall!punk concept
4. Industry-standard approach
5. Recommended Synthesis

Use **gradient** to avoid false binaries and **memory** to preserve consequences of prior choices.

Do not ask questions for choices already covered by AUTO KEEP unless new evidence creates a real conflict.

## Current mid/high-uncertainty frontier

The following remain intentionally unresolved enough to require design questions before broad implementation:

- how physically coupled humanoid bodies should be to dynamic-object forces beyond current collision/movement truth;
- how much material-field simulation should operate continuously versus only around active Priority Hotspots;
- how strong and permissive `snap_locomotion` capture should become across vault/mantle/ladder/ledge/object interactions without reducing failure legibility or player agency;
- later: degree of destructibility, active ragdoll, fluid fidelity, multi-actor force exchange, and parity/difference between Player Crew and non-player bodies.

These are question-subroutine topics, not automatic implementation mandates.
