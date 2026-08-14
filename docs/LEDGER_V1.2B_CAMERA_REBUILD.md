# nightfall!punk — v1.2b Camera Rebuild Ledger

Date: 2026-08-13
Branch: `build/v1.2b-camera-rebuild`
Base: frozen v1.2 human-video candidate `e65e19e931794fbe2f7e7f359b982df6cc10d4de`

## FROM → TO

### Camera architecture

FROM:

`Mouse -> inline yaw/pitch -> predicted body eye -> inline raylib Camera3D`

and, in the standalone v1.2 causal demo, an accidental fixed oblique observer camera used as the only presentation.

TO:

`PlayerLookIntent -> ImmediateYaw/Pitch`

plus:

`Predicted/AuthoritativeBody -> RelationalMotionAnalysis -> PerceptualAuthorityEnvelope -> BoundaryEnvelope -> CameraPresentation`

then:

`PresentedAnchor + ImmediateViewOrientation -> raylib Camera3D`.

Canonical name: **Relational Perceptual Authority Camera**.

Canonical law:

> Aim belongs to player perceptual intent; camera position acknowledges embodied reality; presentation mediates between them.

### Camera role

FROM: camera behavior partly embedded directly in presentation executables.

TO: pure-C `nf_camera` core with explicit modes, look authority, body-follow presentation, bounded impulses, explicit FOV convention, relocation classification, and deterministic tests.

### Demo presentation

FROM: fixed oblique camera at `{48,58,-58}` looking at the causal slice, making the 80x80 m test appear miniature and preventing meaningful FPS-camera proof.

TO: **FIRST_PERSON default**, with explicit diagnostic modes:

- `F1 PLAYER VIEW`
- `F2 OBSERVER / LIVE`
- `F3 DEBUG TOP / LIVE`

Observer visibility remains developer evidence only and does not alter Player/teammate knowledge.

### FOV

FROM: naked raylib `.fovy=82` with ambiguous project terminology.

TO: explicit **vertical projection FOV** internally. Initial perceptual baseline remains 82° vertical (~114° horizontal at 16:9), with user-facing conversion/tuning reserved for later settings work.

### Invalid route presentation

FROM: blocked Route-B utility could leak the numerical `-FLT_MAX` sentinel into the HUD.

TO: blocked/invalid route utility is presented semantically as `BLOCKED`.

## EXISTING — regression-protected

- server-authoritative world/material truth;
- client prediction/reconciliation;
- existing movement controller and traversal states;
- `snap_locomotion`/affordance-capture trajectory remains downstream of legitimate affordances and upstream of physics;
- contamination -> degradation -> STASIS -> return;
- Human Rival AI;
- Dream Cattler ecology;
- topographical energy accounting;
- Lifeworld contracts;
- v1.2 teammate causality, evidence discipline, route gradients, Priority Hotspots, bounded memory, occupancy yielding, positive/negative causal proof;
- existing dedicated-server/network stack.

## ADVANCING — v1.2b implemented contracts

- pure-C `nightfall_camera` module, separate from raylib rendering;
- `NF_CAMERA_FIRST_PERSON`, `NF_CAMERA_DEBUG_OBSERVER`, `NF_CAMERA_DEBUG_TOP`;
- immediate/high-authority yaw/pitch look in player view;
- explicit vertical FOV ownership and horizontal-equivalent conversion;
- predicted/body-derived desired eye anchor;
- bounded vertical Perceptual Authority Envelope;
- small Perceptual Boundary Envelope for near-geometry presentation;
- relocation classification distinct from ordinary presentation smoothing;
- bounded additive translation/rotation impulse hook with decay;
- traversal/movement state controls presentation response bounds rather than authoring body motion;
- first-person causal demo default;
- diagnostic observer/top modes that suspend player input without changing actor epistemology;
- camera diagnostics: mode, correction class, anchor error, vertical/horizontal FOV;
- semantic `BLOCKED` route utility presentation;
- deterministic camera contract test;
- camera proof added to build/test wrapper.

## PAR — intentionally not falsely claimed

- production client fully consuming every `nf_camera` presentation function; the v1.2b proof first validates the shared camera core in the standalone causal harness before broader client replacement;
- user-facing FOV/sensitivity/accessibility settings UI;
- final per-traversal smoothing constants from human testing;
- gait micro-motion tuning;
- recoil/landing/damage/impact impulse tuning;
- moving-platform presentation tuning;
- first-person relational viewmodel implementation (world weapon truth + camera-space presentation proxy);
- selectively visible first-person lower body / full embodied-visibility implementation;
- camera-aware audio-listener refactor;
- controller/gamepad camera input;
- Apophis exceptional authored camera contract;
- cinematic camera graph, third-person gameplay, lock-on, aim magnetism, physical-head simulation, motion blur, DOF, chromatic aberration, photo/replay modes.

## Resolved design policy carried into this build

The v1.2b 100-question sweep Q147–Q246 is fully dispositioned. Key decisions consumed here:

- geometry proximity: Perceptual Boundary Envelope;
- authoritative relocation: Causal Discontinuity Presentation, never fake traversal interpolation;
- ordinary gait: low-amplitude relation-derived micro-motion if later measured useful, not mandatory sine-wave head bob;
- FOV: explicit internal projection convention, preserve perceptual baseline rather than accidental number semantics;
- viewmodel: one authoritative weapon cause with separate legitimate world/first-person presentations;
- first-person body: materially whole body with selectively visible first-person presentation when implemented.

## Build acceptance

Automated camera proof must show:

`LOOK=IMMEDIATE BODY=TRUTH PRESENTATION=BOUNDED RELOCATION=DISCONTINUITY`.

Human proof must show:

1. causal demo starts at Player scale in first person;
2. mouse look is immediate and stable;
3. ordinary movement/crouch/jump remain readable without obvious camera lag;
4. `ALTER_ROUTE` and teammate causality remain visible/functional;
5. F2 observer and F3 top modes are clearly diagnostic, not gameplay views;
6. return to F1 restores player-scale first-person interpretation;
7. no giant negative route-utility sentinel is visible.
