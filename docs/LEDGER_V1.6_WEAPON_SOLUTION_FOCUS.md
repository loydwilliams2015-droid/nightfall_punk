# nightfall!punk v1.6 — Weapon Solution / Focus ledger

Date: 2026-08-16  
Branch: `build/v1.6-weapon-solution-focus`  
Basis: v1.5 frozen head `57e12611da1a4bbe629912ff0989c94330b15bd5`  
Status: IMPLEMENTATION CANDIDATE — CI + HUMAN PROOF REQUIRED

## Accepted design resolutions

- Q196: **C + D synthesis** — a very small truthful physical baseline cone, with most additional dispersion produced by embodied state.
- Q197: **E** — context-sensitive Focus; benefit grows when instability exists and shrinks when already stable.
- Q198: **REC** — transition + information/initiative opportunity cost; no arbitrary heavy movement tax.
- Q199: **E** — layered embodied recoil: little/no long-horizon camera theft, strong weapon/reticle/solution response.
- Q200: **E** — nightfall reticle synthesis: center = intention, sparse brackets = embodied uncertainty.

## FROM — v1.5

v1.5 proved the integrated causal spine for combat, actor-local evidence/belief, nested tactical appraisal, claims, metabolic restitution, communication receipt presentation, teammate/Rival/Cattler population, and human-visible first-person combat.

Human proof exposed one important mismatch in the provisional weapon presentation:

`RETICLE / RMB FOCUS PRESENTATION != AUTHORITATIVE SHOT SOLUTION`

The v1.5 demo visually expanded/contracted spread and narrowed FOV under RMB, but accepted hitscan direction still used the camera yaw/pitch center ray directly. The player therefore could not reliably distinguish marksmanship failure from a Focus implementation gap.

## TO — v1.6

Canonical Weapon Solution spine:

`PLAYER AIM / FIRE / FOCUS INTENT -> FIRE GATE -> PURE WEAPON SOLUTION -> PHYSICAL MUZZLE -> DETERMINISTIC BOUNDED DISPERSION -> HIT/MISS/OBSTRUCTION -> CONSEQUENCE -> TRUTHFUL RETICLE/VIEWMODEL -> RECOVERY`

The weapon solution is relational:

`shot_solution = relation(aim_intent, weapon, motion, posture, contamination, recoil/recovery, Focus, geometry)`

The center aim remains player-owned. The actor–weapon–world relation determines how difficult it is to realize that intention.

## EXISTING — inherited and preserved

- fixed-tick world and server-authority architecture;
- Snap Locomotion / movement / collision / traversal;
- Relational Perceptual Authority camera;
- authoritative ammunition, cadence, reload, switching, contamination, STASIS and revival;
- Information Topography, actor-local beliefs and reports;
- nested tactical candidates, commitments and temporal claims;
- teammate embodied causality;
- Human Rival and Dream Cattler ecology;
- metabolic source/sink/restitution proof;
- WATCH + Crew `SEEN` presentation semantics;
- v1.2/v1.4/v1.5 tests and graphical targets.

## ADVANCING — v1.6 implementation

- `nf_weapon_solution` pure shared-C module;
- stable weapon-specific baseline/motion/air/contamination/recoil/recovery tuning;
- deterministic accepted-shot variation from world/actor/weapon/sequence ancestry;
- state-produced dispersion with a small baseline physical cone;
- context-sensitive Focus that gives little benefit to an already-stable shot and stronger benefit under movement/recoil/state pressure;
- Focus-accelerated recoil recovery;
- sprint/Focus transition cost without a generic walk-speed tax;
- truthful reticle radius derived from the same actual solution cone;
- physical muzzle-origin shot proof so camera visibility does not authorize shooting through nearby geometry;
- v1.6 graphical calibration range and component diagnostics;
- deterministic v1.6 contract tests.

## STABLE COHERENCE RULES

1. One shared solver; no separate hip-fire and ADS firing algorithms.
2. Player owns long-horizon aim; Focus never selects or snaps to a target.
3. Reticle is evidence of actual solution state, not decoration.
4. Focus does not change damage/head multiplier.
5. Sensory effects do not secretly rotate bullets; manipulator/body relations may legitimately alter handling.
6. Accepted shot variation is deterministic and replayable, not frame-time RNG.
7. Viewmodel/FOV are presentation of gameplay state and never author hits.
8. Existing v1.5 combat/AI/ecology systems remain authoritative in their domains.

## PAR / LATER

- Communication Causality / Independent Appraisal (`WATCH -> receipt -> independent accept/modify/reject/defer -> embodied action`) is now the next social-AI layer after v1.6 rather than being mixed into this correction build.
- full six-slot ping wheel;
- voice/subtitle corpus;
- generalized three-carrier economy and diegetic F6/F7 replacement;
- full production replication/prediction of Focus runtime state;
- finished weapon art;
- optional accessibility aim assistance, if ever added, remains explicit and separate from baseline Weapon Solution;
- larger arsenal breadth;
- affect-surge implementation beyond its current interface/concept.

## ACCEPTANCE CONTRACT

Automated:

- [ ] strict `-Wall -Wextra -Wpedantic -Werror` full graphical compile;
- [ ] all inherited CTests remain green;
- [ ] deterministic same-state/same-sequence shot result;
- [ ] different accepted sequences may vary within bounded cone;
- [ ] movement/air/contamination increase truthful state dispersion;
- [ ] crouch modestly improves movement stability;
- [ ] Focus benefit is context-sensitive;
- [ ] Focus recovery beats equivalent hip recovery after a shot;
- [ ] sprint caps Focus transition until sprint state is released;
- [ ] reticle radius orders with actual cone state.

Human:

- [ ] stationary hip-fire remains competent;
- [ ] movement broadens brackets and actual shot behavior coherently;
- [ ] RMB while moving visibly improves stabilization without target magnetism;
- [ ] continuing RMB after stopping produces rapid recapture;
- [ ] recoil feels embodied but does not fight long-horizon mouse aim;
- [ ] reticle center/brackets are understandable as intention/uncertainty;
- [ ] near-wall physical muzzle behavior prevents camera-through-wall shots;
- [ ] no inherited v1.5 regression is visible.

## H3 / discrepancy disposition

The v1.5 human proof triggered a targeted longitudinal correction rather than a broad rewrite. This v1.6 branch resolves the identified Weapon Solution mismatch while preserving the accepted v1.5 causal programme. A full historical sweep is unnecessary before this bounded correction receives CI and human evidence.
