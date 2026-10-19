# M08LINE — Reordering and contradictory feedback
**nightfall!punk 1.8B.7A Group 3 • 2026-10-19 • PRAXIS / SIGNAL / STEWARD**

## Locked public design commitment
Presentation must not regress an already confirmed authoritative event because of delayed packet feedback; client must remain read-only.

## Executable or inherited evidence
HF-01 test rejects late contested at same sequence after applied, rejects stale sequence, permits applied→interrupted; world snapshot remains unchanged.
The new bounded C11 model is in `src/shared/nf_multi18b7.c`, tests in `src/tests/test_multi18b7.c`. The existing HF-01 client projection is tested only as a local nonauthoritative input/display model.

## Unresolved acceptance gates
Real network reorder/loss/acknowledgment, conflicting canonical session events and server repair remain OPEN.

## RET and scope
**A priori:** proposing or observing an action is distinct from its real material consequence. **Formally specified:** bounded snapshot, identity, capacity and revision invariants, not machine-proved for live simulation. **Empirical:** mark only actual reported Group 3 local/CI results, never infer live physics or human acceptance. **Highly predictable:** explicit roles and monotonic status should reduce ambiguity; not yet proven in deployed gameplay. **Uncertain/unresolved:** refer to the gates above.

**Disposition:** M08 design contract LOCKED for the research branch; full-engine implementation or human acceptance (as appropriate) remains HOLD. C01–C12, F01–F09 and B5 persistence release holds are preserved. No merge authorization.
