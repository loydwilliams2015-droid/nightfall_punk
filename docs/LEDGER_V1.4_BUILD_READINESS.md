# nightfall!punk — v1.4 Build Readiness / FROM -> TO

Date: 2026-08-14
Branch: `build/v1.4-relational-combat-communications`
Status: **BUILD CANDIDATE — HUMAN VIDEO GATE REQUIRED**

## Version-line reconciliation

- v1.2 — Teammate Causality / Kinetic Stigmergy Alpha.
- v1.2b — Relational Perceptual Authority Camera corrective interlude.
- v1.2c — Integrated Population / Embodied Causality corrective completion.
- **v1.3 is intentionally unused.** No hidden, abandoned, or implied gameplay build occupies that number.
- v1.4 — Relational Combat Support / Bounded Communications.

The user-supplied repository suffix `nightfall_punkrepository` resolves to the existing authoritative repository `loydwilliams2015-droid/nightfall_punk`; no second repository is created.

## FROM — proven code + human observation

Authoritative v1.4 code base:

- branch: `build/v1.2c-integrated-causality`
- exact green candidate: `27af894fb9148be11447b8e26320bf18307645d6`
- GitHub Actions v1.2c run #362: PASS

The 2026-08-14 v1.2c human recording confirms the corrective proof surface is actually integrated:

- Player: 1;
- Player Crew teammate: 1;
- Human Rivals: 2;
- Dream Cattlers: 2;
- Relational Perceptual Authority Camera remains first-person and usable;
- teammate produces measurable embodied displacement (over 20 m during the recording);
- Rival and Cattler bodies are visibly present in the same slice.

The recording did not cleanly close the positive ALTER_ROUTE -> teammate reroute chain. That chain remains automated-positive / human-incomplete rather than falsely marked human-proven.

Earlier v1.4 design documents named v1.2b as the latest code proof. That baseline is superseded here by v1.2c. The accepted 150-question medium-degree design conclusions are not reopened by that correction.

## Study gate resolved before implementation

See `docs/LEDGER_V1.4_COMBAT_COMPARE10_COMMS.md`.

Conclusion:

1. preserve the high-degree authoritative combat core;
2. do not dump new weapon families or build giant GOAP machinery;
3. deepen incomplete-information coordination around combat;
4. replace exact squad telepathy with bounded reports;
5. generalize temporary coordination into claims;
6. make cover evaluation graded rather than binary;
7. establish stable object identity/revision as a world-change evidence seam;
8. keep visible/player-operable weapon/viewmodel conflict integration for v1.5.

## TO — v1.4 implementation contract

### A. Bounded report objects

New shared modules:

- `src/shared/nf_report.h`
- `src/shared/nf_report.c`

Report state explicitly carries:

`kind + scope + origin + reporter + ancestry + coarse_position + confidence + hops + origin/issue/delivery/expiry ticks`

Rules:

- direct firsthand evidence remains strongest;
- reports have bounded delivery latency;
- unseen reported positions are coarse, not exact transforms;
- Crew/Rival Crew scopes are explicit;
- age and relay depth reduce report weight;
- relay preserves ancestry;
- repeated same-origin messages do not become independent corroboration;
- expiry removes report evidentiary force.

### B. Player Crew route communication

The former implicit Crew-report path is replaced by an explicit `NfReport`.

ALTER_ROUTE success may publish a Crew-scoped route-state report. A teammate outside direct/encounter evidence range must remain unchanged before delivery and may learn after the report becomes live.

### C. Human Rival bounded communication

The previous exact-position instant squad blackboard is removed from Rival cognition.

Rival information channels are now:

- DIRECT;
- AUDIBLE;
- REPORT.

A Rival report is delayed, coarse, scoped, confidence-weighted, provenance-bearing, and short-lived. Report metadata remains diagnostic evidence; it is not promoted to world truth.

### D. Temporal claims

New shared modules:

- `src/shared/nf_claim.h`
- `src/shared/nf_claim.c`

Initial claim kinds:

- COVER;
- TRAVERSAL;
- INTERACTION;
- REVIVE.

Initial claim strengths:

- SOFT — coexistence possible;
- HARD — temporarily exclusive.

Lifecycle:

`CANDIDATE -> CLAIM -> GRANTED/SHARED/BLOCKED -> USE/YIELD -> RELEASE/EXPIRY`

No claim is permanent ownership.

### E. Graded tactical exposure

Human Rival cover choice now evaluates three body-height visibility samples and retains an exposure value in `[0,1]`.

The score remains bounded and combines:

- exposure;
- travel cost;
- future-option distance;
- existing health/ammo/visibility pressure.

This extends utility; it does not create a second navigation or combat authority.

### F. Stable object identity / revision

ALTER_ROUTE gate gains:

- stable `gate_key`;
- monotonic local `revision`;
- existing authoritative changed tick/by provenance.

This gives the bounded proof:

`OBJECT ID + REVISION -> AFFORDANCE CHANGE -> EVIDENCE -> BELIEF/UTILITY CHANGE`

without pretending universal mutable-object replication is already complete.

### G. v1.4 human proof surface

New `nightfall_v14_demo` preserves the v1.2c integrated population and camera.

Diagnostic controls:

- F1 — Player View;
- F2 — Observer / Live;
- F3 — Debug Top / Live;
- F4 — **PROOF SETUP**: debug-only relocation/reset of the bounded route-report experiment;
- F5 — full reset.

F4 does not represent a gameplay teleport mechanic. It is an explicitly labeled human-proof aid so interaction/report causality can be recorded reliably.

HUD exposes, for laboratory proof only:

- population;
- camera state;
- gate key/revision;
- interaction affordance and claim;
- Crew report pending/live/expired + weight;
- teammate evidence/belief/gradient/utility/commitment/control/displacement;
- Rival evidence source, report ID/ancestry/hops, report count, cover claims and exposure.

## EXISTING — must remain green

- [ ] fixed-rate authoritative simulation;
- [ ] ENet dedicated-server/client infrastructure;
- [ ] prediction/reconciliation;
- [ ] authoritative combat outcome pipeline;
- [ ] contamination -> degradation -> STASIS -> revival without causal rewind;
- [ ] Snap Locomotion / existing embodied movement authority;
- [ ] Relational Perceptual Authority Camera;
- [ ] Player Crew teammate evidence/memory/gradient/utility/commitment;
- [ ] positive + negative epistemic controls;
- [ ] physical teammate displacement proof;
- [ ] Human Rival perception/utility/roles/spatial behavior;
- [ ] Dream Cattler distinct ecology/recurrence/weak-point truth;
- [ ] topographical energy accounting;
- [ ] Lifeworld/epistemic-boundary contracts;
- [ ] integrated Player/teammate/Rival/Cattler graphical population.

## ADVANCING — v1.4 acceptance targets

- [ ] bounded report module compiles under strict warnings;
- [ ] report delay positive/negative proof;
- [ ] report scope proof;
- [ ] coarse-position proof;
- [ ] provenance/ancestry proof;
- [ ] report age/hop confidence degradation;
- [ ] explicit Player Crew route report;
- [ ] no teammate report knowledge before delivery;
- [ ] Rival exact blackboard transform removed;
- [ ] Rival REPORT evidence after legitimate delivery;
- [ ] generic temporal claim module;
- [ ] soft-share / hard-block / expiry tests;
- [ ] Human Rival cover claims use shared lifecycle;
- [ ] graded cover exposure enters bounded utility;
- [ ] ALTER_ROUTE stable key/revision;
- [ ] v1.4 graphical proof with F4 controlled setup;
- [ ] v1.4 named CTest/smoke proof;
- [ ] dedicated v1.4 CI workflow;
- [ ] human video review before merge.

## PAR / deliberate boundary

- [ ] player-visible weapon/viewmodel + reload/switch/fire presentation — v1.5;
- [ ] full reciprocal player-vs-Rival/Cattler conflict demonstration — v1.5;
- [ ] new weapon-family content expansion;
- [ ] deliberate Human Rival deception;
- [ ] universal trust scalar;
- [ ] physical/radio channel obstruction, jamming or infrastructure simulation;
- [ ] Cattler pack-report refactor to delayed region-granular/provenance transport;
- [ ] universal mutable-object replication schema;
- [ ] full Dynamic Affordance Graph regeneration;
- [ ] Crew-wide bargaining over every claim type;
- [ ] general GOAP;
- [ ] large world/cell expansion;
- [ ] broad destructibility/ragdoll/fluid expansion.

## Compile gate

Final exact v1.4 head, CI run and PASS disposition are filled into the human-acceptance handoff after the dedicated workflow completes. The branch must not be called green merely because an intermediate head compiled.

## Human acceptance criterion

The shortest positive proof is:

`F4 -> GATE rev1 CLOSED -> EVIDENCE NONE -> hold E -> INTERACTION CLAIM -> GATE rev2 OPEN -> CREW REPORT PENDING -> LIVE -> CREW_REPORT EVIDENCE -> BELIEF OPEN -> GRADIENT/UTILITY/COMMITMENT CHANGE -> EMBODIED TRAJECTORY CHANGE`

Negative proof:

`before report delivery -> no report knowledge -> no artificial belief update`.

The Rival side is accepted automatically when deterministic tests prove report delay/scope/coarsening/provenance and the human HUD visibly demonstrates that DIRECT/AUDIBLE/REPORT are distinct states during ordinary movement.
