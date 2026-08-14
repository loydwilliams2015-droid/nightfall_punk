# nightfall!punk — v1.2c Integrated Population / Embodied Causality Ledger

Date: 2026-08-13
Branch: `build/v1.2c-integrated-causality`
Base: v1.2b camera candidate `edf645ab7e434679d78be88708bddbe9789419c4`

## Why v1.2c exists

Human review of v1.2b established that the camera rebuild itself substantially worked, but the graphical proof surface had become an isolated test chamber:

- no Human Rivals were instantiated in the demo;
- no Dream Cattlers were instantiated in the demo;
- the teammate was instantiated and produced cognitive/route state, but the video showed no embodied locomotion;
- the teammate CTest asserted evidence, belief, utility, commitment and occupancy, but did not require measurable body displacement.

Therefore v1.2b is not accepted as an integrated build. v1.2c preserves the successful camera work and repairs the proof boundary.

## Canonical corrective law

> **An AI decision is not an action until embodied world state measurably changes.**

Testing ladder:

`Decision Produced != Control Produced != Body Actually Moved != World Relation Actually Changed`.

Causal acceptance chain:

`Evidence -> Belief -> Evaluation -> Commitment -> ControlIntent -> MovementController -> PhysicalDisplacement -> ChangedRelation`.

## FROM → TO

### Human proof population

FROM:

`Player + Teammate only`

with Rival/Cattler systems proven only in separate automated smoke tests.

TO:

`1 Player + 1 Teammate + 2 Human Rivals + 2 Dream Cattlers`

inside the same bounded graphical causal slice, using the existing actor/world authority and existing AI/Cattler control generators.

### Teammate proof

FROM:

`Evidence -> Belief -> Utility -> Commitment`

was sufficient for automated acceptance even if the body never visibly moved.

TO:

`Evidence -> Belief -> Utility -> Commitment -> Control -> Velocity/Displacement`.

The test now requires non-zero teammate control and >1 m physical XZ displacement over the bounded proof interval.

### Rival proof

FROM: Human Rival behavior remained available only through inherited standalone smokes during the graphical v1.2b demo.

TO: real Human Rival actors are instantiated and ticked inside the same graphical slice. The bounded automated integration proof requires a live Rival population and measurable physical displacement.

### Dream Cattler proof

FROM: Cattler ecology remained available only through inherited standalone smokes during the graphical v1.2b demo.

TO: real Dream Cattler actors are instantiated in the visible slice and retain existing ecology/evidence rules. Stationarity without evidence remains legitimate; movement proof is therefore elicited by legitimate audible prey evidence rather than by forced random wandering. The test requires resulting control and physical displacement.

### ALTER_ROUTE proof readability

FROM: human video could not distinguish `E not pressed` from `interaction contract failed`.

TO: HUD exposes:

- gate distance;
- `E HELD/UP`;
- `AFFORDANCE READY/NOT_READY/CONSUMED`;
- route state.

This makes failure provenance visible without granting actor knowledge.

## EXISTING — regression protected

- C11 + raylib graphical build;
- ENet/server/client/network contracts;
- client prediction/reconciliation;
- movement and traversal controller;
- `snap_locomotion` trajectory and affordance-capture contract;
- collider/material/physics truth;
- contamination, degradation, STASIS and return;
- Human Rival AI and spatial ecology;
- Dream Cattler habitat, pack, recurrence and evidence discipline;
- topographical energy accounting;
- Lifeworld contracts;
- teammate biography/profile, evidence provenance, bounded memory, route gradients, Priority Hotspots, commitment hysteresis and occupancy yielding;
- positive and no-omniscience negative teammate causal proofs;
- v1.2b Relational Perceptual Authority Camera;
- F1 Player / F2 Observer Live / F3 Debug Top Live;
- semantic `BLOCKED` route presentation.

## ADVANCING — v1.2c

- integrated graphical population: Player, teammate, Human Rivals, Dream Cattlers;
- population-aware actor rendering instead of hard-coded player + teammate only;
- real Rival `nf_ai_tick` controls consumed by the graphical world;
- real Dream Cattler `nf_cattler_tick` controls consumed by the graphical world;
- bounded Cattler placement inside the proof slice while preserving their real cognition/ecology logic;
- teammate `CONTROL -> SPEED -> DISPLACEMENT` diagnostics;
- live population counts plus moving counts;
- ALTER_ROUTE input/affordance diagnostics;
- route-change semantic alert publishing;
- stronger teammate CTest requiring actual embodied displacement;
- integrated Rival population/displacement proof;
- integrated Cattler evidence-to-control-to-displacement proof;
- `v12c.sh` wrapper and dedicated v1.2c acceptance surface.

## PAR — not falsely claimed by v1.2c

- full production dedicated-server combat/event processing inside the standalone greybox harness;
- full multi-client network replication of ALTER_ROUTE in this standalone proof;
- complete Crew population beyond the one causal teammate;
- full Rival crew social organization in the bounded graphical harness;
- broad arbitrary-object Dynamic Affordance Graph mutation;
- full first-person relational viewmodel and selectively visible lower body;
- final FOV/accessibility/camera impulse tuning;
- broad dynamic-object force coupling;
- full active-ragdoll/destruction/fluid fidelity;
- final visual art pass.

These remain compatible future work and must not be represented as v1.2c acceptance requirements.

## Automated acceptance

The exact candidate must pass:

1. strict full graphical Linux compile;
2. all inherited CTests;
3. relational camera test;
4. teammate positive/negative evidence proof;
5. population assertions: Player=1, Teammate=1, Rival=2, Cattler=2;
6. teammate control + physical displacement;
7. Rival physical displacement;
8. Cattler legitimate-evidence -> control -> physical displacement;
9. inherited network/combat/Rival/Cattler/energy/Lifeworld/contamination smokes.

## Human acceptance

The video must make it possible to verify, without inference from hidden code:

- chrome-blue teammate exists and physically changes position;
- red Human Rivals exist in the live world;
- orange/yellow Dream Cattlers exist in the live world;
- HUD population counts agree with visible population sufficiently for the slice;
- teammate `CONTROL` becomes non-zero and `DISPLACEMENT` grows;
- first-person camera remains stable and immediate;
- approaching the purple gate produces `AFFORDANCE READY` when legitimate;
- holding E is visibly registered;
- ALTER_ROUTE becomes OPEN;
- legitimate teammate evidence changes belief/gradient/commitment;
- changed commitment produces a materially changed teammate trajectory;
- F2/F3 remain diagnostic rather than actor knowledge.

## Development maxim produced by the v1.2b failure

> **Integration proof must exercise the relation between subsystems, not merely prove each subsystem exists elsewhere.**

For future builds, a standalone smoke for System A plus a standalone smoke for System B does not establish `A <-> B` in the human candidate. When the build claims a relation, CI should measure the relation itself.
