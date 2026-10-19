# nightfall!punk 1.8B.6 — Housekeeping Audit and Unresolved Issue Register
RET / PRAXIS / SIGNAL / STEWARD • 2026-10-19 • review-only engineering branch.

## Scientific housekeeping findings and remedy
1. **Authority/ancestry ambiguity — BLOCKING:** `main` `bb377ee...` remains v0.9-era and differs by six main-only commits from the >460-commit experimental development line. Issue #30 remains OPEN. Remedy: review the six main-only commits, map conflicting seed/semantic contracts, create an explicit integration plan and test a synthetic merge *without silently merging*.
2. **Three competing canonical labels — BLOCKING:** `docs/LEDGER.md` is master, `docs/PROJECT_STATE_INDEX.md` is a dated canonical dashboard, and corrected `docs/evidence/v18b6_master_ledger.json` is a candidate new research index. Remedy: explicit stewardship/reviewer ownership of promotion; maintain only this branch's append-only RET overlay meanwhile. Do not rewrite older ledgers.
3. **Software rights direction not applied to target tree — BLOCKING distribution:** Issue #36 records GPL-3.0-or-later plus completed files on a separate line, but none of LICENSE/REUSE.toml/LICENSES/... is present on this source tree. Remedy: locate audited stewardship commit(s), independently inspect copyright/dependency/CLA/other rights, create separate reviewed graft. Open-source copyleft **allows commercial activity**; neither philosophy licenses unowned art/assets nor a CLA bypasses author rights.
4. **Missing historical source — BLOCKING full comparative claim:** Original B1 H1 tested archive absent; issues #37 and test-count erratum unresolved. New H2 B1 and B2/M3/M4 alternatives cannot inherit the claimed old H1 measurements. Remedy: verify original archive SHA or explicitly document reproduction+new SHA and run matched comparisons.
5. **Single world authority incomplete — BLOCKING H3:** A6 H1 owner handles one actor+crate with opt-in checked step; legacy combat/AI/energy/contamination not one material transaction. Remedy: end-to-end single-owner tick integration with independent causal oracle, no extra world writers and no silent fallback.
6. **H0 physics effects mistaken for actual simulation — BLOCKING H3:** Group2 F01–F09 discrete doors, push, carry, repair mock analogues; B3 callback isn't verified A4/A5. Remedy: integrate actual A3/A4/A5 contacts and reciprocal impulses then explicitly test material histories.
7. **Network and durable world persistence — BLOCKING H3:** Group3 grants only; Group4 POSIX debit WAL is one resource ledger, not joint A6 material+contract durable publication or power-cut proof. Authenticate sessions, replay, tear/ack unknown, no two grants. Without libsodium the CMake warning explicitly points to non-cryptographic localhost scaffold: production deployment disallowed until verified.
8. **Toolchain and build maturity drift — IMPORTANT:** `CMakeLists.txt` version 1.6.2, root `README.md` v0.9, research target 1.8B.6; distinguish runnable targets vs accepted features in one reviewable README/manifest after baseline acceptance.
9. **Wildcard source collection — IMPORTANT:** `file(GLOB NF_EMBODY_SHARED src/shared/*.c)` may admit unrelated H0 code, experimental modules and new dependencies into a laboratory target. Replace with an explicit manifest after auditing linked symbol dependencies, CTest and benchmarks; do not blindly remove code.
10. **Resource cleanup — NARROW FIX NOW:** `nf4_open` retained file descriptors on `flock`, directory sync or stat failure. New review branch patch closes handles on those failures without touching a quarantined, inspectable corrupt prefix. Add conflicting-opener regression and re-run GCC/Clang/sanitizers.
11. **Evidence and performance integrity — BLOCKING perf release:** synthetic model `clock()` timing does not establish live 60Hz p95/p99 frame time, fairness, long-tail backlog or full networking. Pending EEC counts and M3/M4 independent truth tests remain outstanding.
12. **Human and ecological evaluation — DEFER until real build:** no H4 user testing or full graphical acceptance, privacy consent, accessibility matrix or production biome/8km² evidence.

## Priority queue (correctness / causal fidelity first)
| ID | Class | Priority | Owned by | Gate / next proof |
|---|---|---|---|---|
| HK01 | mandatory production | P0 | STEWARD | Default/main six-only ancestry #30 reconciled |
| HK02 | mandatory production | P0 | STEWARD | Accepted canonical ledger and authority identified |
| HK03 | mandatory distribution | P0 | STEWARD | License/rights and third-party material audit #36 completed in target tree |
| HK04 | mandatory comparative science | P0 | SIGNAL | B1 H1 source recovery or exact-labeled H2; independent M3/M4 oracle |
| HK05 | mandatory H3 | P0 | PRAXIS | Unique complete A6 tick including legacy combat/energy/AI and A4/A5/B3/B4 |
| HK06 | mandatory H3 | P0 | PRAXIS | Real material/geometry contact consequences; no phantom push/carry |
| HK07 | mandatory H3 | P0 | PRAXIS/STEWARD | Full-world A6+B5 crash/recovery, authenticated network replay |
| HK08 | mandatory H3/performance | P1 | SIGNAL | Actual EEC 337/360 root cause and representative tick latency |
| HK09 | mandatory promotion | P1 | STEWARD/SIGNAL | Full builds, independent oracles, H4 and reviewer signoff |
| HK10 | scoped immediate fix | P1 | PRAXIS | Close failed-open journal descriptor and regression-test exact source |
| HK11 | valuable deferred | P2 | PRAXIS | Explicit CMake source list and target dependency manifest |
| HK12 | valuable deferred | P2 | STEWARD | Harmonize README, CMake version, dashboard after true baseline adoption |
| HK13 | valuable deferred | P2 | SIGNAL | Real 60Hz p50/p95/p99 profiling, retry/backpressure fairness |
| HK14 | valuable deferred | P2 | STEWARD | Document full network/data consent, accessibility and community process |

**Status semantics:** "mandatory" here means prerequisite for *the specified H3/H4 milestone or release*, not an instruction to auto-merge any branch. The scoped WAL defect is reversible H1 housekeeping, not clearance of HK01–HK09.

## Minimal consolidation rules
- Prefer a stable A6 owner with typed adapters over another independent world authority.
- Retain old H1/H2 and lab-only B7 filenames as archival provenance until a mechanically verified compatibility migration.
- Keep discrete/mock, physical, human and durable receipts structurally distinct.
- Collapse duplicate prose by **linking to the single inherited contract**, not by deleting disagreements or accepted historical ledger entries.
- Prioritize independent evidence and compiler/CI gates over microbenchmark winners.

**H3 longitudinal sweep disposition: SWEEP NOW** for source and canonical branch ancestry because many generations have accumulated with explicit nomenclature/authority drift. Keep it scoped to the six main-only commits, accepted ledger ownership, and related A6/B1 contractual lineage first, rather than uncontrolled historical rewriting.
