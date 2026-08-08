# nightfall!punk architecture contract

## Durable stack

- C + raylib
- Linux-first
- fixed 60 Hz simulation
- authoritative dedicated server
- data-oriented ECS-lite direction
- presentation observes simulation; it does not own gameplay state
- ENet is a transport dependency hidden behind `nf_net`, not a gameplay API

## Simulation ownership

The player camera is presentation. The actor body is simulation. Movement input is local forward/strafe intent plus yaw and action buttons. The same `NfWorld` and movement functions execute on both client prediction and server authority.

Movement states: GROUND, SPRINT, CROUCH, AIR, LADDER, VAULT, MANTLE and PLATFORM.

## v0.3 network truth contract

The client sends **intent**, not outcome.

`input -> local prediction -> ENet input packet -> authoritative server simulation -> snapshot -> reconciliation -> presentation`

- server simulation: 60 Hz
- client input/prediction: 60 Hz
- snapshot publication: 30 Hz
- reliable channel: handshake/control
- unreliable channel: high-frequency input/snapshots
- input history: 256 commands
- redundant input bundle: latest 3 commands
- current player cap: 4

Snapshots include authoritative position, velocity, movement mode, grounded/crouched state, jump count, attached traversal feature and current Fuzzy Rail feature. The client rewinds to an acknowledged authoritative state and replays pending commands when error exceeds tolerance.

## Fuzzy Rail

Fuzzy Rail is local geometric movement intelligence, not a global AI route. It detects nearby steps, edges, vaults, mantles and ladders and scores possible local transitions from distance + approach alignment. Candidate retention uses hysteresis so usable geometry feels crisp without becoming a hard track.

Network rule: **the client's selected Fuzzy Rail feature is predictive only.** The server independently evaluates geometry and sends authoritative traversal state. A divergence counter makes disagreements visible during graybox development.

## Dynamic Affordance Graph

This remains a later semantic/tactical layer and is distinct from Fuzzy Rail.

Planned composition:

`world affordances -> influence/pressure fields -> faction utility -> squad blackboard -> desired action/destination -> Fuzzy Rail local traversal -> physics/server authority`

## Security boundary

v0.3 introduces session nonces/tokens and optional libsodium-backed token derivation/comparison. This is scaffolding for public servers; encrypted gameplay payloads are intentionally deferred until the transport/movement contract is proven.

## Scope discipline

v0.3 does not add combat, faction AI, round objectives, or aesthetic assets. Networking may replicate movement; movement remains independent from networking and rendering.
