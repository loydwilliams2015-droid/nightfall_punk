# nightfall!punk v1.8A.6 — terminal 1.8A sweep and accountability ledger

## Decision chain and evidence status

1. A.1 contact TOI H1, A.2 capsule/motor/hist policy H1, A.3 **Model C** capsule + verified foot patch + `geometry AND traversal`, A.4 **Model E** motor-driven reciprocal/angular/friction + bounded adaptive solver, A.5 **Model D** authoritative canonical/fine cache and **Model D** cumulative contact-to-durable history, all retained at prior H1 grades.
2. A.6 conditional **MODEL 3** selected for the *grounded, one-actor/one-crate headless embodiment bridge*, not for production gameplay. It couples actual `NfWorld` and `NfActor` state, applied dynamic impulse, material validation, condensed history, and existing `nf_camera` presentation. **Model 2** is retained as the physics-only comparator; legacy and lab-only are negative controls.
3. The full A.6 **production FPS exit remains UNMET**: no jump/crouch/vault/mantle/ladder execution, moving platform, ramp, general actor-actor island, canonical streaming outside loaded chunks, real multi-actor world stepping, AI lifecycle, weapon parity, ENet client/server or raylib frame, cross-platform deterministic sim, H3 hardware latency nor H4 human player evidence. Do not silently relabel an H1 grounded bridge as final production readiness.

## A priori conditional invariants

- Material geometry `G` AND authorized traversal `T` are necessary for any special traversal. The selected grounded subset never stages an unverified special move.
- No `nf_world_step()` may update the bridged actor during the same authoritative tick. World tick and A5 tick must agree before staging; stale/contradictory state must fail closed.
- Every positive dynamic-contact-history sample originates in applied body impulses and a successful commit; no estimated receipt is history.
- Unknown canonical material does not constitute free volume; existing world colliders are independent material witnesses, not overridden by a free voxel.
- A read-only camera update cannot modify the authoritative physics snapshot. The recorded physics hashes between models 2/3 must match on matched seeds.
- A failed journal or an explicitly unsupported nearby actor/platform/ramp must not advance the authoritative actor.

## Controlled comparison

`src/tests/sample_v18a6_embodiment.c` generates six specific adversarial strata × 200 parameter seeds × four models = 4,800 dependent model rows from 1,200 world configurations. The 200 seeded worlds per stratum are **not** random FPS sessions. Models 2/3 show zero physics divergence in 1,200 matched configurations, 200 successful dynamic/history corridor cases, correct block and pending negatives, and honest unsupported outcomes for absent mechanics. The historical controller does not cover canonical authority. See immutable local CSV and `evidence/v18a6/PAR_RESULTS.md` for exact values.

`src/tests/test_v18a6_embodiment.c`: 54 named assertions, including world actor/camera correspondence, blocker and unknown canonical gates, direct and recovered snapshot consistency, journaling failure rollback, actual applied crate contact and history, and no unsupported movement being pretended. Strict GCC/Clang offline CTest and GCC ASan/UBSan additionally performed locally; record real GitHub CI result separately after workflow execution. These remain **laboratory tests**.

## Model adjudication (unweighted)

- Legacy controller: actual actor motion, no new canonical truth or dynamic crate/history, and no selected camera update. Retain for feel/performance ablations and as a deliberately negative material-control case.
- A5 laboratory-only: actual A5 contact component, no `NfActor` movement or camera. Demonstrates component success is not embodiment.
- World bridge: actor material authority + dynamic crate + history, no presentation.
- World + camera bridge: same authoritative physics and contact history as bridge (zero recorded hash divergence) plus camera follower. **Selected conditional H1 candidate.**

Selection is based on admissibility + demonstrated additional functions, **not** a weighted global score. The selected bridge does not defeat alternatives on unsupported subgenres of movement. Future selection needs full world path rather than increased resampling of the same fixtures.

## Scope of GitHub tidy work

- Append new source & strict test/sample registration instead of replacing earlier collision kernels; retain historical branches.
- Record scenario seeds, CSV SHA-256, compiler, offline CMake flags, test logs and known missing features.
- Ensure normal/negative control build separation from A.3 and A.5 experiments; never make unsafe controls runtime behavior.
- Clang build fixed three preexisting missing terminal newlines in `src/shared/nf_cleave17d.{c,h}` and `src/shared/nf_observe17e.c` without functional change.
- Review on a dedicated branch/PR against the actual source parent; do not force-push `main`. The existing source-reconciliation PR #29 and architecture issue #31 are parent references, not asserted merged.

## Required follow-on work (explicitly outside proved A.6 H1)

- Multi-actor contacts and authoritative whole-world tick arbitration; both physics islands and semantic contracts on distinct causal graphs.
- Actual 1.8A.3 stair/ladder clearance integrated with **physically realized** A.4 bounded motor motion and authentic support states.
- Canonical chunk-stream prefetch and latency profiles (loaded parent/fine), multi-body/moving support, generalized geometry and structural revisions.
- True client input, camera rendering, weapon ray/collision provenance, network prediction/reconciliation, durable interoperable WAL.
- H3 native full-engine profiling and H4 player testing including p50/p95/p99 motion/IO overhead and normal/best/worst/pathological seeds.

## Scientific review protocol

Quine: failure can originate in canonical coverage, shape, transaction assumptions or test fixture; isolate. Kuhn: legacy controller's canonical omissions are a measurable anomaly, not automatically defective UX. Lakatos: the bridge predicts new committed dynamic evidence and camera noninterference without changing the hard core of material truth. Feyerabend: keep incumbent and staged controls. Popper: target false clearance, duplicate commits, moving-platform/actor overlaps and unsupported controls. Publish failures and repaired errors without overwriting prior records.
