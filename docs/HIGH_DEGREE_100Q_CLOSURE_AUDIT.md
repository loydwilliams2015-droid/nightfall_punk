# nightfall!punk — High-Degree Systems 100-Question Closure Audit

**Status:** PRAXIS / SIGNAL / STEWARD design closure pass against `main` at commit `72851c98911243665e4f420677eabc2d1c2402de`.

**Compile status:** no compile implied or performed by this document.

## Purpose

Resolve gaps inside systems already treated at high degree. Do not reopen settled foundations unless evidence changes. Automatic decisions follow this rule:

- **LOCK** — deductively follows from an accepted engine/world invariant; becomes canonical.
- **ADOPT+MEASURE** — inductively very likely to improve the project and fits existing architecture; adopt provisionally, instrument, and retain rollback/tuning.
- **ASK** — materially different good answers remain; user judgment is required before canonization.

Result: **72 LOCK / 18 ADOPT+MEASURE / 10 ASK**.

### Decision function

```c
typedef enum NfDecisionBasis {
    NF_DECISION_LOCK = 0,
    NF_DECISION_ADOPT_MEASURE,
    NF_DECISION_ASK
} NfDecisionBasis;

typedef struct NfDesignEvidence {
    float invariant_fit;
    float systemic_fit;
    float evidence_strength;
    float player_value;
    float implementation_cost;
    float regression_risk;
    float uncertainty;
} NfDesignEvidence;

static float nf_design_expected_value(NfDesignEvidence x)
{
    return 1.40f*x.invariant_fit
         + 1.10f*x.systemic_fit
         + 0.80f*x.evidence_strength
         + 1.00f*x.player_value
         - 0.55f*x.implementation_cost
         - 1.10f*x.regression_risk
         - 0.75f*x.uncertainty;
}
```

## A. Authority / simulation invariants

1. **Should authoritative simulation remain fixed at 60 Hz?**
   - **LOCK:** Yes; it is a durable engine invariant.

2. **Should client/player, Human Rival, and Dream Cattler action converge on NfControlFrame rather than separate movement APIs?**
   - **LOCK:** Yes; intention sources differ, execution authority does not.

3. **Should presentation ever own gameplay truth?**
   - **LOCK:** No; rendering/HUD observe authoritative simulation.

4. **Should server-authored outcomes remain distinct from actor intention?**
   - **LOCK:** Yes; actor selects strategy, world resolves consequence.

5. **Should stable entity IDs persist across perception, reservations, combat, and telemetry?**
   - **LOCK:** Yes; cross-system identity must be deterministic.

6. **Should hot-path simulation avoid per-tick heap allocation?**
   - **LOCK:** Yes; use fixed-capacity/bounded structures.

7. **Should world semantics be emitted from authoritative consequences instead of inferred by presentation/network timing?**
   - **LOCK:** Yes.

8. **Should player and AI share physical collision/traversal consequence rules?**
   - **LOCK:** Yes; no privileged AI traversal or damage.

9. **Should subsystem ownership remain separated: perception/utility/navigation/affordance/action/encounter/director?**
   - **LOCK:** Yes; avoid manager monoliths.

10. **Should deterministic seed/state be sufficient to reproduce simulation decisions that do not depend on external player timing?**
   - **ADOPT+MEASURE:** Yes; add reproducibility hashes where practical.

## B. Locomotion / affordance capture

11. **Should traversal candidate acquisition remain intention-sensitive rather than proximity-only?**
   - **LOCK:** Yes; direction/alignment must matter.

12. **Should affordance capture ever teleport the actor into position?**
   - **LOCK:** No; guidance is bounded acceleration/alignment.

13. **Should viable alternatives remain available while an affordance is being acquired?**
   - **LOCK:** Yes; only physical/logical impossibility hard-vetoes.

14. **Should counter-steer/jump-off/abort release non-mandatory capture?**
   - **LOCK:** Yes.

15. **Should environment state alter traversal feasibility and cost?**
   - **LOCK:** Yes; wetness, damage, obstruction, pressure, energy and occupancy may revalue actions.

16. **Should incoming momentum normally survive capture?**
   - **LOCK:** Yes, except where the physical relation truly constrains it.

17. **Should ladders use forgiving acquisition but a stronger physical constraint after committed attachment?**
   - **ADOPT+MEASURE:** Yes; soft acquire, constrained use, easy legitimate release.

18. **Should vault/mantle require compatible player/AI intent rather than trigger automatically from proximity?**
   - **LOCK:** Yes.

19. **Should capture acceleration/angular/position correction have explicit caps and telemetry?**
   - **ADOPT+MEASURE:** Yes; this closes the 'assist feels magnetic' risk.

20. **Should a dedicated slide/slide-hop locomotion verb become part of the core movement grammar now?**
   - **ASK:** Current architecture permits it, but it changes movement identity and needs a player-feel decision.

## C. Navigation / route architecture

21. **Should the 24-region graph remain strategic/coarse rather than becoming a high-resolution navigation mesh?**
   - **LOCK:** Yes; large world must not mean large per-agent thought.

22. **Should fixed geographic substrate remain independent from seeded navigation state?**
   - **LOCK:** Yes; place is canonical, local traversability can change.

23. **Should local navigation be derived from current collision/affordance truth rather than hard-coded cinematic routes?**
   - **LOCK:** Yes.

24. **Should navigation distinguish impossible, temporarily blocked, and merely expensive routes?**
   - **LOCK:** Yes.

25. **Should route valuation include distance plus exposure, congestion, risk, energy, objective and future-option terms?**
   - **ADOPT+MEASURE:** Yes; exact weights remain tuning data.

26. **Should the eventual 8 km² world use hierarchical routing: atlas/region -> local corridor -> immediate Fuzzy Rail?**
   - **ADOPT+MEASURE:** Yes; bounded hierarchy fits current architecture.

27. **Should dynamic obstacles update local route availability/cost without rebuilding the entire strategic graph?**
   - **LOCK:** Yes.

28. **Should route selection include hysteresis so tiny score changes do not thrash goals?**
   - **ADOPT+MEASURE:** Yes.

29. **Should navigation retain multiple near-best routes instead of deterministically selecting one whenever scores are close?**
   - **LOCK:** Yes; current top-region bounded variation already supports this principle.

30. **Should nightfall!punk add an explicit navmesh layer between region routing and Fuzzy Rail, or continue with collision/affordance-derived local navigation?**
   - **ASK:** Both are viable; this affects tooling, AI scale and map-authoring workflow.

## D. Occupancy / reservations / shared movement

31. **Should scarce affordances support soft claims/reservations?**
   - **LOCK:** Yes; ladders, narrow portals, cover, revive access and tasks benefit.

32. **Should claims always expire or be released on invalidation?**
   - **LOCK:** Yes; no accidental permanent ownership.

33. **Should reservations grant collision immunity or magical passage?**
   - **LOCK:** No; they coordinate intention, physics still resolves space.

34. **Should shared-movement arbitration use stable identity/priority rather than frame order?**
   - **LOCK:** Yes.

35. **Should near-future occupancy be estimated, not only present overlap?**
   - **ADOPT+MEASURE:** Yes; short-horizon prediction reduces doorway/spawn clumping.

36. **Should deadlock recovery be mandatory for reservation systems?**
   - **LOCK:** Yes; timeout/yield/replan.

37. **Should squad formations remain soft spatial preferences instead of rigid slots?**
   - **LOCK:** Yes.

38. **Should spawn/deployment logic explicitly penalize congestion and shared egress?**
   - **ADOPT+MEASURE:** Yes; this is the direct response to clumping.

39. **Should cover, revive access, traversal entry and interaction access share a general claim lifecycle where possible?**
   - **ADOPT+MEASURE:** Yes; one bounded claim primitive is preferable to bespoke locks.

40. **Should human players themselves participate in the reservation system as reserving actors, or should reservations remain AI-advisory around player motion?**
   - **ASK:** Player agency and griefing/collision semantics make both answers defensible.

## E. Perception / epistemic privacy

41. **Should server truth remain strictly separate from actor knowledge?**
   - **LOCK:** Yes.

42. **Should meaningful AI beliefs carry source/provenance, confidence and age?**
   - **LOCK:** Yes.

43. **Should vision, sound, report, public record, sensor and inference remain distinct evidence types?**
   - **LOCK:** Yes.

44. **Should confidence decay when evidence is not refreshed?**
   - **LOCK:** Yes.

45. **Should stale beliefs remain usable but less valuable rather than vanish instantly?**
   - **LOCK:** Yes.

46. **Should ordinary clients receive hidden AI field maps or exact ecological ledgers?**
   - **LOCK:** No.

47. **Should Ghost Static be allowed to inject exact hidden transforms into actor cognition?**
   - **LOCK:** No; it changes conditions/salience, not private knowledge.

48. **Should squad/pack reports remain coarser than firsthand perception?**
   - **LOCK:** Yes.

49. **Should uncertainty directly alter pursue/search/hold/withdraw utility?**
   - **LOCK:** Yes.

50. **How lossy should ordinary AI communication be: near-instant coarse reports, latency/noise by distance/infrastructure, or a stronger diegetic communications simulation?**
   - **ASK:** This affects stealth, coordination and systemic complexity.

## F. Tactical fields / game-theoretic spatial reasoning

51. **Should tactical influence fields bias decisions rather than veto actions except for true impossibility?**
   - **LOCK:** Yes.

52. **Should v0.9 add belief-derived energetic opportunity as another bounded spatial input rather than feeding omniscient nf_energy directly into AI?**
   - **LOCK:** Yes; this follows the v0.9 epistemic contract.

53. **Should current route congestion/ally support/enemy threat/exposure/pressure/objective fields remain actor-local rather than one global truth heatmap?**
   - **LOCK:** Yes.

54. **Should flank/pressure role signals remain payoff biases rather than scripted route assignments?**
   - **LOCK:** Yes.

55. **Should tactical candidate evaluation stay bounded to a small set of high-value regions/local samples?**
   - **LOCK:** Yes.

56. **Should badly wounded Human Rivals choose post-safety behavior from situation/future freedom rather than a magic retreat timer?**
   - **LOCK:** Yes; README already flags the timer solution as invalid.

57. **Should escape-route/future-option value become an explicit utility term?**
   - **ADOPT+MEASURE:** Yes; it operationalizes robustness over brittle optimization.

58. **Should resource and ecological opportunity influence combat positioning even when no objective marker is present?**
   - **ADOPT+MEASURE:** Yes; systemic arena topology should reflect the lifeworld.

59. **Should Rival and Cattler spatial weights remain policy-specific even when they consume shared field primitives?**
   - **LOCK:** Yes.

60. **Should the field architecture keep a fixed globally ordered priority list, or migrate to normalized actor/context-specific multi-factor utility while retaining hard safety bounds?**
   - **ASK:** The current ordered fields are simple and inspectable; contextual utility is richer but easier to destabilize.

## G. Energy / ecology / persistent lifeworld

61. **Should the universal energy ledger remain authoritative world truth but not a player score currency?**
   - **LOCK:** Yes.

62. **Should energy transfer conserve modeled stock and conversion record loss?**
   - **LOCK:** Yes.

63. **Should actor energy decisions use belief projections rather than direct current ledger access?**
   - **LOCK:** Yes.

64. **Should topography affect acquisition/transport cost rather than mint/destroy energy bookkeeping by itself?**
   - **LOCK:** Yes.

65. **Should regeneration be recorded as external input to the modeled local system?**
   - **LOCK:** Yes.

66. **Should depletion, infestation and environmental consequences persist into subsequent decisions?**
   - **LOCK:** Yes.

67. **Should energy be only one utility dimension rather than the universal motivational objective?**
   - **LOCK:** Yes; different actors pursue different strategies.

68. **Should future strategic freedom/option preservation be valued alongside immediate energetic return?**
   - **ADOPT+MEASURE:** Yes.

69. **Should the same regional energy opportunity be interpreted differently by Crew, Rival and Cattler policies?**
   - **LOCK:** Yes.

70. **What should be the primary player-facing representation of ecological/energy state: world objects and behavior only, subtle diegetic instrumentation, Ghost Static interpretation, or a mixture?**
   - **ASK:** The simulation contract is clear; the experiential communication layer is not.

## H. Arena topology / encounter geometry

71. **Should cover quality be graded by occlusion, exposure, exit options and occupancy rather than binary cover/no-cover?**
   - **ADOPT+MEASURE:** Yes.

72. **Should firing lanes be measured as dynamic relationships among visibility, weapon ranges, crossings and escape routes?**
   - **ADOPT+MEASURE:** Yes.

73. **Should major chokepoints generally have bypass, timing, traversal or counter-positioning options?**
   - **ADOPT+MEASURE:** Yes; avoid single-solution geometry.

74. **Should flanking routes be valuable because they change exposure/information/position, not because a designer labels them FLANK?**
   - **LOCK:** Yes.

75. **Should verticality be integrated into cost, visibility, traversal and energy rather than treated as decorative height?**
   - **LOCK:** Yes.

76. **Should spawn spaces be evaluated for protected egress, route diversity and congestion rather than protection alone?**
   - **ADOPT+MEASURE:** Yes.

77. **Should landmarks serve both player orientation and AI/region semantics without becoming mandatory waypoints?**
   - **LOCK:** Yes.

78. **Should passive encounters be able to become active through changed evidence/payoffs without a hard combat-mode switch?**
   - **LOCK:** Yes; C40 explicitly locks this.

79. **Should traversal/discovery/route mastery remain the primary positive-rush loop, with violence as pressure/interruption/resolution?**
   - **LOCK:** Yes; C37 explicitly locks this.

80. **What encounter-scale cadence should we optimize around inside the 15-minute possibility envelope: roughly 20–40 s micro-phrases, 45–90 s arcs, or strongly variable local rhythms?**
   - **ASK:** The macro pacing contract exists; the micro encounter cadence needs human feel.

## I. Pacing / Director / sustainable engagement

81. **Should pacing optimize long-horizon engagement rather than stimulation per minute?**
   - **LOCK:** Yes.

82. **Should high-intensity spikes require resolution/release/cooldown afterward?**
   - **LOCK:** Yes.

83. **Should Ghost Static operate through events, attention, commitments and salience rather than direct control input?**
   - **LOCK:** Yes.

84. **Should calm traversal, observation, maintenance and social ambiguity be valid stable states rather than dead time?**
   - **LOCK:** Yes.

85. **Should Director/event eligibility run at a slower cadence than tactical AI?**
   - **LOCK:** Yes.

86. **Should HUNT_SURGE compose/decompose gradually under local eligibility and mob-cap pressure rather than flip an abrupt global enum?**
   - **LOCK:** Yes; this is an explicit readiness watch.

87. **Should difficulty vary information, coordination, pressure, ecology and recovery rather than simply scale health/damage?**
   - **LOCK:** Yes; preserve multidimensional difficulty doctrine.

88. **Should event systems preserve the possibility that nothing spectacular happens when ordinary conditions do not support it?**
   - **LOCK:** Yes.

89. **Should pacing telemetry measure route repetition, encounter density, calm duration and intensity recovery in addition to kills/damage?**
   - **ADOPT+MEASURE:** Yes.

90. **Should spike intensity/duration have a hard global ceiling, or should the governor use context-sensitive ceilings based on recent player/crew pressure and recovery?**
   - **ASK:** A hard cap is predictable; contextual caps better fit the systemic governor.

## J. Networking / telemetry / scale

91. **Should clients continue to send intent rather than authoritative outcome?**
   - **LOCK:** Yes.

92. **Should 60 Hz simulation / 30 Hz snapshot publication remain the baseline until profiling disproves it?**
   - **LOCK:** Yes.

93. **Should hidden ecology/spatial fields remain server-side and be summarized only through legitimate game events/sense-data?**
   - **LOCK:** Yes.

94. **At production scale, which replication relevance model should be canonical: region-based, distance/LOS hybrid, or semantic relevance graph layered over regions?**
   - **ASK:** The current 16-actor lab cap is sufficient now, but 8 km² scaling makes this an architectural choice.

95. **Should every new high-degree systemic rule gain a deterministic test or trace before it is called implemented?**
   - **LOCK:** Yes.

96. **Should large-world scale use simulation LOD and bounded regional aggregation rather than full fidelity everywhere?**
   - **LOCK:** Yes.

97. **Should overlapping seed maps share fixed geographic truth while allowing different realization/history state?**
   - **LOCK:** Yes.

98. **Should spatial telemetry generate development-only heatmaps for congestion, exposure, route choice, deaths, resources and traversal failures?**
   - **ADOPT+MEASURE:** Yes; this closes statistical blind spots without exposing hidden truth to players.

99. **Should telemetry and multiplayer infrastructure follow data minimization/pseudonymity and keep real-player privacy outside the game-theoretic optimization loop?**
   - **LOCK:** Yes.

100. **What measurable acceptance gate should trigger scaling beyond the 0.40 km² / 24-region lab: route/AI stability, CPU budget, encounter diversity, navigation failure rate, human playtest quality, or a required bundle of all five?**
   - **ASK:** Scaling too early risks hiding architectural defects; this deserves an explicit user-owned threshold.

## PRAXIS — automatic implementation backlog from ADOPT+MEASURE

The following are approved engineering directions, but their constants/weights are tuning data and must be measured rather than treated as universal truth:

- **Q10:** reproducibility hashes for deterministic seed/state decisions.
- **Q17:** soft ladder acquisition, constrained committed use, legitimate release.
- **Q19:** explicit semantic-capture correction caps and telemetry.
- **Q25:** multi-factor route valuation including exposure, congestion, risk, energy, objective and future options.
- **Q26:** bounded hierarchical routing for the 8 km² target.
- **Q28:** route-selection hysteresis.
- **Q35:** short-horizon occupancy prediction.
- **Q38:** spawn/deployment congestion and egress pressure.
- **Q39:** generalized soft-claim lifecycle for scarce spatial affordances.
- **Q57:** explicit future-option / escape-route value.
- **Q58:** ecological/resource opportunity as combat-position context.
- **Q68:** future strategic freedom alongside immediate energy return.
- **Q71:** graded cover quality.
- **Q72:** dynamic firing-lane metrics.
- **Q73:** chokepoint counterplay metrics.
- **Q76:** spawn route-diversity/egress metrics.
- **Q89:** sustainable-engagement telemetry beyond damage/kills.
- **Q98:** development-only spatial heatmaps.

Recommended module ownership:

- `src/shared/nf_movement.*`: capture caps, momentum preservation, near-future occupancy hooks.
- `src/shared/nf_region.*` + `src/server/ai/nf_spatial.*`: hierarchical route valuation, hysteresis, escape/future-option value, cover/firing-lane metrics.
- `src/shared/nf_energy_actor.*`: belief-derived energetic opportunity; never direct omniscient actor access.
- `src/server/ai/nf_encounter.*`: spawn egress pressure, gradual intensity composition, sustainable-engagement telemetry.
- `src/shared/nf_protocol.*` / `nf_net.*`: later relevance/interest-management work after Q94 is answered.
- development-only diagnostics: congestion/exposure/route/death/resource/traversal-failure heatmaps.

## SIGNAL — what the audit says

The project is strongest where **world truth, bounded knowledge, spatial value, movement affordances and persistent ecological consequence** meet. The next high-value work is not another isolated AI trick: it is making these mature systems exchange information cleanly without collapsing their boundaries.

The most important current bridge is:

`bounded belief -> spatial/energy valuation -> several viable routes/actions -> authoritative physical consequence -> persistent world change -> new evidence`

## STEWARD — proof rules

- A LOCK is canonical design doctrine, not proof of current code implementation.
- ADOPT+MEASURE requires a deterministic trace/test plus human play evidence when player feel is involved.
- ASK items remain deliberately unresolved and must not be silently implemented by a later agent.
- Do not expose server/debug heatmaps, exact ecological ledgers or hidden beliefs as ordinary player truth.
- Do not scale the world merely because the architecture can allocate more state; scale only after Q100 has an acceptance gate.
- Preserve the fixed geographic substrate / seeded realization / earned history separation.
- Preserve C34: crisp intention, non-exclusive affordance capture, physical consequence.
- Preserve C35: world truth is not common knowledge.
- Preserve C37/C40: traversal is the positive rush; horror/violence alter strategy space rather than forcing a combat switch.

## USER DECISION QUEUE — 10 real forks

- **Q20: Should a dedicated slide/slide-hop locomotion verb become part of the core movement grammar now?**
  - Current recommendation context: defer unless it clearly improves the Neo-Xennial movement identity rather than merely importing Titanfall vocabulary.
- **Q30: Should nightfall!punk add an explicit navmesh layer between region routing and Fuzzy Rail, or continue with collision/affordance-derived local navigation?**
  - Current recommendation context: favor the current region + affordance/Fuzzy-Rail architecture until authoring or AI scale demonstrates a concrete navmesh need.
- **Q40: Should human players themselves participate in the reservation system as reserving actors, or should reservations remain AI-advisory around player motion?**
  - Current recommendation context: favor AI respecting/predicting player occupancy without player reservations constraining agency.
- **Q50: How lossy should ordinary AI communication be?**
  - Current recommendation context: start with coarse reports plus modest latency/confidence decay; add infrastructure-dependent communications only if stealth/systemic play benefits.
- **Q60: Fixed ordered spatial priorities or actor/context-specific normalized utility?**
  - Current recommendation context: retain inspectable hard safety bounds, but migrate soft preferences toward contextual multi-factor utility incrementally.
- **Q70: How should players perceive ecological/energy state?**
  - Current recommendation context: mixture of world behavior/objects + subtle diegetic instrumentation + occasional Ghost Static interpretation; avoid numeric ecology HUD scores.
- **Q80: What micro encounter cadence should sit inside the 15-minute envelope?**
  - Current recommendation context: strongly variable local rhythm, with many 20–90 second phrases but no universal encounter clock.
- **Q90: Hard spike ceiling or context-sensitive ceiling?**
  - Current recommendation context: context-sensitive ceiling bounded by a hard safety maximum.
- **Q94: Which production replication relevance model?**
  - Current recommendation context: region-based coarse relevance with distance/LOS/semantic refinement, rather than one criterion alone.
- **Q100: What acceptance gate triggers scaling beyond the 0.40 km² lab?**
  - Current recommendation context: require a bundle—stable route/AI behavior, acceptable CPU budget, low navigation failure, demonstrated encounter diversity, and human playtest approval.

Until answered, these ten forks remain open; all other 90 decisions in this audit may be treated as canonical or provisionally adopted according to their status.
