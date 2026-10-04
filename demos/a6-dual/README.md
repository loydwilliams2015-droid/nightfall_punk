# nightfall!punk 1.8A — Dual Pac-Man-Style Counterpoint Games

**Two original 2D Pac-Man-style demonstrations, not forks of GZDoom or LibreQuake.**

- **Cinder Circuit** (`./play-cinder.sh`): Doom-like oppressive industrial corridors, collect signals, acquire key K and open door D, confront active pursuers and conservation choices.
- **Slipgate Circuit** (`./play-slipgate.sh`): Quake-like intersecting corridors, authorization switch S, constrained H traversal, and a pushable B crate with actual 1.8A.4 pairwise impulse calculations.

The renderer is native X11 (compatible with Xwayland). Both games also run with `--text` (headless replay) and `--batch N` (CSV simulation); the latter modes require no windowing library.

## Pop!_OS / Ubuntu (Linux x86_64)

Unpack the release archive, then run either game:

```bash
cd nightfall-a6-dual-demo
./play-cinder.sh
./play-slipgate.sh
```

The archive contains both compiled x86_64 ELF binaries. To rebuild, install `build-essential cmake libx11-dev` and run `./build.sh`; to use Clang, `CC=clang ./build.sh`. If X11 development files are unavailable, build.sh produces headless binaries. Xwayland is required if the desktop only exposes native Wayland; games have been smoke-tested under Xvfb X11.

Controls: **WASD/arrows** move, **Space** local stun pulse at a cost to player energy, **M** toggle enemy-AI policy (starts a new comparable game), **N** new seed, **Q/Escape** quit. Difficulty and speed are research settings rather than polished release balance.

## Run science tests

```bash
./bin/nightfall-cinder --selftest
for s in crate ladder pending door; do ./bin/nightfall-slipgate --scenario "$s"; done
python3 scripts/bench.py --seeds 128 --start 31001 --steps 300
```

This emits `results/paired_1_8a6_arcade.csv`, `results/PAR.json` and `results/PAR.md`.

```bash
./play-cinder.sh --text --player goal --seed 42 --steps 150
./play-slipgate.sh --text --player random --seed 42 --steps 150
./play-cinder.sh --batch 100 --player goal --ai classic > results/baseline.csv
```

### Real module provenance

The arcades directly call existing 1.8A routines, linked from `engine/build/dual/libnightfall_embody_prod.a`: `nf18a3_capsule_sweep`, `nf18a4_motor_drive`, `nf18a4_apply_contact`, `nf18a5_load_canonical`, `nf18a5_query`, `nf18a5_history_commit`, `nf18a5_receipt_sample`, and `nf18a6_classify_tick` (built using the 1.7C reciprocal-dependency graph). Neither executable is a full `NfWorld`/camera/network-server runtime. Screen-space movement is a discrete 2D tile projection after physical admissibility checks; moving a box one tile visualizes the real impulse receipt but is **not** a general continuous rigid-body motion integrator.

The 1m chunk state is selectively cached from an authoritative static level layout. Unloaded parent chunks return PENDING, then are loaded/rechecked; the swept capsule query independently forbids overlap, including adjacent obstacles. Contact summaries are held in memory; the archival WAL from the laboratory is **not** connected in these mini-games. Passage through ladder H requires switch S; through door D requires key K. The engine's complete 1.8A3 contract structure is not instantiated in these simplified arcade rules.

### Counterpoint study, not an upstream engine benchmark

The *classic* control uses a simplified globally informed maze pursuer. The *systemic* control has partial local geometric observation, a finite last-seen memory, energy costs, recharging and retreat. Both share our geometry and motor substrate; we **did not** run GZDoom or LibreQuake executables as the baseline. The player policies are goal-seeking BFS (global static maze knowledge) or deliberately weak deterministic random input, respectively. Matched seed initializations are paired, but PRNG draws diverge after choices branch. Two fixed authored maps are **not** a sample of independent generated worlds. Higher player score against systemic ghosts can indicate ghosts are weaker, not necessarily that the gameplay is better. Human play-feel and real FPS graphics are not tested.

### Failed scenarios and limits

The one active crate has a bounded point-contact impulse and is projected to the next tile; no angular CCD, multi-body physics, rotating geometry, weapon system, multiplayer networking, or real Doom/Quake WAD/PAK loader. No claim of GZDoom or Quake protocol, map, asset or save-game compatibility. For those capabilities, separate upstream-compatible engine integration and licensing review are required.

## Third-party projects and licensing

No upstream GZDoom/LibreQuake/Quake code, artwork, soundtrack, maps, WAD, PAK or Doom proprietary data are included. Names identify inspiration and proposed future integration research only. GZDoom's source uses GPLv3 and the LibreQuake project supplies freely licensed Quake-compatible data, typically paired with another engine. LibreQuake resources have component-specific notices; do not assume every upstream asset is freely relicensable or already embedded here. The local `engine/` snapshot belongs to the supplied nightfall!punk source lineage; refer to that project's upstream repository for licensing before redistribution or combining external code.

See `results/PAR.md` for observed results, `results/PAR.json` for reproducible numeric summaries, and `results/paired_1_8a6_arcade.csv` for complete rows. The screenshots are actual X11 frame captures from the executables.
