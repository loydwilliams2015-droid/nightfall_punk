# 1.8A.4 registered alternatives and falsifiers

| Hypothesis | Prediction | Experiment | Preliminary result |
|---|---|---|---|
| H-DYN-01 actual impulse | A dynamic crate must change velocity under incoming actual impulse; an estimate alone will not | Same 1000 point contacts across 5 policies | H1 supported; negative control 0/1000 real crate updates |
| H-DYN-02 momentum | Without external motor work during isolated pair contact, pair linear momentum is conserved within declared tolerance | 1000 direct point contacts x 4 actual models | H1 supported in sampled pairs, max residual 0.00024414 kg*m/s |
| H-DYN-03 angular | Off-centre contact must induce angular response when inertia permits | Off-centre 1000-case corpus | H1 supported for angular models; no rotation in linear model |
| H-DYN-04 friction | Tangential contact impulse respects Coulomb bound and reduces relative tangential motion | 1000 pair cases and unit fixture | H1 supported in tested sliding cases; static resting friction not tested |
| H-DYN-05 adaptive | Bounded adaptive solver preserves admissible outcomes with fewer evaluations | 1000 single + 500 chain matched fixed/adaptive | H1 provisional, 44.7% fewer evaluations in selected mix; no native milliseconds |
| H-DYN-06 overload | On unsolved deep islands, do not commit a partial body state | Eight-body high-speed chain, hard cap six | H1 fixture PASS via explicit pending/rollback |
| H-DYN-07 geometry and traversal | A legitimate 1.8A.3 model C verdict can authorize motor input, not teleport; invalid/stale contracts cannot | Re-adjudication, stale actor version, repeated tick, geometry | H1 fixture PASS, actual stair motor trajectory still unimplemented |
| H-DYN-08 persistent history | Only actual committed contact impulses may be promoted to long-term threshold history | Receipt adapter plus A.2 monotone history check | H1 adapter PASS; durable stream/persistence and full world commit missing |

Next held-out challenge: mass ratios 1:1000 and 1000:1, four-point resting manifolds, rotating/tilting crates, contiguous multiple hits, stairs under low ceilings while pushing an object, friction under gravity, platform-borne support, T=0.005 vs 0.0001 convergence, multiplayer reconciliation, and long-horizon support fatigue. Never infer cross-platform bit-identical behavior from same-process float replay.
