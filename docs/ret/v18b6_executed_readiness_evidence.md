# 1.8B.6 RET / HK — Fresh Executed Engineering Evidence and CI Incident
PRAXIS / SIGNAL / STEWARD. Date 2026-10-19. Working corrected research parent: `aaae30e7082f5ceaea198fe4a37f6c02692bab84`. Code authoring: scoped regression-only branch `engineering/v18b6-ret-readiness-h0` PR #59.

## New source changes
1. `src/shared/nf_wal18b7.c`: release acquired descriptor on failed `flock`, initial parent-directory `fsync`, or `fstat` checks. The existing `nf4_close` also marks the object quarantined. Deliberately **do not** close a corrupt but inspected read-only journal: preserve the evidence path.
2. `src/tests/test_wal18b7.c`: opposing simultaneous openers; failed opener has `fd==-1`, is quarantined and cannot apply; first opener can commit; after it closes, later opener replays same durable receipt without duplicate debit. This is a Linux/POSIX advisory-lock regression, not security-proof across malicious processes.

## Original first CI attempt — preserved anomaly
Run `39337522177` initially reported `source-and-migration = FAILURE` while WAL GCC, Clang and ASan/UBSan regression jobs passed, and offline contact-lab passed. Root cause: the existing `tools/verify_v18b7a_ledger.py` rightly compares the original historical Group4 test Git blob to its pinned object hash. The new 1.8B.6 regression **necessarily modifies** the test file and thus cannot pass *historical* source-integrity verification on this new working tree. This was an invalid validation location, **not** evidence to edit or declassify the original old file/source hash.

## Tested correction
Change the CI job to fetch full Git ancestry and run the historic ledger checker in a detached worktree at original `aaae30e7082f5ceaea198fe4a37f6c02692bab84`. Separately validate the current 1.8B.6 transfer ledger and compile/test repaired source on the new branch. The historical ledger and its pinned source hashes were never modified.

**Exact newly tested PR head before this evidence document:** `7d21f7d6362517e3aabfa978e485568509aa1b91`; GitHub workflow **`39337631051`**:
| CI job | ID | Executed result |
|---|---:|---|
| source-and-migration | 118350996382 | SUCCESS; 72/72 B6 transfer decisions, eight negative controls; original frozen G5 ledger checker PASS with its seven negatives |
| wal-regression (gcc) | 118350996699 | SUCCESS; strict C11 file recovery |
| wal-regression (clang) | 118350996563 | SUCCESS; strict C11 file recovery |
| sanitizer | 118350996309 | SUCCESS; GCC AddressSanitizer+UndefinedBehaviorSanitizer |
| contact-lab | 118350996096 | SUCCESS; offline dependency-free CMake/CTest 10/10 tests |

File WAL executed under **GCC, Clang, ASan/UBSan**, each with **704 five-stratum file cases / 6,161 assertion checks** (the 6,150 historical checks plus 11 targeted new regression checks), and **557 applied generated corpus events**, with separately targeted lock/replay cases. Distinct compilers repeat the same fixture populations, not new independent samples.

Offline contact/A6 CTest: 10/10 tests passed; this is **H1/H0 opt-in lab**, not actual network+raylib full-client and four-actor production simulation.

### Source-verified SHA-256 from this execution
- `docs/evidence/v18b7a_g5_master_ledger.json`: `4afb77e135112566c4f1d9c99ba5268d2f3c4950511cc54f56d303370038e35e` (unchanged original)
- `docs/evidence/v18b6_master_ledger.json`: `abd1fee63fb2da880f737c800e07be1a5480ae74a76dc38cbf59a1d3b74d0f15` (unchanged migrated research register)
- `src/shared/nf_wal18b7.c`: `4f2a356ec492ac754b1b4ebb57ca67fdf1784f336c6f6906d91ade0e10709d55` (new fixed source)
- `src/tests/test_wal18b7.c`: `9d3f363b64fd7b34c1827d887eada65b50aa43f26252e7b7bbf476ab786c00da` (new regression fixture)

## RET classification and unchanged holds
- A priori: acquisition must either return a valid owned descriptor or release any resources on failure.
- Formally specified: returned `NF4_IO_ERROR` after opening the descriptor must leave `wal.fd==-1`; no new authoritative commit after failure; transparent crash/replay semantics unchanged.
- Empirically executed: exact bounded WAL regression + offline contact/owner tests above on tested GitHub commit.
- Highly predictable: preventing descriptor/lock leaks will avoid one class of repeat-open resource issues, but frequency in a live server is unmeasured.
- Uncertain/unresolved: arbitrary power-loss, cross-domain A6/physics/contract durability, multiprocess hostile symlink manipulation, network transport auth, full FPS 60Hz and human acceptance.
- Production promotion requires canonical source/rights, issue #30/#36/#37 reconciliation, reviewer acceptance, full engine CI and independent truth. Release HOLD. No shared/canonical LEDGER.md was overwritten.

**Exit:** Scope-limited H1 code repair CI PASS; full corrected 1.8B.6 engine readiness BLOCKED.
