# nightfall!punk master ledger

Versioning rule: **one flat version = one coherent development contract.** Git commits record fine-grained work. Completed versions are preserved as immutable `archive/v0.X` branches before `main` advances. No nested subversions.

## Locked project decisions

- [x] C + raylib; Linux-first
- [x] fixed 60 Hz simulation; authoritative dedicated server
- [x] four-player target for now; possible five later
- [x] round-based story-oriented arena FPS
- [x] mixed authored + seeded physical/temple mechanisms
- [x] placeholder assets before original aesthetics
- [x] direct confrontation is the gameplay emphasis
- [x] rewards come from simple physical puzzles under combat pressure
- [x] moderate tactical walk + fast sprint; contextual fatigue later
- [x] crouch-jump enables a second jump
- [x] Fuzzy Rail = local geometric traversal intelligence
- [x] Dynamic Affordance Graph = later utility + affordances + influence + blackboard
- [x] World Semantic Alerts = explicit server-authored gameplay-event layer, never inferred from raw packets

# v0.1 — Foundation — COMPLETE / ARCHIVED
- [x] `archive/v0.1`

# v0.2 — Movement — COMPLETE / ARCHIVED
- [x] movement/traversal/Fuzzy Rail contract
- [x] `archive/v0.2`

# v0.3 — Networked Movement — COMPLETE / ARCHIVED
- [x] ENet abstraction + protocol v3
- [x] 60 Hz server/input/prediction + 30 Hz snapshots
- [x] reconciliation, remote interpolation scaffold, reconnect/session tokens
- [x] latency/jitter/loss simulation + four-netbot smoke
- [x] libsodium token derivation on target machine
- [x] normal/impaired-network recording reviewed
- [x] `archive/v0.3`

# v0.4 — Combat — CURRENT BUILD

## Contract thesis

Two guns. Hitscan. Server truth. Medium TTK. Health/death/respawn. Reload/switch. Lag-compensated target history. Four networked combatants. Combat events become the first concrete producers for the later World Semantic Alerts system.

## Existing / regression protection

- [x] v0.2 movement remains the single movement implementation
- [x] v0.3 ENet transport remains behind `nf_net`
- [x] client still sends intent, never authoritative transforms or hit claims
- [x] server still owns movement, collision, Fuzzy Rail and snapshots
- [x] 60 Hz server/input/prediction and 30 Hz snapshots retained
- [x] reconnect/session-token and network impairment scaffolding retained
- [x] raylib remains presentation-only

## Advancing — combat simulation

- [x] `nf_combat` shared simulation module
- [x] explicit weapon state: READY / RECOVERING / RELOADING / SWITCHING / EMPTY
- [x] two weapon slots: carbine + pistol
- [x] carbine: automatic, 30-round magazine, medium damage
- [x] pistol: semi-automatic, 12-round magazine, higher per-shot damage
- [x] 100-health baseline
- [x] body + head hit zones; head multiplier
- [x] server-side damage/death
- [x] 3-second graybox respawn
- [x] faction damage rule with friendly fire off by default
- [x] reload timing with late commit point
- [x] sprint/interact/weapon switch can interrupt reload
- [x] weapon switching is timed and authoritative
- [x] server rate-of-fire and ammo enforcement
- [x] dead actors cannot move/fire

## Advancing — network combat

- [x] protocol v4 / `NF04`
- [x] combat input travels with sequenced input commands
- [x] client sends fire/reload/switch/aim intent, never hit outcome
- [x] combat state included in snapshots: faction, health, alive, weapon, state, ammo
- [x] explicit `COMBAT_EVENT` network message
- [x] server-authored GUNFIRE / DAMAGE / DEATH / RESPAWN / RELOAD / WEAPON_SWITCH events
- [x] transient gunfire/damage events use state channel; important discrete transitions use reliable control channel
- [x] authoritative combat state applies even when movement reconciliation is unnecessary
- [x] first World Semantic Alert producers exist as combat events; full semantic routing is intentionally later

## Advancing — hitscan and latency safety

- [x] server reconstructs shot from authoritative actor + submitted yaw/pitch intent
- [x] 64-frame server actor-history ring
- [x] rewind limited to 12 ticks (~200 ms)
- [x] historical actor target test for hitscan
- [x] world-solid occlusion check
- [x] rewind duration carried on damage/death combat event for diagnostics
- [x] client never submits target/damage
- [ ] tune rewind policy from human impaired-network combat footage

## Advancing — graybox combat lab

- [x] two stationary opposite-faction target dummies in server world so either current faction has a valid target
- [x] alternating player/rival factions across four network client slots for duel testing
- [x] remote actors colored ally/opponent in client presentation
- [x] primitive carbine/pistol viewmodel placeholder
- [x] immediate local muzzle-flash presentation
- [x] server-confirmed hit marker
- [x] incoming-damage screen border
- [x] health / weapon / ammo / weapon-state HUD
- [x] combat event / hit-zone / damage / rewind diagnostics
- [x] network diagnostics retained
- [x] focus-aware mouse capture releases on focus loss and reacquires after focus return
- [x] mouse-look fallback can use absolute-position delta when a backend reports zero relative delta
- [x] HUD reports mouse focus, capture state, delta and fallback activation

## Controls

- [x] WASD move
- [x] mouse look
- [x] Shift sprint
- [x] Ctrl crouch
- [x] Space jump / crouch-gated second jump
- [x] E interact / ladder
- [x] Left Mouse fire
- [x] R reload
- [x] 1 carbine
- [x] 2 pistol
- [x] F10 toggle / reacquire mouse capture

## Automated contracts

- [x] weapon state / rate-of-fire test
- [x] ammo consumption test
- [x] reload commit/cancel test
- [x] weapon-switch test
- [x] semi-auto pistol test
- [x] faction-damage rule test
- [x] damage/death/respawn test
- [x] protocol round-trip for combat input/state/events
- [x] reconciliation test retains authoritative health
- [x] four automated clients generate movement + aiming + firing + reload/switch inputs
- [x] combat-smoke requires authoritative damage events as well as snapshots/combat events
- [x] GitHub CI full raylib + ENet + libsodium compile green on initial v0.4 branch
- [x] GitHub simulation / combat / network contract tests green: 3/3
- [x] GitHub four-client combat smoke green; authoritative damage and death observed by every bot
- [x] project-owned C targets compile under `-Wall -Wextra -Wpedantic -Werror`
- [ ] GitHub CI green on mouse-capture repair tree
- [ ] Pop!_OS clean build/tests/combat-smoke green
- [ ] Pop!_OS mouse-look recapture retest green
- [ ] recorded human combat demo reviewed

## Par / Compare-5

Behavioral targets, not claims about proprietary internals.

- [x] Quake III lineage — shooting remains compatible with expressive movement
- [x] Urban Terror — grounded readable gunfight scale
- [x] Source / Counter-Strike lineage — server-authoritative hitscan + bounded lag-compensation philosophy
- [x] Halo 3 — medium-TTK counterplay rather than one-tap lethality
- [x] Titanfall 2 — traversal states remain valid while weapons are active
- [ ] human tuning of TTK, recoil/spread feel and movement/fire transition from recorded demo

## Deliberate v0.4 boundaries

- [ ] no ADS yet
- [ ] no projectile weapons yet
- [ ] no final weapon art/assets required
- [ ] no full inventory/pickup economy yet; two authoritative weapon slots and reserve ammo are the inventory substrate
- [ ] no AI combat behavior yet
- [ ] no full World Semantic Alert routing/consumers yet
- [ ] no rounds/objectives/reward mechanisms yet
- [ ] recoil/spread sophistication follows the first human gunfeel review rather than preceding it

## Stable-coherence gate

- [x] combat simulation has no raylib dependency
- [x] combat simulation has no ENet dependency
- [x] transport does not decide damage/hits
- [x] client does not claim hits/damage/death
- [x] snapshots are recovery truth; combat events are causal/feedback messages
- [x] movement and combat states remain separate but live on the same actor/world truth
- [x] initial GitHub-hosted full compile green
- [x] all automated tests green
- [x] four-client combat smoke green with damage/death
- [ ] mouse-capture repair exact tree green in CI
- [ ] target Pop!_OS clean build/tests/combat-smoke green
- [ ] target Pop!_OS mouse look confirmed
- [ ] target Pop!_OS run + recorded combat demo reviewed

## Current tuning watch

- [ ] verify HUD reports `mouse FOCUSED | capture ON` and non-zero delta while physically moving the mouse on Pop!_OS
- [ ] if relative delta stays zero but `abs-fallback` rises, keep fallback and investigate raylib/GLFW backend behavior separately
- [ ] inspect larger movement-prediction errors around automated death/respawn transitions (bot max observed up to ~0.82 m) in human footage before deciding whether this is visible or merely a discontinuity diagnostic
- [ ] tune TTK / recoil / spread / weapon transitions from the recorded human combat pass
- [ ] tune hitscan rewind policy under impaired-network human combat

# Planned next graybox contracts

After v0.4 is accepted and archived, prioritize faction AI/game theory + Dynamic Affordance Graph, then broaden World Semantic Alerts, rounds/objectives and physical reward puzzles before the graphics/aesthetic engine phase.
