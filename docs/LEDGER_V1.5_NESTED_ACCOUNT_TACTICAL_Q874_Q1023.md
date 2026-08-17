# nightfall!punk — v1.5 Nested-Account Tactical Implementation Sweep Q874–Q1023

Status: **PRE-BUILD WEAK-POINT CONSOLIDATION — NO COMPILE**
Branch: `build/v1.5-combat-ai-communications`

## Result

150 dependency-ordered questions were evaluated.

- 145 automatically resolved / invalidated / deferred
- 5 remain open: Q1019–Q1023

## Source-level starting point

The current codebase already contains substantial machinery that nested tactical implementation should compose rather than replace:

- `nf_energy`: concrete actor accounts (`reserves`, `income`, `expenditure`, `productive_capacity`), regional energy potential/capacity, uncertainty-aware `NfEnergyBelief`, topographic cost, and bounded energy-opportunity search.
- `nf_tactical`: currently thin geometric helpers for exposure and cover utility.
- `nf_claim`: fixed-capacity temporal claims for cover, traversal, interaction, and revive.

Therefore:

`Existing Energy / Geometry / Claims -> Nested Tactical Projection -> Candidate Appraisal -> Commitment -> Existing Embodiment`

not:

`New Parallel Energy Simulation / Planner`

## Canonical tactical projection

Use a bounded heterogeneous projection:

`E_actor + E_relation + E_crew + E_locality + E_future_options`

These are strategic account dimensions, not literally one conserved physical quantity. Hard feasibility and catastrophic viability constraints occur before normalized scalar comparison.

### Actor

Includes agency, contamination-domain function, metabolic/resource reserve/flow, ammo/capability, immediate exposure, and access to recovery.

Health remains relational functional viability; energy remains metabolic/resource accounting. Equal health does not imply equal future sustainability.

### Relation

Includes threat, obligation, reciprocity/history, assistance/revival relation, role expectation, coordination value, and escalation cost. No morality meter or automatic revenge score.

### Crew

Actor-relative known Crew viability, positions, resources, roles, claims, route/objective continuity, and assistance opportunities. No faction-wide omniscient squad scalar.

### Locality

Routes, cover/exposure, resource state, object revisions, claims, passage objective, ecological/Cattler pressure, and information vantage value relevant to current hotspots.

### Future options

Cheap proxies only: reachable affordances, safe routes, cover/escape, resource access, information access, claims, Crew support, metabolic sustainability, bounded diversity, and reversibility. No exhaustive action tree or Monte Carlo planner.

## Candidate architecture

Recommended compact C shape:

- fixed-capacity `NfTacticalCandidate` array;
- action kind;
- subject/target key;
- destination/affordance;
- feasibility flags;
- normalized account deltas;
- explicit costs / uncertainty / opportunity cost;
- final bounded score;
- reason bits / diagnostic provenance.

Candidate evaluation must be side-effect-free. It may not spend energy, acquire claims, publish reports, or directly move actors. After selection/commitment, existing claim / locomotion / combat / interaction systems embody the action.

Supported first-pass interactions remain:

- attack;
- reposition / cover;
- guard / secure;
- withdraw;
- investigate;
- assist / revive;
- route / resource interaction.

`Target = NONE` remains valid.

## Sustainable Passage / game-theory implementation

Appraisal is a bounded incomplete-information best response over actor-local beliefs.

Damage remains instrumental rather than the score axis. Opportunity cost is explicit. Investigation may dominate attack through value-of-information. Attack must still be able to dominate withdrawal when conflict is reproductively useful; the architecture must not collapse into pacifism.

Cooperation, compromise, accommodation, competition, and avoidance remain context-dependent strategic modes rather than moral rankings.

## Performance / C discipline

- fixed structs and candidate arrays;
- no per-tick heap churn;
- deterministic scoring / tie breaking;
- normalized bounded inputs;
- event-triggered + staggered tactical appraisal around ~1–5 Hz, to be measured;
- reflex/control remains faster;
- cache slow regional/regenerative horizons;
- detailed pathfinding only for shortlisted/selected intents;
- scoring is pure / side-effect-free.

## Test obligations

Must prove at minimum:

1. actor viability cannot be washed out by Crew gain;
2. equal health + different energy/resource state can yield different actions;
3. equal world + different actor beliefs can yield different actions;
4. route/resource transition can rationally beat a damage opportunity;
5. attack can beat withdrawal when sustainable;
6. withdrawal can beat attack;
7. guard can beat chase without becoming the universal attractor;
8. selected tactical intent produces actual physical consequences through existing embodiment machinery.

## Positive criticism / tuning warning

The deterministic Pac-Man harness' guard-heavy result is retained as useful tuning evidence. Do not remove Crew/route/future-option architecture merely to manufacture aggression. Measure action frequencies and score margins, then calibrate account weights and commitment thresholds.

## Five open questions

- **Q1019:** actor viability vs Crew continuity above hard safety floors.
- **Q1020:** strength of future-option diversity/reversibility vs immediate objective gain.
- **Q1021:** how explicit obligation/reciprocity should be in the first relation account.
- **Q1022:** first post-pruning tactical candidate cap per actor.
- **Q1023:** F4/human-proof presentation of nested-account reasons.

Vertical Integration Pass II remains deferred until these and the remaining weak-point sweeps are closed.
