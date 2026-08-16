# nightfall!punk — Protocol 3: Evidence Structures + Inference Governance

Timestamp: **2026-08-16 16:31 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — Evidence Structures and Inference Governance]**
Roles: **PRAXIS / SIGNAL / STEWARD**
Record specificity: **PROJECT-WIDE METHODOLOGY / OBSERVATION + INDUCTION + CONTRADICTION + EXPERIMENT GOVERNANCE**
Status: **PROTOCOL 3 — ADOPTED DESIGN WORKFLOW / NO COMPILE IMPLIED**

## Purpose

Protocol 1 names deduction-active premises. Protocol 2 defines the compile-bracketed observation and credibility routine. Protocol 3 defines **what evidence is stored, how it is related causally, how external references acquire typed authority, how inductive confidence is updated without naive independence assumptions, how contradictions are classified, and how experiments are registered**.

Canonical architecture:

```text
EXPLICIT PREMISES
      |
      v
DEDUCTIVE PREDICTIONS
      |
      v
EXPERIMENT REGISTRY
      |
      v
OBSERVATION PACKETS + DISTRIBUTIONS + CAUSAL CHAINS
      |
      +--> REFERENCE EVIDENCE (typed authority)
      |
      v
DEPENDENCY-AWARE INDUCTIVE UPDATE
      |
      v
CONTRADICTION ENGINE
      |
      v
PATTERN CREDIBILITY + MULTIDIMENSIONAL VALUE
      |
      v
KEEP / AMEND / INVESTIGATE / DEFER / REJECT / PROMOTE-CANDIDATE
```

Deduction and induction remain distinct. External evidence may strongly validate or invalidate **inductive reasons** when its authority and scope match are high, but it does not silently rewrite an accepted deductive premise.

---

# 1. Observation packet

Every material observation should be stored as a typed packet rather than a prose conclusion.

Required conceptual fields:

```text
observation_id
experiment_id
build_commit
version / branch
subsystem / feature
seed / scenario
actor/population scope
spatial scope
performance tier
trigger / starting condition
observed state transition(s)
measurements
causal-chain reference
evidence source type
replication-family id
specificity
security classification
supports / contradicts / neutral / anomaly
notes
```

Observation text should describe what occurred. Interpretation belongs in SIGNAL analysis.

Bad:

```text
Cattlers are too aggressive.
```

Good:

```text
evidence lost at t=41.2 s
search persisted 31.1 s
no new direct/audible/report evidence observed
withdrawal did not occur
```

---

# 2. Store distributions, never averages alone

A mean may be retained as a secondary statistic, but **no repeated behavioral/performance claim should be represented by an average alone**.

For repeated quantitative measures, store where meaningful:

```text
N
median
mean (secondary)
IQR or MAD
P90
P95
P99
minimum
maximum
failure count
failure rate
zero-event count
outlier/tail count
```

When multimodality is plausible, retain histogram/bucket or bounded sample information so two distinct behavioral modes are not collapsed into one mean.

Examples:

```text
search duration:
N=60
median=10.8s
P90=22.1s
P95=28.7s
P99=49.8s
max=51.3s
unexplained-persistence=2/60
```

```text
path query cost:
median=0.4ms
P95=1.2ms
P99=3.8ms
max=9.6ms
budget violations=4/10000
```

**Tail behavior is first-class evidence.** Rare telepathy, deadlock, severe hitching, path paralysis, impossible geometry traversal, or unreadable AI may matter more than the central tendency.

Implementation preference when code is added later:

- online/streaming quantile or histogram summaries where appropriate;
- bounded reservoirs rather than unlimited raw hot-path logging;
- deterministic fixed-capacity telemetry buffers in performance-critical paths;
- preserve enough raw sample identity to diagnose failures without retaining unnecessary personal metadata.

---

# 3. Causal-chain records

Every important experiment should define the expected causal chain before execution and record the observed chain afterward.

Template:

```text
CAUSE C0
  -> STATE C1
  -> EVIDENCE C2
  -> BELIEF / VALUE C3
  -> INTENT C4
  -> AUTHORITATIVE RESOLUTION C5
  -> MATERIAL CONSEQUENCE C6
  -> TRACE / NEW EVIDENCE C7
```

Each edge may record:

```text
expected? yes/no
observed? yes/no/unknown
timestamp/tick
source subsystem
state revision/event id
confidence of observation
```

Example:

```text
PLAYER OPENS GATE
-> gate revision 1 -> 2
-> Crew report issued
-> report PENDING
-> report LIVE
-> teammate evidence becomes CREW_REPORT
-> belief OPEN
-> route utility changes
-> movement intent changes
-> authoritative movement produces displacement
```

A failure can therefore be localized:

```text
world revision       PASS
report issue         PASS
report delivery      PASS
belief revision      PASS
utility revision     FAIL
movement consequence ABSENT
```

This prevents "AI failed" from obscuring the actual causal break.

---

# 4. Metadata security and privacy discipline

Evidence is useful only if its metadata is safe enough to retain and share.

## Public-safe metadata

Prefer:

- build/commit hash;
- semantic branch/version;
- subsystem/feature/test id;
- seed and scenario id;
- anonymous actor/entity ids scoped to the run;
- relative simulation tick/time;
- generalized hardware/performance tier when needed;
- generalized OS/runtime environment when needed;
- semantic region/cell ids rather than private real-world coordinates where not necessary;
- redacted public chat provenance already required by project policy.

## Do not store in public telemetry/ledgers unless specifically necessary and explicitly reconciled

- personal names;
- account names;
- home-directory usernames;
- machine hostnames;
- IP addresses;
- MAC addresses;
- access/session tokens;
- private repository credentials;
- private conversation titles/identifiers;
- exact personal geolocation;
- unrelated absolute filesystem paths;
- raw identifiers that permit unnecessary cross-session user tracking.

## Pseudonymous run identity

Use run/session IDs generated for the experiment. Do not make them stable personal identifiers by default.

## Raw media

Video/audio may contain more private context than machine telemetry. Public records should store a redacted result/reference rather than reproducing unnecessary private details.

## Security maxim

> **Record enough metadata to reproduce the cause, not enough metadata to reconstruct the person.**

---

# 5. Reference material as evidence with typed authority

External reference material is not one undifferentiated category. It receives an explicit evidence type and authority profile.

Recommended reference types:

```text
REF_SCIENTIFIC_EMPIRICAL
REF_TECHNICAL_PRIMARY
REF_HISTORICAL_EMPIRICAL
REF_MECHANISM_PRECEDENT
REF_COMPARATIVE_GAME
REF_LITERARY_STRUCTURAL
REF_PHILOSOPHICAL_THEORETICAL
REF_GENEALOGICAL_INFLUENCE
REF_AESTHETIC_REFERENCE
REF_COUNTEREXAMPLE
REF_DIAGNOSTIC_NEGATION
```

For each reference, record:

```text
reference_id
type
source/provenance
specific mechanism/claim being imported
authority domain
scope match to nightfall problem
independence from other sources
empirical status
known limitations
supports / weakens / bounds which hypothesis
reference status: constitutive / important comparative / diagnostic / genealogical / etc.
```

## Authority rule

Reference authority is **domain-specific**.

Examples:

- primary ethology can have high authority for plausible predator-behavior hypotheses;
- engine/vendor documentation can have high authority for technical API behavior;
- a successful FPS can have high mechanism-precedent authority for interaction design but low authority over nightfall ontology;
- a novel can have high structural/narratological authority for a literary design question but does not establish physical truth;
- philosophical theory may strongly organize hypothesis formation or contradiction analysis without becoming a substitute for measured game behavior.

## Inductive role

References may materially update inductive reasoning when:

```text
AUTHORITY IN DOMAIN
x SCOPE MATCH
x MECHANISM SIMILARITY
x SOURCE QUALITY
x INDEPENDENCE
```

is high.

They may:

- raise or lower a prior plausibility;
- suggest a mechanism;
- provide an external counterexample;
- bound a claim's scope;
- invalidate an inductive generalization when a strong external counterexample applies;
- motivate a new test.

They may **not** by themselves:

- claim that nightfall code implements the reference behavior;
- convert a design analogy into a physical law;
- override an explicit project premise without a separate STEWARD reconciliation.

Canonical maxim:

> **Reference evidence can strongly move inductive reasons without impersonating deductive truth.**

---

# 6. Dependency-aware / non-naive Bayesian model

Naive Bayes assumptions are explicitly rejected for correlated project evidence.

Ten observations from one seed/setup/source are not treated as ten independent confirmations. Repeated reports from one causal origin are not corroboration; the same rule applies to project evidence.

## Evidence grouping

Every observation should carry a `replication_family_id` and, when useful, a broader dependency group:

```text
same build
same seed
same scenario
same actor initialization
same source dataset/reference
same human observer
same causal mechanism
```

## Conceptual update

When calibrated probabilities/likelihoods exist, the preferred conceptual form is:

```text
posterior log-odds
=
prior log-odds
+ SUM(
    dependency_discount
  * scope_match
  * source_reliability
  * measurement_quality
  * log(likelihood_ratio)
  )
```

with explicit caps/discounts on repeated evidence from one dependency family.

Do not invent likelihood ratios when they are not measured. In that case use the Protocol-2 ordinal credibility vector plus qualitative Bayesian direction:

```text
STRONGLY RAISES
RAISES
SLIGHTLY RAISES
NEUTRAL
SLIGHTLY LOWERS
LOWERS
STRONGLY LOWERS
```

## Priors

Priors may come from:

- accepted mechanistic theory;
- strong external empirical reference;
- prior nightfall experiments;
- base rates from the subsystem;
- deliberately weak/uninformative priors when little is known.

Do not encode aesthetic preference as empirical prior unless the question is explicitly about aesthetic preference.

## Hierarchical scope

Prefer hierarchical reasoning where observations share structure:

```text
project-level pattern
  -> subsystem pattern
      -> scenario family
          -> seed/run
```

Evidence can inform nearby levels while remaining partially pooled rather than either fully isolated or falsely universalized.

## Credible intervals / uncertainty

When numerical modeling is warranted, report uncertainty intervals rather than a single posterior point estimate. Do not interpret a Bayesian posterior as metaphysical certainty; it is conditional on model, prior, measurement, and scope.

## Hard-premise boundary

A probabilistic inductive update cannot average away a hard architecture violation.

Example:

```text
99 ordinary successful trials
+
1 verified exact unseen-position leak
```

means the epistemic invariant has a **real violation**, not "99% correctness." Frequency can measure occurrence rate; it does not convert physical/logical illegality into legality.

---

# 7. Contradiction engine

The contradiction engine must classify conflicts before attempting resolution.

## C0 — Apparent contradiction / scope mismatch

Both claims may be correct because they refer to different actors, times, versions, locations, performance tiers, or specificity levels.

Example:

```text
"Cattlers usually foreground 1–2 hunters"
vs
"Cattler ecology may contain larger clans"
```

No contradiction: embodied local presentation != ecological population.

## C1 — Temporal supersession

A later explicit record replaces an older one within the same scope.

Resolution: preserve history, use current precedence.

## C2 — Empirical contradiction

An observation materially conflicts with a hypothesis or predicted pattern.

Resolution: inspect measurement, missing variables, boundaries, model revision.

## C3 — Premise contradiction

A proposal conflicts with an accepted stronger premise.

Resolution: invalidate/defer proposal unless premise itself is explicitly reopened with sufficient evidence.

## C4 — PARADOX / genuine physical or logical impossibility

Project use of **paradox** here means a hard incompatibility within the same authoritative state/scope: two conditions cannot physically or logically both hold under the accepted world rules.

Examples:

```text
same solid wall is simultaneously intact authoritative collision
AND nonexistent authoritative collision
at the same tick/state without a transformation event
```

```text
actor both has and does not have the same unique authoritative body position
in the same authority frame
```

Resolution requires finding an invalid premise/state representation, a missing transformation, a bug, or an incorrectly defined model. Do not synthesize a physically impossible state as a "compromise."

## C5 — DIALECTIC / game-theoretical contradiction

A **dialectic** is a real conflict between simultaneously viable relations, incentives, strategies, values, or tendencies. Both sides may be materially true; the conflict is resolved or transformed through situated action, bargaining, competition, temporal development, institutional change, or changed world state.

Examples:

```text
Crew A benefits from cooperation for Cattler defense
AND
Crew A benefits from contesting the same scarce route/resource
```

```text
fast aggression improves immediate control
AND
restraint preserves future reciprocity/options
```

This is not a physical impossibility. It is a game-theoretical conflict requiring resolution under payoffs, information, history, topology, and time.

Resolution modes may include:

```text
tradeoff
mixed strategy
conditional strategy
sequencing
bargaining
partition
deterrence
cooperation
competition
withdrawal
institutional rule
world-state transformation
```

## C6 — Paraconsistent epistemic contradiction

Different actors/reports may hold mutually inconsistent beliefs without the authoritative world becoming inconsistent and without every conclusion becoming derivable.

Example:

```text
Rival A report: gate OPEN
Rival B stale memory: gate CLOSED
WORLD TRUTH: gate OPEN
```

Resolution: preserve provenance, confidence, time, and actor scope; do not collapse all beliefs into one global truth cache.

## Contradiction maxim

> **Do not synthesize impossibility; do not mistake conflict for impossibility.**

---

# 8. Experiment registry

Every meaningful experiment receives a stable registry entry before execution.

Minimum fields:

```text
EXPERIMENT ID
TITLE
STATUS: planned / running / completed / superseded
QUESTION
HYPOTHESIS
PREMISE DEPENDENCIES
REFERENCE EVIDENCE
PREDICTION
POSITIVE CONTROL
NEGATIVE CONTROL
BOUNDARY CASE
MANIPULATION / INDEPENDENT VARIABLES
DEPENDENT MEASURES
EXPECTED DISTRIBUTION / FAILURE TAILS
CAUSAL CHAIN
SCOPE
REPLICATION PLAN
INDEPENDENCE PLAN
STOPPING RULE
BUILD / COMMIT
SEEDS / SCENARIOS
SECURITY CLASSIFICATION
AUTOMATED RESULTS
HUMAN OBSERVATIONS
VIDEO / TRACE REFERENCES
COUNTEREXAMPLES
CONTRADICTIONS
CREDIBILITY VECTOR
BAYESIAN/QUALITATIVE UPDATE
MULTIDIMENSIONAL VALUE VECTOR
DISPOSITION
FOLLOW-UP EXPERIMENTS
```

## Stable experiment identity

Do not recycle experiment IDs when a hypothesis changes materially. Create a new revision/experiment and link ancestry.

## Stopping rule

Where practical, define the stopping condition before looking at results, e.g.:

```text
20 runs across >=5 seeds
OR
stop immediately on verified architecture/premise violation
```

This reduces opportunistic sampling.

## Experiment ancestry

Track:

```text
PARENT EXPERIMENT
-> modification
-> new prediction
-> new result
```

This lets STEWARD distinguish progressive model improvement from ad hoc patching.

---

# 9. Counterexample and anomaly registry

Counterexamples must remain queryable even after explanation.

Each should carry:

```text
counterexample_id
experiment/observation ids
class: noise / boundary / missing-variable / model contradiction / premise violation
severity
reproducibility
causal chain
resolution status
follow-up test
```

A resolved counterexample becomes part of model history; it is not deleted from the record.

---

# 10. Promotion and invalidation

Inductive pattern promotion requires more than raw frequency.

Candidate promotion should consider:

```text
replication
independence
effect strength
stability
predictive success
causal coherence
scope robustness
counterexample burden
reference support/negation
Bayesian/qualitative update
multidimensional project value
```

Possible promotions:

```text
observation -> repeated pattern
repeated pattern -> subsystem hypothesis
subsystem hypothesis -> robust-within-scope design rule
robust design rule -> version policy
version policy -> project-wide premise only by explicit STEWARD reconciliation
```

Strong external reference evidence can accelerate hypothesis formation and narrow uncertainty, but it does not skip the implementation/evidence ladder.

---

# 11. Implementation sequencing

Protocol 3 is adopted now as methodology. **No compile is implied.**

When implementation work is authorized, prefer this sequence:

1. machine-readable experiment/observation IDs and metadata schema;
2. fixed-capacity/low-overhead telemetry distributions and causal event IDs;
3. experiment registry files/log format;
4. trace-to-video reconciliation helpers;
5. credibility and contradiction analysis tooling;
6. calibrated dependency-aware Bayesian computation only where sufficient data exists.

Do not retrofit heavy telemetry into a green candidate solely to satisfy methodology. Introduce instrumentation when the next build has an actual experimental consumer.

---

# 12. PRAXIS / SIGNAL / STEWARD responsibilities

## PRAXIS

- emit bounded machine-readable events and measurements;
- record distributions/tails rather than means alone;
- expose causal IDs/revisions;
- preserve deterministic controls;
- avoid logging sensitive metadata;
- implement experiment instrumentation only through existing subsystem owners.

## SIGNAL

- compare simple and complex explanations;
- evaluate patterns, counterexamples, external references, and predictive success;
- apply typed reference authority;
- perform dependency-aware inductive updates;
- distinguish credible recurrence from desirable design value.

## STEWARD

- enforce specificity, provenance, security, and status;
- maintain experiment/counterexample ancestry;
- classify contradictions before resolution;
- distinguish paradox/physical impossibility from dialectical/game-theoretical conflict;
- prevent correlated evidence from masquerading as independent corroboration;
- require explicit reconciliation before inductive evidence becomes deductive premise.

---

# 13. Canonical maxims

> **Store the distribution; inspect the tail.**

> **Record the causal chain, not just the outcome.**

> **Reference authority is typed and domain-bounded.**

> **Correlated repetitions are not independent corroboration.**

> **Bayesian updating is conditional reasoning, not manufactured certainty.**

> **Do not synthesize impossibility; do not mistake conflict for impossibility.**

> **A dialectic can contain two real opposing incentives; a paradox cannot contain two physically incompatible authoritative states without model revision.**

> **An experiment has an identity, prediction, controls, scope, and ancestry.**

> **Record enough metadata to reproduce the cause, not enough metadata to reconstruct the person.**

> **Induction may earn promotion; it may not silently seize deductive authority.**
