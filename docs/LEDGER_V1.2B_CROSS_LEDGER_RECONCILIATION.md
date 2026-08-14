# nightfall!punk — v1.2b Cross-Ledger Reconciliation

Date: 2026-08-13
Branch: `build/v1.2b-camera-rebuild`
Status: **CURRENT / SUPERSEDING ACTIVE-TERMINOLOGY RECORD**
Compile status: **no compile implied by this documentation update**.

## Purpose

This record reconciles the active project vocabulary and the accepted recommendations from the high-degree terminology audit against the actual v1.2b build. It does not rewrite historical version ledgers. Historical ledgers remain evidence of what the project called a subsystem at that checkpoint.

Where an older active summary conflicts with this file on current terminology/status, this file wins until the next master-ledger consolidation.

---

# 1. Canonical locomotion terminology

## Current canonical name

**`snap_locomotion`** = the current shared local semantic locomotion / affordance-capture layer.

Operational meaning:

> `snap_locomotion` converts high-authority movement intention plus local legitimate affordances into bounded trajectory guidance before authoritative physical resolution.

It is not global pathfinding, not a navmesh, not a cinematic rail, and not exclusive action ownership.

Canonical local chain:

```text
INTENTION
  ↓
LOCAL AFFORDANCE QUERY
  ↓
SNAP_LOC0MOTION / BOUNDED AFFORDANCE CAPTURE
  ↓
AUTHORITATIVE MOVEMENT / PHYSICS
  ↓
CONSEQUENCE
```

Spelling note: code/docs should use `snap_locomotion`; prose may use **Snap Locomotion**.

## Retired/legacy alias

**Fuzzy Rail** is now a **historical/legacy conceptual alias only**.

- Do not use `Fuzzy Rail` as the canonical name in new architecture, build, handoff, or design records.
- Do not retroactively rewrite immutable historical ledgers merely to erase the older term.
- When an older document says `Fuzzy Rail`, interpret it as the historical ancestor/name for the local affordance-capture movement layer that is now canonically called `snap_locomotion`, unless the document explicitly means something narrower.

Maxim:

> **Snap Locomotion captures affordances; it does not put the actor on rails.**

---

# 2. v1.2b current implementation truth

The following remain current implemented/proven v1.2/v1.2b contracts:

- fixed 60 Hz authoritative simulation;
- ENet dedicated-server/client networking with prediction/reconciliation;
- shared authoritative movement/collision/traversal substrate;
- `snap_locomotion` / bounded affordance-capture trajectory remains downstream of legitimate affordances and upstream of physical consequence;
- Human Rival perception/utility/spatial reasoning;
- Dream Cattler distinct habitat/ecology/recurrence logic;
- topographical energy accounting and Lifeworld contracts;
- teammate causal slice: `WORLD-WRITE -> EVIDENCE -> BELIEF/MEMORY -> GRADIENT/HOTSPOT -> UTILITY -> COMMITMENT -> ACTION`;
- negative causal control: world change without legitimate evidence does not inject artificial knowledge;
- bounded short-horizon occupancy/yielding proof;
- Relational Perceptual Authority Camera in v1.2b;
- FIRST_PERSON canonical gameplay proof; observer/top modes remain diagnostics;
- camera presentation does not author body truth;
- blocked route utility presents semantically as `BLOCKED` rather than exposing numerical sentinel state;
- v1.2b named CI proof is green at branch head `edf645ab7e434679d78be88708bddbe9789419c4`.

Do not promote future/partial seams to implemented status merely because this file accepts a design recommendation.

---

# 3. Accepted recommendations from the high-degree terminology audit

The user accepted the recommendations. These become **design policy**, with implementation status noted separately.

## R1 — local navigation architecture

**ACCEPTED: hybrid, evidence-gated navigation.**

Current behavior remains:

`coarse region reasoning -> local collision/dynamic-affordance reasoning -> snap_locomotion -> physics`.

A conventional navmesh is **not** required as a universal intermediate layer now.

Future rule:

- add a navmesh only where large/static pedestrian surfaces or authoring/scale evidence shows clear benefit;
- navmesh, if added, is derived accessibility data rather than physical authority;
- dynamic object state/affordances may override stale nav accessibility;
- do not duplicate `snap_locomotion` with a second local movement authority.

Status: **accepted architecture; conventional navmesh not presently claimed implemented**.

## R2 — player occupancy / reservation semantics

**ACCEPTED: implicit player occupancy/intention claims without player constraint by AI arbitration.**

- player motion/body truth produces occupancy evidence;
- AI predicts and yields around legitimate player movement where appropriate;
- shared systems may represent implicit temporal occupancy for arbitration;
- reservations do not take control away from the player or grant magical collision immunity;
- hard physical collision remains authoritative.

Status: **direction accepted; v1.2 already proves a bounded short-horizon cooperative occupancy/yield case, full integration remains future work**.

## R3 — slide / slide-hop

**ACCEPTED: defer as optional side-grade movement verb.**

- do not add slide/slide-hop merely because Titanfall-like movement vocabulary exists;
- current movement + camera + snap-locomotion proof stabilizes first;
- later prototype only if it improves the Neo-Xennial movement identity and preserves systemic physical readability.

Status: **deferred, not implemented requirement**.

## R4 — AI communication fidelity

**ACCEPTED: coarse reports + bounded latency + confidence degradation.**

- reports remain poorer than firsthand evidence;
- latency/confidence may vary by situation;
- infrastructure-dependent communications may be added later only if it materially improves stealth/systemic play;
- no report may grant exact unseen world truth.

Status: **design policy; existing provenance/confidence/evidence architecture is compatible, full communications simulation not claimed**.

## R5 — high-value resource recurrence

**ACCEPTED: mixed recurrence ecology.**

Use both:

1. predictable designed recurrence where arena readability/timing benefits from it; and
2. systemic world-driven replenishment/production where Lifeworld causality should matter.

Do not force all resources into one respawn-clock ontology or one ecology-only ontology.

Status: **design policy**.

## R6 — player-facing ecology / energy legibility

**ACCEPTED: world first + subtle diegetic instrumentation + occasional Ghost Static interpretation.**

Priority:

`world behavior/material consequence -> diegetic cues/instruments -> Ghost Static interpretation -> explicit numeric HUD only when diagnostically necessary`.

Exact ecology/energy ledgers remain server/debug truth rather than ordinary player score.

Status: **design policy, extending existing ecological-waterline doctrine**.

## R7 — encounter micro-rhythm

**ACCEPTED: variable local rhythm.**

- no universal encounter clock;
- many useful local phrases may fall roughly in the 20–90 second range;
- actual duration/intensity follows current possibility, evidence, player choice, ecology and sustainable-engagement state;
- spikes/crescendos require release/recovery afterward.

Status: **design policy**.

## R8 — production replication relevance

**ACCEPTED hybrid relevance pipeline:**

```text
REGION / CELL COARSE FILTER
        ↓
DISTANCE / VISIBILITY RELATION
        ↓
SEMANTIC IMPORTANCE / CAUSAL RELEVANCE
        ↓
NETWORK RELEVANCE
```

No single criterion owns production interest management.

Status: **future production architecture; not a v1.2b implementation claim**.

## R9 — scale-up gate

**ACCEPTED: bundled acceptance gate before materially enlarging the bounded lab.**

Scale only when all are credible together:

- acceptable CPU/memory/query budget;
- low navigation/traversal failure rate;
- stable AI/route behavior;
- demonstrated encounter/route diversity;
- networking/reconciliation stability;
- human play/video acceptance.

A large map is not evidence of a mature architecture.

Status: **governance/acceptance policy**.

---

# 4. PRAXIS — implementation ordering implied by the accepted recommendations

Do not branch into nine independent feature systems. Reuse existing owners.

Preferred order:

1. preserve v1.2b human camera/causal proof;
2. finish production integration seams already listed in v1.2/v1.2b PAR;
3. generalize bounded occupancy/claim lifecycle without constraining player agency;
4. enrich route valuation/diagnostics while preserving region + local affordance + `snap_locomotion` separation;
5. add bounded AI-report latency/confidence behavior;
6. add mixed resource-recurrence experiments only where a human proof can distinguish designed recurrence from systemic replenishment;
7. introduce replication relevance only when actual scale makes current actor-cap/locality assumptions insufficient;
8. use the bundled scale gate before expanding simulation footprint.

Implementation law remains:

> **ExistingSystem -> CompatibleExtension -> MeasuredImprovement.**

---

# 5. SIGNAL — terminology and evidence discipline

Use these current terms externally/internally:

- **Snap Locomotion / `snap_locomotion`** — local semantic high-authority locomotion with bounded affordance capture.
- **Region graph / coarse spatial reasoning** — strategic locality selection and bounded routing context.
- **Dynamic affordance reasoning** — current local possibilities under authoritative material state.
- **Relational Perceptual Authority Camera** — immediate perceptual look + body-truth-following bounded presentation.
- **Priority-Hotspot Gradient** — actor-level consequential salience from legitimate evidence/memory, not a magic global objective map.

Avoid:

- calling `snap_locomotion` a rail system;
- calling diagnostic omniscience actor knowledge;
- calling a design recommendation implemented without code/test proof;
- calling a static route graph the whole navigation architecture;
- calling successful CI a substitute for human FPS-feel proof.

---

# 6. STEWARD — record status audit

## Current/superseding records

- `docs/LEDGER_V1.2_BUILD_READINESS.md` — current v1.2 causal-slice proof/status.
- `docs/LEDGER_V1.2B_CAMERA_REBUILD.md` — current v1.2b camera proof/status.
- **this file** — current cross-ledger terminology + accepted-recommendation reconciliation.
- `docs/HANDOFF_V1.2B_CAMERA_REBUILD.md` — current human-video handoff for camera proof.

## Active summaries with known stale wording

- `docs/LEDGER.md` contains historical/current material but its front version-status table and `Fuzzy Rail` naming predate v1.2b. Treat this reconciliation as superseding those current-status/terminology fragments until the master ledger receives a safe full consolidation.
- `docs/ARCHITECTURE.md` preserves important architecture but still contains `Fuzzy Rail` and several v0.8-era section labels. Interpret local traversal references through the `snap_locomotion` terminology in this record until that document is consolidated.
- `README.md` on this branch still describes an older candidate milestone and is not authoritative for v1.2b build status.

## Historical records

Version-specific historical ledgers such as v1.0/v1.1 are **not stale merely because terminology changed later**. They are immutable provenance unless a factual error must be corrected with an explicit addendum.

Record precedence for current work:

```text
LATEST ACCEPTED/ACTIVE BUILD LEDGER
        +
LATEST CROSS-LEDGER RECONCILIATION
        >
OLDER ACTIVE SUMMARY WORDING
        >
HISTORICAL TERMINOLOGY FOR CURRENT-NAME PURPOSES
```

---

# 7. Current unresolved implementation watches

These remain open even after accepting the nine recommendations:

- production dedicated-server integration of the bounded teammate causality system;
- replication of `ALTER_ROUTE` through existing transport;
- full peer teammate combat/perception integration;
- multiple teammates / Crew disagreement and bargaining;
- general dynamic affordance updates across arbitrary world objects;
- full hard/soft/temporal occupancy integration across all actors;
- broader rerouting instrumentation;
- final per-traversal camera smoothing constants from human proof;
- camera-aware audio listener, controller input, relational viewmodel/body presentation;
- production-scale replication relevance implementation;
- large distributed seed/cell orchestration;
- bundled scale-gate evidence.

No recommendation in this record silently closes those implementation watches.

---

# Three-role handoff

## PRAXIS

Use `snap_locomotion` as the current locomotion term and implement accepted recommendations only through existing subsystem seams. No duplicate local movement planner.

## SIGNAL

Report implementation state separately from design confidence. Use the new terminology consistently and preserve bounded-knowledge language.

## STEWARD

Treat this file as the current reconciliation layer. Historical ledgers remain historical; active summaries with stale wording must not override this record.

Maxims:

- **Snap Locomotion, not Fuzzy Rail.**
- **A recommendation can be accepted before it is implemented.**
- **Historical records preserve history; reconciliation preserves current truth.**
- **Scale only after the small world proves the relations that the large world must preserve.**
