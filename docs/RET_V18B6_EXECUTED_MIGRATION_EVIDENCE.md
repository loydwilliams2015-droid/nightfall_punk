# nightfall!punk 1.8B.6 — Executed Migration Verification
Date: 2026-10-19. Research transfer under draft PR #58.
**Exact tested migration-branch commit:** `22be2eac3db96d01659053abb1bb1aab9abca7f2`.
**GitHub Actions run:** `39335949137`. All four jobs completed **SUCCESS**:
- ledger: 118345363517
- reference-reproduction (gcc): 118345363603
- reference-reproduction (clang): 118345363536
- sanitizer (GCC AddressSanitizer+UndefinedBehaviorSanitizer): 118345363448

## Exact executed migration checks
Python transfer validator output:
```json
{"active_build":"1.8B.6","errors":[],"groups":{"G1":12,"G2":9,"G3":11,"G4":26,"G5":14},"inherited_evidence_groups":4,"negative_controls":8,"open_gate_families":18,"status":"PASS","transferred":72}
```
Validator also invokes inherited Group5 ledger checker on unchanged historical source records, preserving its originally pinned SHA and seven negative controls.

Each GCC, Clang, and GCC ASan/UBSan job executed the exact inherited code on the corrected 1.8B.6 migration-branch tree:
- Authority source historically named `nf_authority18b7.c`: 5,248 cases / 17,556 assertions.
- Physical H0 discrete effects historically named `nf_physical18b7.c`: 5,248 cases / 18,153 assertions.
- Multi-actor arbitration historically named `nf_multi18b7.c` with HF-01: 5,248 cases / 46,303 assertions, 422 prospective grants, 15,322 non-granted decisions.
- POSIX debit WAL `nf_wal18b7.c`: 704 real-file cases / 6,150 assertions, 557 applied generated cases.
- Background governor `nf_governor18b7.c`: 10,000 randomized metric inputs / 39,056 assertions.
- These are the **same authored workloads repeated under three different compiler/sanitizer configurations**, not independent statistical sampling populations; passing a test does not prove complete FPS physical integration.

## Epistemic and migration distinction
- **Verified this turn:** GitHub source ancestry, presence and ownership of the 72 migrated decisions, immutable old ledger data and 18 unresolved gate families; new B6 migration-branch reproduction of five standalone H0 suites.
- **Still historical:** original commit/research names labelled 1.8B.7A, original test CI run IDs, source blob hashes, original dated ledgers. None was rewritten to pretend original development happened under a B6 label.
- **Still unverified:** an independent canonical *production* 1.8B.6 baseline SHA/branch (none found by GitHub B6 branch/commit search); complete integrated A6 world owner and real A4/A5/B3/B4/B5 state; physical/contractual independent oracle; production disk power-cut, packet auth/reorder, frame-time p95/p99, contributor-rights and release authority.
- **Disposition:** version-corrected research ownership + transfer validation **PASS**, isolated C11 reproductions **PASS**, production B6 baseline and whole-engine promotion **HOLD**.

## Provenance and dependency chain
Working corrected branch `migration/v18b6-corrected-research-transfer` is based on existing research head `01229af48a099302c778248454d988ee38161855` (previous Group5). Original upstream PR #53–#57 remain unmerged archive/research reviews; each was cross-linked publicly to PR #58 to avoid continued active misdesignation. Legacy code filenames containing `18b7` are preserved pending evidence-backed API-compatible refactoring; the current engineering target is `1.8B.6`.
