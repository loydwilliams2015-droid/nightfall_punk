# nightfall!punk master ledger

Versioning rule: **one flat version = one coherent development contract.** Git commits record fine-grained work. Completed versions are preserved as immutable `archive/v0.X` branches before `main` advances. No nested subversions.

## Locked project decisions

- [x] C + raylib; Linux-first
- [x] fixed 60 Hz simulation
- [x] authoritative dedicated server from day one
- [x] data-oriented ECS-lite direction
- [x] four-player target for now; possible five later
- [x] round-based matches
- [x] mixed authored + seeded physical/temple mechanisms
- [x] placeholder assets before original aesthetics
- [x] direct confrontation is the gameplay emphasis
- [x] rewards come from simple physical puzzles under combat pressure
- [x] moderate tactical walk + fast sprint
- [x] infinite sprint for now; contextual fatigue later
- [x] crouch-jump enables a second jump
- [x] waist/chest mantle; higher mantle needs momentum
- [x] mild fall damage later for major drops only
- [x] Fuzzy Rail = local geometric traversal intelligence
- [x] Dynamic Affordance Graph = later utility + affordances + influence + blackboard
- [x] World Semantic Alerts = later authoritative gameplay-event layer; not part of v0.3 transport

# v0.1 — Foundation — COMPLETE

- [x] repository / CMake / raylib client
- [x] shared simulation and headless server
- [x] fixed-tick world and basic tests
- [x] archived as `archive/v0.1`

# v0.2 — Movement — COMPLETE / ARCHIVED

## Existing / regression protection
- [x] simulation-owned actor body independent of camera
- [x] walk / sprint / crouch / jump / crouch-gated second jump
- [x] gravity, ground, stairs, ramps and moving platforms
- [x] ladder / vault / mantle states
- [x] Fuzzy Rail candidate scoring + hysteresis
- [x] horizontal camera/strafe basis corrected after human playtest
- [x] preserved as `archive/v0.2`

# v0.3 — Networked Movement — COMPLETE

## Existing features retained and unified
- [x] v0.2 movement code remains the single movement implementation
- [x] client sends input intent, never authoritative transforms
- [x] server and client use the same `NfWorld` / `nf_movement` simulation
- [x] deterministic map collider indices serve as initial stable traversal feature IDs
- [x] Fuzzy Rail remains local geometry intelligence; networking does not turn it into a path rail
- [x] headless server remains independent of raylib
- [x] presentation remains replaceable

## Advancing features

### Transport and protocol
- [x] ENet transport isolated behind `nf_net`
- [x] ENet pinned to v1.3.18 when fetched
- [x] reliable control channel + unreliable state/input channel
- [x] protocol magic + version 3 header
- [x] HELLO / WELCOME / REJECT / INPUT / SNAPSHOT / PING / PONG message contracts
- [x] explicit little-endian hot-path serialization; no raw struct-on-wire dependency
- [x] packet size cap (1200 bytes)
- [x] four-player server cap for current design target

### Server truth
- [x] persistent dedicated-server loop
- [x] 60 Hz authoritative simulation
- [x] 30 Hz snapshot publication
- [x] per-peer input sequence validation
- [x] input range / finite-value validation
- [x] server performs collision, Fuzzy Rail and traversal independently
- [x] snapshot-driven remote actor spawn/presence

### Client prediction / reconciliation
- [x] 60 Hz predicted local movement
- [x] 256-slot command/state history
- [x] last three inputs redundantly transmitted
- [x] authoritative acknowledgement sequence
- [x] rewind to authoritative state + replay unacknowledged inputs
- [x] small-error tolerance before correction
- [x] prediction error / max error / correction counters
- [x] client-vs-server Fuzzy Rail feature disagreement diagnostic
- [x] moving-platform geometry resync hook for reconciliation

### Remote actors
- [x] snapshot-fed remote actor presentation
- [x] basic interpolation between received samples
- [ ] deliberate 2–3-snapshot interpolation buffer tuning deferred until combat-era multi-client playtesting

### Network impairment and automated testing
- [x] application-level simulated latency
- [x] simulated jitter
- [x] simulated unreliable-packet loss
- [x] `nightfall_netbot` automated network client
- [x] four-client `./nightfall.sh net-smoke` harness
- [x] protocol round-trip tests
- [x] prediction/reconciliation regression test
- [x] GitHub Actions full compile + contract tests + four-client smoke test

### Reconnect / security readiness
- [x] 20-second server-side entity reservation on disconnect
- [x] resume-token reconnect path
- [x] client automatic reconnect attempts
- [x] libsodium integration when `libsodium-dev` is available
- [x] nonce + session-token derivation abstraction
- [x] constant-time token comparison when libsodium is present
- [ ] authenticated encryption of gameplay payloads deferred to public-server hardening

### Diagnostics
- [x] RTT / application ping / snapshot age
- [x] server tick / client tick
- [x] command / acknowledgement / pending command count
- [x] prediction error / corrections
- [x] Fuzzy Rail candidate disagreement diagnostic
- [x] movement mode / speed / local Fuzzy Rail feature
- [x] crypto mode + impairment preset display

## Par / Compare-5 targets

These are behavioral targets, not claims about proprietary internal implementations.

- [x] Quake III lineage: command-oriented prediction + authoritative snapshots architecture
- [x] Source / Counter-Strike lineage: explicit local prediction vs remote interpolation separation
- [x] Unreal Tournament lineage: authoritative movement-state correction model
- [x] Titanfall 2 target: vault/mantle/ladder transitions represented in authoritative state
- [x] Overwatch target: diagnostics + graceful correction under imperfect network conditions
- [x] human normal-network and impaired-network recording reviewed; architecture accepted for next contract

## Stable-coherence gate

- [x] movement has no ENet dependency
- [x] transport has no raylib dependency
- [x] wire protocol is explicit and versioned
- [x] client does not submit position/traversal success as truth
- [x] server performs Fuzzy Rail candidate evaluation itself
- [x] local and headless build directories remain separate
- [x] contract/syntax validation passes in development environment
- [x] GitHub-hosted Ubuntu full ENet + raylib compile passes
- [x] GitHub-hosted four-client `net-smoke` passes
- [x] Pop!_OS full target build, simulation tests and network contract tests pass
- [x] Pop!_OS four-netbot smoke test passes
- [x] libsodium enabled on target machine
- [x] recorded v0.3 normal/impaired-network demo reviewed
- [x] POSIX `nanosleep` declaration warning fixed in closeout

## Accepted v0.3 deferrals

- [ ] world semantic alerts are **not implemented**; introduce with combat/AI/objective semantics, not as a transport-layer retrofit
- [ ] remote interpolation buffer tuning awaits richer multi-client combat movement
- [ ] live reconnect UX/human multi-client soak remains a hardening task even though reconnect/session machinery and automated multi-client networking are present
- [ ] authenticated encryption of gameplay payloads remains a later public-server hardening contract
- [ ] current Fuzzy Rail counter may include harmless short-lived candidate disagreement; refine when traversal events become semantically meaningful

# Planned next contracts

## v0.4 — Combat
Authoritative hits/damage, weapon state, ammo/reload, simple pickup inventory, combat diagnostics and latency-safe fire semantics. Combat should begin the first useful producers of future World Semantic Alerts (gunfire, damage, death, reload/weapon events), but the semantic event layer itself should remain explicit and server-authored rather than inferred from raw network packets.

## Later graybox contracts
Faction AI/game theory, Dynamic Affordance Graph, World Semantic Alerts expansion, rounds/objectives, physical reward puzzles, then the graphics/aesthetic engine phase.
