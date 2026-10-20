# nightfall!punk 1.8C.0 — Baseline, acceptance and RET handoff
PRAXIS / SIGNAL / STEWARD — 2026-10-20 — research-only branch `research/v18c0-baseline-acceptance-h0`.

## Verified source ancestry
Base is corrected 1.8B.6 PR #62 head `a01276347c177e4660337f10b5170a27ed08b02e`, an **unmerged draft chain** (PR #58 migration, #59 housekeeping, #60 physics, #61 B2 authority, #62 Pac-Man and cultural direction). Original 1.8B.5 `b5870171f3d2ed6794c349944b3ec2eda70e6219` and corrected B6 evidence/contract documents are retained, not overwritten. `main` baseline has an unresolved ancestry difference documented in issue #30.

## Acceptance hierarchy
1. **Research H0 ACCEPT only:** source-locked scoped data; readable direct commands; constrained compare; no production acceptance.
2. **H1 physics:** previously executed offline A6/A4/A5/B2/B3 physical tests at PR #61, but not real multi-actor/world crash-atomic.
3. **H3 full-world:** NOT ACCEPTED: material+contract B3+A6+A5+B5 one durable envelope, authenticated network, multi-actor scheduler and independent oracle absent.
4. **Distribution/release:** HOLD: root GPL/REUSE legal lineage still requires reviewed incorporation from `stewardship/2026-10-10-project-reconciliation`, rights issue #36; B1 original H1 source issue #37; governance and canonical ledger approval unresolved.

## Candidate comparison (justified scope)
**Incumbent I:** `nf_b5_apply` callback-staged snapshot model (`src/shared/nf_persist18b5.c`). Pure in-memory state remains unchanged if persistence callback reports failure. This is not a filesystem WAL.
**Challenger C:** `nf4_apply` POSIX file-backed append+fdatasync/replay model (`src/shared/nf_wal18b7.c`). Checksums and quarantine after uncertain operations; no durable world/actor/physical B3 payload.
**Independent truth baseline O:** a third handwritten integer balance+epoch+receipt oracle in `src/tests/test_v18c0_b5_wal_comparison.c`; not a duplicate code call to either implementation. It checks a **shared debit subset** (only positive actor/action, fixed object=77). C's wire representation does not contain an object ID and therefore MUST NOT be promoted as equivalent for real B3 contractual identity.

**Designed tests:** 320 seeded scenarios, five strata (canonical, randomized, pathological, boundary, held-out) × 64, 35 proposals per scenario = 11,200 authored attempts. Invalid, stale and insufficient funds, duplicates, altered payload, callback-failure nonpublication, actual POSIX replay, deterministic state and conservation. GCC/Clang/ASan+UBSan; offline headless physical CTest, frozen historic-source ledger integrity. The tests count as executed only after exact GitHub Actions success.

## Mandatory gates and risk
- **G00** Source/contract/data provenance: candidate research lineage known, accepted canonical production SHA/merge authority not established.
- **G01** Complete B2 credentials, actor geometry and immutable physical material witnesses from authenticated live server, independent M3 oracle.
- **G02** Versioned crash-atomic full A6/A4/A5+B3/B5 record including action/object/contract identities; no publication before durable commit; power-loss and lost ACK recovery.
- **G03** True A6 multi-actor scheduling and reciprocal body forces, conflict fairness and no duplicate owner.
- **G04** Representative perf: actual hardware p50/p95/p99 physical/input/client latency, throughput, memory, durable sync and degradation.
- **G05** Licensing, user/contributor rights, dependency provenance and actual maintainer signoff, with CC0 limited to authored documentation only.
- **G06** Independent five-strata/held-out engine-wide truth and adversarial physical/multiplayer oracles, human H4 testing.
The parent ledger's 18 unresolved groups and 10 production questions survive. No actual 1.8C milestone may close by calling this common-subset data comparison a complete world acceptance.

## RET
**A priori:** permission != application != durable receipt; missing acknowledgement does not prove failure; one true A6 world owner; no false grant under uncertainty.
**Formally specified:** the restricted debit ledger comparison and deterministic oracle invariants, conditional proposal identities and schema-bound limits.
**Empirically validated:** historical B6 tests only at their own pinned commits; new 1.8C tests to be counted only after executing CI.
**Highly predictable:** fsync-based file logging should improve recovery observability over mock callbacks but may cost more I/O and latency; evidence cannot yet substantiate a superiority claim.
**Uncertain:** hardware power-cut semantics, real-world performance, network/adversarial conditions and human gameplay.
**Unresolved:** complete B6 handoff full-engine gates above; **release HOLD**.

## Proposed exit
Promote the B5 callback model as a *simple reference oracle* and the WAL as *isolated file durability challenger* only if their common-subset test passes. Do not call either a production exit candidate; choose **H0 comparative research ACCEPT / H3 production HOLD** if tests pass. Use the demonstrated missing object/action/contract envelope and independent true A6 integration as the first C.1 engineering frontier. Keep canonical ledgers untouched; this document is a review-branch overlay, not historical rewriting.
