# RET-C M18C.3 H323–H324 — Strategy D implementation / E acceptance: private fresh-owner recovery
Date: 2026-11-20. **Implementation-and-acceptance plan ONLY. No H323/H324 production code or test success is claimed.**
Source baseline: H318 commit `a465670e612b61a82b073acdbc68c758e1528bf0`. Parent plan: draft PR #273 H319–H322, commit `d5e58e321af58acdb2d569302346a09160233803`. This continuation targets a draft PR based on that planning branch; it does **not** imply H319–H322 exist or pass.

## Adopted decision
**D implementation** = one versioned portable transaction WAL + immutable checkpoints + stable transaction identity/idempotent replay.
**E acceptance** = independent new-process owner, kill/crash/power-loss emulation, every stage and durability barrier, corruption/torn-write, repeated recovery, adversarial and held-out testing, reader visibility and realistic native continuity.
**Publication policy** = PRIVATE reconstruct → verify entire world + ledger → single A6-authorized accepted epoch publication; reject whole candidate otherwise. No progressive public restoration.
**Scope** = same A6-owned two-actor PUSH from H319–H322: true source actors 10 and 20, contested object 2010, valid contact A4, same-epoch A5 clearance, independently issued B2 authority, two applied typed B3 PUSH effects, one A6 atomic commit.

## Source-exact existing implementation and limits
- `src/shared/nf_embody18a6.h`: A6 runtime is sole tick/commit owner; owner hooks `begin/prepublish/accept_candidate/stage_joint/finish`; `retc_pending`, `retc_publisher`, `joint_publication_active`, `joint_commit_epoch`, `world_lease`, auth lifecycle lock, `auth_shutdown`. Recovery must work with these existing ownership boundaries, not create a second owner.
- `src/shared/nf_contact18a5_wal.c`: existing A5 snapshot log uses raw `Nf18a5Integrated` ABI-local structs, FNV checksum and fsync, and parent-directory fsync. Not portable across ABI/endianness/version; A5-only record is insufficient for A4/B2/two B3 receipts.
- `src/shared/nf_c2_h22_envelope.[ch]`: experimental A5 + one B3 receipt bundle, `nf_c2_h22_save` writing a staging file, fsync and rename. Current save does **not** fsync the renamed destination's parent directory after final rename; do not infer full power-loss durability. The `Header` and `NfC2H22Receipt` contain ABI-layout-dependent structs; no production cross-version contract.
- `src/tests/test_v18c2_h23_process_restart.c`: child-process write plus recovery of a single-actor PUSH, not full two-actor fresh-owner production proof.
- `src/tests/test_v18c2_h28_kill_boundaries.c`: five kill cuts `before_stage`, `after_physical_stage`, `after_stage_write`, `after_file_sync`, `after_rename`; initial recovery classification is historical lab coverage only.
- `src/tests/test_v18c2_h31_outer_cuts.c`: five outer cuts and envelope integrity; source explicitly says WHOLE-WORLD ATOMICITY NOT PROVEN.
- `src/shared/nf_c2_h19_lab.h`: logical in-memory frame recovery; no physical disk or fresh-world full evidence.
- `src/shared/nf_contact18a4.h`, `nf_contact18a5.h`, `nf_encounter18b2.h`, `nf_interact18b3.h`: source-backed contact/epoch/auth/effect must all be represented, not simulated by a checksum or synthetic flags.
- PR #272 BRIO research candidate snap prefilter is **non-authoritative** and must remain independent of recovery grants.

## H323 — Durable portable whole-transaction WAL + checkpoint
Implement in small, source-exact commits with test-first changes after H322's passing dual-PUSH candidate exists; until then use **explicitly labeled synthetic NEGATIVE codec fixtures only**.
1. Define portable stable binary schema (explicit LE scalar encoding or canonical format; fixed-width integers; version, feature bits, length bounds, digest, actor IDs 10/20, object 2010, A6 prior/new tick and revision, A4 2-contact source IDs + impulse records, A5 material epoch + swept-clearance proof digest, B2 grant issuer IDs/nonces/consumption state, B3 two typed applied receipt IDs, A6 transaction ID, predecessor hash and canonical resultant-state digest). No raw pointers, padding, enum/native struct dumps, floats with undocumented representation or host-specific byte order. Reject unknown required features; versioned backward compatibility must be explicitly tested.
2. Use **transaction identity** (owner epoch + monotone sequence + nonce or equivalent unique authoritative ID) consistently across WAL, checkpoint, accepted A6 frame, B3 receipts, and dedup index. Bind both B3 receipts to the *same* transaction with distinct actor/action identities. Never reissue consumable B2 grants from journal bytes.
3. Define prepare/commit records and explicit checksum/length/sequence framing. A torn last record cannot be accepted; detect truncation, corruption, duplicate commit, forked predecessor hashes, reordered records, future schema, stale epoch, contradictory B3/action set, invalid material revision, spoofed grant issuer and rollback to older checkpoint.
4. WAL ordering: validate and privately stage complete A6 candidate first; serialize PREPARE, fsync file, serialize COMMIT only after full source-backed validation, fsync file, ensure any new file and directory entry durability with parent-dir fsync, then acknowledge *durable* commit. Coordinate A6 public publication with durable commit semantics: after crash between durable commit and public visibility, new owner must publish exactly once; a public acknowledgment cannot precede recoverable durable COMMIT. A physical-storage fsync failure must not receive durable-commit acknowledgment.
5. Checkpoints: immutable versioned snapshot via unique temp file in same directory, complete write, fsync temp, atomic rename, fsync directory, durable manifest referencing exact snapshot SHA and highest included txn ID. Keep predecessor checkpoint until new checkpoint and manifest durably committed; do not silently mix old checkpoint and new WAL generations.
6. Compaction: only truncate/retire WAL prefix after verified checkpoint inclusion and directory durability, with independently replayable remaining tail. Preserve provenance and audit evidence.
7. Durability/performance budget: bounded recovery time/memory for representative world; stream/check limit records, fail on oversized lengths/overflows. No arbitrary filesystem guarantees: test with POSIX fault injection, and label power-loss emulator results distinct from physical power-loss proof.
**H323 scoped PASS** = source-exact portable codec + checksum/migration + complete two-actor transaction serialization + successful fsync/rename/directory boundaries + torn/corrupt/unsupported-version DENIAL; independent exact SHA GCC/Clang matrix and sanitizer/held-out results. WAL codec passes alone cannot promote M18C.3.

## H324 — Genuinely fresh A6 owner and private all-or-none restoration
1. Launch a **new OS process** with fresh `NfWorld` + fresh `Nf18a6Runtime` + new auth state and world lease, not merely a copied old runtime. Reinitialize from documented deterministic seed/static scene and qualifying checkpoint, then replay the ordered committed WAL tail privately into a shadow authoritative candidate. No reuse of stale old owner pointers, volatile B2 issuer credentials or in-memory worker state.
2. Rebuild actor10, actor20 and object2010 as a coherent single scene; restore canonical A4 and A5 history/material/contacts, B2 consumed-grant/issuer proof from validated committed transaction lineage (not creation of fresh authority), and **both** B3 applied PUSH receipts. The result must match the source exact accepted state hash and prior→new tick/version.
3. Validate one transaction as all-or-none; reject incomplete records, missing one B3 receipt, unmatched actor/target, stale material epoch, absent A4 contact, mismatched B2 lineage, unauthorized revision, contradictory world hashes or unknown schema. With corrupt tail, recover at most the last independently validated committed prefix if policy permits, **never** a partly recovered transaction. Log explicit accepted-prefix boundary.
4. Deduplicate txn ID + receipt IDs, including repeated process restarts, journal replays, checkpoint + WAL overlap and crash during replay. Each accepted transaction has exactly **one observable effect per typed receipt**, and at most one accepted A6 epoch advancement. Replay code may execute repeatedly; effects must not.
5. Before accepted publication, acquire valid fresh owner/quiescent lifecycle state, finish **full private** validation, close/fence old reader leases, establish a single atomic published pointer/epoch via existing A6 publisher contract. All public readers must see either old accepted epoch or full new accepted epoch; never partly initialized actors, material, grants or B3 receipt counts. If no valid durable state, refuse publication and remain unavailable/at last valid checkpoint. This is not a rollback of an acknowledged durable commit; handle valid committed tail deterministically.
6. Old owner/network fencing: make old owner tokens invalid for any new publish; confirm recovery cannot create competing live tick owners. B2 consumed/revoked grants remain consumed/revoked post-recovery.
7. Separate process fault injection during recovery read, checkpoint select, WAL scan, physical rehydrate, receipt reconstruction, validation, prepublish, publication barrier and postpublish reader acquisition. Restart from the same bytes twice and repeat faulted recoveries. Zero ghosts, duplicate receipts, partial public state.
**H324 scoped PASS** = independent new A6 OS process, source-exact full two-actor committed transaction restored or whole candidate rejected, no double-credit, immutable single public epoch, repeated restart equivalence, serial/parallel independent verification. All required production gates and human ratification still separate.

## E acceptance: every fault and durability boundary
*Use fault-injection IDs; each run must record exact source SHA, test seed, OS/filesystem, compiler flags, pre/post durable file hashes, original accepted epoch, expected/actual recovered epoch, A4/A5/B2/B3 receipt IDs, A6 reader hashes and stderr.*
| Phase | Injected conditions | Required outcome |
| --- | --- | --- |
| PREPARE serialize/write | pre-write, partial header, payload short write, EINTR, ENOSPC, EIO, corrupted digest, impossible length | Refuse incomplete transaction; keep previously accepted prefix |
| PREPARE durability | fflush failure, file fsync failure, fsync interrupted | No acknowledged durable commit |
| COMMIT write | absent marker, half marker, duplicate marker, wrong txn/predecessor ID, B3 missing/duplicated | No acceptance of partial/contradictory record |
| COMMIT durability | file fsync failure; kill immediately before/after fsync | Only fully durable valid committed records admitted; never infer survival from process memory |
| Rename/dir | temp collision, rename failure, parent directory fsync failure, kill before/after rename/dir sync | No durable acknowledgment before required barrier; valid prior generation survives |
| Checkpoint | checksum failure, truncated snapshot, manifest mismatch, old/new generation mix, double inclusion, rollback | Reject invalid checkpoint or use proved predecessor + WAL |
| Recovery | missing file, wrong schema/version, stale epoch, token replay, torn WAL tail, unknown feature | Whole transaction reject with explicit reason |
| A6 rehydrate | wrong actor, object ghost, omitted A4/A5, one B3 absent, issuer mismatch | Reject before public visibility |
| Publish | old owner still active, lease collision, kill prepublish/at publish/postpublish, reader races | One accepted epoch or no publication; no partial public state |
| Repeated crash | crash at each recovery step over 2+ restarts, duplicate committed ID | Exactly-once observable effects; deterministic acceptance |
| Independent checks | GCC/Clang, ASan/UBSan, TSAN where supported, serial/parallel replay, random/pathological/boundary/held-out | Same accepted state and denial reasons; no race/UB |

**Power-loss emulation limitation:** kill(9) does not simulate power-cut caching, controller/drive failures or torn sectors. Use fault-injectable filesystem and storage emulator when available; separately document any physical power-loss tests. No untested storage guarantees.

## 25 executable recommendations/test cases (X=25, NONE CLAIMED RUN)
H323: 01 schema v1 fixed endian; 02 roundtrip two-actor full envelope; 03 unknown required feature denial; 04 version migration; 05 length/torn CRC denial; 06 predecessor/seq fork denial; 07 txn ID and B3 receipt dedup; 08 prepare write faults; 09 prepare fsync faults; 10 commit write/fsync faults; 11 checkpoint temp fsync/rename; 12 parent-directory sync and failure; 13 manifest generation/checkpoint/WAL match; 14 checkpoint compaction ordering; 15 cross-compiler/ABI golden bytes.
H324: 16 truly fresh A6 process creation; 17 private whole-source actor/object/material/B2/B3 restoration; 18 fail-closed missing actor or B3; 19 repeated replay exactly-once; 20 kill at every rehydrate/publish cut; 21 accepted-pointer reader atomicity; 22 concurrent serial/parallel owner competition and fencing; 23 revoked/consumed B2 grant replay denial; 24 sanitizer + held-out process kill/corruptions; 25 independent end-to-end acceptance report and human sign-off.
These are acceptance tasks, not performed tests. For EACH row, record exact SHA, independent expected/actual, pass/fail and reproduction logs.

## H323/H324 reviewable commit sequence
- H323.A portable header/codec + golden fixtures; H323.B WAL prepare/commit with real fsync + directory sync; H323.C checkpoint/manifest + compaction + negative matrix.
- H324.A fresh OS process + private reconstruction; H324.B idempotency + publication fence; H324.C per-cut crash harness + cross-compiler/TSAN/held-out.
- Independent review and CI run after every cut. Keep PR draft through failures; never rewrite historical CI to PASS. Each new commit invalidates prior same-SHA certification.

## Serial and parallel observations
Serial: two actor PUSH commits with order 10→20 and 20→10, same source snapshot, deterministic arbitration; checkpoint→WAL recovery produces matching world digest under forward/restart/replay and duplicate attempts.
Parallel: competing proposal workers and concurrent readers; only A6 owner writes/commits, fresh-owner activation fenced after old owner stops; match serial accepted epoch and full world/material/B3 receipt digest. No synthetic eligibility substituted.

## Hard acceptance and dependency gates
**H323/H324 and C3 remain HOLD while H319–H322 genuine dual-actor PUSH and M18C.2 P* are unproven.** Codec/kill harness can be developed ahead as research, but positive production recovery cannot be certified before a real durable transaction exists.
**C3 P-recovery**: one independently started owner reconstructs/rejects complete A4/A5/B2/B3 transaction from durable versioned bytes and publishes one coherent immutable A6 epoch *only after private validation*; every storage/commit/restart cut is fault-tested, no ghosts or duplicate effects/receipts, independent same-SHA GCC/Clang + sanitizers + held-out, reader and lifecycle audits, accountable review. Missing any = HOLD.
**P0/P1/P2** native human play remains NOT MEASURED until real sessions. A green unit/codec CI is not native playable evidence.
**No merge, no production readiness, no automatic certificate** on this planning PR. Preserve all prior drafts, branches and failed evidence.
Distance unchanged from H318 ledger: approximately **7–14 substantive five-objective C2 sweeps**, then **4–10 additional potentially overlapping C3 sweeps**, high uncertainty, not a percent complete estimate.
