# H319O/P — two independently directed source-world A4 actor-to-object contacts

Status: **research-only, execution pending**. Date: 2026-11-23. Draft PR #274; do not merge or publish gameplay outcomes.

## Scientific question

Can the engine obtain *two source-real, independently initiated* A4 capsule→dynamic crate CCD receipts from one A6-owned world tick with actors approaching the SAME object from opposite sides?

The preceding H238–H319N stage proves a different contact graph: actor30→crate3050→rival40, where the rival receives an object→rival impulse but does not independently initiate its own PUSH. H319O/P instead evaluates two actual actor→crate A4 contacts, each using an independent copy of the same real source object snapshot.

## Prepared code and test assets

- `src/shared/nf_retc_m2_dual_directed_probe.h`: private, source-bound research probe. Requires A6 tick owner, source roster count two, live collider matching the real object, two active living actors with interaction intent and velocities toward opposite object faces, and real accepted applied swept A4 pair receipts with the same object ID and source revision. Returns two private pair results only. Does not write to the world.
- `src/tests/test_v18c2_retc_m2_actor_object.c`: exact-source owner-tick fixture; set rival actor40 on the right face with negative-X approach and interaction intent, owner actor30 closing from left; require two real A4 CCD receipts for crate3050 and original owner tick, world revision, A5 history, receipt count unchanged. Reject rival's withdrawn intent and outward velocity; require positive again on restoration.
- `.github/workflows/retc-h319op-dual-directed.yml`: source-exact GCC and Clang ASan/UBSan compilation and actual execution. Strictly assert `H319O DUAL_DIRECTED private_pairs=2 actor_ids=30,40 object=3050 owner_preserved=1` and `H319P DUAL_DIRECTED_DENIAL cases=2 restored=1 owner_preserved=1`.

## Evidence categories

1. **Real source**: A6 owner tick, live actor IDs and input, live object collision geometry, current source-world revision.
2. **Real physics**: two independent A4 swept contact solvers against an identical immutable crate starting state.
3. **Still unproven**: a single jointly integrated three-body solver at simultaneous time of impact, two genuine A5 FREE swept-volume paths, two A6 owner-issued current/once-consumed B2 grants, two typed B3 APPLIED effects, one indivisible accepted immutable A6 frame, durability, and independent deterministic serial/parallel scheduling.
4. **Safety invariant**: a pairwise positive result is NOT an authorized multi-actor PUSH, never publishes, and must not be reported as production acceptance.

## Promotion gates

R1: Both GCC and Clang source-exact sanitized H319O/P jobs complete successfully with two authentic directed A4 receipts and both fail-closed controls. **PENDING**.

R2: A coupled three-body solver receives both contacts and produces deterministic, contact-order-invariant shared-object motion without overlap or double application. **OPEN**.

R3: Static A5 swept clearance separately FREE for both actor trajectories; stale, occluded or PENDING must fail closed. **OPEN**.

R4: B2 grants independently owner-issued and consumed once; two typed B3 PUSH effects applied; exactly one all-or-none A6 accepted frame with rollback on failure. **OPEN**.

R5: GCC, Clang, ASan, UBSan, thread sanitizer where feasible, canonical/randomized/pathological/held-out and true concurrent authoritative actor scheduling all pass at same exact SHA. **OPEN**.

Do not promote M18C.2 or M18C.3 until R1–R5 plus independent source review pass. Native playability P0/P1/P2 has not been measured.


## H319Q — co-arrival and private three-body island extension (2026-11-23)

**Purpose:** two actor→crate CCD receipts from one source object snapshot do not establish simultaneous contact. H319Q reads the actual two pairwise A4 times of impact and *refuses* coupled acceptance when their normalized arrival times differ by more than 0.0001. The discrepancy is reported as a measured research finding, not silently coerced into a manifold.

**Implementation:** `src/shared/nf_retc_m2_dual_coupled_probe.h` uses the same engine A4 island solver with two actor→crate constraints and the original, immutable pre-contact actor and crate bodies preserved by `nf_retc_m2_dual_directed_probe.h`. It advances the original bodies to the common time of impact only after co-arrival validation. It compares forward versus reversed constraint order on independent private copies, requiring matching positions/velocities (0.003 tolerance). A failed island solution remains unaccepted, with no world mutation.

**Tests:** `src/tests/test_v18c2_retc_m2_actor_object.c` records a two-pair H319Q feasibility observation, requires no acceptance unless times are coincident and order-equivalent, denies forged `staged=false`, then restores the source and checks A6 tick/history/receipts are unchanged.

**Executable asset:** `.github/workflows/retc-h319op-dual-directed.yml`, GCC and Clang with ASan/UBSan, source-exact. The CI log parser requires H319Q source, co-arrival, coupling invariants and restored negative control. **No successful execution is yet claimed**; CI may report a timing mismatch as an appropriately fail-closed result.

**Still open:** a positive synchronized coupled three-body contact, both independent A5 FREE sweeps, A6-owner-issued B2 authorization, B3 application, one indivisible accepted frame, fault/replay/revocation/true concurrent authoritative and held-out acceptance. Do not confuse private island diagnostics with production PASS.
