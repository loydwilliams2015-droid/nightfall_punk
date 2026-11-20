# RET-C H319–H322 / Strategy D: source-exact two-actor PUSH implementation and acceptance plan
Date: 2026-11-20. **Planning artifact only. No source implementation or tests claimed.**
Baseline exact commit: `a465670e612b61a82b073acdbc68c758e1528bf0` (draft H318 PR #271). The BRIO prefilter PR #272 is a separate research line, not a credential or substitute for A4/A5/B2/A6 admission.
Decision: **D = A (minimal genuine positive PUSH) followed by B (adversarial falsification), while staging C (durability handoff) with no premature C3 promotion. Positive-path first, negative gates mandatory before acceptance.**

## 0. Source map and non-negotiable invariants
- `src/shared/nf_contact18a4.h`: `nf18a4_pair_step`, `nf18a4_apply_contact`, `nf18a4_solve_island`; actual finite contact receipt must show `NF18A4_ACCEPTED`, impulses applied, nonzero contact identity, same tick/version. A4 receipts are **not** world commits.
- `src/shared/nf_contact18a5.h`: `nf18a5_query` FREE/SOLID/PENDING/INVALID; material `global_revision` and integrated/history staging. Both source actors' swept material paths must be FREE at one authoritative material epoch; PENDING is not clearance. Never use `NF18A5_UNSAFE_FINE_CONTROL` or manufacture FREE.
- `src/shared/nf_encounter18b2.h`: `nf18b2_live_frame`, `nf18b2_propose`, `nf18b2_encounter_prepare_joint`, `nf18b2_encounter_validate_joint`; actor IDs from authenticated source; owner-current independently evaluated PUSH grants; stale, revoked, expired, consumed, mismatch prohibited. `nf18b2_attach` must not install a second owner.
- `src/shared/nf_interact18b3.h`: `NF18B3_PUSH`, `Nf18b3Event`, `nf18b3_authorize`, `nf18b3_realize`, `nf18b3_interrupt`; do **not** treat event flags or untrusted callbacks as proved material effect. B3 receipt requires an actually applied source-backed consequence.
- `src/shared/nf_embody18a6.h`: `Nf18a6OwnerHooks.begin/prepublish/accept_candidate/stage_joint/finish`, `Nf18a6Runtime.retc_pending/retc_publisher`, `joint_publication_active`, `joint_commit_epoch` and `nf18a6_step`; A6 remains sole tick/commit/publish owner. Accepted immutable frame is the only public state; publication cannot precede full projection and validation. Respect auth lifecycle lock, world lease and shutdown.
- `src/shared/nf_retc_h310_owner_joint_probe.h`: stage-by-stage *read-only* diagnostic stops, useful for first failed-boundary attribution but **not** a grant, side-effect or commit.
- `src/shared/nf_v18c2_a6_stage.h`: explicit warning `b2_granted` is merely an H3 diagnostic boolean, **not** a production authorization credential. Never pass this flag as proof of grant.
- `docs/retc_h311_h320_positive_physics_report.md`, `docs/retc_h316_h325_visibility_audit.md`, `docs/retc_h318_h327_reverse_stage_audit.md` define prior contact, occlusion and pending failures.
- Existing H318 observed reverse-X A4 contacts **0/10**, actor10 B2 ELIGIBLE, actor20 B2 MATERIAL_PENDING; H315 positive A4 lateral contacts had actor20 B2 OCCLUDED. Neither observation constitutes an implementation defect until contrary source evidence emerges.

## 1. Minimal genuine scene; no policy relaxations
Use actual A6-controlled `NfWorld` with source actor IDs **10 and 20**, contested target **object 2010**, `NF18B2_PUSH` / `NF18B3_PUSH` for *both* independently authenticated intents. The contested action is a PUSH by each actor against one object; each must receive its **own** typed B3 event and receipt. Use the true A4 actor–object contacts (not an actor–actor impact substituted for object contact) and independently verify both swept material trajectories and actual visibility from each actor to target. Keep target identity, collider policy, material revision, world tick, contract rules and A6 source lineage unchanged except explicitly A6-authorized scenario setup. Do not move registered actors underneath issued grants; issue fresh grants after stable geometry snapshots.

Positive candidate search may change **source-backed** placements/speeds in a test fixture while recording each exact source hash, but the positive proof must use only *actual* same-world A4/A5/B2 outputs. Search should identify separate positive physical and eligible contractual domains and intersect them; if no overlap, record **HOLD / genuinely unsatisfied**, not hypothetical grants.

## 2. Four incremental implementation cuts (each on separate reviewable commit)
### H319 — Real PUSH geometry under dual A4/A5 conditions (positive physics)
1. Implement a **read-only bounded geometry solver/search** in `src/tests/test_retc_h319_push_geometry.c`, reusing actual A6 actor 10/20 and object 2010 source setup. Test approach arcs, off-axis lateral distances, bounded velocity and time-of-impact cases. Do not invent an impact or relax collision and occlusion rules.
2. For both actor→object pairs require actual A4 `NF18A4_ACCEPTED`, nonzero contact ID, applied nonzero impulses, finite TOI, coherent tick/version; independent CCD/contact oracle must agree. Actor–actor contact alone is insufficient.
3. Require both actors' real A5 swept paths FREE on the same material epoch, plus static body clearance and B2 target visibility. Classify each failure as A4 no-contact, A3 body occlusion, A5 SOLID/PENDING/INVALID, B2 OCCLUDED/BODY_BLOCKED or stale source.
4. Assert no changes to A6 tick, accepted frame pointer, B3 receipt count or old B2 grant status during read-only exploration. Old proposals after any pose change must reject as stale.
**H319 acceptance**: at least one fully genuine 2×(A4 positive + A5 FREE + visibility) same-source geometry, certified by GCC+Clang and independent oracle. If absent: OPEN, not PASS.

### H320 — Two independent owner-current B2 PUSH authorizations
1. Rebuild `nf18b2_live_frame` only after fixing H319 world pose and epoch, register both source accounts and object 2010 under the same actual A6 owner. Issue two distinct `Nf18b2Intent` proposals; independently verify authorizations, not a reused grant or `b2_granted=true` diagnostic.
2. Bind each grant to authenticated actor, unique action/nonce, object, correct world revision/tick, physical-contact receipt, A5 epoch, contract and issuer source. Enforce both valid simultaneously **before** consuming either; deterministic ordering by existing authority rules and fairness.
3. Negative controls: revoke either grant, expire it, consume one twice, mutate object or source pose, vary actor/session IDs, race shutdown. Every unsafe case rejects without B3 or A6 changes.
**H320 acceptance**: two *independent live issuer-owned grants* for actor10/20 and object2010 at H319 contact geometry, with A5 FREE and original B2 rules; no positive grant fabricated from source traces or fixture fields.

### H321 — Two typed B3 PUSH effects in one private A6 envelope
1. Use `Nf18b3Event` with `NF18B3_PUSH` for both actors, distinct action IDs and source receipts, same source tick/epoch. Invoke `nf18b3_authorize` and `nf18b3_realize` only via the A6-owned validated transaction path. Make effects state-deltas on the single contested object and each actor's contractual/accounting outcome; model deterministic arbitration and physically plausible combined impulse.
2. Stage both independently revalidated grants, two A4 contacts, A5 clearance, typed B3 results and compensation/rollback records in one **private** owner envelope. No external mutable callback side effect until complete acceptance.
3. Test first/second grant loss, B3 realizer failure and duplicate receipt; rollback clears every private delta with no visible partial effect.
**H321 acceptance**: two truly realized source-backed typed B3 consequences, proved by independently readable immutable candidate data, with zero live publication before H322.

### H322 — One immutable all-or-none A6 publication
1. Bind H321's full envelope through the existing A6 `stage_joint` / `prepublish` / `accept_candidate` owner hooks and `retc_pending` / `retc_publisher` lifecycle; do not create a second tick owner or bypass A6's normal step.
2. Stage complete joint object/actor/history/receipt projection privately; check expected previous accepted frame, exact world revision, same material epoch and all B2 issuer-consumption constraints. Publish one immutable revision and one `joint_commit_epoch` advancement **only after every stage is ready**.
3. Failpoints before/after each stage, receipt issuance, A5 history, A6 candidate accept, publication, reader acquisition, shutdown/reinit; no ghost B3 or material events or stale A6 snapshots permitted. Compensating action after an already committed publish is a new authoritative transaction, never a silent history erase.
**H322 acceptance**: a source-exact positive two-actor PUSH transaction, a single public commit transition, two typed consequences visible atomically to independent reader, and all tested negative branches yield zero partial publication.

## 3. Acceptance matrix, serial and parallel
For **each** H319–H322 cut: exact SHA, GCC and Clang C11 builds with strict warnings, canonical/seeded-randomized/pathological/boundary/held-out corpora, source/epoch and oracle hashes. Additional ASan+UBSan, ThreadSanitizer in supported configuration, independent compiler and platform rerun, no test result accepted on another SHA.
- **Serial baseline:** fixed tick, actor10→20 order and actor20→10 reverse order, many seeded schedules; exact immutable hash/receipts expected identical under deterministic tie-break policy.
- **Parallel observation:** true competing proposals in worker threads, A6 as sole authoritative commit; random scheduling/yields, explicit contention, barrier rendezvous and race injection. Compare *accepted* resulting frame/ledger hash and number of committed effects to serial baseline, not mere order-independent helper outputs. Verify bounded fairness/starvation under representative schedules.
- **Falsification:** one grant missing/revoked, A4 no-contact, A5 PENDING/SOLID, B2 stale, two submissions for same nonce, partial B3 failure, publication timeout, shutdown/rebind and fault injection before/after publish. All deny or compensate correctly, without double-credit.
- **Independent held-out:** reviewer supplies unknown seeds and altered geometry; independent result oracle. Record every failing seed, exact SHA, compiler options and minimal reproduction.
- **Observability:** source actors 10/20, object2010, source tick, A4 IDs/contact/impulse, A5 query/mat epoch, B2 issuer credential handles/redemption, two B3 typed receipts, A6 accepted previous/new epochs, journal IDs and reader visibility.
- **Human playable P0/P1/P2** remain separate NOT MEASURED until real sessions and frame/input metrics; diagnostic CI ≠ playability.

## 4. Litmus and immutable HOLD rules
### H322 is not automatically M18C.2 P*
Final C2 gate = conjunction of H319→H322 positive proof **AND** negative fault injection/revocation/deterministic parallel scheduling, independent full-source exact SHA GCC/Clang + ASan/UBSan/TSAN (where supported), inherited historical regressions, lifecycle/reader safety and representative playable evaluation required by RET-C ledger. A positive but incomplete test stays scoped PASS / production HOLD.
M18C.3 requires **separate** portable versioned whole-transaction WAL/checkpoint, ordered fsync/dir sync, corruption and torn-write testing, new-process A6 owner restoration/rejection **once** with zero ghost/double B3 receipts; current local A5 WAL is insufficient.
Never mark H319–H322 complete on a plan, generated fixture, green historical job, synthetic B2 grant, two A4 actor–actor only impacts, or receipt flags without applied effects. Keep PRs DRAFT and release HOLD until independent gate met and human reviewer ratifies.

## 5. Serial positive-first workflow and independently parallel verification
Commit order: H319 positive geometry → H320 live double grant → H321 staged dual typed effects → H322 owner atomic publish. Build focused tests on **each new SHA**, do not pile unexplained red results on later features. In parallel, independent reviewer develops held-out negatives and sanitizers, and STEWARD designs C3 durable envelope compatibility but does not declare C3 acceptance. BRIO traversal prefilter remains optional research independent of this PUSH positive path.

## 6. Twenty acceptance/recommendation items (X=20 plan; none executed here)
1 H319 source-actor/object world fixture; 2 H319 actual actor10→object A4; 3 H319 actor20→object A4; 4 H319 dual A5 FREE; 5 H319 A3 visibility/source restoration;
6 H320 first live B2 PUSH; 7 H320 independent second live B2 PUSH; 8 H320 current owner/tick/epoch binding; 9 H320 contention deterministic fairness; 10 H320 stale/revoked/consumed rejections;
11 H321 first B3 PUSH physical consequence; 12 H321 second B3 PUSH consequence; 13 H321 one private envelope; 14 H321 two-receipt source lineage; 15 H321 rollback/compensation;
16 H322 complete A6 immutable publication; 17 H322 atomic reader visibility; 18 H322 fault injection at every boundary; 19 H322 serial/parallel exact replay and TSAN; 20 H322 independent held-out + inherited acceptance.
Every item MUST become one implemented/verifiable source-exact evidence row, not merely a document checkbox.

## 7. Distance and current certification
H319–H322 **OPEN**. Current positive intersection is not demonstrated: H318 reverse A4=0/10 and actor20 MATERIAL_PENDING; H315 lateral A4 positive had B2 OCCLUDED. Estimate remains **7–14 five-feature C2 sweeps** + **4–10 additional, potentially overlapping C3 sweeps** (high uncertainty; no percentages). Next irreversible positive proof is one same-source **dual actor→object PUSH** with independently issued valid B2 grants, two typed applied B3 outcomes and one atomic A6 commit. No build/tests were performed in creating this plan.
