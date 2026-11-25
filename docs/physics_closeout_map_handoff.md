# Physics closeout — map-first administrative handoff

Policy: provisional-c2-demo-v1. Scope: development prioritization only; not a constitutional amendment or production release. User-directed five-round closeout followed by map-first operations.

## Verified source and evidence

Source SHA c657a3bc20374a5e2911d3ce3e6bbbf44ea12c20: 51 reported PR workflows succeeded, zero reported failures. Dedicated closeout run https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/45529322360 completed successfully on GCC and Clang. Logs independently inspected.

| Round | GCC | Clang | Evidence per compiler |
|---|---|---|---|
| 1 | PASS | PASS | 4 named CTests |
| 2 | PASS | PASS | 5 standalone laboratories; shared contact 6 accepted, 14 pending; 4 refusal controls |
| 3 | PASS | PASS | 5 named CTests |
| 4 | PASS | PASS | 4 named CTests |
| 5 | PASS | PASS | 128 independent first-tick samples; 18 evidence-schema unit tests |

Five rounds are five executed suites, not five separate source revisions or five independent production releases. Both compilers emitted the clean tracked source full-round handoff-review marker. No source failure was observed; no physics correction required on this baseline.

| Timing | GCC | Clang |
|---|---:|---:|
| Tick p50 ns | 8142 | 8032 |
| Tick p95 ns | 8293 | 8423 |
| Tick p99 ns | 22383 | 22243 |
| Tick max ns | 90384 | 88754 |
| Selective queries | 128 | 128 |
| Eager queries | 1024 | 1024 |
| Selective total ns | 22659 | 17389 |
| Eager total ns | 28130 | 32497 |

Matched relevant A5 point evidence; 87.5% fewer inspection queries. Not proof of full physical equivalence, steady-state latency, full memory savings, or gameplay performance. Shared-crate positive cases are low-speed and not owner-authorized. The older synthetic island remained 0/20 accepted. Preserve unresolved outcomes.

## Administrative decision

Map-first engineering may commence on the verified headless baseline. Physics moves to maintenance, with only map-blocking regressions and safety/authority defects receiving priority. This is the development handoff directed by the user, not production P* acceptance or native playability certification. No merge or release approval inferred.

- A6 retains sole authoritative tick publication. Map and render receive accepted read-only state; geometry proposals are inputs to authority, not independent world writes.
- Retain the runtime governor's existing bounded delegation; never use optional-work budget as authorization.
- Advanced shared-object interactions, dual B2/B3 transaction and atomic A6 publication remain OPEN under M18C.2. Unsupported demo actions must remain disabled or refused; no false successful effects.
- M18C.3 recovery remains separate and OPEN. No durable transaction claim.
- Preserve source evidence, dissent/review and human release authority. No new runtime constitutional mechanism asserted.

## Map first work package

1. Inventory existing map sources and assets and choose a single greybox incumbent; do not invent another engine.
2. Establish static geometry and collision correspondence from the same authored map source. Verify bounds, spawn clearance, floors, walls and simple traversal.
3. Connect read-only map projection to accepted A6 tick and source epoch; reject stale or unsupported geometry. Do not allow presentation to mutate authority.
4. Run a native Linux demo with movement, camera and safe exit; record actual frame telemetry and faults. Headless closeout alone does not certify this.
5. Re-run physics closeout only when physics/authority interface changes or a blocking regression occurs; otherwise prioritize map construction.

## Checklist per physics compile

Run `CC=gcc bash tools/physics_closeout.sh all` and `CC=clang bash tools/physics_closeout.sh all` for baseline changes; individual rounds for local iteration. Record commit, compiler, tracked worktree provenance, exact selected CTest count, test outcomes, unresolved cases and log. Full clean-source results permit handoff review; individual rounds do not. Untracked source and native graphics require separate review. Preserve last known-good source revision and artifacts. Never promote partial evidence to production acceptance.

Status: five-round automated closeout PASS on cited SHA; MAP-FIRST DEVELOPMENT; native P-DEMO UNVERIFIED; M18C.2 HOLD; M18C.3 OPEN.


## Append-only design amendment MAP-D01 — 2026-11-24

User-selected near-term map programme. This amendment supplements the preceding handoff; historical evidence and production HOLDs are unchanged.

| ID | Decision (user-selected, verbatim) | Implementation jurisdiction |
|---|---|---|
| 01 | C — Central arena, progressively expanding | Develop central incumbent first; retain existing world bounds; expand only after local acceptance. |
| 02 | D — Hybrid industrial arena | Combine arena circulation and industrial architectural modules within existing geometry. |
| 03 | B — Moderate ramps and elevated platforms | Reuse existing ramp/platform assets; verify visual and collision correspondence. |
| 04 | D — Hybrid, favoring biopunk industrial | Guide module silhouettes and later selective lighting; no new simulation mechanics implied. |
| 05 | B — Handcrafted with reusable modules | Author a bounded module inventory from the incumbent; no procedural generator requirement. |
| 06 | D — Landmarks with optional guidance | Establish recognizable route landmarks before optional guidance UI. |
| 07 | C — Advanced concurrent physical interactions | Adopt as a map gameplay requirement. Integrate only already verified authoritative capabilities; isolate unaccepted interactions behind existing fail-closed boundaries. |
| 08 | D — Consequential regions active; dormant summaries elsewhere | Reuse bounded frontier research; no permission or clearance inferred from scheduling; preserve canonical state. |
| 09 | D — Geometry first, then selective lighting | Require geometry/collision parity before lighting and atmospheric polish. |
| 10 | D — Objective-based demo with measured performance and safe boundaries | Require native objective, movement, safe exit, boundary checks and measured frame timing. |

### Ordered near-term implementation

- MAP-01: verify native screenshot correction and inspect three captures; inventory central collision and traversal assets.
- MAP-02: establish handcrafted modular central arena with industrial landmarks, moderate vertical routes and matching collision geometry.
- MAP-03: integrate existing accepted A6 reader interfaces; design concurrent interaction stations using existing capabilities; keep unsupported interactions unavailable or explicitly refused.
- MAP-04: apply consequential-region scheduling only where existing evidence supports it; verify dormant summaries do not confer authority or false clearance.
- MAP-05: add a bounded objective, optional route guidance and selective lighting after geometry verification; run native performance and safety acceptance.

These are ordered work packages, not completed implementations. No additional engine, world expansion, procedural generation, or new physics subsystem is authorized by this amendment. Concurrent interaction is explicitly in scope, but full M18C.2 production joint B2/B3/A6 acceptance remains HOLD and M18C.3 durability OPEN. Map-first operations continue; only map-blocking physical defects or necessary existing-interface integrations reopen physics maintenance. Demo acceptance must report which concurrent interaction capabilities actually execute; no simulated success or substitution for production evidence.

A6 remains sole tick publisher; map geometry proposals and optional guidance have no independent authority. Existing governor admission is computational delegation, not authorization. Human review and release ratification remain separate.

Proposed implementation ordering above is engineering interpretation of the ten selections; the decisions themselves are user-authorized. No code, map geometry, or screenshot certification changes in this amendment.
