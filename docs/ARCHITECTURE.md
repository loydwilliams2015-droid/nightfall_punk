# nightfall!punk architecture contract

## Durable stack

- C + raylib
- Linux-first
- fixed 60 Hz simulation
- authoritative dedicated server
- data-oriented ECS-lite direction
- presentation observes simulation; it does not own gameplay state
- ENet is a transport dependency hidden behind `nf_net`, not a gameplay API
- combat is a simulation dependency hidden behind `nf_combat`, not a renderer/network API

## Simulation ownership

The player camera and weapon viewmodel are presentation. Actor body, health, weapon state, ammunition and death are simulation/server truth. The same `NfWorld` and movement functions execute on client prediction and server authority; combat outcomes execute authoritatively on the server and recover through snapshots.

Movement states: GROUND, SPRINT, CROUCH, AIR, LADDER, VAULT, MANTLE and PLATFORM.

Weapon states: READY, RECOVERING, RELOADING, SWITCHING and EMPTY.

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
- current player cap: 4

Protocol v4 (`NF04`) extends the v0.3 contract with fire/reload/switch/aim intent, authoritative combat state in snapshots and explicit server-authored combat events.

## Combat authority

The client never submits a target, hit zone, damage value, death result, ammo value or successful shot result.

A shot is resolved as:

`fire intent -> server validates actor/weapon/state/ammo/rate -> reconstruct aim -> bounded history rewind -> world occlusion + actor hit test -> authoritative damage/death -> combat event + later snapshot recovery truth`

The current hitscan layer has two zones: BODY and HEAD. Carbine and pistol are intentionally simple graybox weapons used to prove the combat substrate before recoil/spread/ADS/projectile complexity.

## Lag compensation

The server stores a 64-frame actor-position history. Hitscan rewind is clamped to 12 ticks (~200 ms). Only target geometry is historically sampled; server truth remains current for weapon state, ammo, faction rules and damage application. The applied rewind duration is surfaced in combat-event diagnostics for human tuning.

## Combat events and World Semantic Alerts

v0.4 introduces the first **server-authored combat event producers**:

- GUNFIRE
- DAMAGE
- DEATH
- RESPAWN
- RELOAD
- WEAPON_SWITCH

These are causal gameplay events, not raw packet interpretations. They are the seed of the future World Semantic Alerts layer, but v0.4 intentionally does not yet implement the full alert-routing/retention/consumer system.

Later semantic expansion should include mechanism/reward/objective/route/sighting/threat events and selective consumers for:

- faction AI
- squad blackboards
- Dynamic Affordance Graph
- director / Dragon Master logic
- audio/FX presentation
- objective and story systems

## Fuzzy Rail

Fuzzy Rail remains local geometric movement intelligence, not a global AI route. It detects nearby steps, vaults, mantles and ladders, scores local transitions, and uses hysteresis so geometry is permissive without becoming a hard track. Combat does not own or rewrite Fuzzy Rail.

The client candidate is predictive; the server independently resolves traversal. v0.4 labels the remaining counter as traversal/candidate disagreement rather than implying every short-lived disagreement is an authoritative divergence.

## Dynamic Affordance Graph

This remains the later semantic/tactical layer:

`world affordances -> semantic alerts + influence/pressure fields -> faction utility -> squad blackboard -> desired action/destination -> Fuzzy Rail local traversal -> physics/server authority`

## Faction damage rule

Damage permission is centralized through `nf_combat_can_damage`. Friendly fire is off by default. The combat lab assigns alternating PLAYER/RIVAL factions to human client slots so two-client duels work without temporarily weakening the faction contract.

## Security boundary

Session nonces/tokens and libsodium-backed token derivation/comparison remain intact from v0.3. Encrypted gameplay payloads are still a later public-server hardening task; v0.4 does not conflate combat authority with transport encryption.

## Scope discipline

v0.4 does not add final weapon art, ADS, projectile weapons, faction AI, round objectives, physical reward puzzles or the full World Semantic Alert consumer graph. It proves the authoritative graybox FPS combat substrate first.
