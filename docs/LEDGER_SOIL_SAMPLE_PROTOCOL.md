# nightfall!punk — Soil Sample Protocol

Timestamp: **2026-08-18 21:45 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — Soil Sample Scientific Evidence Protocol]**
Roles: **PRAXIS / SIGNAL / STEWARD**
Record specificity: **PROJECT-WIDE METHODOLOGY / BUILD EVIDENCE SAMPLING + ECOLOGICAL/DATA-SCIENCE ANALYSIS**
Status: **CURRENT CANONICAL TERMINOLOGY + EVIDENCE WORKFLOW / NO COMPILE IMPLIED**

## Definition

A **soil sample** is a bounded scientific evidence packet for a build, subsystem, or feature. It treats gameplay and simulation evidence the way field ecology and soil science treat a heterogeneous site: sample deliberately, preserve context, distinguish replication from subsampling, retain variation, model detectability, and infer only within the scope actually observed.

Canonical formulation:

```text
BUILD / SYSTEM
    -> STRATIFIED EVIDENCE SOURCES
    -> RUNS / VIDEOS / SIMULATED PLAY / TESTS
    -> OBSERVATION PACKETS
    -> DISTRIBUTIONS + CAUSAL CHAINS + COUNTEREXAMPLES
    -> CREDIBILITY + LIMITS
    = SOIL SAMPLE
```

A **soil sample of v1.4** therefore means: collect the available v1.4 human-play video, deterministic/simulated play, automated tests and traces; preserve their source and dependency structure; analyze them through the shared scientific-investigation lessons of environmental/ecological science and computer/data science; then produce a compact scoped evidence profile of what v1.4 actually demonstrates.

A soil sample is not a global score and is not automatically a proof of the whole build.

---

## Why the metaphor

Soils and ecosystems are heterogeneous. A sample is useful only when its spatial/temporal/depth/context metadata and sampling design are known. Game builds are similarly heterogeneous across actors, scenarios, seeds, loads, player behavior, code paths, hardware/network conditions and observation methods.

Canonical maxim:

> **Heterogeneity is often the object of study, not noise to be averaged away.**

---

## Soil Sample — condensed protocol

For each build or subsystem, record:

```text
SOIL-SAMPLE ID
BUILD / COMMIT / VERSION
QUESTION / TARGET CLAIM
SAMPLING SCOPE
EVIDENCE STRATA
SAMPLING UNITS
REPLICATION FAMILIES
OBSERVATION METHODS
MEASUREMENTS / DISTRIBUTIONS
CAUSAL CHAINS
POSITIVE / NEGATIVE / BOUNDARY CONTROLS
COUNTEREXAMPLES / ANOMALIES
DETECTABILITY / MISSINGNESS LIMITS
CREDIBILITY WITHIN SCOPE
WHAT IS NOT ESTABLISHED
FOLLOW-UP SAMPLE
```

### 1. Define the question

Sample the question, not everything.

Example:

```text
Does v1.4 preserve bounded information transfer while producing legible embodied teammate response?
```

### 2. Define the sampling population and strata

State what body of behavior the sample is meant to represent, then divide known heterogeneity into relevant strata.

Possible strata:

```text
human video
simulated/deterministic play
automated regression
AI family: teammate / Rival / Cattler
ordinary / surge / event load
positive / negative / boundary condition
seed / scenario / geometry family
```

Do not mix unlike strata into one average.

### 3. Distinguish sample, subsample, composite and replicate

Project terminology:

```text
OBSERVATION = one measured event/window
SUBSAMPLE   = one bounded observation inside a run/video
CORE/RUN    = one coherent execution or recording
COMPOSITE   = combined evidence across declared subsamples/strata
REPLICATE   = a genuinely independent-ish execution opportunity
```

Ten clips from one video are not ten independent replicates. Ten runs from one identical seed/fixture are repeated measurements with correlated ancestry, not ten fully independent confirmations.

### 4. Preserve specificity metadata

Retain the minimum metadata needed to reproduce and interpret the sample:

```text
commit/build
experiment/run id
seed/scenario
subsystem/actor class
spatial/encounter context
performance tier
relative tick/time
observation source
replication/dependency family
security classification
```

Do not retain unnecessary personal identifiers, exact private location, credentials, IP/MAC addresses, hostnames, private conversation identifiers, or unrelated filesystem/user information.

### 5. Separate world state, observation process and inference

Canonical scientific distinction:

```text
ENGINE / WORLD STATE
        !=
OBSERVATION / TELEMETRY / VIDEO CAPTURE
        !=
OUR INFERENCE
```

Failure to observe a behavior is not automatically observation that the behavior was absent.

For human video in particular, distinguish:

```text
behavior occurred but was not visible
behavior was visible but not noticed
behavior was noticed but misinterpreted
behavior genuinely did not occur
```

### 6. Store distributions, not averages alone

For repeated quantitative measures retain, where meaningful:

```text
N
median
IQR or MAD
P90 / P95 / P99
min / max
failure count/rate
zero-event count
outlier/tail count
mean as secondary statistic only
```

If the system has multiple behavioral modes, preserve buckets/histograms or bounded samples rather than collapsing them into one mean.

### 7. Trace causal chains

Record expected and observed relations:

```text
CAUSE
-> STATE CHANGE
-> EVIDENCE
-> BELIEF / VALUE
-> INTENT
-> AUTHORITATIVE RESOLUTION
-> MATERIAL CONSEQUENCE
-> NEW TRACE / EVIDENCE
```

A soil sample should localize where a relation broke rather than merely label the whole system good/bad.

### 8. Use positive, negative and boundary controls

Every important claim should seek:

```text
POSITIVE: condition where behavior should occur
NEGATIVE: condition where behavior must not occur
BOUNDARY: threshold/edge where outcome changes or becomes uncertain
```

This is especially important for hidden-information leaks, collision/physics legality, report delivery, targeting, and network authority.

### 9. Preserve anomalies and counterexamples

Do not delete a contradictory observation because the dominant pattern looks good.

Classify:

```text
noise / measurement issue
known boundary condition
missing variable
model contradiction
premise / architecture violation
```

A verified illegal state is not averaged into legality by many ordinary successful trials.

### 10. State credibility and limits

Conclude with a scoped credibility statement rather than a universal verdict.

Recommended vector:

```text
replication
independence
effect strength
stability
predictive success
causal coherence
scope robustness
counterexample burden
```

Then explicitly state:

```text
SUPPORTED WITHIN SCOPE
MIXED
FRAGILE
UNDER-SAMPLED
CONTRADICTED
ROBUST WITHIN SCOPE
```

and always include **WHAT IS NOT ESTABLISHED**.

---

## Soil Sample vs PAR sheet

These are complementary but distinct instruments.

```text
SOIL SAMPLE asks:
What does our build actually do, with what variation, under what observed conditions?

PAR SHEET asks:
Is that demonstrated capability on par with the required/peer benchmark on stated dimensions?
```

Preferred order:

```text
SOIL SAMPLE
-> CURRENT DEMONSTRATED STATE
-> PAR SHEET
-> GAP / RESERVE / TARGET
-> NEXT EXPERIMENT OR BUILD CHANGE
```

A PAR sheet should not claim `MEETS PAR` on a dimension for which the current build lacks a sufficient soil sample.

---

## Example — v1.4 soil sample

Target question:

```text
Does v1.4 demonstrate bounded evidence transfer and embodied response without hidden shared cognition?
```

Evidence strata:

```text
A. v1.4 human-play video(s)
B. deterministic communication tests
C. simulated/integrated population runs
D. Rival/teammate diagnostic traces
E. negative-control cases before report/evidence delivery
```

Sampling units may include:

```text
gate interaction sequence
report PENDING -> LIVE transition
teammate belief update
teammate displacement/route response
Rival DIRECT/AUDIBLE/REPORT evidence transitions
claim expiration/release
population/camera regression observations
```

Analysis should answer:

```text
What happened repeatedly?
What did not happen when prohibited?
What varied by context?
What failure tails occurred?
Which causal edges are directly observed versus inferred?
How independent are the runs?
What does video detect poorly?
What does machine evidence detect poorly?
What remains unproven outside the sampled fixture?
```

Output example:

```text
SOIL SAMPLE v1.4

ROBUST WITHIN SCOPE:
- delayed evidence can precede belief revision rather than instant telepathy
- gate revision can causally alter route utility and embodied response

SUPPORTED:
- Rival/teammate communication remains behaviorally active after removal of exact shared blackboard truth

UNDER-SAMPLED:
- broader seed/geometry diversity
- surge-load claim behavior
- long-duration communication tails

NOT ESTABLISHED:
- universal multiplayer/network behavior
- all Cattler communication modes
- project-wide performance under large populations
```

The exact result must come from the collected evidence; this template does not pre-judge v1.4.

---

## Longitudinal use

Soil samples can be compared across builds without pretending that all tests are identical:

```text
SOIL SAMPLE v1.4
-> SOIL SAMPLE v1.5
-> SOIL SAMPLE v1.6
-> SOIL SAMPLE v1.6A
```

This forms a **development transect**: a sequence of scoped samples showing how causal behavior, variation, failure tails and evidence quality change through development.

Do not infer improvement merely from version number. Compare common sampling dimensions and record changed fixtures/conditions.

---

## PRAXIS / SIGNAL / STEWARD

### PRAXIS

- identify sampling units and emit bounded machine-readable measurements;
- preserve deterministic tests, traces and run identity;
- store distributions and causal event relationships;
- avoid unnecessary sensitive metadata.

### SIGNAL

- stratify evidence;
- compare video, telemetry, simulated play and tests without treating them as interchangeable;
- inspect distributions, tails, counterexamples, detectability and alternative explanations;
- report credibility only within sampled scope.

### STEWARD

- preserve provenance, specificity, dependency families and security boundaries;
- prevent subsamples from masquerading as independent replication;
- distinguish observation from inference and absence from non-detection;
- keep unresolved anomalies visible;
- prevent a narrow soil sample from silently becoming a project-wide premise.

---

## Canonical maxims

> **Sample the question, not everything.**

> **A clip is a subsample; a run is not automatically a replicate.**

> **No observation is meaningful without its scope.**

> **Non-detection is not automatically absence.**

> **Store the distribution; inspect the tail.**

> **Heterogeneity is often the phenomenon.**

> **The soil sample establishes current evidence; the PAR sheet evaluates adequacy against a benchmark.**

> **A build can embarrass the theory, and the sample should preserve the embarrassment.**
