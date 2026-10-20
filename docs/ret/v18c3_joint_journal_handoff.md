# 1.8C.3 — Single A5 + UCW logical journal, source and test handoff

PRAXIS / SIGNAL / STEWARD · 2026-10-20 · review-only research development, production release HOLD.

## Implemented
- New `src/shared/nf_ucw18c3.h/.c` forms **one** POSIX fdatasync-before-acknowledge, flock-exclusive journal frame containing the **actual** `Nf18a5Integrated` memory snapshot (physical actors/object, contact history, canonical material grid, revision) and `UcwWorld` (actor/action/object/contract identity, logical receipts and balances).
- Envelope starts with a version/magic, generation, byte length, aggregate checksum. Each component has a 32-bit type and length. A future codec/registry may add additional mechanics but is **not implemented** by these two hard-coded sections. Unknown schema/sections fail closed.
- Source-level bounded maximum 16 MiB; replay checks generation progression and both physical/logical tick/revision advancement, checksum and section lengths; torn/corrupt tails quarantine the file rather than recognizing a complete world.
- Single append+fdatasync publishes both in-memory C3 journal fields together. No second standalone logical event-file writer is introduced in this design.
- Actual A5 object is stored as an opaque ABI-dependent memory snapshot, not canonical portable per-field serialization. This is an **interim compatibility mechanism**; padding, endianness, compiler version and field/schema upgrades are not supported.
- Existing `nf18a5_checkpoint`, `nf18a5_integrated_pair_step_checked`, `nf18a6_step` and `nf18b6_push` remain unaltered. **Critically, C3 is NOT called by A6/A5 prepublication**; there is no production single publication hook and the running game could still write its legacy A5 journal separately. Therefore the actual integrated A6/B3/B5 crash atomicity gate remains OPEN.

## Test contract
`src/tests/test_ucw18c3.c` repeatedly writes and reopens a joint physical+logical record: five strata × 24 scenarios × three commits = **120 seeded scenarios, 360 logical/physical paired snapshot commits**. Validates both histories and identities; rejects repeated generation/commit, truncated tails and changed A5 data with stale checksum. Uses temporary POSIX files, strict GCC/Clang, sanitizer and inherited CTest. The tests use a constructed `Nf18a5Integrated` state, not a completed live rigid-body solver tick, so they are not proof of A4 impulse-coupled B3 contract semantics.

## RET
- **A priori:** two independently fsynced journals cannot be presumed cross-file atomic. A single complete frame reduces that particular split boundary. One successful syscall does not prove real hardware power-loss guarantees.
- **Formally specified:** one versioned two-section frame, per-section kind and size, generation ordering, one checksum and quarantine for partial records, one file sync before local acknowledgement.
- **Empirically validated:** only exact successful commit/CI evidence; post-build functional and sanitizer results must be verified on GitHub.
- **Highly predictable:** modular section framing should accommodate future mechanic codecs more flexibly than an unstructured combined blob, given a typed, versioned registry and stable field encoders are supplied.
- **Uncertain:** ABI migration, actual disk power removal, torn writes at every boundary, real head-of-line fsync and 60 Hz tails, performance with complete large world, and independent physically generated replay oracle.
- **Unresolved P0:** full portable field-level serializer, verified material/contract witnesses, live A6 prepublication replacement with one durable journal (eliminating independent legacy writes), A6 rollback/error propagation, authentic multi-actor arbitration, source rights and release signoff.

## Recommendation / handoff
Keep this H0 C3 journal as a challenger until live A6 stages both complete material and contractual candidates, has a **single** commit gate and one crash-recoverable callback. Implement self-description as explicit mechanic ID+schema version+codec registry with per-version migrations, canonical fixed-endian per-field encoding, independent recovery oracle, and mandatory/optional section semantics. Do not call raw `memcpy` of the A5 struct "self-serialization", and never append two separate authoritative journals as a false substitute for atomic commit.

No canonical ledger edited. PR review required.
