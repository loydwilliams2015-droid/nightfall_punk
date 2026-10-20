# nightfall!punk — Original Pac-Man/v1.5, 1.8A.6 and corrected 1.8B.6
PRAXIS / SIGNAL / STEWARD. 2026-10-20. **Release HOLD**.

## Verdict (read before interpreting performance)
- The historical Pac-Man item is **our original v1.5 relational/metabolic text demo** dated 2026-08-15, 10 ticks/3 ghosts. The archive explicitly states it **is not original Namco Pac-Man source code**. No runnable historical ghost source was located in the reviewed repository commit trees. The archived text is preserved byte-for-byte in `docs/archive/nightfall_punk_v15_pacman_metabolic_demo.txt`, with machine-readable transcription `docs/ret/pacman_original_v15_trace_reconstruction.json`.
- A separate *newly reconstructed*, deliberately simplified **classic-like** controller uses global/current Pac-Man tile information and CHASE when not powered, FLEE when powered. It is not a verified exact Pac-Man arcade ghost algorithm and is not represented as inherited source code.
- Actual **1.8A.6** source was checked out at `e0d6aa1334df92c7f8e4425cac96a9f1d4bb4243`, compiled, and run. Actual **corrected 1.8B.6** source was compiled from commit `1ebd10049b7f555d9ee037690f6cd090255c98ec`, using the **same authored C probe** `src/tests/pacman_three_era_native_probe.c`. The original Pac-Man ghost AI is not thereby revived inside the A6/B6 engine; the real C comparison tests single-actor authoritative movement and contact mechanics on the Pac-Man-themed 3×3 target schedule.

## Executed data and independent source identification
GitHub Actions run **[39482894035](https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/39482894035)** (source commit `1ebd10049b7f555d9ee037690f6cd090255c98ec`), **SUCCESS**, exact archived A6 worktree and live B6 source built by CMake. Reproducible raw per-scenario output files and machine-readable report are stored in [workflow artifact `pacman-three-era-h1-scientific-data`](https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/39482894035/artifacts/12292266542).

| Domain | Original text experiment | Classic-like reconstruction | 1.8A.6 actual native H1 | Corrected 1.8B.6 actual native H1 |
|---|---:|---:|---:|---:|
| Gameplay decisions | 30 | 30 | Not a ghost-AI test | Not a ghost-AI test |
| CHASE | 6 | 21 | Not measured | Not measured |
| GUARD | 24 | 0 | Not measured | Not measured |
| FLEE | Not a separately recorded action | 9 | Not measured | Not measured |
| Player path | B,E,H,G,G,H,I,I,H,E | Same 10-tick path | Same path, repeated 3× | Same path, repeated 3× |
| Initial ghost/metabolic model | Three agents, actor-relative evidence, health and energy | Global target, fixed toy rule | One authoritative actor, one crate, external scripted goal | Same actor, crate and script; B2 checked arming not requested by this parity probe |
| C physics scenarios | N/A | N/A | 160 (5 strata × 32 seeds) | 160 paired |
| A6 committed native ticks | N/A | N/A | **3,923** | **3,923** |
| Real A5 contact observations | N/A | N/A | **200** | **200** |
| Pending termination events | N/A | N/A | **59** | **59** |
| Rejected termination events | N/A | N/A | **0** | **0** |
| Exact per-seed status/hash/history/position parity | N/A | N/A | Reference side | **160/160 identical** |
| Native CTest | N/A | N/A | **7/7 PASS** | **11/11 PASS** |

Native physical experiments cover 160 starts of up to 30 frames = **4,800 potential frame slots per version**. The authoritative world actually committed 3,923; 59 scenarios terminated when a PENDING state was encountered and all remaining frames after those stops were intentionally **not attempted**. It is incorrect to report `4,800 - 3,923` as simulation failures. The 59 PENDING endings are preserved as correct fail-closed outcomes under material/geometry uncertainty, not coerced into successful movement.

The 160 paired physics scenarios reproduce the identical A6 authoritative substrate in this no-B3-request probe. Native A6/B6 contact histories, world hashes, final positions and tick decisions matched **160/160**, so the observed physics-regression difference on this selected corpus was **0/160**. Zero observed differences is NOT a proof that differences never occur in new workloads or compilers.

## Significant comparisons (scientifically scoped)
1. **Epistemic behavior:** classic-like omniscient chase/flee = 21 chase + 9 flee; original nightfall v1.5 evidence-bounded relational trace = 6 chase + 24 guard. This is a meaningful *policy difference*, but is not a common-policy win-rate comparison; neither ghost AI quality nor balanced play has been measured. Original v1.5 had **80% GUARD** and was explicitly marked guard-heavy tuning.
2. **Metabolic accounting:** in the original v1.5 trace B and C each had viability **0.81** at T6 but energy **29.0** versus **69.4**. This demonstrates the toy's distinction between functional viability and available future resources. A6/B6 native physics runs did **not** reproduce a three-ghost metabolic decision benchmark, so no improvement over the v1.5 model is claimed.
3. **Physical truth is preserved across native builds:** 1.8A.6 and corrected 1.8B.6 gave equal results across 160 paired test situations, including the 200 contact observations and 59 fail-closed pending transitions. The later B6 physical core was intentionally retained rather than rewritten for a speculative speed improvement.
4. **B6 contracts add a *separate* verified capability:** B6's new B2-checked A6/B3 push path, from prior real-source CI `39480283991`, had 17 B2-authorized committed ticks and 5 physical contacts corresponding to 5 B3 applied receipts, plus denial/forgery/revocation safety checks. **This source-locked B2 test is separate from the Pac-Man paired native probe**, so do not count it as an AI ghost win, collision count increase, or speed gain.
5. **Performance / playability:** the common probe is a small single-actor headless laboratory. No hardware-comparable p50/p95/p99 latencies, network/client ticks, three-ghost AI pathfinding, 3D FPS level, or user-visible game feel have been measured. No correctness-admissible performance winner is selected.

## How to reproduce
Workflow file `.github/workflows/v18b6-pacman-three-era.yml` uses `git worktree add --detach` on source-pinned A6, builds both versions under `NF_CONTACT_LAB_ONLY=ON`, compiles `src/tests/pacman_three_era_native_probe.c` against each actual version's `libnightfall_embody_prod.a`, and runs:
```sh
python3 tools/pacman_three_era_report.py --a6 /tmp/pacman_a6.txt --b6 /tmp/pacman_b6.txt --out /tmp/pacman_three_era_result.json
```
The script verifies archive trace consistency, valid scenario coverage and numeric integrity, labels classic as reconstruction, and enumerates any mismatched native scenario rather than concealing divergence. Workflow outputs an artifact with all 160 per-version source test records.

## RET classification / governance
- **Archival original**: v1.5 text trace 24 GUARD / 6 CHASE and health-versus-energy difference. Not freshly executed ghost code.
- **Newly executed**: A6 and corrected B6 native H1 160-scenario parity, CTests 7/7 and 11/11, exact pinned inputs and GitHub logs/artifacts. Classic-like controller calculation in Python is newly reconstructed and checked, not original arcade code.
- **A priori**: an apparently valid chase command is not evidence of physical clearance or authority; a pending physical solve must not fabricate success.
- **Highly predictable, unverified**: B2 contractual checking should improve semantic authority where integrated, but it has not been validated for three-ghost Pac-Man behavior.
- **Unresolved**: true three-actor concurrent A6 world (still rejected at H1), controlled AI-policy comparisons using same observation model, independent AI/pathfinding oracle, durable joint A6/B3/B5 crash recovery, authenticated multiplayer, representative 60Hz latency and consumer playtesting, original B1 H1 source and software rights. All source/licensing/merge/release holds unchanged.

**Disposition:** User lock on PR #61 preserved. This comparison is **evidence published on a distinct draft research PR**. No change to production code, original ledger or original archived results. No claim of complete 1.8B.6 physics or improved ghost AI.
