# nightfall!punk v1.6A — Weapon Correspondence / Calibration handoff

Branch: `build/v1.6a-weapon-correspondence-calibration`  
Basis: v1.6 head `580c87ce58637f15b0241a3d1660958c3155f727`  
Acceptance: automated proof first, then native human video. **Do not merge on CI alone.**

## Pull / build / run

```bash
git fetch origin
git switch build/v1.6a-weapon-correspondence-calibration
git pull --ff-only
chmod +x v16a.sh
./v16a.sh standard-check
./v16a.sh build
./v16a.sh test
./v16a.sh demo
```

If GitHub Actions is green on the exact checked-out head, local `./v16a.sh test` is optional. The graphical demo is still required for human correspondence/feel acceptance.

## What changed from v1.6

v1.6A fixes the proof seam rather than replacing Weapon Solution:

```text
v1.6
reticle -> LAST SHOT solution

v1.6A
reticle -> CURRENT envelope
fire accepted -> deterministic realized shot -> LAST SHOT history
```

The shared C law now exposes:

- `nf_weapon_evaluate_envelope(...)` — current sequence-free capability;
- `nf_weapon_realize_shot(...)` — accepted deterministic shot;
- `nf_weapon_solve(...)` — retained v1.6 compatibility wrapper using those same two stages.

The proof demo also adds:

- raw RMB diagnostics;
- `F` developer Focus control through the same runtime as a negative control;
- window-focus state and RMB transition count;
- Focus target vs Focus amount;
- NOW vs LAST SHOT diagnostics;
- ~7° full-Focus FOV prior;
- ~200–250 ms acquisition and faster release prior;
- mild nonlinear FOV-aware live brackets;
- small camera recoil acknowledgement;
- subtle amber blocked-muzzle cue;
- accepted blocked firearm shot -> ammo/recoil/sound -> nearby obstruction impact.

## Controls

- WASD — movement
- Shift — sprint
- Ctrl — crouch
- Space — jump
- LMB — fire
- RMB — Focus
- F — developer Focus control / negative-control input
- R — reload
- 1 / 2 — carbine / pistol
- F5 — reset

## Recording checklist

One continuous **90–150 second** recording is sufficient if the HUD remains readable.

1. **Stationary / no fire** — hold still long enough to read NOW carbine cone.
2. **Move then stop without firing** — live brackets must expand while moving and contract after stopping. This is the direct present-tense regression test.
3. **Sky RMB hold** — aim at a simple sky/background and hold RMB about 3 seconds. `RMB=DOWN` should remain visible and Focus should approach 1.0.
4. **F control** — release RMB and hold `F` about 3 seconds. It must drive the same Focus runtime. This is diagnostic only.
5. **Foreground Focus** — aim near foreground geometry and hold RMB so the ~7° FOV/viewmodel change is visually easier to compare.
6. **Move -> stop while Focused** — move with RMB held, then stop while still held, then release. Watch current cone/reticle recapture.
7. **Focused shot** — fire at least one accepted round while Focus is clearly nonzero. LAST SHOT must preserve that Focus/reduction.
8. **Temporal switch proof** — fire the pistol, switch to carbine without firing. NOW should become carbine immediately while LAST SHOT remains pistol history.
9. **Burst recovery** — short sustained carbine burst, then release fire. Recoil/current envelope should rise then recover; camera should not retain stolen aim.
10. **Blocked muzzle** — move close to solid geometry until `blocked=YES`, then fire. The round should be consumed, recoil/impact should occur nearby, and no target should be hit through the obstruction.
11. **Clear control** — back away until `blocked=NO`, fire again, and confirm ordinary behavior.

## What I need from you

Upload the recording. **No written report is required.** If something looks wrong, a timestamp is helpful for any of:

- RMB unexpectedly switching UP during a continuous physical hold;
- F and RMB producing different Focus behavior;
- NOW brackets staying stale while moving/stopping or switching weapons;
- Focus feeling almost always optimal or almost never useful;
- calm 0.12° behavior still feeling like arbitrary misses despite truthful brackets;
- camera recoil fighting mouse input or leaving persistent aim displacement;
- blocked cue appearing when the muzzle is clearly free;
- target damage through a blocked muzzle path;
- blocked shot failing to consume a round / create nearby impact;
- crash or inherited feature regression.

## What you do **not** need to do

- No marksmanship score is required to prove Focus/input correspondence.
- No server or multiplayer proof is required for v1.6A; production solver/prediction integration is explicitly postponed to v1.6B.
- No extra written questionnaire is required before the video unless the demo itself exposes a new architecture contradiction.

## v1.6B handoff already reserved

v1.6B begins with:

1. production client/server Focus + shared Weapon Solution integration;
2. prediction/reconciliation and bounded rewind parity;
3. then B -> C PAR stress: Quake-like immediate aim authority followed by Titanfall-like movement-combat initiative;
4. advanced Focus mastery: combine Focus timing with Snap Locomotion / Jet Set Radio-like smoothed affordance guidance inside the open systemic physical world, rather than turning Focus into mandatory ADS.
