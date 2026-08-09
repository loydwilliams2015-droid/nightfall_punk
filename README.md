# nightfall_punk

Open-source copy-left **Neo-Xennial / Systemic Arena FPS** developed in C + raylib.

**Current candidate milestone: v0.9 — Topographical Energy Accounting.** Accepted versions remain archived through `archive/v0.7`. v0.8 Dream Cattler Habitat Ecology is still a draft human-acceptance candidate on PR #11; v0.9 is deliberately stacked on that branch rather than pretending v0.8 has already been accepted.

nightfall!punk is rebuilt graybox-first: gameplay, physics, networking, combat, agent intelligence and ecological world truth must become coherent before the original aesthetic engine returns.

## v0.9 thesis

The ecological engine now begins with a **universal energy ledger inside authoritative `NfWorld`**. Energy accounting is the common currency; different actors may still pursue different strategies. The map participates in price formation: distance, route depth, elevation proxy, terrain resistance, structural exposure, productive capacity, regeneration, absorptive capacity and historical depletion all change the energetic value of a place.

The ecological cycle is conceptually:

`world truth -> energy accounting -> Bayesian helicopter search -> bounded actor belief -> strategy -> simultaneous action -> consequence -> new world truth`

The accountant may know the present world state; actors may not. `nf_energy` therefore contains both omniscient regional opportunity search and explicitly bounded/stale belief projections. v0.9 does **not** pipe omniscient energy truth directly into Human Rival or Dream Cattler cognition.

History materially changes resource availability. Human/Rival/Cattler occupancy now participates in a low-resolution world-metabolism proof: regional usable stock can be extracted, potential can be converted with loss, regenerative inputs can restore capacity, externalities can accumulate, and historical overshoot can reduce regional health. This is graybox accounting, not the final gathering interaction.

### C20 accounting invariants

- transfer moves energy; it does not create it
- conversion can create usable output only by consuming potential input and recording loss
- capacity changes future throughput without minting current stock
- regeneration is recorded as external input to the modeled regional system
- topography changes acquisition cost rather than changing conservation bookkeeping

### C21 helicopter-search contract

- bounded 24-region search, not a giant per-object strategic solve
- world-level search may inspect authoritative present state
- actor belief search uses prior + observed evidence + confidence + staleness
- perfect present accounting does not imply perfect future prediction
- deeper horizons are alternative valuations of the same ledger, not separate currencies

### C22 proof pair

The test harness deliberately contrasts:

- **South Works** — high initial usable stock, weaker renewal/absorption, worse terrain/access economics
- **Signal Court** — lower initial stock, stronger productive/regenerative capacity and cheaper topographical access from the central lab

Sustained Rival occupation depletes South Works while Signal Court continues to regenerate. A stale belief and a newly observed belief therefore assign different energetic prospects to the same history-changing world.

## Existing gameplay baseline

v0.9 preserves the accepted and candidate systems underneath it:

- fixed 60 Hz authoritative simulation
- shared Fuzzy Rail movement/traversal
- ENet transport + prediction/reconciliation
- authoritative hitscan/rewind/combat/death/respawn
- Human Rival perception, utility, bounded encounter pressure and spatial ecology
- 0.40 km² / 24-region situated-agency lab
- Dream Cattler habitat, infestation, imperfect pack evidence, locomotor weak points and internal ecology ledger

The production map target remains 8 km². The current lab remains intentionally **0.40 km² (500 m x 800 m)** so architecture is falsifiable before scale increases.

## Build and test

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh combat-smoke
./nightfall.sh encounter-smoke
./nightfall.sh spatial-smoke
./nightfall.sh cattler-smoke
./nightfall.sh energy-smoke
```

`energy-smoke` writes `build/energy-smoke.log` and proves transfer/conversion/capacity invariants, topographical costs, historical depletion/regeneration, helicopter ranking, and stale-vs-fresh Bayesian beliefs.

## Graphical demo

```bash
./nightfall.sh local
```

Default local configuration remains four Human Rivals, two Human-Rival pressure slots, and three Dream Cattlers. Red actors are Human Rivals. Tall orange actors are Dream Cattlers; yellow lower-body bands are temporary knee/foot weak-point diagnostics.

Press **F4** for the accepted 24-region graph. The energy ledger remains server/test-side in v0.9; it is intentionally **not** exposed as a player-facing score HUD.

Controls: WASD move, mouse look, Shift sprint, Ctrl crouch, Space jump, E interact/ladder, left mouse fire, R reload, 1 carbine, 2 pistol, F4 spatial graph, F10 toggle/reacquire mouse capture.

Useful debug variants:

```bash
NF_AI_COUNT=0 NF_CATTLER_COUNT=3 NF_CATTLER_PROFILE=pack ./nightfall.sh local
NF_AI_COUNT=0 NF_CATTLER_COUNT=1 NF_CATTLER_PROFILE=loner ./nightfall.sh local
NF_AI_COUNT=0 NF_CATTLER_COUNT=5 NF_CATTLER_PROFILE=horde ./nightfall.sh local
NF_RIVAL_TRUCE=1 ./nightfall.sh local
```

## Human acceptance target

For v0.9, record the normal graphical demo to prove no regression in movement, networking, combat, Human Rivals or Dream Cattlers. Separately capture the energy proof:

```bash
./nightfall.sh energy-smoke
tail -n 200 build/server.log
cat build/energy-smoke.log
```

The gameplay video proves the old game still coheres; the energy trace proves the new ecological truth substrate. Actor-facing energetic decision integration is a later gate and must use legitimate belief, never direct omniscient state.

## Explicit readiness watches

- v0.8 PR #11 is still unmerged/unarchived pending human acceptance
- the current Cattler `HUNT_SURGE` runtime still switches an event enum abruptly; the locked design requires gradual composition/decomposition and local mob-cap pressure
- post-safety life-state for badly wounded Human Rivals remains unresolved; do not patch with a magic retreat timer
- v0.9 establishes energy truth/search/accounting but does not yet let AI optimize directly against the omniscient ledger
- final visual ecological index objects, resource interaction, Ghost Static event economy and 8 km² simulation LOD remain later work

Completed versions are preserved as immutable `archive/v0.X` branches only after their acceptance gate closes. See `docs/LEDGER.md`, `docs/ARCHITECTURE.md`, and `docs/V0.9_TOPOGRAPHICAL_ENERGY.md`.
