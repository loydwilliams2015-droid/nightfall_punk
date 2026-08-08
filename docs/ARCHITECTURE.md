# nightfall!punk architecture contract

## Durable stack

- C + raylib
- Linux-first
- fixed 60 Hz simulation
- authoritative dedicated server
- data-oriented / ECS-lite direction
- presentation observes simulation; it does not own gameplay state
- ENet is hidden behind `nf_net`, not exposed as gameplay logic
- combat is hidden behind `nf_combat`, not owned by renderer/network presentation
- AI, encounter-governance and spatial-ecology code have no raylib or ENet dependency
- fixed-capacity hot-path state is preferred over per-tick allocation

## Simulation ownership

The player camera and weapon viewmodel are presentation. Actor body, faction, relationship permissions, health, weapon state, ammunition, movement and death are simulation/server truth. The same `NfWorld` and movement functions execute on client prediction and server authority; combat outcomes execute authoritatively on the server and recover through snapshots.

Movement states: GROUND, SPRINT, CROUCH, AIR, LADDER, VAULT, MANTLE and PLATFORM.

Weapon states: READY, RECOVERING, RELOADING, SWITCHING and EMPTY.

## Shared control contract

Network players and server AI converge at the same control vocabulary before movement/combat:

`network packet -> validated NfControlFrame -> NfMoveInput + NfCombatInput`

`AI perception/decision -> NfControlFrame -> NfMoveInput + NfCombatInput`

AI is therefore **not** a fake network client and does not own a privileged damage or traversal path. Human and AI control sources submit intent; authoritative movement/combat systems decide outcomes.

v0.7 inserts spatial situated movement as a bounded **control filter**, not a replacement controller:

`world semantics -> spatial context/task -> ordinary AI control -> spatial movement filter when AI is free to roam -> encounter governance -> movement/combat -> truth`

The spatial layer does not overwrite active combat/cover/retreat decisions merely because a distant region has a higher score.

## Network truth contract

The client sends **intent**, not outcome.

`input -> local movement prediction -> ENet input packet -> authoritative server movement/combat -> snapshot + combat event -> reconciliation/presentation`

- server simulation: 60 Hz
- client input/prediction: 60 Hz
- snapshot publication: 30 Hz
- reliable channel: handshake/control + important discrete combat transitions
- unreliable/state channel: high-frequency input, snapshots and transient combat feedback
- input history: 256 commands
- redundant input bundle: latest 3 commands
- current network-player cap: 4

Protocol v4 (`NF04`) continues to carry fire/reload/switch/aim intent, authoritative combat state in snapshots and explicit server-authored combat events. v0.7 does **not** replicate whole AI field maps or per-agent tactical scratch state.

## Current faction topology

- network slot 0 = PLAYER
- additional network slots = TEAMMATE
- server Human Rival agents = RIVAL
- Dream Cattler implementation currently retains the technical `RANCHER` faction identifier pending a coordinated rename

Faction identity remains distinct from relationship state. Player/Teammate is cooperative. Human Rival relationships may move through hostility, contest, truce and cooperation. Dream Cattlers remain non-negotiable whether a particular disposition is predatory or the rare hostile/non-predatory avoidant type.

Damage permission is resolved through central relationship/faction policy before authoritative hit application. Friendly fire remains off by default.

## Combat authority

The client and AI controller never submit a target, hit zone, damage value, death result, ammo value or successful-shot result.

A shot is resolved as:

`fire intent -> server validates actor/weapon/state/ammo/rate -> reconstruct aim -> bounded history rewind -> world occlusion + actor hit test -> relationship damage permission -> authoritative damage/death -> combat event + snapshot recovery truth`

The current hitscan layer has BODY and HEAD zones. Carbine and pistol remain intentionally simple graybox weapons used to prove combat/AI substrate before recoil/spread/ADS/projectile complexity.

## Lag compensation

The server stores a 64-frame actor-position history. Hitscan rewind is clamped to 12 ticks (~200 ms). Only target geometry is historically sampled; server truth remains current for weapon state, ammo, relationship rules and damage application. Applied rewind duration is surfaced in combat-event diagnostics.

## World Semantic Alerts

Combat events are promoted into fixed-capacity server-authored semantic facts. Current/declared vocabulary includes:

- GUNFIRE
- DAMAGE_TAKEN
- ACTOR_DIED
- ACTOR_RESPAWNED
- ENEMY_SEEN / ENEMY_LOST scaffolding
- SUSPICIOUS_SOUND
- AFFORDANCE_RESERVED / AFFORDANCE_RELEASED scaffolding
- OBJECTIVE_CHANGED
- ROUTE_CHANGED
- STORY_PHASE_CHANGED

Critical AI truth is never inferred from packet arrival/loss.

v0.7 consumes semantic alerts to change regional **activity**, **incident history**, **objective relevance** and interruption urgency. Story/objective/route producers are scaffolding until the relevant gameplay systems exist; the semantic vocabulary is present so those systems can later affect AI without bypassing normal reasoning.

## Epistemic AI contract

The server owns reality, but AI does not automatically know reality:

`server truth != individual knowledge != squad report`

Human Rival perception remains bounded by range/facing/world occlusion. Agents remember last-seen and last-heard positions, attach confidence, decay stale knowledge and may receive lower-confidence secondhand reports. Spatial fields that depend on an enemy are calculated from this **perspectival evidence**, not hidden authoritative transforms.

This separation is required for search, surprise, deception and non-psychic pursuit.

## Utility / decision contract

Ordinary tactical decisions remain bounded utility modes:

- IDLE
- INVESTIGATE
- ADVANCE
- ENGAGE
- SEEK_COVER
- RETREAT
- RELOAD
- TRUCE_HOLD

The subsystem questions remain deliberately non-overlapping:

- **Perception:** what do I know?
- **Utility:** what do I want?
- **Affordances:** what local opportunities exist?
- **Spatial ecology / region graph:** which part of the arena should I inhabit or traverse toward?
- **Navigation/local steering:** how do I approach it?
- **Fuzzy Rail:** how do I physically negotiate immediate geometry?
- **Action execution:** what control input now?
- **Squad blackboard:** what has the group reported/claimed?
- **Encounter governance:** how much simultaneous certainty/lethal pressure reaches execution?

General GOAP remains reserved for later compound multi-step goals.

## Dynamic Affordance Graph — v0.7 slice

v0.5 introduced bounded generated cover candidates with exclusive reservation. v0.7 adds a second spatial scale rather than replacing those local affordances.

### Coarse region graph

The current spatial lab uses 24 named coarse regions. The graph supports:

- nearest-region lookup
- bounded connectivity/path-step queries
- deterministic next-hop routing
- region-level novelty / visit memory
- regional activity / incident / objective relevance
- squad distribution/congestion pressure

The eventual 8 km² maps may contain many more regions, but each agent should continue to reason over a bounded graph frontier/candidate set rather than scale its thought with total map area.

### Local samples

Once a region/next hop is selected, v0.7 evaluates five bounded nearby candidate samples. The locked field preference order is:

1. route congestion
2. ally support
3. enemy threat
4. exposure
5. pressure
6. objective value

Current implementation uses descending ordered weights as the first stable slice. If human footage shows a lower-priority field overriding an obviously more important congestion/support distinction, this can be upgraded to quantized lexicographic tiers without changing the surrounding architecture.

Fields **bias** choice. Hard vetoes belong only to physical/logical impossibility such as invalid/unreachable destinations, occupied exclusive affordances, relationship prohibition or unavailable actors.

Structural exposure is cached at region level and refined against known threat LOS. Threat derives from agent knowledge. Activity/incident/objective impulses are semantic/event-driven and decay.

## Situated agency / habitation

Agents are globally mobile but locally situated.

`roam -> enter region -> sample local world -> adopt task -> inhabit task -> completion/invalidity/boredom/interrupt -> release -> roam`

Current Human Rival local task vocabulary:

- ROAM
- WATCH
- INVESTIGATE
- OCCUPY
- PRESSURE
- FLANK
- REGROUP
- CONTEST

Task commitment/hysteresis prevents marginal score changes from causing thrash. Soft task claims discourage redundant WATCH/OCCUPY behavior; physically exclusive cover affordances retain hard reservation.

Regional selection uses bounded combinations of novelty, semantic activity, squad distribution, role relation, objective value, congestion, incident cost and travel. A small deterministic seeded choice among near-top regions keeps actors from converging on one mathematically identical optimum.

Seeded personality envelopes modestly vary support/risk/residency preferences while remaining subordinate to faction policy, relationships and physical truth.

Human Rivals use temporary tactical residency. Future teammates should remain more relationally anchored to the Player Crew. Future Dream Cattlers should use more persistent habitat/territorial **infestation** rather than a renamed Rival policy.

## Spatial memory

v0.7 establishes three compatible timescales:

- short individual tactical memory — existing last seen/heard/confidence
- medium individual regional memory — recent region visits/task context
- slower squad/regional incident memory — damage/death/activity history

Respawn clears the individual's immediate spatial/task state while shared regional incident history may persist. Incident memory decays; there is no cross-match persistent adaptation or invisible machine-learning counterstrategy.

## Interruption contract

Interruption severity remains intentionally small:

`NONE -> REEVALUATE -> TASK_BREAK -> EMERGENCY -> AUTHORITATIVE`

Typical sources:

- route invalidation / unavailable destination -> AUTHORITATIVE
- severe direct danger/damage -> EMERGENCY
- nearby gunfire/death/suspicious sound -> REEVALUATE
- story/objective phase change -> TASK_BREAK

An interrupt normally means **reconsider now**, not “execute this hidden script.” Exact forced behavior is reserved for physics/server-truth necessities or rare explicitly authored synchronization later.

## Story-authoring authority

Future story-mode integration uses three levels:

1. **CONTEXT** — preferred: change what region/objective/person matters
2. **CONSTRAINT** — occasional: require a broad reach/remain/defend condition
3. **DIRECT ACTION** — rare cinematic/synchronization escape hatch

The default story system should change **what matters** while allowing AI to decide how to act within that context.

## Fuzzy Rail

Fuzzy Rail remains local geometric movement intelligence, not a global route planner. It detects nearby steps, vaults, mantles and ladders, scores local transitions and uses hysteresis so geometry is permissive without becoming a hard track. Spatial ecology selects context/destination; Fuzzy Rail still owns immediate traversal execution.

The client candidate is predictive; the server independently resolves traversal. Diagnostics distinguish candidate disagreement from authoritative positional correction.

## v0.6 encounter-governance layer

v0.6 remains a separate bounded layer after AI/spatial intent and before authoritative execution:

`perception -> utility/affordance -> spatial situated control -> encounter governance -> movement/combat -> truth`

It owns:

- finite yaw/pitch tracking speed
- aim-settle state
- target-motion / own-motion / suppression effects on firing readiness
- soft squad pressure authorization (two slots by default)
- contextual role rebidding
- damage-driven suppression and retreat response

It does not weaken weapon damage, grant hidden knowledge, move actors directly or decide hit outcomes.

## Multi-rate / efficiency contract

Not every subsystem runs at 60 Hz.

- physical movement/control execution: simulation rate
- aim tracking: simulation-rate bounded controller
- perception: staggered sub-rate
- utility decisions: sub-rate
- role/pressure rebidding: slower cadence
- region selection: ~2 Hz baseline
- local spatial samples: ~5 Hz staggered
- regional activity/incident decay: bounded sub-rate
- semantic changes: event-driven
- path search: bounded by current region graph size/frontier

Expensive facts should be sensed once and reused from cached knowledge. New intelligence should prefer event-driven semantics, indexed local affordances and bounded candidate sets over whole-world polling.

**Large world != large per-agent thought.** The 0.40 km² v0.7 demo intentionally exercises the same hierarchy intended for the eventual 8 km² maps.

## v0.7 diagnostics

Server diagnostics report, per Rival:

- role / utility mode
- target/confidence/visibility
- health / cover
- pressure authorization / aim settle / suppression / aim error
- current region -> target region / next hop
- local spatial task / interrupt severity
- congestion / ally support / threat / exposure / pressure / objective values + aggregate preference

The graphical client exposes **F4** to render the shared 24-region topology and shows the player's local region. Full per-agent tactical scratch state remains server-side to avoid turning debug replication into a gameplay dependency.

## Security boundary

Session nonces/tokens and libsodium-backed token derivation/comparison remain intact. Encrypted gameplay payloads remain a later public-server hardening task; gameplay authority is not conflated with transport encryption.

## Current scope discipline

v0.7 intentionally does **not** add a full Ghost Static Director, autonomous teammate combat brain, live Dream Cattler ecology, giant continuous influence grid, general GOAP, complete objective/ecological score system, polished negotiation UI, final art/audio or the 8 km² production map.

The next Pop!_OS recording determines whether v0.8 should emphasize teammate/social intelligence, Dream Cattler habitat ecology, Ghost Static/director context, or a spatial/navigation refinement pass.
