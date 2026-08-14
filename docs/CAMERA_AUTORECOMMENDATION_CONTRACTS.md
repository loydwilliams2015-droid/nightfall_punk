# nightfall!punk — Camera Autorecommendation Contracts

Date: 2026-08-13
Base policy branch: `design/engine-autorecommendation-contracts`
Frozen gameplay candidate remains `e65e19e931794fbe2f7e7f359b982df6cc10d4de` on `build/v1.2-teammate-causality`.
Scope: camera architecture/design policy only. This does not modify the frozen v1.2 human-video candidate.

## Trigger / observed failure

The v1.2 causal greybox used a fixed oblique camera for diagnostic overview. The human screenshot demonstrated that this observer camera is unsuitable as the canonical player camera: it miniaturizes the 80x80 m causal slice and prevents evaluation of FPS movement, snap locomotion, embodied traversal, and player-scale combat topography.

The failure is architectural rather than a failure of the existing production first-person camera mathematics. The production client already derives first-person eye position from the predicted player body and derives view direction from immediate yaw/pitch input. v1.2 temporarily substituted a static observer camera in the standalone causal harness.

Therefore v1.2b is a justified **Camera Rebuild / Camera Contract** interlude.

## Canonical camera thesis — AUTO KEEP

Adopt **Relational Perceptual Authority Camera**:

- **Option 5 is the implementation architecture.**
- **Option 3, Perceptual Authority Envelope, is the governing relational guideline.**

Canonical law:

> Aim belongs to player perceptual intent; camera position acknowledges embodied reality; presentation mediates between them.

Equivalent pipeline:

`PlayerLookIntent -> ImmediateYaw/Pitch`

and independently:

`Authoritative/PredictedBodyMotion -> RelationalMotionAnalysis -> BoundedPresentationEnvelope -> Traversal/State Bounds -> CameraAnchor`

then:

`ImmediateViewOrientation + PresentedBodyAnchor -> FinalFirstPersonCamera`.

The camera is presentation, not physics truth and not a second locomotion authority.

## Relationship to movement stack — AUTO KEEP

Canonical control stack remains:

`RelationalActionSelection -> SemanticSteering -> snap_locomotion -> EmbodiedArcadeLocomotion -> Collider/Material/PhysicsTruth -> WorldTrace`.

Camera is downstream of embodied realization:

`EmbodiedMotion/PhysicsTruth -> CameraPresentation`.

Never:

`CameraCurve -> BodyMotion`.

Never:

`snap_locomotion -> DirectCameraTeleport`.

Instead:

`snap_locomotion -> LegitimateBodyRealization -> PhysicsResolution -> Camera observes/presents resulting motion`.

This guarantees that an interrupted vault, mantle, impact, platform movement, or failed affordance is presented according to what physically occurred rather than according to a canned camera animation that assumes success.

Canonical maxim:

> Camera curves do not determine motion; they bound the presentation of motion that actually occurred.

## Player look authority — AUTO KEEP

Primary FPS yaw/pitch remains high-authority and near-immediate.

Automatic defaults:

- mouse/right-stick look is not spring-filtered by default;
- ordinary aim does not inherit camera lag from body translation smoothing;
- `snap_locomotion` may align the body to a legitimate traversal relation but may not unexpectedly rotate the player's aim toward an affordance;
- pitch/yaw clamps remain explicit and independently testable;
- movement orientation may consume player yaw where the existing movement contract requires it, but camera presentation does not silently rewrite movement intent;
- recoil/impact rotation, if present, is a bounded additive presentation impulse rather than replacement of player orientation authority.

## Camera translation — AUTO KEEP

Use body truth as the camera's positional source, with a bounded **Perceptual Authority Envelope**.

Conceptual response:

`PresentationResponse = F(BodyDelta, Velocity, Acceleration, Duration, Momentum, TraversalRelation, PhysicalConsequence)`.

Do not turn this expression into an oversized continuous dynamics solver unless measurement demonstrates need. Use a small number of explicit variables and traversal/state bounds.

Implementation architecture:

1. derive desired eye anchor from predicted/authoritative player body;
2. classify the currently meaningful locomotion/physical relation;
3. derive a small relational presentation response from actual body motion;
4. bound that response with state-specific limits;
5. build the final raylib camera.

Examples:

- ordinary walk/run: nearly direct body following;
- crouch: bounded eye-height interpolation;
- small step: suppress harsh high-frequency vertical pop while preserving real displacement;
- vault/mantle: present the actual upward/forward body trajectory inside bounded lag/offset limits;
- landing: bounded translational/rotational impulse proportional to actual consequence;
- moving platform: preserve stable relative framing without detaching camera from body truth;
- physical shove: body displacement remains real; camera communicates it through the same bounded presentation envelope;
- network micro-correction: hide or soften only the presentation component that does not need to be perceived;
- major authoritative relocation/STASIS return: explicit discontinuity, not ordinary locomotion smoothing.

## Traversal-specific bounds — AUTO KEEP

Use named traversal/movement states as **bounds**, not as camera-motion authors.

Preferred relationship:

`ActualBodyMotion -> RelationalResponse -> MovementModeBounds -> CameraPresentation`.

Not:

`MovementMode -> CannedCameraAnimation -> Camera`.

This preserves generality for unusual material/physics outcomes while retaining production clarity and easy debugging.

Likely bounded parameters per meaningful mode:

- maximum translational lag;
- maximum presentation offset;
- maximum angular additive impulse;
- entry/exit response time;
- whether discontinuity is permitted;
- whether player look remains fully independent (normally yes).

Do not build a separate camera graph for every object or traversal affordance.

## First-person versus observer camera — AUTO KEEP

Canonical gameplay/demo default is **FIRST_PERSON**.

Observer views are diagnostic tools and must be explicitly separated from player epistemology.

Likely minimal modes:

`NF_CAMERA_FIRST_PERSON`
`NF_CAMERA_DEBUG_OBSERVER`
`NF_CAMERA_DEBUG_TOP`

Rules:

- human gameplay proof starts in first-person;
- debug observer camera may inspect world relations unavailable to the player;
- observer information must never be fed into actor belief or gameplay state;
- HUD/debug labels should clearly identify `PLAYER VIEW` versus `OBSERVER VIEW` when modes differ;
- the static v1.2 oblique camera is retained only as an optional diagnostic observer, not discarded as useless machinery.

This preserves the epistemic law:

`ObserverVisibility != PlayerKnowledge != TeammateKnowledge`.

## Proposed `NfCamera` code contract — HIGH-LIKELIHOOD DEFAULT

When v1.2b implementation begins, prefer a small dedicated camera module rather than continuing inline camera construction in the client/demo.

Likely shape:

```c
typedef enum {
    NF_CAMERA_FIRST_PERSON,
    NF_CAMERA_DEBUG_OBSERVER,
    NF_CAMERA_DEBUG_TOP
} NfCameraMode;

typedef struct {
    NfCameraMode mode;
    float yaw;
    float pitch;
    float base_fov;
    float current_fov;
    NfVec3 desired_anchor;
    NfVec3 presented_anchor;
    NfVec3 presentation_offset;
    NfVec3 impulse_translation;
    NfVec3 impulse_rotation;
    float eye_height_current;
    float eye_height_target;
} NfCameraState;
```

This is a design sketch, not a mandate to add unused fields. Implement only fields with an immediate causal consumer.

Likely functional split:

- `nf_camera_init()`
- `nf_camera_apply_look_input()`
- `nf_camera_follow_body()`
- `nf_camera_present_motion()`
- `nf_camera_apply_impulse()`
- `nf_camera_build_raylib()`

Prefer pure/bounded helper functions and ordinary C structs over a generalized cinematic camera framework.

## Networking/prediction relation — AUTO KEEP

The camera follows the locally predicted body for responsiveness.

Rules:

- server remains material truth;
- prediction/reconciliation may alter local body state;
- camera presentation may soften small visual correction while the movement/prediction system retains correct simulation state;
- camera smoothing cannot change the underlying reconciled body position;
- diagnostics must distinguish ordinary presentation smoothing from authoritative relocation;
- camera should not become a hidden second prediction system.

Conceptual separation:

`SimulationCorrection != CameraPresentationCorrection`.

## FOV — AUTO KEEP + MEASURE

Current production baseline around 82 degrees remains a valid starting point, but the camera subsystem should own FOV explicitly.

Automatic defaults:

- one stable base FOV is the gameplay truth for aiming/scale perception;
- any sprint/traversal/impact FOV modulation must be small, bounded, optional/tunable, and presentation-only;
- debug observer FOV is independent of first-person FOV;
- no large cinematic FOV animation is assumed for v1.2b.

Exact baseline and modulation amplitudes remain measurement/tuning choices rather than architectural truths.

## Physical camera impulse — AUTO KEEP + MEASURE

Camera may communicate genuine embodied consequence, but physical impulses do not directly take over player look authority.

Preferred model:

`PhysicalEvent -> ConsequenceMagnitude/Direction -> BoundedPresentationImpulse`.

Potential consumers:

- landing;
- recoil;
- heavy object/body contact;
- strong contamination-related locomotor instability if later justified;
- explosions or authored set-piece forces;
- moving-platform discontinuity.

Small irrelevant contacts should produce approximately zero perceptual impulse. A material event must cross a meaningful consequence threshold before consuming camera attention.

## Camera and Priority Hotspots — HIGH-LIKELIHOOD DEFAULT

Priority Hotspots may regulate **presentation attention**, not player look direction.

Legitimate uses:

- increase diagnostic emphasis;
- permit bounded event-specific camera impulse when the player's body is actually affected;
- select which concurrent presentation effects survive a limited camera-effect budget.

Forbidden use:

`Hotspot -> ForceCameraToLookAtTarget` as an ordinary gameplay default.

If a future authored Apophis/set-piece sequence needs direct camera authorship, treat it as a separate explicitly signaled exceptional contract rather than contaminating ordinary FPS control.

## Failure behavior — AUTO KEEP

Success may snap; camera failure/presentation should flow.

Examples:

- failed mantle -> camera follows actual failed body trajectory and settles naturally;
- affordance lost before capture -> no canned camera completion;
- collider blocks movement -> camera cannot cross geometry merely to preserve presentation smoothness;
- camera anchor would penetrate geometry -> solve presentation locally without moving the authoritative body;
- large body correction -> preserve correctness first, presentation second.

## v1.2b automatic scope

Unless contradicted by implementation evidence, v1.2b should automatically include:

- extract first-person camera construction into a dedicated module;
- first-person as default causal-demo view;
- retain fixed oblique view as explicit debug observer;
- optional top/debug view if cheap;
- immediate yaw/pitch player look;
- predicted-body eye anchor;
- bounded translational presentation envelope;
- crouch/step/traversal-compatible eye-height handling;
- framework for bounded physical/recoil/landing impulses without requiring all effects immediately;
- explicit separation of ordinary correction smoothing and authoritative relocation;
- camera diagnostics exposing mode, desired/presented anchor delta, FOV, and active impulse without overwhelming the normal causal HUD;
- replace closed Route-B `-FLT_MAX` HUD sentinel with a semantic `BLOCKED`/`INVALID` presentation in the demo;
- preserve all v1.2 teammate-causality automated proofs.

Do not automatically add:

- cinematic camera graph;
- third-person gameplay camera;
- animation-driven head camera;
- universal head-bob;
- full physical head simulation;
- camera-authored traversal movement;
- broad motion-blur/post-processing stack;
- automatic target lock or aim magnetism.

## Camera testing — AUTO KEEP

Minimum camera contract tests should cover:

1. immediate yaw/pitch response remains deterministic;
2. camera anchor derives from current player body/eye state;
3. ordinary movement presentation cannot change authoritative body state;
4. `snap_locomotion` changes body realization first, camera second;
5. material obstruction cannot be bypassed by camera presentation;
6. failed traversal does not complete a canned camera trajectory;
7. small reconciliation error can be presentation-smoothed without altering simulation truth;
8. authoritative relocation is classified separately;
9. observer camera does not change actor evidence/belief;
10. first-person is default human-proof mode.

Human proof should specifically evaluate:

- aim responsiveness;
- traversal readability;
- motion comfort;
- whether body consequence is perceptible without feeling like input lag;
- whether observer/debug switching preserves epistemic clarity.

## Automatic recommendation summary

### AUTO KEEP

- Relational Perceptual Authority Camera.
- Option 5 implementation architecture with Option 3 as governing guideline.
- immediate/high-authority primary look;
- body-derived positional truth;
- bounded translational presentation;
- traversal modes bound rather than author camera motion;
- camera downstream of `snap_locomotion` and physics;
- FPS default, observer diagnostic;
- explicit epistemic separation of player and observer views;
- authoritative relocation distinct from ordinary smoothing;
- camera cannot modify simulation truth.

### AUTO KEEP + MEASURE

- exact translation smoothing constants;
- step/crouch/mantle/vault presentation envelopes;
- physical/recoil/landing impulse thresholds and amplitudes;
- small prediction-correction presentation smoothing;
- FOV baseline/tiny contextual modulation;
- moving-platform stabilization thresholds;
- camera-effect Priority Hotspot budget.

### QUESTION SUBROUTINE

Ask only when a choice materially changes player agency, embodiment, comfort, or future architecture. Current open camera questions are maintained in the Engine Autorecommendation Handoff.
