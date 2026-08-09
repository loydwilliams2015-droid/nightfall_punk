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
- AI, encounter, spatial and habitat-ecology code have no raylib or ENet dependency
- fixed-capacity hot-path state is preferred over per-tick allocation

## Simulation ownership

Actor body, faction, relationship permissions, health, weapon state, ammunition, movement and death are simulation/server truth. The player camera and viewmodel are presentation. The same `NfWorld` movement path supports client prediction and server authority; combat outcomes are server-authored and recover through snapshots/events.

Movement states remain GROUND, SPRINT, CROUCH, AIR, LADDER, VAULT, MANTLE and PLATFORM.

Weapon states remain READY, RECOVERING, RELOADING, SWITCHING and EMPTY.

## Shared control contract

Network players and both server AI families converge on one intent vocabulary:

`network packet -> validated NfControlFrame -> NfMoveInput + NfCombatInput`

`Human Rival perception/utility/spatial reasoning -> NfControlFrame`

`Dream Cattler habitat/pack-predator reasoning -> NfControlFrame`

No AI is a fake network client. No AI owns privileged damage or traversal. Authoritative movement/combat systems decide outcomes.

## Parallel AI-family pipeline — v0.8

Human Rivals and Dream Cattlers share truth primitives but not motivational policy.

```text
AUTHORITATIVE WORLD + WORLD SEMANTIC ALERTS
        |
        +--> HUMAN RIVAL
        |    perception -> imperfect memory -> utility/local affordances
        |    -> v0.7 region/task reasoning -> v0.6 encounter governance
        |    -> NfControlFrame
        |
        +--> DREAM CATTLER
             perception -> imperfect individual knowledge
             -> imperfect pack reports / habitat state / infestation
             -> lurk/stalk/snipe/swoop/infest/expand policy
             -> NfControlFrame

        -> SHARED MOVEMENT / FUZZY RAIL
        -> AUTHORITATIVE COMBAT
        -> WORLD TRUTH + SEMANTIC / ECOLOGICAL CONSEQUENCES
```

Subsystems do not solve each other's questions:

- **Perception:** what do I know?
- **Utility / motivation:** what do I want?
- **Affordances:** what local opportunities exist?
- **Region/habitat reasoning:** which part of the arena matters?
- **Navigation/local steering:** how do I approach it?
- **Fuzzy Rail:** how do I physically negotiate immediate geometry?
- **Action execution:** what control input now?
- **Blackboard:** what has the group reported/claimed?
- **Encounter governance:** how much Human-Rival lethal certainty reaches execution?
- **Director/Event layer:** what world circumstances become important now?

General GOAP remains reserved for later multi-step goals.

## Network truth contract

The client sends intent, not outcome:

`input -> local movement prediction -> ENet input packet -> authoritative server movement/combat -> snapshot + combat event -> reconciliation/presentation`

- server simulation: 60 Hz
- client input/prediction: 60 Hz
- snapshot publication: 30 Hz
- reliable channel: handshake/control + important discrete transitions
- state channel: high-frequency input, snapshots and transient feedback
- input history: 256 commands
- redundant input bundle: latest 3 commands
- network-player cap: 4
- snapshot actor cap: 16, sufficient for the current four clients + four Human Rivals + up to five Cattlers

Protocol v4 (`NF04`) carries ordinary actor/combat state. v0.8 does **not** replicate whole AI field maps, pack scratch state, infestation grids or exact ecological ledger values to ordinary clients.

## Current faction topology

- network slot 0 = PLAYER
- additional network slots = TEAMMATE
- server Human Rival agents = RIVAL
- Dream Cattler implementation temporarily retains technical `RANCHER`

Faction identity is distinct from relationship state. Player/Teammate is cooperative. Human Rival relationships may be hostile/contested/truce/cooperative. Dream Cattlers are non-negotiable regardless of whether a seeded individual is predatory or rare hostile/non-predatory.

Damage permission is centrally relationship-gated before authoritative hit application. Friendly fire remains off by default.

## Combat authority / morphology

The client or AI never submits target, hit zone, damage, death, ammo or successful-shot outcome.

`fire intent -> validate actor/weapon/state/ammo/rate -> reconstruct aim -> bounded rewind -> world occlusion + actor hit test -> relationship permission -> authoritative damage/death -> event/snapshot`

Human graybox hit zones remain BODY + HEAD.

Dream Cattler graybox hit geometry adds:

- HEAD
- BODY
- KNEE
- FOOT

Knee/foot hits reduce ordinary health through the same combat system and separately notify the Cattler ecology layer so locomotor integrity can decline. Locomotor integrity is **not** a hidden damage system; it changes mobility/behavioral confidence such as swoop/relocation willingness.

The current Cattler visual/combat silhouette is taller than a human. Shared movement still uses the common human-sized collision configuration in this v0.8 candidate; faction-specific tall clearance is explicitly a post-compile/human-proof watch so movement truth is not destabilized prematurely.

## Lag compensation

The server stores a 64-frame actor-position history. Hitscan rewind is clamped to 12 ticks (~200 ms). Historical actor position is sampled while weapon state, ammo, relationships and damage remain current server truth.

## World Semantic Alerts

Current/declared vocabulary includes GUNFIRE, DAMAGE_TAKEN, ACTOR_DIED, ACTOR_RESPAWNED, ENEMY_SEEN/LOST scaffolding, SUSPICIOUS_SOUND, AFFORDANCE reservations, OBJECTIVE_CHANGED, ROUTE_CHANGED and STORY_PHASE_CHANGED.

Critical AI truth is never inferred from packet loss/arrival.

Human Rival spatial reasoning consumes semantic impulses for activity/incidents/interruption. Dream Cattler ecology consumes ordinary combat semantics/evidence as prey activity and disturbance without granting exact hidden prey transforms.

## Epistemic contract

**Server owns reality; AI does not automatically know reality.**

Human Rival:

`server truth != individual Rival knowledge != squad report`

Dream Cattler:

`server truth != individual Cattler knowledge != pack report`

Cattler firsthand evidence may come from bounded LOS and audible semantic events. Pack reports contain coarse position/region, evidence type, confidence and age. Reports decay and may be stale. Loners ignore ordinary pack coordination.

Ghost Static/Director may later use authoritative truth to schedule **Events**, but an Event changes circumstances/context; it does not inject exact hidden target coordinates into individual cognition.

## v0.7 spatial architecture retained

The current 0.40 km² / 500 m × 800 m lab contains 24 coarse regions and bounded local samples. This is 5% of the eventual 8 km² map-area target.

Human Rival field order remains:

1. route congestion
2. ally support
3. enemy threat
4. exposure
5. pressure
6. objective value

Fields bias rather than veto except for true physical/logical impossibility. Region selection is slower than physical simulation and uses bounded candidate sets. Fuzzy Rail remains local traversal, never global strategic route selection.

**Large world != large per-agent thought.**

## Dream Cattler habitat ecology — v0.8

Dream Cattlers use the same 24-region graph but interpret it through a different value system. Habitat score order is:

1. habitat continuity
2. resource/ecological value
3. prey evidence
4. disturbance
5. territorial pressure
6. access cost

A Cattler's power is partly spatial. Occupying useful territory gradually raises infestation. Unoccupied infestation decays slowly. Mature infestation increases residency/inertia; it does not grant magical health/damage multipliers.

Propagation is grounded in behavior:

`occupy -> lurk/exploit -> foothold -> weak infestation -> continued residence -> mature infestation -> adjacent physical relocation/occupation -> expansion`

No adjacent region becomes infested merely because a number crosses a threshold.

### Predator grammar

Current graybox modes:

- LURK
- STALK
- SNIPE
- SWOOP
- INFEST
- EXPAND
- RECOVER
- WITHDRAW
- AVOID

The v0.8 SWOOP is a committed sprint/jump approach through shared movement. A dedicated aerial primitive is deferred until human video proves it is necessary.

### Pack blackboard

May share:

- approximate prey region / position hint
- evidence type
- confidence / age
- territory/hunting occupancy context
- relevant outcomes

May not share:

- exact unseen prey transform
- exact unseen velocity
- another actor's complete perception
- global LOS
- perfect prey routes

This supports emergent ambush geometry: one visible Cattler may occupy attention while another uses a different report/region to stalk or lurk. There is no scripted military PRESSURE/FLANK role assignment for Cattlers.

## 90 / 5 / 5 social ecology

The intended distribution is not a per-tick random roll:

- ~90% normal pack-predator ecology
- ~5% rare Director-conditioned HUNT_SURGE/dogpile episodes
- ~5% seeded persistent loners

Normal profile uses deterministic seeded loner disposition and rare ecology-conditioned hunt-surge eligibility. Debug profiles force PACK, LONER or HORDE behavior for testing.

HUNT_SURGE changes convergence incentives for a short period and has a repose/cooldown. It does not bypass perception, navigation, combat or server truth.

## Ecological ledger vs player sense-data

Server/debug can maintain exact internal channels:

- infestation
- prey
- displacement
- expansion
- survival

There are individual and aggregate ecology ledgers.

The ordinary player does **not** receive `+5 infestation` style score feedback. Player-facing ecology must emerge as sense-data: recurrent occupation, lurking, route pressure, territorial persistence, prey behavior and later environmental/audio traces or Ghost Static interpretation.

Principle:

> **The scoreboard measures the ecology for the simulation; the world expresses the scoreboard for the player.**

## Director boundary

Ghost Static is not a v0.8 full AI Director. The current HUNT_SURGE state is an event-level scaffold proving that higher-order context can alter ecological convergence without directly steering actors.

Future Director authority:

- may know authoritative state
- may schedule/change Events or broad context
- may alter significance/eligibility
- may not directly drive movement/combat controls as normal behavior
- may not make individual actors omniscient

> **Ghost Static conducts conditions; the ecology performs them.**

## Multi-rate / efficiency contract

Not every subsystem runs at 60 Hz:

- physical execution: simulation rate
- Human Rival aim tracking: simulation-rate bounded controller
- perception: staggered sub-rate
- utility decisions: sub-rate
- Rival region selection: slow bounded cadence
- local spatial sampling: staggered bounded cadence
- Cattler perception: staggered sub-rate
- Cattler habitat/decision: slower sub-rate
- infestation/prey/disturbance values: cheap bounded updates + event impulses
- Director event eligibility: very slow cadence

Fixed arrays and bounded graph queries are preferred to whole-world high-resolution fields.

## Diagnostics

Human Rival server trace reports role/mode, knowledge/confidence, encounter pressure, aim settle/suppression, current/target/next region, situated task and spatial fields.

Dream Cattler server trace reports social mode, predator mode, prey evidence/confidence, locomotor integrity, current/target/next region, infestation/prey-activity/disturbance, current event mode and exact internal ecology ledger values.

The graphical client shows red Human Rivals, tall orange Dream Cattlers and temporary yellow knee/foot graybox bands. It deliberately does not expose the internal Cattler ecology score.

## Current scope discipline

v0.8 intentionally does **not** add final Cattler art/audio, full aerial navigation, a full Ghost Static Director, an autonomous teammate brain, full resource/reclamation objectives, general GOAP, giant influence grids, cross-match ML adaptation or the 8 km² production map.
