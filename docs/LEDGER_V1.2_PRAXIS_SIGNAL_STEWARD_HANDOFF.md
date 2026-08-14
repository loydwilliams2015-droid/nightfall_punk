# nightfall!punk — PRAXIS / SIGNAL / STEWARD Handoff

Date: 2026-08-13
Branch: `build/v1.2-teammate-causality`
Status: **living handoff / design-and-proof reconciliation; no gameplay code added by this document.**

## Governing handoff rule

`FROM -> COMPARE/CONTRAST -> PRAXIS -> SIGNAL -> STEWARD -> TO`

The handoff must preserve epistemic status. A useful text-model comparison can motivate a coding question, but it does not become an implementation claim merely because its explanation is coherent.

> **Analogy != implementation. Text harness != executable proof. Current build is allowed to correct the theory.**

---

# FROM — v1.2 Teammate Causality / Kinetic Stigmergy Alpha

Current active PR: **#18 — v1.2 Teammate Causality / Kinetic Stigmergy Alpha**.

Pre-handoff code/document head inspected: `e65e19e931794fbe2f7e7f359b982df6cc10d4de`.

GitHub Actions `nightfall v1.2 CI` run **#322** on that head: **PASS**.

The current implemented causal chain is:

`PLAYER WORLD-WRITE -> LEGITIMATE EVIDENCE -> BELIEF/MEMORY -> GRADIENT/HOTSPOT -> ROUTE UTILITY -> COMMITMENT -> ACTION/REROUTE`

Negative control remains:

`WORLD CHANGE + NO LEGITIMATE EVIDENCE -> NO ARTIFICIAL KNOWLEDGE OR REACTION`

Current v1.2 implementation already proves, in bounded form:

- one authoritative teammate body;
- biography-derived strategic tendencies;
- `ALTER_ROUTE` as material collider truth;
- direct / Crew-report / encounter evidence;
- evidence confidence and bounded memory;
- route energy + priority-hotspot gradient;
- commitment hysteresis;
- authoritative material override of remembered possibility;
- projected cooperative occupancy/yielding;
- reuse of existing local-goal movement rather than duplicate locomotion/pathfinding;
- explicit causal diagnostics.

Current v1.2 does **not** yet claim:

- production dedicated-server teammate integration;
- replicated `ALTER_ROUTE` in the ordinary multiplayer transport path;
- a general plural-logic reasoning engine;
- full contradictory-evidence handling;
- generalized dynamic navmesh/affordance graph mutation;
- multiple full peer teammates;
- thousands-player seeded-world orchestration.

---

# COMPARE / CONTRAST — Pac-Man text-game probe

A deterministic text-only comparison was run conceptually against the current v1.2 source contracts. It compared a classic Pac-Man-style perfect-state chase/flee baseline with a nightfall-style situated-agent abstraction.

## Epistemic status

**ANALYTIC / INTERPRETIVE TEST.**

This was not a compiled C executable, CI test, benchmark, or proof that the current binary literally contains a Pac-Man mode. Its value is comparative: it exposes which architectural distinctions survive when both systems are compressed into the same simple maze problem.

## Comparative finding

Classic baseline:

`GLOBAL PLAYER STATE -> CHASE/FLEE POLICY`

nightfall abstraction:

`WORLD -> EVIDENCE -> BELIEF -> MEMORY -> RELATIONAL VALUE -> STRATEGY -> ACTION -> WORLD CHANGE`

The strongest v1.2 delta is not merely a more complicated utility function. It is **epistemic and causal situatedness**:

- world truth need not become actor knowledge;
- direct evidence, reports and encounter evidence have different standing;
- memory persists with confidence/freshness/consequence;
- material impossibility overrides remembered possibility;
- occupancy with another cooperative actor may alter immediate action;
- attack/pursuit is one strategy among several rather than the sole ontology of the agent.

## Protect this distinction

> **Do not turn nightfall actors into more sophisticated Pac-Man ghosts.**

The engine distinction is not `better chase AI`; it is actors inhabiting different material, epistemic, social and energetic situations while sharing authoritative world rules.

---

# PRAXIS — implementation consequences

## P1 — preserve the causal spine

Protect:

`WORLD FACT -> LEGITIMATE EVIDENCE -> ACTOR JUDGMENT -> STRATEGY -> CONTROL -> CONSEQUENCE`

No new reasoning layer may bypass authoritative material truth, ordinary movement/combat authority, or the distinction between truth and actor belief.

## P2 — production integration before abstraction explosion

The first next implementation priority should be to integrate the bounded teammate causal chain into the **production dedicated-server tick and ordinary networking path** before creating a large new cognitive framework.

Reuse:

- `NfControlFrame`;
- existing movement/Fuzzy Rail;
- existing region/spatial reasoning;
- existing energy belief/topographic cost;
- existing semantic evidence/report infrastructure where compatible;
- authoritative world/material state.

## P3 — plural reasoning by domain, not one master calculus

Adopt the following design rule prospectively:

- **classical/deductive logic** for hard invariants and material impossibility;
- **many-valued/fuzzy judgment** for genuinely graded predicates such as exposure, danger, defensibility, trustworthiness or habitat fit;
- **paraconsistent evidence handling** where legitimate sources can support incompatible claims without arbitrary overwrite or logical explosion;
- **probability/statistics** for uncertainty about events and recurrence;
- **game-theoretic utility/strategy** for situated choice among viable actions.

Do not collapse these into one scalar and call it truth.

Canonical distinction:

`FUZZY APPLICABILITY != EVENT PROBABILITY != EPISTEMIC CONFIDENCE != EVIDENCE CONTRADICTION != UTILITY`

## P4 — smallest contradictory-evidence proof

Before general paraconsistent machinery, prove one bounded case.

Example:

1. Crew report says `ROUTE_B OPEN`.
2. Actor stores support for the report with source/confidence/age.
3. Later direct material encounter shows `ROUTE_B CLOSED`.
4. The actor may retain historical evidence that the route had been reported/open, but **current material legality forbids traversal**.
5. Contradiction changes investigation/trust/uncertainty; it does not manufacture a route or force an arbitrary global conclusion.

Possible compact evidence shape, **DESIGN SKETCH ONLY**:

```c
typedef struct NfClaimEvidence {
    float support_for;
    float support_against;
    float confidence;
    uint64_t newest_tick;
} NfClaimEvidence;
```

Do not implement this exact struct unless it reduces duplication with existing memory/report primitives.

## P5 — Simondonian category discipline

Store causal relations first; derive categories second.

Do not make labels such as `ALLY`, `DANGEROUS`, `INFESTED`, `SAFE`, `TRUSTED` or future regime descriptors into magic causes when the underlying relations can remain authoritative.

> **The category follows individuation; it does not replace it.**

---

# SIGNAL — interpretive / communication contract

## S1 — engine identity

The useful public/internal contrast is:

> **Situated agents, not omniscient chase functions.**

Do not market the AI distinction as mystical consciousness or universal intelligence. The concrete distinction is architectural:

- bounded perception;
- evidence provenance;
- memory and confidence;
- heterogeneous goals/strategies;
- shared material authority;
- world-changing consequence;
- actor-relative interpretation.

## S2 — post-Apollonian formulation

The relevant philosophical synthesis is:

`FORM + METASTABILITY + LOGICAL PLURALITY + REVISABLE DISTINCTION`

Working maxim:

> **Order capable of becoming otherwise without ceasing to be order.**

Simondon supplies the ontology of individuation/metastability; plural/non-classical logics supply disciplined ways of reasoning when strict bivalence is not the right representation.

## S3 — statistics / post-scarcity continuity

Preserve the distinction:

`LOGICAL ELIMINATION != STRUCTURAL SUPPRESSION != STATISTICAL RARITY != EMPIRICAL ABSENCE`

Post-scarcity therefore does not imply post-danger, post-conflict or post-history. Successful organization may lower frequency/severity of calamity while leaving recurrence structurally possible.

## S4 — AI brand relation

This technical philosophy supports the larger brand core:

> **AI amplification, not AI substitution.**

AI-assisted development should help compare, falsify, maintain memory, expose contradictions and connect research to implementation under human creative/technical authority.

---

# STEWARD — ledger / status discipline

## ST1 — protect current truth boundary

Keep these distinct:

- **IMPLEMENTED:** current v1.2 source/CI-backed causal teammate slice.
- **OBSERVED:** human video evidence after it is actually recorded/reviewed.
- **ANALYTIC TEST:** Pac-Man text compare/contrast probe.
- **DESIGN COMMITMENT:** plural reasoning by appropriate domain; causal-category discipline.
- **ASPIRATION:** broader lifeworld AI in which many actor families use situated plural reasoning coherently.
- **OPEN:** exact representations for contradiction, fuzzy predicates, probability and multi-agent strategic integration.

## ST2 — historical archives

Do not rewrite accepted historical archive branches to retrofit this terminology. Apply it prospectively to living ledgers and future contracts.

## ST3 — exact-head CI rule

The inspected pre-handoff head `e65e19e...` has successful CI run #322. This documentation handoff creates a newer branch head; therefore **do not describe the newer exact head as CI-green until its workflow succeeds.**

No gameplay compile was requested by this handoff.

---

# TO — next bounded development contract

Working candidate only; **do not treat as accepted v1.3 until the v1.2 human gate closes and the next contract is explicitly frozen.**

Provisional thesis:

> **Production-Integrated Situated Reasoning / Plural Evidence Alpha**

Proposed smallest proof sequence:

1. integrate one teammate causal loop into the production authoritative server tick;
2. route its actions through ordinary network-visible world consequences rather than a parallel demo-only truth path;
3. preserve `world truth != actor knowledge != group report`;
4. add one bounded contradictory-evidence test;
5. distinguish current material legality from remembered/reported possibility;
6. add at most one genuinely graded predicate where fuzzy/many-valued representation demonstrably improves behavior;
7. keep probability/confidence separate from fuzzy applicability;
8. retain game-theoretic choice only after deductive legality and situated evidence constrain the candidate action set;
9. reuse existing movement, energy, spatial, semantic and memory machinery;
10. run compare/contrast again against the simple chase/flee baseline only after executable behavior changes.

## Candidate automated proof

`REPORT(A) + LATER_DIRECT_NOT(A) -> CONTRADICTION_RETAINED -> MATERIAL_NOT(A) BLOCKS ACTION -> BELIEF/CONFIDENCE/TRUST MAY REVISE -> NO EXPLOSION / NO MAGIC KNOWLEDGE`

## Candidate human proof

One short recording should make this legible without a giant diagnostic HUD:

`actor receives/encounters evidence -> acts on a reasonable belief -> contradictory world evidence appears -> actor revises strategy -> authoritative geometry/material state remains decisive`

## Explicit non-goals

- no universal theorem prover;
- no full symbolic ontology;
- no giant GOAP rewrite;
- no one master truth/utility scalar;
- no arbitrary fuzzy numbers for crisp material facts;
- no contradiction-as-randomness gimmick;
- no claim of human-like consciousness;
- no MMO-scale distributed implementation merely because the architecture remains future-compatible.

---

# Handoff maxims

> **Deduction constrains the possible; induction estimates the probable; game theory chooses among situated strategies; action changes the field.**

> **World probability != observed frequency != actor credence.**

> **Meaning != probability != confidence != contradiction.**

> **The category follows individuation.**

> **Do not make a smarter ghost when the design calls for a situated participant.**
