# C01 Decision — Conditional adoption of B1 H2 as new-source reference
Date: 2026-10-17. PR #41 source head: 3fb792eab6e035e1a9736db7dbcf22fc36bac410.
## Decision
SELECT OPTION 3: Adopt the clean B1 H2 C11 reproduction as the forward engineering reference, conditional on a source-specific verification and maintainer approval. This **does not** recover the original H1 code, prove historical equivalence, or promote a production B1 engine. Preserve issue #37 OPEN pending explicit new-lineage approval and tests. Maintain current full release hold.
## Weighted editorial assessment
Scientific fidelity 4/5 x30%=24. Reproducibility/provenance 5/5 x25%=25. Open-source contributor friendliness 5/5 x20%=20. Downstream integration readiness 3/5 x15%=9. Maintenance efficiency 5/5 x10%=10. **Total 88/100 provisional suitability**, not a measured performance or correctness score. Scores reflect GitHub source documentation and reported test lineage, not new execution.
## Evidence
PR #41 README labels H2 a new simplified source, not original H1, and not integrated with A3/A4/A5/A6. The inherited 6000 seeded test cases do not cover positive commits due LCG parity; a separate documented 256-case supplement has three positives. H1 reported M4 2304/2304 and 55 targeted tests, neither reproduced by this H2 source. The inherited RP-01 38 vs 55 count discrepancy remains open.
## Usability and commons defaults
Keep original H1 hashes and results in archive, never relabel them H2. Publish stable simple C11 API, portable compiler instructions, positive/negative demo fixtures, deterministic reproducible seeds and reasoned error states. Specify actual interface limits and exact provenance. Preserve GPL-3.0-or-later stewardship direction with unresolved legal issue #36; never assume all reference materials are CC0. Copyleft is compatible with legitimate commercial use subject to its license conditions.
## Approval gates
- [ ] Freeze exact H2 file/source/test hashes and reproduce strict GCC/Clang + ASan/UBSan.
- [ ] Repair positive-case coverage without altering historical test output. Independent canonical, boundary, random, adversarial and held-out oracle.
- [ ] Trace each B1 smart-object requirement to H2 and B2; record preserved, changed and unsupported behavior, and 38/55 count erratum.
- [ ] Keep M4 incumbent / M3 challenger separate from H2 source identity. Verify any claimed live A3/A4/A5/A6 integration.
- [ ] Record contributor rights, legal review, main ancestry and authorized maintainer acceptance before formal closure of #37.
## RET
A priori: a new reproduction is not historical-source recovery. Formally defined: distinct lineage and explicit gates. Empirical: inherited H2 source/test record only; zero new tests in this decision. Highly predictable: clear onboarding and less speculative archaeology, not observed usability metrics. Open: historical source, acceptance, live integration.
**C01 disposition: Option 3 selected; CONDITIONAL H2 adoption, historical issue OPEN, release HOLD.**
