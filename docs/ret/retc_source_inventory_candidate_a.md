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
