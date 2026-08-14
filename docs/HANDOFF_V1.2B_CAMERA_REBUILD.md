# v1.2b Camera Rebuild — Handoff

Branch: `build/v1.2b-camera-rebuild`
Base: v1.2 candidate `e65e19e931794fbe2f7e7f359b982df6cc10d4de`
Purpose: repair the causal-demo camera failure while converting camera presentation into a bounded engine contract without destabilizing v1.2 causality.

## Human-video candidate intent

Start in **F1 PLAYER VIEW**. The first-person camera is the canonical gameplay proof. F2 and F3 are explicitly diagnostic views only.

Record a short sequence that proves:

- immediate mouse look;
- player-scale navigation through the 80x80 m slice;
- crouch/jump/ordinary movement remain comfortable enough for this stage;
- player reaches and opens the purple ALTER_ROUTE gate;
- teammate evidence/belief/gradient/commitment changes remain legible in the HUD;
- teammate independently reroutes when legitimate evidence exists;
- blocked Route B displays `BLOCKED`, not a floating-point sentinel;
- optional: F2 observer/live and F3 debug-top can inspect the experiment, then F1 returns to player view.

## Controls

- F1: PLAYER VIEW
- F2: OBSERVER / LIVE
- F3: DEBUG TOP / LIVE
- F5: reset causal experiment
- Mouse: look in Player View
- WASD: move
- Shift: sprint
- Ctrl: crouch
- Space: jump
- E: ALTER_ROUTE interaction near the purple gate

## Automated gates

Run:

```bash
chmod +x v12b.sh
./v12b.sh standard-check
./v12b.sh build
./v12b.sh test
./v12b.sh regression
./v12b.sh demo
```

Expected camera smoke includes:

`camera relational perceptual authority tests: PASS`

and:

`LOOK=IMMEDIATE BODY=TRUTH PRESENTATION=BOUNDED RELOCATION=DISCONTINUITY`.

## Do not broaden before video proof

Do not add cinematic camera takeover, third-person gameplay, full physical-head simulation, large procedural head bob, aim magnetism, lock-on, large FOV pulses, motion blur, DOF, chromatic aberration, or full first-person body/viewmodel reconstruction merely to polish this interlude.

The point of v1.2b is to restore a valid first-person proof surface and establish the smallest camera contract that future movement/physics work can safely consume.
