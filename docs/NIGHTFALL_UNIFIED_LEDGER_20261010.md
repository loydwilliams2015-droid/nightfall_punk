# NIGHTFALL Unified Research Ledger — 2026-10-10
Canonical ID: NFP-RET-18B2-20261010-R1

## Source channels
1. "Assemble evidence report": historical 1.8B.1 H1 evidence, M0-M4 comparisons.
2. "Define Research Programme": B1–B5 PAR and hard acceptance gates; PR #34.
3. Main Nightfall Dev Chat One: RP-01, independently executed synthetic reference, PR #39.
4. Praxis implementation/handoff: current clean B2 C source / contract handoff, PR #40.
5. GitHub: authoritative versioned source and open issue #37.

## Version evidence
- 1.8A.3: `nf18a3_adjudicate` independently adjudicates geometry and contract, support and clearance and ladders.
- 1.8A.4: `nf18a4_pair_step` resolves dynamic contact and applied impulses (receipts alone are not commits).
- 1.8A.5: `nf18a5_query` exact material witness; `nf18a5_integrated_pair_step_checked`, support step, outbox and WAL for local physical transaction.
- 1.8A.6: `nf_world_step_checked` + tick-owner bridge; no implicit legacy fallback on pending/rejected owner tick.
- 1.8B.1: 18 authored families × 128 variations × five models = 11,520 model evaluations. Historical M4: 2,304/2,304 classifications correct, 0 unsafe, 384 pending; 55/55 assertions; 6,000 held-out seeds with 0 replay mismatches. M4 = provisional H1 incumbent; M3 = mandatory challenger. M0-M2 = ablations. These are **reported historical outcomes**, not newly rerun source-verified results. Historical archive/data hashes were reportedly verified, but exact tested C source was not grafted into repository. #37 OPEN.
- RP-01 standalone C11 synthetic comparison: 49,216 cases × three models = 147,648 model-case evaluations, 0 unsafe approvals and 0 classification differences against common oracle. Corpus canonical 2,048 / randomized 20,000 / pathological 4,096 / boundary 3,072 / held-out 20,000. Held-out single-run timings M4_reference 34.06 ns/case; M3_global_proxy 197.31; M5_local_fast 13.81. Because candidate model workloads and oracle are synthetic/shared, NOT proof M5 beats historical M4 or real-engine latency.
- PR #40 clean B2 source: new `nf_eligibility18b2.{h,c}` and basic tests, CMake wiring to contact lab and main shared library. A3 physical/contract screening plus A5 material query. Not original B1 source; not a complete B1 M0-M4 reproduction. Source retrieval/compile/CTest/live owner integration and publish durable atomicity require independent confirmation.

## Mandatory gates
G1 source ancestry and build data reproducibility; G2 contact/clearance/support/ladder physical authority; G3 contractual authorization; G4 fail-closed pending/stale and cache-epoch changes; G5 no double grants/debits and same-tick determinism; G6 WAL atomic publication/recovery & idempotency; G7 real impulses/history, never fabricated Purple; G8 held-out plus pathological/failure-injection controls; G9 performance/frame responsiveness; G10 branch CI and human graphical acceptance.
Binary gates first. Correctness and causal fidelity outrank microbenchmark cost; instability, starvation, observer interference are disqualifying. Full B2 exit = BLOCKED.

## RET register
- A priori: permission does not establish clearance; material and contract jurisdictions both required.
- Formal: only within explicit individual contracts, not proven end-to-end integration.
- Empirical: historical B1 H1 reports and separately executed synthetic B2 lab; current new GitHub code not independently measured here.
- Highly predictable: cheaper fail-first checks may reduce common rejection path work.
- Uncertain: production CPU savings, cache coherency, multiplayer conflict ordering, camera/AI behavior.
- Unresolved: exact B1 source, independent B1 reproduction, B2 transaction publication, actual world owner, full integration/CI and acceptance.

## Convergence and communication
PR #34 = H0 B1-B5 programme; PR #35 = stewardship; PR #39 = RP-01; PR #40 = clean B2 implementation. #37 = B1 provenance blocker. Keep all draft pending proof; no silent merge to main. GitHub ledger is canonical cross-chat reference. This file does not update ChatGPT conversations' actual messages.

## STEWARD tasks
PRAXIS: replace one-point screening with tested physical/contract witnesses under world tick ownership; wire real prepublish gate/WAL; independent full M0–M4 reproduction.
SIGNAL: run adversarial, boundary, randomized and held-out corpora and real timing; check causal fidelity and player-observable consequences.
STEWARD: SHA-256 source/data identities, toolchain/platform flags, archived logs and failure cases, branch CI and no unsupported promotions.
