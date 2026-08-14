# nightfall!punk — Autorecommendation Decisions Q140–Q146

Date: 2026-08-13
Branch: `design/engine-autorecommendation-contracts`
Scope: design/engine policy only; frozen v1.2 human-video candidate remains unchanged.

These decisions apply the project rule:

`ExistingSystem -> CompatibleExtension -> MeasuredImprovement`

and:

> Automatically adopt choices that are deductively implied by accepted contracts or inductively very likely under the project's game-theoretic C-programming method; reserve questions for materially important mid/high uncertainty.

## RESOLVED

### Q140 — Humanoid ↔ dynamic-object force coupling

**Disposition: KEEP E + C.**

Canonical model: **Embodied Authority Envelope** implemented on top of a stable responsive character-controller architecture.

`ExternalImpulse -> AuthorityEnvelope -> Absorb | Bias | Interrupt | Displace`

Rules:

- Player/AI bodies retain high-authority arcade control under ordinary contact.
- Physical events can increasingly affect the body as genuine impulse/consequence rises.
- Body stability, posture, momentum, capability, energy, contamination and current traversal relation may modulate resistance.
- Ordinary contact does not become universal ragdoll simulation.
- Full loss of control remains exceptional and materially justified.
- Physical world must be able to "answer back" without turning locomotion into continuous Exanima-style indirect control.

Maxim:

> The actor commands the body strongly; the world can become strong enough to answer back.

### Q141 — Continuous vs hotspot-activated material simulation

**Disposition: KEEP E + C.**

Canonical model: conventional sleep/tick-rate/LOD machinery governed by a **Priority-Hotspot Simulation Gradient**.

`Dormant -> Background -> Active -> Hotspot`

As consequence rises, simulation resolution/frequency may rise. When consequence falls:

`DetailedState -> Compress -> PersistentConsequence`

Rules:

- Persist authoritative state even when detailed simulation sleeps.
- Use event-driven/sparse updates before universal continuous fields.
- Consequential/relevant localities may receive higher-frequency simulation.
- Gradient controls degree; hotspot identifies concentration; Priority Hotspot Filter regulates budget.
- Memory/history preserves the consequences that remain causally relevant.
- Every simulated variable needs a causal consumer.

Maxim:

> Simulate consequence intensely; remember consequence cheaply.

### Q142 — snap_locomotion capture strength/permissiveness

**Disposition: KEEP E + C.**

Canonical model: industry-style physical eligibility windows + an **Intent–Affordance Capture Gradient**.

`PhysicalEligibility -> IntentAffordanceGradient -> CaptureStrength`

Capture strength may depend on:

`IntentAlignment, AffordanceQuality, Distance, MomentumCompatibility, InputPersistence, EnergyCost, Risk, CompetingAffordances`.

Rules:

- No eligibility -> no capture.
- Weak relation -> ordinary locomotion/no capture.
- Moderate relation -> steering assistance.
- Strong relation -> soft capture.
- Very strong legitimate relation -> snap.
- Acquire/release hysteresis prevents chatter.
- Player and AI may share the same embodied capture contract while deriving intent differently.
- Invalid/failed capture flows into ordinary locomotion, micro-adjustment, local reroute or release.

Maxim:

> Success should snap; failure should flow.

### Q143 — camera translation during snap_locomotion

**Already resolved.**

**Disposition: KEEP Option 5 with Option 3 as governing guideline.**

Canonical model: **Relational Perceptual Authority Camera**.

`ActualBodyMotion -> RelationalMotionAnalysis -> PerceptualAuthorityEnvelope -> Traversal/StateBounds -> CameraPresentation`

Camera curves bound presentation of motion that actually occurred; they do not author locomotion.

### Q144 — physical-impact transmission to first-person camera

**Disposition: KEEP E + C.**

Canonical model: **Perceptual Impulse Gradient** expressed through a small bounded presentation vocabulary.

`PhysicalEvent -> ConsequenceGradient -> PresentationClass -> BoundedCameraImpulse`

Rules:

- Low-consequence contact produces approximately zero camera impulse.
- Genuine impact can produce translation/rotation proportional to meaningful embodied consequence.
- Presentation limits amplitude/duration so aim authority and comfort remain primary.
- Impulse decays through bounded presentation state rather than replacing player yaw/pitch.
- Do not simulate a free physical head.

Maxim:

> Physics earns camera attention; presentation limits how much attention it receives.

### Q145 — observer/debug camera relation to live simulation

**Disposition: KEEP E + C.**

Canonical model: **Epistemic Observer Contract** with explicit view/simulation states.

Initial states:

- `PLAYER_VIEW / LIVE`
- `OBSERVER_VIEW / LIVE`
- `DEBUG_TOP / LIVE`

Later, if useful:

- `OBSERVER_VIEW / FROZEN`
- single-step diagnostic mode.

Rules:

- First-person gameplay remains default.
- Entering observer mode suspends player gameplay input but does not move/teleport the player body.
- Live simulation continues by default in observer mode.
- Returning to player view reattaches to the body's current eye anchor and preserves legitimate look state where possible.
- Observer visibility is developer evidence only and must never become Player/teammate evidence.

Maxim:

> Change what the developer can see without changing what the actor knows.

### Q146 — body/camera yaw coupling during traversal

**Disposition: KEEP E + C.**

Canonical model: traversal-specific physical requirements bounded by an **Orientation Authority Envelope**.

`DeltaYaw = PlayerViewYaw - BodyYaw`

Rules:

- Player view remains independently controlled inside a legitimate envelope.
- `snap_locomotion` may align body orientation only as much as material traversal requires.
- Vault: normally weak alignment.
- Mantle: moderate alignment.
- Ladder: stronger body-to-surface alignment.
- Narrow aperture: enough alignment to satisfy body/collider relation.
- `snap_locomotion` does not force the camera to look at the affordance.
- If view/body divergence becomes materially incompatible, capture weakens, adjusts, releases or fails rather than stealing camera aim.

Maxim:

> The body may align to what it must traverse; the eyes remain the player's until material reality makes the traversal untenable.

## Combined locked architecture

`RelationalActionSelection`
`-> SemanticSteering`
`-> snap_locomotion`
`-> EmbodiedAuthorityEnvelope`
`-> ExistingLocomotion/Physics`
`-> WorldConsequence`
`-> RelationalPerceptualAuthorityCamera`

Material simulation follows:

`PersistentWorldState -> PriorityHotspotSimulationGradient -> SmallestSufficientSimulation -> CompressedPersistentConsequence`.

The camera follows:

`PlayerLookIntent -> ImmediateViewAuthority`

plus:

`ActualBodyMotion/PhysicalConsequence -> BoundedPresentationEnvelope -> FinalCamera`.

## No longer open

Do not re-ask Q140–Q146 unless implementation evidence materially contradicts these contracts.

Exact constants, thresholds and amplitudes remain **AUTO KEEP + MEASURE**, not architectural questions.
