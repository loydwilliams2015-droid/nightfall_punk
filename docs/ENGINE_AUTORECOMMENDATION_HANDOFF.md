# Engine Autorecommendation Handoff

Base gameplay candidate remains frozen at `e65e19e931794fbe2f7e7f359b982df6cc10d4de` on `build/v1.2-teammate-causality`.

This design branch adds policy only. It does not modify the frozen human-video candidate.

Current interlude trajectory: **v1.2b — Camera Rebuild / Camera Contract** before broader engine integration.

Detailed resolved decisions: `docs/AUTORECOMMENDATION_DECISIONS_Q140_Q146.md`.

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

- **Q140 — KEEP E + C:** responsive character-controller foundation + Embodied Authority Envelope for dynamic-object force coupling.
- **Q141 — KEEP E + C:** standard sleep/tick/LOD machinery + Priority-Hotspot Simulation Gradient.
- **Q142 — KEEP E + C:** physical eligibility windows + Intent–Affordance Capture Gradient for `snap_locomotion`.
- **Q143 — KEEP 5 with 3 as guideline:** Relational Perceptual Authority Camera.
- **Q144 — KEEP E + C:** bounded presentation vocabulary driven by a Perceptual Impulse Gradient.
- **Q145 — KEEP E + C:** explicit Epistemic Observer Contract; player/live and observer/live are distinct states.
- **Q146 — KEEP E + C:** traversal-specific body alignment inside an Orientation Authority Envelope; body may align, camera aim remains player-owned.

Do not re-ask Q140–Q146 unless implementation evidence creates a material contradiction.

### AUTO KEEP — camera

- **Relational Perceptual Authority Camera** is canonical.
- Primary yaw/pitch look remains immediate/high-authority.
- Camera translation derives from predicted/authoritative body reality and is presentation-filtered only.
- Traversal states bound presentation response rather than authoring body/camera motion.
- `snap_locomotion` changes legitimate body realization first; camera presents resulting motion second.
- First-person is canonical gameplay/demo view.
- Static oblique/top views are explicit debug observer modes, not player presentation.
- Observer visibility does not become Player or teammate knowledge.
- Ordinary correction smoothing is distinct from authoritative relocation.
- Camera presentation cannot modify authoritative simulation truth.
- Closed/invalid route utility displays semantically as `BLOCKED`/`INVALID`, never as a leaked `-FLT_MAX` sentinel.

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
- base FOV and any tiny contextual FOV modulation;
- moving-platform stabilization thresholds;
- camera-effect Priority Hotspot budget;
- exact Orientation Authority Envelope angles by traversal type.

## v1.2b likely implementation envelope

Unless contradicted by code/test evidence:

1. extract inline FPS camera into a small `nf_camera` module;
2. preserve current direct mouse yaw/pitch behavior;
3. derive desired eye anchor from predicted player body;
4. add bounded translational presentation state;
5. make first-person the v1.2 causal demo default;
6. preserve fixed oblique overview as explicit diagnostic observer mode;
7. optionally add a cheap top/debug mode;
8. expose small camera diagnostics: mode, desired/presented anchor delta, FOV, active impulse;
9. preserve all v1.2 causality tests;
10. fix invalid-route HUD sentinel presentation;
11. add camera-specific deterministic tests before human proof;
12. establish bounded camera-impact and orientation-envelope hooks without overbuilding unused effects.

Do not automatically add third-person gameplay, cinematic camera graphs, universal head-bob, physical-head simulation, aim magnetism, or camera-authored traversal.

## QUESTION SUBROUTINE — current camera frontier

Q140–Q146 are resolved. The next genuinely material camera choices are:

1. **Q147 — first-person camera/geometry boundary:** how the camera should respond when a legitimate body/eye anchor approaches or intersects nearby geometry during crouch, mantle, moving-platform motion, network correction, or unusual physical contact.
2. **Q148 — authoritative-relocation presentation:** how STASIS return, respawn-like repositioning, or other explicit non-locomotor relocation should be perceptually presented without implying that the body traversed the intervening space.
3. **Q149 — gait-scale camera motion:** whether ordinary walking/running should transmit no procedural motion, a small relation-derived micro-motion, or a more explicit authored head-bob/sway model.

These should use Thesis / Antithesis / Original / Industry / Synthesis positions and should be decided only at the level necessary for v1.2b.

## Deferred broader engine frontier

Do not reopen these during camera work unless v1.2b materially consumes them:

- degree/scope of destructibility;
- active-ragdoll scope;
- fluid fidelity beyond current sparse/local-state approach;
- broader multi-actor force exchange;
- later parity/difference among Player Crew, Human Rivals, and Dream Cattlers under the new body/physics contracts.
