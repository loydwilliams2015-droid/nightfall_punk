# nightfall!punk — Protocol 2: Observation + Pattern-Credibility Routine

Timestamp: **2026-08-16 16:22 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — Observation and Pattern Credibility Routine]**
Roles: **PRAXIS / SIGNAL / STEWARD**
Record specificity: **PROJECT-WIDE METHODOLOGY / TEST + OBSERVATION + INDUCTIVE EVALUATION**
Status: **PROTOCOL 2 — ADOPTED WORKFLOW / NO COMPILE IMPLIED**

## Purpose

Protocol 1 names the explicit premises deduction may use. Protocol 2 governs how observations become inductive evidence and how recurring patterns earn or lose credibility.

Canonical distinction:

```text
DEDUCTION: accepted premises -> prediction / constraint
INDUCTION: scoped observations -> recurring pattern -> credibility evaluation
```

Neither path silently overwrites the other.

The routine combines:

```text
AUTOMATED TELEMETRY
        +
DETERMINISTIC TESTS
        +
HUMAN PLAY OBSERVATION
        +
VIDEO / TRACE REVIEW
        +
PATTERN-CREDIBILITY EVALUATION
```

Pattern evaluation must include:

- pros and cons;
- supporting examples and counterexamples;
- binary and nonbinary judgments;
- simple and complex interpretations;
- multidimensional value analysis;
- explicit specificity and evidence status.

---

# 1. Compile-boundary rule

The routine is intentionally split across the compile boundary because the evidence types have different optimal timing.

```text
PRE-COMPILE
  -> state question
  -> name premises
  -> state hypothesis
  -> record predicted positive case
  -> record predicted negative case
  -> define measurements
  -> define counterexample / falsifier
  -> define scope
        |
        v
COMPILE / BUILD GATE
        |
        v
POST-COMPILE MACHINE EVIDENCE
  -> deterministic tests
  -> automated telemetry / trace capture
  -> regression checks
  -> distributions + failure tails
        |
        v
POST-COMPILE HUMAN EVIDENCE
  -> human play observation
  -> video / trace review
  -> causal legibility check
        |
        v
CREDIBILITY SYNTHESIS
  -> binary contract result
  -> nonbinary pattern result
  -> pros / cons
  -> examples / counterexamples
  -> simple / complex interpretation
  -> multidimensional value analysis
  -> disposition
```

### Why pre-compile prediction comes first

Writing expected outcomes before execution reduces post-hoc explanation and makes novel predictive success visible.

### Why telemetry/tests are post-compile

Executable state is required to observe actual causal transitions and performance distributions.

### Why human/video review follows machine evidence

Human review answers questions machine tests cannot answer well: legibility, perceived causality, pacing, affordance readability, surprise, confusion, aesthetic coherence, and whether a technically correct system communicates its cause.

### Why credibility synthesis comes last

Credibility must compare prediction, machine evidence, human evidence, counterexamples, and scope rather than privilege whichever source was most vivid.

---

# 2. Pre-compile observation card

Before a meaningful build/test cycle, record:

```text
QUESTION
What relation are we trying to learn about?

PREMISES
Which accepted premises constrain the test?

HYPOTHESIS
What pattern do we expect?

POSITIVE CONTROL
Under what conditions should the behavior occur?

NEGATIVE CONTROL
Under what conditions must the behavior not occur?

BOUNDARY CASE
Where may either outcome be legitimate?

PREDICTION
What measurable result should occur before we see the run?

COUNTEREXAMPLE / FALSIFIER
What result would materially weaken the proposed explanation?

MEASURES
Which values / events / transitions / player observations are recorded?

SCOPE
Build, subsystem, population, seed, region, scenario, performance tier.
```

Do not require a complex hypothesis for every smoke test. A simple invariant may use only question + positive/negative control + expected result.

---

# 3. Post-compile machine-evidence pass

## A. Deterministic tests

Use deterministic tests for exact causal contracts wherever possible.

Examples:

```text
NO LOS + NO REPORT -> NO TARGET KNOWLEDGE
REPORT PENDING -> NO REPORT-BASED BELIEF YET
INVALID GEOMETRY -> NO PHYSICAL TRAVERSAL
RELATIONSHIP FORBIDS DAMAGE -> NO AUTHORITATIVE HIT APPLICATION
```

Binary result:

```text
PASS / FAIL
```

A failed hard invariant is not averaged away by good playtest impressions.

## B. Automated telemetry / trace

Telemetry records what actually happened rather than a conclusion about what happened.

Preferred measurements include where applicable:

- event/state-transition counts;
- report issue/delivery/expiry times;
- perception/evidence source;
- route changes;
- claim wait/expiry/deadlock;
- exposure and cover state;
- search/reacquisition/withdrawal duration;
- combat duration;
- resource/energy consequences;
- frame/query/AI cadence cost;
- network correction or latency metrics;
- actor distribution / congestion;
- failure counts.

For repeated measures prefer distributions over means alone:

```text
median / P90 / P95 / P99 / maximum / failure rate
```

Rare tails may be more important than ordinary averages when the tail represents telepathy, deadlock, severe hitching, path paralysis, unreadable behavior, or other memorable failure.

---

# 4. Human play observation

Human observation is first-class evidence but must remain typed as human interpretation rather than simulation truth.

Observe at least:

- did the player understand what changed?
- was cause visible/audible/readable?
- was actor behavior plausibly connected to available evidence?
- did movement remain responsive?
- did the environment communicate affordances?
- did combat/systemic consequence feel materially connected?
- was uncertainty productive or merely confusing?
- did the player identify a strategy the designers did not script?
- did the player misattribute a technically correct behavior to the wrong cause?
- did pacing preserve contrast between ordinary pressure and spikes?

Useful observation vocabulary:

```text
SEEN
HEARD
INFERRED
MISINTERPRETED
SURPRISING-BUT-LEGIBLE
SURPRISING-AND-OPAQUE
EXPECTED
FRUSTRATING
EXPLOITABLE
MEANINGFULLY-EMERGENT
```

Do not rewrite a human comment into a machine fact. Example:

```text
HUMAN OBSERVATION:
"The Rival seemed to know where I was."

TRACE QUESTION:
What evidence did the Rival actually possess?
```

---

# 5. Video / trace reconciliation

Video and trace should be paired when diagnosing causality.

```text
VISIBLE / AUDIBLE PLAYER EXPERIENCE
        <->
AUTHORITATIVE STATE / EVENT TRACE
```

Typical classifications:

```text
SYSTEM CORRECT + PRESENTATION CLEAR
SYSTEM CORRECT + PRESENTATION UNCLEAR
SYSTEM WRONG + PRESENTATION APPEARS PLAUSIBLE
SYSTEM WRONG + PRESENTATION EXPOSES FAILURE
INSUFFICIENT EVIDENCE
```

A visible failure is evidence. Do not silently patch before recording the discrepancy.

---

# 6. Pattern-credibility evaluation

Pattern credibility means evidence that a relation will recur under stated conditions.

Do not reduce credibility to raw observation count.

Evaluate seven primary dimensions on an ordinal 0–4 scale:

```text
0 = absent / unknown
1 = weak
2 = moderate
3 = strong
4 = very strong
```

### C1 — Replication
Does the pattern recur across repeated trials?

### C2 — Independence
Are confirmations meaningfully independent rather than repeats of one seed/setup/causal source?

### C3 — Effect strength
Is the effect large enough to matter mechanically, strategically, perceptually, or materially?

### C4 — Stability / perturbation survival
Does the relation persist under modest changes in timing, seed, geometry, population, tuning, or starting state?

### C5 — Predictive success
Was the result predicted before observation? Did the hypothesis generate a novel successful prediction?

### C6 — Causal coherence
Can the state-transition chain explain why the pattern occurred?

### C7 — Scope robustness
Has the pattern survived tests across the domain for which it is being claimed?

Do **not** convert the seven scores into a fake precise probability by default. The vector itself matters.

Example:

```text
PATTERN: bounded Cattler search after evidence loss

C1 replication        4
C2 independence       3
C3 effect strength    3
C4 stability          3
C5 prediction         4
C6 causal coherence   4
C7 scope robustness   2

STATUS: STRONGLY SUPPORTED WITHIN TESTED LOCAL-PREDATOR SCOPE
```

High replication with zero independence means repeatable fixture behavior, not broad generality.

---

# 7. Counterevidence classification

Do not count every failure equally.

Classify counterexamples where possible:

```text
NOISE / MEASUREMENT ERROR
KNOWN BOUNDARY CONDITION
MISSING VARIABLE
MODEL CONTRADICTION
ARCHITECTURE / PREMISE VIOLATION
```

An architecture violation carries substantially greater diagnostic weight than ordinary behavioral variance.

Examples:

```text
BOUNDARY:
Cattler persists because den/habitat value remains high.

MISSING VARIABLE:
Third-party gunfire changed route utility.

PREMISE VIOLATION:
Actor obtains exact unseen position without legitimate evidence.
```

Counterexamples remain in the evidence record even when later explained.

---

# 8. Binary + nonbinary evaluation

Use both forms because they answer different questions.

## Binary layer — hard contract

For invariants and forbidden states:

```text
PASS / FAIL
TRUE / FALSE
OCCURRED / DID-NOT-OCCUR
LEGAL / ILLEGAL
```

Examples:

```text
Can an actor walk through a solid wall? NO.
Did report-based belief update before delivery? YES/NO.
Did the authoritative hit path bypass relationship permission? YES/NO.
```

Binary failure of a hard premise is immediately actionable.

## Nonbinary layer — graded pattern

For ecology, tactics, legibility, pacing, utility, pressure, complexity, and human interpretation use graded/continuous evidence.

Examples:

```text
search persistence
cover usefulness
route diversity
player comprehension
threat intensity
negotiation viability
systemic value
performance cost
```

A nonbinary pattern may be strong, weak, mixed, fragile, context-dependent, or multimodal rather than simply correct/incorrect.

Canonical rule:

> **Use bivalence for hard causal contracts; use graded analysis for genuinely graded relations.**

---

# 9. Simple + complex interpretation

Every significant pattern should receive both a minimal and, when earned, richer explanation.

## SIMPLE MODEL

Ask:

```text
What is the smallest causal explanation consistent with the evidence?
```

Prefer this when it explains the observed pattern without hidden mechanisms or duplicate authorities.

## COMPLEX MODEL

Ask:

```text
Do interacting variables materially improve explanation or prediction?
```

Potential variables include:

- actor history;
- evidence age/confidence;
- topographical cost;
- topological dependencies;
- morphology;
- resource state;
- third-party pressure;
- habitat state;
- congestion;
- temporal claims;
- player intervention.

Complexity is justified when it produces explanatory or predictive improvement, not merely because more variables are available.

Comparison:

```text
SIMPLE EXPLANATION
Rival moved because direct LOS changed threat utility.

COMPLEX EXPLANATION
Rival moved because LOS + depleted resource + prior truce + blocked retreat + Cattler pressure jointly altered utility.
```

If the simple model predicts equally well, keep the simple model.

---

# 10. Pros / cons + examples / counterexamples

Every credibility review must explicitly include all four categories:

```text
PROS
What does this pattern improve or explain?

CONS
What costs, fragilities, ambiguities, or side effects accompany it?

EXAMPLES
Which observations clearly instantiate the claimed pattern?

COUNTEREXAMPLES
Which observations weaken, bound, or contradict it?
```

This prevents a successful demonstration from becoming self-confirming doctrine.

---

# 11. Multidimensional value analysis

Credibility answers "is this pattern supported?"

Value analysis answers a different question:

> **Even if credible, is this pattern good for nightfall!punk?**

Evaluate where relevant:

```text
V1  causal coherence
V2  gameplay / strategic value
V3  player legibility
V4  systemic breadth / cross-system reuse
V5  ecological / social plausibility
V6  performance / network cost
V7  implementation complexity / maintenance burden
V8  failure-tail severity
V9  compatibility with accepted premises
V10 future-option value / extensibility
V11 aesthetic / experiential value
V12 accessibility / comprehension cost
```

Use ordinal scores or qualitative descriptors. Do not hide tradeoffs in one scalar total.

A mechanic may have high credibility but low project value.

Example:

```text
PATTERN:
Five Cattlers reliably create intense combat.

CREDIBILITY:
HIGH.

VALUE:
pressure = high
legibility = low
predator individuality = low
performance cost = high
routine-horde drift = high

DISPOSITION:
Do not generalize as ordinary encounter policy.
```

Conversely, a rare low-frequency mechanic may have modest statistical prevalence but high evental value and therefore remain desirable when explicitly scoped as rare.

---

# 12. Credibility states

Preferred nonnumeric states:

```text
UNOBSERVED
SINGLE INDICATION
REPEATED
SUPPORTED
STRONGLY SUPPORTED
ROBUST WITHIN SCOPE
PROMOTION CANDIDATE
```

Lateral/problem states:

```text
UNDER-SAMPLED
MIXED
ANOMALOUS
FRAGILE
CONTRADICTED
```

Promotion requires STEWARD reconciliation. Credibility increase does not automatically widen scope.

---

# 13. Final disposition

After each substantial cycle assign one primary disposition:

```text
KEEP
AMEND
INVESTIGATE
DEFER
REJECT
PROMOTE-CANDIDATE
```

Meaning:

- **KEEP** — evidence supports current implementation/design within scope;
- **AMEND** — causal core is useful but measurable change is needed;
- **INVESTIGATE** — anomaly/counterexample/missing variable prevents confident disposition;
- **DEFER** — evidence insufficient or test not worth current cost;
- **REJECT** — evidence or higher premises materially invalidate the proposal;
- **PROMOTE-CANDIDATE** — replicated, stable, predictive, causally coherent pattern may deserve broader subsystem/version premise after explicit reconciliation.

Never auto-promote an observation directly into project-wide canon.

---

# 14. Routine template

```text
[NF OBSERVATION / CREDIBILITY CARD]

Build / commit:
Seed / scenario:
Subsystem:
Population:
Spatial scope:
Performance tier:

QUESTION:
PREMISES:
HYPOTHESIS:

POSITIVE CONTROL:
NEGATIVE CONTROL:
BOUNDARY CASE:
PREDICTION:
COUNTEREXAMPLE / FALSIFIER:

AUTOMATED TEST RESULT:
TELEMETRY / DISTRIBUTION:
HUMAN PLAY OBSERVATION:
VIDEO / TRACE RECONCILIATION:

PROS:
CONS:
EXAMPLES:
COUNTEREXAMPLES:

BINARY CONTRACT:
NONBINARY PATTERN:

SIMPLE EXPLANATION:
COMPLEX EXPLANATION (if earned):

CREDIBILITY VECTOR:
C1 replication:
C2 independence:
C3 effect strength:
C4 stability:
C5 predictive success:
C6 causal coherence:
C7 scope robustness:

COUNTEREVIDENCE CLASS:
CREDIBILITY STATE:

MULTIDIMENSIONAL VALUE:
V1 causal coherence:
V2 gameplay/strategy:
V3 legibility:
V4 systemic breadth:
V5 ecology/social plausibility:
V6 performance/network cost:
V7 implementation complexity:
V8 failure-tail severity:
V9 premise compatibility:
V10 future-option value:
V11 aesthetic/experience:
V12 accessibility/comprehension:

DISPOSITION:
NEXT TEST / ACTION:
STEWARD SCOPE:
```

---

# 15. PRAXIS / SIGNAL / STEWARD ownership

## PRAXIS

- define machine-observable state transitions;
- create deterministic positive/negative controls;
- expose bounded telemetry and traces;
- preserve build/test reproducibility;
- avoid instrumentation that changes gameplay behavior materially.

## SIGNAL

- compare prediction against observation;
- identify distributions, recurrence, covariance, anomalies and counterexamples;
- perform simple-vs-complex interpretation;
- evaluate pattern credibility and multidimensional value;
- distinguish mechanism evidence from external analogy.

## STEWARD

- preserve build/commit/seed/scenario provenance;
- preserve observation independence/correlation families;
- maintain specificity and evidence status;
- prevent local success from becoming universal doctrine;
- require explicit reconciliation before premise promotion.

---

# 16. Integration with existing build workflow

Current v1.4 already separates:

```text
standard-check -> build -> deterministic tests/smokes -> demo -> human/video review
```

Protocol 2 overlays rather than replaces this sequence.

For the next implementation/build cycle, the preferred routine is:

```text
1. PRE-COMPILE OBSERVATION CARD
2. standard-check
3. compile/build
4. deterministic tests + regression/smokes
5. telemetry/trace capture
6. human demo/play
7. video/trace reconciliation
8. credibility + multidimensional value evaluation
9. KEEP / AMEND / INVESTIGATE / DEFER / REJECT / PROMOTE-CANDIDATE
```

Do not modify the already-green v1.4 gameplay candidate solely to retrofit this methodology. Hook machine-readable telemetry or wrapper automation into the next implementation build when there is a direct consumer.

---

## Maxims

> **Predict before observing when prediction is possible.**

> **Record what happened before explaining why it happened.**

> **Repeated evidence from one source is not independent corroboration.**

> **Use bivalence for hard contracts and degrees for genuinely graded relations.**

> **Complexity earns its place by improving explanation or prediction.**

> **A credible pattern can still be a bad design; credibility and value are separate judgments.**

> **A pattern becomes general only when evidence earns generality.**
