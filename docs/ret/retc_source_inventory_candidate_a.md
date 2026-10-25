# RET-C source-bound reader/writer inventory (first pass)
Baseline H40 and Candidate A research branch; status **partial inventory, release HOLD**.

| Domain | Actual source location | Read/write | Present authority / escape |
|---|---|---|---|
| A5 physics | `src/shared/nf_contact18a5_close.c` `publish` | Writes mutable A5 state, optionally local WAL | Independently committed physical state, before B3 |
| A6 owner | `src/shared/nf_embody18a6.c` `nf18a6_step` | Writes actor, crate collider, world tick, camera, derived hash | Separate publication path |
| A6 dispatcher | `src/shared/nf_embody18a6.c` `nf18a6_tick_dispatch` | Calls begin, physical step, then B3 finish | H40 publication flag only protects participating readers |
| A6 snapshot | `src/shared/nf_embody18a6.c` `nf18a6_snapshot` | Reads actor/runtime directly | Does not inherently acquire immutable published frame |
| B2 eligibility | `src/shared/nf_b6_push18b6.c` `b2_check` | Reads server-owned world/provider snapshots | Ownership, inventory and reservations not fully serialized in H23 |
| B3 event | `src/shared/nf_interact18b3.c` `nf18b3_realize` | Mutates typed event status | Runs from B6 finish after A6 physical publication |
| B3 receipts | `src/shared/nf_b6_push18b6.c` `finish` | Appends receipt / optional second journal | Separate postcommit consequence |
| Composite view | `src/shared/nf_b6_push18b6.c` H37 | Reads A6+B3+envelope | Checked narrow view, not a replacement for raw world readers |
| World API | `src/shared/nf_world.c` `nf_world_find_actor`, `nf_world_find_actor_const`, `nf_world_set_input`, `nf_world_step_checked` | Mutable source access/reader bypass | Must migrate or isolate |
| Network transport | `src/shared/nf_net.c` | Transport routines inspected; no direct world references seen in first-pass scan | Indirect call paths not audited |
| Client prediction | `src/shared/nf_prediction.c` | Uses `nf_world_find_actor` and `nf_world_set_input` to mutate the raw world | Authoritative reader/writer escape; BLOCKER |
| Camera/presentation | `src/client/nf_camera.c` | Reads raw `NfWorld` colliders and actor transform | Unversioned reader escape; BLOCKER |
| AI and rendering | No validated source-wide call graph yet | Unverified | Global reader coverage BLOCKED |

## Candidate A bounded prototype
- `src/shared/nf_c2_retc_frame.h/.c` defines an **immutable after-publication** `NfRetcFrame` carrying physical candidate, typed B3 consequence, owner IDs, monotonically increasing tick/transaction IDs, up to eight materialized obligation records and derived hash inputs.
- `nf_retc_stage` allocates and deep-copies the candidate and refuses invalid source linkage.
- `nf_retc_publish` changes one atomic pointer after source/epoch checks; `nf_retc_read` observes either no frame or the accepted complete frame.
- It is single-writer and bounded to eight retained frames. No reader may outlive `nf_retc_dispose`. No concurrent reclamation scheme, durable journal, B2 semantic totality, real A6 integration or full network/read migration is claimed.
- Current schema is a **bounded prototype**: eight generic obligation tuples demonstrably do **not** constitute the complete B2/B3 obligation model.
- Tests exercise private staging, no premature view, copied obligations, strictly increasing epochs, replay and stale decision refusal; they do **not** demonstrate crash safety.
- Existing atomic-publication spec invariants INV-1 through INV-9 remain promotion gates.

## Next RET-C gate
1. Complete raw reader/writer inventory including network/AI/render, with exact symbol and owner mappings.
2. Upgrade typed obligations to full source-derived B2/B3 contract state, durability and versioned journal.
3. Bind A6 owner to private frame staging and make authoritative readers observe the frame (not direct mutable world).
4. After that, conduct process-kill matrix + fresh recovery + deterministic continued ticking.

## Source-derived B2 schema update
The Candidate A frame now binds a copy of actual B2 declarations from `nf_eligibility18b2.h`: `Nf18b2Use`, `Nf18b2UseDecision`, `Nf18b2Actor`, `Nf18b2Object`, and `Nf18b2Authority` (bounded replay receipts). `nf_retc_bind_b2` validates eligible physical and contractual decisions and frame actor/object/action/tick/revision identity, then copies all fields into the unpublished frame. Existing generic obligation tuples remain explicitly **unverified research fields**, not authoritative contractual rights. This is still insufficient for full B2/B3 obligation transitions, inventory debits, durable journal, or live A6 integration.

## RET-C transition and recovery contract (source-reviewed)
- **B2 eligibility (source: `nf_eligibility18b2.h/.c`)** is an adjudication, not an inventory debit. `Nf18b2UseDecision` has distinct physical and contract status; a successful decision alone does not mutate ownership or membership. The bounded authority tracks at most 64 historical actor/feature/tick receipts. The authoritative publication callback has a documented atomicity obligation but does not implement the storage backend.
- **B3 authorization (source: `nf_interact18b3.c`)**: PROPOSED -> WAITING for authorized, REJECTED for denied, WAITING pending uncertain evidence. WAITING -> APPLIED only upon `nf18b3_realize` with matching world/material/contract epochs AND successful material receipt callback. A failed callback leaves WAITING. After APPLIED a second realize returns DUPLICATE. Interrupt can change WAITING or APPLIED to INTERRUPTED, but has no compensating physical or inventory transaction.
- **Prototype supported transition**: verified `NF18B3_PUSH` physical/contact consequence with eligible B2 source witness. `nf_retc_seal_b2_b3` validates typed source identity and replay authority before composite publication; rejects any non-PUSH kind rather than claiming missing debit/transfer/repair implementations.
- **ABORT before decision**: discard unpublished staged frame. No B2, B3 or A6 mutation may escape staging. **ABORT after durable COMMIT is forbidden**: use a new compensating transaction and preserve old history. Current implementation does not implement compensation.
- **Restart requirement**: read a complete versioned A6-owned durable COMMIT record with B2 authority receipts, B3 typed event, A5 material state, world-side actor/collider/tick, transaction ID and all derived replay inputs. Validate checksums and causal linkage, rebuild indices, and advance exactly once. No such complete durable record is yet implemented; H22/H23 narrower envelope cannot substitute.
- **Unresolved**: explicit ownership and inventory delta formats, reserves/releases, membership transfers, interruption compensation, and precedence of B2 receipt memory relative to A6 commit remain production blockers.

## Extended verified reader categories
- `nf_prediction_simulate_command` directly invokes `nf_world_find_actor`, `nf_world_set_input`, `nf_world_step` and mutable combat/weapon functions; this is an isolated prediction simulation only if given a private world, which the current API does not enforce.
- `nf_camera_follow_actor` accepts an arbitrary live `NfWorld` and `NfActor`, reads world colliders and actor transforms; publication epoch is not enforced.
- `nf_net.c` contains transport-level code without direct actor/world access in the file inspected; this does not establish safety of protocol and server call sites.
- AI, visual renderer and network server call-graph completeness remain unverified. Do not label the authority inventory 'complete' until a repository-wide code-symbol search or full checkout call-graph scan closes these gaps.

## Source-backed PUSH realizer increment
- `nf_retc_stage_source_push` now constructs one complete **bounded immutable PUSH candidate** by combining actual A5 integrated physical state, the actual B3 applied event and actual B2 actor/object/use/decision/authority snapshots; it rejects source/epoch mismatches and any unimplemented non-PUSH event.
- It has **no side effects** on external A6, B2 or B3 state, and the frame is invisible until `nf_retc_publish` changes the accepted pointer. It is a scientific staging seam, not a real-world consequence or a durable commit.
- The second-epoch regression exercises this source-backed constructor before atomic prototype publication, while retaining stale-epoch refusal and deterministic frame-content assertions.
- **Outstanding blocker:** the actual live A6 owner still uses A5 publication followed by B3 finish. Do not route normal gameplay through the prototype or claim real-world atomicity before an atomic journal + reader migration.
- **Authority inventory completeness:** repository-wide server/AI/render call graph still unverified. Identified raw prediction and camera accesses are blockers; `nf_net.c` alone is insufficient to certify network readers.

## Integrated convergence gate — source audit, 2026-10-24
Verified primary source paths:
- `nf_contact18a5_close.c:publish` calls `nf18a5_checkpoint` then updates `*dst` independently of A6/B3. This function has no frame-publication API.
- `nf_embody18a6.c:nf18a6_step` computes a private `next` but then writes `r->physical`, collider bounds, live actor transform and `w->tick` in separate mutable operations. `nf18a6_tick_dispatch` invokes `owner_hooks.finish` after that step, before ending its lease.
- `nf_b6_push18b6.c:prepublish` already persists a narrow H22/H23 A5+B3 event envelope for the actual contact branch; however, it is missing full B2 server authority, ownership/membership/debit and world-derived replay state. `finish` still calls B3 realization and writes receipts after A6 mutation. This is therefore not a complete transaction decision.
- `nf_prediction.c:nf_prediction_simulate_command` mutates raw `NfWorld`/actor/weapon authority; `nf_camera.c:nf_camera_follow_actor` reads raw actor/collider state; `nf_world.c` exposes direct mutable actor retrieval. These are known reader/writer escape paths.
- `nf_net.c` contains no direct actor/world read in the examined transport source. This is not proof about all server/network handlers. Repository-wide AI/renderer/server inventory remains **unverified**, so INV-4 is still blocked.

**Fail-closed prototype rule:** `nf_retc_publish` now rejects frames missing source-backed `Nf18b2Use/Decision/Actor/Object/Authority` even when the epoch and B3 event are syntactically valid. It cannot make the legacy A6 pathway atomic. New regression asserts the unbound candidate is rejected without changing the published epoch.

**Must not claim** full contractual realizers exist: B2 eligibility grants neither ownership nor an inventory delta. B3 `nf18b3_realize` sets an event phase only after a material receipt callback, but currently does not produce a complete ownership/inventory/contract journal. PUSH physical evidence can be staged; debit/transfer/repair/interruption semantics need explicit independently verified source-backed realization.

**Replacement gate (not implemented):** A6 owner must pass *one* prepared `NfRetcFrame` with B2/B3 consequences and complete A6 world projections to an A6-owned durable commit writer; successful durable decision must precede *one* published frame pointer, and former A5/B3 mutable publications must cease being authoritative. Reader APIs must return leased immutable frame data, including network, AI, camera and server logic. Do not run whole-world crash matrix until this is true.

## A6 owner convergence seam — current implementation
- The real A6 prepublication gate now invokes `owner_hooks.accept_candidate` over its exact private A5 candidate **after** geometry/B3 prepublication validation but **before** A5 checkpoint and physical publication.
- The B6 PUSH adapter binds `accept_candidate` and rejects contact-producing joint-envelope candidates unless the staged B3 record is applied, identity-consistent and tick-aligned. Legacy non-envelope mode remains unchanged and is explicitly **not** joint authority.
- This closes a prepublication ordering check, but **does not implement** the accepted immutable `NfRetcFrame` as the authoritative A6 world publisher. A5 still writes its own snapshot and B3 finish still changes live state. No single immutable journal record containing all B2/B3 rights exists.
- CI and source-exact regression results are required before even the narrower owner-seam change is eligible for acceptance. Whole-world crash/replay/continued-tick matrix remains deferred.

## B2 encounter authority and RET-C compilation gate
- `src/shared/nf_encounter18b2.c:nf18b2_attach` assigns its own `Nf18a6OwnerHooks` and `finish` mutates encounter receipt counts and transitions after the owner tick. This is an **additional contractual authority/publication path** independent of the B6 PUSH adapter.
- Adding a positional field to `Nf18a6OwnerHooks` broke the encounter initializer, caught by strict Clang/GCC CI at prior source SHA `229186685388ea9a2a037087f3da6468c6435e36`. The initializer now uses designated members, avoiding silent ABI misbinding.
- Full Candidate A requires migrating **both** the B6 PUSH owner and the B2 encounter owner to one owner-prepared frame contract; current implementation has not done so.
- Do not claim source-access inventory complete, world reader isolation, or a durable authoritative frame decision until source-wide owner/call-site audit and exact-source CI acceptance.

## RET-C B2 encounter staged-registry integration
The real `nf_encounter18b2.c` owner maintains `registry` and a prepared `staged` registry (accounts with balance/inventory/credentials and objects with contract metadata). `finish` presently writes `r->registry=r->staged` after independent A6 physical commit. Candidate A now has `Nf18b2Registry encounter_registry` and `nf_retc_bind_encounter`, which copies only a prepared, A6-owner-linked registry under a matching prior tick; the source-backed test checks deep-copy isolation and duplicate refusal. **No journal, direct A6 frame publication, recovery, or suppression of encounter finish has been implemented.** Candidate A is not authorized for live gameplay; pending conversion of encounter and B6 PUSH to one A6-owned frame writer/reader API, source-wide raw-reader isolation, and crash tests. 

## RET-C whole-frame file prototype (limited durability)
- `nf_retc_commit_file` writes the complete bounded `NfRetcFrame` (A5, applied B3, typed B2 and optionally encounter registry) to a local checksummed file, then publishes the immutable pointer only after write and flush report success. `nf_retc_restore_file` validates and returns a complete frame rather than piecemeal domains.
- This file is **not yet a durable production commit decision**: the experimental writer now flushes/fsyncs a temporary file before rename, but no directory fsync, write-ahead recovery ordering, ABI-portable encoding, actual A6 owner integration, contractual consequence realization or all-reader isolation. The new test checks ordinary restart readability, not power loss.
- Current invariant is prototype file-before-pointer ordering; all RET-C atomicity and production gates remain HOLD.

## A5/A6 physical staging seam
The real A5 integrated pair solver has been split into a shared checked implementation with two outcomes: legacy `nf18a5_integrated_pair_step_checked` still publishes an A5 WAL, while `nf18a5_integrated_pair_stage_checked` computes its complete candidate into caller-owned private state without writing the A5 WAL. Opt-in A6 RET-C mode now invokes this actual no-WAL solver on a private copy and refuses live publication because contract and reader migration remain incomplete. This is an explicit incremental isolation of the independent physical publisher, NOT an accepted durable joint commit. Full on-disk restore, B2 registry, B3 receipt, reader conversion and publication cutover are not implemented here. Comprehensive crash matrix remains deferred.

## Joint owner staging hook, no premature commit
A6 RET-C mode now calls the real no-WAL A5 stage and, if supplied, invokes `owner_hooks.stage_joint` to obtain a privately allocated complete `NfRetcFrame`. The owner validates B2 evidence, staged encounter registry, applied B3 PUSH state and physical candidate identity before disposing the candidate. Because the live A6 world, encounter/B3 finish paths and all raw readers have not been cut over together, the path still returns UNSUPPORTED and does **not** write or publish a joint frame. This is a fail-closed preparatory contract, not the requested durable atomic replacement. No complete end-to-end stage_joint callback has yet been bound by both B2/B3 source owners, and interruption tests are deferred.

## Composite B2/B3 source-backed stage provider
`src/shared/nf_retc_joint_provider.c` introduces `nf_retc_joint_stage`: it validates the A6 owner lease, matching B2 encounter owner/world, prepared source registry, B3 PUSH staged APPLIED receipt, common contract epoch, and candidate A5 physical tick. It uses the existing typed B2 PUSH constructor and actual prepared B2 encounter registry binder and returns a privately allocated complete Candidate A frame. The regression checks refusal without prepared B2, acceptance with both domains, and refusal when B3 staging is missing. **This is not yet bound as the exclusive live A6 stage_joint owner callback** because B2 and B3 existing owner attachments are mutually exclusive and the live B2/B3 finish paths remain independent. The source snapshots are supplied in the composite adapter context, not yet rederived from one atomic live-world snapshot. Durable acceptance, recoverability and reader migration remain HOLD.

## RET-C owner-derived single-grant PUSH evidence (current revision)
`nf_retc_joint_stage_owned` is a stricter bounded constructor over the existing composite provider: it requires one granted, nonwithdrawn `NF18B2_PUSH` intent from the actual B2 encounter queue; uses that queue's witness, the encounter trace's revalidation, and the encounter scene's actor/object; cross-checks all against the staged B3 applied source event and the actual A5 candidate. Caller-supplied B2 use/decision/actor/object fields are ignored in this path. Unsupported multi-grant and mismatch cases refuse staging. A zeroed replay authority is only a **research assumption** and is not a persistent-authority substitute; no live atomic publication is enabled. 
The two legacy A6 owner-hook attachments remain mutually exclusive; this change is a source-derived preparatory bridge, not yet an operational combined owner. Integration requires explicit combined `begin`/revalidate/prepare and a single accepted-frame commit driving both B2/B3 state changes. Existing independent `finish` callbacks must no longer be authoritative after cutover. CI evidence required; global visibility and full crash matrix remain HOLD.

## Exclusive combined A6 owner hook prototype
`nf_retc_joint_attach` now installs one A6 `stage_joint` callback for the composite B2 encounter/B3 PUSH transaction, refusing an existing encounter/PUSH hook or active tick; `nf_retc_joint_detach` requires the corresponding ownership. No `finish` callback is installed, so **this adapter never independently publishes contractual consequences**. Regression asserts exclusive attach/detach and absence of a split finish path. It does not yet invoke the real B2 encounter `begin` preparation, so the prepared registry and PUSH record must still exist before staging; it is **not** a complete live combined owner. No joint durable decision, accepted-frame world projection, or all-reader migration is claimed. RET-C production and crash matrix HOLD.

## Composite preparation inside actual A6 tick (RET-C iteration)
The exclusive `nf_retc_joint_attach` now binds **one** A6 `begin` callback that calls the existing source-owned `nf18b6_push_prepare_joint` and `nf18b2_encounter_prepare_joint`. The A6 `stage_joint` callback calls the B2 source's prepublication revalidation, checks material contact and history changes, privately constructs a staged B3 applied PUSH event from its authorized WAITING event, and invokes `nf_retc_joint_stage_owned`. The intermediate event is restored after constructing the candidate, never independently realized. The frame constructor now checks the B2 witness's pre-tick world revision against the candidate's post-tick revision. Regression fixture updated accordingly.
**Critical unresolved:** the A6 RET-C branch still returns UNSUPPORTED and frees any staged frame; the combined owner does not invoke `nf_retc_commit_file`, publish the world, recover a transaction, or migrate raw readers. The existing raw-source mutation of encounter preparation/traces is not an authoritative joint commit; the server must keep those state changes private or reversible on refusal. The B2 source's actual `begin` can mutate prospective trace/queue staging; rollback semantics still need integration and testing. Complete crash matrix deferred until true authoritative cutover.

## B2 speculative preparation rollback (RET-C)
The composite A6 begin now preserves a pre-begin copy of the B2 encounter runtime and installs a **single joint finish callback used solely to roll back rejected transactions**, replacing the prior no-finish assertion. On a noncommitted A6 result it restores prospective grants, reservations, staged balances, trace transitions and queue bookkeeping; on all results it releases the copy. This is in-process rollback, not crash durability. Regression deliberately mutates staged balance after a rejected begin and checks restoration. The current RET-C mode still refuses all commits, so no successful-commit projection is yet exercised. World/readers and durable A6 authority remain unconverted; do not run full crash matrix or claim globally atomic publication.

## Reader visibility barrier for joint acceptance
A6 RET-C now calls `nf_retc_persist_file` to persist a validated complete frame **without** changing `NfRetcPublisher.current`; stores the accepted frame as `retc_pending`; projects A5/A6 physical bodies; then the combined A6 `finish` projects B2 prepared registry and B3 PUSH consequence before calling `nf_retc_publish`. The public immutable-frame pointer therefore no longer becomes visible ahead of physical/contractual projection in this owner path. Regression checks that durable persistence alone does not advance the published pointer. **Limitations:** all raw NfWorld reader/writer paths remain capable of observing partial projection; frame publication from finish has no robust recovery strategy on unforeseen failure; directory fsync, full restart recovery and power-loss guarantees are unproven. The composite-owner success path still requires executed end-to-end tests. RET-C production HOLD.

## Source-exact reader/writer audit, 2026-10-24 (partial, no global clearance)
- `src/shared/nf_prediction.c`: `nf_prediction_simulate_command` writes `NfWorld` through actor lookup, input setting, world stepping, combat and weapons. `nf_prediction_reconcile_v16b` also mutates actor state. A6-owner and active-tick guards now refuse these operations; regression added to the real A6 owner test. Client-owned prediction worlds remain independently simulated, not RET-C authoritative.
- `src/client/nf_camera.c`: iterates `world->colliders` for camera collision. This is an unconverted projection reader; no immutable-frame migration has been established.
- `src/shared/nf_world.c`: `nf_world_find_actor`, `nf_world_find_actor_const`, `nf_world_set_input`, `nf_world_step_checked` and direct actor/collider loops remain raw-world APIs; neither pointer publication nor this audit makes them atomically observable.
- `src/shared/nf_embody18a6.c`: A6 itself still reads and mutates raw `NfWorld`, requiring the owner lifetime and reader isolation; the immutable frame pointer is NOT a replacement for all those pointers.
- Network serializers, server handlers, AI and other rendering modules **not yet exhaustively traversed**. No claim of complete reader inventory or globally atomic visibility is warranted.
- Latest verification requirement: source-exact CI for the prediction isolation, plus successful **real** combined A6 dispatcher tick with A5 actor/collider/B2 registry/B3 receipts checked at tick N and N+1. Detached completion callback tests are insufficient. The complete interruption/recovery matrix remains deferred.
