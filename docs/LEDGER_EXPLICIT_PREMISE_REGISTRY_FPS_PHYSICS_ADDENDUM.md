# nightfall!punk — Explicit Premise Registry: FPS / Environmental Physics Addendum

Timestamp: **2026-08-16 16:14 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — FPS Environmental Physics Premises]**
Roles: **PRAXIS / SIGNAL / STEWARD**
Record specificity: **PROJECT-WIDE METHODOLOGY / SUBSYSTEM ENVIRONMENTAL PHYSICS + FPS CAUSALITY**
Status: **PROTOCOL 1 ADDENDUM — 12 PREMISES ADOPTED / NO COMPILE IMPLIED**

## Adoption disposition

All twelve candidate FPS/environmental-physics premises are **ADOPTED**.

This addendum remains part of **Protocol 1 — Explicit Premise Registry**. It does not begin Protocol 2 and does not yet create support/conflict/dependency edges.

The original registry contains 47 deduction-active premises. This accepted addendum raises the current Protocol-1 total to:

> **59 deduction-active premises = 47 existing + 12 FPS/environmental-physics premises.**

Status discipline matters: adoption establishes deductive authority at the stated scope; it does **not** falsely claim that every premise is already fully implemented in code.

---

# M. FPS / environmental-physics premises

### P-PHYS-01 — Matter resists intention

**Statement:** Actor intention proposes action; authoritative material constraints determine whether that action can physically occur. Intent does not override impossible geometry.

Canonical relation:

`INTENT -> PROPOSED MOTION/ACTION -> MATERIAL CONSTRAINT TEST -> LEGITIMATE PHYSICAL RESULT`

**Scope:** PROJECT-WIDE / ENVIRONMENTAL PHYSICS + MOVEMENT

**Status:** ADOPTED / SUBSTANTIALLY IMPLEMENTED BY CURRENT MOVEMENT-COLLISION PATH

**Existing support:** current body-overlap checks, horizontal/vertical collision resolution, Snap Locomotion premise, and `P-MOVE-02`.

---

### P-PHYS-02 — Authoritative geometry determines occupiable space

**Statement:** Whether an embodied actor can occupy a location is determined by authoritative world geometry plus the actor's physical dimensions, not by presentation, AI desire, or semantic labeling alone.

**Scope:** PROJECT-WIDE / COLLISION + EMBODIMENT

**Status:** ADOPTED / IMPLEMENTED FOR CURRENT COLLIDER MODEL

**Existing support:** `nf_movement_space_is_free`, collider/body-overlap resolution, `P-AUTH-01`, `P-ARCH-03`.

---

### P-PHYS-03 — Support and gravity govern unsupported embodiment

**Statement:** Valid support surfaces may carry/ground actors; when legitimate support is absent, embodied actors respond to the world's gravity/airborne rules rather than remaining suspended by intention or presentation.

**Scope:** PROJECT-WIDE / MOVEMENT PHYSICS

**Status:** ADOPTED / IMPLEMENTED IN CURRENT MOVEMENT MODEL

**Existing support:** grounded/air states, support-height queries, gravity, jump/landing logic, moving-platform support.

---

### P-PHYS-04 — Traversal affordances are actor-relative relations to geometry

**Statement:** STEP, VAULT, MANTLE, LADDER, jump, climb-like, and future traversal possibilities exist as relations among world geometry, actor morphology/capability, relative position, and current intention; the object alone does not contain a universal affordance for every actor.

Canonical relation:

`OBJECT/GEOMETRY + ACTOR MORPHOLOGY/CAPABILITY + RELATIVE POSITION + INTENTION -> CURRENT AFFORDANCE`

**Scope:** PROJECT-WIDE / MOVEMENT + AFFORDANCE REASONING

**Status:** ADOPTED / HUMAN-SCALE CORE IMPLEMENTED; MORPHOLOGY-DIFFERENTIATED GENERALIZATION REMAINS FUTURE WORK

**Existing support:** current traversal candidates, Snap Locomotion, Cattler morphology distinction.

---

### P-PHYS-05 — Geometry materially determines occlusion and cover

**Statement:** Semantic cover reasoning may help actors identify useful positions, but authoritative geometry and attack trajectory determine whether matter actually occludes movement, perception, or fire. Cover is not made true merely because an AI node or label says `COVER`.

Canonical relation:

`COVER EFFECT = GEOMETRY + BODY MORPHOLOGY + ATTACK/PERCEPTION TRAJECTORY + POSITION/EXPOSURE`

**Scope:** PROJECT-WIDE / COMBAT + PERCEPTION + COVER

**Status:** ADOPTED / PARTLY IMPLEMENTED; RICHER MATERIAL/MORPHOLOGY INTERACTION MAY ADVANCE LATER

**Existing support:** authoritative world occlusion in combat, graded cover/exposure, claim system, `P-COMBAT-01`.

---

### P-PHYS-06 — Dynamic physical change revises future affordances

**Statement:** When authoritative physical state changes, any dependent traversal, cover, exposure, line-of-sight, route, occupancy, or interaction affordance must be eligible for revision. Stale affordances do not outrank changed material truth.

Canonical relation:

`PHYSICAL CHANGE -> AFFORDANCE/EXPOSURE/ROUTE REVISION -> NEW POSSIBLE ACTION SET`

**Scope:** PROJECT-WIDE / DYNAMIC WORLD + AFFORDANCES

**Status:** ADOPTED / PARTLY IMPLEMENTED THROUGH DYNAMIC GEOMETRY, ROUTE REVISION, CLAIM INVALIDATION, AND v1.4 RELATIONAL CAUSALITY

**Existing support:** moving platforms, stable object/revision semantics, `P-MOVE-02`, v1.4 causal chain.

---

### P-PHYS-07 — Physical consequences generate evidence; they do not grant knowledge

**Statement:** A physical event may create visible, audible, spatial, semantic, or persistent evidence. Actors learn only through legitimate access to that evidence; the physical event itself does not automatically inject world truth into cognition.

Canonical relation:

`PHYSICAL EVENT -> CONSEQUENCE -> POTENTIAL EVIDENCE -> LEGITIMATE PERCEPTION/REPORT -> BELIEF REVISION`

**Scope:** PROJECT-WIDE / ENVIRONMENTAL PHYSICS + ACTOR EPISTEMOLOGY

**Status:** ADOPTED / ARCHITECTURALLY SUPPORTED; GENERALIZED ENVIRONMENTAL-EVIDENCE COVERAGE REMAINS INCREMENTAL

**Existing support:** `P-EPI-01`, `P-EPI-02`, semantic gunfire/damage/object/route events, v1.4 report system.

---

### P-PHYS-08 — One material event may propagate into several systems

**Statement:** One authoritative material cause may legitimately alter several downstream relations without becoming several unrelated fictional causes.

Example:

`BARRIER DESTROYED`
`-> route/access changes`
`-> cover/exposure changes`
`-> sound/visibility evidence`
`-> resource/process consequences`
`-> social interpretation`
`-> ecological opportunity`

**Scope:** PROJECT-WIDE / CROSS-SUBSYSTEM CAUSALITY

**Status:** ADOPTED DESIGN PREMISE / PARTLY PRESENT; FULL GENERALIZED PROPAGATION NOT YET CLAIMED

**Existing support:** `P-COMBAT-02`, `P-COMBAT-03`, Lifeworld/evental causality, topography/topology distinction.

---

### P-PHYS-09 — Physical laws persist across exploration, combat, and social play

**Statement:** Entering combat, negotiation, ordinary habitat activity, traversal, or another gameplay intensity does not silently switch the authoritative material world into a contradictory physics ruleset. Context may change utility and intention; material causality remains continuous.

Canonical relation:

`ONE AUTHORITATIVE PHYSICAL WORLD / MANY ACTIVITY INTENSITIES`

**Scope:** PROJECT-WIDE / LIFEWORLD + PHYSICS CONTINUITY

**Status:** ADOPTED / CONSISTENT WITH CURRENT CONTINUOUS SIMULATION ARCHITECTURE

**Existing support:** no global combat-mode ontology; `P-COMBAT-02`, `P-AUTH-01`, shared movement/combat world.

---

### P-PHYS-10 — Actors share material reality while morphology may differ

**Statement:** Human Crews, Human Rivals, Dream Cattlers, and other embodied actors resolve against one authoritative material world, but different morphology, condition, equipment, or capability may produce different legitimate affordances within that same world.

Example:

`SAME GEOMETRY -> HUMAN VAULT / INJURED HUMAN BLOCKED / CATTLER ALTERNATE TRAVERSAL`

when those capabilities are legitimately implemented.

**Scope:** PROJECT-WIDE / EMBODIMENT + ACTOR ECOLOGY

**Status:** ADOPTED DESIGN PREMISE / SHARED WORLD IMPLEMENTED; FULL MORPHOLOGY-SPECIFIC TRAVERSAL NOT YET CLAIMED

**Existing support:** shared authoritative movement/combat path, Cattler family distinction; current common human-sized clearance remains a known implementation boundary.

---

### P-PHYS-11 — Fidelity may simplify mechanics but may not contradict causal state

**Statement:** Physics/simulation may change representation or resolution for performance, networking, distance, or relevance, but the simplified representation may not contradict the causal state it stands for. Compression may reduce precision; it may not invent or erase material history without a legitimate cause.

Canonical relation:

`LOWER FIDELITY = LESS PRECISION, NOT DIFFERENT HISTORY`

**Scope:** PROJECT-WIDE / PHYSICS LOD + WORLD PERSISTENCE

**Status:** ADOPTED / DESIGN COMMITMENT

**Existing support:** `P-WORLD-05`, seeded-world causal-delta architecture, bounded simulation policy.

---

### P-PHYS-12 — Material properties earn simulation through causal relevance

**Statement:** A material property should be simulated when it changes a meaningful causal relation available to world systems or actors. The project does not require laboratory-complete material physics for properties with no gameplay, ecological, informational, or structural consequence.

Examples of potentially earned properties include friction, restitution, penetration resistance, structural strength, conductivity, permeability, and acoustic transmission—but only where a consumer and proof exist.

Canonical rule:

`MATERIAL PROPERTY + MEANINGFUL CAUSAL CONSUMER + PROVABLE CONSEQUENCE -> EARNED SIMULATION`

**Scope:** PROJECT-WIDE / MATERIAL PHYSICS DESIGN METHOD

**Status:** ADOPTED DESIGN PREMISE / GENERAL MATERIAL-PROPERTY SYSTEM NOT YET CLAIMED

**Existing support:** `P-ARCH-01`, `P-ARCH-02`, minimum-sufficient-machinery architecture.

---

# Cross-premise synthesis

The adopted FPS/environmental-physics layer can be summarized as:

```text
AUTHORITATIVE MATERIAL WORLD
        +
ACTOR EMBODIMENT / MORPHOLOGY
        +
INTENTION
        ->
LEGITIMATE AFFORDANCE / PHYSICAL CONSEQUENCE
        ->
POTENTIAL EVIDENCE
        ->
SITUATED BELIEF
        ->
UTILITY / GAME-THEORETIC ACTION
        ->
NEW MATERIAL WORLD
```

This does not reduce the game to a rigid-body simulator. It establishes **causal materialism with bounded simulation fidelity**: enough physics to make relations intelligible, persistent, exploitable, and shared, without requiring every decorative object or physical property to receive maximum-resolution simulation.

## STEWARD boundary

These premises are now deductively active at their stated scopes, but their statuses remain heterogeneous:

- some are already substantially implemented in current movement/collision/world code;
- some generalize existing implemented architecture;
- some are accepted design premises for future morphology, material, and cross-system physics work.

Therefore:

`ADOPTED PREMISE != CLAIM OF COMPLETE IMPLEMENTATION`

Protocol 2 remains unopened.

## Maxim

> **Physics determines what can materially happen; affordances determine what an embodied actor can do with it; evidence determines what an actor can know about it; game theory helps determine what the actor has reason to do about it.**
