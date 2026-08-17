# nightfall!punk v1.6 — Weapon Solution / Focus ledger

Date: 2026-08-16  
Branch: `build/v1.6-weapon-solution-focus`  
Basis: latest v1.5 branch head `89f80564f4e7f3a1ee2652fc69d130e8d935cb53` (v1.5 code/proof head plus canonical PAR terminology ledger)  
Status: **CI-GREEN IMPLEMENTATION CANDIDATE — HUMAN PROOF REQUIRED**

## Accepted design resolutions

- Q196: **C + D synthesis** — a very small truthful physical baseline cone, with most additional dispersion produced by embodied state.
- Q197: **E** — context-sensitive Focus; benefit grows when instability exists and shrinks when already stable.
- Q198: **REC** — transition + information/initiative opportunity cost; no arbitrary heavy movement tax.
- Q199: **E** — layered embodied recoil: little/no long-horizon camera theft, strong weapon/reticle/solution response.
- Q200: **E** — nightfall reticle synthesis: center = intention, sparse brackets = embodied uncertainty.

## FROM — v1.5

v1.5 proved the integrated causal spine for combat, actor-local evidence/belief, nested tactical appraisal, claims, metabolic restitution, communication receipt presentation, teammate/Rival/Cattler population, and human-visible first-person combat.

Human proof exposed one important mismatch in the provisional weapon presentation:

`RETICLE / RMB FOCUS PRESENTATION != SHOT SOLUTION`

The v1.5 proof demo visually expanded/contracted spread and narrowed FOV under RMB, but its accepted hitscan direction still used the camera yaw/pitch center ray directly. The player therefore could not reliably distinguish marksmanship failure from a Focus implementation gap.

## TO — v1.6

Canonical Weapon Solution spine:

`PLAYER AIM / FIRE / FOCUS INTENT -> FIRE GATE -> PURE WEAPON SOLUTION -> PHYSICAL MUZZLE -> DETERMINISTIC BOUNDED DISPERSION -> HIT/MISS/OBSTRUCTION -> CONSEQUENCE -> TRUTHFUL RETICLE/VIEWMODEL -> RECOVERY`

The weapon solution is relational:

`shot_solution = relation(aim_intent, weapon, motion, posture, contamination, recoil/recovery, Focus, geometry)`

The center aim remains player-owned. The actor–weapon–world relation determines how difficult it is to realize that intention.

## EXISTING — inherited and preserved

- fixed-tick world and dedicated-server authority architecture;
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
- deterministic shot variation from world/actor/weapon/sequence ancestry;
- state-produced dispersion with a small baseline physical cone;
- context-sensitive Focus that gives little benefit to an already-stable shot and stronger benefit under movement/recoil/state pressure;
- Focus-accelerated recoil recovery;
- sprint/Focus transition cost without a generic walk-speed tax;
- truthful reticle radius derived from the same actual solution cone;
- physical muzzle-origin proof so camera visibility does not authorize shooting through nearby geometry;
- v1.6 graphical calibration range and component diagnostics;
- deterministic v1.6 contract tests.

### Deliberate integration boundary

The shared solver is compiled into `nightfall_shared` and exercised by deterministic tests and the v1.6 human-proof demo. The existing production dedicated-server `process_combat_control` path still uses its inherited center-ray hitscan and has **not yet been silently replaced**. This is a staged-coherence choice: first prove the solver's feel and causal legibility; then route the accepted solver through production client/server prediction/rewind in the same v1.6 line or immediate integration follow-up. Until that routing is proven, do not claim production multiplayer Weapon Solution parity.

## STABLE COHERENCE RULES

1. One intended shared solver; no permanent separate hip-fire and ADS firing algorithms.
2. Player owns long-horizon aim; Focus never selects or snaps to a target.
3. Reticle is evidence of actual solution state, not decoration.
4. Focus does not change damage/head multiplier.
5. Sensory effects do not secretly rotate bullets; manipulator/body relations may legitimately alter handling.
6. Shot variation is deterministic and replayable, not frame-time RNG.
7. Viewmodel/FOV are presentation of gameplay state and never author hits.
8. Existing v1.5 combat/AI/ecology systems remain authoritative in their domains.
9. Production server routing must reuse the shared solver rather than reimplement its mathematics after the focused proof is accepted.

## PAR — comparative benchmark sheet

PAR is the ordinary comparative term, not an acronym and not a synonym for defer.

Current v1.6 benchmark dimensions:

- **Quake / ioquake3:** immediate weapon/spatial readability and high-authority mouse intention. Target: `MEETS PAR` on immediacy without copying Quake's exact weapon mathematics.
- **Halo design practice:** weapon-role legibility and sensitivity to small tuning changes. Target: `MEETS PAR` on readable role/tuning discipline through nightfall's relational solver.
- **Titanfall:** movement-combat coupling and preservation of initiative. Target: `DIFFERENT-BUT-COMPARABLY-VALID`; Focus must not collapse locomotion into stop-and-ADS play.
- **Prey (2017):** material/systemic consequence. Target: `DIFFERENT-BUT-COMPARABLY-VALID`; contamination/body state legitimately enters handling without adding a detached RPG accuracy stat.
- **F.E.A.R.:** readable immediate combat causality. Target: `MEETS PAR` on retrospectively intelligible hit/miss/recovery relations.

CI establishes implementation coherence and regressions only. Human-facing PAR results for responsiveness, recoil feel, Focus tradeoff, reticle readability, and muzzle feasibility remain **INSUFFICIENT EVIDENCE** until the native recording is reviewed.

## LATER / NOT IN THIS FOCUSED PROOF

- Communication Causality / Independent Appraisal (`WATCH -> receipt -> independent accept/modify/reject/defer -> embodied action`) is the next social-AI layer after the Weapon Solution seam is accepted;
- production client/server routing, prediction/reconciliation and rewind of the new Focus/solution state remains an explicit v1.6 integration seam, not silently complete;
- full six-slot ping wheel;
- voice/subtitle corpus;
- generalized three-carrier economy and diegetic F6/F7 replacement;
- finished weapon art;
- optional accessibility aim assistance, if ever added, remains explicit and separate from baseline Weapon Solution;
- larger arsenal breadth;
- affect-surge implementation beyond its current interface/concept.

## ACCEPTANCE CONTRACT

Automated — GitHub Actions run `31982557834` at code head `e5baffefd23f3d7d45c4d34bbcc73c85a2c0b423`:

- [x] strict `-Wall -Wextra -Wpedantic -Werror` full graphical compile;
- [x] all inherited plus v1.6 CTests green;
- [x] deterministic same-state/same-sequence shot result;
- [x] different accepted sequences may vary within bounded cone;
- [x] movement/air/contamination increase truthful state dispersion;
- [x] crouch modestly improves movement stability;
- [x] Focus benefit is context-sensitive;
- [x] Focus recovery beats equivalent hip recovery after a shot;
- [x] sprint caps Focus transition until sprint state is released;
- [x] reticle radius orders with actual cone state;
- [x] inherited Human Rival, spatial, Cattler, energy, lifeworld, contamination and camera smokes green.

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

The v1.5 human proof triggered a targeted longitudinal correction rather than a broad rewrite. v1.6 now has a CI-green focused Weapon Solution proof while preserving the accepted v1.5 causal programme. Human evidence is the next gate. Production multiplayer routing follows the accepted shared solver rather than preceding feel validation. A full historical sweep remains unnecessary unless the recording or server-routing pass exposes material drift.
