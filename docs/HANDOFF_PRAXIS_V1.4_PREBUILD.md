# nightfall!punk — PRAXIS Handoff Before v1.4 Build

Date: 2026-08-14
Branch: `build/v1.2b-camera-rebuild`
Status: **HANDOFF ONLY — DO NOT COMPILE YET**

## FROM — latest proven code state

The latest gameplay/code proof remains the v1.2b camera/causality line.

- v1.2 frozen causal candidate base: `e65e19e931794fbe2f7e7f359b982df6cc10d4de`
- v1.2b green code/CI proof head: `edf645ab7e434679d78be88708bddbe9789419c4`
- later commits on the branch are documentation/design reconciliation and do **not** constitute a newer gameplay proof.

Protected implemented contracts carried forward:

- fixed 60 Hz authoritative simulation;
- ENet dedicated-server/client networking + prediction/reconciliation;
- authoritative combat outcomes;
- Snap Locomotion / `snap_locomotion` as the current local semantic affordance-capture layer;
- Relational Perceptual Authority Camera;
- contamination -> functional degradation -> STASIS -> revival without causal rewind;
- Human Rival situated perception/utility/spatial reasoning;
- Dream Cattler distinct ecology/recurrence logic;
- topographical energy accounting;
- teammate causal slice: `WORLD-WRITE -> EVIDENCE -> BELIEF/MEMORY -> GRADIENT/HOTSPOT -> UTILITY -> COMMITMENT -> ACTION`;
- bounded short-horizon occupancy/yielding;
- negative-control epistemic proof: no legitimate evidence -> no artificial knowledge/reaction.

## DESIGN STATE CARRIED FORWARD

`docs/LEDGER_V1.4_MEDIUM_DEGREE_RESOLUTION.md` closes the 150-question medium-degree planning layer.

Current v1.4 thesis:

```text
WORLD CHANGE
  -> TOPOGRAPHICAL + TOPOLOGICAL REVISION
  -> AFFORDANCE REVISION
  -> LEGITIMATE EVIDENCE
  -> DIVERGENT BELIEF / UTILITY
  -> CLAIMS / OCCUPANCY
  -> SNAP LOCOMOTION
  -> AUTHORITATIVE PHYSICS / COMBAT
  -> CONSEQUENCE
  -> TRACE / RESOURCE / ROUTE / EXCHANGE CHANGE
  -> NEW EVIDENCE
```

Topography and topology remain distinct:

- **topography** = geographic/metric shape and distribution;
- **topology** = connectivity/exchange/conversion/dependency structure.

Spatial base rule:

- Southern-California / Las Flores substrate may be indexed by **8 m x 8 m base geographic elements**;
- larger regions / simulation cells / authority cells are aggregations of many base elements;
- seed worlds aggregate larger localities;
- do not confuse the 8 m x 8 m element with an 8x8 lattice count or an 8 km x 8 km world scale.

## PRAXIS — next build study gate

Do **not** begin implementation until two study passes are completed and reconciled against current code.

### A. Combat Systems — Compare-10

The next combat pass should compare **ten strong references/mechanisms**, not ten feature inventories.

Evaluate each against current nightfall combat in these families:

1. combat authority and hit legitimacy;
2. weapon-role ecology;
3. lethality / time-to-consequence;
4. recoil / spread / recovery / handling grammar;
5. cover / exposure / firing-lane relations;
6. suppression / pressure / flank interaction;
7. ammunition / reload / weapon-switch resource rhythm;
8. movement-combat coupling and mobility envelope;
9. enemy-composition synergy / encounter micro-rhythm;
10. combat aftermath: contamination, STASIS, inventory, traces, route/resource consequences.

For each reference use:

`OBSERVE -> CURRENT NIGHTFALL EQUIVALENT -> GAP -> KEEP/ADAPT/DEFER/REJECT -> SMALLEST CODE CONSEQUENCE -> HUMAN PROOF`

Hard boundary:

> **Combat authority is already high-degree; do not rewrite the server-authoritative combat core merely to broaden weapon content.**

The likely medium-degree targets are combat ecology and relations: graded cover, directional exposure, firing lanes, future-option value, teammate combat integration, resource recurrence, composition, and micro-rhythm.

### B. AI Communications

Study/resolve communications as an incomplete-information and signaling system.

Current accepted policy:

- truthful-but-incomplete reporting first;
- coarse reports are informationally poorer than firsthand evidence;
- provenance, confidence, age, scope, and source ancestry matter;
- repeated receipt from one originating source is not independent corroboration;
- latency/confidence degradation should be bounded and channel/context sensitive;
- world/network packet timing is never itself gameplay meaning;
- no report may inject exact unseen world truth;
- deliberate Rival deception is deferred until truthful/incomplete reporting is stable and legible.

Primary questions for the next pass:

1. What information should a teammate, Human Rival, or Cattler be able to report?
2. At what spatial/semantic granularity?
3. What is lost in transmission?
4. How does latency change utility without making actors feel unresponsive?
5. How should trust/provenance alter weighting without becoming one universal trust scalar?
6. Which signals are private, shared, public, or plausibly common knowledge?
7. How should reports interact with topographical distance/obstruction and topological communication/exchange relations?
8. What is the smallest communication failure that produces interesting gameplay rather than noise?
9. How do communications affect combat composition and map control without becoming squad telepathy?
10. What deterministic positive + negative controls prove the system?

## DO NOT IMPLEMENT YET

Until Compare-10 combat + AI-communications review is complete:

- no new weapon family dump;
- no general GOAP;
- no universal squad telepathy;
- no random lying system;
- no global exact blackboard transforms;
- no rewrite of Snap Locomotion;
- no rewrite of combat authority;
- no large map/cell expansion;
- no compile.

## TO — intended v1.4 decision point

After the two study passes, PRAXIS should produce:

1. combat Compare-10 synthesis;
2. AI-communications architecture synthesis;
3. contradictions with current v1.4 design, if any;
4. automatic high-confidence recommendations;
5. a small set of genuinely mid/high-uncertainty questions;
6. one bounded v1.4 implementation contract;
7. only then branch/code/compile when explicitly authorized.

Maxim:

> **First understand the combat conversation; then change the combat code.**
