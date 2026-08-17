# nightfall!punk v1.6 — Weapon Solution / Focus handoff

Branch: `build/v1.6-weapon-solution-focus`  
Basis: latest v1.5 branch head `89f80564f4e7f3a1ee2652fc69d130e8d935cb53`  
Build evidence: GitHub Actions run `31982557834` — **SUCCESS** at code head `e5baffefd23f3d7d45c4d34bbcc73c85a2c0b423`  
Acceptance: automated gate green; native human video remains required.

## Pull / build / run

```bash
git switch build/v1.6-weapon-solution-focus
git pull --ff-only
chmod +x v16.sh
./v16.sh standard-check
./v16.sh build
./v16.sh test
./v16.sh demo
```

GitHub CI has already passed the strict graphical compile, inherited/v1.6 CTests, Human Rival encounter/spatial smokes, Dream Cattler ecology, energy, lifeworld, contamination, camera regressions and the named v1.6 Weapon Solution smoke. Local `./v16.sh test` is therefore optional if the checked-out head contains no newer code changes, but running it is still useful if convenient. The graphical demo is required for feel acceptance.

## What the v1.6 demo proves

This is a focused calibration range, not a replacement for the v1.5 integrated population demo. v1.5 remains in the same build and its inherited tests/target remain available.

v1.6 specifically tests:

`AIM INTENT -> RELATIONAL WEAPON SOLUTION -> PHYSICAL MUZZLE -> BOUNDED DISPERSION -> CONSEQUENCE -> TRUTHFUL RETICLE -> RECOVERY`

The center reticle is player intention. Sparse brackets report the actual current solution cone.

Important boundary: the shared solver is compiled and proven in the focused v1.6 path, but production dedicated-server combat still uses the inherited center-ray path. Do not treat this recording as proof of final multiplayer prediction/rewind integration. If the human proof accepts the solver, the next v1.6 integration step is to route production client/server firearm resolution through this shared solver rather than duplicate its mathematics.

## Controls

- WASD: move
- Shift: sprint
- Ctrl: crouch
- Space: jump
- LMB: fire
- RMB: Focus
- R: reload
- 1 / 2: carbine / pistol
- F5: reset range

## Recording checklist

One continuous **60–120 second** run is enough if all items are visible.

1. **Stationary hip-fire** — fire several controlled shots at near/mid/far silhouettes. Confirm it feels competent rather than deliberately inaccurate.
2. **Moving hip-fire** — strafe while firing. Keep the solution diagnostics visible long enough to show motion contribution and wider brackets.
3. **Moving Focus** — hold RMB while moving for about five seconds. Focus should narrow FOV modestly and reduce/recapture instability without choosing a target.
4. **Stop while still holding RMB** — remain on RMB for about three seconds after stopping. The brackets/weapon should rapidly settle toward the player's intended vector.
5. **Short sustained burst** — show recoil-instability rising and recovering. The viewmodel should carry most expressive movement; the camera should remain under player control.
6. **Crouch and jump samples** — briefly demonstrate that crouch modestly improves stability and air state worsens it without lottery-like behavior.
7. **Physical muzzle / nearby geometry** — move near a solid block and try an angle where the camera has visibility but the weapon/body is close to obstruction. Watch for impossible camera-through-wall shots.
8. **Diagnostics** — capture at least one readable frame containing total cone, base/state decomposition, Focus fraction/reduction and intended -> solved direction.

## What to report back

Upload the video. **No written report is required.** If practical, mention timestamps for any of:

- a shot that appears impossible or outside the displayed uncertainty;
- reticle brackets that do not match shot behavior;
- Focus that feels useless or always mandatory;
- RMB target magnetism or camera possession;
- severe sprint/Focus lockout;
- recoil that steals long-horizon aim;
- camera-through-wall shooting;
- crash, disappearing target or regression.

## Acceptance caution

Do not merge merely because CI is green. The v1.6 contract explicitly separates deterministic correctness from FPS feel. Human video is the acceptance evidence for aim ownership, Focus tradeoff, recoil presentation and reticle readability.

## Next integration/design layers

1. If the recording passes: route production client/server shot resolution through `nf_weapon_solution`, including Focus input, prediction/reconciliation and bounded rewind state, then regression-test the same laws in multiplayer.
2. After the Weapon Solution seam is fully accepted: advance **Communication Causality / Independent Appraisal** — player WATCH request -> teammate receipt -> independent ACCEPT / MODIFY / REJECT / DEFER -> commitment -> embodied action.
