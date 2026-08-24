# nightfall!punk v1.7B — Encounter Ecology & Navigation (tentative)

## Working thesis
v1.7B should convert the spatial possibilities proved by v1.7A into bounded, historical, actor-relative use of space. An actor should not merely ask `which adjacent cell scores highest?`; it should maintain uncertain route hypotheses, estimate future options and costs, revise after events, and participate in an ecology whose trails, populations, contracts, and encounter pressure persist independently of the player.

Canonical direction:

`material world -> actor-relative topology -> observation/belief -> route hypotheses -> bounded game-theoretic appraisal -> commitment -> action -> event -> spatial/ecological/social memory -> revised hypotheses`

## Reference lessons — use, do not imitate
### Rain World
- Creature capability profiles make reachability and route preference actor-relative.
- Loaded local pathing and abstract room/exit connectivity can share one conceptual route grammar at different resolutions.
- A path can be impossible in full but still useful as a partial/closest-reachable approach; the AI can know whether its current route can actually complete.
- Actor knowledge of targets can be incomplete and revised through observation.
- Multiple routes and soft room attractiveness allow ecology to be influenced by level structure without reducing creatures to fixed room obstacles.
- Autonomous populations should have reasons to move that are not simply `attack player`.

### Sebastian Lague — Coding Adventure: Ant and Slime Simulations
Reference video: https://www.youtube.com/watch?v=X-iSQQgOd1A
Reference implementation: https://github.com/SebLague/Ant-Simulation

Useful abstractions:
- local sensing rather than centralized route command;
- deposited directional/history fields;
- positive feedback from repeated successful traversal;
- decay/evaporation so obsolete paths lose influence;
- bounded randomness/exploration so alternatives can be discovered;
- simple local rules can create population-scale route structure.

Counterpoint / nightfall!punk adaptation:
- trail fields are evidence/history, not truth, authority, telepathy, or commands;
- human-like actors should not become pheromone-following insects;
- positive feedback needs saturation, decay, congestion/contestation cost, novelty/exploration, and actor-specific utility to prevent route monoculture;
- population fields, private memories, and social/shared artifacts require different ownership and visibility contracts;
- material legality always outranks field preference.

## Four-quarter development sketch

### Quarter 1 — Route intelligence + historical fields
Goal: solve the five v1.7A ACTOR_APPRAISAL_GAP cases and reduce the LOGIC_ONLY/perceptual gap without adding omniscience.

Deliverables:
- actor traversal/capability profile;
- typed route-legality/preferences;
- bounded rolling-horizon route hypotheses;
- partial-path/closest-reachable behavior;
- decomposed route-cost vector;
- actor-private route memory;
- typed decaying trail/gradient fields;
- MICRO/MESO route abstraction bridge;
- new multi-step AI control policy;
- regression suite built from v1.7A pathological/disagreement seeds.

### Quarter 2 — Persistent populations + realization
Goal: make ecology own persistent population processes rather than treating ecological risk/resource as static local scalars.

Tentative work:
- fixed-capacity population reservoirs by region/capability/ecological role;
- abstract migration using MESO connectivity;
- habitat/resource attraction as soft bias, not confinement;
- local realization/de-realization contracts;
- resource depletion/regeneration and habitat history;
- population-level trail fields distinct from actor-private memory;
- explicit birth/recruitment/migration/departure accounting where applicable.

### Quarter 3 — Encounter ecology + social/contract appraisal
Goal: allow player, AI and persistent populations to create encounters through lawful spatial/ecological opportunities.

Tentative work:
- encounter-pressure governor constrained by population and topology legality;
- pursuit, retreat, waiting, yield, observe, flank, defend, forage and migrate as competing actions;
- role-relative contract utility: steward, guest, claimant, maintainer, thief, forager, rival;
- group reports and shared artifacts without telepathy;
- congestion/territory/contestation costs;
- quiet/intense pacing and pressure release.

### Quarter 4 — Integrated stress + handoff to 1.7C
Goal: prove that actor ecology survives integration with movement, weapons, perception and pacing.

Tentative work:
- corridor/loop/vertical/cover/Snap/dynamic/ecology/contract combined cells;
- high-speed Snap + route-planning stress;
- weapon/muzzle/exposure reasoning during traversal;
- multi-population encounter soil samples;
- instrumented vs perceptual vs multi-step vs greedy vs random comparisons;
- OITOA + PAR + pathological seed gallery;
- native interactive viewer/play pass;
- freeze successful contracts and hand off district synthesis to 1.7C.

## Quarter 1 — detailed to-do list
- [ ] Define `NfTraversalProfile` or equivalent capability contract: ground, ladder, mantle, vault, drop, platform/support transitions, actor envelope, speed/energy restrictions.
- [ ] Define route legality as a typed relation, not a scalar: legal/preferred, legal-costly, conditionally legal, partial-only, blocked/unknown.
- [ ] Add actor-relative route queries over the v1.7A Universal Cell/portal substrate.
- [ ] Add a bounded rolling horizon (initially KEEP+MEASURE; e.g. several cells/portals rather than whole-map omniscience).
- [ ] Define a decomposed `RouteCostVector`: progress, exposure, ecological/energy cost, uncertainty, commitment, retreat quality, branch/option retention, affordance continuity, contract relevance, trail evidence.
- [ ] Preserve vector components in logs before any utility reduction; no opaque universal `best_route` scalar.
- [ ] Add partial-path semantics: move toward the best reachable frontier while explicitly recording whether the intended goal is reachable, uncertain, or impossible under current beliefs/capabilities.
- [ ] Add route commitment/hysteresis so actors do not thrash between nearly equal alternatives every tick.
- [ ] Add replanning triggers: new observation, topology revision, material obstruction, threat evidence, resource change, contract event, route failure, large utility reversal.
- [ ] Add actor-private route memory keyed to spatial IDs with age, provenance/confidence and experienced cost.
- [ ] Generalize weighted gradient memory into typed trail fields with explicit owner/visibility policy.
- [ ] Start with a deliberately small trail-channel experiment: resourceward, shelter/homeward and danger/contestation; treat names as provisional.
- [ ] Implement deposit -> local diffusion/topological propagation -> saturation -> decay/evaporation with deterministic budgets.
- [ ] Add bounded exploration/noise so actors/populations can discover alternatives; measure rather than maximize randomness.
- [ ] Add anti-lock-in controls: decay, saturation, congestion cost, contested-route cost, novelty bonus and route diversity metrics.
- [ ] Implement MICRO local path detail + MESO portal/exit connectivity using one route-cost interface with different resolution.
- [ ] Keep exact world/material topology separate from actor-known topology and trail history.
- [ ] Add new `multistep` AI-player control policy to the existing `informed-local`, `greedy`, and `random` controls.
- [ ] Promote the five v1.7A ACTOR_APPRAISAL_GAP seeds and `loop/best/182812` LOGIC_ONLY seed to named regression fixtures.
- [ ] Re-run at least median/best/worst/pathological/disagreement soil strata after every material route-planning change.
- [ ] Add ablation tests: no trail, no decay, no exploration, no option retention, no uncertainty, no retreat value.
- [ ] Reject any design where a trail or contract-memory gradient directly changes authority, movement intention, or material reachability.
- [ ] Keep graphical referee output separate from actor input; add interactive seed stepping only as a diagnostic convenience.
- [ ] Define Quarter-1 acceptance numerically after first implementation data rather than tuning targets in advance.

## Quarter 1 acceptance questions
1. Does bounded multi-step appraisal beat or meaningfully differentiate from greedy control specifically on loop/dynamic/corridor pathological seeds without degrading the strong cover/vertical/Snap cases?
2. Can an actor pursue a partially unreachable goal, recognize the limit, and rationally switch when a better reachable opportunity appears?
3. Do trail fields improve route discovery or persistence without creating self-reinforcing route monoculture?
4. Does removing decay or exploration produce predicted pathological behavior in negative controls?
5. Can MICRO and MESO routing produce coherent handoffs without giving the actor exact remote tactical knowledge?
6. Can topology changes invalidate a committed route and trigger bounded replanning without thrashing?
7. Can two actors with different traversal profiles rationally derive different legal and preferred routes through the same geometry?
8. Does the perceptual/player channel contain enough information to explain the multi-step actor's better choices after referee replay?

## Guardrails
- `material legality != route preference`
- `trail field != truth`
- `historical weight != authority`
- `shared field != shared omniscience`
- `reachable != desirable`
- `shortest != best response`
- `best response != permanent commitment`
- `encounter pressure != spawn authority`
- `population pattern != individual intention`

## Handoff target
Quarter 1 should end with an executable route-intelligence laboratory, not a district. Quarter 2 earns persistent ecology; Quarter 3 earns encounter orchestration; Quarter 4 integrates and validates. 1.7C begins only after these relations survive combined spatial/AI/player evidence.