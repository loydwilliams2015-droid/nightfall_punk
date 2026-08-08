# nightfall!punk master ledger

Versioning rule: **one flat version = one coherent development contract.** Git commits carry fine-grained history. Accepted versions are preserved as immutable `archive/v0.X` branches before `main` advances. No nested subversions.

## Locked project decisions

- [x] C + raylib; Linux-first
- [x] fixed 60 Hz simulation; authoritative dedicated server
- [x] server truth + client prediction/reconciliation
- [x] four-player target for now; possible five later
- [x] round-based story-oriented systemic arena FPS
- [x] graybox mechanics / architecture before original aesthetics
- [x] direct confrontation remains the gameplay emphasis
- [x] physical puzzle/mechanism play later creates route/reward advantages under combat pressure
- [x] **Fuzzy Rail** = shared local geometric traversal intelligence, never global strategic pathing
- [x] **Dynamic Affordance Graph** = utility + bounded affordance queries + spatial fields + blackboards; selective planning only when earned
- [x] **World Semantic Alerts** = server-authored gameplay meaning above packet semantics; AI never infers critical truth from packet arrival/loss
- [x] epistemic contract: `server truth != individual knowledge != squad report`
- [x] faction identity is distinct from relationship state
- [x] Human Rivals = secondary antagonism / temporary terrain antagonism / negotiable
- [x] Dream Cattlers = primary ecological antagonism / semi-permanent environmental antagonism / always non-negotiable
- [x] Dream Cattler `predatory` and `non-negotiable` are independent axes; rare hostile/non-predatory disposition may permit avoidant detente without diplomacy
- [x] existing technical `Rancher` identifiers may remain until a coordinated rename; player-facing term is **Dream Cattler**
- [x] ecological score topology: Player Crew + teammates contend with Dream Cattlers over habitat/ecology; Player Crew and Rival Crews compete over ecological resources
- [x] score-play eventually measures control, preservation, extraction, access, denial and resource contest as well as direct combat
- [x] player/rival factions use carefully adapted verified-public-domain literary `[X] Crew` naming
- [x] crew-member grammar = public-domain literary given-name variation + naturalized Old English-derived modern surname
- [x] public-domain provenance and trademark clearance are separate shipping gates
- [x] formal genre identity = **Neo-Xennial Shooter / Neo-Xennial FPS**
- [x] subtype = **Systemic Arena FPS**
- [x] genre thesis = Quake immediacy + Deus Ex systemic possibility + Halo spatial clarity + F.E.A.R. encounter intelligence + contemporary simulation/network infrastructure
- [x] manifesto = **The neo-xennial shooter continues the FPS future that the early 2000s started: fast, physical, systemic, open, and unpredictable.**
- [x] Ghost Static / Apophis replaces Dragon Master lore: Ghost Static is the public pirate-radio arena-conductor identity; Apophis is the deeper mythic/signal layer
- [x] aesthetic target = PG-13 ambient/paranormal/psychological cyberpunk horror, not splatter/gore-centric horror
- [x] visual thesis = objective realism haunted by violence, with comic-book comic relief as quiet understatement
- [x] aesthetic engine remains a later boxed-off phase so presentation does not rewrite movement/network/combat/AI truth

## Version / archive status

- [x] **v0.1 Foundation** — archived at `archive/v0.1`
- [x] **v0.2 Movement** — archived at `archive/v0.2`
- [x] **v0.3 Networked Movement** — archived at `archive/v0.3`
- [x] **v0.4 Combat** — archived at `archive/v0.4`
- [x] **v0.5 Agent Intelligence Framework** — archived at `archive/v0.5`
- [x] **v0.6 Encounter Intelligence** — code/CI baseline merged to `main`
- [ ] **v0.6 archive remains intentionally pending** because its Pop!_OS human acceptance recording was not separately closed before v0.7 work began
- [x] **v0.7 Spatial Ecology / Situated Agency** — active candidate on PR #10 / `build/v0.7-spatial-ecology`
- [ ] do not merge v0.7 or create `archive/v0.6` / `archive/v0.7` until the next human Pop!_OS demo is reviewed

# v0.6 — Encounter Intelligence — CARRY-FORWARD BASELINE

The v0.6 contract remains regression-protected under v0.7:

- [x] movement + Fuzzy Rail remain the only physical traversal truth
- [x] ENet / server authority / prediction/reconciliation remain intact
- [x] weapon/ammo/hitscan/damage/death/respawn remain the only combat truth
- [x] perception / imperfect memory / semantic hearing / utility / cover affordances remain live
- [x] finite yaw/pitch tracking + aim-settle remain live
- [x] default soft pressure budget remains two Rivals
- [x] contextual PRESSURE / FLANK_LEFT / FLANK_RIGHT / HOLD rebidding remains live
- [x] damage-driven suppression / retreat remains live
- [x] AI still emits ordinary `NfControlFrame` intent and cannot teleport, manufacture damage or bypass ammo/reload/collision
- [x] AI/encounter code remains free of raylib/ENet dependencies
- [x] strict project warnings remain `-Wall -Wextra -Wpedantic -Werror`

# v0.7 — Spatial Ecology / Situated Agency — ACTIVE CANDIDATE

## Contract thesis

**Agents can traverse the whole arena, choose meaningful regions, temporarily inhabit local tasks, coordinate occupation with their squad, remember recent spatial history, and respond to authoritative events without continuously solving the entire world.**

The hierarchy is:

`region chooses context -> local task chooses purpose -> action chooses behavior -> Fuzzy Rail handles physical execution -> server validates truth`

Global mobility does not mean global continuous thought. **Large world != large per-agent thought.**

## Existing / regression protection

- [x] v0.1 fixed simulation and headless/graphical split preserved
- [x] v0.2 movement/Fuzzy Rail still owns step/vault/mantle/ladder/physical traversal
- [x] v0.3 networking still owns validated client intent, snapshots and reconciliation
- [x] v0.4 combat authority unchanged
- [x] v0.5 epistemic AI / utility / local cover architecture retained
- [x] v0.6 encounter-governance layer remains downstream of raw AI intent
- [x] spatial intelligence is server-side tactical scratchwork; normal clients receive consequences, not complete AI field state
- [x] no whole-map high-resolution influence grid
- [x] no general GOAP introduced

## A1 — spatial-field priority

Default local spatial preference order is locked:

1. **route congestion**
2. **ally support**
3. **enemy threat**
4. **exposure**
5. **pressure**
6. **objective value**

- [x] all six fields exist in `NfSpatialFields`
- [x] field values are bounded `[0,1]`
- [x] current local preference score weights those fields in the locked descending order
- [ ] if footage shows lower-priority fields overturning tactically obvious congestion/support decisions, promote the score from ordered weighted refinement to a quantized lexicographic/tier comparison
- [x] objectives select the **problem**; spatial intelligence selects a sensible way to solve it

## A2 — representation

- [x] hybrid representation = **coarse region graph + bounded local samples**
- [x] shared `nf_region` module defines deterministic graph topology usable by server logic and client diagnostics
- [x] v0.7 lab graph contains **24 coarse regions**
- [x] local tactical selection considers **5 bounded samples** around the active region/next-hop center
- [x] global route selection and local movement remain separate from Fuzzy Rail

## A3 — temporal / bandwidth model

- [x] event-driven semantic changes feed activity / incident / objective-dynamic memory
- [x] region selection runs at a slow bounded cadence (~2 Hz baseline)
- [x] local sampling runs at ~5 Hz staggered cadence
- [x] spatial activity/incident fields decay at bounded sub-rate
- [x] structural exposure is cached on regions and refined locally against known threat LOS
- [x] tactical spatial state is not replicated as ordinary snapshot state
- [x] client F4 overlay draws shared region topology only; authoritative per-agent task/field reasoning remains in server diagnostics for this build

## A4 — bias vs veto

- [x] spatial fields **bias** choices by default
- [x] hard vetoes are reserved for physical/logical impossibilities: invalid/unreachable region, occupied exclusive affordance, relationship/combat prohibition, dead/unavailable actor, etc.
- [x] high threat or low objective value alone never makes a physically valid region impossible

## A5–A9 — global wandering / local habitation / individuality

- [x] agents are globally mobile through the region graph
- [x] agents pick a region, then adopt a bounded local task to **inhabit** that situation
- [x] local-first movement escalates through region routing when relocation is required
- [x] flank/objective-style relocation may consult regional context proactively
- [x] local task families implemented: ROAM / WATCH / INVESTIGATE / OCCUPY / PRESSURE / FLANK / REGROUP / CONTEST
- [x] task commitment/hysteresis prevents 2% utility differences from causing constant thrash
- [x] low-activity residency eventually loses value through novelty / visitation logic
- [x] region choice considers novelty, semantic activity, distribution, role relation, objective value, congestion, incident cost and travel steps
- [x] recent visits are remembered per agent
- [x] top regional candidates are bounded to four deep candidates; deterministic seeded selection prevents all agents collapsing onto one exact optimum
- [x] modest seeded personality envelopes (~0.90–1.10) vary support/risk/residency tendency without overriding faction policy or server truth
- [x] Human Rival policy uses temporary tactical residency
- [ ] teammate residency policy remains later
- [ ] Dream Cattler habitat persistence/infestation policy remains later and must not be a renamed Rival policy

## A10–A12 — claims / memory / bounded adaptation

- [x] existing cover nodes retain hard exclusive reservation
- [x] local tactical tasks use soft claim pressure so duplicate WATCH/OCCUPY work is discouraged rather than globally forbidden
- [x] short tactical target memory remains in v0.5 `NfAiKnowledge`
- [x] individual regional visitation memory lives in `NfSpatialAgentState`
- [x] slower crew-level regional incident memory lives in `NfSpatialSystem`
- [x] damage/death activity raises temporary incident cost
- [x] incident memory decays, preventing permanent cowardice / map blacklisting
- [x] respawn clears individual spatial visitation/task state while retaining shared crew-level incident history
- [ ] no cross-match persistent adaptation / invisible ML counterstrategy

## A13 — interruption hierarchy

Interrupt severities are intentionally small and generic:

`NONE -> REEVALUATE -> TASK_BREAK -> EMERGENCY -> AUTHORITATIVE`

- [x] route invalidation may force AUTHORITATIVE interruption
- [x] direct damage may force EMERGENCY interruption
- [x] nearby gunfire/death/suspicious sound may trigger REEVALUATE
- [x] objective/story phase change may trigger TASK_BREAK
- [x] interruption means **reconsider now**, not “execute this hidden script,” except where physics/server truth leaves no choice
- [x] semantic vocabulary now includes `OBJECTIVE_CHANGED`, `ROUTE_CHANGED`, `STORY_PHASE_CHANGED` for future authored producers

## A14 — story authority

Story-mode authoring contract is locked even though producers are not yet live:

1. **CONTEXT** — normal: raise/lower relevance of region/objective/person
2. **CONSTRAINT** — occasional: actor must reach/remain/defend a broad region
3. **DIRECT ACTION** — rare synchronization/cinematic escape hatch

- [x] v0.7 interruption semantics are compatible with the three-level model
- [ ] no story campaign logic is added merely to exercise the hook

## A15 — diagnostics

- [x] server diagnostics now report region current->target, next hop, local task, interrupt severity and six field values + aggregate preference score
- [x] graphical client title/footer identify v0.7 Spatial Ecology / Situated Agency
- [x] **F4** toggles a visual 24-region graph overlay in the graybox client
- [x] HUD shows local player region/name, graph count and 0.40 km² lab scale
- [ ] per-Rival task/field values are deliberately not network-replicated into the client in v0.7; use `build/server.log` for authoritative reasoning traces

## A16 — determinism

- [x] region selection / personality variation / tie variation derive from authoritative seed + actor id + bounded epoch
- [x] no new uncontrolled gameplay `rand()` path
- [x] same relevant state + seed should produce reproducible spatial decisions suitable for bug reproduction

## A17 — computational boundaries

- [x] 24 regions in this demo; architecture supports a larger bounded graph later
- [x] 5 local candidate samples per local query
- [x] top 4 regional candidates retained for deep choice
- [x] region path search is bounded by `NF_REGION_MAX`
- [x] region decisions ~2 Hz, local samples ~5 Hz, event-driven semantic impulses, simulation-rate physical execution
- [x] fixed-capacity hot-path state; no per-tick allocation
- [x] cover-affordance capacity expanded from 32 to 96 to remain locally useful across the larger graybox

## A18 — v0.7 map / milestone scale

Ultimate map-area target remains **8 km²**. v0.7 deliberately tests only **5%** of that area:

- [x] demo area = **0.40 km² / 400,000 m²**
- [x] graybox dimensions = **500 m x 800 m**
- [x] sparse connective territory is intentional so roaming/relocation can be observed rather than every square meter becoming a combat toy
- [x] map includes distributed solid structures, local cover objects, vertical landmarks, ladders, ramps and a moving platform while retaining open regional corridors

## Build pipeline / implementation order

Current authoritative flow:

`semantic/world state -> spatial region/task update -> v0.5 AI control intent -> spatial situated movement filter -> v0.6 encounter governance -> shared movement/combat -> server truth`

The spatial filter only supplies situated movement when ordinary tactical AI is free to roam/watch; live combat/investigation/cover/retreat modes retain their existing authority.

## Automated contracts / compile gate

- [x] CMake project version advanced to 0.7
- [x] shared `nf_region.c/.h` added
- [x] server AI `nf_spatial.c/.h` added
- [x] situated control adapter `nf_spatial_filter.c` added
- [x] 0.40 km² graybox integrated through the existing shared world builder so client prediction and server collision see identical geometry
- [x] 24-region graph connectivity/path contracts added
- [x] spatial field bounds/order contract added
- [x] deterministic roaming/control contract added
- [x] existing simulation/combat/network/encounter contracts retained
- [x] first v0.7 PR CI attempt: strict full graphical compile green; new spatial test exposed a real central-spawn geometry collision and failed intentionally
- [x] central spawn lanes subsequently cleared in the graybox instead of weakening the roaming test
- [ ] exact final-head 4/4 CTest green
- [ ] combat smoke green on final head
- [ ] encounter smoke green on final head
- [ ] spatial smoke green on final head
- [ ] exact final-head GitHub CI green before human demo handoff

## Par / Compare-5 for v0.7

Behavioral/design references, not claims about proprietary internals.

- [x] **Dishonored 2** — bounded shared knowledge, search/coordination and systemic local context
- [x] **F.E.A.R.** — local tactical behavior should read as purposeful without requiring a giant planner
- [x] **Halo 3** — map-scale spatial clarity and readable encounter geography
- [x] **The Last of Us** — believable search, uncertainty, spatial mistakes and human-scale reacquisition
- [x] **Left 4 Dead** — distinguish individual agent competence from higher-order encounter/spatial intensity; do not bake a Director into every actor

## Deliberate v0.7 boundaries

- [ ] no full Ghost Static / global AI Director yet
- [ ] no live autonomous teammate combat brain yet
- [ ] no live Dream Cattler ecological brain yet
- [ ] no giant continuous influence-map texture/grid
- [ ] no general GOAP planner
- [ ] no full objective/ecological scoring implementation
- [ ] no polished negotiation UI
- [ ] no final art/audio pass
- [ ] no cross-match adaptive AI
- [ ] no 8 km² production map yet

## Human v0.7 demo acceptance checklist

Record roughly **90–150 seconds** if possible.

- [ ] title reads `v0.7 - SPATIAL ECOLOGY / SITUATED AGENCY LAB`
- [ ] client reaches `AUTHORITATIVE + PREDICTED`
- [ ] movement/mouse/combat remain functional
- [ ] use **F4** at least once to show the coarse region graph
- [ ] move far enough to make the 0.40 km² scale legible; the whole map does not need to be traversed
- [ ] observe whether Rivals actually leave the immediate spawn neighborhood when combat evidence goes stale
- [ ] break LOS and note whether Rivals investigate/reposition rather than magically track
- [ ] look for Rivals distributing rather than four bodies choosing the same region/cover/task
- [ ] look for temporary local habitation: watch / occupy / investigate / flank / pressure before relocation
- [ ] standing exposed should still be dangerous; v0.7 must not erase v0.6 encounter competence
- [ ] strafe/reverse/jump should still buy counterplay against finite tracking
- [ ] kill at least one Rival and allow at least one player death/respawn if practical
- [ ] traverse at least one ramp/ladder/obstacle so larger-map geometry does not hide a movement regression
- [ ] note any AI repeatedly walking into the same obstacle, ping-ponging between two regions, freezing, or clustering
- [ ] note anything that is **extremely right** as well as anything obviously wrong

After closing the demo, capture:

```bash
tail -n 160 build/server.log
```

The recording shows outward behavior; the log shows region/task/field reasoning.

## Current tuning watch

- [ ] verify regional goals actually produce visible cross-region travel at human timescale
- [ ] verify region centers/corridors do not create straight-line obstacle traps
- [ ] verify route-congestion preference meaningfully reduces clumping
- [ ] verify ally-support preference does not recreate clumping through another route
- [ ] verify objective value remains subordinate to spatial coherence until hard objective semantics exist
- [ ] verify novelty/boredom does not make agents wander during an obviously meaningful local fight
- [ ] verify local task commitment reads as intention rather than hesitation
- [ ] verify incident memory changes repeated approaches without making the squad permanently timid
- [ ] verify F4 graph overlay is useful without becoming ordinary HUD clutter
- [ ] verify 96 cover affordances remain a bounded useful cost at this map scale
- [ ] keep v0.6 pressure-slot / finite-tracking / suppression tuning on watch
- [ ] prediction max-error spikes around death/respawn remain a watch item

## Pre-v0.8 decision gate

The v0.7 human recording chooses the next emphasis. Do **not** lock v0.8 before that evidence. Candidate branches are:

1. teammate / crew social intelligence
2. Dream Cattler habitat ecology
3. Ghost Static / director-level pacing and story context
4. a spatial/navigation refinement pass if v0.7 exposes region-routing or local-obstacle failures

Viewing for v0.8 should be processed through the standing **question subroutine**: `WATCH -> QUESTION -> COMPARE WITH CURRENT CODE -> KEEP / ADAPT / DEFER / REJECT -> CODING CONTRACT -> COMPILE -> HUMAN VIDEO`.
