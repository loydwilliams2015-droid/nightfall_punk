# 1.8C.2 H5 — B2/A5 owner-bound witness research, RET
## Source
Ancestor H4: ca46f2b316044260d5fe54c4f525f612389653ac (three GitHub Actions workflows passed). H5 candidate: engineering/v18c2-b2-a5-witness-h5; source-exact test registered in CMake.

## Implementation
The previous H3/H4 `b2_granted` diagnostic remains *not* an authority; H5 adds a separate `nf_c2_witness` entry point. It validates the bound A6 owner identity, nonexecuting tick, source tick and material epoch; then performs `nf18a5_query(..., exact=true)` on a copy of the actual A5 grid to avoid query-side mutation of source material. It calls the existing real `nf_ma03_b2_owner_evaluate` against the A6 world/runtime, preserving the repository's B2 geometry and material checks and server-provider obligation.

## Acceptance suite
Source-exact test `src/tests/test_v18c2_witness_h5.c` covers rejected stale material epoch, pending missing B2 provider, pending missing fine material in MIXED voxel, SOLID denied, and world tick unchanged. Registered in offline CTest inherited GCC/Clang strict and sanitizer CI. Do NOT claim these tests passed until GitHub run at source SHA actually concludes.

## RET
A priori: provider-granted boolean is not trusted source; MIXED with missing fine is not FREE, material epoch stale is not authoritative.
Formal implementation: bound-owner and source revision preconditions, exact A5 query plus existing B2 evaluation; no publishing.
Empirical evidence: H4 regression passes; H5 pending verification.
High predictability: A5 MIXED without fine returns PENDING. False if loader/residency changes beyond tested conditions.
Uncertain: source-provider credential integrity, positive eligibility fixture, full swept-volume static and dynamic material coverage.
Unresolved / HOLD: actual A6-internal capability issuance, positive genuine server-owned B2 fixture at tick, live A6 prepublish, actor-object & actor-actor physical witness and B3 consequence, atomic A5/B3/B5 durable receipt, representative 60Hz p95/p99, independent blind held-out, rights and governance.

## Next mandatory acceptance
(1) Verify source-exact H5 GCC, Clang and sanitizers across inherited suite.
(2) Replace caller-mounted B2 provider with an A6-controlled capability/snapshot and test a positive B2 result with real actor, object, material and credentials. Explicitly assert no C2 stage READY from a diagnostic boolean alone.
(3) Integrate exact swept A5 witness with physical consequence under the A6 owner and test authorized, denied, stale and pending. No world publication until all necessary witnesses and durable joint transaction are verified.
