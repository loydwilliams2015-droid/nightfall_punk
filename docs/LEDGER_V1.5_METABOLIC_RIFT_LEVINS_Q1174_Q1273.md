# nightfall!punk v1.5 — Metabolic Accounting Re-evaluation II

## Metabolic Rift + Levins — Q1174–Q1273

Status: **PRE-BUILD WEAK-POINT CONSOLIDATION — NO COMPILE**

This pass re-evaluates the prior metabolic-accounting closure using:

- John Bellamy Foster, *Marx's Theory of Metabolic Rift: Classical Foundations for Environmental Sociology* (1999);
- the metabolic-rift/social-metabolism reference trail through Liebig, nutrient cycling, town-country separation, waste/restitution and material-flow analysis;
- Richard Levins, *The Strategy of Model Building in Population Biology* (1966);
- Richard Levins, *Dialectics and Systems Theory* (1998);
- relevant Levins/Lewontin modeling themes: environmental heterogeneity, indirect effects, sufficient parameters, clusters of models and robustness.

## Historiographic precision

Foster's **metabolic rift** is a historically specific Marxian critique of capitalist social metabolism. nightfall deliberately transposes its **causal structure** into a fictional systemic ecology; the engine should not claim that every ecological rupture is, historically/theoretically, *the* metabolic rift.

Prefer neutral engine terms such as:

- `restitution_gap`
- `metabolic_discontinuity`
- `return_deficit`

while keeping the theoretical lineage explicit in design documentation.

## Major re-evaluation

The first metabolic sweep correctly separated:

`PhysicalTruth != Health != EnergyCarrier != ShadowValue`

This pass adds two decisive distinctions:

`CurrentStock != ReproductionOfStockOrCapacity`

and

`GlobalBalance != LocalMetabolicContinuity`

A system can conserve total material while one locality loses the conditions of reproduction and another accumulates unusable waste/externality load. A locality may therefore be temporarily abundant while the relations that reproduce that abundance deteriorate.

## Metabolic causal spine

`SourceCapacity`
`-> Extraction / Appropriation`
`-> Transfer / Use`
`-> Work / Consumption`
`-> Sink / Waste / DisplacedMaterial`
`-> Recovery / Return / Regeneration (or failure)`
`-> ChangedSourceAndSinkCapacities`
`-> NewPhysicalState`
`-> ActorLegitimateEvidence`
`-> ActorBelief`
`-> ShadowValue / NestedAccount`
`-> TacticalAction`
`-> NextFlow`

## Prior open questions re-evaluated

### Q1169 — AUTO KEEP + AMEND
First proof keeps three concrete strategic channels:

- metabolic / usable reserve;
- ammunition;
- repair material.

Only materially compatible carriers convert or participate in restitution. Generic energy is not universal fungibility.

### Q1170 — AUTO KEEP
Exact actor-local shadow/scarcity values remain F4/debug-only. Normal gameplay presents material scarcity, condition and consequence rather than optimizer arithmetic.

### Q1171 — AUTO KEEP + AMEND
The first proof now requires a **minimal restitution loop**, not depletion/recharge alone. Extraction/use changes a source; return/recovery/absorption can partially reproduce future capacity.

### Q1172 — AUTO DEFER
Do not implement individual metabolic overdraw/negative-energy debt in the first v1.5 proof. Physical stocks remain nonnegative. The new theoretical priority is reproduction of conditions and broken return relations.

### Q1173 — AUTO KEEP
Temporal claims govern current physical access. Actor-local marginal value plus relation/Crew/future-option accounts govern yielding, transfer, contest or redirection. No bidding market.

## New implementation contracts

1. **Return/restitution becomes a first-class relation.**
2. **Source and sink locality remain distinct.** Global conservation cannot hide local depletion/overload.
3. **Lag matters.** Extraction, capacity decline, sink overload and restoration can occur on different timescales.
4. **External subsidy is distinct from restored circulation.** A system may remain viable but subsidy-dependent.
5. **Rift-like state is derived, not a magic scalar.** Candidate sufficient parameters include restitution gap/ratio, source reproductive capacity, sink/externality load, cycle delay/dependency and uncertainty.
6. **Sufficient parameters are many-to-one.** Keep low-level posting provenance beneath them.
7. **`posterior_yield` remains useful but cannot become the rift metric.** It collapses multiple causal pathways.
8. **Extend `nf_energy`; do not duplicate stocks.**
9. **Likely C seam:** fixed-capacity metabolic links/cycle summaries keyed by source/sink/process, holding bounded recent/cumulative outflow, restitution, loss, delay/dependency and derived gap.
10. **Actor epistemic limits remain intact.** World truth records the rift; AI uses only legitimately learned beliefs about it.

## Levins modeling contract

Do not answer ecological complexity with a one-to-one simulator.

Use a **cluster of bounded models**:

1. stock/flow ledger balance;
2. source-sink-return graph/capacity model;
3. actor-local tactical valuation over beliefs.

A qualitative result becomes stronger if materially different simplifications preserve its sign/order. Disagreement is evidence that the theorem or parameterization is fragile, not something to conceal.

### Candidate robust theorem

> Persistent net extraction/export without adequate return/restoration tends to reduce source reproductive capacity or increase dependency/sink burden, eventually changing sustainable future action value — even when current output or total global stock remains high.

This is a theorem candidate to test across models, not a claim that one exact parameterization is universally true.

## C update-order recommendation

`PhysicalAction / Posting`
`-> StockAndFlowSettlement`
`-> Sink / Regeneration / CapacityUpdate`
`-> DerivedRestitutionSummaries`
`-> Evidence / Belief`
`-> SlowerTacticalAppraisal`

Candidate evaluation remains side-effect-free.

## Acceptance tests added

- equal starting stock: persistent export without return eventually reduces source reproductive capacity relative to bounded restitution;
- global total can remain equal while source capacity falls and sink load rises;
- two actors with different evidence about the same metabolic discontinuity assign different shadow values/actions;
- a known reproduction failure changes tactical appraisal, while an unknown one does not magically enter actor utility;
- at least two alternative simplified parameterizations preserve the qualitative restitution theorem or reveal its fragility.

## Q1269–Q1273 — OPEN

### Q1269 — First-proof restitution geometry
Two-node source↔use/return pair, three-node source→use/sink→recovery loop, or single-node embedded loop?

### Q1270 — First explicit return material/process
Recovered repair material, reclaimed charge/metabolic carrier, processed contamination/organic matter, or a more abstract compatible return flow?

### Q1271 — Encounter-timescale severity
Subtle capacity drift, strategically significant reversible decline, or visible threshold event?

### Q1272 — Player/lore naming
Hidden `restitution_gap`, diegetic eco-tech language such as cycle break/return deficit, or occasional explicit theoretical terminology?

### Q1273 — Pre-build robustness requirement
Two deterministic harnesses plus graphical proof, three independent simplified harnesses before graphics, or one minimal harness with later robustness testing?

## Result

100 questions considered: **95 automatically resolved/amended/invalidated/deferred; 5 remain open.**

No compile was run.
