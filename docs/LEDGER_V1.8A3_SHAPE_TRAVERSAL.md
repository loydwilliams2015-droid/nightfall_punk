# nightfall!punk 1.8A.3 — Shape & Traversal: H1 dual-jurisdiction exit candidate

## Source provenance / status

A complete strict-C 1.8A.3 source-and-evidence package was built and tested in the current work session, separately from this branch. **This GitHub branch records the design, experiments, and handoff; the tested 1.8A.3 C source has not yet been grafted into this branch.** Do not claim GitHub CI has certified the new code. Install the supplied source package over the existing 1.8A.2 working tree for local reproduction.

## Conditional a priori design law

Physical passage and actor traversal permission are independent, simultaneous jurisdictions. A traversal is `commit_eligible = geometry_approved && contract_approved`; the actual authoritative world commit remains a separate transaction. Neither actor permission without clearance nor clearance without authority suffices.

The authoritative traversal snapshot must bind actor ID, feature ID, tick, world revision, authoritative initial pose, body radius/height/foot patch, grounded/ladder state, support surface, step limit, reach and minimum support/facing rules. The actual capsule shape is not replaced when using rounded-box-like support logic. Ladder affordances are triggers, not automatically rigid surfaces.

## Tested candidate families

0. Swept upright box + two-jurisdiction gate (historical control).
1. Swept upright capsule + two-jurisdiction gate (strict capsule control).
2. Swept capsule + independently validated flat foot-support patch + two-jurisdiction gate (**selected H1 laboratory exit candidate**).
3. Permissive capsule climbing ignoring height limit (negative control, test-only).
4. Geometry-only contract bypass (negative control, test-only).

Standalone strict-C local test: 38/38 targeted assertions; separate production API gate passed. Historical A1: 27/27; A2: 46/46. In 4,000 distinct matched world/input fixtures, model errors were 250, 0, 0, 250, and 1,500 respectively. In 1,200 additional held-out foot-support fixtures, errors were 254, 254, 0, 254, and 0. The negative contract bypass is inadmissible despite its near-edge geometry. Independent numerical reference of 2,400 translating capsule/AABB trajectories: 890 true hits, 1,510 true misses, 0 false hits, 0 false misses, maximum normalized TOI discrepancy 2.977e-8.

These are stratified H1 lab fixtures, not population-weighted estimates or full-gameplay/H3 evidence. Flat-support acceptance is explicitly conditional on the chosen 0.10 m foot support patch.

## Required next steps

- Source graft and full GitHub CI/cumulative CTest verification.
- Integrate motor-driven dynamic body, reciprocal linear/angular impulses and physical realization of approved step paths; eliminate any unaccounted step teleport.
- Integrate authoritative Cell/Nexus support, world versioning, actual chunk geometry and persistent contact history.
- Character/character moving platform, slope, arbitrary mesh, rotation, near-parallel and manifold pathological corpora.
- Linux FPS camera/locomotion and network prediction/reconciliation tests with human observers.

## Production guard

Unsafe controls compile only under `NF18A3_TEST_CONTROLS` in test builds; default APIs reject them. The C contract struct must originate from server authority—it is not a cryptographically authenticated capability if supplied by an untrusted client. Diagnostic hashes are not security signatures.

## Industry precedents

PhysX documents capsule climb-over beyond nominal step offset. Rapier and Jolt separate obstacle navigation, stair stepping, floor attachment and character constraints. They are comparison baselines, not reasons to treat our material/semantic dual jurisdiction as already proven in gameplay.
