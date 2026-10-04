# nightfall!punk 1.8A.6 — unweighted embodiment-model PAR

**Scope:** opt-in grounded actor + one translating dynamic AABB + canonical material + contact-history + existing camera, offline headless H1. **Not** a full FPS client, networked authoritative server, AI scheduler, or complete stair/ladder/crouch/jump integration.

**Sampling unit:** six deliberately selected scene classes × 200 independent parameter seeds = **1,200 independent world/input configurations**; each is replayed across four model controls, making **4,800 matched model rows**. Seed perturbations vary crate location. Within each row, up to 45 ticks are *dependent temporal observations*, not independent trials. Model 1 is limited to one tick because it deliberately never commits NfActor. Cases 100–199 within each scene are an ex ante parameter hold-out, **not** unseen scene types.

## Unweighted policy comparison

| Model | Final status distribution (committed / blocked / pending / unsupported) | World actor moved | Camera updated | Applied contact history | Crate response | Mean successful ticks |
|---|---|---:|---:|---:|---:|---:|
| 0 — Legacy movement (negative material control) | 400/200/0/600 | 400/1200 | 0/1200 | 0/1200 | 0/1200 | 15.00 |
| 1 — A5 lab-only (negative embodiment control) | 200/200/200/600 | 0/1200 | 0/1200 | 0/1200 | 0/1200 | 0.17 |
| 2 — Physical world bridge | 0/200/400/600 | 200/1200 | 0/1200 | 200/1200 | 200/1200 | 6.33 |
| 3 — Physical world bridge + camera | 0/200/400/600 | 200/1200 | 200/1200 | 200/1200 | 200/1200 | 6.33 |

**Paired physical-authority invariant:** Model 2 versus Model 3 produced **0/1,200** mismatches over all compared physical fields, including the authoritative result hash. This establishes that the existing read-only camera follower does not modify physical authority in the sampled fixtures; it does not demonstrate actual rendered frame-time or input feel.

**Scene strata, each 200 independent parameter seeds:** 0: loaded static canonical corridor, motor pushes real crate, both integrated models eventually PENDING at edge of loaded canonical world; 1: existing world obstacle despite free canonical voxels -> BLOCKED; 2: missing authoritative canonical chunk -> PENDING for integrated models, but historical reference continues -> negative material-coverage control; 3: ramp -> UNSUPPORTED; 4: nearby actor -> UNSUPPORTED; 5: moving platform -> UNSUPPORTED. These last three are open integration deficits rather than PASS for full embodiment.

**Hard H1 PAR (pass/fail):** No sampled model 2/3 clearance from absent canonical material; no sampled false clearance from existing world collider; every recorded world-bridge crate response is backed by applied contact-history samples; NfActor projection agrees with authority snapshots; camera cannot modify authority (0/1,200). Tests E01–E54 separately cover selected stale tick, rollback-on-WAL-failure and recovery. No claim of zero problems beyond this declared corpus.

**Graded PAR:** mean committed ticks and counts above; memory, P95/P99 loading latency, CPU and wall time, human feel and network reconciliation **NOT MEASURED** in the controlled comparison. The 45-tick loaded corridor intentionally becomes PENDING before completion, demonstrating incomplete canonical world streaming. The old controller making progress without canonical data is not considered authoritative correctness.

**Selection:** Model 3 (WORLD_CAMERA) is the strongest **conditional H1 grounded embodiment bridge** because it includes verified actor projection, dynamic object response, physical contact history, and camera correspondence without additional physics divergence. Model 2 remains the nonpresentation comparator; Model 0 remains legacy control; Model 1 documents false embodiment claims if world projection is omitted. **Do not certify a full A6 gameplay exit:** the present API deliberately rejects jump/crouch/ladder/ramp/moving-platform and near actor interactions and runs outside ordinary world scheduling.

## Scientific method and accountability

- A priori (conditional): authoritative history requires applied, committed material contact; visual observation cannot cause world state; unknown canonical geometry cannot imply free passage; geometry AND traversal jurisdiction remain necessary for genuine traversal.
- Quine: pending may arise from canonical coverage, actor constraints, contact-budget convergence or bad test fixtures; diagnose causes individually rather than attributing every pending to collision failure.
- Kuhn: the historical control and new source of material authority disagree in the absent-canonical stratum—an explicit anomaly to resolve before architectural promotion.
- Lakatos: the selected bridge predicts matching world/camera hashes while additionally preserving applied impulses and history; further held-out game genres of movement required.
- Feyerabend: four distinct control policies remain reproducible, rather than erasing the incumbent.
- Popper: future severe tests are low ceilings, ladder transitions, multi-actor same-tick ownership, moving supports, physics stacks, unsampled chunk edges, client loss/jitter and corrupted world WAL.

## Reproducibility

```sh
bash ./v18a6.sh all
```

CSV SHA-256: `a54596fb7836f5dbad6dcf1cadb4f38ae19eee2407188bd45f67d8e9897d8459`. This is exact reproduction on this machine/compiler only; floating-point cross-platform identical hashes are unverified.

**Source-provenance requirement:** test C code and analysis must be attached to the same GitHub commit/CI run before claiming GitHub CI results; an independently reviewed PR is required for release promotion.
