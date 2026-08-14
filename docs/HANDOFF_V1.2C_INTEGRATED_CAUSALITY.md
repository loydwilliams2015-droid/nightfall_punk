# v1.2c Integrated Population / Embodied Causality — Handoff

Branch: `build/v1.2c-integrated-causality`
Base: v1.2b camera candidate `edf645ab7e434679d78be88708bddbe9789419c4`
Status: human-video gate; do not merge before review.

## Purpose

v1.2c keeps the successful v1.2b first-person camera contract but repairs the human proof surface so existing game systems remain present together.

The required causal claim is now:

`Evidence -> Belief -> Utility -> Commitment -> Control -> Body Displacement -> Changed World Relation`.

A cognitive decision without measurable embodied action is not accepted as action.

## Demo population

The bounded graphical slice should instantiate:

- 1 Player — cool white;
- 1 Player Crew teammate — chrome blue;
- 2 Human Rivals — red;
- 2 Dream Cattlers — orange with yellow weak-point bands.

Dream Cattlers are not required to wander without cause. Their automated movement proof uses legitimate prey evidence so ecology is preserved rather than replaced with animation-for-proof.

## Controls

- F1 — PLAYER VIEW
- F2 — OBSERVER / LIVE
- F3 — DEBUG TOP / LIVE
- F5 — reset
- Mouse — look
- WASD — move
- Shift — sprint
- Ctrl — crouch
- Space — jump
- E — ALTER_ROUTE when the purple gate affordance is legitimate

## HUD proof fields

Watch these fields:

- `POP PLAYER / TEAMMATE / RIVAL / CATTLER`
- moving counts for Rival/Cattler
- `ALTER_ROUTE`
- gate distance
- `E HELD/UP`
- `AFFORDANCE READY/NOT_READY/CONSUMED`
- teammate `CONTROL`
- teammate `SPEED`
- teammate `DISPLACEMENT`
- `EVIDENCE`
- `BELIEF`
- `GRADIENT`
- `COMMIT`
- Route A/B energy and utility.

## Preferred recording sequence

1. Start in F1 and spend several seconds looking around at Player scale.
2. Identify the chrome-blue teammate and allow enough time to see whether its displacement grows.
3. Locate at least one red Human Rival.
4. Locate at least one orange/yellow Dream Cattler; use F2/F3 briefly if useful for proof.
5. Return to F1.
6. Approach the purple ALTER_ROUTE gate until the HUD says `AFFORDANCE READY`.
7. Hold E and show `E HELD` followed by `ALTER_ROUTE OPEN` / `AFFORDANCE CONSUMED`.
8. Keep the teammate causal HUD visible long enough to see evidence/belief/gradient/commitment change.
9. Observe the teammate's physical trajectory after the commitment change.
10. If convenient, show F2 and F3 once more and return to F1.

## Automated commands

```bash
chmod +x v12c.sh
./v12c.sh standard-check
./v12c.sh build
./v12c.sh test
./v12c.sh regression
./v12c.sh demo
```

Expected integration smoke includes:

```text
[v1.2c] POPULATION PLAYER=1 TEAMMATE=1 RIVAL=2 CATTLER=2
[v1.2c] TEAMMATE CONTROL=... DISPLACEMENT=... | RIVAL=... | CATTLER CONTROL=... DISPLACEMENT=...
nightfall v1.2c teammate causality / integrated embodied action tests: PASS
```

The camera smoke must still include:

```text
LOOK=IMMEDIATE BODY=TRUTH PRESENTATION=BOUNDED RELOCATION=DISCONTINUITY
```

## Acceptance boundary

Do not accept merely because the actors are rendered. For the teammate, distinguish:

`decision -> control -> body displacement`.

For Cattlers, distinguish legitimate lurking from broken locomotion. Their ecology permits stationarity; movement is only mandatory when a real evidence/action condition calls for it.

Do not broaden this correction into a cinematic-camera pass, full viewmodel rebuild, arbitrary destruction, or a new AI planner before this integrated human proof is reviewed.
