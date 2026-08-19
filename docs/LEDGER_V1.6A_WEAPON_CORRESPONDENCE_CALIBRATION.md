# nightfall!punk v1.6A — Weapon Correspondence / Calibration ledger

Branch: `build/v1.6a-weapon-correspondence-calibration`  
Basis: v1.6 head `580c87ce58637f15b0241a3d1660958c3155f727`  
Status: **IMPLEMENTATION CANDIDATE — AUTOMATED PROOF REQUIRED — NATIVE HUMAN PROOF REQUIRED — DO NOT MERGE ON CI ALONE**

## Build thesis

> **The player owns the aim; the actor–weapon–world relation determines the difficulty of realizing it. Focus temporarily organizes that relation toward the player's intention without possessing the intention itself.**

v1.6A is not a replacement weapon system. It is a correspondence/calibration pass over the v1.6 Weapon Solution programme.

Canonical causal relation:

```text
RAW INPUT
-> FOCUS INTENT / TARGET
-> FOCUS RUNTIME
-> CURRENT WEAPON ENVELOPE
-> LIVE RETICLE / FOV / VIEWMODEL
-> ACCEPTED FIRE GATE
-> DETERMINISTIC SHOT REALIZATION
-> PHYSICAL MUZZLE / OBSTRUCTION
-> HIT / MISS / NEARBY IMPACT
-> LAST-SHOT RECORD
```

`CURRENT` and `LAST SHOT` are deliberately different temporal objects.

## FROM

v1.6 already supplies:

- shared pure-C `nf_weapon_solution`;
- C+D precision identity: small truthful physical floor + primarily state-produced dispersion;
- deterministic accepted-shot ancestry;
- movement, airborne, manipulator-contamination and recoil terms;
- context-sensitive Focus affecting actual ballistic stability/recovery rather than damage;
- competent hip-fire / optional Focus;
- layered recoil with player long-horizon aim ownership;
- physical muzzle-origin focused demo;
- deterministic weapon tests and strict CI;
- production server integration deliberately not yet migrated to the shared solver.

Human inspection of v1.6 exposed two correspondence questions:

1. the demo reticle was driven by `last_solution`, so it could present historical shot uncertainty as if it were current capability;
2. RMB/Focus human evidence was intermittent/ambiguous, so the raw input chain needed direct instrumentation before changing semantics.

## TO

v1.6A advances the proof path to:

- `NfWeaponEnvelopeInput` / `NfWeaponEnvelope`: **present-tense, sequence-free capability**;
- `nf_weapon_evaluate_envelope(...)`: pure current-state evaluation;
- `nf_weapon_realize_shot(...)`: accepted-shot deterministic realization;
- legacy `nf_weapon_solve(...)` retained as a compatibility wrapper over the same law;
- live reticle driven by `CURRENT`, never by `LAST SHOT`;
- `LAST SHOT` retains weapon id, accepted sequence, Focus/reduction and realized direction as retrospective evidence;
- FOV-aware, mildly nonlinear, monotonic screen-space reticle projection;
- raw RMB, developer-F Focus control, window-focus, transition count, Focus target and Focus amount diagnostics;
- calibrated Focus acquisition prior around 200–250 ms and faster release prior around 120–180 ms;
- context-sensitive full-Focus state reduction approximately 50–70% as instability rises;
- carbine full-Focus FOV narrowing prior around 7 degrees;
- small camera recoil acknowledgement, approximately 10–15% of perceived recoil, without changing underlying aim ownership;
- material blocked-muzzle behavior: accepted ordinary firearm shot consumes the round and produces recoil/sound/nearby obstruction impact;
- blocked-muzzle communication: diegetic weapon/world response plus a subtle reticle blocked-state cue;
- production client/server solver graft explicitly postponed to v1.6B.

## Stable coherence / ownership

### Authority

- Player/camera owns **aim intention**.
- Weapon envelope owns **present realizability / bounded uncertainty**.
- Fire gate owns **whether a round is accepted**.
- Accepted-shot sequence + shared solver own **realized deterministic direction**.
- Physical world/muzzle owns **obstruction and impact possibility**.
- Reticle owns **presentation of current envelope only**.
- LAST SHOT owns **retrospective event evidence only**.
- Debug instrumentation owns **observation**, never gameplay truth.

### C discipline

- no per-shot or per-envelope heap allocation;
- current envelope is side-effect-free;
- no future shot sequence is consumed to draw the reticle;
- accepted sample remains deterministic from stable ancestry;
- current and historical state are separately named;
- v1.6 compatibility wrapper prevents a second weapon law;
- production server integration will call the shared law in v1.6B rather than reimplement it.

## Adopted calibration priors

| Dimension | v1.6A prior | Status |
|---|---:|---|
| carbine calm baseline | 0.12° | keep + human proof |
| context-sensitive Focus state reduction | ~50–70% with rising instability | adopted prior |
| full-Focus carbine FOV narrowing | ~7° | adopted prior |
| camera recoil share | ~10–15% perceived recoil | adopted prior |
| Focus acquisition to near-settled | ~200–250 ms | adopted prior |
| Focus release to near-zero | ~120–180 ms | keep + measure |
| reticle perceptual smoothing | ~50–60 ms | adopted prior |
| reticle small-cone mapping | mild nonlinear amplification | adopted family; coefficients human-calibrated |

These are calibration priors, not values protected from falsifying native evidence.

## OITOA acceptance lenses

### Observability

The video must make raw input, Focus, current cone, live reticle, last shot and obstruction legible.

### Inspectability

The HUD must distinguish:

```text
INPUT
NOW
LAST SHOT
```

and decompose base / motion / airborne / contamination / recoil / Focus reduction.

### Testability

Automated tests must prove:

- present envelope changes with state without needing a shot;
- envelope evaluation consumes no shot sequence;
- realization is deterministic for identical ancestry;
- different accepted sequences may vary within the bounded envelope;
- legacy v1.6 wrapper and new envelope/realization seam produce the same law;
- Focus acquisition/release lands in the intended calibration bands;
- reticle mapping is monotonic and FOV-aware.

### Organizationality

One owner per concept; no demo-local cone formula and no last-shot state masquerading as current state.

### Assessability

Human proof distinguishes:

- PASS;
- PASS + TUNE;
- FAIL / correspondence defect;
- INSUFFICIENT EVIDENCE.

## PAR sheet

PAR is comparative benchmarking, not an acronym and not a DEFER label. The project remains architecturally itself.

### PAR-1.6A-D — perception / body / weapon correspondence

**Reference type:** primary typed precedent / methodology.  
**Reference family:** modern ranged-combat camera/body/weapon correspondence and debugging practice.  
**Comparable dimensions:** perceptual truth, input-to-result correspondence, inspectability, debugging.  
**Nightfall synthesis:** player intention remains authoritative as intention; CURRENT envelope reports present realizability; LAST SHOT records realized history.  
**Negation:** no third-person camera grammar, no target magnetism, no animation-authored ballistics.  
**Current disposition:** **IMPLEMENTED CANDIDATE / HUMAN PAR INSUFFICIENT EVIDENCE until v1.6A video**.

### PAR-1.6A-A — Halo / Griesemer-style micro-tuning discipline

**Reference type:** primary typed precedent for parameter sensitivity.  
**Comparable dimensions:** weapon-role readability, small-parameter consequence, controlled A/B tuning.  
**Nightfall synthesis:** C+D dispersion + state-dependent Focus; calibrate coupled consequences rather than copy values.  
**Negation:** do not copy Halo aim assist, exact numbers, exact reticle, or sandbox roles.  
**Current disposition:** **DIFFERENT-BUT-COMPARABLY-VALID architecture; final calibration evidence pending**.

### PAR-1.6A-B — Quake / ioquake3 immediacy guardrail

**Comparable dimensions:** mouse authority, competent hip-fire, spatial immediacy.  
**Nightfall synthesis:** Focus cannot become mandatory ADS; camera recoil cannot possess long-horizon aim.  
**Current disposition:** **GUARDRAIL PRESERVED / dedicated B stress proof later**.

### PAR-1.6A-C — Titanfall movement-combat coupling guardrail

**Comparable dimensions:** initiative, motion-to-firing-opportunity coupling.  
**Nightfall synthesis:** Focus is designed to become an advanced expressive tool when combined with Snap Locomotion / smoothed affordance guidance and the open physical world, not a stationary accuracy ritual.  
**Current disposition:** **INSUFFICIENT EVIDENCE intentionally; dedicated v1.6B proof**.

### PAR-1.6A-PF — Prey / F.E.A.R. systemic and causal readability

**Comparable dimensions:** material consequence, obstruction, readable cause/reaction.  
**Nightfall synthesis:** blocked ordinary firearm shot still happens materially and produces nearby impact evidence; reticle supplies only a subtle feasibility cue.  
**Current disposition:** **IMPLEMENTED CANDIDATE / human obstruction proof pending**.

## Existing / advancing / later

### EXISTING — preserve

- v1.5 actor-local epistemology, tactical appraisal, metabolic/restitution and communication presentation;
- Snap Locomotion and Relational Perceptual Authority;
- contamination domain specificity;
- current combat authority and weapon state machine;
- C+D dispersion and deterministic accepted-shot ancestry;
- competent hip-fire and bounded airborne shooting;
- sparse reticle center + brackets language;
- restrained hit confirmation;
- project-wide PAR terminology.

### ADVANCING — v1.6A

- present-tense envelope;
- last-shot provenance separation;
- input-chain observability;
- calibrated Focus timing/curve;
- truthful live reticle;
- mild nonlinear reticle readability mapping;
- blocked-muzzle material consequence and subtle cue;
- small non-possessive camera recoil;
- deterministic correspondence tests;
- dedicated native proof range.

### LATER — v1.6B

1. **Production authority graft first:** carry Focus input, envelope state, accepted-shot realization, prediction/reconciliation and bounded rewind through the production client/server path without a second implementation.
2. **Advanced Focus synergy:** treat Focus as a fun mastery tool for players who combine it with Snap Locomotion / Jet Set Radio-like smoothed affordance guidance in an open, systemic Oxygen Not Included-style physical world.
3. **B -> C PAR stress:** Quake-like immediate aim authority first, then Titanfall-like movement-combat initiative under locomotor pressure.
4. Prospective weapon feasibility may later enter tactical AI as one bounded opportunity term; it does not command ATTACK and is not part of v1.6A.

## Predictions for native v1.6A proof

1. Calm stationary carbine remains near its small 0.12° baseline; full Focus changes it only slightly.
2. Movement broadens NOW brackets without firing; stopping contracts them without firing.
3. Clean RMB hold makes raw RMB remain DOWN and Focus approach near-settled in roughly 200–250 ms.
4. Developer F control follows the same Focus runtime; divergent RMB/F behavior localizes input/capture faults.
5. Focus produces substantially greater marginal benefit under instability than when already calm.
6. Pistol -> carbine switch updates NOW immediately while LAST SHOT remains pistol history until another accepted shot.
7. Burst fire broadens NOW through recoil; recovery contracts NOW while camera returns without fighting mouse authority.
8. Near-wall blocked shot consumes ammunition, produces recoil and a nearby impact, and does not damage the target through geometry.
9. 60/120 FPS presentation cannot change fixed-tick envelope or accepted-shot ancestry.

## Human-only unresolved evidence questions

These are no longer pre-code architecture forks; the video should answer them:

- Does Focus become rational to hold almost continuously?
- Is Focus almost never worth using?
- Does the truthful 0.12° calm baseline still feel like arbitrary miss behavior?
- What exact mild-nonlinear reticle coefficients are the weakest that remain legible at 1280×720?
- Do ~200–250 ms acquisition and ~120–180 ms release feel immediate but meaningful?
- Does ~10–15% camera recoil feel embodied without camera possession?
- Is the blocked reticle cue subtle enough?
- Does accepted-round + near-wall impact feel materially fair?

## Automated implementation checklist

- [x] branch from exact v1.6 head
- [x] preserve v1.6 compatibility wrapper
- [x] add sequence-free current envelope API
- [x] add accepted-shot realization API
- [x] tune Focus curve/timing priors
- [x] add FOV-aware mild nonlinear reticle projection
- [x] add v1.6A deterministic tests
- [x] add v1.6A graphical proof demo
- [x] instrument RMB / developer Focus / window / Focus target
- [x] implement CURRENT vs LAST SHOT HUD
- [x] implement blocked-muzzle accepted-shot/impact behavior in proof demo
- [x] implement subtle blocked reticle cue
- [x] preserve production server boundary for B
- [x] add ledger / PAR / handoff
- [ ] strict graphical compile on exact final head
- [ ] inherited + v1.6 + v1.6A CTests on exact final head
- [ ] named v1.6A smoke on exact final head

## Human acceptance checklist

- [ ] no-fire move -> brackets expand
- [ ] no-fire stop -> brackets recover
- [ ] RMB raw state remains legible during hold
- [ ] Focus acquisition/release feels correct
- [ ] F developer control proves same runtime path
- [ ] sky and foreground Focus controls are visibly distinct
- [ ] moving -> stop Focus recapture is useful but not mandatory
- [ ] focused accepted shot records nonzero Focus in LAST SHOT
- [ ] pistol -> carbine switch proves temporal separation
- [ ] burst recovery does not steal long-horizon aim
- [ ] blocked cue appears only when appropriate
- [ ] blocked shot consumes round and impacts nearby obstruction
- [ ] no target damage through close obstruction
- [ ] no crash or inherited regression

## Acceptance rule

Do not merge on green CI alone. v1.6A closes when deterministic/compile evidence is green **and** the native recording establishes present-tense correspondence, Focus usefulness without dominance, aim ownership, readable recoil, and material obstruction behavior. Production network/server integration is intentionally a v1.6B obligation.
