# UCW-1 — provisional 1.8C architecture lock and industry-practice review
Date: 2026-10-20. PRAXIS / SIGNAL / STEWARD.
Parent: 1.8C.0 research PR #63 exact source `458c03a7541b5fa24b9fb1db820afffba7b3ff68`.
**Status: LOCKED AS RESEARCH DIRECTION ONLY. NO H3 INTEGRATION, PRODUCTION ACCEPTANCE, DEFAULT-BRANCH MERGE, OR RELEASE.**

## Selected eight groups (all provisional)
- G1 durability: one complete versioned physical+contract record plus event log/checkpoints.
- G2 power loss: acknowledgement after confirmed durable commit, paired verified checkpoints, interruption/fault testing.
- G3 reconnect: single authoritative server as incumbent; replication or regions deferred until independently justified, requiring fenced leadership if attempted.
- G4 actors: shared immutable per-tick snapshot, deterministic ordered arbitration, nonconflicting parallel subgroups, reservations and compensation.
- G5 responsiveness: fixed mandatory solve budget plus measured adaptive optional scheduling and tested safe parallelism.
- G6 world: chunked sparse 1m canonical grid, 0.25–0.5m selective refinement, bounded local causal frontiers.
- G7 governance: source-pinned accepted commit, contract provenance, licensing and staged reviews.
- G8 science: independent small-world oracle, property-based five-stratum tests, adversarial end-to-end and consented human evidence.

## Three accepted design constraints, not implemented claims
1. **Durability vs frame time:** staged immutable A6+A4+A5+B2+B3 candidate, one versioned B5 WAL envelope including action/object/actor/contract IDs, disk sync on a separate worker if safe, and no durable acknowledgment or publication of final irreversible state before verified commit. Group commit only if its batching deadline and acknowledgment consequences meet measured p95/p99 targets. A provisional client representation must remain cancellable/reconcilable. Log replay and double checkpoint with checksums/genesis/version fences are separate required proofs. If a whole-world transaction cannot fit within real latency budgets, scale granular domains or partition by independent safe boundaries without splitting one atomic action. Standards analogues: Unreal asynchronous SaveGame; FoundationDB durable ordered mutation log, fsync before confirmed commit.
2. **Parallelism vs deterministic arbitration:** fixed tick, immutable proposal snapshot, explicit read/write and contact dependencies, deterministic stable transaction ordering, actor/resource conflict graph, ordered reductions, platform-specific float controls or integer/fixed-point authority for contractual and canonical conserved quantities. Parallel workers never own a second authoritative world. A claim of cross-platform bit identity requires actual cross-compiler/architecture tests; NVIDIA PhysX documents platform-dependent floating point and optional enhanced island determinism with performance cost. Parallelize broadphase/independent physics islands/proposals first, serial or deterministically reduce overlapping constraints.
3. **Causal frontier vs complete authoritative truth:** distinguish `resolved`, `provisional`, `pending` and `quarantined` fields/ticks at domain boundaries. A frontier budget cannot guarantee arbitrary pathological physical cascades fit one 60Hz frame; when the budget runs out, defer the affected material frontier with exact dependencies and resume under a deterministic work queue, or withhold publication if a transaction requires the unresolved neighborhood. Unrelated proven-independent regions may continue. No dropping cross-boundary forces, heat, support, pressure, contractual publication, or conservation debt. Check every frontier truncation against an independent unbounded/reference solve.

## Industry references, not proof of implementation
- Unreal Engine official Saving and Loading: https://dev.epicgames.com/documentation/unreal-engine/saving-and-loading-your-game-in-unreal-engine?lang=en-US
- FoundationDB commit path and technical architecture: https://github.com/apple/foundationdb/wiki/Technical-Overview-of-the-Database
- NVIDIA PhysX 5 simulation deterministic behavior and enhanced determinism: https://nvidia-omniverse.github.io/PhysX/physx/5.7.0/docs/Simulation.html
- NVIDIA PhysX physics determinism platform/compilers: https://nvidia-omniverse.github.io/PhysX/physx/5.4.1/docs/API.html
- Unity Physics ECS stateless / simulation design: https://docs.unity.cn/Packages/com.unity.physics%401.3/manual/design.html
- Unity multithreaded physics read-many/write-once restrictions: https://docs.unity.com/en-us/engine/6000.7/manual/unity2d/2d-physics-api/multithreading

## Acceptance experiments (not yet executed)
- Durability: 100% no half-published A6 material/B3 action/contract after crafted cuts around append/fsync/checkpoint; exact duplicated ACK and unknown status recovery; test on actual power removal separately from simulated kill -9/torn writes.
- Determinism: same input corpus across worker counts {1,2,4,8}, order permutations, synthetic long-running conflict stress, GCC/Clang, physical momentum/conservation, identical authoritative IDs/contract states and histories. Define tolerance only for explicitly non-authoritative float presentation or solver approximations.
- Frontier: canonical/random/pathological/boundary/heldout material cascades; identical committed prefix and conserved quantities under bounded vs independent higher-budget reference, explicit PENDING where not proven, bounded backlog/fairness; 60Hz p50/p95/p99 and tail stability on named hardware.
- Governance: current default-main ancestry issue #30, GPL/REUSE source issue #36, original B1 H1 source #37, authenticated B2 owner and independent M3/M4, real multi-actor A6 and A5+B3+B5 single-envelope are OPEN.
- Independent H3 acceptance requires the real integrated world, not only H0 test double or deterministic same-source replay.

## RET classification
**A priori:** acknowledged durable transactions cannot predate actual persistence; simultaneous conflicting writes require arbitration; bounded frontier may leave unresolved effects.
**Formally specified:** selected architecture and 3 invariants above; no mathematical end-to-end proof.
**Empirically tested predecessors:** scoped 1.8B.6 H1; 1.8C.0 callback-vs-debit-WAL 320 scenarios/11,200 proposals/62,763 checks at exact prior CI. Those findings do not validate UCW-1.
**Highly predictable:** batching improves I/O amortization but increases ACK age; disjoint island computation can scale; sparse frontiers can cut work when material effects are localized.
**Uncertain:** actual full-world 60Hz p95/p99, disk sync tails, cross-platform numerical determinism, pathological causal cascades and multi-node fault behavior.
**Unresolved:** production durable envelope, representative hardware testing, authenticated multi-actor/contractual integration, source/rights and governance acceptance.

## Selection
Provisional coherent model UCW-1 LOCKED as a research recommendation. Durable B5 transaction envelope + independent crash oracle is the highest engineering frontier, jointly with controlled A6 actor arbitration. Mandatory truth beats speed; where mandatory correctness and optimization align, pursue both. Retain release HOLD and no canonical ledger mutation.
