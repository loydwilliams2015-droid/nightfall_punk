# v1.8A source lineage, decision authority and open integration gates

**Audit date:** 2026-10-04. Repository `loydwilliams2015-droid/nightfall_punk`. This record is intentionally additive to earlier handoffs: it corrects *source-location status at this reconciliation branch* without rewriting their historical wording.

## Chain of custody

| Milestone | GitHub relationship | Evidence decision | Tested source before this reconciliation |
|---|---|---|---|
| v1.7E | `build/v1.7e-graphical-observability` / `archive/v1.7e-observer-par0` | graphical diagnostic baseline, not live physical FPS | branch contains code |
| v1.8A.1 | `build/v1.8a1-contact-truth-laboratory` / `archive/v1.8a1-contact-truth-h1` | contact/TOI and conservative pending H1 | branch contains contact code |
| v1.8A.2 | `build/v1.8a2-capsule-motor-condensed-history` | policy lock and local H1 tests | separately supplied laboratory archive |
| v1.8A.3 | `build/v1.8a3-shape-traversal` | **LOCK Model C**: swept capsule + independent foot patch; geometry AND traversal | separately supplied laboratory archive |
| v1.8A.4 | `build/v1.8a4-dynamic-interaction` | **LOCK Model E**: motor-driven reciprocal angular/friction impulses + adaptive 2→4→6 | separately supplied laboratory archive |
| v1.8A.5 | `build/v1.8a5-canonical-history` | **LOCK Model D grid and Model D condensed history** | separately supplied laboratory archive |
| v1.8A.5 closure | `build/v1.8a5-closure-h1` | H1 integrated local world/contact/history proof; **not H3 production** | separately supplied closure archive |
| Source reconciliation | `integration/v1.8a5-h1-source-provenance` | graft previously local C source, tests, scripts, provenance and offline CI | this review branch; verify CI independently |

The development line starts from v1.7E and the 1.8A.1 branch; A.2–A.5 and the closure branch each add handoff decisions. The original historical branches remain unchanged. `main` is separately diverged (audit: A.5 closure was 293 commits ahead and six behind main); no automatic rebasing or merge to main is authorized by an H1 result. Other historic `agent/`, `build/`, `archive/`, `review/` branches are retained until separately reviewed, never silently deleted.

## Snapshot and data fingerprints

- Historical source archive `nightfall-v18a5-closure-source.tar.gz`: SHA-256 `7be596305111a2846778a5aa7692d107b75ca82a5cbbcb35149207b6b630094d`.
- Original local closure PAR `nightfall-v18a5-closure-PAR.md`: SHA-256 `024fff2b02af323856318b2e0bb22f304486888d994d694adedb5144c92aa136`.
- Original 1,400-case CSV `nightfall-v18a5-closure-samples.csv`: SHA-256 `87338da586ff4d9993b36ead8654628f4d9e9a37455bcf7ed04a6cf191fa00dd`.
- The source package contains upstream files and local generated artifacts; this review graft intentionally selects changed source, build scripts and research docs, **not** build outputs or binary data.
- The copied `evidence/v18a5/` fixture/PAR files are audit snapshots. The scripted re-run is the independent reproduction. Store both; do not overwrite historical test counts silently.

## Current scientific gates

- Local closure test assertion count: 46/46 PASS, inherited A.1–A.5 regressions reported pass, standalone offline CMake/CTest: 4/4 PASS on the local source snapshot. Corpus: seven deliberately adversarial strata x 200 perturbations = 1,400 **not a natural gameplay sample**.
- Both the legacy end-position query and unsafe policy controls are retained in tests. Their failures are negative controls, not 'production behavior'.
- Snapshot WAL is single-process, POSIX, same ABI; it is not a distributed transactional event store or a cross-platform long-term file format.
- Fine loading is under already-authoritative canonical parent chunks; absent canonical chunk streaming and full moving-world support remain open.
- **No live player control integration, full ENet/raylib build certification, authoritative multiplayer/reconciliation, native P95/P99 streaming measurements or H4 human validation yet.**

## Required next PRs (do not silently merge them into this H1 source graft)

1. `A6-01`: install this source behind an explicitly gated real-world tick and preserve incumbent as control; validate motor, support, camera, actor AI and weapon correspondence.
2. `A6-02`: global material/Cell/Nexus and 3D chunk-streaming contracts, moving supports/rotating dynamic bodies, spatial broadphase and hardware latency data.
3. `A6-03`: portable authoritative WAL with multiwriter/recovery and client/server reconciliation, then H3 benchmarks and H4 human tests.
4. `REPO-01`: compare six main-only commits/docs with 1.8A branch before any main promotion; prohibit blind fast-forward or force-push.

## Governance and acknowledgement

All changes must be traceable to reviewed pull requests, explicit acceptance criteria and recorded results. No retrospective change to earlier ledgers shall turn an H1 lab finding into H3 production success. Correct mistakes with append-only errata, and keep review blockers visible.
