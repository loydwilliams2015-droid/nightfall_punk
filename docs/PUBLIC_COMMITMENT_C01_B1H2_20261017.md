# Public Engineering Commitment — C01 B1 H2 Source Lineage
**nightfall!punk 1.8B.7A / PRAXIS · SIGNAL · STEWARD**
**Recorded:** 2026-10-17. **Status:** Publicly documented candidate decision; no merge or release authorization.

## Commitment
We select the **new-source B1 H2 C11 reproduction** (PR #41, source commit `3fb792eab6e035e1a9736db7dbcf22fc36bac410`) as the **proposed forward reference** for B1 contract reconciliation and subsequent build development, rather than pretending that the missing historical H1 artifact has been recovered. The user has directed Option 3 and the engineering assessment provisionally scored its suitability 88/100; that score is editorial, not an observed reliability rate.

**Immutable provenance boundary:** H1 is an earlier historical laboratory report with a missing exact tested source archive; H2 is new code. Never relabel, overwrite or transfer H1 test outcomes, file hashes, archive identity, experimental sample counts or authorship to H2. Continue to publish historical disagreements including inherited 38-versus-55 targeted-test reports, and the biased original 6,000-case H2 generator.

**Correctness commitment:** H2 originally wrapped `world_revision` and `material_revision` on `UINT32_MAX`. The Group 1 corrective implementation now rejects an increment when either counter is exhausted, before any mutation. The associated new overflow regression suite and previously reported local strict GCC/Clang/sanitizer and 13,312-case acceptance runs are documented in `docs/RET_V18B7A_C01_OVERFLOW_FIX_20261017.md`; local scope only, NOT full engine or source-head CI. Preserve negative and failure evidence.

**Build-appropriate open-source usability:** Use small and portable C11 code, stable interfaces, explicit error states, comprehensible positive and rejection fixtures, reproducible independent test seeds, plain-English comments, discoverable documentation and exact GitHub links. Publish contribution/source attribution and reproducibility context. Copyleft commercialization is legitimate subject to license and contributor rights; GPL-3.0-or-later is the adopted direction, not a claim that all branches, documentation, data and assets are automatically licensed. Do not misrepresent CC0/public-domain status.

## Mandatory preconditions before C01 closure
- [ ] Independent exact-GitHub-checkout compiler/sanitizer/full regression run on patched head, with recorded toolchain and source hashes.
- [ ] C02 traceability: every B1 rule evaluated against H2 source and B2–B5 contracts; discrepancies and unsupported behaviors explicitly adjudicated.
- [ ] C03 independent M3/M4 comparison rather than identical-code labels; retain held-out, positive, negative and pathological evidence.
- [ ] Explicit maintainer adoption of new-source H2 lineage with issue #37 closure criterion reconciled; review default-branch ancestry and licensing status (#30/#36).
- [ ] Downstream A3–A6 authoritative integration and actual B3/B4/B5 effects pass declared acceptance before release.

## Public hold
This is the **strongest evidence-grounded commitment presently supportable**: adopt the direction, preserve H1/H2 identity and hold implementation acceptance. **C01 = CONDITIONAL NEW-LINEAGE ADOPTION; historical archive not recovered. PR #53 stays draft; no merge, production or release promotion.** Public revision history and issue status must not be silently rewritten. Maintainer authority remains with the authorized project reviewers.
## Next C02
Reconcile B1 smart-object contract obligations across source and implementation; prioritize authority and fail-closed preconditions, execution separation, identity/revisions, resource atomicity, receipt semantics, and interface/provenance traceability.
