# Engine Autorecommendation Handoff

Base gameplay candidate remains frozen at `e65e19e931794fbe2f7e7f359b982df6cc10d4de` on `build/v1.2-teammate-causality`.

This design branch adds policy only. It does not modify the frozen human-video candidate.

Current interlude trajectory: **v1.2b — Camera Rebuild / Camera Contract** before broader engine integration.

Detailed decisions:

- `docs/AUTORECOMMENDATION_DECISIONS_Q140_Q146.md`
- `docs/V1_2B_100_DECISION_SWEEP_Q147_Q246.md`

## Automatic recommendation status

### AUTO KEEP — engine-wide

- Relational Action Selection above steering.
- Semantic Steering distinct from pathfinding and physics.
- `snap_locomotion` is canonical; retired `Fuzzy Rail` terminology should not return.
- `snap_locomotion` is a regime inside Embodied Arcade Locomotion, not the whole locomotion layer.
- High-authority arcade intention must still pass legitimate affordance, capability, energy/contamination/momentum, collision, and material truth.
- Existing navigation/physics machinery remains primary; relational layers extend rather than duplicate it.
- Material impossibility breaks commitment.
- World truth does not imply actor knowledge.
- Sparse/event-driven material simulation is preferred over universal continuous simulation.
- Persistent consequences and compressed expectations are preferred over decorative micro-history.
- Negative-control testing is mandatory for non-omniscience claims.
- Standard geometric machinery first; novelty budget goes to relational interpretation, semantic affordances, energy, memory, Crew negotiation, and consequence.

### RESOLVED BY AUTORECOMMENDATION

Q140–Q146 are resolved and should not be re-asked without contradictory implementation evidence.

The v1.2b 100-question sweep processed Q147–Q246. All but three choices collapsed into AUTO KEEP, AUTO KEEP + MEASURE, or deliberate DEFER. This is now the project default: do not re-litigate resolved camera details merely because implementation begins.

Key resolved camera laws:

- **Relational Perceptual Authority Camera** is canonical.
- Aim belongs to player look intent; body position belongs to embodied reality; presentation mediates between them.
- Camera remains downstream of simulation, prediction, `snap_locomotion`, physics, damage, and actor evidence.
- Camera cannot modify authoritative simulation truth.
- Body motion happens first; camera presents what actually occurred.
- Traversal states bound presentation response; they do not author body/camera motion.
- Small prediction corrections may be presentation-smoothed; authoritative relocation remains an explicit discontinuity.
- Near-geometry handling may protect presentation but cannot manufacture a new viewpoint.
- Ordinary gait micro-motion, impact impulses, recoil, landing response, moving-platform stabilization, and traversal presentation are bounded/measureable effects rather than new movement authorities.
- Observer tools change developer perception without changing actor knowledge.
- Accessibility may reduce camera motion/shake without reducing gameplay/material consequence.
- Motion blur, DOF, chromatic aberration, lock-on, aim magnetism, photo/replay, leaning, cinematic graphs, and networked debug observer state remain deferred/absent for v1.2b.

### AUTO KEEP + MEASURE

Engine-wide:

- exact Embodied Authority Envelope thresholds;
- exact material-field tick/sleep bands;
- bounded snap-capture windows and acquire/release thresholds;
- path-corridor/local-steering compatibility;
- shared abstract steering vocabulary with body-specific realization;
- selective rigid-body/contact physics on materially consequential objects;
- object/surface/volume local state plus sparse fields for material systems;
- local reroute before wider reroute;
- Priority Hotspots as both attention and computation regulators.

Camera:

- exact translation smoothing constants;
- crouch/step/vault/mantle presentation envelopes;
- physical/recoil/landing camera impulse thresholds;
- small prediction-correction presentation smoothing;
- sprint/traversal FOV modulation amplitudes, if any;
- moving-platform stabilization thresholds;
- camera-effect Priority Hotspot budget;
- exact Orientation Authority Envelope angles by traversal type;
- gait micro-motion amplitude;
- near-geometry tolerance/near-plane values;
- mouse/pitch/sensitivity tuning.

## v1.2b minimum-sufficient implementation envelope

Unless contradicted by code/test evidence:

1. extract inline FPS camera into a small `nf_camera` module;
2. preserve current direct mouse yaw/pitch behavior;
3. derive desired eye anchor from predicted player body;
4. add bounded translational Perceptual Authority Envelope;
5. add local Perceptual Boundary Envelope for near-geometry presentation;
6. make first-person the causal-demo default;
7. preserve fixed oblique overview as explicit diagnostic observer mode;
8. optionally add cheap debug-top mode;
9. expose small diagnostics: mode, desired/presented anchor delta, FOV, active impulse, correction class, traversal relation;
10. preserve all v1.2 causality/regression tests;
11. replace invalid Route-B `-FLT_MAX` HUD output with `BLOCKED`/`INVALID`;
12. add deterministic camera tests before human proof;
13. establish bounded camera-impact/orientation-envelope hooks without overbuilding unused effects;
14. keep camera work bounded/local and avoid full-world scans;
15. persist only user-facing camera settings, not transient runtime presentation state.

## QUESTION SUBROUTINE — only three open questions from the 100-question sweep

The only Q147–Q246 choices still requiring explicit design choice are:

1. **Q151 — FOV convention:** current raylib `fovy=82` is vertical FOV, while FPS players often reason in horizontal FOV. Decide the canonical project convention before tuning.
2. **Q217 — weapon/viewmodel anchoring architecture:** camera-space viewmodel, world/body-space first-person weapon, or hybrid representation.
3. **Q224 — first-person body representation:** no visible body, lower-body proxy, or unified full-body first-person representation.

Everything else in Q147–Q246 is resolved or deliberately deferred.

## Deferred broader engine frontier

Do not reopen these during camera work unless v1.2b materially consumes them:

- degree/scope of destructibility;
- active-ragdoll scope;
- fluid fidelity beyond current sparse/local-state approach;
- broader multi-actor force exchange;
- later parity/difference among Player Crew, Human Rivals, and Dream Cattlers under the new body/physics contracts.
