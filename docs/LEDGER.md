# nightfall!punk master ledger

Versioning rule: **one flat version = one coherent development contract.** Git commits record fine-grained work. We do not create nested subversions.

## Locked project decisions

- [x] C + raylib
- [x] Linux-first
- [x] authoritative dedicated-server architecture from day one
- [x] fixed 60 Hz simulation
- [x] data-oriented ECS-lite direction
- [x] four-player target for now; possible five later
- [x] round-based matches
- [x] mixed authored + seeded physical/temple mechanisms
- [x] placeholder assets before original aesthetics
- [x] direct confrontation is the gameplay emphasis
- [x] rewards come from simple physical puzzles performed under combat pressure
- [x] moderate tactical walk + fast sprint
- [x] infinite sprint for now; contextual fatigue later
- [x] crouch-jump enables a second jump
- [x] waist/chest mantle; higher mantle needs momentum
- [x] mild fall damage later for major drops only
- [x] Fuzzy Rail = local geometric traversal intelligence
- [x] Dynamic Affordance Graph = later utility + affordances + influence + blackboard

# v0.1 — Foundation — COMPLETE

## Existing features retained and unified
- [x] repository and CMake build
- [x] shared simulation library
- [x] headless dedicated-server executable
- [x] raylib client executable
- [x] canonical world/entity state
- [x] smoke testing
- [x] placeholder-only presentation policy

# v0.2 — Movement — CURRENT BUILD

## Existing / regression protection
- [x] preserve 60 Hz shared simulation
- [x] preserve server and client executables
- [x] preserve headless test path
- [x] separate headless/full CMake build directories to eliminate cache regression
- [x] keep raylib outside simulation code
- [x] keep four factions represented in shared types

## Advancing features
- [x] compact graybox movement lab
- [x] stairs/steps and two slope/ramp test surfaces
- [x] low vault obstacle and mantle obstacle
- [x] elevated platform / narrow bridge
- [x] ladder test volume and crouch tunnel
- [x] moving platform
- [x] simulation-owned player body independent of camera
- [x] WASD intent + yaw input command shape
- [x] acceleration / deceleration / reverse braking
- [x] gravity and grounded state
- [x] walk / sprint / crouch / jump
- [x] crouch-gated second jump
- [x] air control and step assist
- [x] analytical ramp support
- [x] ladder / bounded vault / bounded mantle states
- [x] moving-platform carry

### Fuzzy Rail
- [x] local traversal candidate types
- [x] distance + alignment scoring
- [x] step / vault / mantle / ladder classification
- [x] acquire / retain hysteresis
- [x] bounded assistance rather than hard snapping
- [x] debug candidate visualization
- [ ] multi-candidate top-N set (single best candidate in v0.2)
- [ ] richer edge probes / arbitrary mesh edge extraction
- [ ] AI consumer of the same candidate API

### Diagnostics and tests
- [x] position / velocity / horizontal speed
- [x] grounded / crouched / movement mode / jump count
- [x] Fuzzy Rail candidate type / score / feature
- [x] gravity-ground regression test
- [x] walk-vs-sprint regression test
- [x] crouch-gated second-jump test
- [x] crouch-clearance test
- [x] Fuzzy Rail vault-candidate test

## Par / Compare-5 targets
- [x] Quake III: immediate acceleration/deceleration vocabulary represented
- [x] Red Eclipse: traversal creates alternate geometric routes
- [x] Urban Terror: base locomotion remains grounded and human-readable
- [x] Halo 3: platform scale and forgiving vertical route readability represented
- [x] Titanfall 2: vault/mantle/ladder are explicit transition states rather than camera tricks
- [ ] tuning pass after human playtest

## Stable-coherence gate for v0.2
- [x] simulation has no raylib dependency
- [x] movement state is explicit and inspectable
- [x] input shape can later be serialized for networking
- [x] Fuzzy Rail does not choose global routes
- [x] Dynamic Affordance Graph remains deferred from movement implementation
- [x] aesthetics remain non-blocking
- [ ] full graphical build verified on target Pop!_OS machine
- [ ] human movement playtest completed

## Deferred intentionally

### v0.3 — Networked Movement
Transport, protocol versioning, input commands over network, authoritative snapshots, interpolation, prediction, reconciliation, reconnect and latency/loss simulation.

### Later graybox contracts
Combat, faction AI/game theory, Dynamic Affordance Graph, round lifecycle, physical reward puzzles, cargo/relic mechanics, then graphics/aesthetic layer.
