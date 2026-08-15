# nightfall!punk — FROM v1.4 TO v1.5 Combat + Communications Ledger

Timestamp: **2026-08-14 22:42 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — Combat & Communications Planning]**
Roles: **PRAXIS / STEWARD / SIGNAL**
Branch: `build/v1.4-relational-combat-communications`
Status: **CURRENT DESIGN HANDOFF / v1.5 RESOLUTION LAYER**
Compile status: **no compile implied by this documentation update**.

## STEWARD provenance rule

Every new public ledger/handoff entry must carry:

1. an exact local timestamp with timezone;
2. a chat-origin field;
3. a **redacted public chat label** rather than a private conversation title, personal name, account name, or other unnecessary identifying information.

When an older/current public record is materially amended, add or preserve this provenance block at the amended record level. Historical immutable records need not be rewritten merely to retrofit metadata; use a later reconciliation/addendum when appropriate.

## FROM — current v1.4 truth

v1.4 is now a real implemented candidate, not merely a planning target.

Current branch:

`build/v1.4-relational-combat-communications`

Automated validated implementation head:

`283d4ff09e3a432242aa69f0f285ebd6105f480e`

Current documentation-stamped branch head at time of this ledger update began:

`0d6032e2fc47e01e3f099d0d9751eae170bc19d3`

Current acceptance state:

- automated v1.4 proof: **GREEN**;
- named workflow: `nightfall v1.4 CI`;
- validated run recorded by the v1.4 human handoff: **#396 / 31856528457 PASS**;
- Pop!_OS human-video acceptance remains a distinct gate and must not be silently equated with automated CI.

v1.4 implemented additions include:

- fixed-capacity `NfReportBus`;
- report scopes: private / Crew / Rival Crew / public;
- report provenance: origin / reporter / ancestry / hops;
- report time: origin / issue / delivery / expiry;
- coarse unseen-position reports rather than exact hidden transforms;
- delayed report acquisition with negative pre-delivery control;
- explicit Player Crew `ALTER_ROUTE` report;
- Human Rival evidence categories DIRECT / AUDIBLE / REPORT;
- removal of exact instant Human Rival squad blackboard truth;
- fixed-capacity `NfClaimTable`;
- soft/hard temporal claims;
- cover-claim migration;
- graded three-height cover exposure;
- stable `ALTER_ROUTE` semantic key/revision;
- dedicated v1.4 relational tests and human-proof executable.

v1.4 governing proposition:

> **Information spreads; cognition does not copy itself. Coordination claims possibilities; it does not own them forever.**

Inherited contracts remain protected:

- server is truth; clients/AI provide intent, not outcome;
- Snap Locomotion / `snap_locomotion` remains the local bounded affordance-capture layer;
- Relational Perceptual Authority Camera;
- bounded actor knowledge / provenance / memory;
- contamination -> functional degradation -> STASIS -> revival without causal rewind;
- topography != topology;
- 8 m x 8 m is the base Southern-California geographic/topographical element, not an authority cell;
- regions / simulation cells / authority cells aggregate base elements.

---

# TO — v1.5 combat + AI communications

v1.5 is the next **combat-and-communications synthesis layer** after v1.4 acceptance/reconciliation.

The resolved 150-question design audit closes as:

- **84 LOCK**;
- **33 ADOPT**;
- **30 INVALIDATE**;
- **3 DEFER**;
- **0 ASK**;
- **150 / 150 design questions dispositioned**.

These are design resolutions, not claims that v1.5 gameplay code already exists.

## Combat influence hierarchy — LOCKED

The hierarchy is weighted, not eclectic:

0. **nightfall!punk story + existing mechanics** — identity / ontology;
1. **Titanfall** — movement-combat coupling, initiative, velocity;
2. **Left 4 Dead** — encounter composition, pressure ecology, cooperation;
3. **Prey (2017)** — systemic environment and multiple tactical solutions;
4. **Quake** — immediacy, spatial/weapon clarity, direct rivalry.

Later references remain subordinate and problem-specific.

External references provide mechanisms and counterexamples. They do not own the final combat ontology.

## Three-level combat grammar — LOCKED

### MICRO — embodied combat

Seconds-scale:

`MOVE -> AIM -> FIRE -> DODGE -> COVER -> RELOAD -> SWITCH -> TAKE/LOSE ANGLE -> ESCAPE`

Primary external lessons: Titanfall + Quake.

### MESO — encounter ecology

Tens of seconds to minutes:

`CREW RIVALRY/NEGOTIATION + TEAMMATE COORDINATION + CATTLER PREDATION + COMMUNICATION + CLAIMS/OCCUPANCY + RESOURCE CONTROL + SYSTEMIC OBJECTS`

Primary external lesson: Left 4 Dead composition/pressure ecology, transformed into a multi-party system.

### MACRO — Lifeworld consequence

Longer-duration causal history:

`CONTAMINATION + STASIS + INVENTORY + TRACE + ROUTE + RESOURCE + TRUST/HOSTILITY + ECOLOGY + EXCHANGE/DEPENDENCY CHANGE`

Primary identity source: nightfall!punk itself, with Prey-like systemic-world lessons.

Canonical recursion:

`MICRO -> MESO -> MACRO -> changes future MICRO`

Combat filter:

> **Move crisply. Know imperfectly. Fight materially. Leave consequences.**

---

# Human Crew relation — LOCKED

Other human Crews are **negotiable actors who may also become Quake-style rivals**.

Possible situated relations include:

- observe / signal;
- negotiate;
- trade;
- cooperate temporarily;
- deter;
- disengage;
- compete;
- contest resources/space directly;
- pursue;
- fight lethally.

Combat does **not** permanently convert a Crew into an ontologically fixed enemy faction.

Crew disposition must not use one universal hostility percentage. Derive it from:

- seed/local history;
- prior interaction;
- immediate threat;
- resource/territorial stakes;
- compact/reciprocity history;
- report confidence/provenance;
- topographical position/cost;
- topological exchange/dependency relation;
- expected future interaction / option value.

Quake-like rivalry is therefore a legitimate local game relation inside a richer repeated/social game.

---

# Dream Cattler predator model — LOCKED / AMENDED

Canonical real-animal reference pair:

> **HYENA ECOLOGY + LEOPARD PREDATION**

This complements the fictional/genre references:

- *Alien: Isolation* Alien — persistent stalking/search pressure after direct evidence is lost, without omniscient exact tracking;
- H. G. Wells's Morlocks — habitat/social ecology and a parallel predatory way of inhabiting infrastructure/world space.

## MICRO — leopard-like predation

- stalking;
- concealment;
- ambush geometry;
- individual initiative;
- sudden commitment;
- disengagement;
- evidence-driven search/reacquisition;
- opportunistic redirection.

## MESO/MACRO — hyena-like ecology

- clan/den/habitat organization;
- territorial recurrence;
- social information;
- opportunistic cooperation and competition;
- predator/resource ecology;
- local pressure that may persist beyond one visible individual.

Ordinary proof should usually foreground **1–2 high-salience embodied hunters** while permitting a larger clan/habitat context at lower fidelity/offscreen representation.

Three or more simultaneous high-fidelity Cattlers should usually communicate **convergence / surge / unusual ecological pressure**, not routine horde logic.

Cattlers remain non-negotiable ecological predators; they do not inherit human Crew diplomacy.

---

# AI communications — v1.5 direction LOCKED

v1.4 proves the communication substrate. v1.5 uses it inside combat ecology.

Canonical chain:

`FIRSTHAND EVIDENCE -> BOUNDED REPORT -> COMPRESSION + PROVENANCE + CONFIDENCE + LATENCY -> OTHER ACTOR BELIEF -> MOVEMENT / COMBAT / NEGOTIATION UTILITY -> ACTION`

Rules:

- firsthand evidence generally outranks relayed report;
- repeated relay from one source is not independent corroboration;
- report ancestry matters;
- exact hidden transforms are not ordinary communicated cognition;
- public material signals such as audible gunfire may become broadly shared evidence where physically perceivable;
- first intentional information loss should be **spatial granularity**: exact observation -> coarse semantic landmark/region + direction;
- bounded latency follows spatial coarsening;
- deliberate Rival misinformation remains later work until truthful-but-incomplete reporting is stable and legible;
- Cattler pack/ecological communication may share evidence primitives but must not simply inherit human social/radio semantics.

Communication topography and topology remain distinct:

- **topography:** range, obstruction, location, noise, physical distribution;
- **topology:** who can relay to whom, channel/dependency structure, broken paths, information connectivity.

---

# First v1.5 combat extension — LOCKED

Do not begin with an arsenal dump.

First additional combat role:

> **one systemic-tool / area-control hybrid**

Its function should test multiple existing relations at once:

- Prey-like environment interaction;
- Quake-like space control;
- Titanfall-compatible movement/initiative;
- occupancy/route pressure;
- material/resource consequence;
- future-option change.

Exact fictional/device form remains an implementation and art-direction choice rather than an architectural fork.

---

# v1.5 proof scenario — LOCKED

Use one compact **Las Flores industrial / transit / service-space cluster** spanning multiple 8 m x 8 m base elements.

Required ingredients:

- contested resource/process node;
- player + teammate/Crew actors;
- another human Crew capable of bargaining or Quake-like rivalry;
- 1–2 foreground Cattlers with larger hyena-like habitat/clan context;
- leopard-like individual Cattler stalking/ambush behavior;
- at least one bounded/delayed/coarsened report that changes another actor's decision;
- at least one systemic object/resource relation that changes viable tactics;
- persistent readable aftermath.

Valid outcomes must be plural, for example:

- negotiate;
- deter;
- compete without annihilation;
- fight the other Crew;
- temporary cooperation under Cattler pressure;
- exploit predator pressure against rivals;
- manipulate the world to escape/control access;
- withdraw to preserve future options.

Acceptance checks causal legibility, not one scripted outcome.

---

# Questions Subroutine — CURRENT

For unresolved future design forks use:

```text
0. ZERO-FAMILIARITY
   What problem are we solving without assuming the solution?

1. PRECEDENT
   What mechanism do the strongest references actually demonstrate?

2. NIGHTFALL STATE
   What does the current build already implement/constrain?
   Which existing owner should solve it?

3. NEGATION / COUNTEREXAMPLE
   Which attractive solution conflicts with stronger architecture,
   story, evidence, game theory, or causal proof?

4. SYNTHESIS
   What is the smallest compatible extension producing measurable improvement?

5. UNCERTAINTY GATE
   Deductively forced or inductively high-confidence/reversible -> recommend automatically.
   Materially different good designs remain -> ask the user.

6. PROOF
   What deterministic positive/negative controls and human-play observation
   distinguish success from failure?
```

Dependency rule:

> **Resolved stronger questions may validate or invalidate downstream uncertainties. Do not ask an already-determined question again.**

---

# PRAXIS — next implementation spine after v1.4 gate

1. Preserve v1.4 report/claim/cover proof and inherited v1.2c/v1.2b regressions.
2. Deepen combat utility/diagnostics before arsenal breadth: directional exposure, graded cover, future-option value, information value, congestion/resource opportunity.
3. Use `NfReportBus` inside combat decisions with coarsened spatial reports and bounded latency.
4. Generalize multiple teammate/Crew actors through the same evidence/communication/combat relation without telepathy.
5. Make human Crew rivalry scoped/negotiable while permitting direct Quake-like contests when incentives justify them.
6. Deepen Cattler persistent search: evidence loss -> bounded leopard-like search/stalk + hyena-like habitat/clan pressure -> reacquire / redirect / withdraw.
7. Add one systemic object/resource focal point that changes route, exposure, resource/communication topology and negotiation incentives.
8. Add only the minimum systemic-tool/area-control combat role needed to prove that relation.
9. Record macro aftermath through existing persistence/energy/history owners rather than a parallel combat-history system.
10. Prove MICRO + MESO + MACRO in one plural-outcome Las Flores scenario.

Implementation law remains:

> **ExistingSystem -> CompatibleExtension -> MeasuredImprovement.**

---

# STEWARD status

- v1.4 implementation: **automated-green candidate; human-video gate tracked separately**.
- v1.5 design-resolution layer: **150/150 closed**.
- v1.5 gameplay implementation: **NOT YET CLAIMED**.
- public provenance rule: **timestamp + redacted chat label required on new ledger/handoff records**.
- no compile is implied by this ledger update.

Precedence for combat/communications planning:

`v1.4 IMPLEMENTED/PROVEN RECORDS + THIS v1.4->v1.5 LEDGER > EARLIER v1.5 LOCAL PLANNING NOTES > HISTORICAL COMBAT TERMINOLOGY`
