# M18C.2 H318/H319L–M acceptance and asset handoff

Date: 2026-11-23. Status: RESEARCH — production HOLD. Source is append-only draft PR #274; do not merge until production litmus passes.

## Baseline (confirmed GitHub CI)

- Source SHA `d0227f280bbeffc9a466628149454d70be3a14e0`: GitHub Actions 95 of 96 completed workflows passed; H318 attribution alone FAILED at the workflow's Python post-test assertion, under both GCC and Clang.
- Exact H319L source-level evidence: genuine M2 A4 actor30→crate3050→actor40 chain staged privately; both compilers/ASan/UBSan passed the H319L workflow with 1 positive and 12 forged-source denials, preserving A6 owner state. See run 44962277152.
- Dedicated H319 source grid and H55 sanitizer suites passed, including H319I/J/K parallel reference and cancellation controls. These are **not** parallel A6 actor authority and **not** a direct second actor-to-object PUSH.
- H318 actual C logs: cases 0–9 each had A4=1, dx=-800 mm, dv=-450 cm/s, projected closing=1, A5 body PENDING (2), A5 ray FREE (0), B2 reason MATERIAL_PENDING (7), and owner preserved. Post-test workflow expected A4=0 based on an old oracle before H319F; mismatch caused the red result.

## This iteration's code and test assets

1. `.github/workflows/retc-h318-reverse-attribution.yml`: update the narrow source-exact H318 assertion to the observed, now-valid A4 positive **without** relaxing B2 material-pending denial. Preserve the ten case index, displacement, velocity, A5 and H317 negative intersection checks.
2. `src/shared/nf_retc_m2_three_contact_tick.h`: H319M conservative read-only witness extension checks that each A5 private material sample magnitude agrees within bounded numeric tolerance with the corresponding authentic A4 receipt's three-dimensional normal impulse norm. Positive but synthetic magnitudes are no longer accepted.
3. `src/tests/test_v18c2_retc_m2_actor_object.c`: extend previous H319L 12 forgery denials to **14** by rejecting a forged positive A5 magnitude and a modified A4 receipt impulse; restore all source fields and verify owner nonmutation.
4. `.github/workflows/retc-h319l-two-edge-source.yml`: dedicated two-compiler (GCC/Clang), ASan+UBSan, exact-source laboratory target; assert one genuine private M2 witness, 14 forged denials, preserved owner. The test workflow is the executable acceptance asset.

## Certification matrix

| Gate | Criterion | Evidence state |
|---|---|---|
| R1 | H318 10 source-exact reverse approach traces, A4=1, A5 body PENDING, B2 reason 7 | Earlier actual logs PASS; amended workflow result PENDING |
| R2 | Genuine three-body / two-edge M2 A4→A5 private source ancestry | Baseline PASS; latest H319M stronger checks PENDING |
| R3 | 14 forgery denials including nonzero impulse forgery | Implemented; exact-source result PENDING |
| R4 | GCC/Clang ASan/UBSan M2 lab and H55 lifecycle suite | Baseline PASS; latest new-SHA outcome PENDING |
| R5 | Serial deterministic + 4 C11 immutable-reference workers (two reversed), cancellation joins | Baseline PASS; not a production parallel actor grant |
| P1 | Both actor10 and actor20 issue real independently directed actor→object2010 PUSH contacts, A5 sweep FREE | OPEN |
| P2 | Two distinct current A6-issued B2 grants consumed once, stale/revoked/replay denied | OPEN |
| P3 | Two APPLIED typed B3 consequences and exactly one indivisible A6 accepted frame, rollback on failure | OPEN |
| P4 | Canonical/randomized/pathological/held-out, fault, sanitizer, **true concurrent actor** deterministic source-exact final audit | OPEN |

## Next tasks, in order

1. Wait for **actual finished** H318 and H319M exact-SHA CI before marking R1–R4 PASS. Distinguish workflows queued from executed; inspect actual job logs if red.
2. Use `src/shared/nf_retc_m2_three_contact_tick.h` as an actor/object physical ancestry reference, but do not conflate the object→rival contact edge with a second independently initiated actor→object PUSH action.
3. Construct a separate same-tick two-actor-to-object source scene, prove both A4 directed contacts and A5 FREE swept paths before any B2 authorization.
4. Bind independently current owner-issued B2 grants, then two typed B3 realizations, then one all-or-none A6 immutable accepted frame with explicit negative fault/replay tests.
5. Run independent GCC/Clang, ASan/UBSan, true multi-actor scheduling, held-out and source-exact acceptance.

Distance remains high uncertainty; previous 7–14 substantial five-objective C2 sweeps is only a planning envelope. M18C.2 production certification remains HOLD. M18C.3 durability promotion remains HOLD.


## H319N roster and intent source-coherence amendment — 2026-11-23

### Why this exists
An A4 receipt and matching A5 sample can be internally consistent while the live actors have moved or changed their intents since the A6 tick that produced the private contact. Source-epoch equality alone does not exclude such a stale candidate.

### Implementation and test assets
- `src/shared/nf_retc_m2_three_contact_tick.h`: stage the exact immutable `NfMa03Roster` during original A6 owner tick; after the tick rejects privately, re-capture the authoritative live roster and compare actor identities, XYZ positions, XYZ velocities, and the full interaction/movement input fields individually. Reject any divergence; no world mutation.
- `src/tests/test_v18c2_retc_m2_actor_object.c`: expand source-witness falsifications to 18 by temporarily changing (then restoring) the rival's live position, rival velocity, owner interaction hold, and a captured roster position. Positive restaging after all controls is required. No real B2 grant, B3 effect or A6 publication is performed.
- `.github/workflows/retc-h319l-two-edge-source.yml`: source-exact GCC and Clang, ASan/UBSan enabled, must assert `H319L M2_TWO_EDGE_SOURCE positive=1 forged_denied=18 owner_preserved=1` before accepting the research milestone.
- H318 reverse-motion source diagnostics remain separate; A4=1 coupled with A5 body PENDING and B2 MATERIAL_PENDING is an intentional fail-closed outcome, not PUSH authorization.

### Acceptance and limitations
R1: 18 distinct negative controls plus authentic positive recheck pass on GCC and Clang at the **same exact SHA**, with sanitizer success and no authority mutation. Status: **PENDING** until CI job logs verified.
R2: The source roster must be rechecked after the candidate is created and before considering any subsequent permission stage. Status: implemented research gate, final production integration OPEN.
P1–P4: Both independently directed actor→object A4+ A5 FREE, owner-issued B2 grants, two APPLIED B3 effects, and single atomic A6 immutable publication remain OPEN, as do real concurrent actor scheduling and independent held-out acceptance.
