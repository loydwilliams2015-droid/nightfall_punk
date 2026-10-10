# nightfall!punk — Licensing decision matrix

**Current status:** no project-wide license selected. This document is a decision aid, not a license.

## Preconditions before selection

1. Identify copyright holders/contributors for code intended for the licensed release.
2. Audit imported/adapted code and generated assets for compatible terms.
3. Separate engine/source license from documentation, data, art, music, trademarks, and hosted services.
4. Decide whether network-service reciprocity is a project requirement.
5. Decide whether future dual licensing is genuinely needed; if so, obtain legal advice on contributor rights before accepting broad external code.

## Candidate families

| Candidate | Fit | Main implication |
|---|---|---|
| **GPL-3.0-or-later** | strong candidate for a distributed copyleft game/engine commons | distributed modified/combined covered works must satisfy GPL source/reciprocity obligations |
| **AGPL-3.0-or-later** | consider only if network-service source reciprocity is an explicit goal | adds network-interaction source-offer obligations; stronger operational consequences |
| **MPL-2.0** | file-level copyleft alternative | weaker boundary than GPL; may fit modular ecosystems better |
| **Permissive (MIT/BSD/Apache)** | maximum downstream reuse | does not preserve copyleft reciprocity; probably weaker fit with stated commons preference |

## Commercial sustainability

Open-source status is compatible with selling software and services. A commons-oriented project can charge for hosting, support, custom development, stewardship, media, convenience, and packaged distributions while respecting the license freedoms.

## Preliminary fit

Given the project's stated preference for **strong copyleft + legitimate commercial services**, GPL-3.0-or-later is the natural first license for counsel/contributor review. AGPL should be evaluated separately if hosted network service reciprocity is truly desired. This is a recommendation for review, **not a license grant**.

After selection, add the exact LICENSE text, SPDX identifiers, contribution terms, release notices, and update `LICENSE_STATUS.md`.
