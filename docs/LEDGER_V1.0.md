# nightfall!punk — v1.0 Topographic Lifeworld ledger

This file is the **current superseding implementation/readiness ledger** for v1.0. `docs/LEDGER.md` remains the cumulative historical design ledger through the v0.9 planning pass; where an older unchecked or failed item conflicts with this file, this file governs current readiness.

Versioning remains flat: one coherent contract per v0.X / v1.X version; accepted versions are archived before `main` advances.

## Governing thesis

> **Do not simulate ecology as numbers that occasionally produce life. Simulate life whose continuing relations can be accounted for ecologically.**

> **Give the world enough lawful structure to select consequences, and enough minimal difference that it never has to repeat the same history.**

Topographic biopower remains a relation rather than a master scalar: the situated capacity of an actor or ecology to sustain, reproduce and project life-activity through a particular material landscape.

## Dependency / discrepancy disposition

### v0.8 Dream Cattler Habitat Ecology — repaired, human gate still open

- [x] PR #11 remains draft/open and mergeable; no archive/merge performed.
- [x] previous human recording correctly established a real weak-point truth failure: visible lower-body shots did not change health/locomotor truth.
- [x] shared `nf_hitbox` geometry now owns both yellow weak-point presentation dimensions and authoritative Cattler ray classification.
- [x] visible FOOT/KNEE bands and authoritative FOOT/KNEE volumes use the same widths/heights.
- [x] server hitscan now routes Cattler shots through the shared classifier.
- [x] deterministic test proves visible-band ray -> `NF_HIT_FOOT` / `NF_HIT_KNEE` -> authoritative damage event -> `nf_cattler_on_combat_event()` -> `locomotor_integrity < 1.0`.
- [x] strict v0.8 exact-head GitHub Actions is green at `3448e3b26165d94943e6527eee4a93a13886c119`.
- [ ] human Pop!_OS confirmation of the repaired lower-body truth remains required before v0.8 merge/archive.
- [ ] explicit ENTRENCH terminal behavior remains future work.
- [ ] Hunt Surge remains abrupt; C2 gradual composition/decomposition is not represented yet.
- [ ] ~500 m² Cattler carrying/mob-cap concept remains unimplemented and distinct from the ~800 m² v1.0 landbase proof window.

### v0.9 Topographical Energy Accounting — behavior accepted, dependency conflict open

- [x] v0.9 human recording / energy proof remains accepted on its own evidence.
- [x] C20–C22 universal accounting, topographical cost, bounded helicopter search and energy smoke remain preserved in v1.0.
- [ ] PR #12 is currently merge-conflicted because its base recorded the pre-repair v0.8 head while v0.8 has since advanced to the repaired head.
- [ ] reconcile v0.9 only after v0.8 human acceptance; do not bypass the flat archive order.
- [x] a `reconcile/v0.9-after-v0.8` scaffold exists only as a reconciliation staging point; it is not itself a completed v0.9 replacement.

### v1.0 Topographic Lifeworld

- [x] branch: `build/v1.0-topographic-lifeworld`.
- [x] draft PR #13 is stacked on v0.9 and remains open/mergeable.
- [x] exact code head `2bec566053b0a7497c6521f96f4e4037fcfc9695` passed full compile, all tests and all smoke gates before this docs-only ledger commit.
- [ ] after this docs-only commit, require one final exact-head CI green before declaring the candidate compile gate closed.
- [ ] do not merge/archive v1.0 before dependency closure and human proof.

# Q1–Q10 locked contracts

## Q1 — substantial landbase

> **A landbase is where enough cycles depend on one another that disturbing one local process can reorganize the others.**

- [x] ~800 m² is an embodied graybox observation scale, not an ontological boundary.
- [x] actual boundary is environmental interdependence among material/ecological/social/player-accessible flows and recurring activity.
- [x] require metabolism, multiple situated accounts, inflow/outflow, internal topographic heterogeneity, recurrent feedback, player-perceptible and interruptible expression.
- [x] physical energy flows/degrades; material/nutrient cycles and account balances may recur. Do not imply perpetual physical energy cycling.

## Q2 — legitimate energetic knowledge

- [x] baseline = direct experience plus meaningful inference from traffic, traces, other actors and reports.
- [x] stronger ecological inference is selective by role/experience/evidence.
- [x] actor cognition may never query mutable authoritative energy truth.

## Q3 — one currency, many forms of life

- [x] shared universal energy opportunity -> actor belief -> actor-local topography -> distinct Human/Cattler conversion grammar.
- [x] Human and Cattler conclusions may differ because embodiment, access, exposure, social relation, habitat fit, history and knowledge differ.

## Q4 — first evental transition: contested corner

> **The corner becomes an event when occupying it changes how everyone else can live through the surrounding space.**

- [x] two Cattlers establish recurring local occupation.
- [x] Rival pressure should arise from legitimate perception/route/threat systems rather than an encounter script.
- [x] Rival attack geometry should pull Cattler attention from one approach and make an opposite player flank newly attractive.
- [x] outcome remains open: hold / withdraw / entrench / displacement.

## Q5 — infestation waterline

> **The darkness is not the infestation. The darkness is what infestation does to the perceptible lifeworld.**

- [x] Cattler infestation visibly darkens occupied space.
- [x] presentation is topographical/environmental, not a radius sphere or generic enemy aura.
- [x] darkness is secondary presentation derived from causal occupation/history; it is not authoritative ecology truth.
- [x] client may smoothly predict this visible state from replicated physical evidence.
- [x] recovery should leave visible residue rather than snap to normal.

## Q6 — persistence under pressure

- [x] local structural viability, not health total alone, determines HOLD / WITHDRAW / ENTRENCH / DISPLACEMENT.
- [x] defeat means inability to reproduce a form of life in that place, not automatic erasure.

## Q7 — reclamation timescales

> **Recovery begins at once; return is earned through recurrence.**

- [x] response begins immediately after displacement.
- [x] visible/behavioral recovery may take 45+ seconds.
- [x] deeper recovery may require multiple rounds through persistent datakeeping.
- [x] displacement != reclamation != recovery.
- [x] recovery depends on renewed use/maintenance/conditions, not elapsed time alone.

## Q8 — primary vs secondary data

> **Save causes, reconstruct effects. Save history's difference, not the whole world.**

- [x] primary persistence = tiny exact quantized causal data.
- [x] static seeded facts are not redundantly persisted.
- [x] secondary state = disposable approximation for prediction/rendering and may be corrected.
- [x] server-private actor knowledge is not replicated as public truth.
- [x] predict appearances/continuity, never privileged truth.

Current exact landbase primary core:

```c
typedef struct NfLandbaseMemory {
    uint16_t infestation_q;
    uint16_t human_use_q;
    uint16_t maintenance_q;
    uint16_t disturbance_q;
    uint64_t last_cattler_tick;
    uint64_t last_human_tick;
} NfLandbaseMemory;
```

Primary JSON deliberately contains schema/seed/tick/round/region, these structural memories and process-object identity/condition/flags. It deliberately excludes darkness, traffic, hotspot labels and client prediction state.

## Q9 — mobile epistemic hotspots

> **A hotspot belongs to the relation between a changing world and an uncertain observer, not to a coordinate.**

- [x] hotspots are derived/mobile, never permanent map nodes.
- [x] first families: contested occupation, changing access, recovery/succession edge.
- [x] hotspot requires consequential change + incomplete/stale/conflicting knowledge + obtainable evidence + action divergence.
- [x] unexpected absence / expectation violation may later create a hotspot.
- [x] information seeking must remain embodied; actors do not query secrets.

## Q10 — attention through minimal difference + ecological selection

> **Randomness proposes; situated selection disposes.**

- [x] uniform random choice is a blunt minimal-difference/exploration mechanism among near-equivalent candidates.
- [x] random influence falls as necessity/consequence becomes decisive.
- [x] proximity alone is insufficient.
- [x] cooperative coverage can distribute the cost of uncertainty.
- [x] competitors' visible behavior can itself become evidence.
- [x] technical formulation = ecological selection among cooperative and competitive strategies; competition is not the sole ecological organizer.
- [x] selection acts on situated behavior/strategy, not fixed personality classes or literal genetics at encounter timescale.
- [x] no opaque reinforcement-learning system or convergent global optimizer.
- [ ] persistent short situated success/failure memory beyond current bounded attention coverage remains future work.

# v1.0 EXISTING — regression-protected checklist

- [x] fixed 60 Hz authoritative simulation.
- [x] dedicated server truth + client prediction/reconciliation.
- [x] ENet network abstraction and reconnect/session scaffolding.
- [x] movement/Fuzzy Rail step/vault/mantle/ladder/local physical traversal.
- [x] authoritative carbine/pistol combat, reload, rewind hitscan, damage, death and respawn.
- [x] faction/relationship damage policy.
- [x] Human Rival limited perception, memory/confidence, semantic hearing, utility/cover and ordinary `NfControlFrame` control.
- [x] bounded Rival encounter pressure / contextual pressure-flank-hold behavior.
- [x] 24-region / 0.40 km² spatial ecology and bounded next-hop reasoning.
- [x] Dream Cattler distinct AI family, habitat logic, imperfect pack reports, physical infestation, modes and ecology ledger.
- [x] universal topographical `NfEnergySystem`, historical regional metabolism, accounting invariants and helicopter ranking.
- [x] F4 spatial diagnostics.
- [x] repaired Cattler lower-body weak-point authoritative truth boundary.

# v1.0 ADVANCING — implemented in this candidate

## Situated epistemics

- [x] new actor-safe energy facade in `nf_energy_actor.*`.
- [x] actor belief initialization uses seeded/static prior information rather than mutable current stocks.
- [x] actor evidence update accepts an estimated return/confidence and **does not accept `NfEnergySystem` truth**.
- [x] actor belief search constructs opportunity returns from belief and static topographical pricing.
- [x] deterministic test mutates hidden authoritative energy state and proves actor search remains unchanged until legitimate evidence is supplied.
- [x] Human/Cattler situated valuation wrapper produces different local interpretations of the same belief-derived opportunity.

## Bounded landbase / metabolism

- [x] first ~40m x 20m (~800 m²) embodied landbase proof window.
- [x] generator process object + water/pump process object.
- [x] generator/pump throughput depends on actual condition/access and each other rather than decorative animation alone.
- [x] infestation/disturbance degrade access and can damage process condition.
- [x] human recurrence without Cattlers increases maintenance and repairs process condition.
- [x] exact quantized `NfLandbaseMemory` records only irreducible structural causes.
- [x] deterministic primary JSON writer produces byte-identical causal state for identical inputs.
- [x] test proves primary JSON excludes `darkness`, `traffic` and hotspot/prediction labels.

## Contested corner / waterline

- [x] first two Cattlers are seeded into the landbase as initial ecological conditions, without hidden target knowledge.
- [x] existing Cattler/Rival perception and combat remain responsible for actual physical conflict.
- [x] local Cattler occupation accumulates infestation structural memory.
- [x] two-Cattler / Human co-presence raises disturbance and derives a contested-occupation epistemic hotspot.
- [x] client predicts topographical environmental darkening only from replicated Cattler position/presence.
- [x] darkening is rendered as a landbase stain/geometry-following proxy, not a Cattler-centered sphere.
- [x] visible darkening rises faster during occupation and recedes slowly after displacement (~45+ second visual timescale from mature occupation).
- [x] generator/pump receive first graybox visible process expression.

## Hotspots / attention

- [x] mobile hotspot families: `CONTESTED_OCCUPATION`, `ACCESS_CHANGE`, `RECOVERY_EDGE`.
- [x] bounded attention considers consequence, uncertainty, evidence change and proximity.
- [x] already-covered hotspots receive a cooperative coverage penalty, reducing redundant scouting pressure.
- [x] uniform deterministic die may choose the second near-equivalent candidate as minimal exploratory difference.
- [x] attention emits bounded diagnostic life activities: TRAVEL / WORK / MAINTAIN / OCCUPY.
- [x] server logs `[attention]` decisions for human verification.

## Recovery / persistence proof

- [x] Cattler absence begins structural recovery at the next bounded lifeworld update rather than a long hard-coded delay.
- [x] maintenance can accelerate recovery once Human reuse becomes substantial.
- [x] deterministic test proves mature contested infestation remains nonzero after 45 seconds of recovery while visibly improving.
- [x] recovery edge becomes an epistemic hotspot while residual infestation and Human reuse coexist.
- [x] secondary surface derives darkness/traffic/infrastructure/disturbance/recovery from primary causal state.

## Build / diagnostics

- [x] CMake advanced to v1.0 and includes `nf_energy_actor`, `nf_hitbox`, `nf_lifeworld`.
- [x] seven CTests: simulation, combat, network, Cattler ecology, topographical energy, weak-point truth, topographic lifeworld.
- [x] `nightfall.sh lifeworld-smoke` added.
- [x] v1.0 CI runs full graphical compile plus all existing and advancing smoke gates.
- [x] server emits `[lifeworld]`, `[attention]`, `[primary]` diagnostics.

# ADVANCING — intentionally incomplete / next evidence-dependent work

- [ ] actual JSON **load/restore** across round boundaries is not implemented yet; v1.0 currently proves the minimal exact serialization contract and in-process historical continuity.
- [ ] the project does not yet have a full round-boundary persistence pipeline to exercise multi-round recovery; add only after this local proof is accepted.
- [ ] lifeworld attention currently diagnoses selected hotspot/life activity but does not override the existing Rival `NfControlFrame` routing controller; the first human proof should tell us where the smallest safe integration hook belongs.
- [ ] TRAVEL/WORK/MAINTAIN/OCCUPY are represented in lifeworld attention/process consequences but are not yet a complete Human labor simulation.
- [ ] generator/pump are causal graybox process endpoints but do not yet expose a full player interaction/repair puzzle contract.
- [ ] local `NfLandbaseMemory.infestation_q` overlaps conceptually with the older region-level Cattler infestation system. Keep this v1.0 local structural proof bounded; consolidate only after footage establishes which history belongs at which scale.
- [ ] C26 eventality can be inferred from the contested-corner causal chain but there is no general event/regime engine.
- [ ] C27–C30 remain derived design/test contracts; no resilience, hysteresis, succession or regime master scalar/state machine has been added.
- [ ] Q10 short situated success/failure memory remains future work; current mechanism covers cooperation, consequence, proximity and minimal random exploration only.
- [ ] infestation waterline is still graybox rendering; final material/light/audio ecology is deliberately deferred.

# PAR — deliberate non-goals / complexity guard

- [x] no giant global GOAP/economic planner.
- [x] no omniscient actor utility map.
- [x] no raw energy/infestation/ecology HUD meter as primary communication.
- [x] no universal trace TTL.
- [x] no single resilience/hysteresis/eventality/regime scalar as authoritative truth.
- [x] no scripted succession ladder.
- [x] no final fauna, final Cattler art, final infestation shader or final ecological audio pass.
- [x] no full Ghost Static economy/director layer.
- [x] no 8 km² expansion.
- [x] no literal genetic evolutionary simulation or opaque RL.
- [x] no client access to private actor beliefs / helicopter truth.
- [x] no merge/archive of v0.8, v0.9 or v1.0 before their dependency/human gates.

# v1.0 compile / verification gate

Pre-ledger-code head `2bec566053b0a7497c6521f96f4e4037fcfc9695`:

- [x] standard-check green.
- [x] strict full graphical compile green.
- [x] all 7/7 CTests green.
- [x] combat/network regression smoke green.
- [x] bounded-pressure Human Rival encounter smoke green.
- [x] situated Rival spatial smoke green.
- [x] Dream Cattler habitat/pack smoke green.
- [x] topographical energy smoke green.
- [x] topographic lifeworld/epistemic-boundary smoke green.
- [ ] final exact-head GitHub Actions after this docs-only ledger commit.

# v1.0 human proof checklist — next Pop!_OS recording

Target roughly **3–5 minutes**. Do not try to force a scripted sequence; divergence is useful evidence.

- [ ] client reaches `AUTHORITATIVE + PREDICTED` play normally.
- [ ] ordinary movement, traversal, weapon switching, reload and combat remain stable.
- [ ] briefly shoot a visible yellow Cattler knee/foot band and confirm the repaired hit truth behaves materially; this is the final v0.8 acceptance check.
- [ ] observe the two initially concentrated Cattlers and whether they remain/recur around the darkened landbase long enough to read as occupation rather than mere spawn coincidence.
- [ ] observe whether Human Rivals legitimately detect/engage Cattlers from another approach through existing perception/combat.
- [ ] approach from another side and attempt the player flank; no objective marker should be required.
- [ ] note whether Rival pressure visibly redirects Cattler attention and makes the flank tactically meaningful.
- [ ] if one/both Cattlers are displaced, remain nearby or return over **45–60+ seconds** and check that the area recovers gradually rather than snapping clean.
- [ ] inspect generator/pump graybox expression and whether infestation darkening reads as environmental occupation rather than a generic enemy aura.
- [ ] briefly toggle F4 once for spatial context.
- [ ] say the quiet part aloud if anything feels extremely right or extremely wrong.

After recording, capture:

```bash
./nightfall.sh energy-smoke
./nightfall.sh lifeworld-smoke
tail -n 300 build/server.log
cat build/lifeworld-smoke.log
```

Acceptance thesis:

> **A small interdependent place can become visibly occupied, contested and reorganized through autonomous Cattler/Rival/player activity; actors remain epistemically situated; the client may predict the environmental waterline without receiving hidden truth; displacement begins an immediate but historically persistent recovery rather than resetting the place.**

# Questions / study subroutine for post-video reassessment

Use the existing discipline:

`WATCH/READ -> QUESTION -> COMPARE CURRENT CODE -> KEEP/ADAPT/DEFER/REJECT -> CODING CONTRACT -> COMPILE -> HUMAN VIDEO -> REASSESS`

Before the next architectural addition ask:

1. Did the two-Cattler corner **actually become a place** or merely look like two enemies standing near each other?
2. Did the Rivals' approach arise legibly enough that the player's opposite-side flank felt discovered rather than staged?
3. Did darkening communicate Cattler occupation without behaving like an arbitrary aura?
4. Which information changed actor/player decisions, and which diagnostics were merely noise?
5. Did 45+ second recovery feel like history, or merely slow interpolation?
6. What is the smallest safe hook from `NfAttentionChoice` into existing spatial/Rival control without creating a parallel planner?
7. Which piece of local `NfLandbaseMemory` belongs in region-level ecology, which belongs in a landbase, and which can be reconstructed?
8. Is generator/pump causality readable enough to justify player manipulation next, or should traffic/work first become more embodied?
9. Did uniform minimal-difference exploration create useful variation without irrational behavior?
10. What can be deleted or merged now that the code has produced evidence?
