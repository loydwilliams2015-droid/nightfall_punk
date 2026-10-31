# 1.8C.2 H42 — Authoritative reader inventory and bounded joint view

## Source and epistemic limits

Parent: H41 at `3836898cdba44e857959a7e3cb7af9a3bcdfe31b`.
H42 changes a **bounded research reader interface**, not all live readers.
Acceptance is source-SHA-specific; do not infer production release readiness.

## Inspected read paths

| Path | Current source behavior | H42 migration? | Release implication |
|---|---|---|---|
| `src/shared/nf_c2_retc_frame.c` | `nf_retc_read` atomically loads last accepted frame | YES: `nf_retc_read_joint_view` acquires once and rejects incomplete B2/B3/encounter source evidence | Prototype only; lifetime needs coordinated dispose |
| `src/tests/test_v18c2_retc_real_twotick.c` | Reads accepted frame and separately compares live A6 actor/collider and B2 registry/B3 receipts | YES: test reads all accepted domains through one view, checks epochs N and N+1, and replay denial; retained N snapshot stays immutable | Real two-tick lab, not full live migration |
| `src/tests/test_v18c2_retc_frame.c` | Tests bounded publisher; source-backed B2-only candidate may publish absent encounter registry | YES: new view refuses incomplete registry instead of treating it as a valid full joint projection | Generic publisher itself remains bounded |
| `src/shared/nf_prediction.c` | Predict/reconcile advances mutable `NfWorld` | NO | Raw-world authority outside this gate |
| `src/client/main.c` | Client renders player from `nf_world_find_actor_const`, handles snapshots and prediction | NO | Presentation/camera not isolated to accepted RET-C frame |
| `src/client/nf_camera.c` | Camera tracks raw `NfActor` and `NfWorld` | NO | Third-person/first-person convergence not implemented |
| `src/server/main.c` | Server forms network snapshots from raw actor/world state; simulation and AI read `NfWorld` | NO | Network, AI and remote reader consistency remain open |

These are confirmed representative source paths, **not an exhaustive
cross-repository data-flow proof**. An additional inventory and controlled
migration are required before whole-engine atomicity may be claimed.

## H42 focused hypothesis

For an accepted, typed B2/B3 encounter-PUSH frame, one acquire-load exposes
consistent immutable physical, event, eligibility and encounter projections.
No frame, missing encounter source, or replay-denied publication must not
produce a newly accepted mixed-domain view.

H42 does **not**:
- prevent an external reader from bypassing the API;
- make independent live projections jointly atomic;
- make `nf_retc_dispose` concurrent-safe with readers;
- prove writer/reader race freedom using TSan;
- make the journal portable, directory-fsynced or production durable;
- implement VPC-01, third-person camera or new graphics.

## Evidence classes

- Positive: two consecutive real A6/B2/B3 dispatcher ticks, joint view from
  one accepted pointer for each tick and immutable retained tick-1 view.
- Negative: no accepted frame; B2-only published frame lacking encounter
  registry; rejected duplicate transaction keeps accepted view at N.
- Source exact: GCC and Clang debug build, focused CTest with preserved logs.
- Remaining: fuzz/property/held-out multi-reader stress and true full-source
  acceptance; TSAN environment runtime qualification, crash cutpoints,
  externally visible network/AI/camera reader migration.

## Release holds

Maintain H41/H42 as draft research; no 1.8C.3 or production promotion.
VPC-01 remains a *proposed* visual–physical convergence acceptance standard;
the concept art is not an in-engine screenshot.
