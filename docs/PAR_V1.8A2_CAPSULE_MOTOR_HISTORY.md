# 1.8A.2 graded PAR — policy kernel

| Gate | Status | Scientific interpretation |
|---|---|---|
| 46 native named C assertions | PASS | Components obey policy in tested fixtures |
| 1,000 seeded rows, 5 stratified cohorts | PASS | Controlled, synthetic/native sampling |
| Distinct box versus capsule geometry at low edge | PASS | At least one genuine shape difference; not swept geometric correctness |
| Motor acceleration limited by force/mass | PASS | Test-level motor mechanics |
| Equal/opposite two-body normal momentum | PASS | Pure linear normal exchange; no full rigid-body island |
| Deterministic duplicate contact solve | PASS | Same-process replay tested, not cross-platform fixed point |
| 2->4->6 adaptation | PARTIAL | 1,000 samples all solved in cheap pass; reserve path requires severe new scenarios |
| Fine data request cannot succeed on metadata alone | PASS | 4m chunk data and 0.5m/0.25m loading contracts |
| Every accepted contact sample summarized | PASS in fixtures | 15 consecutive samples aggregate with no sample-count loss |
| Small accumulated impulses can cause threshold event | PASS in fixtures | 30 samples yield one cumulative event instead of 30 stored events |
| Contact history preserved after event-cache eviction | NOT PROVEN | Durable append-only event sink/ack/backpressure missing |
| Capsule/rounded-box swept gameplay controller | NOT PROVEN | Shape policy and static distance query only |
| Integrated dynamic collision in player runtime | NOT PROVEN | Pair impulse component and motor component not coupled to authoritative movement loop |
| Actual chunked material solver | NOT PROVEN | Data cache exists; world loading and solve coupling are separate work |

## Parameters to calibrate

- Shape: radius, height, flat sole radius, step height, slope cutoff.
- Motor: target velocity, acceleration/force caps, air motor, damping, reaction impulses and mass scaling.
- Solve: 2 initial, +2 reserve, 6 hard max; set via falsifying tests.
- Spatial: 4m chunks /1m canonical /0.5m or0.25m refinement; refine on material requirement, not mere contact.
- Temporal: 15-tick summaries, >=60 sampled ticks for duration threshold, 20 Ns peak, 80 Ns cumulative. Threshold units and physical calibration to be confirmed.

Keep outcome distributions by cohort, with no universal weighted scalar and no premature production claim.
