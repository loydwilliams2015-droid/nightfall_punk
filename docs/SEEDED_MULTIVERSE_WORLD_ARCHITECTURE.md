# nightfall!punk — Seeded Multiverse World Architecture

Status: **long-horizon world-scale architecture contract; current small maps remain bounded proofs, not claims of present MMO-scale implementation.**

## Governing thesis

nightfall!punk should scale toward a multiplayer world in which thousands of players are **diffuse across many persistent seeded local worlds**, rather than concentrated inside one monolithic megamap or one globally high-resolution simulation.

> **Large multiplayer population != large local simulation population.**

> **Seed is infrastructure; server is truth.**

The intended hierarchy is:

`MULTIVERSE -> SEED WORLD -> REGION/CELL -> LOCAL SIMULATION -> ACTOR`

with authority approximately:

`ACCOUNT/DIRECTORY -> SEED AUTHORITY -> CELL AUTHORITY -> LOCAL SIMULATION SYSTEMS`

## Seed-world scale

A seed world is a substantial persistent locality. The current preferred long-horizon scale is **up to roughly 8 km x 8 km = 64 km² per seed world**, while retaining the option to use smaller bounded 8x8-style unit arrangements when simulation, geography, hosting or design evidence recommends them.

The architectural commitment is therefore **hierarchical locality**, not one immutable cell size.

A seed may contain an 8x8 lattice of operational cells. Example only:

- 8 x 8 cells = 64 cells;
- 1 km x 1 km cells would yield one 8 km x 8 km seed world;
- smaller cells remain valid where higher granularity improves authority transfer or simulation loading.

Cell size is an engineering parameter. The seed-world/cell distinction is the durable contract.

## World address

Long-horizon spatial identity should be representable as:

`LOCATION = SEED_ID + CELL_ID + LOCAL_COORDINATES`

rather than requiring one enormous global floating-point coordinate plane.

Illustrative C shape:

```c
typedef uint64_t NfSeedId;

typedef struct {
    NfSeedId seed;
    int32_t cell_x;
    int32_t cell_y;
    NfVec3 local_pos;
} NfWorldAddress;
```

Exact types are deferred; the address decomposition is the contract.

## Generated baseline + causal delta

A seed is not merely an RNG convenience and a seed world is not a disposable MMO instance.

Current-world state should conceptually satisfy:

`CURRENT_WORLD = GENERATE(SEED) + PERSISTENT_CAUSAL_DELTA`

Generate deterministically what can be regenerated. Persist departures that matter historically.

Examples of persistent deltas:

- changed/destroyed/repaired infrastructure;
- contamination and ecological traces;
- territorial occupation/history;
- important inventory/object provenance;
- social commitments and local institutions where implemented;
- player/community construction where later supported;
- durable resource changes;
- event consequences.

Storage should track **meaningful change** rather than blindly serializing every generated static primitive.

## Hierarchical deterministic seeds

Avoid a single fragile RNG stream. Child systems should derive deterministic sub-seeds from stable parent identities, for example:

`child_seed = HASH(parent_seed, subsystem_or_object_id)`

Potential namespaces include terrain, architecture, infrastructure, ecology, resources, culture, event eligibility, cell and object generation.

Changing one generator should not silently reroll unrelated world domains.

## Simulation-resolution contract

World scale and simulation scale are deliberately different.

nightfall already uses the principle:

- simulate consequences globally;
- simulate intentions regionally;
- simulate embodiment locally.

The seeded multiverse extends that into multiplayer infrastructure.

Suggested fidelity ladder:

1. **DORMANT / persisted** — no embodied simulation required;
2. **STATISTICAL** — bounded aggregate stocks, ecology, territory, infrastructure and scheduled transitions;
3. **REGIONAL / proxy** — important actors or groups retain identity, intent and causal travel without full bodies;
4. **LOCAL EMBODIMENT** — precise bodies, perception, navigation, affordances and inventories exist;
5. **ENCOUNTER / FULL LOCAL** — 60 Hz movement/combat/physics and other high-resolution relations run where causally necessary.

> **Changing simulation resolution may change precision, never history.**

On unload, compress rather than erase. On reload, reconstruct plausible embodiment from preserved causal state; do not retroactively reroll history simply because no player observed the interval.

## Diffuse multiplayer population

Thousands of concurrent players should naturally distribute across dozens or hundreds of seed worlds and their cells.

The infrastructure goal is not to guarantee low local density everywhere. Cultural hubs, events, conflicts and resource opportunities may create temporary concentration. The goal is that global concurrency does not require every player, AI actor and world process to share one high-resolution authority domain.

Compute should broadly follow current causal activity:

`COMPUTE(seed/cell) ~ CURRENT_CAUSAL_ACTIVITY`

Sparse or empty localities may collapse to cheaper representation; busy localities may receive additional server resources.

## Server authority and transfer

Global/directory services may own slow or cross-world concerns such as identity, authentication, seed registry, persistence routing, inter-seed travel and higher-level social/community metadata.

They do **not** author local bullets, collision or ordinary actor cognition.

Cell/local authority owns high-resolution local truth such as:

- movement and collision;
- combat;
- local physics;
- local AI embodiment;
- local contamination transfer;
- immediate affordances;
- high-resolution environment interactions.

Seed authority may coordinate slower seed-scale processes such as regional ecology, infrastructure flows, migration, event eligibility and persistence orchestration without making local actors omniscient.

Cross-cell traversal should become **authority transfer**, not respawn. A handoff eventually preserves relevant actor identity/state/history across source and destination authorities. Seamless boundaries may use bounded overlap/interest halos when engineering evidence requires them.

## World geography, not disguised server browser

The long-horizon goal is that seed worlds become persistent places with distinct histories, not interchangeable `Server #42` copies.

Population may be redistributed through world relations such as:

- travel infrastructure;
- Ghost Static events/announcements;
- social contacts/invitations;
- resource opportunities;
- faction/ecological pressure;
- cultural hubs;
- discovered routes and seed-local history.

Technical sharding should support emergent human/ecological geography rather than replace it.

## Present-development boundary

This document is **architecture**, not a claim that nightfall presently supports thousands of players, seamless distributed authority, inter-seed travel or 64 km² production worlds.

Current small arenas/labs are to be treated as **local-cell implementations of the eventual hierarchy** where doing so does not burden present proofs.

Do not prematurely implement distributed systems merely to imitate the final scale. Preserve compatible seams now:

- stable seed identity;
- local coordinates;
- bounded regional/cell state;
- authority boundaries;
- deterministic generation namespaces;
- causal persistence/delta thinking;
- simulation LOD that preserves history.

## Relationship to current architecture

This contract extends rather than replaces:

- `Seed is infrastructure; server is truth; clients predict/reconcile`;
- server truth != individual knowledge != group report;
- large world != large per-agent thought;
- simulate consequences globally / intentions regionally / embodiment locally;
- changing simulation resolution may change precision, never history;
- minimum sufficient machinery; maximum relational consequence.

## Epistemic status

- **DESIGN COMMITMENT:** hierarchical seeded multiverse; seed-world/cell distinction; generated baseline + causal delta; locality-based simulation and authority.
- **ASPIRATION:** thousands of players distributed across dozens/hundreds of persistent seed worlds; seamless cross-cell/cross-seed travel at mature scale.
- **OPEN QUESTION:** final production seed size, cell size, handoff protocol, persistence database topology, orchestration stack, failure recovery, hotspot splitting and exact inter-seed travel rules.
- **NOT IMPLEMENTED:** do not report present v1.x code as already providing MMO-scale sharding or distributed world authority.

## Maxims

> **A large world is many authoritative localities with conserved history, not one gigantic coordinate plane.**

> **When nobody is looking, compress the world; do not erase it.**

> **Generated sameness is a starting condition. History makes a place.**

> **The present map is a local proof of an architecture that should not need to be thrown away when the population becomes large.**
