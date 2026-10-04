# nightfall!punk 1.8A.2 — Chosen contact architecture

## Design locks (project policy, not automatic H3 promotion)

1. **Primary body geometry**: upright capsule for ordinary movement. Support/step and ladder affordances may switch to a rounded-box *contact profile* only after authoritative clearance, material support/attachment and affordance checks. A shape/profile change cannot legalize a material intersection.
2. **Motion ownership**: motor-driven dynamic body. Input specifies desired velocity or motion and a capped motor force/acceleration, *not teleportation*. Dynamic-dynamic normal contacts exchange equal-and-opposite impulses. Input motors are explicit external work; the energy accounting should distinguish work from contact dissipation and restitution.
3. **Contact solve**: cheap deterministic initial pass (2 iterations provisional), then an additional 2 as needed, with hard maximum 6. Exhaustion yields explicit PENDING, not fabricated free motion. The v1.8A.1 swept-box CCD remains incumbent until true capsule CCD and integrated dynamic contact satisfy independent PAR.
4. **Space**: 1m canonical authority, optional fine 0.5m and 0.25m cells loaded only for relevant 4m chunks. A chunk must contain *real authoritative fine data* to claim fine resolution; changing a flag without data is invalid. Cache capacity failures remain explicit. Separate physical solve island from global semantic/nexus relations.
5. **Time**: each committed contact sample enters a bounded per-contact accumulator. Every 15 authoritative 60Hz ticks (250ms provisional) summarize impulse sum, peak, approach speed and digest. Successive summaries may trigger persistent events from peak, accumulated impulse or sampled duration (60 ticks provisional). Short rare spikes can be promoted at a guarded critical barrier when tested. Ordinary subthreshold history ages into bounded summaries plus integrity digest. Long-term archive retains *meaningful threshold-triggered event records*, not every 60Hz raw sample.

## Logical separation

- Contact event recording depends on successfully committed authoritative tick/version. A contact *proposal* is never a historical fact.
- Actor-local evidence does not equal world contact history.
- A proposed support affordance is not physical permission to pass an obstacle.
- An impulse estimate is not a two-body exchange. The v1.8A.2 pair-normal impulse formula resolves two linear bodies, but not six-degree rigid-body islands/rotational inertia.
- Collision geometry can be more precise than canonical semantic cells without forcing global cell refinement.
- A threshold is a *consequence policy*, not an ontological definition of whether a physical contact occurred.
- Below-threshold samples may matter cumulatively, and summaries must therefore accumulate continuous and peak effects instead of dropping all small events.
- Storage policy is not the same as observability: ephemeral detail may be available for immediate diagnostics even when the eventual long-term record is compact.

## Status: what is compiled

- Capsule-to-AABB exact static distance/overlap query, plus authorization-gated capsule/rounded-box profile selection.
- Bounded force/acceleration motor update; equal-and-opposite normal impulse exchange for two linear bodies.
- Adaptive wrapper over retained AABB swept contact solver, 2->4->6 max passes.
- 4x4 canonical occupancy/material cells per loaded 4m chunk; explicit 8x8/16x16 fine load data, version stamp, capacity and deterministic idle eviction.
- Bounded, post-commit per-contact history aggregation with 15-tick summaries, cumulative/peak/duration threshold promotion and cold digest/eviction statistics.
- Strict C11 tests and seeded baseline.

## Not yet compiled / not proven

- Actual continuous swept capsule AND rounded box narrowphase/manifold resolution; no shape switching is wired into production `nf_movement_step_actor` yet.
- An integrated motor + world solve applying all reciprocal impulses, rotational inertia, friction, warm starts, gravity/support contact and moving platforms together.
- Spatial-indexed broadphase; the inherited sweep still scans the collider list.
- Actual cell/nexus material-state loading from the production world and distributed authority.
- Permanent disk/network storage for promoted events. The current event ring is an *in-memory staging cache* and **does not constitute durable long-term storage**. Storage-before-eviction and replay verification are hard production gates.
- Verified inter-platform fixed-point authority equivalence, client prediction or H4 feel.
- Optimum threshold values (15 ticks, 60 sample ticks, peak 20 Ns, cumulative 80 Ns): all are initial adjustable test priors.

## Next severe tests

A. Shape-specific swept narrowphase: capsules versus rounded-box stair/ladder edge cases under high speed, normal symmetry, safe steps and clearances.
B. Dynamic broadphase/body islands: finite masses, friction, restitution, stacked supports, reciprocal impacts, energy accounting and warm starting.
C. Adaptive budget workload ramps: 3+ sequential contacts, budget pressure, starvation and unresolved authoritative pending.
D. Chunk streaming cross-seam continuity, sparse/world-scale scaling, negative coordinates and invariance under chunk relocation.
E. History falsification: contact gaps, long-duration low-level loading, peak impulses, delayed threshold crossing, concurrent contacts, durable event backpressure and reader replay.
F. Human play, network replay and Pop!_OS performance.

## Scientific disposition

Lakatosian research programme: hard core material authority, no fabricated contact, bounded causality, actor-relative evidence. Adjustable protective belt: shape combination, motor tuning, 2->4->6 scheduler, chunk granularity, summary period and thresholds. Use Quinean auxiliary-constraint audits, Kuhnian anomaly fixtures, Feyerabendian retained incumbent/challenger methods and Popperian falsification. Promotion from H1 to H3 requires integrated compiled simulation and representative native performance; H4 requires player trials.
