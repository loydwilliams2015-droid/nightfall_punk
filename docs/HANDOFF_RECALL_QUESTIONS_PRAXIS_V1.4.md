# nightfall!punk — Recall Questions / PRAXIS Handoff

Date: 2026-08-14
Status: **QUESTION/RESEARCH HANDOFF — DO NOT COMPILE**

This handoff gives the Recall Questions PRAXIS context the same source-of-truth as the main PRAXIS lane.

## Source of truth

Latest green gameplay/code proof: v1.2b head `edf645ab7e434679d78be88708bddbe9789419c4`.

Later branch commits are documentation/design only unless separately proven by code/CI.

Current design source:

- `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`
- `docs/HANDOFF_PRAXIS_V1.4_PREBUILD.md`

Do not treat historical `Fuzzy Rail` terminology as current. Canonical term is **Snap Locomotion / `snap_locomotion`**.

## Next-build Recall Questions focus

### Track 1 — Combat Systems Compare-10

Recall Questions should help select, interrogate, and compare ten combat references/mechanisms against current nightfall—not ask for a feature wishlist.

Use these ten lenses:

1. authoritative hit/damage grammar;
2. weapon-role ecology;
3. lethality and error consequence;
4. recoil/spread/recovery/handling;
5. cover/exposure/firing lanes;
6. suppression/flank/pressure interaction;
7. ammunition/reload/switching rhythm;
8. movement-combat coupling;
9. enemy-composition and encounter rhythm;
10. aftermath/persistence: contamination, STASIS, inventory, traces, route/resource effects.

For every comparison ask:

- What is the mechanism, not merely the feature?
- What already exists in nightfall?
- What is genuinely missing?
- Does the lesson improve the causal grammar or merely increase content count?
- Is the answer KEEP / ADAPT / DEFER / REJECT?
- What is the smallest C-level consequence that a bounded human test can prove?

Preserve the high-degree combat core:

`intent -> validation -> aim reconstruction -> rewind/occlusion/hit test -> relationship permission -> authoritative damage/contamination/STASIS -> event/snapshot`.

### Track 2 — AI Communications

Use communications as a game-theoretic incomplete-information/signaling problem.

Current accepted rules:

- firsthand evidence > relayed report;
- reports have provenance/confidence/age/scope;
- source ancestry matters;
- repetition is not independent corroboration;
- bounded latency and confidence degradation are desirable;
- truthful-but-incomplete reporting precedes deliberate deception;
- exact unseen transforms are never injected by communication;
- public/common signals must be materially/publicly available rather than silent omniscient blackboard synchronization.

Recall Questions should interrogate:

1. reportable content by faction/actor type;
2. granularity and compression;
3. latency;
4. confidence loss;
5. provenance and trust weighting;
6. private/shared/public/common-knowledge distinctions;
7. communication-channel topography versus communication-network topology;
8. failure/interruption;
9. combat/map-control consequences;
10. deterministic positive/negative proof.

## Spatial/economic vocabulary guardrail

- **Topography:** geographic/metric distribution—distance, elevation, density, exposure, gradients, spatial access.
- **Topology:** connectivity/exchange/conversion/dependency structure—what relates/exchanges with what and through which paths.
- **8 m x 8 m:** base Southern-California geographic grid element.
- Larger simulation/network regions/cells are aggregations of many base elements.

## Question discipline

Use the dialectical prompting sequence:

`ZERO-FAMILIARITY PROBLEM -> STRONGEST PRECEDENT -> ACTUAL NIGHTFALL STATE -> NEGATION/COUNTEREXAMPLE -> SYNTHESIS`.

Automatically recommend when an answer is deductively forced or inductively high-confidence and reversible. Ask the user only where materially different good designs remain.

Do not compile, branch into gameplay work, or broaden the feature set from Recall Questions. The role is to sharpen the next implementation contract before PRAXIS codes it.

## Handoff proposition

> **The next build should deepen how actors fight and how they communicate about the fight, without weakening the authority, epistemic, movement, or persistence contracts already proven by v1.2b.**
