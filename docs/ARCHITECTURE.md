# nightfall!punk architecture contract

## Durable stack

- C + raylib
- Linux-first
- fixed 60 Hz simulation
- authoritative dedicated server
- data-oriented / ECS-lite direction
- presentation observes simulation; it does not own gameplay state
- ENet is a transport dependency hidden behind `nf_net`, not a gameplay API
- combat is a simulation dependency hidden behind `nf_combat`, not a renderer/network API
- AI and encounter-governance code have no raylib or ENet dependency
- fixed-capacity hot-path state is preferred over per-tick allocation

## Simulation ownership

The player camera and weapon viewmodel are presentation. Actor body, faction, relationship permissions, health, weapon state, ammunition, movement and death are simulation/server truth. The same `NfWorld` and movement functions execute on client prediction and server authority; combat outcomes execute authoritatively on the server and recover through snapshots.

Movement states: GROUND, SPRINT, CROUCH, AIR, LADDER, VAULT, MANTLE and PLATFORM.

Weapon states: READY, RECOVERING, RELOADING, SWITCHING and EMPTY.

## Shared control contract

Network players and server AI converge at the same control vocabulary before movement/combat:

`network packet -> validated NfControlFrame -> NfMoveInput + NfCombatInput`

`AI perception/decision -> NfControlFrame -> NfMoveInput + NfCombatInput`

AI is therefore **not** a fake network client, and it does not own a privileged damage or traversal path. Human and AI control sources submit intent; authoritative movement/combat systems decide outcomes.

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

Protocol v4 (`NF04`) carries fire/reload/switch/aim intent, authoritative combat state in snapshots and explicit server-authored combat events.

## Current faction topology

The v0.4 temporary alternating duel scaffold is retired.

- network slot 0 = PLAYER
- additional network slots = TEAMMATE
- server Human Rival agents = RIVAL
- Dream Cattler implementation currently retains the technical `RANCHER` faction identifier pending a coordinated rename

Faction identity is distinct from current relationship state. Player/Teammate is cooperative. Human Rival relationships may conceptually move through hostility, contest, truce and cooperation. Dream Cattlers remain non-negotiable regardless of whether a particular disposition is predatory or the rare hostile/non-predatory avoidant type.

Damage permission is resolved through the central relationship/faction policy before authoritative hit application. Friendly fire remains off by default.

## Combat authority

The client and AI controller never submit a target, hit zone, damage value, death result, ammo value or successful-shot result.

A shot is resolved as:

`fire intent -> server validates actor/weapon/state/ammo/rate -> reconstruct aim -> bounded history rewind -> world occlusion + actor hit test -> relationship damage permission -> authoritative damage/death -> combat event + later snapshot recovery truth`

The current hitscan layer has two zones: BODY and HEAD. Carbine and pistol remain intentionally simple graybox weapons used to prove the combat/AI substrate before recoil/spread/ADS/projectile complexity.

## Lag compensation

The server stores a 64-frame actor-position history. Hitscan rewind is clamped to 12 ticks (~200 ms). Only target geometry is historically sampled; server truth remains current for weapon state, ammo, relationship rules and damage application. The applied rewind duration is surfaced in combat-event diagnostics for human tuning.

## World Semantic Alerts

v0.4 produced server-authored combat events. v0.5 promoted those facts into the first live World Semantic Alert bus.

Current semantic producers/consumers include:

- GUNFIRE -> bounded hearing radius/lifetime
- DAMAGE -> direct attacker/threat evidence
- DEATH
- RESPAWN
- AI hearing and memory consume semantic alerts rather than transport packets

The semantic bus is fixed-capacity and gameplay-authored. Critical AI truth is never inferred from packet arrival/loss.

Later vocabulary should include mechanisms, ecological resources, objectives, route changes, cargo, sightings and Dream Cattler activity.

## Epistemic AI contract

The server owns truth, but AI does not automatically know truth.

`server truth != individual knowledge != squad report`

Human Rival perception is bounded by range/facing/world occlusion. Agents remember last-seen and last-heard positions, attach confidence, decay stale knowledge, and may receive lower-confidence secondhand squad reports. This separation is required for searching, surprise, deception and non-psychic pursuit.

## Utility / decision contract

v0.5 ordinary tactical decisions use bounded utility modes rather than a giant behavior tree or continuous general planner:

- IDLE
- INVESTIGATE
- ADVANCE
- ENGAGE
- SEEK_COVER
- RETREAT
- RELOAD
- TRUCE_HOLD

Utility asks **what should I attempt?** Affordances ask **where can it happen?** Navigation/local steering asks **how do I approach it?** Fuzzy Rail answers **how do I physically negotiate the immediate geometry?** General GOAP remains reserved for later compound multi-step goals.

## Dynamic Affordance Graph — current slice

v0.5 generates bounded local cover candidates from world geometry. Cover selection considers travel cost and world occlusion; exclusive reservation prevents all Rivals selecting the same slot. Tactical goals feed normal shared movement rather than moving actors directly.

The long-term graph remains:

`world affordances -> semantic alerts + influence/pressure fields -> faction utility -> squad blackboard -> desired action/destination -> navigation -> Fuzzy Rail local traversal -> physics/server authority`

Global tactical influence fields and coarse region/A* routing remain deliberate post-v0.6 expansions if human footage demonstrates the need.

## Fuzzy Rail

Fuzzy Rail remains local geometric movement intelligence, not a global AI route. It detects nearby steps, vaults, mantles and ladders, scores local transitions and uses hysteresis so geometry is permissive without becoming a hard track. Combat and encounter governance do not own or rewrite Fuzzy Rail.

The client candidate is predictive; the server independently resolves traversal. Diagnostics distinguish candidate disagreement from authoritative positional correction.

## v0.6 encounter-governance layer

v0.6 adds a separate bounded layer **after AI intent selection but before authoritative movement/combat execution**:

`perception -> knowledge -> utility/affordance -> raw AI control -> encounter governance -> shared movement/combat -> world truth`

Encounter governance currently owns:

- finite yaw/pitch tracking speed
- aim-settle state
- target-motion / own-motion / suppression effects on firing readiness
- soft squad pressure authorization (two slots by default)
- contextual bounded-cadence role rebidding
- damage-driven suppression and retreat response

The encounter layer does **not** weaken weapon damage, grant AI hidden knowledge, move actors directly or decide hit outcomes. Its purpose is to preserve capable agents while bounding aggregate simultaneous certainty/lethality.

Pressure authorization is intentionally softer than a classic attack-token system. At most two Rivals receive normal pressure authorization by default; non-pressure actors may still take rare opportunistic shots or defend themselves at close range.

## Multi-rate / efficiency contract

Not every AI subsystem runs at 60 Hz.

- physical movement/control execution: simulation rate
- aim tracking: simulation-rate bounded controller
- perception: staggered sub-rate
- utility decisions: sub-rate
- role/pressure rebidding: slower bounded cadence
- semantic hearing: event-driven
- path replanning/planning: future event-driven bounded work

Expensive facts should be sensed once and reused from cached knowledge. New intelligence should prefer event-driven semantics, local indexed affordances and bounded candidate sets over whole-world polling.

## Security boundary

Session nonces/tokens and libsodium-backed token derivation/comparison remain intact. Encrypted gameplay payloads are still a later public-server hardening task; gameplay authority is not conflated with transport encryption.

## Current scope discipline

v0.6 intentionally does **not** add a global AI Director/Ghost Static pacing system, full tactical influence maps, coarse region/A* routing, an autonomous teammate combat brain, live Dream Cattler ecological behavior, a general GOAP planner, polished negotiation UI, round/objective/ecological scoring systems, or final art/audio. Human v0.6 footage determines which of those systemic layers provides the highest next value.
