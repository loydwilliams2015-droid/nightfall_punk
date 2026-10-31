# VPC-01 — Visual–Physical Convergence (proposed benchmark)

Status: **design specification, not implemented/accepted game behavior**.
Authority: companion to 1.8C.2 RET / RET-HK; it neither bypasses nor
relaxes the A6/B2/B3 atomic publication, recovery, fairness, or safety gates.

## Visual north star (concept art, not an engine capture)

Postcyberpunk biopunk adventure: bright/reclaimed ruins and natural diversity
contrasted with industrial danger, colorful environmental readability, and
biological/technological enemies whose forms are **not required** to have
television heads. Maintain atmospheric tension without constant grime.
Three compatible encounter types, one world and one actor authority:

- Exploration: vertical ruins, traversal, support, contacts, water, ecological cues.
- Arena combat: fast responsive movement, hits, impulses, readable enemy silhouettes.
- Cooperative survival: fair multi-actor scheduling, collision, support, group tactics.

The preferred primary gameplay camera is close third-person over-the-shoulder,
with responsive first-person as a supported presentation mode. They must share
one authoritative simulation, combat outcomes, and actor movement semantics.

## Candidate acceptance contract (not test results)

| Gate | Evidence required | Release status |
|---|---|---|
| VPC-G1 identity | Independent reviews of silhouette/scene legibility in daylight and darkness | OPEN |
| VPC-G2 physical fidelity | Paired graybox/visual tests: contacts, support, water where interactive, material consequences | OPEN |
| VPC-G3 camera | Wall avoidance, aim alignment and FOV at both perspectives; no camera-derived authority | OPEN |
| VPC-G4 gameplay | Blind playtest comparison of traversal, combat and team encounters | OPEN |
| VPC-G5 budget | 60 Hz simulation (fixed 16.67 ms tick) and independent render/network frame-time measurements | OPEN |
| VPC-G6 source exactness | Canonical/randomized/pathological/boundary/held-out reproducible seeds and independent review | OPEN |

For camera: stage experiments with 0.8–1.5 m nominal follow distance,
subject to empirical tests, and explicitly measure crosshair-to-hit divergence,
obstacle penetration, target occlusion, input latency, and camera jitter.

For physics: only interactive affordances require authoritative simulation;
visual detail may remain presentation-only. Renderer does not own world truth.

## RET-C dependency

Before promoting a live visual build: restore a committed full A6/B2/B3 joint
frame after process exit; verify both domains and all reader projections,
replay rejection, fault cutpoints, thread safety, and two consecutive real A6
ticks from one accepted authoritative snapshot. This is **not** established by
the image references nor by the bounded H41 restart fixture.

## Reusability and evidence

Design camera/interaction presentation as C modules not bound to one specific
game aesthetic. Document source SHA, seed, acceptance matrices, benchmark
hardware, incumbent/challenger differences, held-out results, and holds.
Do not describe visually generated mockups as actual engine screenshots.
