# nightfall!punk 1.8B.7A Group 2 — F01–F09 RET first-test handoff
Date: 2026-10-19. Review branch: build/v18b7a-g2-physical-interactions-h0. Parent: Group 1 PR #53, commit 768c0d156219932efc6be5620f2539427300fd7e (unmerged). Do not claim this is the default branch, a final A4/A5/A6 integration, or a physical world release.

## Governing commitment
**High-fidelity simplicity with occasional finer checks**, sharing one A6 world coordinate/authority and preserving typed EEC-01 permission/commitment/execution distinctions. Each of seven B3 families has a bounded test-model effect or explicitly fail-closed result. Multi-actor arbitration and persistent registry durability stay separate in Group 3/4. No phantom physical impulse/contact A5 history: H0 local state receipts are NOT A4/A5 receipts.

## F01–F09 design patterns, source state, and holds

| ID | Selected minimal pattern | Laboratory source scope | Mandatory integrated remaining work |
|---|---|---|---|
| F01 Door motion | Progressively moved slider + obstruction and endpoint latch; finer swept geometry only when necessary | integer position 0–100, max ±10/tick, obstruction interval PENDING, latch only at endpoints | real hinge/slider body and joint, A3/A5 dynamic collision/latched impulses |
| F02 Switch | Explicit power-known gate, signal published after real activation | known power toggles state, unknown waits | physical switch contact, downstream wire/signal network and atomic delivery |
| F03 Pickup | Single holder + support/possession transfer; no duplicate transfer | floor-supported→held-by actor in one copied state | physical support removal + inventory/attachment authority and multi-actor races |
| F04 Push | Equal/opposite impulse bookkeeping; never pass display-only as physics | signed integer J moves actor/object momenta symmetrically, fixed limits | actual 3D A4 reciprocal impulses, friction, restitution, A5 contact history |
| F05 Carry | Versioned ownership/attachment with explicit attach/release | one actor can attach and release; no two holders | joints/constraints, actor-object collision/support and cooperative carrying |
| F06 Repair | Bounded work with explicit resource conservation | 1–10 repair units per action consume equal supplies; health upper bound | actual material property repair, partial work and damage interaction |
| F07 Interruption | Postcommit history immutable; correction via new event | distinct INTERUPT release action increments history; no erasure | full cancel/reserve/compensate across B3/B4/B5 and real physical effects |
| F08 Contact/refinement | Canonical parent/material identity is stable under state change | epochs advance; no actual voxel refinement tested | selective 0.25–0.5m A5 physical/island checks and 1m canonical identity |
| F09 Responsive fidelity | Coarse minimal primary rule, refine on uncertain passage | deterministic integer lab, five test strata | game controller input→contact→A6 commit→client latency, 60Hz and human H4 |

No automatic promotion from a passing local toy model. Door position and 1D impulse are deliberately simplified *test analogues*. Physical eligibility flags in H0 are mock trusted inputs, never authenticated production server evidence. A successful mock publisher callback does not establish fsync or atomic durable WAL.

## Source and provenance
src/shared/nf_physical18b7.h / .c; src/tests/test_physical18b7.c; .github/workflows/v18b7a-g2-physical.yml. Local compiler tests were performed from source bytes that match exact GitHub object IDs:
- C Git blob 392f77533c789501abb04d5cd83c8b28717d571a ; SHA-256 61ca225724160b193010d51a48ae43c372b7b72e689ea156282c1cb8ba7eced2
- H Git blob 669f6510877a8b8c3edd9b2e12d6b1bbdedd82ec ; SHA-256 fa88b2a4335d615f14ebf5079405ea34b2771d1453f30eae5e7ad0da3fec6d47
- tests Git blob c7e87aafeeb9bcd8ff320bdb86e59a877bf5ce74 ; SHA-256 2d1b58bde6e7935fea43d5d8e1f6562101c28ea7d922ccc716d4c7527784a469
Direct clone was not required; GitHub files were pulled through connected repository action and matched via blob identity. Parent B3 header nf_interact18b3.h is used for action type enums, not B3 callback execution; this component does not invoke actual B3 realization callback.

## Fresh executed tests
- GCC  -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror: PASS.
- Clang same strict C11 warnings: PASS.
- GCC -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer: PASS, ASAN_OPTIONS=detect_leaks=0.
- Each: **5,248 corpus cases and 18,153 assertion checks**, **1,524 applied cases**, zero assertion mismatches. These repetitions use the SAME corpus/fixtures and are not independent samples.
- Strata: canonical 128, randomized 2,048, pathological 512 (obstruction), boundary 512 (stale/overflow/receipt cap/resource/power), held-out 2,048 (seed 0xF010700A). Seeds for others: 0xF0100001/02/03/04.
- Separate targeted multi-step fixtures exercised all seven kinds, same-action altered-payload rejection, unknown/power obstruction PENDING, stale epoch, mock failed publication no RAM change, impulse sum and repair supplies conservation, receipt existence, carry release and interrupt release.
- Controlled expectation check shares the model's declared contract. No independently implemented continuous collision oracle, real material topology truth or world-scale uncertainty confidence interval.
- Whole-program clock() timings from local process are **NOT valid FPS/tick latency evidence**. No independent production implementation comparison, M3/M4 result, actual player AI rounds or H4 human playtests.

## RET evidence status
A priori: authorization and execution differ; published history cannot be undone retroactively; causal sums require stated sources. Formally specified: finite integer model, explicit case transitions, separate copy-before-publish, strict epoch/replay/capacity checks, and bounded impulse/resource accounting; this is not a mathematical proof for a physical engine. Empirically verified: only the above local test model. Highly predictable: ordinary geometry checks + infrequent fine checks may be a favorable fidelity/cost tradeoff, but unmeasured on live A3/A5. Uncertain: hinge/contact accuracy, cooperative carry/repair, user input response, solver performance. Unresolved: B3 seven family live A4/A5, B4 multi-actor A6, B5 durable world+registry journal, pending 337/360 owner frames, licensing #36, historical H1 #37, branch #30.

## Gate disposition
- G0 provenance exact source: PASS for three new files, inherited B3 contracts traced; history H1 OPEN.
- G1 model positive/negative fail-closed: PASS in authored local scope.
- G2 **real seven kinds in live A6/physics**: OPEN.
- G3 A4 impulse/A5 history and energy: OPEN (integer momentum surrogate only).
- G4 real atomic B5 publication and interruption/replay: OPEN.
- G5 five reproducible strata and expected checks: PASS scoped H0, independent continuous truth OPEN.
- G6 local GCC/Clang/ASan/UBSan PASS; full source-head GitHub CI separately check before calling PASS; full engine CI OPEN.
- G7 60Hz interactive tick, material pending root cause and latency: OPEN.
- G8 legal, human, maintainer acceptance: OPEN.

**Decision: F01–F09 specifications and H0 discrete laboratory are COMMITTED. No F01–F09 integrated physics gate is closed. RELEASE HOLD.** Preserve Group 1 C01–C12 independent acceptance and source selection; do not merge code outside authorized review.
