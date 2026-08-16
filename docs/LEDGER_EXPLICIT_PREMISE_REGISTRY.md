# nightfall!punk — Explicit Premise Registry

Timestamp: **2026-08-16 15:55 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — Deductive Premise Registry]**
Roles: **PRAXIS / SIGNAL / STEWARD**
Record specificity: **PROJECT-WIDE METHODOLOGY / CROSS-SUBSYSTEM PREMISE INVENTORY**
Status: **PROTOCOL 1 — EXPLICIT PREMISE REGISTRY / NO COMPILE IMPLIED**

## Purpose

This is the first protocol in the project evidence/reasoning upgrade.

It inventories **already accepted explicit premises** that constrain deduction. It does not yet add an inference engine, confidence updater, observation database, Bayesian weighting, contradiction engine, or experiment registry.

A premise is included here only when an accepted record already states the relation strongly enough to constrain downstream reasoning.

The following are **not automatically premises**:

- aesthetic preferences;
- literary or game references by themselves;
- one playtest observation;
- one benchmark result;
- implementation constants that have not been promoted beyond their local scope;
- aspirations;
- attractive analogies.

Those may later support induction or interpretation, but they do not become project laws merely by being useful.

---

# A. Project / implementation premises

### P-ARCH-01 — Existing-system extension

**Statement:** Prefer extension of an accepted owner over creation of a parallel authority.

`ExistingSystem -> CompatibleExtension -> MeasuredImprovement`

**Scope:** PROJECT-WIDE / IMPLEMENTATION METHOD

**Status:** LOCKED DESIGN/IMPLEMENTATION LAW

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`

### P-ARCH-02 — Bounded hot paths

**Statement:** Fixed-capacity hot-path state and bounded queries are preferred over per-tick allocation or whole-world high-resolution solving.

**Scope:** PROJECT-WIDE / ENGINE + AI EFFICIENCY

**Status:** DURABLE ARCHITECTURE CONTRACT

**Primary source:** `docs/ARCHITECTURE.md`

### P-ARCH-03 — Presentation observes simulation

**Statement:** Presentation may represent, smooth, frame, or mediate simulation truth; it does not own gameplay state.

**Scope:** PROJECT-WIDE / PRESENTATION AUTHORITY

**Status:** DURABLE ARCHITECTURE CONTRACT

**Primary source:** `docs/ARCHITECTURE.md`, `docs/LEDGER_V1.2B_CAMERA_REBUILD.md`

---

# B. Authority and control premises

### P-AUTH-01 — Server truth

**Statement:** Authoritative world/material state and combat outcomes belong to simulation/server truth.

**Scope:** PROJECT-WIDE / NETWORK + SIMULATION AUTHORITY

**Status:** IMPLEMENTED / REGRESSION-PROTECTED

**Primary source:** `docs/ARCHITECTURE.md`

### P-AUTH-02 — Intent is not outcome

**Statement:** Clients and AI provide intent; authoritative systems resolve movement, combat, damage, death, ammunition, and world consequences.

**Scope:** PROJECT-WIDE / CONTROL + COMBAT AUTHORITY

**Status:** IMPLEMENTED / REGRESSION-PROTECTED

**Primary source:** `docs/ARCHITECTURE.md`

### P-AUTH-03 — Shared control vocabulary

**Statement:** Network players and server AI families converge on ordinary control/input semantics rather than privileged AI outcome paths.

**Scope:** PROJECT-WIDE / ACTOR CONTROL

**Status:** IMPLEMENTED

**Primary source:** `docs/ARCHITECTURE.md`

### P-AUTH-04 — AI is participant, not administrator

**Statement:** AI may perceive, value, plan locally, communicate, and emit control intent; it does not administer world truth or bypass normal movement/combat resolution.

**Scope:** PROJECT-WIDE / AI AUTHORITY

**Status:** DURABLE ARCHITECTURE PREMISE

**Primary source:** `docs/ARCHITECTURE.md`

---

# C. Epistemic premises

### P-EPI-01 — Reality is not automatic knowledge

**Statement:** Server truth does not automatically become actor knowledge.

Human form:

`server truth != individual knowledge != group report`

Cattler form:

`server truth != individual Cattler knowledge != pack report`

**Scope:** PROJECT-WIDE / ACTOR EPISTEMOLOGY

**Status:** IMPLEMENTED / LOCKED

**Primary source:** `docs/ARCHITECTURE.md`

### P-EPI-02 — Legitimate evidence precedes belief revision

**Statement:** Actor belief may change only through legitimate perception, semantic evidence, memory, or bounded communication available to that actor.

**Scope:** PROJECT-WIDE / AI EPISTEMOLOGY

**Status:** IMPLEMENTED + DESIGN INVARIANT

**Primary source:** `docs/ARCHITECTURE.md`, `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`

### P-EPI-03 — Information spreads; cognition does not copy itself

**Statement:** A report can change another actor's evidence/belief without transferring the originator's complete cognition or exact hidden world state.

**Scope:** VERSION v1.4+ / SUBSYSTEM AI COMMUNICATIONS

**Status:** IMPLEMENTED v1.4 / GOVERNING PROPOSITION

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-EPI-04 — Provenance matters

**Statement:** Reports carry source/provenance/ancestry, and repeated relay from one origin is not independent corroboration.

**Scope:** VERSION v1.4+ / SUBSYSTEM AI COMMUNICATIONS

**Status:** IMPLEMENTED v1.4 + LOCKED v1.5 DIRECTION

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-EPI-05 — Bounded rationality

**Statement:** Actor reasoning remains local, evidence-limited, multi-rate, and bounded even when authoritative world truth is larger.

**Scope:** PROJECT-WIDE / AI COGNITION

**Status:** LOCKED INTERPRETIVE/ARCHITECTURAL PREMISE

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`, `docs/ARCHITECTURE.md`

### P-EPI-06 — Common reality does not imply common utility

**Statement:** One material event may rationally generate different actions because shared reality does not imply common knowledge, topographical cost, topological opportunity, capability, history, or utility.

**Scope:** PROJECT-WIDE / GAME-THEORETIC ACTOR MODEL

**Status:** LOCKED PROOF PROPOSITION

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`

---

# D. Movement and camera premises

### P-MOVE-01 — Snap Locomotion is local affordance capture

**Statement:** `snap_locomotion` converts high-authority movement intention plus legitimate local affordances into bounded trajectory guidance before authoritative physical resolution; it does not own global strategy and does not put the actor on rails.

**Scope:** PROJECT-WIDE / SUBSYSTEM SNAP LOCOMOTION

**Status:** IMPLEMENTED / CANONICAL TERMINOLOGY

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`, `docs/LEDGER_V1.2B_CAMERA_REBUILD.md`

### P-MOVE-02 — Impossibility may veto; preference normally biases

**Statement:** Spatial/utility fields normally bias choices rather than veto them; true physical/logical impossibility may exclude an action.

**Scope:** PROJECT-WIDE / MOVEMENT + AFFORDANCE REASONING

**Status:** DURABLE ARCHITECTURE PREMISE

**Primary source:** `docs/ARCHITECTURE.md`

### P-CAM-01 — Relational Perceptual Authority Camera

**Statement:** Aim belongs to player perceptual intent; camera position acknowledges embodied reality; presentation mediates between them.

**Scope:** PROJECT-WIDE / SUBSYSTEM CAMERA

**Status:** IMPLEMENTED v1.2b / REGRESSION-PROTECTED

**Primary source:** `docs/LEDGER_V1.2B_CAMERA_REBUILD.md`

### P-CAM-02 — Relocation is discontinuity, not fake traversal

**Statement:** Authoritative relocation must be presented as causal discontinuity rather than fabricated traversal interpolation.

**Scope:** PROJECT-WIDE / CAMERA PRESENTATION

**Status:** LOCKED CAMERA POLICY

**Primary source:** `docs/LEDGER_V1.2B_CAMERA_REBUILD.md`

---

# E. Topography, topology, value, and world premises

### P-WORLD-01 — Topography and topology are distinct

**Statement:** Topography concerns geographic/metric distribution and acquisition cost; topology concerns connectivity, exchange, conversion, dependency, substitution, cycles, and bottlenecks.

**Scope:** PROJECT-WIDE / WORLD + ECONOMY + COMMUNICATION REASONING

**Status:** LOCKED TERMINOLOGY / DESIGN PREMISE

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`

### P-WORLD-02 — Situated value is relational

**Statement:** Realized situated value depends on accounted stock/flow, topographical acquisition cost, topological exchange/conversion structure, and actor-situated information/capability/history.

**Scope:** PROJECT-WIDE / ENERGY + UTILITY REASONING

**Status:** LOCKED DESIGN PREMISE

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`

### P-WORLD-03 — 8 m x 8 m is geographic, not authority

**Statement:** The 8 m x 8 m element is a base Southern-California geographic/topographical element, not automatically a network authority cell, simulation region, seed world, or shard.

**Scope:** PROJECT-WIDE / WORLD SPATIAL MODEL

**Status:** LOCKED

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`

### P-WORLD-04 — Larger partitions are engineering parameters

**Statement:** Region, simulation-cell, authority-cell, and larger seed dimensions are aggregates chosen from evidence rather than ontological constants.

**Scope:** PROJECT-WIDE / DISTRIBUTED WORLD ARCHITECTURE

**Status:** LOCKED DESIGN POLICY

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`, `docs/ARCHITECTURE.md`

### P-WORLD-05 — Persistent world = generated baseline + causal delta

**Statement:** Meaningful historical departures persist; reducing simulation fidelity may compress causal state but must not erase or reroll history.

`CURRENT_WORLD = GENERATE(SEED) + PERSISTENT_CAUSAL_DELTA`

**Scope:** PROJECT-WIDE / LONG-HORIZON WORLD PERSISTENCE

**Status:** DESIGN COMMITMENT

**Primary source:** `docs/ARCHITECTURE.md`

### P-WORLD-06 — Large world does not require large local thought

**Statement:** Large-scale world/population goals must preserve bounded local simulation and bounded per-agent cognition.

**Scope:** PROJECT-WIDE / SCALE + PERFORMANCE

**Status:** DESIGN COMMITMENT; MMO-SCALE IMPLEMENTATION REMAINS ASPIRATIONAL

**Primary source:** `docs/ARCHITECTURE.md`

---

# F. Actor/social premises

### P-SOC-01 — Faction identity != relationship state

**Statement:** Faction/actor identity is distinct from current relationship state.

**Scope:** PROJECT-WIDE / SOCIAL + COMBAT RELATIONS

**Status:** IMPLEMENTED / LOCKED

**Primary source:** `docs/ARCHITECTURE.md`

### P-SOC-02 — Human Crews are negotiable and may become direct rivals

**Statement:** Other human Crews may negotiate, trade, deter, cooperate temporarily, disengage, compete, pursue, or fight lethally; entering combat does not permanently convert a Crew into an ontologically fixed enemy faction.

**Scope:** VERSION v1.5 DESIGN / HUMAN CREW RELATIONS

**Status:** 150/150 CLOSED DESIGN RESOLUTION; v1.5 GAMEPLAY IMPLEMENTATION NOT YET CLAIMED

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-SOC-03 — Crew disposition is situated, not one hostility percentage

**Statement:** Human Crew relation should derive from evidence, history, stakes, reciprocity, resource/topographical/topological conditions, and future options rather than one universal hostility probability.

**Scope:** VERSION v1.5 DESIGN / HUMAN CREW UTILITY

**Status:** LOCKED DESIGN RESOLUTION

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-SOC-04 — Commands are coordination bids, not possession

**Statement:** Player commands/pings may strongly alter teammate salience/coordination but do not universally possess teammate agency; physical impossibility and world truth remain overriding constraints.

**Scope:** PROJECT-WIDE / TEAMMATE COORDINATION

**Status:** ACCEPTED DESIGN POLICY

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`

---

# G. Dream Cattler premises

### P-CAT-01 — Cattlers are a distinct ecology, not reskinned Human Rivals

**Statement:** Human Rivals and Dream Cattlers may share truth/control primitives while retaining distinct motivation, habitat, social-information, and predator policies.

**Scope:** PROJECT-WIDE / DREAM CATTLER AI FAMILY

**Status:** IMPLEMENTED FAMILY DISTINCTION + LOCKED DESIGN

**Primary source:** `docs/ARCHITECTURE.md`

### P-CAT-02 — Cattlers are non-negotiable ecological predators

**Statement:** Human Crew diplomacy/compact grammar does not ordinarily apply to Dream Cattlers.

**Scope:** PROJECT-WIDE / DREAM CATTLER SOCIAL ECOLOGY

**Status:** LOCKED CURRENT DESIGN

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-CAT-03 — Hyena ecology + leopard predation

**Statement:** Mature Cattler design uses leopard-like micro predation (stalk, conceal, ambush, commitment, disengagement, search) and hyena-like meso/macro ecology (clan/den/habitat recurrence, territorial/resource pressure, social information, opportunistic cooperation/competition).

**Scope:** VERSION v1.5 DESIGN / DREAM CATTLER ECOLOGY

**Status:** LOCKED / AMENDED DESIGN RESOLUTION

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-CAT-04 — Persistent search without omniscient tracking

**Statement:** After legitimate evidence is lost, Cattlers may search, stalk, redirect, withdraw, lurk, or reacquire through bounded evidence; they do not receive exact hidden target tracking.

**Scope:** PROJECT-WIDE / DREAM CATTLER PREDATOR EPISTEMOLOGY

**Status:** LOCKED DESIGN; PARTLY BUILT THROUGH EXISTING EPISTEMIC CONTRACT

**Primary source:** `docs/ARCHITECTURE.md`, `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

---

# H. Combat and consequence premises

### P-COMBAT-01 — Combat authority remains centralized

**Statement:** Successful damage outcomes remain authored by the authoritative combat/world path; v1.5 breadth must not create a second combat authority.

**Scope:** PROJECT-WIDE / COMBAT AUTHORITY

**Status:** IMPLEMENTED / REGRESSION-PROTECTED

**Primary source:** `docs/ARCHITECTURE.md`, `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-COMBAT-02 — MICRO / MESO / MACRO are one continuous system

**Statement:** Embodied combat, encounter ecology, and Lifeworld aftermath are analytical timescales over one continuous simulation, not separate combat modes.

`MICRO -> MESO -> MACRO -> changes future MICRO`

**Scope:** VERSION v1.5 DESIGN / COMBAT SYNTHESIS

**Status:** LOCKED DESIGN RESOLUTION

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-COMBAT-03 — Combat must leave material consequence

**Statement:** Combat can alter bodies, contamination/STASIS, inventory/resources, routes, traces, relationships, ecology, exchange/dependency structures, and therefore future strategy sets; post-combat calm is not equivalent to world reset.

**Scope:** VERSION v1.5 DESIGN / COMBAT AFTERMATH

**Status:** LOCKED DESIGN RESOLUTION; PARTLY SUPPORTED BY EXISTING PERSISTENCE/CONTAMINATION SYSTEMS

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-COMBAT-04 — First breadth extension is systemic, not arsenal quantity

**Statement:** The first v1.5 additional combat role is one systemic-tool / area-control hybrid before broad arsenal expansion.

**Scope:** VERSION v1.5 / FEATURE COMBAT EXTENSION

**Status:** LOCKED DESIGN RESOLUTION; NOT YET IMPLEMENTATION CLAIM

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

---

# I. Communications premises

### P-COMM-01 — Report chain

**Statement:** Combat communication follows the causal chain:

`FIRSTHAND EVIDENCE -> BOUNDED REPORT -> COMPRESSION + PROVENANCE + CONFIDENCE + LATENCY -> OTHER ACTOR BELIEF -> UTILITY -> ACTION`

**Scope:** VERSION v1.4+ / SUBSYSTEM AI COMMUNICATIONS

**Status:** v1.4 SUBSTRATE IMPLEMENTED / v1.5 COMBAT INTEGRATION LOCKED

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-COMM-02 — Spatial coarsening precedes richer uncertainty

**Statement:** The first deliberate loss in ordinary human report fidelity is spatial granularity—exact firsthand observation becomes coarse semantic landmark/region + direction—followed by bounded latency.

**Scope:** VERSION v1.5 / FEATURE REPORT FIDELITY

**Status:** LOCKED DESIGN DIRECTION

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-COMM-03 — Communication has both topography and topology

**Statement:** Physical range/obstruction/noise/distribution are communication topography; relay/channel/dependency/connectivity structure is communication topology.

**Scope:** VERSION v1.5 / SUBSYSTEM AI COMMUNICATIONS

**Status:** LOCKED DESIGN PREMISE

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

---

# J. Reference and interpretation premises

### P-REF-01 — References supply mechanisms, not ontology ownership

**Statement:** External games, literature, science, philosophy, and cultural references may supply mechanisms, constraints, analogies, or counterexamples; nightfall!punk story + accepted mechanics remain the identity/ontology owner.

**Scope:** PROJECT-WIDE / DESIGN REFERENCE METHOD

**Status:** LOCKED v1.5 HIERARCHY / CONSISTENT WITH QUESTIONS SUBROUTINE

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-REF-02 — Literary/theoretical ideas are relational constraints

**Statement:** Polyphony, focalization, unreliable narration, chronotope, individuation, niche construction, repeated/congestion games, and bounded rationality are used to clarify relations and constraints rather than being implemented as feature names.

**Scope:** PROJECT-WIDE / SIGNAL INTERPRETIVE METHOD

**Status:** ACCEPTED METHOD

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`

---

# K. Questions / deduction premises

### P-Q-01 — Stronger resolved premises constrain downstream questions

**Statement:** A deductively forced or inductively very-high-confidence/reversible answer should be recommended automatically; genuinely different good designs remain ASK questions.

**Scope:** PROJECT-WIDE / QUESTIONS SUBROUTINE

**Status:** CURRENT METHOD

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-Q-02 — Do not re-ask a forced question

**Statement:** Resolved upstream questions may validate or invalidate downstream uncertainties; do not reopen an already-determined fork without materially new contradictory evidence.

**Scope:** PROJECT-WIDE / QUESTIONS SUBROUTINE + STEWARD

**Status:** CURRENT METHOD

**Primary source:** `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md`

### P-Q-03 — Proof needs positive and negative epistemic controls

**Statement:** Important causal/epistemic claims should be distinguishable by tests that show both when the behavior should occur and when it must not occur.

**Scope:** PROJECT-WIDE / TEST METHODOLOGY

**Status:** ACCEPTED TEST PRACTICE

**Primary source:** `docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md`, v1.4 implemented report negative-control work

---

# L. Recordkeeping / evidence premises

### P-REC-01 — Public records carry provenance

**Statement:** New public project records require timestamp, timezone, redacted chat provenance, role attribution where relevant, specificity, and evidence/implementation status.

**Scope:** PROJECT-WIDE / PUBLIC RECORDKEEPING

**Status:** CURRENT POLICY

**Primary source:** `docs/LEDGER_PROVENANCE_POLICY.md`

### P-REC-02 — Authority is scope-bounded

**Statement:** A record is authoritative only within the specificity it actually earned.

**Scope:** PROJECT-WIDE / RECORD INTERPRETATION

**Status:** CURRENT POLICY

**Primary source:** `docs/LEDGER_PROVENANCE_POLICY.md`

### P-REC-03 — Narrow evidence does not silently generalize

**Statement:** Observation/sample -> repeated evidence/comparison -> test/proof -> feature/subsystem confidence -> version policy -> project-wide rule only through explicit reconciliation.

**Scope:** PROJECT-WIDE / EVIDENCE PROMOTION

**Status:** CURRENT POLICY

**Primary source:** `docs/LEDGER_PROVENANCE_POLICY.md`, `docs/LEDGER_RECORD_SPECIFICITY_INDEX.md`

### P-REC-04 — Design, implementation, automated proof, and human proof are distinct

**Statement:** Do not infer implementation from design resolution, human acceptance from CI, or project-wide truth from a successful local proof.

**Scope:** PROJECT-WIDE / EVIDENCE STATUS

**Status:** CURRENT POLICY

**Primary source:** `docs/LEDGER_PROVENANCE_POLICY.md`

---

# Current registry summary

This first pass registers **47 deduction-active premises** across:

- architecture/method;
- server/control authority;
- epistemology;
- movement/camera;
- topography/topology/world persistence;
- social relations;
- Dream Cattlers;
- combat/consequence;
- communications;
- reference interpretation;
- Questions Subroutine;
- recordkeeping/evidence scope.

## Important STEWARD boundary

This registry does **not** claim that all 47 premises have equal specificity or evidence status.

A project-wide implemented invariant such as `P-AUTH-01` has different authority from a v1.5 locked-but-not-yet-implemented design premise such as `P-COMBAT-04`.

Deduction must therefore use both:

`PREMISE CONTENT + PREMISE SPECIFICITY + PREMISE STATUS + PRECEDENCE`

not premise text alone.

## Next protocol boundary

Do not yet add automatic derived conclusions to this registry.

The next step, when explicitly taken, should be to define **premise dependency / support / conflict edges** so deductions can state exactly which accepted premises entail or invalidate a downstream conclusion.

## Maxim

> **Deduction is only as explicit as the premises it can name.**
