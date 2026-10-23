# nightfall!punk 1.8C.2 — real A4/A5/B3 contact laboratory and RET handoff
PRAXIS / SIGNAL / STEWARD — 2026-10-22. Ancestor 1.8C.1 PR #82, H1 only, all production holds.

## Scope and actual implementation
New `src/tests/test_v18c2_real_a4_a5_b3_gate.c` invokes the actual production-source signatures `nf18a4_apply_contact` (reciprocal physical impulse), `nf18a5_load_canonical` and `nf18a5_query` (real MIXED voxel refuses absent fine material data), and `nf18b3_authorize` for each B3 typed interaction kind. Tests use a separate fixed-seed 173-case held-out-style stratum; because its seed and source are visible to implementation, it is NOT independently blinded validation. Cases: canonical 128, randomized 256, pathological 192, boundary 128, held-out-style 173 = 877. Verify momentum conservation of non-motored pair and refusal on unavailable material; for contract denial also check B3 result.
Existing incumbent `nf_b6_push18b6` has actual A6/A4/A5/B3 push evidence; preserve it unchanged. New deterministic H1 transaction arbitration remains abstract and is NOT wired into that actual A6/B6 implementation. No new actual door/carry/repair/interruption actuator can be accepted until material witnesses, contract authority and interruption compensation are executed.

## Results/provenance
Previously executed local abstract arbiter corpus: 1280 cases, 1116 incumbent/challenger state disagreements (not correctness failures); see prior `docs/ret/v18c2_concurrent_materials_h1.md`. GitHub source-exact physical/material tests are now CMake/CTest registered and new GCC/Clang+ASan/UBSan CI added. As of publication, this new test has NOT been observed running successfully. Do not call its 877 planned cases empirical passes. Source-exact full regression and CI are gates, not assumed facts.

## RET
A priori: conflicts cannot both commit to same exclusive object from one snapshot; absent authoritative material data is not clearance; B3 semantic receipts cannot substitute for A4 impulse + A5 material truth.
Formally specified: bounded deterministic ordering from prior abstract H1; A4 momentum invariant and B3 fail-closed test expectations.
Empirically established: previous abstract 1280 local tests and inherited 1.8B.6 source-exact real PUSH test reports; new 877-case test unverified until CI.
Highly predictable: A4 reciprocal equal-and-opposite impulses conserve linear momentum within tolerance, provided a valid constraint; A5 mixed voxel requires fine witness.
Uncertain: multi-contact chain accuracy, live arbitration integration, door joints, actual carry support, repair compensation, refinement cache performance, load distribution and starvation.
Unresolved: real A6 owner-locked positive/denied/stale B2 publication; fully typed durable A5/B3/B5 transaction; crash replay, 1/2/4/8 worker determinism, p95/p99 performance; rights #30/#36/#37.

## Mandatory exit gates for 1.8C.3
(1) Run source-exact GCC/Clang/sanitized CTest; verify 877 cases and full inherited suite; independently blind held-out data.
(2) Implement one A6-owned immutable snapshot arbitration path, stage actual A4 and A5 contact/material history for actor–actor and actor–object, and real B3 typed realization for PUSH, DOOR, CARRY, REPAIR, INTERRUPT (with compensation), SUPPORT, selective REFINE.
(3) Prove no split-success logical/physical commit. B2 owner-snapshot authorization, source revision and immutable dependencies must validate at prepublication.
(4) Compare correctness, causal fidelity, stability, responsiveness and representative p95/p99 frame latency; physical/contact/contract conservation.
(5) Only then integrate portable per-field serialization and crash-injection/replay in C.3; not a production merge yet.

## Decision
H1 incumbent: existing A6/A4/A5/B3 B6 PUSH path; H1 challenger: deterministic snapshot arbiter with real API gate test only. No justified production winner yet. All holds remain.
