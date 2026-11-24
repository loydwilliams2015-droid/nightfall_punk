# Finite-world simulation ecology — governing engineering principle (research adoption)

> **Architectural objective: efficient simulation ecology, not simply more complicated collision physics.**
>
> Simulate the smallest sufficient portion of the world at the lowest fidelity consistent with observable causality, conserved quantities, player agency, and reproducible results. **Do not attempt to simulate the entire universe.**

This policy constrains future design; it does not imply a completed in-game simulation system.

## Operational interpretation

- **Relevant/near/active:** run actual detailed source physics and material interactions when the actor, object or consequence requires them, under a declared tick budget.
- **Relevant but nonconsequential when budget-constrained:** represent state through an explicitly defined summary, retaining mass/energy inventories and causal records. Summary is *not* assumed to be microscopic physics. Do not synthesize impacts.
- **Irrelevant/inactive:** keep durable canonical metadata or dormant state without stepping its internals. Reconstruct only from legitimate source state when reactivated.
- **Consequential but unavailable:** **PENDING**. If a player action would require detail not present, do not forge a collision, permission, energy transfer or A6 commit to fill the gap.
- **Material uncertainty:** canonical A5 `MIXED` with nonresident fine data stays PENDING in selective mode; coarse-only conservatively returns SOLID. No test may convert a defensive SOLID into FREE merely to pass CI.
- **Simulation and authority:** selection of a simulation region is only a scheduling hint. B2 grants, A4 contact witnesses, A5 material facts, B3 effects and A6 publication remain independently authoritative, immutable and atomically bound.
- **Conservation:** across detailed/summary/dormant boundaries, keep ledger totals and causally significant events; skip computation, never skip accounting.
- **Spatial/temporal multiresolution:** interest-driven refinement and hysteretic retention may reduce oscillation, but must use bounded resident memory and explicit revision/epoch provenance.
- **Graceful degradation:** when time is exhausted, lower rendering/secondary granularity first. Never retroactively make a consequential transaction “valid” or silently drop a necessary interaction.
- **Scientific accounting:** report detailed, summarized, dormant and pending populations; CPU time/tick, resident bytes, event backlog, aggregate conserved quantities, source epoch and rejected actions. A 60 Hz full-loop target remains a design hypothesis until measured.

## O1 prototype: deterministic simulation frontier

`src/shared/nf_retc_h319_simulation_frontier.h` introduces a **pure**, bounded, integer-priority scheduler for up to 16 canonical region IDs. Ranking is determined by consequential relevance, observation, activity, spatial distance and capped idle age, breaking ties by stable ID. Each requested region declares fixed work cost and residency. No dynamic memory allocation, floating priority, source mutation or world tick publication.

Four output decisions: `DETAIL`, `SUMMARY`, `DORMANT`, `PENDING`. Budget-starved consequential regions are PENDING, not SUMMARY. Pending work is a refusal of evidence, **not** a successfully deferred transaction.

`src/tests/test_retc_h319_simulation_frontier.c` exercises 17 resource budgets, reversed source ordering, nonresident consequential evidence, budget saturation, and invalid input no-output-mutation. GCC/Clang strict C11 CI runs independently of A6 world. A passed test certifies only **the scheduler policy**, never gameplay physics or cosmological simulation.

## Bounded implementation stages

| Stage | Source of truth | Work admitted | Outside the boundary |
| --- | --- | --- | --- |
| F0 research | A5 canonical query / prototype inventories | 8 ledger cells; 16 edges | not a world simulation |
| F1 frontier policy | immutable region identity / declared costs | at most 16 region candidates | summary, dormant or PENDING |
| F2 source-wired | A6 owner immutable frame; A5 epochs | only relevant/observed contacts and material | fail-closed rather than fabricate physics |
| F3 persistence | durable snapshot + journal + recovery | replay causally necessary impacts | unimportant detail reconstructed from canonical summaries |
| F4 measured game | actual player and object sessions | frame/tick budget and bounded tail latency | nonconsequential work may decay gracefully |

**Acceptance condition:** Every gameplay-consequential rejected or deferred item remains unresolved until a source-backed decision is possible; every accepted transfer preserves global inventories; every committed event has a unique authoritative source. O1, H319 and M18C.2 are **OPEN** until independent source-integrated tests pass.

## Current CI diagnosis

At commit `eab40457`: three inherited research workflows failed because their tests expected `NF18A5_Q_PENDING` from `NF18A5_COARSE_ONLY` material policy, which intentionally returns `NF18A5_Q_SOLID` for MIXED. Corrected these three *test fixtures* to `NF18A5_SELECTIVE_FINE`, where unresolved MIXED correctly returns PENDING. The production A5 implementation was not weakened. Do not certify repaired suites until exact-head CI runs succeed.

## Sole reference

**Oxygen Not Included** provides the current external design reference for environmental causality and practical region-level simulation. The project is not committed to reproducing its complete gas/liquid, thermodynamic, electrical or biological world. Select the minimal mechanics necessary for a coherent player experience.
