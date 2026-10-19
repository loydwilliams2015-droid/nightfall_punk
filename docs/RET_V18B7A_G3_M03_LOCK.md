# M03LINE — Deterministic fairness over ticks
**nightfall!punk 1.8B.7A Group 3 • 2026-10-19 • PRAXIS / SIGNAL / STEWARD**

## Locked public design commitment
Retain deterministic arbitration while measuring cross-tick outcomes; rotating priority is a challenger to fixed ID order, not proven globally fair.

## Executable or inherited evidence
Fixed and rotating ordering implemented; eight alternating winner/release rounds are tested on a single scope.
The new bounded C11 model is in `src/shared/nf_multi18b7.c`, tests in `src/tests/test_multi18b7.c`. The existing HF-01 client projection is tested only as a local nonauthoritative input/display model.

## Unresolved acceptance gates
Long-run distribution, starvation bounds, actor dropout and fair scheduling with variable opportunities not established.

## RET and scope
**A priori:** proposing or observing an action is distinct from its real material consequence. **Formally specified:** bounded snapshot, identity, capacity and revision invariants, not machine-proved for live simulation. **Empirical:** mark only actual reported Group 3 local/CI results, never infer live physics or human acceptance. **Highly predictable:** explicit roles and monotonic status should reduce ambiguity; not yet proven in deployed gameplay. **Uncertain/unresolved:** refer to the gates above.

**Disposition:** M03 design contract LOCKED for the research branch; full-engine implementation or human acceptance (as appropriate) remains HOLD. C01–C12, F01–F09 and B5 persistence release holds are preserved. No merge authorization.
