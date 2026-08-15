# nightfall!punk — v1.4 Human Acceptance Handoff

Date: 2026-08-14
Branch: `build/v1.4-high-medium-integration`
Status: **AUTOMATED GREEN — DRAFT HUMAN-VIDEO GATE**

## Identity

v1.4 is the high/medium-degree systems-integration build. Communications + fuller combat integration are v1.5. v1.3 has no compiled gameplay build.

FROM: v1.2c exact green head `27af894fb9148be11447b8e26320bf18307645d6`.

## Thesis

> **One material revision can change affordances, evidence and situated utility while coordination remains bounded and embodied.**

Causal proof:

`OBJECT REVISION -> AFFORDANCE -> LEGITIMATE EVIDENCE -> UTILITY -> TEMPORAL CLAIM -> CONTROL -> BODY -> NEW RELATION`.

## Automated validation

Validated implementation head:

- head: `a07179a0293f93dca06481da1abd2941b3ddcdef`
- workflow: `nightfall v1.4 CI`
- run: **#416** / `31857243391`
- result: **PASS**

Run #416 passed standard-check, strict graphical compile, all inherited + v1.4 CTests, Human Rival combat/encounter/spatial regressions, Dream Cattler ecology, topographical energy, Lifeworld, contamination/STASIS, camera, v1.2c integrated population/embodied action, and the named v1.4 object/claim/exposure proof.

This documentation stamp creates a newer branch head; the final frozen human-video candidate is accepted only after that exact documentation head also passes the same v1.4 CI workflow. Draft PR identity is recorded externally after that exact-head check.

Keep the PR draft until the Pop!_OS video is reviewed.

## Run locally

```bash
cd ~/Downloads/nightfall_punk_v12
git config remote.origin.fetch '+refs/heads/*:refs/remotes/origin/*'
git fetch origin
git switch build/v1.4-high-medium-integration || git switch --track origin/build/v1.4-high-medium-integration
git pull --ff-only
chmod +x v14.sh
./v14.sh standard-check
./v14.sh build
./v14.sh test
./v14.sh demo
```

## Controls

- F1 Player View
- F2 Observer / Live
- F3 Debug Top / Live
- F4 **PROOF SETUP**
- F5 full reset
- mouse/WASD/Shift/Ctrl/Space
- hold E at purple gate

F4 is laboratory-only. It resets the bounded slice and places Player + teammate where the positive proof is easy to observe. It is not a gameplay teleport mechanic.

## Recording sequence

1. Confirm the title reads `nightfall!punk v1.4 — HIGH/MEDIUM SYSTEMS INTEGRATION`.
2. Keep the diagnostic HUD visible.
3. Give a short 360-degree first-person survey and confirm Player/TEAM/RIVAL/CATTLER population.
4. Press **F4**.
5. Confirm gate `rev=1`, `CLOSED`, teammate `EVIDENCE NONE`, `BELIEF UNCHANGED`.
6. Hold **E**.
7. Capture `TEMPORAL CLAIM interaction=GRANTED` and gate transition to `rev=2 OPEN`.
8. Keep recording as teammate obtains legitimate `DIRECT` evidence.
9. Capture belief/gradient/route utility/commitment response and physical teammate movement/displacement.
10. Observe the `TACTICAL GRADIENT` exposure/option-utility diagnostics while rotating/moving around the slice.
11. Optional: F2 -> F3 -> F1 once.

## PASS indicators

- integrated population survives the version transition;
- camera remains coherent;
- gate revision changes only with actual gate state change;
- no route belief before legitimate evidence;
- temporal interaction claim grants/releases without deadlock;
- opening the gate produces direct evidence and causal teammate response;
- tactical exposure remains bounded 0..1 and varies with geometry;
- no regression in Rival/Cattler movement.

## Report even if it fails

Send the video even if the gate, claim, teammate, camera, Rival or Cattler behavior misbehaves. Visible failure is causal evidence and should produce KEEP/AMEND/DEFER rather than silent patching.

## Explicit v1.5 boundary

Do not treat the absence of a player viewmodel/full playable weapon loop as a v1.4 failure. The next frontier is communications + combat integration, including bounded AI reporting and player-visible weapon/viewmodel work.
