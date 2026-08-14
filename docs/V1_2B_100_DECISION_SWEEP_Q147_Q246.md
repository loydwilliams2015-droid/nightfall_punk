# nightfall!punk — v1.2b 100-Question Decision Sweep

Date: 2026-08-13
Branch: `design/engine-autorecommendation-contracts`
Scope: Q147–Q246, exactly 100 v1.2b-relevant game-building questions.

Method:

`AcceptedContracts -> Deductive/InductiveRecommendation -> SmallestCompatibleImplementation -> Measure`

Statuses:

- **AUTO KEEP** — architecture is sufficiently determined; implement when consumed.
- **AUTO KEEP + MEASURE** — direction is determined; exact constants/feel remain empirical.
- **DEFER** — deliberately outside the bounded v1.2b camera rebuild; do not add machinery yet.
- **QUESTION** — genuine mid/high uncertainty with materially different architectural consequences.

The frozen v1.2 human-video candidate remains unchanged. This file is design policy only.

## Camera truth, projection, and update order

### Q147 — What happens when the FPS eye approaches geometry?
**AUTO KEEP — Relational Perceptual Boundary Envelope.** Body eye remains truth; near-plane handling and only tiny bounded presentation correction may prevent visual penetration. Camera cannot manufacture a viewpoint or solve locomotion.

### Q148 — How should authoritative relocation/STASIS return be presented?
**AUTO KEEP — Causal Discontinuity Presentation.** Brief bounded perceptual interruption -> immediate re-anchor -> recovery. Never visually interpolate through space the actor did not traverse.

### Q149 — How much ordinary gait motion reaches the camera?
**AUTO KEEP + MEASURE — Relational Micro-Motion.** Low-amplitude response may derive from actual acceleration/ground contact; no mandatory decorative sine-wave head-bob. Accessibility scaling required eventually.

### Q150 — Near/far clipping policy?
**AUTO KEEP + MEASURE.** Keep near plane as small as practical without depth/weapon artifacts; far plane follows actual scene need. Do not use camera displacement to compensate for bad clipping configuration.

### Q151 — What does the project mean numerically by first-person FOV?
**QUESTION.** Current raylib `Camera3D.fovy=82` is a vertical-FOV value, while player expectations often discuss horizontal FOV. Preserve 82° vertical, reinterpret the intended number as horizontal, or formalize another convention before 1.2b tuning.

### Q152 — Sprint FOV modulation?
**AUTO KEEP + MEASURE.** Optional, tiny, bounded, presentation-only. Default amplitude should be near zero until human proof shows benefit.

### Q153 — Crouch FOV modulation?
**AUTO KEEP.** No default FOV change merely because posture changes; eye-height already communicates crouch.

### Q154 — Vault/mantle/ladder FOV modulation?
**AUTO KEEP + MEASURE.** Default none. Add only small bounded modulation if human testing proves a traversal-readability benefit.

### Q155 — Aspect-ratio handling?
**AUTO KEEP.** Preserve a stable explicit world-FOV convention across aspect ratios; do not tune camera feel by resolution-specific magic numbers.

### Q156 — Resolution independence?
**AUTO KEEP.** Camera world transform and HUD layout remain separate concerns. Camera behavior must not depend on pixel dimensions except projection/aspect calculations.

### Q157 — Camera update order?
**AUTO KEEP.** Input -> prediction/simulation -> reconciled/predicted body state -> camera presentation -> render. Camera never feeds simulation truth backward.

### Q158 — Fixed-tick versus render-rate camera updates?
**AUTO KEEP.** Simulation/body truth remains fixed-tick; camera presentation may update/interpolate at render rate from current body state without becoming a second simulation.

### Q159 — Prediction/reconciliation visual smoothing?
**AUTO KEEP + MEASURE.** Small corrections may be presentation-smoothed; large/authoritative corrections prioritize truth. Maintain explicit error thresholds.

## Camera impulses and presentation budget

### Q160 — How do multiple camera shakes/impulses combine?
**AUTO KEEP.** Small bounded effect accumulator with caps/decay; no unbounded additive stack.

### Q161 — Which concurrent camera effects survive?
**AUTO KEEP.** Priority-Hotspot presentation budget: consequence and relevance regulate which effects consume limited perceptual bandwidth.

### Q162 — Where does recoil originate?
**AUTO KEEP.** Weapon/combat event owns recoil cause; camera/viewmodel only present its consequence. Camera does not invent recoil.

### Q163 — Recoil recovery?
**AUTO KEEP + MEASURE.** Bounded recoverable presentation with player input remaining authoritative; tune per weapon later.

### Q164 — Landing response?
**AUTO KEEP + MEASURE.** Derived from actual vertical consequence/velocity and body state; low impacts approximately zero, harder landings produce bounded response.

### Q165 — Jump takeoff response?
**AUTO KEEP + MEASURE.** Minimal; body motion already communicates takeoff. Add only if readability improves without aim disturbance.

### Q166 — Moving-platform stabilization?
**AUTO KEEP + MEASURE.** Preserve stable relative framing to platform/body while retaining real displacement; do not detach camera into world-space smoothing.

### Q167 — Ladder camera presentation?
**AUTO KEEP + MEASURE.** Stronger body-to-surface relation, player look stays locally independent inside orientation envelope; no forced target-look.

### Q168 — Mantle camera presentation?
**AUTO KEEP + MEASURE.** Present actual body trajectory with moderate bounded translation response; failed mantle follows actual failure.

### Q169 — Vault camera presentation?
**AUTO KEEP + MEASURE.** Weaker alignment and lighter presentation than mantle; preserve momentum/readability.

### Q170 — Crouch eye-height transition?
**AUTO KEEP + MEASURE.** Short bounded interpolation of eye anchor; body/collider posture changes first.

### Q171 — Step-up/down camera response?
**AUTO KEEP + MEASURE.** Suppress harsh high-frequency vertical pop without hiding real elevation change.

### Q172 — Slopes?
**AUTO KEEP.** Camera follows body/eye truth nearly directly; do not add slope-specific cinematic tilt.

### Q173 — Airborne camera handling?
**AUTO KEEP.** Preserve immediate look and body-derived translation; no automatic horizon correction beyond explicit project constraints.

### Q174 — Wall-contact/wall-slide presentation?
**AUTO KEEP + MEASURE.** Only actual embodied/contact consequence may generate small response; wall contact alone is not a camera event.

### Q175 — Double-jump presentation?
**AUTO KEEP + MEASURE.** Minimal impulse derived from actual second-jump acceleration; no special camera animation required.

### Q176 — Sprint presentation beyond FOV?
**AUTO KEEP + MEASURE.** Allow only low-amplitude body-derived micro-motion; sprint cannot introduce aim lag.

### Q177 — Reverse/braking presentation?
**AUTO KEEP.** Camera follows actual deceleration; no separate braking animation unless measured need appears.

## Contamination, agency, and relocation

### Q178 — Locomotor contamination camera effects?
**AUTO KEEP + MEASURE.** Contamination changes embodied locomotion first; camera presents resulting body consequence. Do not fake movement degradation only in camera.

### Q179 — Sensory contamination camera effects?
**AUTO KEEP + MEASURE.** Legitimate sensory-domain degradation may affect perception presentation, but must be bounded, readable, accessibility-aware, and never fabricate world truth or actor knowledge.

### Q180 — STASIS camera state?
**AUTO KEEP.** Explicit agency transition. Camera no longer behaves as ordinary free locomotion while actor is in STASIS; preserve causal legibility.

### Q181 — Return/re-entry camera state?
**AUTO KEEP.** Use Q148 causal discontinuity and immediate re-attachment to returned body truth.

### Q182 — Look input during temporary agency loss?
**AUTO KEEP.** Input behavior must follow agency contract explicitly; never let camera imply locomotor agency the actor does not possess.

## Debug/observer camera

### Q183 — Debug camera keys?
**AUTO KEEP + MEASURE.** Prefer small stable mapping such as F1 player, F2 observer-live, F3 debug-top; exact keys are ergonomic tuning.

### Q184 — Observer movement speed?
**AUTO KEEP + MEASURE.** Bounded normal/fast diagnostic speeds, independent of gameplay locomotion.

### Q185 — Observer collision?
**AUTO KEEP.** Observer is a diagnostic freecam/no-clip by default; it must not alter physical world state.

### Q186 — Player input while observer-live is active?
**AUTO KEEP.** Suspend player gameplay input while leaving player body/world simulation live unless explicitly frozen.

### Q187 — Observer HUD?
**AUTO KEEP.** Clearly label `OBSERVER / LIVE`, `DEBUG TOP / LIVE`, etc.; diagnostics may expose world truth unavailable to actors.

### Q188 — Observer epistemic isolation?
**AUTO KEEP.** Observer visibility is developer information only and cannot update Player, teammate, Rival, or Cattler evidence.

### Q189 — Debug-top purpose?
**AUTO KEEP.** Spatial/causal inspection only; not a gameplay camera and not used to judge FPS feel.

### Q190 — Minimum camera diagnostics?
**AUTO KEEP.** Mode, desired anchor, presented-anchor delta, yaw/pitch, FOV, active impulse, correction class, traversal relation.

### Q191 — Camera causal logging?
**AUTO KEEP.** Log consequential transitions, not every frame: `CAUSE -> BODY CHANGE -> PRESENTATION RESPONSE -> SETTLE`.

### Q192 — Camera deterministic tests?
**AUTO KEEP.** Pure/bounded tests for look response, anchor derivation, smoothing classifications, relocation, observer isolation, and failure cases.

## Accessibility and input

### Q193 — Camera-motion accessibility scale?
**AUTO KEEP.** Eventually expose a presentation-motion multiplier including a zero/minimal option; simulation/gameplay remains unchanged.

### Q194 — FOV accessibility?
**AUTO KEEP.** User-configurable within safe bounds once Q151 convention is resolved; world logic cannot depend on chosen presentation FOV.

### Q195 — Shake/impulse accessibility?
**AUTO KEEP.** Expose reduced/disabled presentation impulse while retaining material/gameplay consequences.

### Q196 — Horizontal vs vertical look sensitivity?
**AUTO KEEP + MEASURE.** Default coherent ratio; allow independent tuning only if useful, without hidden acceleration.

### Q197 — Mouse capture/focus behavior?
**AUTO KEEP.** Preserve explicit capture state; release/reacquire cleanly across focus loss, menus, and observer modes.

### Q198 — Mouse-delta fallback?
**AUTO KEEP.** Keep robust fallback only as compatibility machinery; ordinary relative input should remain stable and diagnosable.

### Q199 — Input timing/order?
**AUTO KEEP.** Sample view input once per render frame, apply bounded look state, feed simulation commands consistently; avoid double-applying latched deltas across fixed ticks.

### Q200 — Yaw wrapping?
**AUTO KEEP.** Normalize/wrap for numerical stability and diagnostics without perceptual discontinuity.

### Q201 — Pitch clamp?
**AUTO KEEP + MEASURE.** Keep explicit clamp near current practical range; exact limit is tuning/accessibility rather than architecture.

### Q202 — Gamepad/controller look?
**DEFER.** Preserve module interfaces so it can be added later; do not expand 1.2b scope before mouse camera proof is stable.

### Q203 — Invert-Y?
**AUTO KEEP.** Straightforward user setting; no architecture question.

### Q204 — Sensitivity setting?
**AUTO KEEP.** Explicit user setting with stable units/scale; camera code owns application, not gameplay physics.

### Q205 — Mouse acceleration?
**AUTO KEEP.** No hidden engine-side acceleration by default. Platform/raw-input differences remain compatibility details.

## Body/view orientation and traversal

### Q206 — Remember view/body yaw divergence?
**AUTO KEEP.** Orientation envelope maintains explicit body and view yaw rather than silently overwriting one with the other.

### Q207 — Re-coupling after traversal?
**AUTO KEEP.** Gradual/bounded re-coupling through ordinary movement intent; no forced camera snap unless material constraint requires traversal release.

### Q208 — Narrow-passage orientation?
**AUTO KEEP + MEASURE.** Body aligns enough to satisfy collider/affordance relation; view remains locally independent within bounded envelope.

### Q209 — Ladder orientation?
**AUTO KEEP + MEASURE.** Stronger body alignment than ordinary traversal, but no automatic camera target-lock.

### Q210 — Dynamic-object impact camera response?
**AUTO KEEP + MEASURE.** Use Embodied Authority Envelope -> Perceptual Impulse Gradient. Tiny contacts vanish; material impacts communicate bounded consequence.

### Q211 — Actor-actor collision camera response?
**AUTO KEEP + MEASURE.** Same shared impulse/consequence contract; no special faction-based camera rule.

### Q212 — Damage impact response?
**AUTO KEEP + MEASURE.** Damage event may produce bounded directional perceptual impulse proportional to legitimate consequence, independently reducible for accessibility.

### Q213 — Explosion response?
**AUTO KEEP + MEASURE.** Physical/concussive consequence drives bounded translation/rotation/FOV effect; distance alone is not sufficient without actual relation.

### Q214 — Per-weapon recoil camera profile?
**AUTO KEEP + MEASURE.** Use shared bounded recoil grammar with weapon-specific parameters; do not create bespoke camera code per weapon.

### Q215 — Hit reaction vs aim authority?
**AUTO KEEP.** Aim authority remains primary; hit response is additive and bounded, never a replacement view controller.

### Q216 — Crosshair stability?
**AUTO KEEP.** Crosshair represents player aim direction, not transient body/camera presentation offset, unless a later explicit weapon-accuracy system says otherwise.

## Weapon/viewmodel relation

### Q217 — What is the canonical weapon/viewmodel anchoring architecture?
**QUESTION.** Choose among camera-space viewmodel, world/body-space first-person weapon, or a hybrid split. This materially affects clipping, recoil, animation, body visibility, and future networking/presentation.

### Q218 — Viewmodel FOV/projection?
**DEFER pending Q217.** Do not decide projection constants before deciding what the viewmodel physically represents.

### Q219 — Viewmodel bob?
**DEFER.** No need for a decorative bob subsystem in 1.2b; derive later from the chosen viewmodel architecture and body motion if useful.

### Q220 — Viewmodel sway?
**DEFER.** Same reason; preserve aim clarity first.

### Q221 — Viewmodel recoil animation?
**DEFER.** Combat owns recoil cause; exact weapon presentation waits for viewmodel architecture.

### Q222 — Split camera recoil vs viewmodel recoil?
**DEFER.** Resolve after Q217 so two presentation layers do not duplicate the same event unintentionally.

### Q223 — Viewmodel clipping?
**DEFER pending Q217.** Avoid solving a representation problem before choosing representation.

## First-person body representation

### Q224 — Should the player see a first-person body, and what kind?
**QUESTION.** No body, lower-body proxy, or unified full-body first-person representation have materially different animation/camera/collision costs and benefits.

### Q225 — First-person shadow/body proxy?
**DEFER pending Q224.** Keep world shadow/body representation separate from camera decision for now.

### Q226 — Own-body occlusion?
**DEFER pending Q224.** Cannot define self-occlusion policy before choosing visible-body architecture.

### Q227 — Camera anchor versus hitbox/head geometry?
**AUTO KEEP.** Camera perceptual anchor, physical collider, and damage hitbox remain distinct contracts even if spatially related. Never use camera location as damage truth by default.

## Audio/render presentation

### Q228 — Audio listener position?
**AUTO KEEP.** Derive listener from player perceptual/head relation while respecting body truth; observer camera must not silently become gameplay hearing source.

### Q229 — Audio listener orientation?
**AUTO KEEP.** Player look orientation drives first-person listener orientation; diagnostic observer audio, if any, is explicitly separate.

### Q230 — Camera owns lighting/exposure state?
**AUTO KEEP.** No. World/render systems own lighting/material truth; camera may own presentation exposure adaptation only if later required.

### Q231 — World visual-state versus camera postprocess?
**AUTO KEEP.** World state remains causal source; postprocess is bounded presentation. Visual effects cannot manufacture gameplay world state or evidence.

### Q232 — Motion blur?
**DEFER / default off.** Not required for 1.2b and can obscure high-authority motion diagnosis.

### Q233 — Depth of field?
**DEFER / default off.** Not relevant to causal FPS camera proof.

### Q234 — Chromatic aberration?
**DEFER / default off.** May become an authored/sensory effect later; not a baseline camera feature.

### Q235 — Camera roll?
**AUTO KEEP + MEASURE.** Ordinary roll approximately zero. Only bounded event/traversal response if actual embodiment/readability warrants it.

### Q236 — Leaning system?
**DEFER.** Not part of current 1.2b movement proof; do not create camera/cover machinery prematurely.

### Q237 — Aim magnetism?
**AUTO KEEP — absent by default.** `snap_locomotion` assists legitimate traversal, not aiming at actors.

### Q238 — Lock-on targeting?
**AUTO KEEP — absent by default.** Incompatible with current high-authority FPS aim unless a future explicit mode requires it.

### Q239 — Ordinary cinematic camera takeover?
**AUTO KEEP — prohibited.** Normal gameplay systems do not seize first-person look authority.

### Q240 — Apophis-authored exceptional camera authority?
**DEFER.** Possible only as a clearly signaled exceptional/set-piece contract; not part of baseline camera architecture.

### Q241 — Photo mode?
**DEFER.** Useful later, but debug observer is sufficient for v1.2b.

### Q242 — Replay/spectator camera?
**DEFER.** Distinct future networking/presentation system; do not conflate with local diagnostic observer.

### Q243 — Network-replicated debug observer state?
**DEFER.** Debug observer is local developer presentation in 1.2b and must not burden gameplay protocol.

## Performance, persistence, acceptance

### Q244 — Camera performance budget?
**AUTO KEEP + MEASURE.** Camera work should remain O(1) or bounded local queries per frame; no full-world scan. Diagnostics instrument time/cost if camera geometry queries grow.

### Q245 — Camera configuration persistence?
**AUTO KEEP.** Persist user-facing settings such as sensitivity/invert/FOV/motion scale when implemented; do not serialize transient runtime impulses/observer state as gameplay continuity.

### Q246 — v1.2b acceptance gate?
**AUTO KEEP.** Require strict compile/regression + camera deterministic tests + human first-person proof. Human proof must show responsive aim, correct body anchoring, traversal presentation, causal teammate demo readability, observer separation, and no `-FLT_MAX` HUD leak.

## Results

Exactly **100 questions** were processed.

- **3 QUESTION:** Q151, Q217, Q224.
- All other questions are resolved as **AUTO KEEP**, **AUTO KEEP + MEASURE**, or deliberate **DEFER**.

### Strong architectural convergence

The sweep repeatedly converges on the same small set of laws:

1. **Aim authority is immediate; body position is embodied; camera presentation is bounded.**
2. **Camera is downstream of simulation and cannot become locomotion, physics, damage, evidence, or prediction truth.**
3. **`snap_locomotion` changes the body first; the camera presents what actually happened.**
4. **Gradient determines response magnitude; Priority Hotspots regulate limited perceptual/computational budget; memory/decay settles presentation over time.**
5. **Use shared contracts with parameters rather than bespoke camera code for every traversal, weapon, impact, or actor.**
6. **Accessibility changes presentation amplitude, not gameplay consequence.**
7. **Observer tools change developer perception without changing actor knowledge.**
8. **Defer cinematic/viewmodel/body machinery until a concrete representation choice creates a consumer.**

### v1.2b minimum-sufficient implementation implied by the 100 decisions

- extract `nf_camera` from inline client/demo code;
- first-person default for causal demo;
- immediate yaw/pitch;
- predicted-body eye anchor;
- bounded Perceptual Authority Envelope for translation;
- near-geometry Perceptual Boundary Envelope;
- bounded impulse accumulator with Priority-Hotspot budget;
- explicit relocation/discontinuity class;
- observer-live/debug-top modes with epistemic isolation;
- camera diagnostics and deterministic tests;
- semantic `BLOCKED/INVALID` route HUD;
- preserve all existing v1.2 causal/regression proofs;
- do **not** yet add cinematic graph, motion blur, DOF, lock-on, leaning, photo/replay, generalized viewmodel/body machinery, or networked debug observer state.

Do not reopen Q147–Q246 unless implementation evidence materially contradicts the recorded disposition. Q151, Q217, and Q224 are the only current questions from this sweep that require explicit design choice.