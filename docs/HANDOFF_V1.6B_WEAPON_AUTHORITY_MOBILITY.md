# nightfall!punk v1.6B — Weapon Authority / Mobility Handoff

## Branch / basis

- Branch: `build/v1.6b-weapon-authority-mobility`
- Basis: v1.6A exact head `23a791a23c132ba6dadd9e6f1ad54f709e666d2b`
- Do not merge on CI alone. Native human evidence remains required.

## Get / build / test

```bash
git fetch origin
git switch build/v1.6b-weapon-authority-mobility
git pull --ff-only
chmod +x v16b.sh
./v16b.sh standard-check
./v16b.sh build
./v16b.sh test
./v16b.sh sample-db
./v16b.sh net-smoke
./v16b.sh demo
```

`sample-db` writes:

- `build/v16b-samples/samples.csv`
- `build/v16b-samples/report.md`
- `build/v16b-samples/soil.csv`

These are automated simulation evidence. They do not replace native human feel/readability evidence.

## Demo controls

- WASD move
- mouse look
- Shift sprint
- Ctrl crouch
- Space jump
- LMB fire
- RMB Focus
- F developer Focus negative-control input
- R reload
- 1 / 2 weapon select

## Native recording checklist

One continuous **90–150 second** recording is enough. Normal screen recording is fine; no written report is required.

1. **Stationary NOW baseline, no fire** — current cone/Focus should remain live and stable.
2. **Explicit sustained RMB hold ~3 seconds** — this time keep RMB physically held, not repeated clicking. Focus should acquire smoothly and remain settled; release once and confirm clean decay.
3. **Repeated RMB taps control** — short presses should produce corresponding partial acquisitions without a stuck state.
4. **Move → stop without firing** — live CURRENT brackets/cone must change before any shot history changes.
5. **Fast redirect without Focus, then with timed Focus** — redirect component should rise from embodied motion; Focus should organize part of it without moving the reticle center.
6. **Sprint → Focus → release → continue** — Focus must not hard-brake earned momentum; sprint-period Focus remains bounded.
7. **Reload + RMB Focus** — Focus remains legal but should acquire visibly slower than ordinary ~200–250 ms acquisition; release remains fast.
8. **Vault or mantle + fire** — firearm remains legal, but handling difficulty should be materially higher and the shot still originates from the physical muzzle.
9. **Ladder fire if comfortably reachable in the lab** — legal but difficult; no target magnetism or arbitrary fire lock.
10. **Weapon switch after recoil / Focus** — Focus remains actor-level; one weapon's recoil history must not leak into the other.
11. **Burst then hands-off recovery** — CURRENT cone should recover; historical LAST SHOT remains history; camera aim should not be possessed by correction/recoil.
12. **Near-wall blocked accepted shot** — approach until the muzzle is physically constrained, fire, and show ammo/shot ancestry/recoil plus a nearby realized impact; a target behind the wall must not receive through-wall damage. Then back away for a clear negative control.

Helpful but optional: if anything looks strange, say the approximate timestamp after uploading the video.

## What does NOT need human proof in this recording

- packet serialization internals;
- duplicate-seed sample determinism;
- automated network latency matrix;
- MCI / Movement–Combat Initiative (reserved for Pac-Man testing);
- autonomous AI Focus strategy;
- new traversal verbs.

## Automated sample/video commands

Headless database:

```bash
./v16b.sh sample-db
```

Graphical scripted samples after a full build:

```bash
./build/full/nightfall_v16b_demo --auto focus_hold --seconds 5
./build/full/nightfall_v16b_demo --auto redirect_focus --seconds 5
./build/full/nightfall_v16b_demo --auto reload_focus --seconds 5
./build/full/nightfall_v16b_demo --auto blocked --seconds 5
```

For frame capture:

```bash
mkdir -p build/v16b-video/focus
./build/full/nightfall_v16b_demo --auto focus_hold --seconds 5 --frames-dir build/v16b-video/focus
```

CI may run the same scenarios under a virtual display and encode selected frame sets into MP4 artifacts. The simulation/script is the source evidence; the video is its visualization.

## Expected human-visible causal grammar

```text
INPUT
  ↓
newly integrated mobility
  ↓
PREDICTED / CURRENT Weapon Authority
  ↓
accepted trigger
  ↓
accepted-shot ancestry
  ↓
deterministic solved direction
  ↓
physical muzzle + current material world
  ↓
impact / hit / miss
  ↓
LAST SHOT / authoritative result
```

The center remains intention. Movement affects realizability through embodied state, not aim assistance.
