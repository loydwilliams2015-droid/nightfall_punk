# nightfall!punk v1.7A — Graphical AI-player Review Handoff

## Run order
1. `bash ./v17a.sh sample-db 512`
2. `python3 tools/select_v17a_graphical_seeds.py build/v17a-samples/samples.csv build/v17a-review/selections.csv`
3. `./build/v17a/nightfall_v17a_graphical_review build/v17a-review/selections.csv build/v17a-review/review.csv`
4. `bash ./v17a.sh viewer-build`
5. Under X11/Xvfb: `./build/v17a-viewer/nightfall_v17a_viewer --batch <absolute selections.csv> <absolute frames-dir>`
6. `python3 tools/analyze_v17a_graphical_review.py review.csv report.md soil.csv par.md classifications.csv frames-dir`

## Invariants
- Referee overlay never feeds the player/perceptual controller.
- Instrumented mode remains bounded to locally observed cells rather than a universal tactical best-cell oracle.
- Perceptual mode uses quantized player-legible cues and memory.
- All review paths must remain materially valid.
- Every selected seed/mode is duplicated and deterministic hashes must match.
- Random is the negative control; greedy is the stronger goal-directed control.

## Graphical evidence
The viewer shows instrumented and perceptual paths side by side on the same selected seed. It displays exact post-run referee fields only for diagnosis: exposure, ecological risk, affordance, contract memory, start/goal, path, and unobserved cells.

## Boundary to 1.7B
LOGIC_ONLY and ACTOR_APPRAISAL_GAP cases become primary 1.7B route-planning/game-theory inputs. PERCEPTUAL_DIFFERENCE remains a 1.7A/1.7C spatial communication and legibility concern. MATERIAL_GAP blocks the spatial substrate and must be repaired before progression.
