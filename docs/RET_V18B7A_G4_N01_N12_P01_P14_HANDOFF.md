# 1.8B.7A Group 4 — N01–N12 / P01–P14 RET handoff
Date: 2026-10-19; parent Group3 PR #55 at 3bd7a95fa4fb1134dba6abf6203239b7be286fcf. This Group4 branch is a research proposal atop unmerged PR #55/#54/#53, not default branch or production server.

## User-locked hard core
1. **Durable authoritative transaction:** under one A6 tick owner, each accepted complete world transaction has a verifiable durable record before acknowledgment; permission, local state effect, physical realization and journal durability must never be conflated.
2. **Occluded crash resolution:** if a crash hides the result of a commit, absence of ACK does NOT mean action failure. Recover verified records and reconcile immutable action IDs. Incomplete, contradictory or corrupt records require explicit quarantine/evidence inspection rather than speculative commit or silent truncation.
3. **Graceful degradation:** preserve physical and contractual truth under storage/network/compute pressure. Prefer rejecting, pending, bounded queued requests or read-only mode; shed nonessential visuals/maintenance before authoritative correctness. Reject overflow or outdated state.
4. **Background stability:** checkpointing, compaction, retries and health observation must run inside measured residual budgets, without creating a second authority, stalling forever or modifying committed history without new transactions.
5. **High-fidelity simplicity:** simple deterministic routine checks plus occasional local fine A5 material/geometry checks when necessary. M4 provisional eligibility incumbent, truly independently implemented M3 challenger under locked C03 tripartite procedure.

## Source-provenance, implemented test scope
- `src/shared/nf_wal18b7.h/.c`: **new POSIX file-backed H0 journal**, 64-byte fixed versioned/checksummed records, max 64 receipts, little-endian explicit wire serialization, monotonic epoch and balance before/after, unique actor/action receipt, nonblocking exclusive file lock, fdatasync before acknowledging applied state, parent directory fsync when creating the path, replay on process restart, fail-closed quarantine of truncated/corrupt/contradictory trails, read-only mode after write/sync uncertainty. `NF4_TESTING` permits injecting a lost-ack error *after a complete write before fsync*, solely for process-level uncertainty testing.
- `src/tests/test_wal18b7.c`: real file IO, reopen/replay, duplicate/changed replay, stale epoch/insufficient balance, simulated lost ACK, torn suffix, corrupt record, full 64-record capacity, and five seeded corpora (64 canonical/256 randomized/64 pathological/64 boundary/256 held-out = **704 authored cases**). C11 GCC, Clang and sanitizer workflow in `.github/workflows/v18b7a-g4-wal.yml`; do not report passing until run outcome verified.
- **IMPORTANT limitation:** WAL stores only a narrow integer debit ledger, not complete physical A4 impulses, A5 contacts, actor possession, network packets, authoritative B3 realizer, durable snapshot+WAL checkpoint or live A6 world tick. It does NOT solve a general distributed transaction, SSD firmware power-loss safety, recovery under data center failures, sustained 60 Hz, performance p95/p99, or client-visible durability. The journal checksum is accidental corruption detection, **not** cryptographic authentication. File genesis initial_balance must be consistently supplied from separate authenticated configuration; this lab has no portable genesis checkpoint/manifest.
- Closed during this sweep: **design/policy determinations N01–N12 and P01–P14** as individually traceable commitments under docs/RET_V18B7A_G4_*_LOCK.md. **Implementation and empirical closure of the 25 non-N01 determinations remains OPEN.**

## The design map (simple-English patterns)
N01 WAL and ambiguous outcomes; N02 idempotent action ACK; N03 snapshot/replay; N04 torn/corrupt tail; N05 atomic multiresource compensation; N06 reconnection/session; N07 network loss/reorder; N08 scoped client snapshots; N09 occluded crash reconciliation; N10 graceful degrade; N11 stable background maintenance; N12 provenance/disaster exercises.
P01 real profiling; P02 owner tick budget; P03 physical frontier islands; P04 selective spatial refinement; P05 revision-stamped derived caches; P06 interest-based network deltas; P07 actual durable sync time; P08 budgeted background load shedding; P09 actor contention/fairness; P10 bounded memory/capacity; P11 worst-tail responsiveness; P12 five-stratum workload; P13 true independent incumbent/challenger; P14 final safety-first release gates.

## Test and release discipline
- **RET a priori:** uncertainty is not positive evidence; eligibility does not entail material execution or successful durability; an unacknowledged action may have been committed.
- **RET specified/formal:** bounded IDs, correct record ordering, versioned serialization, pending/quarantine states, deterministic same-snapshot arbitration, accounting preservation. Not a machine-checked proof of entire game.
- **Empirical:** include only actual CI results after they complete, and always label tested scope. Prior Group1–3 corpora are inherited unrelated evidence; do not count as tests of disk journaling.
- **Highly predictable yet unmeasured:** appending bounded records plus periodic compaction could be efficient; actual fsync latency and scheduler impact unknown.
- **Uncertain/unresolved:** raw device power fault behavior, file rename/directory sync portability, cross-domain atomic commit, timestamp/packet semantics, provenance and rights, actual performance and human recovery experience, B1 H1 historical source #37, licensing #36 and branch ancestry #30, EEC pending 337 of 360 network tick workload.

## Mandatory remaining gates
1. Actual production A6 writer with physically correct A4/A5/B3/B4 state and one atomic durable B5 transaction; fail-injection near each publication boundary.
2. Independent crash oracle, power-loss hardware/filesystem matrix and verified recovery/resumption/backup. Manual quarantine repair process documented; no silent lost/duplicate debit.
3. Network session authority, duplicate/loss/reorder tests, client/observer read-only projections, reconnect receipts.
4. Implemented snapshot/checkpoint and stable background compaction, measured stalls and 60 Hz budgets.
5. Paired real incumbent/challenger measured throughput, p50/p95/p99 sync/tick/display latency, memory/CPU and fairness under canonical, randomized, pathological, boundary, held-out worlds.
6. Full engine CI (GCC/Clang/ASan/UBSan), independent sources/rights, maintainer signoff, historical H1 ledger reconciliation.

**PUBLIC COMMITMENT:** N01–N12, P01–P14 normative contracts LOCKED for research; N01 POSIX H0 journal introduced as a challenger to B5 mock callback, not yet a production replacement. All full-integrated engineering acceptance, merge and release HOLD.
