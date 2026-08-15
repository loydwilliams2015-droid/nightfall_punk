# nightfall!punk — v1.4 Combat Compare-10 + Bounded Communications

Date: 2026-08-14
Status: **STUDY GATE RESOLVED — IMPLEMENTATION BOUNDED**

## Source reconciliation

The earlier v1.4 planning handoff named v1.2b as the latest code proof. That is now stale.
The actual v1.4 FROM code is the green v1.2c corrective candidate:

- branch: `build/v1.2c-integrated-causality`
- head: `27af894fb9148be11447b8e26320bf18307645d6`
- v1.2c human recording confirms the integrated proof surface is genuinely v1.2c: one Player, one teammate, two Human Rivals, two Dream Cattlers, preserved first-person camera, and measurable teammate displacement.
- positive ALTER_ROUTE -> reroute remains primarily automated proof rather than a clean human-video closeout.
- v1.3 is intentionally unused. No hidden/abandoned gameplay build occupies that number.

The 150-question v1.4 medium-degree design resolution remains accepted; only its FROM baseline is amended from v1.2b to v1.2c.

## Governing implementation rule

`ExistingSystem -> CompatibleExtension -> MeasuredImprovement`

Do not rewrite high-degree authoritative combat, movement, camera, contamination, or epistemic boundaries merely to broaden content.

## Combat Compare-10

The comparison is mechanism-first rather than a feature inventory.

| # | Reference / mechanism lens | Useful mechanism | Current nightfall equivalent | v1.4 disposition |
|---|---|---|---|---|
| 1 | F.E.A.R. planning / replanning | small action vocabulary, context-sensitive replanning, squad pressure/flank behavior | utility modes + commitment + material impossibility | **ADAPT**: improve information and claim inputs, no giant GOAP |
| 2 | Counter-Strike bot believability | human-bounded attention/perception and credible reaction | bounded LOS, reaction delay, confidence | **KEEP/ADAPT**: remove exact shared blackboard truth |
| 3 | Splinter Cell: Blacklist perception | graded awareness with legible evidence and feedback | direct/audible evidence + confidence | **ADAPT**: explicit evidence source/provenance |
| 4 | Watch Dogs 2 cover selection | cover is evaluated, not merely tagged | generated cover affordances | **ADAPT**: graded exposure + future-option value |
| 5 | Days Gone squad-space coordination | role/space interaction and bounded coordination | Rival roles + encounter pressure | **ADAPT**: temporal claims instead of permanent reservation |
| 6 | Quake 3 combat grammar | immediate spatial agency and legible firing-space consequence | responsive movement + authoritative combat | **KEEP**: do not bury action in planner latency |
| 7 | Halo encounter composition | readable role interaction and spatial counterplay | pressure/flank/hold roles + encounter cap | **KEEP/ADAPT**: composition remains relational rather than script-only |
| 8 | Titanfall 2 movement-combat coupling | high-authority movement remains combat-relevant | Snap Locomotion + embodied controller | **KEEP**: combat cannot create a second locomotion authority |
| 9 | Prey 2017 systemic affordances | combat meaning emerges from object/material state | contamination, route change, Lifeworld, semantic alerts | **KEEP/ADAPT**: stable object IDs/revisions become evidence anchors |
| 10 | Alien: Isolation incomplete information | threat is meaningful because knowledge is partial and locally earned | evidence-bounded actor knowledge | **KEEP**: communications must not restore omniscience |

### Compare-10 synthesis

High-degree core remains:

`INTENT -> VALIDATION -> AIM/HIT LEGITIMACY -> RELATIONSHIP PERMISSION -> AUTHORITATIVE DAMAGE/CONTAMINATION/STASIS -> EVENT/SNAPSHOT`

v1.4 advances the relations around that core:

`EVIDENCE -> REPORT/PROVENANCE -> BELIEF -> GRADED EXPOSURE/UTILITY -> TEMPORAL CLAIM -> CONTROL -> EMBODIED ACTION -> CONSEQUENCE`

Player-visible weapon/viewmodel, reload UX, hit presentation, and full reciprocal playable conflict are not required to prove this build. They remain the intended v1.5 integration frontier.

## AI communications synthesis

### Rejected predecessor

The pre-v1.4 Human Rival blackboard copied an exact target transform immediately across the squad. It was computationally useful but epistemically too strong.

### v1.4 contract

A report is a bounded gameplay object:

```c
Report = {
    kind,
    scope,
    origin,
    reporter,
    ancestry_id,
    coarse_position,
    confidence,
    hops,
    origin_tick,
    issued_tick,
    deliver_tick,
    expiry_tick
};
```

Rules:

1. firsthand direct evidence remains stronger than a report;
2. reports are delayed rather than instant;
3. unseen position reports are quantized/coarse, not exact transforms;
4. reports have scope (private / Crew / Rival Crew / public);
5. confidence decays with age and relay depth;
6. relays preserve ancestry;
7. repetition from the same ancestry does not add independent corroboration;
8. expired reports cease to be evidence;
9. report receipt may change belief/utility, never world truth;
10. packet/network timing is not itself gameplay meaning.

For the bounded v1.4 slice, Player Crew and Human Rival communication use the explicit report contract. Physical channel interruption, radio infrastructure, and richer social trust weighting remain PAR until this basic contract is human-legible.

Dream Cattler pack communication remains its existing species-specific ecological report mechanism in v1.4. It is deliberately **not** forced through the humanoid `NfReport` schema in the same build. A later Cattler pass should make pack reports delayed and region-granular/provenance-aware while preserving distinct ecology; until then, that transport is explicitly PAR rather than falsely claimed as generalized.

## Temporal claims synthesis

Replace one-off reservation booleans with a bounded shared lifecycle:

`Candidate -> Claim -> Granted/Shared/Blocked -> Use/Yield -> Release/Expiry`

v1.4 claim kinds:

- COVER
- TRAVERSAL
- INTERACTION
- REVIVE

Strength:

- SOFT: can coexist with other soft claims;
- HARD: exclusive while live.

Claims are defeasible and time-bounded. They do not create permanent ownership.

## Graded tactical exposure

Cover is no longer scored only as `blocked / not blocked`.
The bounded v1.4 evaluator samples several body heights and produces exposure in `[0,1]`.
Cover utility then combines:

- lower exposure;
- travel cost;
- future-option distance;
- existing health/ammo/visibility pressure.

This is a tactical gradient, not a second navmesh or combat authority.

## Stable object revision seam

The ALTER_ROUTE gate now has:

- a stable semantic key;
- a monotonically advancing local revision;
- authoritative changed tick/by provenance.

This is the smallest v1.4 proof of:

`Object Identity + Revision -> Affordance Change -> Evidence -> Different Actor Interpretation`

It does not attempt universal mutable-object serialization yet.

## Bounded implementation decision

### AUTO KEEP

- v1.2c population integration;
- Relational Perceptual Authority Camera;
- Snap Locomotion / embodied movement authority;
- server-authoritative combat and contamination/STASIS;
- Human Rival utility modes and roles;
- Dream Cattler distinct ecology;
- topographical energy and Lifeworld;
- negative epistemic controls;
- action requires embodied displacement.

### AUTO KEEP + MEASURE

- 4 m humanoid report-position quantum;
- ~0.2 s Crew report delay in the human proof;
- 3 s short report lifetime;
- report confidence decay and relay penalty;
- hard cover claims with ~1 s renewal horizon;
- three-height exposure sampling;
- F4 proof-setup diagnostic.

### DEFER / PAR

- deliberate Rival deception;
- universal social trust scalar;
- radio/network infrastructure simulation;
- Cattler pack-report delay/region-granularity/provenance refactor;
- multiple Crew teammates bargaining over all claim classes;
- full dynamic-affordance graph regeneration;
- general mutable-object replication schema;
- player viewmodel/weapons and reciprocal conflict presentation (v1.5);
- new weapon-family content dump;
- general GOAP;
- large map/cell expansion.

## Human proof target

The v1.4 recording should make this legible:

`GATE rev1 CLOSED`
`-> Player earns INTERACTION claim`
`-> GATE rev2 OPEN`
`-> CREW REPORT PENDING`
`-> report becomes LIVE after bounded latency`
`-> teammate acquires CREW_REPORT evidence without exact transform injection`
`-> BELIEF OPEN`
`-> utility/gradient/commitment changes`
`-> embodied trajectory changes`

In parallel, Rival HUD diagnostics should expose DIRECT / AUDIBLE / REPORT evidence, live report count, report ancestry, and live cover-claim count without giving the player those values as diegetic gameplay knowledge.

Maxim:

> **Information spreads; cognition does not copy itself. Coordination claims possibilities; it does not own them forever.**
