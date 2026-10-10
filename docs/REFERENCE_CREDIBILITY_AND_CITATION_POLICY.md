# nightfall!punk — Reference credibility and citation policy

**Effective:** 2026-10-10 stewardship draft.

Credibility is **typed and domain-bounded**. A source can be authoritative for a software license, API contract, court rule, or game mechanic without becoming general authority over nightfall!punk design.

## Reference classes

| Class | Typical source | Appropriate use |
|---|---|---|
| **A — Primary / governing** | source code at an exact commit, official license text, standard/specification, statute/court opinion, official project documentation | exact behavior, rights/obligations, formal interface, historical fact |
| **B — Scholarly / maintainer technical** | peer-reviewed research, serious books, maintainer talks/docs, audited technical reports | theory, measured precedent, interpretation with stated scope |
| **C — Reliable secondary** | reputable technical journalism, well-sourced explanatory articles, independent reviews | context, discovery, cross-checking |
| **D — Community / anecdotal** | forums, Reddit, informal videos, issue discussions, personal reports | hypotheses, usability signals, edge-case discovery; verify before hard claims |

A source's class does not decide whether its conclusion is correct. It controls how strongly it may support a claim before independent checking.

## Project evidence is a separate axis

H0/H1/H2/H3/H4 describes **nightfall!punk's own evidence level**, not the prestige of an external source. An official engine manual can be Class A reference evidence while our implementation remains H0. Conversely, an H3 runtime result can disagree with a secondary design article.

## Citation record

For material external influence, record as available:

- author/organization and title;
- version/release/date;
- stable URL, DOI, repository + commit/tag, or legal citation;
- what exact claim/mechanism it supports;
- whether code/assets were copied, adapted under license, independently implemented, or merely compared;
- retrieval date for changeable web material.

## Conflicts

Prefer the most direct governing source for the question. When credible sources disagree, preserve the disagreement, compare versions/jurisdictions/assumptions, and avoid averaging incompatible claims into a false consensus.

## Games as PAR references

A peer game can be high-authority precedent that a mechanic or experience is achievable. It does not prove the same mechanism is correct for nightfall!punk. PAR should identify the transferable function, the project's distinct implementation, and the proof needed here.

## AI-generated material

AI output is not an external authority. Where AI summarizes research, cite the underlying source. Where AI helps author code/tests, treat the result as project work requiring provenance review, compilation/testing, and human acceptance; do not use the model's confidence as evidence.

## Anti-plagiarism rule

If wording, structure, code, data, art, or a distinctive idea is materially derived from another identifiable work, attribute it. For source code, comply with the actual license/permission. For prose/research, paraphrase where appropriate and quote only what is necessary. Generic techniques and independently developed implementations should still cite major precedents when they materially shaped design decisions.
