# nightfall!punk — v1.5 Player-Visible Combat Q719–Q720

Status: **LOCKED DESIGN CONTRACTS — ACTIVE BUILD**
Branch: `build/v1.5-combat-ai-communications`

## Q719 — KEEP: Sparse role-readable reticle

Follow the prior recommendation.

Use a sparse Quake-like central aiming mark with restrained Halo-like role readability. Dynamic reticle elements may expand/recover only when they truthfully project real weapon handling/spread state.

Invariant:

`ReticlePresentation -> RealHandlingState`

not:

`ReticleAnimation -> HiddenAccuracyRule`

The carbine should therefore read immediately as an FPS weapon while preserving nightfall's preference for legible causality over decorative HUD complexity.

## Q720 — KEEP + AMEND: Snap-Guided Physical Focus

Follow the prior Focus/ADS recommendation, amended by the project's physics + snap-guided guardrail goal.

Right-click Focus remains an optional precision sidegrade rather than an accuracy gate. Hip-fire remains competent.

The Focus state should **not** become a rigid camera lock or perfectly stabilized sight picture. Instead it should combine physically derived weapon/body motion with a bounded snap-guided capture envelope.

### Governing relation

`PhysicalImpulse -> Bounded Weapon/Body Offset -> Snap-Guided Capture -> Readable Focus Alignment`

The weapon may bobble and jostle from legitimate embodied causes such as:

- locomotor acceleration / deceleration;
- landing impulse;
- collision or shove impulse;
- recoil recovery;
- manipulator contamination / handling degradation;
- posture transition.

But the snap-guided guardrail continuously biases the presentation back toward the player's intended focus alignment.

### Aim-authority invariant

The bobble/jostle is **embodied presentation and bounded handling consequence**, not random aim theft.

`Player Aim Intent -> Focus Target`
`Physics -> bounded deviation / handling pressure`
`Snap Guardrail -> capture/recovery toward intended alignment`
`Authoritative Ballistics -> actual shot truth`

The player should feel the body and weapon negotiating with the world while still feeling that aiming belongs to them.

### Damped physical model

Implementation should prefer a cheap critically- or near-critically-damped spring-like presentation offset rather than unconstrained rigid-body chaos.

Conceptual form:

`offset_accel = physical_forcing - spring_restore - damping`

Then clamp/capture through the semantic focus envelope.

Desired qualitative behavior:

- small acceleration -> small readable bobble;
- sharp landing/collision -> brief jostle;
- sustained normal locomotion -> bounded rhythmic movement;
- focus input -> stronger capture toward sight alignment;
- severe handling degradation -> wider/slower recovery, still bounded;
- no persistent random drift;
- no violent camera oscillation;
- no physics bypass of collision/material/weapon authority.

### Game-theoretic sidegrade

Focus value remains relational rather than universally superior:

`FocusValue = PrecisionBenefit - PeripheralInformationCost - ModestMobilityCost - Handling/JostlePressure`

The guardrail reduces noise enough for deliberate precision while preserving the informational and embodied cost of narrowing attention.

### Design synthesis

- Quake: aim remains immediate and owned.
- Halo: focus remains readable and role-consistent.
- Prey/systemic design: material/physical conditions visibly matter.
- nightfall: physics negotiates the motion; snap-guided capture organizes it without erasing it.

Governing maxim:

> **The sight picture may wobble because the body is physical; it recenters because intention has authority.**

This remains presentation-compatible with the canonical Relational Perceptual Authority Camera and Snap Locomotion contracts.
