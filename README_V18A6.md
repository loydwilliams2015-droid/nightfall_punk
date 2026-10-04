# nightfall!punk 1.8A.6 — Embodiment integration, H1 conditional exit

This final **1.8A laboratory sweep** links the A.3 verified capsule shape, A.4 reciprocal motor-driven contact, and A.5 canonical material/condensed history to *existing* `NfWorld`, `NfActor`, `NfCollider`, and `NfCameraState` data. It does not call `nf_world_step` while the bridge owns its actor tick; two independent movement controllers must never advance the same actor in one tick.

## Reproduce

```bash
bash ./v18a6.sh all
```

Requirements: C11 compiler, CMake ≥3.20, Python 3, POSIX filesystem for the existing WAL/sink tests; this **offline** lab does not require ENet/raylib. `v18a6.sh all` runs historical 1.8A.1–A.5 regressions, the new strict CMake/CTest suite, four-policy × six-scenario × 200-seed matched samples, PAR analyzer, and byte-for-byte repeat. For an independent second compiler, run CMake into a *different* build directory with `-DCMAKE_C_COMPILER=clang`; do not reuse a GCC-configured CMake cache.

## Selected conditional laboratory model

`NF18A6_WORLD_CAMERA` = opt-in authoritative world projection + actual dynamic crate impulse + canonical material conservative query + contact-history commit + existing read-only camera follow. `NF18A6_WORLD_BRIDGE` is its paired physics-only control. `NF18A6_LAB_ONLY` deliberately cannot advance the real actor and is a **negative embodiment control**. `NF18A6_LEGACY_REFERENCE` remains the historical movement-only baseline and is a **negative canonical authority control**. No control policy is quietly identified as the production default.

A selected result is valid *only for grounded horizontal movement in a configured finite canonical corridor*. The code explicitly rejects jumping, crouching, ladders/vault/mantle, ramps, moving platforms and nearby additional actors rather than inventing support or silently ignoring contacts. No normal game-loop scheduler, AI action, server-authoritative multiplayer, integrated ammunition/weapon hit logic, raylib rendered client or human user-feel result is claimed. Missing canonical chunks return `PENDING`. `nf18a5_load_canonical()` can supply an authoritative parent explicitly, but dynamic chunk prefetch/streaming is not implemented. Camera presentation never commits world truth.

## Scientific status

- **Logical hard core:** actual physical clearance and traversal authority are necessary; only actually applied contact impulses may enter authoritative condensed history; a camera and a predicted client are not authoritative; unavailable canonical geometry is never called free.
- **H1 evidence:** strict-C fixture success and native headless `NfWorld`/`NfActor`/`NfCameraState` component integration; same-seed paired physical snapshot equality; local WAL recovery; generated PAR and CSV.
- **Not yet H2 across real gameplay:** sample outcomes cover hand-authored scene strata and seeded parametric variants, not a representative distribution. The model 2/3 comparison is a controlled paired study within that scope; hold-out parameter seeds are not unseen mechanics.
- **Not yet H3 or H4:** full 3D world client and network CI, entire shared runtime scheduler and AI, native P95/P99 load latency, full-world durable WAL, actual multiplayer, human play tests are required.

## Branch policy and code hygiene

All A.1–A.5 model decisions and their evidence remain historically intact. This patch adds source, explicit CMake registration, tests, reproducible model data, PAR, a dedicated GitHub CI workflow, and a review handoff without force-pushing or silently merging into `main`. Three preexisting 1.7D/E source files gained a terminal newline solely for Clang `-Werror` compliance. The `.gitignore` in this branch ignores generated `build/`, local artifacts and binaries, but never tests or sample-generating scripts.

See `docs/LEDGER_V1.8A6_EMBODIMENT_SWEEP.md`, `docs/RESEARCH_LINEAGE_V1.8A.md`, `docs/SCIENTIFIC_ACCOUNTABILITY_PROTOCOL.md`, and `evidence/v18a6/PAR_RESULTS.md`.

## Resumed 1.8A.6 single-owner tick addendum

For the optional headless world tick, bind `nf18a6_bind_world_tick(world,runtime,loader,ctx,journal)` then use the **ordinary** `nf_world_set_input()` + `nf_world_step_checked()` / `nf_world_step()` entrypoints. A bound `NfWorld` dispatches to one embodiment authority; `PENDING` never falls back to the historical locomotion controller. `nf18a6_unbind_world_tick` restores the original path. This mode rejects a second active actor to avoid silently skipping any actor in its still-limited H1 scheduler. Direct calls to `nf18a6_step` while the owner is bound are rejected unless routed through the active world tick.

The new Red (actor proposal) / Blue (material response) / Purple (temporary reciprocal same-tick SCC) classification reuses `nf17c_build_purple_envelopes` and labels actual applied actor-object impulses Purple only after commit. One-way, cross-target and disjoint-channel dependencies decompose rather than becoming Purple. See `docs/LEDGER_V1.8A6_TICK_AUTHORITY.md` and `src/tests/test_v18a6_tick_owner.c`. The complete combat, contamination, energy, multi-actor AI and network scheduling contract is **still open**; this is an architectural solution for opt-in ownership, not proof of finished multiplayer world ticks.
