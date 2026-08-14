# nightfall!punk — Social Ecology / Structural Constraint Code Sketch

Status: **accepted design / pre-implementation only**

This file does not alter the v1.0 code-complete acceptance candidate and implies no compile. It consolidates only the currently useful Lewontin/stigmergy/institution concepts for a later earned implementation slice.

Canonical closeout for the expanded social/political-economy question round: `docs/LEDGER_SOCIAL_ECONOMY_CLOSEOUT.md`.

## Scope

Keep:

- objective causal memory distributed across actors, objects, traces, routes, infrastructure and routines;
- existence before essence: the world need not assign one authoritative memory-object or ecological interpretation;
- self-stigmergy: one actor may be its own sufficient expectation and later encounter its own prior world-writing;
- distributed cognition: shared artifacts carry claims/evidence, not a shared mind;
- kinetic stigmergy: fast embodied action writes materially relevant traces into the Lifeworld;
- passive institutional drift from changing conditions;
- active institutional drift from intentional reinterpretation, repair, appropriation, sabotage or restoration;
- structural constraint as an actor-relative inference from observable/believed relations;
- micro/macro constitutive continuity with nonidentity of scale/form;
- Crew affiliation and class position as nonidentical;
- surplus value as a consequence of energy accounting after reproduction/maintenance costs;
- property, commons, stewardship, enclosure, rent, wage, credit, debt, risk and mutual aid as relational patterns rather than global categorical truth.

Discard/defer for now:

- global Institution Manager;
- authoritative ecological-balance scalar;
- authoritative structural-violence scalar;
- universal trust/reputation meter;
- global cultural-power score;
- omniscient provenance for ambiguous traces;
- giant social planner / GOAP / opaque ML;
- fixed universal memory or institution TTL;
- full macroeconomics simulator;
- authoritative class assignment from Crew identity;
- automatic Rawlsian/non-Rawlsian truth flag;
- generalized finance AI;
- universal property/commons enum that replaces actual access and stewardship relations.

## 1. Objective trace, situated interpretation

```c
typedef enum NfTraceKind {
    NF_TRACE_TRAFFIC = 0,
    NF_TRACE_COMBAT,
    NF_TRACE_RESOURCE_USE,
    NF_TRACE_REPAIR,
    NF_TRACE_CONTAMINATION,
    NF_TRACE_CATTLER_OCCUPATION,
    NF_TRACE_DROPPED_EQUIPMENT,
    NF_TRACE_ROUTE_CHANGE,
    NF_TRACE_SIGN
} NfTraceKind;

typedef struct NfWorldTrace {
    NfTraceKind kind;
    NfVec3 position;
    uint16_t strength_q;
    uint16_t material_persistence_q;
    uint16_t counter_erasure_q;
    uint64_t changed_tick;
} NfWorldTrace;
```

The trace owns only physically justified state. It does not need an authoritative social meaning or actor/faction provenance unless that provenance is materially explicit.

Actor-side interpretation remains separate:

```c
typedef struct NfTraceInterpretation {
    NfTraceKind observed_kind;
    uint16_t confidence_q;
    uint16_t source_confidence_q;
    uint16_t consequence_relevance_q;
    uint16_t familiarity_q;
} NfTraceInterpretation;
```

Canonical boundary:

`objective trace != situated interpretation != socially transmitted report`

## 2. Rhizomatic memory / no mandatory event essence

Do not require every distributed residue to share one semantic event ID. Networking/debugging may keep technical provenance where required, but actor cognition reconstructs continuity from evidence.

```c
typedef struct NfMemoryEvidence {
    uint16_t spatial_coherence_q;
    uint16_t causal_compatibility_q;
    uint16_t temporal_relation_q;
    uint16_t source_support_q;
    uint16_t material_continuity_q;
} NfMemoryEvidence;
```

A memory may be objectively instantiated across multiple objects even where no actor reconstructs exactly the same essence from those manifestations.

## 3. Self-sufficient expectation

No minimum social population is required before a repeated practice can guide behavior.

```c
typedef struct NfSituatedExpectation {
    uint16_t support_q;
    uint16_t contradiction_q;
    uint16_t consequence_q;
    uint16_t applicability_q;
    uint16_t social_corroboration_q; /* may legitimately be zero */
} NfSituatedExpectation;
```

An individual may externalize a practice, later encounter its own trace, and reproduce it:

`actor -> trace -> same actor later -> renewed action`

Social corroboration can strengthen or transform the expectation but is not ontologically required for the expectation to exist.

## 4. Passive and active institutional drift

Institutional drift is not a state-machine transition.

```c
typedef enum NfDriftCause {
    NF_DRIFT_PASSIVE = 0,
    NF_DRIFT_ACTIVE
} NfDriftCause;

typedef struct NfDriftEvidence {
    NfDriftCause cause;
    uint16_t expectation_mismatch_q;
    uint16_t material_change_q;
    uint16_t actor_agency_q;
    uint16_t intervention_affordance_q;
    uint16_t consequence_q;
} NfDriftEvidence;
```

Passive drift: topology, access, infrastructure, population, ecology or rhythms change and inherited expectations cease matching consequences.

Active drift: an actor intentionally repairs, reinterprets, appropriates, erases, redirects, sabotages or restores the practice/world support.

Both may coexist.

## 5. Actor-relative structural constraint

Do not store `structural_violence = X` as world truth. Preserve the causal relations and let actors/diagnostics estimate how the reproduced arrangement constrains situated viability.

```c
typedef struct NfStructuralConstraintEvidence {
    uint16_t access_asymmetry_q;
    uint16_t dependency_q;
    uint16_t constraint_persistence_q;
    uint16_t risk_transfer_q;
    uint16_t contestability_q;
    uint16_t alternative_viability_q;
    uint16_t evidence_confidence_q;
} NfStructuralConstraintEvidence;

typedef struct NfStructuralConstraintEstimate {
    uint16_t constrained_access_q;
    uint16_t reproduced_dependency_q;
    uint16_t transferred_risk_q;
    uint16_t practical_exit_loss_q;
    uint16_t contestability_loss_q;
    uint16_t confidence_q;
} NfStructuralConstraintEstimate;
```

Suggested relation, deliberately decomposed rather than collapsed to one master score:

```c
static NfStructuralConstraintEstimate
nf_estimate_structural_constraint(const NfStructuralConstraintEvidence *e)
{
    NfStructuralConstraintEstimate out = {0};
    if (e == NULL) return out;

    out.constrained_access_q = e->access_asymmetry_q;
    out.reproduced_dependency_q =
        (uint16_t)(((uint32_t)e->dependency_q *
                    (uint32_t)e->constraint_persistence_q) / 1000u);
    out.transferred_risk_q = e->risk_transfer_q;
    out.practical_exit_loss_q = (uint16_t)(1000u - e->alternative_viability_q);
    out.contestability_loss_q = (uint16_t)(1000u - e->contestability_q);
    out.confidence_q = e->evidence_confidence_q;
    return out;
}
```

The estimate is descriptive/actor-relative. It does not itself force hostility, moral judgment, faction alignment or event state.

## 6. Structural violence implementation criterion

A reproduced institution/practice becomes a candidate case of structural violence for actor `i` when the ordinary causal relations repeatedly:

1. constrain `i`'s sustainable activity/access;
2. reproduce dependency or transfer disproportionate risk/cost toward `i`;
3. reduce viable alternatives or practical exit;
4. reduce `i`'s practical capacity to contest/transform the arrangement;
5. preserve or enlarge corresponding possibilities for another actor/group/institutional position;
6. persist through ordinary reproduction rather than one isolated hostile act.

This criterion is relational and evidential. Several components may be present without all being equally strong, and useful institutional function can be compoundly instantiated with domination.

A situated activity may itself be one concrete existence/determination of the structural relation; do not force an artificial causal ladder `actor action -> separate structure -> consequence`. The micro relation may instantiate the macro totality without exhausting it.

Severity remains nonidentical to structural character: harsh normalized arrangements and sometimes relatively soft recurrent arrangements may satisfy the structural criterion, while severe one-off harm may not.

## 7. Kinetic stigmergy remains the physical write path

Do not create a separate social-writing mode. Existing/future Fuzzy Rail + Relational Physics interactions should be able to emit materially justified traces while preserving kinetic flow:

`movement -> affordance capture -> world alteration -> trace -> momentum-preserving release`

A later actor may read that trace differently, or the same actor may encounter its own prior trace.

Kinetic stigmergy is a first-class gameplay feature: compatible manipulation should be snappy, context-sensitive and chainable with movement rather than requiring a separate slow interaction mode.

## 8. Social gradient / collective action

Use the already accepted gradient-memory logic instead of a new collective-mind system:

`action -> memory -> changed affordance gradient -> future action`

Collective action emerges when actors become part of one another's conditions of action through changed affordances, commitments, traces, reports or expectations.

Shared intention is one form of collective coordination but is not required for all material cooperation.

A collective relation becomes structurally reproductive when its traces/material supports make future collective capacity easier to reproduce even after initiating actors leave.

## 9. Economic relation evidence

Crew identity is not class identity. A Crew may intersect multiple class relations depending on access, dependency, surplus claims, risk and alternatives.

Do not store a permanent actor/class essence where a situated relation is sufficient.

Candidate compact evidence:

```c
typedef struct NfEconomicRelationEvidence {
    uint16_t access_q;
    uint16_t dependency_q;
    uint16_t alternative_viability_q;
    uint16_t contestability_q;
    uint16_t risk_borne_q;
    uint16_t contribution_q;
    uint16_t reproduction_cost_q;
    uint16_t claim_q;
    uint16_t confidence_q;
} NfEconomicRelationEvidence;
```

For actor cognition, fields must be legitimate beliefs/evidence rather than hidden authoritative interpretation.

## 10. Surplus value from energy accounting

Surplus is accepted as **surplus value of energy accounting** rather than an unrelated economic currency.

Candidate diagnostic/disposition shape:

```c
typedef struct NfSurplusDisposition {
    int32_t produced_q;
    int32_t reproduction_cost_q;
    int32_t maintenance_q;
    int32_t repair_q;
    int32_t distributable_q;
} NfSurplusDisposition;
```

Conceptually:

`distributable = produced - reproduction_cost - maintenance - repair`

Do not automatically materialize this as one omniscient gameplay meter if existing energy-account flows already preserve the necessary causes. The important simulation question is how claims/distribution/reinvestment alter future reserves, access, infrastructure, disagreement points and gradients.

## 11. Rawlsian / non-Rawlsian test boundary

Do not author an automatic gameplay moral classifier.

For design/diagnostic reasoning, an inequality is Rawlsian only when the unequal arrangement improves the sustainable possibilities of the least-advantaged position relative to materially feasible alternatives without manufacturing the disadvantage through suppressed access, alternatives or contestability.

A bargain can be voluntary and individually rational while participating in a non-Rawlsian relation if the weak disagreement point is itself structurally reproduced.

Working design shorthand:

- equality or Rawlsian inequality on a relation -> no class struggle on that relation;
- non-Rawlsian inequality for a situated person -> micro-economic class struggle;
- materially similar non-Rawlsian relations reproduced across a structural position -> macro-economic class struggle.

## 12. Commons / stewardship / enclosure

A commons may be:

- spontaneous/self-existing and substantially maintained by its own ecological/material recurrence; or
- methodically stewarded by an interested union, guild, Crew, cooperative or local association responsible for that segment of the commons.

Candidate stewardship relation:

```c
typedef struct NfStewardshipRelation {
    uint16_t upkeep_q;
    uint16_t regeneration_support_q;
    uint16_t shared_access_q;
    uint16_t exclusion_q;
    uint16_t future_viability_q;
    uint16_t controller_advantage_q;
} NfStewardshipRelation;
```

Restriction is not automatically enclosure. Enclosure is indicated where access/stewardship control increasingly reproduces exclusion/dependency for positional advantage rather than common sustainable reproduction.

## 13. Wage / rent / credit / debt / risk

Keep these as relational configurations over existing energy, claim, access and memory systems:

- **wage exploitation:** surplus claims importantly enabled by a reproduced non-Rawlsian bargaining asymmetry rather than legitimate Rawlsian contribution/risk/stewardship;
- **rent:** access claims beyond legitimate reproduction of the valuable affordance, especially when scarcity/dependency is manufactured or preserved;
- **credit:** intertemporal cooperation via present transfer and future claim;
- **credit as class power:** future claims recursively reshape borrower disagreement points/access/risk/activity;
- **exploitative debt:** repayment suppresses feasible alternatives beyond lender sacrifice/risk/contribution and reproduces non-Rawlsian dependency;
- **risk allocation:** derive from control, expected benefit, contribution, knowledge, exposure and loss-absorption capacity rather than role label;
- **insurance/mutual aid:** commons-like when pooled reserves are stewarded to reproduce shared resilience instead of leverage over members requiring aid.

## 14. Integration target

Later earned implementation should extend the existing `nf_lifeworld`, `nf_energy`, actor-belief, trace, spatial and relationship substrates rather than replace them.

Preferred chain:

`world facts -> situated evidence/belief -> disagreement alternatives -> ordinary action/bargain -> material consequence -> trace/memory -> changed gradient`

No full social/economic planner is earned by this design round.

## 15. SIGNAL handoff

Question round is paused. Resume from the three questions recorded in `docs/LEDGER_SOCIAL_ECONOMY_CLOSEOUT.md`:

1. money relative to universal energy accounting;
2. dues/taxes/levies as stewardship vs extraction;
3. bankruptcy/restructuring/debt forgiveness vs strict repayment.

No compile is authorized by this document.