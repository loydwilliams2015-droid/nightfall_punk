# nightfall!punk — v1.5 Metabolic Accounting Closure Q1024–Q1173

Status: **PRE-BUILD WEAK-POINT CONSOLIDATION — NO COMPILE**

## Reference synthesis

This sweep uses four reference families:

1. David Thompson / City of Edmonton, *The Power of Prices and the Failure of Markets: Addressing Edmonton's Environmental and Fiscal Challenges* (2010): prices are useful coordinating signals but omitted environmental costs, institutional design and market failure matter.
2. GIST / Green Accounting for Indian States: output measures alone omit natural, human and social capital and important environmental externalities.
3. Austria Environmental Accounts / Umweltgesamtrechnung: physical energy/material-flow accounts should remain distinct from monetary accounts, with hybrid accounts linking the systems without conflating them.
4. Richard L. Sandor / environmental finance: bounded environmental markets require measurable claims, explicit rules, verification and settlement; the useful contribution is mechanism design, not universal commodification.

Canonical rule:

`PHYSICAL TRUTH != ACCOUNTING VALUATION != PRICE / SHADOW VALUE`

`Physical State -> Account -> Actor Belief -> Shadow Valuation -> Tactical Choice -> Embodied Consequence`

## Automatic closure result

Q1024–Q1173 contains 150 questions.

- 145 auto-resolved / invalidated / deferred.
- 5 remain open: Q1169–Q1173.

## Core metabolic ontology

`Health = current relational functional viability`

`Metabolic Accounting = stocks + flows + transformations + productive/regenerative capacities that reproduce or sustain future function`

`Carrier / Resource != Energy != Health != Price`

Preserve distinct semantics for:

- reserves / stocks;
- income / inflows;
- expenditure / outflows;
- productive capacity;
- regenerative capacity;
- absorptive capacity;
- external input;
- conversion efficiency and loss;
- transfers;
- stock change;
- waste / contamination / sinks.

Do not represent physical debt by negative stocks.

## Existing machinery

`nf_energy` remains the authoritative physical/accounting substrate. Do not replace it with a new monolithic metabolism system.

Its existing useful machinery includes actor reserves/income/expenditure/productive capacity, regional potential, regenerative and absorptive capacity, external inputs, conversion loss, topographic cost, multiple time horizons and uncertainty-aware `NfEnergyBelief` search.

The new work should strengthen the operational linkage:

`nf_energy physical state -> actor-local belief -> tactical shadow value -> nested-account candidate -> embodied action -> authoritative posting`

## Physical accounting rules

- Every authoritative posting must have a world cause.
- Transfers debit a source and credit a destination, with explicit loss when applicable.
- External inputs and sinks are explicitly marked as outside the modeled boundary.
- Pure tactical evaluation cannot mutate accounts.
- Transfers/conversions require idempotent event/command handling.
- Stock underflow/overflow must not silently corrupt accounting.
- Debug/tests should reconcile beginning stock + inputs/transfers/conversions - expenditures/losses = ending stock under the model's explicit rules.

## Ecological material-flow rule

Important flows should be traceable through:

`Extraction -> Stock/Input -> Conversion/Use -> Work/Transfer -> Waste/Contamination -> Absorption/Decay/Recovery -> Changed Productive/Regenerative Capacity`

Environmental consequences exist in authoritative world state whether or not actors know them. Actor tactical utility may internalize those consequences only when legitimate evidence/belief supports doing so.

Thus a high-private-return action can rationally occur under incomplete knowledge while producing a real externalized cost; later traces/reports/world degradation can internalize that cost into future decisions.

## Shadow valuation

A tactical shadow price/value is an **actor-relative marginal opportunity-cost signal**, not a conserved physical quantity and not intrinsic worth.

It may depend on:

- current reserves and flows;
- productive/regenerative capacity;
- accessibility and travel cost;
- substitutes and alternatives;
- uncertainty;
- Crew need and relation history;
- route/objective value;
- known locality/externality consequences;
- reversibility and future options.

The same physical resource may therefore have different rational shadow values for different actors.

No shadow price overrides physical feasibility or hard viability gates.

## Market/mechanism design boundary

v1.5 does **not** implement a literal open market for combat/ecological resources.

Use market theory as a mechanism-design reference:

- prices can coordinate scarce alternatives;
- omitted externalities make private signals incomplete;
- incomplete information can create divergent rational valuations;
- route/resource control can produce market-power-like asymmetry;
- claims require measurement, rights, validity, transfer and settlement rules;
- non-price rules remain legitimate where market signals are inappropriate.

Literal emissions markets, generic exchanges, credit instruments and macroeconomic firm/consumer simulation are deferred.

## Credit/debit boundary

Credit/debit is primarily an accounting grammar for postings.

- Physical energy/resource stocks remain nonnegative.
- If debt, obligation or future claims become literal later, they belong in a separate claim/obligation ledger.
- Code should prefer explicit posting names (`TRANSFER_IN`, `EXPENDITURE`, `EXTERNAL_INPUT`, etc.) over ambiguous financial terminology.

## Tactical implications

Metabolic accounting must make these states strategically distinct:

- high stock / weak replenishment;
- low stock / strong flow;
- high current health / unsustainable metabolism;
- damaged body / strong metabolic reserve;
- abundant resource / inaccessible location;
- scarce resource / strong substitute access;
- identical physical resource / different actor-local marginal value.

## Diagnostics and acceptance

F4 should distinguish:

`PHYSICAL STOCK | FLOW | CAPACITY | ACTOR BELIEF | SHADOW VALUE | SELECTED CONSEQUENCE`

Normal play should not become an economics dashboard.

Acceptance tests should prove:

1. transfer conservation minus explicit loss;
2. conversion loss;
3. external-input accounting;
4. nonnegative physical stock;
5. equal physical resource + different actor state/belief -> different shadow value;
6. same functional health + different reserve/flow -> different rational action;
7. known externality can change appraisal;
8. unknown externality cannot magically enter actor utility;
9. extraction/use can change future locality capacity where configured;
10. high-stock/low-regeneration can be less sustainable than lower-stock/high-flow.

## Positive criticism

The current energy architecture is already unusually well aligned with this direction. The principal deficit is not missing economic theory; it is the still-thin executable linkage between physical accounts, actor belief, marginal scarcity valuation, nested tactical appraisal and world consequences.

## Open Q1169–Q1173

- **Q1169:** concrete first-proof resource ontology / number of carriers.
- **Q1170:** how much shadow/scarcity valuation becomes visible outside F4.
- **Q1171:** how much locality externality/regeneration change the first integrated proof should simulate.
- **Q1172:** whether any short-term 'energy credit' mechanic exists (strict stock only vs controlled capacity drawdown vs separate obligation claim).
- **Q1173:** scarce-resource allocation mechanism among multiple actors (claims/role priority vs marginal-value yielding vs synthesis).

Vertical Integration Pass II remains deferred until these and the remaining weak-point sweeps close.
