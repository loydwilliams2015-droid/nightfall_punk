# nightfall!punk v1.7D — Handoff to v1.7E

## FROM
v1.7C Calibration, Coupling, and Causal Attribution.

## TO
v1.7D Cleave: supported incumbent paths are polished; speculative paths are isolated; read-only observability contracts prepare v1.7E.

## 1.7D locked interfaces
- Nf17dCleavePolicy: evidence maturity + disposition for major subsystems.
- Nf17dConflictSet: local connected arbitration groups.
- Nf17dCacheStamp: versioned dependencies for derived state.
- Nf17dBudgetPool: guaranteed floors + borrowable surplus + emergency reserve.
- Nf17dReasonTraceRecord: explicit trace-field presence and consequential completeness.
- Nf17dViewPreset/Nf17dObservabilityFrame: read-only presentation contract.

## Commands
- `./v17d.sh standard-check`
- `./v17d.sh build`
- `./v17d.sh test`
- `./v17d.sh regression`

## CMake/CTest
For a logic-only composition build:
`cmake -S . -B build/cmake-v17d -DNF_BUILD_CLIENT=OFF -DNF_BUILD_SERVER=OFF -DNF_BUILD_NETBOT=OFF -DNF_BUILD_TESTS=ON`
`cmake --build build/cmake-v17d --parallel 2`
`ctest --test-dir build/cmake-v17d --output-on-failure`

## v1.7E graphical boundary
The graphical build reads v1.7D observability frames and authoritative/actor diagnostic data. It may add rendering, camera, selection, temporal replay, overlay batching, and benchmark instrumentation. It may not create an alternative gameplay state path.

Initial thin presets:
- PLAY: no debug overlays.
- WORLD: cells, nexuses, boundaries, material.
- ACTOR: evidence, belief, confidence, contracts.
- CAUSAL: transactions, conflict sets, Purple, frontier, refinement, reason trace.
- FULL: union for stress/debug.

## Empirical questions reserved for v1.7E
- overlay generation/batching cost;
- retained-history memory window;
- CPU vs GPU debug-geometry generation;
- nexus aggregation level for distant overlays;
- benchmark thresholds for diagnostic/full views.
