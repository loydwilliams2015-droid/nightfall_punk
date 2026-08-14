# nightfall!punk — v1.2b 100-Question Sweep Closeout

Date: 2026-08-13
Branch: `design/engine-autorecommendation-contracts`

User disposition: **KEEP ALL**.

## Final resolutions

- **Q151 — KEEP E + C:** explicit Perceptual FOV Contract + conventional user-facing FOV setting/conversion. Preserve current perceptual baseline for comparison rather than preserving an ambiguous naked `82`.
- **Q217 — KEEP E + C:** conventional separate first-person viewmodel/world representation with one causal authoritative weapon state. `WeaponTruth != WeaponPresentation`; one cause may have multiple legitimate presentations.
- **Q224 — KEEP E + C:** Embodied Visibility Envelope + practical hybrid first-person representation: authoritative full body remains materially whole, while first-person selectively presents dedicated hands/weapon, useful lower-body visibility, and world-consistent presence without forcing problematic head/upper-body geometry through the camera.

## Sweep status

Q147–Q246 = **100/100 dispositioned**.

**Open questions remaining inside the v1.2b 100-question sweep: 0.**

Exact constants, thresholds, amplitudes, FOV default, viewmodel offsets, body visibility cutoffs, and other feel parameters remain `AUTO KEEP + MEASURE`; they are empirical tuning tasks, not unresolved architecture questions.

## Broader deferred frontier

Five broader engine topics remain intentionally deferred outside the bounded v1.2b sweep and should not be counted as unfinished v1.2b questions unless a future build materially consumes them:

1. degree/scope of destructibility;
2. active-ragdoll scope;
3. fluid fidelity beyond current sparse/local-state approach;
4. broader multi-actor force exchange;
5. later parity/difference among Player Crew, Human Rivals, and Dream Cattlers under the new body/physics contracts.

Project rule remains:

`AcceptedContracts -> Deductive/InductiveRecommendation -> SmallestCompatibleImplementation -> Measure`

and only materially important mid/high uncertainty should trigger the next question subroutine.
