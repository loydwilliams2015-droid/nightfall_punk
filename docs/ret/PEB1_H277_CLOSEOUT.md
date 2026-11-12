# RET-C H277 — PEB-1 evolutionary common-physics closeout

Date: 2026-11-12. Status: **bounded research PASS**, whole 1.8C.2 and production **HOLD**.

## Exact evidence
- Branch `benchmark/peb1-three-native-generations-h277`, accepted test head `88f91fb8d005700ca707a73a0d85c7925579d133`, parent H276E `4a28fdb1a04e6e36421c3fbb7c87d1ef97173d86`.
- Successful source-exact PEB-1 pull-request workflow: https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/43158340161
- Raw data artifact `peb1-h277-common-physics`: https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/43158340161/artifacts/13916498041
- First attempt https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/43158278476 FAILED because executable path was also the historical worktree directory. This was a test-harness artifact failure, fixed before successful source-exact run; do not erase failure provenance.

## Five X=5 acceptance determinations
1. **PEB01 — source-exact common corpus:** PASS. Three separately compiled actual C engine generations, 160 scenarios per generation, five families of 32.
2. **PEB02 — physical accounting:** PASS. Each generation 3,923 committed ticks, 200 contact observations, 59 fail-closed pending terminations, zero rejections.
3. **PEB03 — historical physical control:** PASS. Actual pinned 1.8A.6 `e0d6aa1334df92c7f8e4425cac96a9f1d4bb4243` and 1.8B.6 `1ebd10049b7f555d9ee037690f6cd090255c98ec` match 160/160 exact scenario records.
4. **PEB04 — C2 common physical regression:** PASS. H276E-derived candidate and B6 match 160/160 exact scenario records, including hash/history/final positions.
5. **PEB05 — scientific scope/whole-engine promotion:** HOLD. Source records include neither executable 1.7x historical build nor genuine three-ghost decisions, multi-A6 authorization, off-axis production B2/B3, WAL crash recovery, real 3D playability or p95/p99 frame time. No production/1.8C.3 acceptance inferred.

## Independent guardrails at H276E source
- Full-source H59 GCC/Clang accepted: https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/42765503388
- B2 eligibility accepted: https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/42765502997
- B3 physical acceptance accepted: https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/42765503502
- H15 TSAN smoke control still fails in repeat attempts with a SEGV at 0x18, exit 66, before the engine lifecycle TSAN suite. Root cause not attributed: https://github.com/loydwilliams2015-droid/nightfall_punk/actions/runs/42765503012

## Next five milestone recommendations
- **M2.5-01** Production-owned H275F off-axis source geometry (no research mode callback substitution).
- **M2.5-02** Real B2 granted PUSH and B3 staged source-binding negative and positive acceptance for this off-axis candidate.
- **M2.5-03** A6 private atomic A4/A5/B2/B3 accepted immutable publication, exact rollback on refusal.
- **M2.5-04** Durable recovery and concurrently leased reader correctness; separate TSAN runtime smoke triage from code races.
- **M2.5-05** Independent H59/GCC/Clang, sanitizers, held-out geometry and frame-time / multi-actor acceptance on exact SHA.

## Distance estimate
Planning allocation remains **8–15 five-determination sweeps** (40–75 determinations) to full 1.8C.2, contingent on no substantial architectural redesign. H277 closes a bounded regression-observability milestone, not an implementation dependency. This is not a statistically derived completion percentage.

**PR remains DRAFT.** RET provenance, research-only scope, GPL-3.0-or-later and source rights preserved.
