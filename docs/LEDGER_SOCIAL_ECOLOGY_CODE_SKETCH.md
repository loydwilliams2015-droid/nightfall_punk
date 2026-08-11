# nightfall!punk — Social Ecology / Structural Constraint Code Sketch

Status: **accepted design / pre-implementation only**

This file does not alter the v1.0 code-complete acceptance candidate and implies no compile. It consolidates only the currently useful Lewontin/stigmergy/institution concepts for a later earned implementation slice.

## Scope

Keep:

- objective causal memory distributed across actors, objects, traces, routes, infrastructure and routines;
- existence before essence: the world need not assign one authoritative memory-object or ecological interpretation;
- self-stigmergy: one actor may be its own sufficient expectation and later encounter its own prior world-writing;
- distributed cognition: shared artifacts carry claims/evidence, not a shared mind;
- kinetic stigmergy: fast embodied action writes materially relevant traces into the Lifeworld;
- passive institutional drift from changing conditions;
- active institutional drift from intentional reinterpretation, repair, appropriation, sabotage or restoration;
- structural constraint as an actor-relative inference from observable/believed relations.

Discard/defer for now:

- global Institution Manager;
- authoritative ecological-balance scalar;
- authoritative structural-violence scalar;
- universal trust/reputation meter;
- global cultural-power score;
- omniscient provenance for ambiguous traces;
- giant social planner / GOAP / opaque ML;
- fixed universal memory or institution TTL.

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

## 7. Kinetic stigmergy remains the physical write path

Do not create a separate social-writing mode. Existing/future Fuzzy Rail + Relational Physics interactions should be able to emit materially justified traces while preserving kinetic flow:

`movement -> affordance capture -> world alteration -> trace -> momentum-preserving release`

A later actor may read that trace differently, or the same actor may encounter its own prior trace.

## 8. Integration target

Later earned implementation should extend the existing `nf_lifeworld` causal-memory substrate rather than replace it. The present v1.0 landbase already stores objective causes such as infestation, human use, maintenance, disturbance and process condition, while actor-facing valuation is explicitly belief-derived. Social ecology should preserve that same truth/knowledge separation.

Implementation order when earned:

`world trace -> situated interpretation -> expectation -> passive/active drift -> structural-constraint estimate -> ordinary actor choice`

No compile is authorized by this document.