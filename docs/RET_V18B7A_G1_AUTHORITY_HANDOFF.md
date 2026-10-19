# 1.8B.7A Group 1 — Authority & Contracts Evidence Handoff
Date 2026-10-17. Baseline user-selected latest B5 PR #51: b5870171f3d2ed6794c349944b3ec2eda70e6219. Owns C01–C12. Review branch audit/v18b7a-g1-authority-contracts. **Release HOLD.**
## Retrieved evidence
Read B2 authority interfaces, B4 H0/HF-01 ledgers, B5 H0 source and ledger from GitHub. B1 H1 original source still missing issue #37; B2 live EEC reported narrow executed proofs but default multiplayer and full B3/B4/B5 remained open. Earlier B4 5376 authored cases M3/M4 share identical arbitration (no independent challenger). B5 earlier 76 local checks, 2688 authored standalone cases; no actual network WAL. These are inherited assertions and not rerun here.
## Verified source defect / narrowly justified fix
B5 src/shared/nf_persist18b5.c before patch returned NF_B5_DUPLICATE for any existing same actor/action, even if new object's ID, debit, or original event epoch differed. This suppresses a conflicting retry rather than explicitly rejecting it; ambiguity should be fail closed. Changed callback path to NF_B5_INVALID if payload differs, preserving NF_B5_DUPLICATE only for matching event. Added targeted test assertions for mismatched target and mismatched debit with no RAM/persisted state mutation. This change modifies API disposition and must be independently checked against live server retry semantics and cross-session action IDs. **Patch and tests committed; neither new test nor whole-engine CI claimed PASS yet.**
## Execution and reproducibility
Exact GitHub raw clone attempt from runtime failed DNS resolving raw.githubusercontent.com; consequently no exact-source compiler run completed in this pass. New CI result not inspected. No fabricated performance, randomized, pathological, boundary or held-out results for 7A. Previously recorded B5 corpus is not a fresh experiment. Reproduce on branch: cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -Isrc/shared src/shared/nf_persist18b5.c src/tests/test_persist18b5.c -o /tmp/nf_b5_group1 && /tmp/nf_b5_group1 .
## Group 1 gates
C01 OPEN lost historical H1 archive, new lineage distinction preserved.
C02 OPEN smart-object contract source-level reconciliation.
C03 OPEN independent M3/M4 in 1.8B.3–B5.
C04 PARTIAL EEC stage semantics documented B2; not integrated B5.
C05 PARTIAL documented A3/A5 fail-closed B2, extended interactions unverified.
C06 OPEN cross-system alias, epochs, typed state.
C07 PARTIAL A6 narrow single-owner B2; B3/B4/B5 binding not proven.
C08 PARTIAL B5 mock persistence-first RAM update; no actual durable physical+ledger atomicity.
C09 OPEN live shared multi-actor deterministic arbitration/resource aliases.
C10 PATCHED UNVERIFIED mismatched action replay; persistence/retry integration pending.
C11 PARTIAL HF-01 original code client-only; real network and observer authority unverified.
C12 OPEN new independent adversarial deterministic replay/conservation.
No release gate closed by this isolated patch.
## RET classifications
A priori: event ID identity must not silently mask contradictory payload if the declared idempotency policy treats an ID as one immutable action. Formal narrow source reasoning: equality fields now compared before duplicate status; no proof of concurrent retries or durable effects. Empirical new Group 1 validation: NONE; inherited B2/B4/B5 experimental scope is historical. Highly predictable: malformed/conflicting retries will be explicitly denied rather than labeled identical, when this pure C function is invoked. Uncertain: whether a greater identity namespace (session epoch, actor/target IDs) is required. Unresolved: canonical B1 lineage and real world-authoritative atomic multiplayer.
## Next
Clean checkout and run new unit+five stratum tests with GCC/Clang/ASan/UBSan, independent oracle for mismatched payload, compare M4/M3 live B2 evaluator and real B3/B4 consequences, assert exact source SHA, record raw results and maintain #37/#45/#30/#36 holds. Return these links to coordinating 1.8B.7 chat.