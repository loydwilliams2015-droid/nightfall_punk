# nightfall!punk 1.8B.5 H0 — Network and persistent consequences / first test
Date: 2026-10-17. Source parent: PR #50 research/v18b4-multi-actor-h0 at 219d12d348d8998c63f14c5133ae50f70ba5f0df.
Status: **standalone snapshot-first persistence laboratory; no integrated network server, no physical transaction journal and no release promotion**.

## Source/contracts retrieved
B4 H0 is a laboratory arbitration experiment (5376 authored cases; M3/M4 arbiter code identical and no independent speed comparison). HF-01 branch has sequence-bound user feedback and the later terminal-state regression fix, not tested in-game. The upstream B2 encounter contract distinguishes eligibility, publication, execution and historical consequence. A3/A4/A5/A6 hold material/world authority. The original B1 H1 code (#37), B3 true realizers, B4 shared multiplayer physical interactions and end-to-end human testing remain outstanding.

## Scientific programme
Question: Can a bounded deterministic server-authoritative consequence record prevent duplicate debit/grant under duplicates, stale epochs and persistence failures, and permit recovery without inventing physical consequences?

Terms: proposal is not a grant; durable publish is the visibility boundary; receipt is unique by (actor, action) in the scoped model. Epochs bind decisions to state, even if network packets are delayed or reordered. The snapshot-first incumbent constructs a proposed next state, calls a trusted persistence adapter, then updates RAM on success. **The callback is a contract, not actual fsync/atomic rename/WAL or crash-safe I/O.**

Hard invariants: unique action receipts, no second debit for retransmitted actions, known epochs, bounded 64 receipts and explicit exhausted status, no negative balance, fail-without-RAM-change if callback refuses, and matching RAM/persisted state on successful trusted callback. Ledger identity does not authorize A4/A5 physical impulses. No infinite eviction of irreversible receipts.

## Hypotheses / candidates
Incumbent I: snapshot-first, bounded receipt table + epoch check + persistence-before-visible RAM publication.
Challenger C1 (defined, NOT implemented): append-only WAL with commit markers + versioned checkpoint and replay, requiring real filesystem fault injection.
Challenger C2 (defined, NOT implemented): staged intent reservation + idempotent commit/ack split for actual network transport, requiring independent packet and server harness.
Negative control: eager visible mutation before persistence; expected to fail crash/failure-atomicity.
Do not claim a performance winner without executing a real challenger and comparable inputs.

## Executed local C11 tests
New source src/shared/nf_persist18b5.c/h; src/tests/test_persist18b5.c and test_corpus18b5.c. Strict gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror with AddressSanitizer+UBSan unit compiled: 76 checks PASS. First local run failed the capacity assertion because the fixture exhausted balance before reaching capacity; corrected by using zero-cost remaining valid actions. Retain this failure as a test defect, not an engine defect.
Five deterministic corpora compiled with GCC + UBSan and executed:
canonical 128/0 errors; randomized 1024/0; pathological 256/0; boundary 256/0; heldout 1024/0; **2688 total, zero model-vs-expected mismatches**. Seed calibration 0xB518CAFE; held-out 0xB518A11D. These are authored single-event isolated snapshots; they do not test production storage or external network. The independent expectation is deliberately simple, and there are shared assumptions in input preparation. B4 H0 comparison is not rerun here. No CPU latency sample, network delay measurement, memory-footprint profiling, source-exact GitHub CI run or B5 full engine build was performed. Do not invent them.

## RET classifications
A priori under chosen specification: grant, successful material realization and network notification are different events; a duplicate should not make a second consequence.
Formally specified (not machine-proved): exactly-once scoped (actor,action) memory invariant, no negative resources, epoch consistency and before-visible local callback ordering; no general disk crash proof.
Empirically verified: isolated C11 local unit (76 checks) and authored five-stratum 2688 snapshots. NOT server/network/disk durability.
Highly predictable: replay-safe receipts help with transport retransmissions and failure-atomic callbacks on modeled state.
Uncertain: event-ID namespace changes after reconnect, actual journal tear, torn rename/fsync, partial physical+registry sync, serialization ABI, capacity/performance and network packet loss.
Unresolved: full B5 durability semantics, clean EEC server adapter, B3/B4 material realization, executable independent challengers, human responsiveness, original B1 provenance #37, licensing #36 and main promotion #30.

## Mandatory gates — status
G0 original baseline and source SHA / exact checkout repeat: PARTIAL.
G1 full independent model comparison (I versus C1/C2): OPEN.
G2 canonical/random/pathological/boundary/heldout scoped unit corpus: PASS local only.
G3 authoritative A6 physical+registry atomic outcome: OPEN.
G4 restart/crash/torn-write durable replay, duplicate/loss/reorder under network emulator: OPEN.
G5 fairness/authorization/identity, no double spends across reconnect and multiple actors: OPEN.
G6 strict GCC/Clang sanitizers, full engine CTest and CI: OPEN (local GCC unit only).
G7 real CPU/frame/network latency and human-facing consistent acknowledgment: OPEN.
G8 legal/provenance/governance and maintainer review: OPEN.

## Selection and next implementation
Candidate I is a **provisional laboratory incumbent** only. No comparative winner or release exit candidate is validated. Diagnose EEC pending frontier, implement actual versioned durable checkpoint/WAL before replay, exercise full crash and network adversarial suites, compare I vs C1/C2 using independent oracle on identical corpora. Retain M4 B2 physical eligibility and B4 client HF-01 identity as upstream contracts. All code and evidence on a dedicated draft PR; release HOLD.
