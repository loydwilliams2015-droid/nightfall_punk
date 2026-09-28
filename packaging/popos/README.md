# nightfall!punk v1.7E — Pop!_OS observer package

This is the primitive graphical observability build, not a production-ready game presentation.

Run:
  ./run-v17e.sh

Controls:
  F1 PLAY
  F2 WORLD
  F3 ACTOR
  F4 CAUSAL
  F5 FULL
  Arrow keys pan
  Mouse wheel zoom
  Left click selects a diagnostic cell/object

Benchmark:
  ./bin/nightfall_v17e_observer --benchmark --frames 360 --csv v17e_benchmark.csv

If the prebuilt executable is incompatible with your Pop!_OS release, use:
  ./build-on-popos.sh

The local rebuild script installs the normal C/raylib development dependencies and builds the observer from the included source snapshot.

Graphical contract:
- observation is read-only;
- higher-priority dimensions suppress lower-priority presentation, not information;
- equal-priority dimensions remain compound/rainbow;
- overload degrades detail before causal identity or authoritative correctness.
