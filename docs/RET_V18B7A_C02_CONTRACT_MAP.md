# C02 — Authoritative smart-object contract map and executable acceptance plan
2026-10-17. Source baseline: Group 1 PR #53, B1 H2 new-source branch ancestry PR #41. This map is versioned as a *review candidate*, not a silent replacement for B1 H1 or EEC-01.

| Contract ID | Rule | Owning boundary | Source and executable evidence | Disposition |
|---|---|---|---|---|
| B1-C01 | Valid actor, object and action identity | B1 / B2 | nf_b1_evaluate; test_contract_map18b1 | Fixture authored |
| B1-C02 | Material identity epoch and world revision match | B1 / B2 | nf_b1_evaluate; stale fixture | Fixture authored |
| B1-C03 | Unknown material/actor knowledge remains PENDING | B1 / A5 / B2 | nf_b1_evaluate; unknown fixture | Fixture authored |
| B1-C04 | Physical reach, clearance and support required | A3/A5 / B1 | nf_b1_evaluate; clearance fixture; B2 real A3/A5 separate | Isolated-only |
| B1-C05 | Contract permission and locked owner authority | B1/B2 | nf_b1_evaluate; denied-contract fixture | Fixture authored |
| B1-C06 | State duplication and conflicting replay rejected | B1/B5 | nf_b1_evaluate/commit; B5 receipt comparison from Group 1 | Integrated open |
| B1-C07 | Revalidate witness immediately before commit | B1/B2 EEC | nf_b1_commit; forged witness fixture | Fixture authored |
| B1-C08 | No state publication when refusal or capacity full | B1/B2/B5 | nf_b1_commit; unchanged-state fixture | Isolated-only |
| B1-C09 | Committed action records causal event, revision and digest | B1/A5 | nf_b1_commit; event digest fixture | B1 isolated, physical open |
| B1-C10 | Material identity stays stable while state revision changes | B1/A5 | nf_b1_commit; positive fixture | Fixture authored |
| B1-C11 | Revision exhaustion must fail closed | B1/B2 | B1 overflow correction on PR #53; boundary fixtures | Previous executed overflow checks |
| B1-C12 | Eligibility does not imply applied impulse or persisted transaction | B1/B2/B3/B5 | EEC-01 specification; actual B3/B5 integration pending | OPEN |
| B1-C13 | Multi-actor same-tick contention and ownership | B2/B4 | B2 bounded resolver; B4 H0 lab not integrated | OPEN |
| B1-C14 | Durable receipts and network retransmission integrity | B5 | B5 snapshot H0 not WAL; C10 replay patch | OPEN |
| B1-C15 | Client/presentation cannot grant authority | B2/B4 HF-01 | client sequence state only, live server not proven | OPEN |
| B1-C16 | Diagnostic fingerprint collision not security identity | B1/B5 | historic H1 warning; 32-bit digest | OPEN threat model |

## Contract hierarchy, compatibility and simplification
1. **Single authority:** Actual A6 tick owner resolves material/contract claims; B1 H2 is a transparent standalone reference, not a second game-world owner.
2. **Common event language:** PROPOSED → eligibility/revalidation → reserved/committed → contingent execution / interruption; use EEC-01 for the integrated lifecycle. No universal global orchestration added.
3. **Specialization:** Keep A3 clearance, A4 applied impulse, A5 material/history, B4 same-tick actors, B5 durable recovery, client UI projection distinct. Shared invariants do not imply identical mechanisms.
4. **Contradiction policy:** H1 M4 2304/2304, H2 6000 seeded (zero positive due parity), 256 supplementary (3 positive), 13312 later authored (59 positive), and B2 M3/M4 are different lineages/test corpora. H1 55 vs RP-01 38 remains an open documentation erratum, not reconciled by fiat.
5. **New executable evidence:** src/reproduction/v18b1/test_contract_map18b1.c contains authored positive/negative/unknown/stale/forged/duplicate/overflow fixtures. Do not report them as PASS until compiled on the new head. Reproduction: cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -Isrc/reproduction/v18b1 src/reproduction/v18b1/nf_smart18b1.c src/reproduction/v18b1/test_contract_map18b1.c -o /tmp/nf_c02 && /tmp/nf_c02 .
6. **Open-source usability:** readable IDs, build-specific owners, direct source/test paths, plain-language failure semantics and version-locked provenance. No claim of public-domain entitlement over third-party assets. Existing GPL direction and issue #36 retain legal gates.

## Acceptance gates
- [ ] Run exact source-head C02 tests under GCC, Clang, ASan/UBSan, preserving full logs and hashes.
- [ ] Map each smart-object production/derived contract to current authoritative owner and independently verify positive and refusal paths.
- [ ] Independently compare M3/M4 using real B2 evaluator (C03), not proxy identical arbiters.
- [ ] Reconcile 38/55 inherited H1 documentation and current H2 new lineage, with reviewer approval.
- [ ] Live integration: physical and ledger publication, multi-actor contention, interruption, replay, observer noninterference.
- [ ] Full CI, contributor/license review and authorized promotion.

## RET and strongest present commitment
A priori: new H2 source is not historical H1; permission cannot manufacture physical success.
Formal/narrow: explicit contract owners and specified invariants, not formal verification of whole engine.
Empirical inherited: prior documented H2 and B2 tests and revision overflow fix; not new C02 execution unless separately recorded.
Highly predictable: a traceable fixture map improves future regression detection; no empirical developer-usability study.
Uncertain/unresolved: real engine integration, test-independence, historical document discrepancy, adversarial network.
**Public commitment:** Maintain this map as the reviewable C02 contract index and the executable fixture as an acceptance gate; preserve release and historical holds. C02 = SPECIFIED + TEST AUTHORED, acceptance OPEN.
