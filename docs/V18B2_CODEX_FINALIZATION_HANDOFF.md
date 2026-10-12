# nightfall!punk — Unified Ledger and Codex 1.8B.2 Finalization Handoff
Revision NFP-RET-18B2-CODEX-01 | 2026-10-10 | planning only, no release acceptance

## Source reconciliation
- WorkGPT Assemble evidence report: M4/H1 provisional, M3 challenger, strict provenance; reported historical evidence not verified against missing original archive.
- ChatGPT Define Research Programme: RET classifications and independent incumbent/challenger, canonical/randomized/pathological/boundary/held-out comparisons, correctness, causal fidelity, performance, stability, responsiveness.
- Codex Set up nightfall_punk: current GitHub PR #42 and its detailed implementation/evidence ledger are available; a full independent transcript was not retrievable. Treat GitHub, not chat summaries, as source of executable truth.
- PR #34 B1–B5 PAR; #35 stewardship; #39 research-only; #40 closed premature B2 implementation; #41 new B1 H2 reproduction; #42 active B2 H2 code. Issue #37 original B1 archive OPEN.
- 1.8A.6 is verified B1 predecessor; 1.7A.6 is not an established predecessor. PR #42 is based on the older experimental B2 branch, and must be inspected for unintended inherited source from closed PR #40.

## Evidence discipline / RET
- Historical B1 H1: M4 2304/2304, M3 2176/2304, 55/55 targeted, 6000 property seeds as historical report. The conflicting 38/38 reference in RP-01 needs erratum. The exact tested original C/archive remain unrecovered.
- B1 H2 PR #41: clean new-source C11 reference, 6000 seeded cases reported passing, but no positive commits occur within those seeded cases because successive LCG low bits alternate; a separate 256-state supplement has 3 positive and 253 refusals. It is not original B1 source.
- Earlier B2 proxy: 49,216 input cases times 3 models, one common oracle; not real engine M4/M3 ranking.
- PR #42 B2 H2: 21,504 indexed authored cases (2048 canonical, 8192 randomized, 1024 pathological, 2048 boundary repeated states, 8192 held-out), five policies, 107520 case IDs per repeat, three dependent repeats. M3/M4 zero correctness/unsafe/replay mismatches in authored data. 66 targeted assertions, nine offline tests under GCC/Clang/sanitizers, 28 full engine tests, network smoke. PR reports all 11 CI checks green at f30ef6b48179afd28f0911b9922cdcf41af6fd33.
- PR #42 measured held-out M4 p50 32.25ns and M3 p50 202.375ns batch averaged library calls. Not whole tick or player responsiveness. M4 provisional LAB candidate, M3 legitimate challenger, M0–M2 negative controls.
- RET: a priori material+contract conjunction; narrow formal arguments about bounded grid arithmetic/LCG parity; empirical evidence only for executed listed corpora; low-cost preflight benefits highly predictable, general world performance uncertain; original B1 source and live integration unresolved.

## Binding programme contracts
Deterministic server-authored world and exactly one A6 tick owner. A3 geometry/traversal AND contracts, A4 real applied impulses, A5 authoritative canonical/fine material/epochs and transactional contact history, 1.7 typed cell/nexus/scopes. Actor knowledge is not server truth. Unknown and stale data fail closed, never invisible grants. Same-tick arbitration avoids double grants/debits. Journal failure does not partially publish; Purple must prove mutual material dependency. Observer must not alter authoritative outcomes.
PAR means an on-par comparison sheet, not a backronym. Maturity H0 plan / H1 lab / H2 source+integration experiment / H3 system / H4 human proof must remain distinct.

## Codex finalization execution plan
0. READ & FREEZE: fetch exact PR #42 head SHA, all its evidence CSVs, scripts, CI results, PR #41 original hash, H1 ledger and Issue #37. Record toolchain, compiler flags, branch/base/parents, dataset SHA256. Inspect PR #42 ancestry because parent contains previously closed experimental B2 code.
1. B1 LINEAGE: attempt to find archived original source and validate SHA256. If not recovered, keep #37 open and explicitly seek human approval to treat clean H2 as new baseline; never claim H1 original recompilation.
2. REPRODUCE BASELINES: clean checkout; run bash v18b2.sh all; bash v18b2.sh engine; bash tools/check_v18b1_pr41.sh; capture stdout/stderr, exits, all raw cases, negative control failures, seed identity and CI check outcomes.
3. LIVE B2 WORLD-TICK: integrate B2 intent processing into a real authoritative NfWorld/A6 tick owner. Produce actor/object registry snapshots at tick boundary, bind world/material/actor/object epochs, A3 clearance/reach/support/ladder and A5 full canonical/fine sweeps, contracts/credentials/shared set/slots/inventory preconditions. Client sends intent only. No duplicate movement owner, stale approval or silent fallback.
4. TRANSACTIONAL AUTHORITY: deterministic 64-proposal bounded arbitration, 16-shared-slot capacity or justified revision, resolved membership, fairness and conflicting read/write sets. Validate at prepublication under owner lock. Stage resource reservation, debits, ownership, history, WAL; durable-before-visible commit or no change. Crash/torn journal/reboot/replay recovery tests; no double spend/grant and no partial publication. Return explicit PENDING/FAILED.
5. MATERIAL CAUSALITY: separate immutable material identity epoch from changing object state revision. A4 impulses only if applied, A5 summaries/events only after real commit. B3 moving supports and multi-stage action realization are separate future scope; do not claim they passed in B2.
6. INDEPENDENT SIGNAL TESTS: paired M3/M4 canonical/randomized/pathological/boundary/held-out with independently constructed truth; failures and seed artifacts retained. One-ULP and nonfinite geometry, stale cache, permutations, 2/4/8/16 competing actors, cooperative/shared and exclusive claims, journal exhaustion, lost/reordered network messages, cross-tick fairness. Compare correctness, causal precision/recall, conservation, deterministic replay, p50/p95/p99 and frame overruns.
7. FULL BUILD/ACCEPTANCE: strict GCC+Clang C11 -Werror ASan/UBSan, full Linux/Pop!_OS client+server, CI all suites, native 60Hz instrumented physics, network 4 clients+bot, graphical interactions, user-facing input-to-result time. Explicit human acceptance only with recorded proof; no silent source promotion.
8. EXIT REPORT: present commit/source/data SHA256, tests and failure logs, model PAR, RET six-category claims, PASS/FAIL/OPEN gating, performance with correct scope, unresolved source and feature distinctions, recommendation HOLD or MERGE. Do not merge into main until B2 mandatory gates accepted by human.

## Roles and delivery
PRAXIS owns real engine implementation; SIGNAL owns independent challenge and UX causal observations; STEWARD owns provenance, ledgers, reproducibility, acceptance. GitHub canonical. These updates cannot edit previous ChatGPT/Work/Codex chat messages directly; other chats must read the shared ledger URL.
Sources: GitHub PR #34 #35 #39 #40 #41 #42, Issue #37, docs/LEDGER_V1.8B1_SMART_OBJECT_H1_HANDOFF.md, docs/LEDGER_V1.8B2_PHYSICAL_CONTRACTUAL_ELIGIBILITY.md.
