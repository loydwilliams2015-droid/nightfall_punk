# nightfall!punk 1.8C.1 — UCW-1 C integration and RET evidence

PRAXIS / SIGNAL / STEWARD • 2026-10-20 • experimental review branch only.

## Implemented in actual C11 source
- `src/shared/nf_ucw18c1.h/.c`: a versioned, bounded `UcwWorld` and `UcwProposal` structure with actor, action, object, contract, authority epoch, material and contractual deltas, immutable read revision and priority. Canonical conflict ordering independent of proposal input order. Refuse stale epoch, duplicate receipts, contested same-tick object/contract, overflow, insufficient resources and unproven state.
- A deterministic pending-frontier continuation function: an incomplete dependency budget leaves the region explicitly `UCW_PENDING`; `ucw_stage` refuses to publish a successor until it is resolved. No proof that this H0 counter is a physical causal frontier in the A4/A5 world.
- POSIX journal proof-of-concept: version-marked fixed-width record with checksum, file locking, `fdatasync`, restart scan and quarantine on unknown/torn data. **CRITICAL:** the fixed-width storage format does NOT serialize the per-action receipt list, material details or real A4/A5 world. Code intentionally refuses any receipt-bearing candidate as `UCW_INVALID` rather than falsely claiming crash-atomic A6/B3/B5 integration. Only empty-action logical ticks can be replayed. This is NOT an accepted durable-world envelope or a complete WAL.
- Explicit CMake integration to the headless `nightfall_embody_h1` test target and CTest for the UCW module. No server/client API wiring, production multiplayer authority, or high-resolution frame-time claims.

## RET epistemic classes
**A priori:** an action and its persisted receipt cannot be claimed atomic if the journal excludes the receipt; a pending causal dependency cannot be reported fully resolved.
**Formally specified:** stable comparator, staging identity and revision checks, single-tick resource/contract conflicts, pending/resolved/quarantined states, versioned record header and checksum.
**Fresh empirically executed:** report only completed GitHub CI runs on an exact SHA; first strict-C CI initially failed over missing POSIX `fcntl.h` in the test harness, corrected without changing production logic.
**Highly predictable:** sorting proposal arrays avoids arrival-order nondeterminism for this scope; a staged snapshot can reduce shared mutation, but broader physics and disk-latency performance remain unmeasured.
**Uncertain:** real crash power-cycle durability, fsync latency tails, platform determinism, multi-actor actual physical interactions, frontier material dependency tracking and independent physical world oracle.
**Unresolved:** the 1.8B.6 production gates plus complete B5 action-bearing disk envelope, staged actual A4/A5 material and B3 contractual publication, authenticated server witnesses, replication, world-scale timings and legal/governance acceptance.

## Compared candidates
- **Incumbent:** corrected 1.8B.6 A6/A4/A5/B2/B3 physical pipeline and limited B5 callback or debit WAL H0. Existing source remains unedited.
- **Challenger:** UCW-1 typed transaction staging, sorted proposals and bounded pending-frontier + extremely conservative disk journal. New C source is separate from the incumbent until real material staging can be added.
- **Mandatory correctness:** challenger must not fabricate a committed world for a receipt-bearing transaction. This rejection is intentional even if it means ZERO such transactions have been durably processed. Do not call a zero-completion path a completed durability upgrade.
- **Performance:** no comparative p50/p95/p99 or full workload timing for these different domains; a standalone microtest is not throughput acceptance.
- **Governance:** original historical files and canonical ledgers unchanged; code is unmerged. CC0 applies only to separately authorized authored prose and cannot override GPL-3.0-or-later software obligations.

## Test coverage
`src/tests/test_ucw18c1.c`: five strata × 256 cases = **1,280** candidate comparisons under canonical, randomized, pathological, boundary and held-out test families. Proposed nonoverlapping calls compare canonical order against reversed arrival, reject stale/contested/insufficient actions, preserve unsigned counters, require pending-to-resolved transitions, verify empty-action journal replay, and refuse incomplete receipt serialization. Strict GCC/Clang and GCC ASan/UBSan via `.github/workflows/v18c1-ucw-h0.yml`; full headless predecessor CTest.

## Next mandatory engineering decisions
1. Replace H0 fixed 64-byte journal with complete versioned A6/A4/A5/B3/B5 envelope encoding, checksum, generation, schema migration, action receipts and conservative size checks; implement an independent decoder and fault oracle. Verify semantic consistency after restart, not merely two integer balances.
2. Move the actual physics and contractual staging to one **prepublication** call; never permit A5 world effects to publish ahead of B5 durable confirmation. Compare synchronous commit vs worker-owned group commit (with authoritative acknowledgment pending).
3. Make local causal frontiers dependency-aware: when a support/pressure/contact chain remains incomplete, block only materially dependent transactions and conservatively carry exact state forward.
4. After transaction correctness, integrate true multiple body contact, deterministic conflict arbitration and documented fairness; test cross-worker counts and distinct compilers.
5. Measure actual representative hardware and 60Hz p50/p95/p99; select challengers only after correctness, causal completeness, stability and security gates pass.

**Release, accepted production baseline, canonical ledger promotion and full-engine acceptance: HOLD.**
