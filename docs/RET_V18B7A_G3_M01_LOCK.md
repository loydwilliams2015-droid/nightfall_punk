# M01LINE — Single authoritative multi-actor tick
**nightfall!punk 1.8B.7A Group 3 • 2026-10-19 • PRAXIS / SIGNAL / STEWARD**

## Locked public design commitment
Many actors propose on one immutable S_t; only the existing A6 owner may resolve and publish S_(t+1).

## Executable or inherited evidence
Bounded nf_m7_step evaluates several intents against one snapshot and stages one callback publication.
The new bounded C11 model is in `src/shared/nf_multi18b7.c`, tests in `src/tests/test_multi18b7.c`. The existing HF-01 client projection is tested only as a local nonauthoritative input/display model.

## Unresolved acceptance gates
No live A6 owner/physics integration; owner lock and callback nonreentrancy are caller obligations.

## RET and scope
**A priori:** proposing or observing an action is distinct from its real material consequence. **Formally specified:** bounded snapshot, identity, capacity and revision invariants, not machine-proved for live simulation. **Empirical:** mark only actual reported Group 3 local/CI results, never infer live physics or human acceptance. **Highly predictable:** explicit roles and monotonic status should reduce ambiguity; not yet proven in deployed gameplay. **Uncertain/unresolved:** refer to the gates above.

**Disposition:** M01 design contract LOCKED for the research branch; full-engine implementation or human acceptance (as appropriate) remains HOLD. C01–C12, F01–F09 and B5 persistence release holds are preserved. No merge authorization.
