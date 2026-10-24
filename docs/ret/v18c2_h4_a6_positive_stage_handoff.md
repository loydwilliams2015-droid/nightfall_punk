# 1.8C.2 H4 — source-exact A6-owned staging checkpoint

PRAXIS / SIGNAL / STEWARD, 2026-10-24. Ancestor H3 passing SHA `108c78b1707f003edcaae12213eb9f561bd81f8d`, H3 CI 3 workflows / 11 jobs succeeded.

## H4 implementation
Source-exact H4 test `src/tests/test_v18c2_a6_stage_positive_h4.c` added to headless CTest. Constructs real NfWorld and actor, initializes Nf18a6Runtime, binds sole A6 owner using `nf18a6_bind_world_tick`, and constructs an A4 island from runtime-owned actor and object. Calls `nf18c2_a6_stage` to test (a) read-only READY stage; (b) denied no-grant; (c) pending unsupported CARRY/material edit; (d) stale revision; verifies no world tick publication. Unlike H3 negative-only checks this tests a real A6 owner-bound positive staging request.

## Evidence
H3 source-exact 3/3 workflows PASS at H3 SHA. H4 GitHub CI is required; register its result only once the actual head SHA run completes. New source-exact positive test is not inherently proof of authorized physical contact or durable B3 receipt.

## RET
A priori: logical decision flags are not physical evidence or credentials; pending A5 refinement may not be treated as free. Formally implemented: stage guard requires actual bound idle A6 owner and source revisions; A4 multi-body solver only stages candidate. Empirically validated: H3 CI. H4 positive/denied/pending/stale acceptance only when CI run confirms. Highly predictable: invalid epoch refuses. Uncertain: B2 policy-source authenticity, candidate contact provenance and A5 witness propagation. Unresolved: authoritative B2 credential receipt, positive material fine-voxel witness at staging, live A6 multi-actor publisher, full B3 typed realizers, journal atomicity, independent blind held-out seeds, fairness, 60Hz frame-time, code rights.

## Mandatory publication HOLD
The `b2_granted` flag is caller supplied and **cannot** establish trusted B2. No positive A5 exact query is established in H4; canonical/fine material would need an owner-owned prepublication verifier. The new H4 stage is read-only. Do NOT call `nf_multi18c1_publish`, issue B3 receipts, or begin C.3 durability integration as though A4/A5/B2 authority is proved.

## Next
Implement A6-internal capability-issued B2 scene and use stable source epoch/revisions; validate A5 exact swept material witness and contact history; stage B3 consequence and collision receipts under a single owner transaction; test positive/denied/stale/missing-fine and source-staleness with held-out inputs; only then reconsider publication.
