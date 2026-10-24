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
