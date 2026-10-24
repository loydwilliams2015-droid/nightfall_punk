# RET-C convergence contract: A6/A4/A5/B2/B3 atomic publication

Status: **SPECIFICATION / HOLD**, not an implemented, verified transaction protocol.
Baseline: H40 `ef77a71e77d8b87aaef29812848df46dd5ad1953`.
Evidence: three exact-SHA GitHub workflows succeeded for H40 on 2026-10-24.

## Source-exact defect statement
- `nf18a5_integrated_pair_step_checked` stages the next physical state and calls `publish(s,&next,journal)` after prepublication gating.
- `nf18a6_step` then copies physics to the A6 runtime, live actor, crate collider and world tick.
- `nf18a6_tick_dispatch` calls `owner_hooks.finish` only *after* the physical step.
- B3 `finish` calls `nf18b3_realize`, updates the receipt and then the A6 joint epoch.
- H40 `joint_publication_active` is a participating-reader exclusion flag; raw `NfWorld` readers are not synchronized. It is **not a global atomic publication barrier**.
- Existing H23/H26 checkpoint restores an integrated A5 checkpoint plus one applied B3 event; it does not represent all B2 obligations, contract ownership/reservations or world-side derivative state. Do not promote it as complete contract recovery.

## Nonnegotiable invariants (RET-C acceptance contract)
**INV-1 Sole decision:** A6 owner chooses exactly one durable `COMMIT(tx_id,tick,hash)` or a definitive `ABORT`; no independent authoritative A5 or B3 commit transition.
**INV-2 Whole state:** journal carries physical actor/object dynamics, material history and impulses; A6 world/actor/collider authoritative components; all B2/B3 ownership, reservation, obligation and receipt consequences; required derivation inputs and provenance.
**INV-3 All-or-nothing:** uncommitted candidate is inaccessible to readers; committed snapshot exposes both physical and contract consequences together; a rejected proposal exposes neither.
**INV-4 Linearizable visibility:** all authoritative readers access a versioned immutable snapshot or equivalent shared synchronization; forbidden raw mutable reader access is enumerated and enforced.
**INV-5 Durable recovery:** fresh-process recovery picks a complete validated journal decision, reconstructs both domains and all derived indices (or safely recomputes them), and refuses malformed/torn/incompatible records without partial exposure.
**INV-6 Exactly once:** tx IDs, ticks and world/contract epochs prevent duplicate application and unauthorized replay, including across multiple restarts.
**INV-7 Continued ticking:** recovery permits the next world tick, maintaining deterministic state hashes and contractual invariants compared with uninterrupted control execution.
**INV-8 Fail-stop cutpoints:** terminate an independent writer before/after staging, write, fsync, rename, directory fsync, decision, reader publication and B3-related transitions; launch new recovery process for each, including combinations and pre-existing committed checkpoints.
**INV-9 Evidence:** GCC and Clang strict, ASan/UBSan, canonical, randomized, boundary, adversarial and held-out comparisons, with exact SHA, platform, seed and results.

## Incumbent and challenger evaluation
**Incumbent H40:** physical publish + later B3 finish, checked by joint composite-read flag. Low integration cost; fails INV-1, INV-2 and INV-4.

**Challenger A (recommended prototype): Immutable `WorldTxFrame` + durable journal decision.**
- A6 computes a *private* complete candidate (physical/contract/derived input) and validates capacity/authority/material dependencies without mutating published state.
- Serialize and durably commit one authenticated/versioned full frame under sole A6 owner; no second A5/B3 authoritative WAL.
- Publish the accepted frame pointer/epoch as a **single reader-visible transition**; all authoritative readers acquire a stable snapshot/lease. Publish no mutable raw state until conversion is complete.
- Restart loads the last validated accepted frame and reconstructs both A6 and B2/B3 with idempotent transaction identities. Generated caches are rebuildable, not independently authoritative.
- Migration must identify and remove or isolate *all* existing raw readers; physical solver may still use private mutable candidate buffers.

**Challenger B: Shared mutex + copy-on-write world and contract state.**
- A6 stages everything privately, commits the durable transaction, then acquires a writer lock to atomically replace both states; all readers obey the same lock.
- Fewer representation changes; risks contention and raw-reader bypass. Compare latency and proof coverage.

**Selection rule:** prefer A only if mandatory invariants and paired held-out tests pass and memory/latency gates are met; B retained as measured challenger. Neither is accepted yet.

## Ordered implementation and test plan
1. Inventory raw A6, B2, B3, network and render readers/writers; report unconverted paths. This inventory is a **blocker** to global visibility claims.
2. Specify complete `WorldTxFrame` and B2/B3 obligation serialization, journal version/checksum/directory durability and replay keys.
3. Implement candidate staging and validation as pure operations; prohibit independent A5/B3 durable writes in joint mode.
4. Implement single journal decision, atomic reader-facing frame publication and synchronized access; leave legacy mode isolated.
5. Implement fresh-process recovery, derived-state reconciliation and exactly-once tracking.
6. Register exhaustive killpoint matrix: physical stage, contract stage, journal write, fsync, rename, directory fsync, before/after epoch publish, before/after former B3 finish, and multi-tick repeats.
7. For every cutpoint assert: prior complete frame OR new complete frame OR definitive refusal, never a mixed result. Assert replay refusal, deterministic next N ticks, conservative durability under crash, and no raw reader escapes.
8. Compare incumbent and challengers under common seeds and representative frame-time workloads; report performance and complexity as well as correctness.

## RET-C governance
- No automatic H41 increment or new PR per micro-fix. This branch is a **convergence specification**, not a completed build.
- A candidate is not accepted merely because legacy CI is green. Source-exact acceptance test matrix, all INV gates and negative controls must pass.
- After two repeated failures on the same invariant, perform root-cause review, not an additional patch-only increment.
- STEWARD production and 1.8C.3 durability gates remain HOLD until independent verification.
