# nightfall!punk v0.1 Ledger

## Locked decisions

- [x] C + raylib
- [x] Linux-first
- [x] authoritative dedicated-server architecture from day one
- [x] fixed-timestep simulation
- [x] data-oriented ECS-lite direction
- [x] 4-player target for now; possible 5 later
- [x] full traversal target: walk, sprint, crouch, jump, crouch-jump/double-jump, mantle, vault, ladders, moving platforms
- [x] mixed authored + seeded temple mechanisms
- [x] round-based matches
- [x] placeholder assets before original aesthetics

## Contract A — Foundation
- [x] repository initialized
- [x] CMake build system
- [x] shared world module
- [x] dedicated-server entry point
- [x] raylib client entry point
- [x] world smoke test
- [x] build helper script
- [ ] CI
- [ ] configuration loader
- [ ] structured logging/assertions

## Contract B — Core Engine
- [x] entity ID type
- [x] canonical world state
- [x] actor storage
- [x] fixed simulation tick constant
- [ ] component pools / ECS-lite refinement
- [ ] event queue
- [ ] serialization
- [ ] seeded RNG service
- [ ] resource manager

## Contract C — Simulation / Traversal
- [ ] gravity and grounded state
- [ ] collision world
- [ ] walk/sprint
- [ ] crouch
- [ ] jump
- [ ] crouch-jump/double-jump
- [ ] stairs/slopes
- [ ] mantle/vault
- [ ] ladders
- [ ] moving platforms
- [ ] triggers

## Contract D — Networking
- [ ] transport abstraction
- [ ] dedicated server loop
- [ ] connection handshake and protocol versioning
- [ ] input commands
- [ ] authoritative snapshots
- [ ] interpolation
- [ ] prediction
- [ ] reconciliation
- [ ] spawn/despawn replication
- [ ] relevance management
- [ ] reconnect handling
- [ ] latency/loss simulation
- [ ] automated network clients

## Contract E — Gameplay
- [ ] player
- [ ] teammate
- [ ] rival
- [ ] Nightmare Rancher
- [ ] weapons/damage/health
- [ ] pickups/inventory
- [ ] round lifecycle
- [ ] respawn
- [ ] cargo objective
- [ ] relic objective
- [ ] temple mechanisms

## Contract F — AI / Game Theory
- [ ] reflex layer
- [ ] tactical utility layer
- [ ] strategic layer
- [ ] squad event layer
- [ ] cooperative teammate utility
- [ ] competitive rival utility
- [ ] opportunistic/ecological Rancher utility

## Contract G — Presentation
- [x] minimal raylib client shell
- [ ] 3D graybox map
- [ ] debug actor capsules
- [ ] collision visualization
- [ ] traversal diagnostics
- [ ] network diagnostics
- [ ] AI state visualization
- [ ] placeholder asset integration

## v0.1 readiness rule

Build infrastructure deeply, content narrowly. No aesthetic subsystem may become a dependency of simulation, gameplay, AI, or networking.
