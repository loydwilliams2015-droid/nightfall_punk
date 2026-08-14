# Engine Autorecommendation Handoff

Base gameplay candidate remains frozen at `e65e19e931794fbe2f7e7f359b982df6cc10d4de` on `build/v1.2-teammate-causality`.

This design branch adds policy only. It does not modify the frozen human-video candidate.

Current interlude trajectory: **v1.2b — Camera Rebuild / Camera Contract** before broader engine integration.

## Automatic recommendation status

### AUTO KEEP — engine-wide

- Relational Action Selection above steering.
- Semantic Steering distinct from pathfinding and physics.
- `snap_locomotion` as canonical replacement for retired Fuzzy Rail terminology.
- `snap_locomotion` is a regime inside Embodied Arcade Locomotion, not the whole locomotion layer.
- High-authority arcade intention must still pass legitimate affordance, capability, energy/contamination/momentum, collision, and material truth.
- Existing navigation/physics machinery remains primary; relational layers extend rather than duplicate it.
- Material impossibility breaks commitment.
- World truth does not imply actor knowledge.
- Sparse/event-driven material simulation preferred over universal continuous simulation.
- Persistent consequences and compressed expectations preferred over decorative micro-history.
- Negative-control testing is mandatory for non-omniscience claims.
- Standard geometric machinery first; novelty budget goes to relational interpretation, semantic affordances, energy, memory, Crew negotiation, and consequence.

### AUTO KEEP — camera

- **Relational Perceptual Authority Camera** is canonical.
- Q143 disposition: **Option 5 implementation architecture, with Option 3 Perceptual Authority Envelope as the governing guideline.**
- Primary yaw/pitch look remains immediate/high-authority.
- Camera translation derives from predicted/authoritative body reality and is only presentation-filtered.
- Traversal states bound presentation response rather than authoring body/camera motion.
- `snap_locomotion` changes legitimate body realization first; camera presents resulting motion second.
- First-person is canonical gameplay/demo view.
- Static oblique/top views are explicit debug observer modes, not player presentation.
- Observer visibility does not become Player or teammate knowledge.
- Ordinary correction smoothing is distinct from authoritative relocation.
- Camera presentation cannot modify authoritative simulation truth.
- Closed/invalid route utility should display semantically as `BLOCKED`/`INVALID`, never leak `-FLT_MAX` to the HUD.

### AUTO KEEP + MEASURE

Engine-wide:

- bounded snap-capture windows;
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
- camera-effect Priority Hotspot budget.

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
11. add camera-specific deterministic tests before human proof.

Do not automatically add third-person gameplay, cinematic camera graphs, universal head-bob, physical-head simulation, aim magnetism, or camera-authored traversal.

## QUESTION SUBROUTINE — current camera frontier

Q143 is resolved and should not be re-asked unless contradictory evidence appears.

The next mid/high-uncertainty camera choices are:

1. **Q144 — physical-impact transmission:** how much genuine body/material impulse should be communicated through first-person camera presentation without reducing aim authority or comfort;
2. **Q145 — observer transition/control:** how debug observer modes should be entered, exited, and frozen relative to live simulation so diagnostics remain useful without creating accidental gameplay/epistemic ambiguity;
3. **Q146 — camera/body orientation coupling during traversal:** how much body yaw realignment `snap_locomotion` may perform under an independently controlled first-person view, especially for mantles, ladders, narrow passages, and strong affordance capture.

These should use the five-position Thesis / Antithesis / Original / Industry / Synthesis format where useful.

## Deferred but still unresolved engine frontier

The previous broader questions remain valid but are not required to begin the bounded v1.2b camera interlude:

- humanoid-to-dynamic-object force coupling;
- continuous versus hotspot-activated material-field simulation;
- global snap-capture strength/permissiveness and failure behavior.

Return to them when the next build materially consumes those choices rather than reopening them during camera work.
