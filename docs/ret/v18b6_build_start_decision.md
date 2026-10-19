# nightfall!punk corrected 1.8B.6 — RET Engineering Implementation Plan & Build-Start Gate
Date: 2026-10-19. PRAXIS / SIGNAL / STEWARD. **Research parent** `aaae30e7082f5ceaea198fe4a37f6c02692bab84`. This is not a production release.

## Preflight readiness decision
| Mandatory criterion | Evidence | Outcome |
|---|---|---|
| Correct build identity | `docs/evidence/v18b6_master_ledger.json`, 72 migration decisions and 18 OPEN gates | PASS for research ownership |
| Reproducible H0 parent | 1.8B.6 migration workflow `39335949137` GCC/Clang/ASan+UBSan PASS | PASS for original bounded H0 |
| Documented source lineage | Git branch parent pinned, old B7 documents unchanged | PASS for scoped research |
| Repository write authority | Connected account has `push` and `admin` GitHub permissions | PASS for separate review branch, not collective merge governance |
| Canonical production baseline | `main` divergence of six commits; v0.9 README, v1.5 canonical dashboard, CMake 1.6.2 | HOLD |
| Canonical authoritative ledger write | Multiple source-of-truth conventions; no maintainer-approved B6 promotion | HOLD; do not mutate canonical ledger |
| GPL/REUSE/provenance on target tree | Source files verified on separate stewardship branch; missing here; issue #36 open | HOLD for distribution |
| Historical B1 source and independent oracle | Original H1 source missing and #37 open | HOLD for H3 science |
| Actual world physics/durable network integration | A6 owner opt-in single actor/crate, B3/4/5 H0, no full H3 | HOLD |
| New WAL cleanup scoped test gates | New unit fixture and dedicated GitHub CI | PASS only after exact fresh CI completion |

**Decision:** research housekeeping and low-risk reversible source-level bug-fix work **AUTHORIZED** on a separate branch with mandatory exact-source strict C11 regression. **Do not begin accepted full-engine 1.8B.6 production implementation** until baseline, rights, one-authority causal integration plan, and independent acceptance are resolved. This is a two-gate distinction, not a waiver of the higher gates.

## Changes performed in this scoped candidate
1. Reviewed actual source before editing; modified only `src/shared/nf_wal18b7.c` failed-open paths where a file descriptor was already acquired. Use existing `nf4_close` to release file descriptors and advisory locks when `flock`, initial parent directory sync or `fstat` validation fails; intentionally retain quarantined corrupt record handles for inspection.
2. Added `src/tests/test_wal18b7.c::test_failed_open_cleanup`: a second opener receives IO_ERROR, has no leaked descriptor, cannot apply; existing first writer still works; after first writer closes, the second can open and replay exactly once.
3. Added `.github/workflows/v18b6-ret-engineering.yml`: strict GCC/Clang, GCC ASan/UBSan, 1.8B.6/old ledger validators, exact SHA-256 reporting, opt-in A5/A6 offline CMake/CTest integration checks; and explicit warning if root GPL/REUSE remains absent.
4. Added this **review-only RET overlay** with evidence inventory, ranked technical debt, licensing/provenance source paths, build-start decision and release holds. No historical test reports or original license contracts replaced.

## Phased engineering plan and gates
**Phase 0 — archive reconciliation / licensing / source-of-truth (P0 mandatory):**
- Compare six main-only commits, net three contracts, to inherited A5 material and C06 relational ownership; mark conflicts as decisions, not merge requests.
- Map `stewardship/2026-10-10-project-reconciliation` GPL/REUSE and third-party notices to B6 source with maintainers; choose canonical version/date story. Preserve original tree and discuss externally owned docs/asset licenses.
- Recover original B1 H1 source or explicit source-identified H2 reproduction; independent M3/M4 oracle; determine canonical B6 build SHA and ledger location with authority.

**Phase 1 — authoritative physical integration (P0 mandatory):**
- Map A6 `nf_world_step_checked` call graph, legacy AI/combat/energy/contamination, material owner and B3; construct one typed per-tick transaction with immutable read/write sets and full domain revision checks.
- Prove one actual A4/A5 applied impulse & contact history through the authorized pipeline. Keep bounded physical frontier independent of semantic topology; coarse/fine C05 support checks and B3 door/push/carry trials. No H0 mock publication used as live physics.

**Phase 2 — durable/network operation (P0 mandatory):**
- Real B5 physical+contract full-world WAL transaction and crash-consistent checkpoint/genesis; partial write/sync and power-loss fault injection; choose behavior after unknown outcome. Do not infer crash recovery solely from POSIX process interruption.
- Authenticated session/adversarial packet loss/reorder/reconnect and safe compensation. Never send trusted physical witness bits from client.

**Phase 3 — RET-HK challenger and responsive FPS (P1 mandatory for build acceptance):**
- Resolve historical 337/360 pending EEC owner frames using rooted reason traces, not eager success.
- Real matched incumbent/challenger tests: M4 vs independent M3 physical/contract truth, causal fidelity, finite budget/fairness and p50/p95/p99 tick/network/render.
- Canonical/randomized/pathological/boundary/held-out tests on actual engine; repro seeds/hardware and no cherry-picking from medians.

**Phase 4 — H4, commons and release governance (P1 to release; some UX enhancements P2):**
- CI all targets including graphical demo and latency; consented H4 accessibility/privacy, provenance audit, CLA/contribution rules, validated GPL/third-party notices and fair compensated community participation.
- Human reviewer accepts source, test, legal obligations; authorized merge only then. Cooperative open-source commercialization possible under license obligations; no rights assumptions.

## Complexity controls
Minimize new abstractions. Keep one A6 author, stable public interfaces, typed material, actor and contract witnesses; no extra global transaction engine, no shadow authority, no unbounded maintenance jobs. Favor profiling and independent falsification over early performance tuning. Revise decisions only when newly observed evidence favors the challenger under mandatory safety gates.

## Exit criteria
H0 unit fix: CI strict GCC/Clang/ASan+UBSan PASS, no new test failures, no historical rewriting and review. H3 build exit: source/rights baseline + real A6/B3/A4/A5/B5 + network/auth recovery + independent compare + bounded performance and full CI; not achieved. H4 release exit: actual human acceptance and cooperative maintainer approval; not achieved.

**Status: H0 housekeeping corrective candidate ONLY. Integrated B6 production build-start: BLOCKED/HOLD.**
