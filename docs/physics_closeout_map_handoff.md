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
