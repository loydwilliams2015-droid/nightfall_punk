# nightfall!punk: scientific records, GitHub provenance, and release accountability

**Effective 2026-10-04; scope: 1.8A laboratory-to-production integration.**

## Five mutually exclusive evidence statuses

| Status | Permitted statement | Insufficient evidence |
|---|---|---|
| DESIGN | decision, hypothesis, or intended policy | no executable proof required |
| H1 LAB | local compiled fixture(s), declared platform, corpus and limits | cannot claim CI, live runtime, cross-platform determinism or human feel |
| H2 COMPARATIVE | matched controls and held-out tests; denominators and uncertainty tracked | same seed repeated at budgets is not independent evidence |
| H3 RUNTIME | source committed, targeted CI and native authoritative tick tested; performance profile attached | CTest of isolated laboratory alone is not H3 |
| H4 PLAY | observed player trials, preregistered instrumentation and reproducible qualitative/quantitative evaluation | benchmark scores are not play-feel data |

A 'LOCK' records a decision at its **stated evidence level**, not a universal proof of correctness. The term 'a priori' denotes consequences of adopted project axioms, not successful empirical verification by itself.

## Mandatory evidence record for every version

1. **Provenance:** exact commit SHA or immutable local archive SHA-256, source path, compiler/toolchain, operating system, configuration flags, and date; distinguish committed bytes from local-only bytes.
2. **Claims:** identify logical invariant, empirical hypothesis, negative control, and preliminary design preference. Do not infer performance from operation counts or conservation from estimated impulses.
3. **Methods:** declare independent seed configurations vs repeated rows, fixture selection, held-out definition, oracle and tolerance. Keep denominators explicit.
4. **Outcomes:** publish pass/fail, observations, worst/pathological/disagreement strata, uncertainties, failures and unexpected outcomes; never quietly discard failed fixtures.
5. **Accountability:** associate the code change with a human/agent reviewer, a specific test, an issue/PR, and reproducible evidence. Source authoring and acceptance are separate decisions.
6. **Immutability:** do not force-push or delete milestone history to make retrospective results look clean. Append errata with dates, supersession reasons, and links to the prior claim.
7. **Commit:** only authoritative world-state commits may generate authoritative contact history; diagnostic/predicted state and rendering never silently become truth.
8. **Privacy:** preserve reproducible aggregates, seed inputs and code without committing personal gameplay logs, credentials, private server data, or unconsented telemetry.

## Review procedure and promotion gates

- Develop on a named integration branch. Open a scoped PR against its **true source parent**, not automatically against `main` if long-running histories diverged.
- Run strict-C tests, CMake/CTest offline lab, source integrity check, sanitizer where applicable; publish what was *actually run*. GitHub Actions pass status is separate from local verification.
- Require a code-reviewer confirmation and reproducibility check before promoting to broader authoritative runtime status. Do not mark an H1 pass as a playable multiplayer build.
- Preserve A/B comparators (legacy swept box, alternative iteration budgets, eager fine, raw-all history) to make later falsification possible.
- Correctly label project research standards: Quine (auxiliary hypotheses), Kuhn (anomalies), Lakatos (progressive predictions), Feyerabend (fair rival experiments), Popper (severe negative tests). These are methodological lenses, not substitute statistics.

## Exact local reproducibility

```bash
bash ./v18a5_close.sh all
cmake -S . -B build/ci-contact -DNF_CONTACT_LAB_ONLY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/ci-contact --parallel
ctest --test-dir build/ci-contact --output-on-failure
```

The offline mode exercises the contact laboratory **only**. Full project compilation and runtime integration with ENet/raylib remain independent gates.
