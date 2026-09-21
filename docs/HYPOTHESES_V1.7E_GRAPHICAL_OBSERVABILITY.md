# nightfall!punk v1.7E — Graphical Observability Sampling Hypotheses

## Sampling doctrine
Every sample is paired where possible: identical seed/input with viewer OFF and with one selected preset. Samples are stratified by canonical, median, worst, pathological and disagreement cases. Performance claims require compiled native measurements; synthetic/headless results may validate logic but not GPU/frame-rate claims.

| ID | Hypothesis | Sample organization | Supporting result | Falsifying result |
|---|---|---|---|---|
| E1 | Read-only observation is behaviorally inert | OFF vs PLAY/WORLD/ACTOR/CAUSAL/FULL, identical seed | identical authoritative hashes/outcomes | any unexplained authoritative divergence |
| E2 | CPU semantic snapshot + batched presentation scales better than per-object debug drawing | actor/cell/transaction count sweep | draw/batch growth substantially slower than object growth | near-linear draw-call growth or severe CPU submission cost |
| E3 | Hierarchical local epochs reduce unnecessary recomputation without stale diagnostic state | global-only vs nexus/chunk epochs | fewer invalidations/recomputes with zero stale reads | stale reads or no cost reduction |
| E4 | Phase-barrier borrowing lowers timing jitter while emergency reserve preserves consequential responsiveness | equal workloads, immediate-borrow challenger vs barrier incumbent | lower P95/P99 variance with no critical-event latency regression | starvation, critical delay, or higher variance |
| E5 | Smallest-common-authority scopes reduce false conflict grouping without losing true conflicts | object/cell/nexus/contract/island scope fixtures | fewer oversized sets with 100% true-conflict recall | missed material/contract conflict |
| E6 | Hot + promoted + cold trace organization keeps memory bounded while preserving consequential reconstruction | long replay with ordinary + rare consequential events | bounded resident memory and high reconstruction of promoted events | unbounded memory or missing consequential cause chain |
| E7 | Graceful visual aggregation preserves causal identity under load | near/nexus/region representations of same event | same event/state identity with reduced detail/cost | aggregate view changes meaning or implies false state |
| E8 | Actor/world overlay separation makes epistemic disagreement visible without leaking truth | disagreement/pathological seeds | WORLD and ACTOR views diverge exactly when evidence warrants | actor view reveals inaccessible truth or suppresses legitimate disagreement |
| E9 | Purple visualization has high causal precision | one-way, reciprocal, cross-target false-cycle, three-domain-cycle samples | Purple only around actual reciprocal target-local SCC | Purple on one-way/cross-target interaction or missed reciprocal coupling |
| E10 | Event-driven observability is cheaper than continuously rebuilding unchanged overlays | static/quiet vs rapidly changing scenes | quiet scenes sharply reduce snapshot/batch rebuild work | unchanged scenes incur similar rebuild cost |
| E11 | Diagnostic detail can decay before responsiveness | overload ramp | interaction/selection stays responsive while detail aggregates | UI/inspection stalls before detail reduction |
| E12 | FULL is a forensic mode, not the performance model for ordinary play | same scene across presets | PLAY near baseline; individual presets cheaper than FULL | instrumentation cost remains high even with overlays disabled |
| E13 | Primitive graphical scenes expose architecture defects earlier than large composite maps | isolated labs vs combined sweep | failures localize to specific subsystem/interaction | defects only observable after full composite complexity |
| E14 | Reason traces improve causal reconstruction beyond visual outcome alone | blinded developer/tester reconstruction, trace off/on | higher correct explanation rate with traces | no improvement or traces mislead |
| E15 | Observability cost is dominated by visible/changed causal information rather than total world size | same local scene embedded in increasing inactive world sizes | modest cost increase with inactive distant scale | cost tracks total world size despite inactivity/aggregation |

## Core data columns
Every benchmark row should retain build, scene, seed class, seed, preset, visible cell count, nexus count, actor count, transaction count, conflict-set count, Purple count, frontier size, refined-unit count, trace bytes, batch count, debug primitive count, simulation time, snapshot time, debug-build time, render time, total frame time, authoritative hash and replay hash.

## Scientific promotion
E1/E5/E8/E9 may progress from H0 to H1/H2 through deterministic and controlled graphical fixtures.
E2/E3/E4/E6/E7/E10/E11/E12/E15 require compiled runtime profiling for H3.
E14 requires human/developer testing before H4.

## Additional overlap-composition hypotheses
| ID | Hypothesis | Sample organization | Supporting result | Falsifying result |
|---|---|---|---|---|
| E16 | Multidimensional composition reduces clutter without losing causal information | same scene with naive stacked overlays vs composed object | fewer visible primitives/draw submissions with equal inspector reconstruction | missing dimension, misleading primary, or no clutter reduction |
| E17 | Priority suppression improves immediate diagnostic legibility | timed identification of invariant/pending/conflict state with/without priority composition | faster correct identification with no causal-reconstruction loss | slower or less accurate identification |
| E18 | Equal-priority rainbow composition preserves genuine simultaneity better than arbitrary tie-breaking | equal-priority belief/contract/causal fixtures | all co-primary dimensions recoverable and visibly represented | one dimension dominates/disappears |
| E19 | Anchor-local composition scales better than global overlay ordering | increasing object/cell counts with local overlap clusters | cost tracks local overlap density rather than global dimension count | composition cost approaches global all-to-all ordering |
