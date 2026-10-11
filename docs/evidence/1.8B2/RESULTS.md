# 1.8B.2 executed comparative results

21,504 indexed authored cases × 5 models = 107,520 model-case IDs; 3 dependent timing repeats.
Case IDs do not imply distinct stimuli: boundary repeats two one-ULP states 1,024 times each.
No independent game-world failure probability or formal proof is inferred.

| Corpus | Model | Mismatches (3 repeats) | Unsafe grants | p50 ns | p95 ns | p99 ns |
|---|---|---:|---:|---:|---:|---:|
| boundary | M0 | 3072 | 3072 | 3.5 | 3.8 | 7.2 |
| boundary | M1 | 0 | 0 | 7.0 | 8.0 | 8.5 |
| boundary | M2 | 3072 | 3072 | 5.1 | 5.8 | 6.6 |
| boundary | M3 | 0 | 0 | 227.9 | 234.9 | 328.8 |
| boundary | M4 | 0 | 0 | 221.5 | 231.1 | 237.4 |
| canonical | M0 | 6141 | 6141 | 3.5 | 5.4 | 6.5 |
| canonical | M1 | 3717 | 381 | 5.8 | 10.5 | 14.0 |
| canonical | M2 | 5955 | 189 | 4.2 | 9.1 | 11.9 |
| canonical | M3 | 0 | 0 | 222.0 | 441.1 | 693.6 |
| canonical | M4 | 0 | 0 | 25.8 | 382.9 | 444.2 |
| held-out | M0 | 18429 | 18429 | 3.5 | 4.1 | 6.4 |
| held-out | M1 | 11454 | 1212 | 6.2 | 8.5 | 12.0 |
| held-out | M2 | 18147 | 603 | 4.6 | 7.1 | 11.2 |
| held-out | M3 | 0 | 0 | 202.4 | 416.9 | 455.6 |
| held-out | M4 | 0 | 0 | 32.2 | 227.1 | 387.8 |
| pathological | M0 | 3072 | 3072 | 3.5 | 3.9 | 6.4 |
| pathological | M1 | 2496 | 273 | 5.5 | 8.4 | 12.5 |
| pathological | M2 | 3060 | 111 | 4.1 | 7.9 | 11.4 |
| pathological | M3 | 0 | 0 | 24.2 | 431.6 | 469.6 |
| pathological | M4 | 0 | 0 | 23.1 | 41.8 | 394.5 |
| randomized | M0 | 18432 | 18432 | 3.5 | 4.6 | 6.4 |
| randomized | M1 | 11427 | 1197 | 6.4 | 8.9 | 12.6 |
| randomized | M2 | 18114 | 648 | 4.6 | 7.5 | 11.1 |
| randomized | M3 | 0 | 0 | 202.2 | 418.4 | 471.5 |
| randomized | M4 | 0 | 0 | 31.8 | 214.2 | 375.4 |

Selected scoped laboratory candidate: **M4**.
Truth gates precede cost. M3 remains the admissible exhaustive counterpoint.
Timings include clock overhead amortized over eight calls; fixture construction and I/O excluded.
These are distributions of batch-average CPU times, not individual-action tail latency.
Three repeats assess local run stability, not independent replication or confidence intervals.
A finite-radius conservative visibility query can deny marginal surface interactions.
Movement support is stationary only; moving support is honestly blocked.
Full release exit **BLOCKED** on the individually listed integration/provenance/H4 gates.
