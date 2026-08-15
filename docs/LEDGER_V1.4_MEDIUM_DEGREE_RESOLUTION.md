# nightfall!punk — v1.4 Medium-Degree Resolution Ledger

Date: 2026-08-14
Source baseline: `build/v1.2c-integrated-causality` / `27af894fb9148be11447b8e26320bf18307645d6`
Status: **DESIGN RESOLUTION — 150/150 CLOSED**

> Baseline reconciliation: this ledger was originally drafted while v1.2b was the latest proven line. The green v1.2c corrective integrated-population candidate now supersedes that FROM state. The 150 dispositions remain accepted; this correction does not reopen them. v1.3 is intentionally unused.

## Governing thesis

v1.4 should generalize relations that v1.2–v1.2c already prove rather than add parallel authorities or feature-count complexity.

Primary causal chain:

```text
WORLD CHANGE
    -> TOPOGRAPHICAL + TOPOLOGICAL REVISION
    -> AFFORDANCE REVISION
    -> LEGITIMATE EVIDENCE
    -> DIVERGENT BELIEF / UTILITY
    -> CLAIMS / OCCUPANCY
    -> SNAP LOCOMOTION
    -> AUTHORITATIVE PHYSICS
    -> CONSEQUENCE
    -> TRACE / RESOURCE / ROUTE / EXCHANGE CHANGE
    -> NEW EVIDENCE
```

Implementation law remains:

> **ExistingSystem -> CompatibleExtension -> MeasuredImprovement.**

## Topography / topology distinction — LOCKED

These terms are related but not interchangeable.

### Topography

Topography describes the **shape and distribution of embodied/material relations in geographic or metric space**.

Use it for:

- GIS-like spatial distribution;
- distance;
- elevation;
- slope / terrain;
- exposure;
- density / concentration;
- gradients;
- physical access;
- geographic placement of actors, stocks, infrastructure, hazards, resources and routes.

Question form:

> **WHERE is it, HOW is it distributed, and WHAT spatial shape/cost does that distribution create?**

### Topology

Topology describes **relational structure under allowed transformations**, including exchange, conversion, dependency and connectivity relations among energy accounts, goods, actors, processes and opportunities.

Use it for:

- what can exchange with what;
- what can convert into what;
- substitutions;
- dependency chains;
- cycles;
- bottlenecks / cut relations;
- relational equivalence;
- which exchange/conversion structures remain connected or viable despite changes in metric/spatial presentation.

Question form:

> **WHAT relates/exchanges with WHAT, THROUGH WHICH relations, and WHAT relational structure remains viable or equivalent?**

An arithmetic equality is not by itself topology. A hammer costing three energy-account units while a candy bar costs two and a drink one becomes a topological game relation when the permitted exchange/conversion graph determines whether the two-plus-one bundle can substitute for, fund, or transform into the three-unit requirement.

### Energy-accounting synthesis

```text
REALIZED SITUATED VALUE
    = ACCOUNTED STOCK / FLOW
    - TOPOGRAPHICAL ACQUISITION COST
    +/- TOPOLOGICAL EXCHANGE / CONVERSION STRUCTURE
    +/- ACTOR-SITUATED INFORMATION / CAPABILITY / HISTORY
```

A place can therefore be:

- **topographically cheap but topologically isolated**; or
- **topographically expensive but topologically powerful** because many exchange/conversion paths converge there.

Do not overload one `topography_cost` or one `energy_score` with both relations.

## Southern California grid scale — LOCKED

The world is modeled as **multiplications of a Southern-California geographic grid whose base spatial element may be as small as 8 m x 8 m (64 m²).**

This 8 m x 8 m element is the **base geographic/topographical grid element**, not automatically a network authority cell, simulation region, seed world, or server shard.

Hierarchy:

```text
SOUTHERN CALIFORNIA / LAS FLORES GEOGRAPHIC SUBSTRATE
        -> 8 m x 8 m BASE GRID ELEMENTS
        -> MULTIPLES / AGGREGATIONS OF ELEMENTS
        -> REGIONS / SIMULATION CELLS
        -> SEED-WORLD LOCALITIES
        -> MULTIVERSE
```

Rules:

- fixed geographic substrate and canonical place truth may be sampled/indexed at 8 m x 8 m base resolution where appropriate;
- regions and authority cells aggregate many base elements and remain engineering parameters;
- seed worlds aggregate regions/cells and need not be 8 m units themselves;
- simulation LOD may aggregate many base elements while preserving causal history;
- topological exchange/social/resource relations may cross topographical grid, region and authority-cell boundaries;
- do not confuse an 8 m x 8 m geographic element with an 8 x 8 lattice count or an 8 km x 8 km seed-world scale.

Maxim:

> **Small grid element; large composed world.**

## 150-question medium-degree audit disposition

The v1.4 design-resolution pass closed as:

- 50 deductive `LOCK` decisions;
- 60 high-confidence `ADOPT` decisions;
- 27 `INVALIDATE` decisions where stronger architecture ruled an option out;
- 7 `DEFER` decisions compatible with the project but not earned by v1.4;
- 6 former `ASK` decisions, resolved below by user acceptance of recommendations.

The topography/topology correction changes interpretation of several energy/economy relations but does not reverse the 150 dispositions.

## Six formerly open questions — resolved

### R10 — teammate command authority

**ACCEPTED:** player pings/orders are strong coordination/salience bids plus a small class of explicit emergency commands.

- no universal possession of teammate agency;
- teammate action remains situated and evidence-disciplined;
- commands may raise priority or establish a scoped coordination compact;
- physical impossibility and authoritative world truth still override commitment;
- emergency commands should be few, legible and explicitly scoped.

Status: **design policy; exact command vocabulary remains an implementation constant.**

### R11 — Human Rival strategic deception

**ACCEPTED:** truthful-but-incomplete reporting first; deliberate misinformation later only when it creates legible strategic play.

- v1.4 must first prove provenance, latency, confidence and incomplete-information reporting;
- strategic deception is not required for the first implementation;
- later deception must have incentives, provenance and possible detection/correction;
- do not use random lying as artificial unpredictability.

Status: **truthful/incomplete reporting is v1.4 priority; deliberate deception deferred.**

### R12 — production local actor cap

**ACCEPTED:** do not canonize one local actor number yet.

Profile at least:

- ordinary tier;
- surge tier;
- stress tier.

Derive local caps from:

- CPU budget;
- memory/query budget;
- network bandwidth/relevance budget;
- occupancy/pathfinding failure rates;
- AI cadence cost;
- human readability and encounter quality.

Status: **measurement policy; no fixed production cap yet.**

### R13 — seed / cell dimensions

**ACCEPTED:** larger region/cell/seed dimensions remain engineering parameters rather than one fixed canonical size.

The fixed spatial statement is the **8 m x 8 m base Southern-California grid element**. Larger partitions are integer/structured aggregations chosen from evidence.

Relevant constraints include:

- traversal density;
- relevance radius;
- persistence cost;
- simulation-LOD cost;
- hotspot pressure;
- authority handoff cost;
- topographical distribution;
- topological exchange/dependency relations crossing boundaries.

Status: **base element fixed; aggregation scales open to evidence.**

### R14 — Apophis camera exception

**ACCEPTED:** defer exceptional authored Apophis camera behavior until ordinary camera/viewmodel/audio integration is production-stable.

Any future exception:

- is explicitly exceptional presentation;
- does not rewrite authoritative body/world truth;
- does not inject knowledge into actor cognition;
- must preserve player comprehension and accessibility.

Status: **deferred.**

### R15 — v1.4 integrated human proof

**ACCEPTED:** one integrated shared-world causal scenario is the preferred v1.4 proof.

Proof grammar:

```text
PLAYER / WORLD CHANGES A MEANINGFUL OBJECT OR ROUTE
        -> TOPOGRAPHICAL AND/OR TOPOLOGICAL CONDITIONS CHANGE
        -> TEAMMATE / RIVAL / CATTLER RECEIVE DIFFERENT LEGITIMATE EVIDENCE
        -> THEIR BELIEFS / VALUES DIVERGE
        -> OCCUPANCY / RESOURCE / ROUTE PRESSURES DIVERGE
        -> EACH CHOOSES DIFFERENTLY THROUGH EXISTING CONTROL + SNAP LOCOMOTION
        -> SHARED AUTHORITATIVE WORLD RESOLVES THE ACTIONS
        -> PLACE RETAINS A READABLE TRACE / RESOURCE / ROUTE / EXCHANGE CONSEQUENCE
```

The central proof proposition is:

> **One material event can rationally generate different actions because common reality does not imply common knowledge, common topographical cost, common topological opportunity, or common utility.**

Status: **preferred v1.4 human acceptance scenario.**

## PRAXIS — v1.4 implementation spine

1. Preserve v1.2c camera, movement, server authority, teammate causality, integrated population and negative-control proofs.
2. Generalize dynamic object/affordance revision locally with stable IDs/revisions and immediate material invalidation of impossible commitments.
3. Generalize one bounded hard/soft/temporal claim lifecycle across cover, traversal, interaction and revive use; player occupancy remains implicit and never yields player control to AI arbitration.
4. Move teammate causality toward production peer integration while preserving separate evidence and bounded Crew reporting.
5. Enrich situated utility with graded cover, directional exposure, future-option value, information gain, short-horizon congestion, actor-belief energy opportunity, topographical acquisition cost and topological exchange/conversion opportunity.
6. Build one mixed-resource focal-point experiment containing both predictable designed recurrence and one systemic production/replenishment relation.
7. Add a minimal causal waterline/trace layer whose persistence depends on material/history rather than one universal TTL.
8. Instrument route distributions, occupancy waits/deadlocks, exposure, resource traffic, traversal failures and query budgets; examine tails/failure cases as well as means.
9. Keep production replication relevance, distributed authority and large-scale cell orchestration as seams until the bundled scale gate is satisfied.
10. Prove integration through one coherent scenario rather than fifteen isolated demonstrations.

Items 6–9 remain broader design commitments; the bounded v1.4 implementation may DEFER them when the current proof has no direct consumer.

## SIGNAL — interpretive rule

Use literary/game-theoretic/scientific ideas as relational constraints rather than feature names:

- **polyphony:** distinct actors may rationally interpret the same world differently;
- **focalization:** player experience is privileged without making player knowledge/world position ontologically privileged;
- **unreliable narration:** reports may be incomplete, mistaken or later strategically deceptive, but must retain provenance;
- **chronotope:** place is topography + time/history + recurring activity, while topology captures exchange/dependency structures that may become part of the place's organization;
- **Simondon / individuation:** actors and places acquire identity through ongoing relations rather than fixed essence;
- **niche construction / stigmergy:** repeated activity can alter topography and/or topology and therefore alter future strategies;
- **repeated and congestion games:** reciprocity, crowding, claims and route competition carry history into future payoff;
- **bounded rationality:** actor thought remains local, multi-rate and evidence-limited even when world truth is larger.

## STEWARD — terminology and status

Canonical current locomotion name remains **Snap Locomotion / `snap_locomotion`**. `Fuzzy Rail` is historical terminology only.

For current/future design records:

- say **topography** for distributional/geographic/metric shape;
- say **topology** for connectivity/exchange/conversion/dependency structure;
- say **8 m x 8 m base grid element** for the smallest canonical Southern-California spatial unit;
- reserve **region/cell/authority cell** for aggregates chosen for simulation/networking purposes;
- distinguish design policy from implemented/tested behavior.

## Closed design status

The six former questions are dispositioned according to the accepted recommendations. The v1.4 **design-resolution layer is closed at 150/150 questions**, while implementation constants, profiling thresholds and human tuning remain open and evidence-driven.
