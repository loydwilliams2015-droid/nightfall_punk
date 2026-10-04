# Contact-system hypothesis register and five open questions

## Logic distinction
**A priori** here means a design/logical entailment *conditional on agreed engine axioms*, not a claim to timeless physical truth. E.g., if semantics cannot cause physics without a material carrier, a semantic-only path must not be a contact. **Inductive** engineering predictions remain defeasible: e.g. a capsule feels better than a box or 4 iterations usually suffice.

| ID | Statement | Type | Falsifying test | Preliminary disposition |
|---|---|---|---|---|
| D1 | No actor-local collision truth from inaccessible material state | hard | hidden-wall world changes with evidence held fixed | LOCK architectural; no integrated proof |
| D2 | One physical contact cannot be independently committed twice | hard | repeated contact at same tick/ID | LOCK architectural; history partial |
| D3 | Solver exhaustion cannot become a clear path | hard | corner with deliberately insufficient iteration budget | H1 pass selected fixtures |
| D4 | Obstacle identity/normals/TOI must reconstruct a consequential decision | hard | trace reconstruction comparison | H1 limited, no full engine event integration |
| D5 | Rendering/observer never mutates physical authority | hard | OFF/ON world replay | inherited 1.7E gate; collision overlay unimplemented |
| I1 | CCD beats endpoint-only queries on high-speed thin geometry | defeasible | matched adversarial fixtures | H1 support |
| I2 | Small bound (2–4) is enough for ordinary movements | defeasible | dense contact + held-out scene | H1 restricted; 2 enough for existing fixtures |
| I3 | Broadphase should scale with local contacts, not world colliders | defeasible | sweep 16→100,000 unrelated bodies | UNSUPPORTED by current O(N) scan |
| I4 | True capsule improves slopes/steps/comfort vs swept box | defeasible | paired cap/box + human movement | H0 |
| I5 | Event promotion beats full contact logging for causal trace per byte | defeasible | long-duration rest/sliding events + reconstruction | H0/H1 partial |

## Five open questions + planned adversarial studies

1. **Capsule vs box:** HOLD geometry/actor/input fixed. Compare thin edges, door frames, staircase corners, ledges and cylindrical obstacle paths. Predict capsule improves edge behavior at small cost; falsify by more false grounding, clipping, or greater P95 cost with no play-feel benefit. Selection requires H3 native profiling + H4 human feel, otherwise incumbent box remains.
2. **Dynamic player vs kinematic controller:** paired kinematic+impulse interface vs dynamic constrained body. Impulse conservation and player turnaround latency are distinct metrics. Falsify full dynamics if control overshoot or catastrophic contact jitter dominates visible benefit. Do not equate current estimated `normal_impulse` with physical conservation.
3. **Contact iteration budget:** compare 1/2/4/8 with fixed seeds and additional adversarial held-out geometry. Hard failures are tunneling or invented success; soft failures are pending and poor traversal. Stop at cheapest strategy satisfying reliability with a small adaptive emergency budget; current 2-success finding applies only to the first corpus.
4. **Collision vs cell resolution:** compare continuous geometric surface authority with 1 m and local 0.5/0.25 m semantic refinement. Check lossless ownership/material consequence and whether refined cells increase meaningful causal recall enough to justify runtime. Collision does not inherit semantic cell resolution.
5. **Contact-event granularity:** trace begin/end/hit/threshold transitions; compare all-substep logging with threshold/promoted histories. Hard zero: consequential event loss. Soft objectives: bytes/sec, reconstruction accuracy, frame overhead. Persistent resting support must generate *support state* even with zero displacement; current sweep-only path does not.

## Methodological pluralism
- **Quine:** a failed prediction tests a network of assumptions (shape, scale, solver, fixture, timestep, support contract). Revise the most local unsupported assumptions first; avoid ad hoc rescue of a favored solver.
- **Kuhn:** anomalous stair jitter, false grounding and tunneling are candidate diagnostic crises for incumbent geometric assumptions; measure them rather than redefining the objective away.
- **Lakatos:** hard core = material truth, causal conservation, information locality; protective belt = collider shape, bias tolerances, iteration budget, cache/broadphase. A progressive change must make new successful predictions on held-out fixtures.
- **Feyerabend:** rival implementations need a fair common corpus. A deliberately simple legacy controller remains useful as a control, not an embarrassing artifact to delete.
- **Popper:** design severe negative controls targeting false positives; a pass on hand-authored fixtures does not show the hypothesis true in all environments. Report negative and pathological failures transparently.

## Evidence promotion rubric
H0 coherent specification only; H1 compiled/synthetic fixtures; H2 controlled ablation with confidence estimates and held-out scenarios; H3 native full-engine runtime and hardware profiling; H4 human play outcomes. **H2 is not earned merely by a 12,000-row same-fixture resample**, and H3 is not earned merely by having a C executable.