# 1.8C.2 — Concurrent Interaction and Materials, RET H1 (2026-10-22)

PRAXIS / SIGNAL / STEWARD. Baseline `integration/v18c1-research-closeout-h1` / PR #82. Independent logical `nf_ucw18c2` is inherited as existing H0 research, not mistaken for material contact production.

## Executed laboratory
Local compiled artifact: `/mnt/data/v18c2_interaction_lab.c`, executable `/mnt/data/v18c2_interaction_lab`.
Command: `cc -std=c11 -O1 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=undefined -fno-omit-frame-pointer v18c2_interaction_lab.c -o v18c2_interaction_lab && ./v18c2_interaction_lab`.
Result: PASS: 256 each canonical/randomized/pathological contention/boundary/held-out-style seeded stratum (1,280 scenarios). Incumbent vs challenger state differences respectively: 238,229,197,222,230, total 1,116. CPU microbenchmark on one host ~0.001877 seconds for all 1,280 cases; this is **not** 60Hz frame-time evidence. Seeded held-out-style means a reserved group, not independent truly blind held-out provenance.

Incumbent: arrival-order first-eligible per object. Challenger: immutable snapshot priority/actor/op stable arbitration, one accepted write per object, fail-closed invalid revisions, conflict count/pending accounting. Local test compared reverse-input invariance and bounded health/refinement state. Seven **abstract** operations: push, carry, door toggle, repair, interruption, support toggle, selective refinement. Actor–actor/actor–object collision impulse resolution, structural support and real A5 adaptive-grid refinement are NOT simulated. No CTest/CI source-exact GitHub execution has occurred for this local source.

## RET claims
A priori: two conflicting writes to the same authoritative object cannot both be unconditionally committed from the same snapshot; unresolved contacts must not publish; a logical action does not entail physically valid contact.
Formally specified within H1: per-object stable arbitration and basic state bounds. Not a proof of whole-engine correctness.
Empirically validated: 1,280 executed local cases and 1,116 incumbent/challenger disagreements, UBSan no reported error. No regression or integration claim for production A4/A5/A6/B2/B3/B5.
Highly predictable: canonical ordering eliminates arrival permutation dependence for the modeled writes, but can produce starvation and cannot substitute for fair scheduler.
Uncertain: actor–actor and actor–object CCD/manifolds, door/joint constraint impulses, push/carry support load transfer, repair provenance, interruption compensation and selective refinement fidelity under memory budget.
Unresolved: genuine A6 owner-locked positive B2 prepublication, server issued immutable snapshots, real A4/A5 material/contact witnesses, unified A5/B3/B5 durable replay, fairness, worker-count determinism, 60Hz p95/p99, source/rights #30/#36/#37.

## Candidate / gates
Select **bounded deterministic snapshot arbitration as H1 logical challenger**, not as production authority. Do not claim it dominates incumbent physically, because causal fidelity and real latency remain unmeasured. Require genuine A4 contacts, B3 object realizers, A5 support/history, B2 owner binding, joint durable transaction, independently checked held-out fixtures and crash injection prior to release promotion.

## 1.8C.3 handoff
Use existing `nf_ucw18c3` unified framed journal as research reference; expand portable payload to actor–actor and actor–object physical impulses, doors, carry/push, repair/interruption, support and refinement island metadata with compensating transactions. One durable commit identity and replay; no split-success publication. All production and release holds remain.
