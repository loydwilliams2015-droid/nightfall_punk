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

## O1 actual A5 material-source preflight (new)

The frontier now has a bounded source-linked adapter:
- `src/shared/nf_retc_h319_a5_frontier.h`: binds the pure region scheduler to `nf18a5_query` using a source material epoch; makes only selected DETAIL-region A5 queries; downgrades unresolved or blocked consequential requests to PENDING and nonconsequential requests to SUMMARY. Cost already reserved by the scheduler is **not redistributed** within the tick. This avoids invisible second chances or accidental unfair reallocation.
- `src/tests/test_retc_h319_a5_frontier.c`: five canonical candidate regions, three actual A5 chunks (FREE, MIXED, SOLID) plus missing material, 13 simulation budgets, reversed-order reproducibility, canonical epoch mutation and invalid-source refusals.
- `.github/workflows/retc-h319-a5-frontier.yml`: exact-source GCC/Clang strict-C11 compilation and diagnostic verification.

**Limits:** A5 reads update cache-access bookkeeping; this is not a pure read-only lookup in all senses. The adapter does not edit canonical voxels, ledger inventories, actor bodies, world ticks, B2 authorizations or B3 effects, and does not imply A6 publication. Its per-region source sample is an inspection point, *not* complete capsule/sweep clearance. Stable source identity binding to an actual A6 frame, cross-frame summary replay, and production environment scheduling remain OPEN.

**Minimum review gate:** Budget cannot be exceeded, source epoch must match, input order must not affect decisions by region ID, missing/MIXED/SOLID material can never be interpreted as a successful detailed interaction, and invalid requests cannot modify the output receipt. Exact-head CI must PASS before describing this as a verified research milestone.

## O1 A6-owned snapshot gating (research stage only)

New `src/shared/nf_retc_h319_owner_frontier.h` checks actual `NfWorld` and `Nf18a6Runtime` fields before calling the source-bound A5 frontier:
- world tick is nonzero, representable in material tick units and at a safe tick boundary;
- world tick owner exists and its context is the exact supplied runtime object;
- A6 runtime's latest committed tick and its material-world tick equal the authoritative world tick;
- owner actor ID is nonzero and has a live, active source actor in the world roster;
- material revision is nonzero and A5 provenance matches the source epoch;
- a fresh receipt is written only after all checks, with the world tick, local material revision, material epoch and owner identity.

`src/tests/test_retc_h319_owner_frontier.c` uses a controlled, **synthetic** bound-owner fixture to sweep 32 work budgets and refuse eleven detached/stale/inconsistent owner-and-caller provenance conditions without altering the output receipt. It does **not** execute `nf18a6_step` or exercise the true source-authentication lifecycle. The accompanying GCC/Clang workflow `.github/workflows/retc-h319-owner-frontier.yml` is a source-field validation laboratory, not an A6 publication test.

**Practical boundary:** Input selection at an A6 owner boundary is a preflight hint, not authority for contacts, permissions or consequences. A5 queries may alter last-access/cache bookkeeping. It does not write B2 grants, B3 effects, A6 frame state, or durable journals. Full 60 Hz runtime time/memory benchmarking, deterministic summary-to-detail replay, source-authenticated A6 binding and H319 coupled physics still remain OPEN.

**Caller-supplied provenance update:** the owner preflight now requires exact expected tick, local material-world revision and canonical A5 epoch supplied by the caller. Neither later material changes nor stale caller assumptions can be automatically adopted as an acceptable snapshot. The extra two negative tests cover stale caller revision and material epoch. This does not provide concurrency-safe world-lock ownership or a complete A6 commit certificate.

## O1 lifecycle-bound A6 source regression (new)

The prior H319 source guard used a controlled synthetic `NfWorld` and `Nf18a6Runtime`. The next evidence increment extends the **existing production-path A6 tick owner regression** `src/tests/test_v18a6_tick_owner.c`, without modifying A6 production code:

1. Initialize a real world, spawn player actor 10 and bind the real A6 embodiment tick owner through `nf18a6_bind_world_tick`.
2. Load canonical A5 evidence, then use the actual `nf_world_step` owner route to achieve an authoritative committed world tick (existing S13 scenario).
3. Immediately stage one finite-world A5 region against that **same** committed world tick, the caller-pinned owner-local material revision and canonical A5 epoch.
4. Reject an incorrect expected revision, incorrect expected epoch and future tick without modifying the preflight receipt; verify the operation did not run a second world tick.

`.github/workflows/retc-h319-owner-lifecycle.yml` compiles and executes the full tick-owner regression under GCC and Clang, capturing logs and requiring the five new H319 assertions. These checks are **read/stage preflight only**: no extra B2 grant, B3 effect or A6 joint transaction. The real lifecycle route is a meaningful upgrade from the standalone synthetic fixture, but not a certification of contested shared-object simulation.

**Measurement boundary:** the finite scheduler's work budget is declared **abstract units**, not measured CPU microseconds. No measurable 60 Hz budget, cache-efficiency advantage or ONI-equivalent performance should be claimed from these unit tests; next benchmark must instrument actual tick duration and resident allocation separately from deterministic correctness.
