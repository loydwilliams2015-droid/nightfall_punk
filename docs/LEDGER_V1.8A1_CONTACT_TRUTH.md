# nightfall!punk 1.8A.1 — Contact Truth Laboratory

## Purpose
Add an opt-in, engine-shared collision-query/constraint/history kernel as a replacement *challenger* without changing `nf_movement_step_actor`. Preserve 1.7E observer and every prior historical regression. Exit as H1 contact correctness evidence, not as a production FPS or multiplayer physics certification.

## Distinctive rules
1. Physically meaningful contact is defined by a geometric collision/constraint relation, not by actor intention or semantic adjacency.
2. Contact reports use obstacle-to-moving-body normals, stable body identity, fractional [0,1] TOI, magnitude of normal velocity removed, a bounded contact manifold, and tick/provenance.
3. A material contact may produce an authoritative event only when its threshold/state change/contract warrants it. Contact and event are not interchangeable.
4. An actor's belief can incorporate contact only if it has a material evidence channel; `WORLD` collision truth is not secretly copied into `ACTOR` belief.
5. The core query is read-only. Contact resolution outputs a proposed state. The authoritative transaction commit belongs to the world orchestration layer.
6. Consequential work never silently disappears. `PENDING_BUDGET`, `INVALID_START`, and `INVALID_INPUT` are distinct from `SOLVED`.
7. Deterministic replay requires stable contact ordering and authoritative field serialization; quantized hash is diagnostic rather than proof of universal floating-point determinism.
8. Collision detail degrades by conservatively stopping/falling back, never by allowing a forbidden crossing.
9. Contact history is bounded and event-like. Hot events may age into a cold summary; meaningful active contact identity must not be erased until an explicit end/revision.
10. The physical solve graph and the semantic/nexus graph remain distinct; only material causal boundary crossing connects domains.

## Current implementation boundary
`nf18a_sweep_aabb` sweeps an upright radius/height **box** against a static or linearly moving AABB in relative coordinates. `nf18a_solve` computes a small earliest-TOI contact set, clips remaining displacement along normals, estimates normal impulse for the kinematic body, and either solves or explicitly defers. `nf18a_history_record` records begin/persist/end and threshold hit summaries. `nf18a_extract_world_colliders` adapts `NfWorld.colliders` (excluding ladder triggers) without altering world data. Ramps are not yet represented.

This is NOT a general six-degree-of-freedom rigid-body dynamics solver; there is no rotational support, actual momentum exchange, friction constraint, positional depenetration, surface feature identity, persistent resting-contact query, shape-specific capsule narrowphase, or cell/nexus contact authority commit.

## Next proof sequence
A. Baseline legacy endpoint query vs swept AABB with common high-speed inputs.
B. Independent from-input shape challenger (true capsule), including edge support and low stair cases.
C. Dynamic-mass contact solver / actual equal-and-opposite impulse conservation.
D. Contact island + material/semantic Cell–Boundary–Nexus event translation.
E. Network prediction/reconciliation, client-vs-authority parity, and player-facing 1.8A feel data.

## Industry analogs (not copied code)
Box2D separates collision queries from dynamics and exposes TOI/shape casts, contact events and contact manifolds; PhysX distinguishes character controller overlap recovery, steps and slopes; Jolt supports character virtual controllers with motion/physical interactions. Our architecture follows those separations while preserving the nightfall contract/epistemic rules.

## Run
`./v18a1.sh all` (requires C11 compiler, libc math, Python 3). It emits a 22-assertion fixture result, 12,000-row comparative CSV and scientifically graded `PAR_RESULTS.md`.