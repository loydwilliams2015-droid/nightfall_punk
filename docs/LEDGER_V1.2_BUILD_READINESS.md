# nightfall!punk — v1.2 Build Readiness / Handoff

Date: 2026-08-13
Branch: `build/v1.2-teammate-causality`
Build thesis: **Teammate Causality / Kinetic Stigmergy Alpha**
Status: **CODE/CI READY FOR HUMAN GREYBOX PROOF; do not merge until video review.**

## FROM — accepted v1.1

- Immutable archive: `archive/v1.1`
- Accepted v1.1 closeout head: `cf1c1958e6d4a78ca838b5d2e39d6659dfaa96d9`
- Human mechanical proof point: `6eb0a397b2f5c28d9aab20bf26429c80106a1b6d`
- Exact v1.1 closeout CI: run #316 — PASS.
- Acceptance ledger: `docs/LEDGER_V1.1_HUMAN_ACCEPTANCE.md`.

Carried presentation/coherence findings:
- player-facing contamination should not visually imply >100% unless deliberately reframed;
- persistent trace rendering should become less rectangular while causal accounting remains unchanged;
- prediction correction and authoritative STASIS/return relocation should be diagnostically distinct;
- Dream Cattler distance salience remains a presentation task;
- large HUD remains laboratory diagnostics;
- `Rancher` identifiers are compatibility debt only.

## TO — v1.2 causal slice

Positive proof:

`PLAYER WORLD-WRITE -> LEGITIMATE EVIDENCE -> BELIEF/MEMORY -> GRADIENT/HOTSPOT -> EXISTING MOVEMENT UTILITY -> INDEPENDENT REROUTE`

Negative control:

`WORLD CHANGE + NO LEGITIMATE EVIDENCE -> NO ARTIFICIAL KNOWLEDGE OR REACTION`

Implementation law:

`ExistingSystem -> CompatibleExtension -> MeasuredImprovement`

> Add consequence before adding machinery.

> Existing systems solve movement; gradients measure changing relations; hotspots concentrate consequence; memory carries consequence forward.

## EXISTING — regression protected

- [x] C11 + raylib full Linux build path.
- [x] ENet dedicated-server/client networking and prediction/reconciliation remain source-compatible and unchanged by the v1.2 proof harness.
- [x] Existing movement, traversal capture, colliders and physical world truth remain authoritative.
- [x] Existing combat + contamination/STASIS/return remain intact.
- [x] Human Rival encounter/spatial AI remains intact.
- [x] Dream Cattler ecology/recurrence remains intact.
- [x] Topographical energy + Lifeworld remain intact.
- [x] Existing region graph, soft spatial claims, cover reservations and attention-hint architecture remain intact.
- [x] Existing `nightfall.sh` remains the inherited regression runner.

## ADVANCING — implemented in this build

### One real teammate body
- [x] One `NF_FACTION_TEAMMATE` actor in the authoritative `NfWorld` simulation used by the causal harness.
- [x] Chrome-blue greybox identity `#2EA8FF` in the demo.
- [x] Same movement/body/contamination substrate as other actors.
- [x] Shared local-goal steering seam extracted from existing spatial control so new AI does not create a second locomotion authority.

### Causal individuality
- [x] Three minimum-sufficient biography hotspots: formative cooperation, mentorship/institution, Crew entry.
- [x] Biography deterministically produces strategic/personality tendencies rather than dice-random archetypes.
- [x] Initial result is competition-avoidant, reciprocally cooperative, independently minded and evidence-disciplined.
- [x] Counterfactual test changes one biography cause and verifies changed strategic pressure.
- [x] Decision API contains no protected demographic metadata shortcut.

### ALTER_ROUTE / combat topography
- [x] Clearly bounded 80x80 m design slice inside the existing 0.40 km^2 laboratory world.
- [x] Route A, Route B, reconnection, cover elements, prospect/elevation element, goal.
- [x] `ALTER_ROUTE` requires player proximity + `E` + sufficient manipulator function.
- [x] Gate movement changes the authoritative collider state; it is not a visual-only flag.
- [x] Material override test proves remembered possibility cannot manufacture a closed passage.

### Gradient + Priority Hotspot
- [x] Route candidates preserve ordinary spatial/energy evaluation.
- [x] Bounded material hotspot projection can influence route utility without becoming total meaning.
- [x] Actor-level Priority-Hotspot Gradient uses legitimate evidence, consequence, freshness and memory.
- [x] Commitment hysteresis resists small utility noise.
- [x] Authoritative material impossibility immediately breaks an invalid route commitment into reroute.

### Evidence + memory
- [x] Direct perception evidence.
- [x] Terrain-bounded Crew report evidence.
- [x] Later encounter evidence.
- [x] No evidence -> no belief update control.
- [x] 0–15 real-world minutes = recent priority memory.
- [x] Consequential/repeated memory can consolidate rather than disappear on a timer.
- [x] Memory records source, confidence, consequence, freshness, repetition and observed tick.

### Occupancy synthesis
- [x] Existing spatial movement remains operational authority.
- [x] Cooperative actor proximity + short-horizon projected motion form a bounded temporal occupancy claim.
- [x] Competition-avoidant teammate can yield rather than clump.
- [x] No actor-collision rewrite and no duplicate pathfinding system added.

### Causal diagnostics
- [x] Compact transition log: `EVIDENCE -> BELIEF -> GRADIENT -> HOTSPOT -> UTILITY -> COMMITMENT -> ACTION`.
- [x] Explicit negative-control log: `EVIDENCE=NONE ... ACTION=UNCHANGED`.
- [x] Graphical demo HUD exposes evidence, belief, gradient, route energy/utility, commitment and occupancy yield.

## PAR — deliberately future-compatible / not acceptance requirements for this proof

- [ ] Integrate teammate system into the production dedicated-server tick rather than only the bounded authoritative greybox harness.
- [ ] Replicate `ALTER_ROUTE` through the existing server/client transport once the network-bearing source path can be edited cleanly; do not create a duplicate protocol solely for this proof.
- [ ] Generalize teammate combat/perception as a full peer without contaminating Human Rival encounter roles.
- [ ] Multiple teammates and full Crew disagreement/bargaining.
- [ ] Full autobiographical personality/social simulator and Crew institutions.
- [ ] General dynamic affordance graph/navmesh updates across arbitrary world objects.
- [ ] Full hard/soft/temporal occupancy integration across all actors.
- [ ] Full re-routing ladder instrumentation beyond the local proof.
- [ ] Production planar-reflection (`PR`) layer/dynamic-lighting system.
- [ ] Broad destructive physics.
- [ ] Large distributed seed/cell world orchestration.
- [ ] Final presentation fixes listed in v1.1 human findings.
- [ ] Wholesale legacy technical identifier migration.

## Automated proof status

Implementation head `20c166f2fabd6125b8b67f7fdce523785a8fcdb0` passed GitHub Actions run #319 after one evidence-driven correction:

1. first implementation compiled but the material-override assertion failed because Route-B commitment hysteresis survived a newly closed gate;
2. decision logic was corrected so material impossibility breaks the commitment;
3. all 10 CTests then passed;
4. every inherited v1.1 smoke also passed.

Final wrapper/handoff head `564f2c28970ba9763ebfc28e63a0be193369d57c` then passed GitHub Actions run #320, including:
- v1.2 standard check;
- strict full graphical Linux compile including the causal greybox;
- all inherited + teammate causal CTests;
- all inherited combat/network/Human Rival/Cattler/energy/Lifeworld/contamination smokes;
- named teammate causality positive + negative proof.

This ledger-finalization commit is documentation-only; its exact head should also be green before the branch is frozen for human recording.

## Human video gate

Use the v1.2 demo wrapper after final CI is green.

Record one clean run showing:
- [ ] initial chrome-blue teammate and Route-A commitment;
- [ ] player moves toward the purple ALTER_ROUTE gate;
- [ ] hold `E` near the gate and visibly change collider/topography;
- [ ] HUD shows legitimate evidence rather than instantaneous hidden knowledge;
- [ ] belief changes to Route B open;
- [ ] gradient/utility changes;
- [ ] commitment changes to Route B;
- [ ] teammate independently reroutes through the changed possibility;
- [ ] show enough of the 80x80 m slice that Route A/B and reconnection are visually understandable.

Optional second clip:
- automated negative control is already a hard test; human negative-control footage is useful but not required for this gate.

## Merge gate

- [ ] final exact-head CI green;
- [ ] human video reviewed against this ledger;
- [ ] video findings categorized KEEP / AMEND / DEFER / RETIRE;
- [ ] only then prepare merge/next integration step.

## Administrative note

PR #17 remains a draft administrative loose end because the connected GitHub mutation path rejected PR-state/metadata changes despite accepted code and green CI. The exact v1.1 state is nevertheless preserved at `archive/v1.1`, and v1.2 branches directly from that accepted head. Do not confuse PR UI state with code-lineage evidence.

---

## 2026-08-13 PRAXIS / SIGNAL / STEWARD handoff addendum

Canonical handoff: `docs/LEDGER_V1.2_PRAXIS_SIGNAL_STEWARD_HANDOFF.md`.

### FROM — current evidence

- PR #18 remains the active v1.2 causal-slice review target.
- Pre-handoff head `e65e19e931794fbe2f7e7f359b982df6cc10d4de` passed `nightfall v1.2 CI` run **#322**.
- Current executable/source-backed claim remains the bounded chain:
  `WORLD-WRITE -> LEGITIMATE EVIDENCE -> BELIEF/MEMORY -> GRADIENT/HOTSPOT -> UTILITY -> COMMITMENT -> ACTION`.
- The Pac-Man compare/contrast run is recorded as **ANALYTIC / INTERPRETIVE TEST**, not a compiled game mode, benchmark or CI proof.

### TO — provisional next slice, not yet accepted v1.3

Working direction:

> **Production-Integrated Situated Reasoning / Plural Evidence Alpha**

Priorities after the v1.2 human gate:

1. production dedicated-server integration of the teammate causal loop;
2. ordinary network-visible world consequences for the causal route change;
3. preserve `world truth != actor knowledge != group report`;
4. one bounded contradictory-evidence proof;
5. material legality remains deductively decisive even when memory/report evidence conflicts;
6. at most one earned many-valued/fuzzy predicate proof;
7. keep fuzzy applicability, probability, confidence, contradiction and utility distinct;
8. no giant GOAP, theorem prover, master truth score or duplicate locomotion architecture.

### Readiness status after this documentation update

The pre-handoff code head has successful CI evidence. These documentation commits move the branch head forward, so **exact-head CI must be re-established before v1.2 is described as final-head green or frozen for human acceptance.** No gameplay compile was requested by this ledger/handoff operation.
