# nightfall!punk master ledger

Versioning rule: **one flat version = one coherent development contract.** Fine-grained history belongs in commits. Accepted versions are preserved as immutable `archive/v0.X` branches before `main` advances. No nested subversions.

## Project identity

- [x] C + raylib; Linux-first
- [x] fixed 60 Hz simulation
- [x] authoritative dedicated server
- [x] server truth + client prediction/reconciliation
- [x] four-player target for now; possible five later
- [x] round-based story-oriented systemic arena FPS
- [x] graybox architecture/mechanics before original aesthetics
- [x] direct confrontation remains the gameplay emphasis
- [x] physical mechanisms/puzzles later create route/reward advantages under combat pressure
- [x] genre = **Neo-Xennial Shooter / Neo-Xennial FPS**
- [x] subtype = **Systemic Arena FPS**
- [x] thesis = Quake immediacy + Deus Ex systemic possibility + Halo spatial clarity + F.E.A.R. encounter intelligence + contemporary simulation/network infrastructure
- [x] manifesto = **The neo-xennial shooter continues the FPS future that the early 2000s started: fast, physical, systemic, open, and unpredictable.**

## Architectural maxims

- [x] **Seed is infrastructure; server is truth; clients predict/reconcile.**
- [x] **Fuzzy Rail** = shared local geometric traversal intelligence, never global strategic routing.
- [x] **Dynamic Affordance Graph** = utility + bounded affordance queries + spatial fields + blackboards; selective planning only when earned.
- [x] **World Semantic Alerts** = server-authored gameplay meaning above packet semantics; AI never infers critical truth from packet arrival/loss.
- [x] `server truth != individual knowledge != group report`
- [x] faction identity is distinct from relationship state.
- [x] minimum sufficient machinery; maximum relational consequence.
- [x] keep primitives boring enough to trust; composition sophisticated enough to surprise.
- [x] AI is a simulation participant, not an administrator.
- [x] one perception architecture may support multiple epistemologies/motivations.

## Faction / ecology identity

### Player Crew / teammates

- [x] player-facing game is experienced from a privileged phenomenological position, not an ontologically privileged universe.
- [x] core world principle = **The world exists for the player because the world exists for itself.**

### Human Rivals

- [x] secondary antagonism
- [x] temporary terrain antagonism
- [x] negotiable
- [x] may cooperate tactically without ceasing to be Rivals
- [x] faction hostility and informational credibility are independent

### Dream Cattlers

- [x] primary ecological antagonism
- [x] semi-permanent environmental antagonism
- [x] always non-negotiable
- [x] `predatory` and `non-negotiable` are independent axes
- [x] rare hostile/non-predatory disposition may create avoidant detente without diplomacy
- [x] technical `Rancher` identifiers may remain temporarily; player-facing term = **Dream Cattler**

### Ghost Static / Apophis

- [x] Ghost Static = literal and metaphorical DJ, event promoter, media broker and organizer inhabiting the world
- [x] Apophis = deeper mythic/signal layer
- [x] Ghost Static curates circumstances; he does not author cognition
- [x] he may amplify salience, organize real commitments and move information through legitimate networks
- [x] Director-level world truth may gate event eligibility; individual actors still discover truth through world evidence
- [x] **He can promote the party. He cannot decide what happens when everyone arrives.**

## Version / archive status

- [x] **v0.1 Foundation** — `archive/v0.1`
- [x] **v0.2 Movement** — `archive/v0.2`
- [x] **v0.3 Networked Movement** — `archive/v0.3`
- [x] **v0.4 Combat** — `archive/v0.4`
- [x] **v0.5 Agent Intelligence Framework** — `archive/v0.5`
- [x] **v0.6 Encounter Intelligence** — accepted/merged/archived at `archive/v0.6`
- [x] **v0.7 Spatial Ecology / Situated Agency** — human accepted/merged/archived at `archive/v0.7`
- [ ] **v0.8 Dream Cattler Habitat Ecology** — PR #11, draft/open, CI-green candidate; still requires human acceptance before merge/archive
- [ ] **v0.9 Topographical Energy Accounting** — stacked candidate on `build/v0.9-topographical-energy`; PR #12 is intentionally based on the v0.8 branch until v0.8 acceptance closes

## Regression-protected baseline

### v0.1–v0.4

- [x] fixed simulation and headless/graphical split
- [x] movement/Fuzzy Rail owns step/vault/mantle/ladder/local traversal
- [x] ENet behind `nf_net`
- [x] server validates client intent
- [x] snapshots + reconciliation authoritative
- [x] carbine/pistol ammo/reload/hitscan/rewind/damage/death/respawn authoritative
- [x] relationship/faction policy gates damage

### v0.5–v0.6 Human Rival intelligence

- [x] limited perception
- [x] imperfect memory/confidence
- [x] semantic hearing
- [x] utility and cover affordances
- [x] Human Rival AI emits ordinary `NfControlFrame`
- [x] no teleport or privileged damage path
- [x] finite yaw/pitch tracking and aim settling
- [x] default two-slot soft pressure economy
- [x] contextual PRESSURE / FLANK_LEFT / FLANK_RIGHT / HOLD roles
- [x] damage-driven suppression/retreat
- [x] AI/encounter code has no raylib/ENet dependency

### v0.7 situated agency

- [x] 0.40 km² / 500 m x 800 m graybox = 5% of eventual 8 km² map-area target
- [x] deterministic 24-region coarse graph
- [x] bounded next-hop routing
- [x] five local samples/query
- [x] top-four bounded regional candidates
- [x] spatial tasks ROAM/WATCH/INVESTIGATE/OCCUPY/PRESSURE/FLANK/REGROUP/CONTEST
- [x] task hysteresis / claims / visitation memory / incident memory / deterministic variation
- [x] F4 region diagnostic overlay
- [x] server-side spatial reasoning; client receives consequences rather than whole scratch state
- [x] no giant influence texture/grid
- [x] no general GOAP

### v0.8 Dream Cattler habitat ecology candidate

- [x] separate `nf_cattler` AI family; not a renamed Human Rival controller
- [x] habitat priority = continuity -> resource/ecological value -> prey evidence -> disturbance -> territorial pressure -> access cost
- [x] physical occupation deepens infestation
- [x] abandoned infestation decays slowly
- [x] infestation creates residency/inertia rather than raw damage/health buffs
- [x] pack reports are imperfect and stale
- [x] loners ignore ordinary pack coordination
- [x] individual Cattlers cannot read authoritative prey truth directly
- [x] reciprocal Human Rival/Cattler perception works through normal LOS/evidence and relationships
- [x] dedicated knee/foot hit zones reduce locomotor integrity
- [x] tall combat silhouette; shared human-sized movement clearance remains a deliberate graybox boundary
- [x] internal Cattler ecology ledger: infestation / prey / displacement / expansion / survival
- [x] ordinary HUD does not expose exact ecology score values
- [x] default 3 Cattlers; debug PACK/LONER/HORDE profiles
- [x] current swoop = committed sprint/jump proxy, not final aerial traversal

## C1 — ecological legibility / show, do not tell

- [x] player first interprets world anthropically: enemies appear to exist for the player
- [x] later evidence reveals the player as one ecocentric participant in a larger social ecology
- [x] independent activity must leave persistent consequences the player can later discover
- [x] preferred sense-data order: recurring actor behavior -> persistent spatial consequence -> reactions of other actors -> environmental traces -> audio -> Ghost Static interpretation -> explicit HUD only if unavoidable
- [x] exact ecological state is server truth; player-facing ecology is inferred through the world

Maxim: **The world exists for the player because the world exists for itself.**

## C2 — Hunt Surge composition/decomposition

Locked design:

- [x] Hunt Surges are phase transitions in local ecology, not hard encounter switches
- [x] convergence pressure should rise and fall gradually
- [x] individuals remain controlled by their own evidence, habitat inertia, locomotor condition and local claims
- [x] surge decomposition should leave residual danger; actors do not all disengage on one tick
- [x] local density/territorial pressure should help dissolve surges naturally
- [x] eventual map uses a local carrying/mob-cap per roughly **500 m²**, enough for a small local pack and exceptional 4–5 Cattler surge without normal overcrowding

Current implementation discrepancy:

- [ ] v0.8 runtime still expresses `HUNT_SURGE` as an abrupt event enum with duration/cooldown; gradual convergence pressure + 500 m² carrying-cap are **not yet implemented**

## C3 — wounded Cattler life-state

Recommendation locked:

`WOUNDED -> WITHDRAW -> defensible infestation -> ENTRENCH -> LURK / SNIPE / DEFEND`

- [x] no magic healing purely to close a state machine
- [x] locomotor injury should reduce geographic dominance while potentially increasing local danger
- [ ] explicit ENTRENCH terminal behavior not yet implemented

## C4 — Rival target choice

- [x] target utility should be situated, not fixed faction priority
- [x] immediate threat, exposure, distance, damage source, swoop commitment, role, resource/objective context and evidence confidence may redirect attention
- [x] de facto temporary alignment against a Cattler does not rewrite Rival hostility

## C5 — infestation collapse

- [x] show through absence, changed routes, changed actor reuse, disappearing territorial recurrence and decaying traces
- [x] avoid generic `INFESTATION CLEARED` score popups unless later proven necessary

## C6 — world simulation for itself

Target fidelity stack:

1. scenic/static
2. scheduled persistence
3. statistical ecology
4. regional simulation
5. proxy-agent simulation
6. full local embodiment

Recommended operating model:

- [x] simulate consequences globally
- [x] simulate intentions regionally
- [x] simulate embodiment locally
- [x] same actor may change simulation resolution; precision may change, history may not
- [x] simulation LOD should follow causal significance as well as proximity

Maxim: **Changing simulation resolution may change precision, never history.**

## C7 — plural situated economy, revised under universal currency

Earlier framing of separate currencies is superseded by C18.

Locked revision:

- [x] **one world, one universal energy-accounting currency, different strategies and production functions**
- [x] resource meaning remains relational because the energetic return depends on actor situation, affordances, knowledge and topography
- [x] material / spatial / ecological / informational / social / temporal resources can all alter energetic accounts
- [x] focal-point salience may make actors converge because they expect others to notice the same place/event
- [x] shared salience does not imply shared interest

## C8 — off-screen collisions between economies

- [x] abstract choreography; preserve causality
- [x] preserve who was present, what they knew/intended, injuries, resources, territorial/ecological state, deaths/withdrawals/displacement and causal outcomes
- [x] abstract exact recoil, animation, per-frame collisions and other embodied details when no observer requires them
- [x] actors choose locally; world resolves globally
- [x] no retroactive reroll when player arrives

## C9 — Ghost Static as promoter / Director

- [x] promotion changes salience, not actor preferences
- [x] organization creates real commitments: venues, broadcasts, invitations, equipment, schedules, contacts
- [x] media brokerage carries provenance, delay and possible distortion
- [x] curation selects/juxtaposes world possibilities like a DJ rather than scripting actor cognition
- [x] Ghost Static may fail, overhype, attract the wrong participants or produce unintended consequences

## C10 — information economy

- [x] information is a situated economic resource
- [x] knowledge has provenance, confidence, age and scope
- [x] information may propagate, decay, distort or become obsolete
- [x] actors act on beliefs, not authoritative truth
- [x] beliefs may alter traffic/salience/resource value and become partially self-fulfilling or self-defeating

Maxims:

- **World truth is not common knowledge.**
- **Beliefs can change the world without becoming true.**
- **Resources create information; information can create resources.**

## C11 — trust / credibility

- [x] trust is actor-relative, domain-specific and historically revised
- [x] relationship status does not determine credibility
- [x] repetition is not corroboration
- [x] independent sources may reinforce claims
- [x] Human social trust remains richer than Cattler pack-evidence weighting

Maxim: **Reputation answers “trusted about what?”**

## C12 — situational compacts

- [x] Rival remains Rival while bounded cooperation occurs
- [x] support tacit restraint, explicit ceasefire, transactional compact, common-threat cooperation and limited collaboration
- [x] compacts have scope: parties / location / use / prohibitions / termination condition
- [x] breach affects trust, information and future terms rather than being mechanically forbidden
- [x] accidents should be interpreted probabilistically rather than one damage event automatically becoming betrayal
- [x] compacts compose/decompose rather than instantly flipping faction allegiance
- [x] Dream Cattlers remain outside human compact grammar

## C13 — norms / institutions

- [x] compact = current agreement; norm = expectation shaped by repeated history
- [x] compress socially important history into contextual expectations
- [x] norms remain scoped by region, parties and context
- [x] repeated compliance can self-reinforce; violation is especially informative
- [x] norms may become informal institutions and may later decay

## C14 — topographical tenure / shared use

Note: **topography** is the governing spatial idea; topology remains useful only where connectivity itself matters.

- [x] distinguish possession, claim, access, commons and habitat
- [x] rights/claims attach to uses and times rather than only colored polygons
- [x] shared space can be partitioned spatially, temporally or behaviorally
- [x] Cattler habitat does not recognize human property concepts
- [x] social prohibitions should normally remain physically violable; trespass produces consequences rather than invisible walls

Maxims:

- **Possession says who controls it. A claim says who expects to use it.**
- **Shared space does not require shared use.**
- **Territory is topography plus history.**

## C15 — phenomenological boundaries

- [x] a boundary becomes real when crossing it predictably changes perceived affordances, expectations, encounters and atmosphere
- [x] human Crew boundaries are performed through culture, movement, occupation, style, sound, argot and social response
- [x] Cattler boundaries are ecological: changed traffic, recurring perches, hunting routes, disturbance traces, temporal patterns and avoidance by other actors
- [x] Ghost Static events can temporarily redraw experiential currents without rewriting land ownership
- [x] player-facing territoriality should usually be a gradient of evidence

Maxims:

- **A border is real when crossing it changes what things mean.**
- **Territory is performed before it is labeled.**
- **The map has lines. The city has gradients.**

## C16 — history changes resource availability

- [x] past extraction, construction, destruction, occupation, migration, conflict, cooperation, ecological change and cultural activity alter present resources
- [x] resources renew, disappear, transform or emerge through world processes rather than generic respawn timers
- [x] history may create resources as well as deplete them
- [x] economic history may change physical/social access and redirect future actors
- [x] place memory should often be stored directly in present material/ecological/social consequences rather than a separate lore meter

Maxim: **The economy remembers through the landscape.**

## C17 — world metabolism / ecological index objects

- [x] resources participate in causal world metabolisms: source -> production/renewal -> transport -> use -> damage/waste -> repair/recycling/recovery
- [x] maintenance competes with extraction
- [x] one ecology's waste can become another ecology's resource
- [x] visible objects may act as interfaces, bottlenecks, indicators and proxies for larger simulated/presumed processes
- [x] process fidelity categories: embodied / aggregated / presumed / decorative
- [x] presumption is legitimate; contradiction is not
- [x] regions have productive and absorptive capacity
- [x] production can itself create focal points

Maxim: **An object can be the visible tip of a system.**

## C18 — universal topographical energy accounting

Correction locked:

- [x] **energy accounting is the universal currency**
- [x] economic value is topographical: distance, elevation, slope/terrain, exposure, density, infrastructure, water/environmental condition and geographic distribution affect acquisition cost
- [x] different actors may use the same currency through different strategies and conversion paths
- [x] information/trust/social order are not separate currencies; they matter because they reduce or redirect energetic costs and returns
- [x] sustainability is a material problem of increasing energetic command without destroying regenerative, absorptive and productive conditions for future growth

Maxims:

- **Energy accounting is the universal currency.**
- **Topography participates in price formation.**
- **The same currency permits different games.**
- **History continually reprices the landscape.**

## C19 — recurrent Bayesian helicopter search

Locked formulation:

`TRUTH -> ACCOUNT -> SEARCH -> BELIEF -> STRATEGY -> SIMULTANEOUS ACTION -> CONSEQUENCE -> NEW TRUTH`

- [x] authoritative accountant may know present world truth
- [x] perfect present truth does not imply perfect future prediction
- [x] world-level helicopter search may inspect authoritative state for trajectories/pressures
- [x] actor search must use actor belief, not omniscient state
- [x] immediate / round / regional / regenerative are search horizons over one universal ledger, not distinct currencies
- [x] deeper forecasts should become more uncertain and expensive

Maxim: **One energy ledger; many Bayesian perspectives.**

## C20 — accounting invariants — v0.9 coding contract

- [x] TRANSFER conserves stored energy across locations
- [x] CONVERSION consumes potential input, creates usable output and records conversion loss
- [x] CAPACITY changes future throughput without minting current stock
- [x] REGENERATION is explicit external input into the modeled regional system
- [x] movement/extraction can register expenditure/externality
- [x] `NfEnergySystem` is embedded in authoritative `NfWorld`

Invariant: **Every gain needs a causal source; every conversion has a cost; every apparent multiplication must be capacity or explicit external input, not free energy.**

## C21 — helicopter-search resolution — v0.9 coding contract

- [x] first slice uses the accepted 24-region spatial lab
- [x] no giant per-object world solve
- [x] regional energy truth includes potential, usable stock, productive capacity, regeneration, absorption, health, externality load and topographical parameters
- [x] topographical route cost includes distance, graph-step depth, elevation proxy, terrain resistance and structural exposure
- [x] world metabolism ticks inside fixed simulation truth
- [x] global helicopter ranking refreshes at low cadence
- [x] `NfEnergyBelief` supports prior/evidence update, confidence and staleness
- [x] actor belief search is separate from world truth search

## C22 — minimum human/automated proof — v0.9

Proof pair:

- [x] **South Works** = high initial stock / weaker regeneration / higher topographical cost
- [x] **Signal Court** = lower initial stock / higher production+regeneration / cheaper central access
- [x] three stationary Rival actors can materially deplete South Works in the test harness
- [x] Signal Court can regenerate while South Works is depleted
- [x] stale vs fresh beliefs diverge after history changes the world
- [x] helicopter ranking returns bounded top candidates
- [x] new `topographical_energy_v09` CTest
- [x] new `energy-smoke`

Important boundary:

- [x] v0.9 does **not** inject omniscient energy truth into existing Human Rival/Cattler cognition
- [ ] future actor-facing energy integration must pass through legitimate belief projection

# v0.9 — existing / advancing / par checklist

## Existing systems that must remain green

- [ ] fixed 60 Hz simulation
- [ ] movement/Fuzzy Rail
- [ ] ENet authority/prediction/reconciliation
- [ ] authoritative combat/hitscan/rewind/death/respawn
- [ ] Human Rival perception/memory/utility/affordances
- [ ] bounded encounter pressure/suppression
- [ ] 24-region / 0.40 km² spatial ecology
- [ ] Dream Cattler habitat/pack ecology/locomotor weak points

## Advancing systems targeted in v0.9

- [x] new `nf_energy.c/.h`
- [x] universal energy ledger embedded in `NfWorld`
- [x] topographical acquisition-cost function
- [x] regional potential/usable stock
- [x] productive/regenerative/absorptive capacities
- [x] regional health/externality/history
- [x] low-resolution occupancy extraction proof
- [x] accounting transfer/conversion/capacity invariants
- [x] omniscient bounded helicopter ranking
- [x] bounded actor belief prior/evidence update
- [x] stale-belief decay
- [x] C22 South Works / Signal Court proof pair

## Par / deliberate non-goals for this build

- [x] do not merge/archive v0.8 merely because v0.9 is being developed
- [x] do not let AI read omniscient energy truth directly
- [x] do not build final barter/currency UI
- [x] do not add a giant global GOAP/economic planner
- [x] do not add full Ghost Static economy yet
- [x] do not add final ecological index art/audio yet
- [x] do not scale to 8 km² yet
- [x] do not patch wounded Rival retreat with a timer
- [x] do not pretend Hunt Surge composition/decomposition is implemented when it is not

# Readiness discrepancies / watches

1. [ ] **v0.8 acceptance:** PR #11 remains draft/open. No `archive/v0.8` yet.
2. [ ] **Hunt Surge:** current runtime is abrupt; C2 requires gradual composition/decomposition.
3. [ ] **Cattler mob cap:** 500 m² local carrying-cap design is not yet represented in runtime.
4. [ ] **Wounded Human Rival:** prolonged RETREAT still lacks a systemic post-safety life-state.
5. [ ] **Actor energy cognition:** v0.9 establishes truth/search/belief primitives but does not yet feed energy beliefs into Human Rival or Cattler decision selection.
6. [ ] **Player energy sense-data:** exact values remain diagnostic; final expression should be world consequences, not meters.
7. [ ] **Version strings:** any remaining hard-coded v0.8 runtime/test text encountered during final compile should be treated as documentation/presentation debt, not silently ignored.

# v0.9 compile gate

- [ ] strict full graphical compile green
- [ ] 5/5 CTest green
- [ ] combat smoke green
- [ ] encounter smoke green
- [ ] spatial smoke green
- [ ] Cattler smoke green
- [ ] energy smoke green
- [ ] final exact-head GitHub Actions green
- [ ] draft PR #12 remains open until Pop!_OS proof is reviewed

# v0.9 human acceptance checklist

Record roughly **2–3 minutes** of the normal graphical demo.

- [ ] client reaches authoritative/predicted play normally
- [ ] ordinary movement remains stable
- [ ] combat remains stable
- [ ] Human Rival behavior remains coherent
- [ ] Dream Cattler behavior remains coherent
- [ ] no new obvious hitching or simulation instability from world-energy accounting
- [ ] briefly show F4 region graph if useful
- [ ] say the quiet part aloud if anything feels extremely right or extremely wrong

Then capture:

```bash
./nightfall.sh energy-smoke
tail -n 200 build/server.log
cat build/energy-smoke.log
```

Acceptance thesis:

> **The world now maintains a universal, historically changing topographical energy account without regressing the existing FPS simulation, and bounded Bayesian search can distinguish current omniscient opportunity from stale actor belief without leaking world truth into cognition.**

# Study / questions subroutine

For each research pass:

`WATCH/READ -> QUESTION -> COMPARE CURRENT CODE -> KEEP/ADAPT/DEFER/REJECT -> CODING CONTRACT -> COMPILE -> HUMAN VIDEO -> REASSESS`

Before any major compile ask:

1. What learned material actually changes code?
2. What merely confirms the current design?
3. What is attractive but premature?
4. What adds complexity without proportionate value?
5. What is the smallest implementation the next human recording can prove or disprove?

# 2026-08-08 superseding human-proof status + C23–C30 lifeworld coding ledger

This section **supersedes the unchecked human-proof status above** without rewriting historical checklist text.

## Human-proof disposition after the v0.9 recording + server trace

- [x] v0.9 graphical regression proof: PASS — authoritative/predicted play, movement, combat, Human Rival behavior and Dream Cattler behavior remain coherent enough to advance design.
- [x] v0.9 server trace corroborates Rival↔Cattler reciprocal targeting and imperfect pack evidence.
- [x] v0.9 topographical-energy smoke remains the authoritative proof of C20–C22.
- [x] targeted v0.8 lower-body weak-point test was actually performed in the recording.
- [ ] **v0.8 weak-point truth boundary remains FAILED/OPEN:** the player visibly aimed at the yellow lower-body/foot bands, while the end-of-video locomotor diagnostic remained `locomotor=1.00` and displayed Cattler health remained `100`. Treat this as an implementation discrepancy, not missing human effort.
- [ ] do not merge/archive v0.8 until authoritative hit classification and locomotor consequence are proven.
- [ ] v0.9 may be human-accepted on its own behavioral/energy evidence but remains dependency-ordered behind closure of v0.8.

Weak-point repair chain to instrument:

```text
VISIBLE YELLOW KNEE/FOOT BAND
        ↓
authoritative hitscan intersection
        ↓
NF_HIT_KNEE / NF_HIT_FOOT
        ↓
combat event reaches nf_cattler_on_combat_event()
        ↓
locomotor_integrity < 1.00
        ↓
reduced relocation / swoop / geographic dominance
```

Required deterministic proof sketch:

```c
NfCombatEvent hit = {0};
hit.type = NF_COMBAT_EVENT_DAMAGE;
hit.target = cattler_id;
hit.hit_zone = NF_HIT_FOOT;
hit.amount = 10.0f;

nf_cattler_on_combat_event(&cattlers, &world, &hit);
const NfCattlerAgent *c = nf_cattler_find_const(&cattlers, cattler_id);
assert(c != NULL);
assert(c->locomotor_integrity < 1.0f);
```

## C23 — situated topographical energy rationality — LOCKED / next-build coding contract

Core contract:

> **Energy accounting is universal; its application is situated. Local topography determines the application of universal currency.**

- [x] shared physical world and shared universal energy accounting remain authoritative.
- [x] actor conclusions need not converge because actors do not occupy the same local topography.
- [x] actor position, embodiment, route access, elevation, terrain, exposure, infrastructure, social access, history and legitimate knowledge alter realized energetic cost/return.
- [x] world accountant may use authoritative `NfEnergySystem`; actor strategy may only use legitimate `NfEnergyBelief` + actor-local costs.
- [x] same energy currency must not become a shared omniscient utility map.
- [ ] add actor-facing situated valuation wrapper around existing `nf_energy_belief_search()`.
- [ ] keep Human Rival and Cattler strategy grammars distinct even when both ultimately conserve/grow energy accounts.
- [ ] convergence should arise only when a genuinely strong focal opportunity overcomes topographical differences.

Current infrastructure to reuse: `NfEnergyBelief`, `NfEnergyOpportunity`, `nf_energy_belief_decay()`, `nf_energy_belief_search()`.

Coding sketch:

```c
typedef struct NfSituatedEnergyCost {
    double embodiment;
    double access;
    double exposure;
    double social_friction;
    double habitat_fit;
} NfSituatedEnergyCost;

double nf_actor_energy_value(
    const NfActor *actor,
    const NfEnergyOpportunity *op,
    const NfSituatedEnergyCost *local)
{
    (void)actor;
    return op->score
         - local->embodiment
         - local->access
         - local->exposure
         - local->social_friction
         + local->habitat_fit;
}
```

Actor-search wrapper sketch:

```c
size_t nf_actor_energy_candidates(
    const NfEnergyBelief *belief,
    const NfEnergySystem *energy,
    const NfRegionGraph *graph,
    uint8_t origin_region,
    NfEnergyHorizon horizon,
    NfEnergyOpportunity *out,
    size_t cap)
{
    return nf_energy_belief_search(
        belief, energy, graph, origin_region, horizon, out, cap);
}
```

Maxims:

- **Energy is universal; its price is situated.**
- **Same world does not mean same energetic landscape.**
- **Reality supplies the bill; belief supplies the estimate.**

## C24 — ecological waterline / hidden but not secret — LOCKED

Core contract:

> **The ecological waterline is the locally embodied perceptual foreground through which a much larger ecology becomes intelligible. The player sees a sample, not a summary.**

- [x] imitate a forest walk: nearby living motion, trails, wind, water, sound, organisms/actors and material traces imply a deeper ecology beyond immediate vision.
- [x] deep simulation may remain hidden; strategically meaningful hidden states must have causal paths to observable consequences.
- [x] no raw infestation/energy/ecology meter as the primary expression.
- [x] waterline channels = living motion / metabolic motion / traces / traffic-flows / ambient fields.
- [x] foreground samples may be partial and stochastic but cannot contradict authoritative ecology.
- [x] proxy manifestations are legitimate when they derive from real lower-resolution causes.
- [ ] create a bounded ecological surface-expression layer after causal life activity exists.
- [ ] classify visible effects internally as CAUSAL / DIAGNOSTIC / DECORATIVE so decoration does not masquerade as evidence.

Coding sketch:

```c
typedef struct NfEcologySurface {
    float actor_activity;
    float infrastructure_activity;
    float traffic;
    float environmental_motion;
    float historical_trace;
    float ambient_intensity;
} NfEcologySurface;

typedef enum NfSurfaceRole {
    NF_SURFACE_CAUSAL = 0,
    NF_SURFACE_DIAGNOSTIC,
    NF_SURFACE_DECORATIVE
} NfSurfaceRole;
```

Sense-data rule:

```text
AUTHORITATIVE STATE
       ↓
WORLD CONSEQUENCE
       ↓
PERCEPTIBLE EVIDENCE
       ↓
PLAYER / ACTOR INFERENCE
```

Maxims:

- **The waterline shows life; the iceberg explains it.**
- **Hidden is acceptable. Secret is not.**
- **We simulate the cause and economize the manifestation.**

## C25 — structural ecological memory — LOCKED

Core contract:

> **Persistence is a relation, not a timer. A trace persists, accumulates, disappears or transforms according to the structures acting upon it.**

- [x] reject one universal trace TTL/decay variable.
- [x] trace evolution depends on trace type + substrate + water/weather + traffic + occupation + maintenance + destruction + reuse + later history.
- [x] repetition can transform trace -> pattern -> route -> corridor -> expectation -> institution/habitat.
- [x] disappearance of an individual trace may coincide with strengthening of a higher-order trail/structure.
- [x] maintenance materially edits history; succession often transforms rather than deletes evidence.
- [x] traces can become affordances/resources: salvage, cover, information, route value, habitat value.
- [ ] implement small structural trace substrate only after the next build has actual life activities worth tracing.

Coding sketch:

```c
typedef struct NfTraceMaterial {
    float water_sensitivity;
    float wind_sensitivity;
    float traffic_sensitivity;
    float repair_sensitivity;
    float reuse_sensitivity;
    float ecological_sensitivity;
} NfTraceMaterial;

typedef struct NfTraceContext {
    float water;
    float wind;
    float traffic;
    float maintenance;
    float disturbance;
    float occupation;
    float ecological_change;
} NfTraceContext;

void nf_trace_step(NfTrace *trace,
                   const NfTraceMaterial *material,
                   const NfTraceContext *context,
                   float dt);
```

Maxims:

- **The trace does not contain its own destiny.**
- **Repeated traces can become topography.**
- **History is transformed more often than it is deleted.**

## C26 — evental difference / structural tipping — LOCKED

Core formulation:

> **A difference becomes evental when consequence becomes cause.**
>
> **The evental tipping point occurs when a difference changes not merely the state of the world, but the structure governing the world's next possible states.**

- [x] eventality is not proportional to spectacle or perturbation magnitude.
- [x] tipping belongs to structural susceptibility, not a universal scalar threshold.
- [x] evental transition requires a difference to alter future affordances/costs/expectations/relations/ecological processes and feed back into later causation.
- [x] eventality may be objectively real before actors name or commonly know it.
- [x] composition and decomposition are symmetrical; promoted structures remain reversible.
- [ ] do **not** add one `event_score > threshold` mechanism.
- [ ] later diagnostics should inspect structural predicates/feedback relationships.

Coding sketch:

```c
bool nf_difference_is_evental(
    const NfWorldState *before,
    const NfWorldState *after)
{
    return nf_future_possibility_structure_changed(before, after)
        && nf_consequence_feeds_back(after)
        && nf_historical_residue_remains(before, after);
}
```

Diagnostic dimensions are relational predicates, not an additive score:

```text
DIFFERENCE
+ STRUCTURAL CONSEQUENCE
+ RECURSIVE RESPONSE
+ HISTORICAL RESIDUE
→ EVENTAL TRANSITION
```

## C27 — resilience — LOCKED

Core contract:

> **Resilience is the capacity of a structure to undergo disturbance while continuing to reproduce the generative relations that make it the kind of structure it is.**

- [x] resilience preserves organization, not exact state.
- [x] substantial state change can occur without regime change if the core causal organization reforms.
- [x] do not make `region.resilience` a magic authoritative scalar.
- [ ] derive resilience from access recovery + metabolism recovery + actor-network reformation + survival of core feedback relations.

Coding sketch:

```c
bool nf_structure_can_reproduce_after(
    const NfRegion *region,
    const NfDisturbance *disturbance)
{
    return nf_access_can_recover(region, disturbance)
        && nf_metabolism_can_recover(region, disturbance)
        && nf_actor_network_can_reform(region, disturbance)
        && nf_core_feedbacks_remain_viable(region, disturbance);
}
```

Maxim: **Resilience means persistence of organization, not preservation of state.**

## C28 — hysteresis — LOCKED

Core contract:

> **Hysteresis is history embedded in the conditions of return. Removing the initiating cause does not necessarily remove the resulting structure.**

- [x] collapse and recovery need not follow the same path.
- [x] historical consequences may become current causes: lost routes, altered norms, broken infrastructure, depleted stocks, new habitat, changed traffic.
- [x] do not use a universal hysteresis coefficient as truth.
- [ ] determine recoverability from current material/social/ecological conditions, not from whether the original disturbance has ended.

Coding sketch:

```c
bool nf_previous_regime_can_return(const NfRegion *region)
{
    return nf_routes_support_prior_flows(region)
        && nf_required_capacity_exists(region)
        && nf_social_expectations_support_return(region)
        && nf_ecological_feedback_allows_return(region)
        && nf_energy_metabolism_supports_return(region);
}
```

## C29 — succession — LOCKED

Core contract:

> **Succession is the historically conditioned reorganization of structures as one configuration alters the possibilities available to whatever comes next.**

- [x] reject a scripted `ABANDONED -> INFESTED -> RECOVERING -> NORMAL` stage ladder.
- [x] each configuration leaves stocks, traces, damage, access, habits, actors and affordances that constrain successors.
- [x] same disturbance may produce different succession depending on local topography/history.
- [x] succession is C25 history recursively reorganizing both waterline and deeper ecology.
- [ ] later succession step should update opportunities/pressures/metabolism/history/affordances rather than increment a stage enum.

Coding sketch:

```c
void nf_region_succession_step(NfRegion *region)
{
    NfAffordanceSet available = nf_current_affordances(region);
    NfPressureSet pressures = nf_current_pressures(region);

    nf_update_actor_opportunities(region, &available, &pressures);
    nf_resolve_world_metabolism(region);
    nf_apply_historical_transformations(region);
    nf_rebuild_emergent_affordances(region);
}
```

Maxim: **Each historical configuration prepares, constrains or prevents its successors.**

## C30 — regime change — LOCKED

Core contract:

> **Regime change occurs when an evental difference reorganizes enough mutually reinforcing relations that subsequent states are generated according to a different structural logic.**

- [x] a regime is a relatively self-reproducing configuration of generative relations, not a biome label.
- [x] dominant causal loop matters more than population count or appearance.
- [x] `NF_REGIME_CATTLER`-style enum may be useful as derived debug description but must not cause the ecology it describes.
- [x] regime change follows C26 at larger scale: the structure producing the future changes.
- [ ] derive compact debug signatures from metabolic / habitat / social / logistics / restorative feedbacks.

Coding sketch:

```c
typedef struct NfRegimeSignature {
    float metabolic_loop;
    float habitat_loop;
    float social_loop;
    float logistics_loop;
    float restorative_loop;
} NfRegimeSignature;

NfRegimeSignature nf_derive_regime_signature(const NfRegion *region);
```

Maxim: **Regime change occurs when the structure producing the future changes.**

# Lifeworld synthesis — topographic biopower / life-activity

Governing development contract:

> **nightfall!punk models a lifeworld rather than a collection of systems. Universal energetic accounting becomes concrete only through local topography, embodiment, knowledge and history. Actors convert energy into life-activity—movement, work, maintenance, occupation, predation, cooperation and conflict—and thereby alter the material and social landscape that conditions subsequent activity. Real ecological processes provide the principal model: flows, patch heterogeneity, niche construction, disturbance, carrying capacity, succession, resilience and regime change. The deep simulation remains largely beneath perception, while its dynamic waterline appears as organisms, traffic, mechanisms, traces, sound, weather, water and changing patterns of use. Formal variables are infrastructure for the lived world, not substitutes for it.**

Topographic biopower working definition:

> **The situated capacity of an actor or ecology to sustain, reproduce and project life-activity through a particular material landscape.**

Do not make `biopower` one master stat. Derive it through relationships among energy, access, embodiment, infrastructure, knowledge, organization, habitat, maintenance and history.

Primary life-activity rule:

- [x] combat belongs inside life activity; life activity is not filler between combat encounters.
- [x] actors should perform ordinary activity that would make sense even if the player were absent.
- [ ] first bounded Human/Rival life-activity slice: TRAVEL / WORK / MAINTAIN / OCCUPY.
- [ ] Cattlers continue distinct LURK / STALK / SNIPE / SWOOP / INFEST / EXPAND / WITHDRAW grammar rather than inheriting Human work behaviors.
- [ ] connect life activity to existing energy postings, spatial tasks, semantic events and material consequences rather than creating a parallel ecosystem administrator.

Coding sketch:

```c
typedef enum NfLifeActivity {
    NF_LIFE_TRAVEL = 0,
    NF_LIFE_WORK,
    NF_LIFE_MAINTAIN,
    NF_LIFE_OCCUPY
} NfLifeActivity;

void nf_lifeworld_tick(NfWorld *world, float dt)
{
    nf_energy_tick(&world->energy, world, dt);
    nf_update_actor_beliefs(world);
    nf_update_life_activity(world);
    nf_apply_material_consequences(world);
    nf_update_affordances(world);
}
```

Metabolic object slice — start with **generator + water/pump** rather than a general crafting/economy framework:

```c
typedef struct NfProcessObject {
    NfEntityId id;
    double input_energy;
    double throughput;
    double efficiency;
    float condition;
    float accessibility;
    NfAffordanceMask outputs;
} NfProcessObject;
```

Object principle:

```text
PROCESS OBJECT
    ↓
changes local throughput / access / water / light / transport
    ↓
changes actor costs and opportunities
    ↓
changes traffic / work / occupation / predation
    ↓
leaves material and informational traces
```

Maxim: **Do not simulate ecology as numbers that occasionally produce life. Simulate life whose continuing relations can be accounted for ecologically.**

# Next-build itinerary — candidate v1.0 Topographic Lifeworld

Do **not** branch/compile v1.0 until the v0.8 weak-point discrepancy is repaired and v0.8 -> v0.9 dependency order can close cleanly.

## Phase 0 — repair/close existing truth boundary

- [ ] instrument authoritative Cattler knee/foot hit classification.
- [ ] add deterministic `NF_HIT_KNEE` / `NF_HIT_FOOT` -> `locomotor_integrity < 1.0f` test.
- [ ] ensure yellow lower-body presentation matches authoritative hit geometry.
- [ ] prove reduced locomotor integrity alters swoop/relocation behavior rather than merely logging a number.
- [ ] human retest only after deterministic test is green.
- [ ] then accept/merge/archive v0.8; rebase/retarget/close v0.9 in dependency order; preserve flat-version archive history.

## Phase 1 — Compare-5 before new architecture

Compare five ecological/systemic references specifically for **how activity becomes visible world structure**, not for feature imitation:

1. **Rain World** — off-screen creature continuity, food-chain pressure, local embodiment.
2. **S.T.A.L.K.E.R. / A-Life** — regional simulation, actor travel, encounter emergence, world continuation outside player focus.
3. **Red Dead Redemption 2** — environmental/life-activity waterline and readable ordinary routines; selectively adapt, do not imitate content scale.
4. **Dwarf Fortress** — consequence-rich material history and emergent regime/succession logic; adapt low-resolution causality, reject universal high-detail simulation.
5. **The Long Dark** (or equivalent survival ecology reference) — patch scarcity, weather/topography changing local energetic cost, readable resource ecology.

For each comparison classify: `KEEP / ADAPT / DEFER / REJECT`, then map only accepted lessons to existing `nf_energy`, spatial, AI, semantics and world structures.

## Phase 2 — first questions before coding

1. **What are the smallest two or three actual life processes that can visibly change a place within one 2–3 minute human recording?** Recommendation: generator throughput + water/pump state + recurring Rival traffic/work.
2. **What evidence can Human Rivals legitimately perceive to build/update `NfEnergyBelief` without receiving `NfEnergySystem` truth?** Recommendation: nearby process-object state, observed traffic/work, direct resource condition, semantic reports; confidence and staleness remain bounded.
3. **How should the same local energy opportunity differ for a Human Rival and a Dream Cattler because their embodied topographies differ?** Recommendation: Human access/infrastructure/cover/work cost versus Cattler habitat/verticality/prey/disturbance/locomotor fit.
4. **Which consequence should become our first deliberately observable C26 evental transition?** Recommendation: a generator/pump or route change redirects recurring activity; redirected activity then changes future opportunity enough to reinforce or oppose the new pattern.
5. **What must remain below the waterline in v1.0?** Recommendation: exact energy scores, derived regime signatures, most trace bookkeeping and deep ecological accounting; expose only causal world consequences.

## Phase 3 — v1.0 bounded coding contract

- [ ] add actor-situated energy candidate valuation on top of existing belief search; never pass raw world energy truth into cognition.
- [ ] add four Human/Rival life activities: TRAVEL / WORK / MAINTAIN / OCCUPY.
- [ ] add two process objects: generator + water/pump with real local energetic/topographical consequences.
- [ ] route life activities through existing `NfControlFrame`, spatial routing/Fuzzy Rail and authoritative world interactions.
- [ ] produce first ecological waterline: process-object motion/state + recurring traffic + one or two persistent/diagnostic traces + ambient response.
- [ ] add minimal C26 structural-transition diagnostics sufficient to tell whether consequence has become cause; no general regime engine yet.
- [ ] retain Cattler ontology/strategy distinction; Cattlers respond to changed prey/habitat conditions through existing Cattler grammar.
- [ ] keep C27–C30 mostly as derived tests/diagnostics until the v1.0 slice produces enough history to justify richer succession machinery.

## Phase 4 — explicit v1.0 non-goals

- [x] no giant new ecosystem subsystem.
- [x] no universal GOAP/economic planner.
- [x] no omniscient actor energy map.
- [x] no universal trace TTL.
- [x] no single resilience/hysteresis/eventality/regime scalar as authoritative truth.
- [x] no final fauna/art/audio pass yet.
- [x] no 8 km² expansion yet.
- [x] no final Ghost Static economy yet.
- [x] no arbitrary scripted succession ladder.
- [x] no HUD energy/ecology meter as the proof of the system.

## Phase 5 — smallest human proof for v1.0

Target one readable local ecological story:

```text
PLAYER ENTERS ACTIVE PLACE
      ↓
Rivals visibly travel/work around functioning process objects
      ↓
player/world changes generator, pump or access condition
      ↓
actor beliefs and local energetic costs update through legitimate evidence
      ↓
traffic/work/occupation pattern changes
      ↓
Cattlers respond to changed prey/habitat geography through their own grammar
      ↓
player returns / observes consequence
      ↓
place visibly bears history without an ecology HUD
```

Acceptance questions:

- [ ] can a player infer that a place is active, degraded or reorganizing without seeing raw energy numbers?
- [ ] do Human Rivals and Cattlers reach different situated conclusions from the same universal energetic world?
- [ ] does at least one consequence feed back into future opportunity strongly enough to qualify as an evental transition?
- [ ] does the world remain an FPS first: movement/combat/networking remain stable and readable?
- [ ] can every visible ecological claim be traced back to authoritative world causation rather than decorative scripting?

## C31 — 15-minute possibility envelope / session dramaturgy — LOCKED

Core design principle:

> **As a general design target rather than a hard-written rule, almost any fifteen-minute segment of nightfall!punk should contain or plausibly offer the possibility of several interlocking modes of play: ordinary habitat life, traversal, environmental reading, casual physical challenge, social or Rival interaction, Quake-style arena fire, Dream Cattler stalking/ambush pressure, and an artifact/Temple-Run-like escalation.**

- [x] this is a **possibility envelope**, not a mandatory checklist, timer, encounter quota or scripted sequence.
- [x] a given fifteen-minute segment may remain calm, become overwhelmingly violent, or never instantiate one or more modes; the design target is that the local world has enough connected affordances for these possibilities to coexist.
- [x] desired session grammar may include: **wander -> notice -> learn -> traverse -> play/work -> socialize/contest -> investigate -> become uneasy -> evade/fight -> recover**, with ordering, omission and repetition determined by world state and player action.
- [x] **Legends of the Hidden Temple** contributes the loose progression grammar of traversal -> knowledge -> playful physical challenge -> high-pressure artifact expedition.
- [x] **Tomb Raider / Myst / Peripeteia** contribute environmental interpretation, physical puzzles, alternate routes, secrets and embodied navigation.
- [x] **Echo Point Nova / Quake / Urban Terror** contribute movement pleasure, spatial freedom, arena fire and readable direct confrontation.
- [x] **Alien: Isolation** contributes the possibility that exploration or casual activity can be interrupted by stalking/ambush pressure without requiring continuous combat.
- [x] casual play should primarily reuse the same movement, physics, objects, world interaction and social systems as combat rather than becoming a disconnected collection of minigames.
- [x] combat remains part of the lifeworld rather than a separate mode; likewise, habitat life is not filler between fights.
- [x] uncertainty is valuable: evidence of a Cattler or Rival may alter how the player reads an otherwise ordinary traversal/puzzle/habitat activity before any attack actually occurs.
- [x] Ghost Static may modulate transitions among these states through music, broadcast, salience and presentation, but must not force every local session into the same dramatic arc.
- [x] artifact runs should function as compositional peaks where traversal, learned environmental knowledge, route choice, social contest and predatory uncertainty can converge.
- [x] a mature session should support the player choosing to remain in low-intensity activity when conditions allow; the world should not automatically escalate merely because a pacing timer elapsed.

Session-design test:

> **Could the same local fifteen-minute window plausibly support helping someone repair an antenna, discovering environmental information, crossing the district through skilled movement, encountering or negotiating with Rivals, surviving Quake-style fire, noticing Cattler evidence, and deciding whether to risk an artifact run—without any of those activities feeling like they belong to a different game?**

Maxims:

- **The life of the habitat is what gives the gunfire something to interrupt.**
- **The possibility of interruption makes peaceful exploration suspenseful; the possibility of peace makes violence consequential.**
- **One world, many intensities.**

## C32 — post-scarcity horror / multidimensional difficulty architecture — LOCKED

Core thesis:

> **nightfall!punk is not primarily scarcity horror. It uses selective scarcity, but its distinctive horror is post-scarcity ecological infestation: a socially lively, materially capable world is threatened by colonizing structures that reorganize access, habitat, safety, meaning and reciprocity rather than merely making ammunition or food rare.**

Antagonistic value axis:

- [x] Human Rivals occupy a **multidimensional semi-cooperative rivalry space**: they may compete, trade, cooperate, heal, negotiate, contest resources and fight without collapsing into a single good/evil or ally/enemy scalar.
- [x] Dream Cattlers occupy the colonizing end of the antagonistic spectrum: increasingly non-reciprocal ecological domination, infestation and conversion of living relations into predatory infrastructure.
- [x] rare/major **Cattler Goliath** encounters may express a Hexxus-like concentration of that colonizing logic, while ordinary Cattlers remain its diffuse distributed form.
- [x] Goliath difficulty should come primarily from **structural persistence, changing phases, ecological control and topographical consequences**, not merely an inflated health bar.
- [x] Human Rivals should remain comparatively human-scale and lethal; difficulty comes from intelligence, positioning, coordination, uncertainty and social context rather than sponge durability.

Post-scarcity horror rule:

- [x] abundance must be allowed to exist: social spaces, music, repair, food, ordinary tools, shared infrastructure, recreation, fashion, traversal pleasure and Crew life can be enjoyable in themselves.
- [x] horror should therefore threaten **relations and conditions of flourishing**, not depend on making every ordinary resource miserable to obtain.
- [x] infestation may make an abundant thing dangerous, inaccessible, captured, redirected, contaminated, surveilled or socially costly without implying universal material shortage.
- [x] this supports the Chambers/Bookchin/Anthropocene direction: the question is not merely “can we survive scarcity?” but **“what kind of social ecology can continue flourishing when colonizing systems reorganize the commons?”**

Scarcity-budget reallocation:

- [x] reduce dependence on chronic ammo/food/med scarcity as the principal difficulty lever.
- [x] spend more of the difficulty budget on **lethality**: mistakes can matter quickly and opponents need not be bullet sponges.
- [x] spend more on **endurance**: dangerous expeditions, long pursuit arcs, Goliath phases, contested artifact runs and ecological recovery can demand sustained composure.
- [x] spend more on **productive recurrence / grinding**: repeated play may deepen mastery, relationships, habitat condition, route knowledge, reputation, equipment familiarity and ecological history.
- [x] grind should normally reproduce the lifeworld rather than function as an arbitrary stat wall; repetition should change knowledge, place, social possibility or player competence.
- [x] avoid “do the solved action 40 more times” padding. Repetition earns its place when the world, relationship, route, pressure or player understanding meaningfully changes.

Social/fun baseline:

- [x] desired broad accessibility = **Destiny / Cyberpunk 2077-style social density, build/play expression, wandering pleasure and immediate fun**, adapted to nightfall!punk rather than copied literally.
- [x] ordinary movement, shooting, social interaction and exploration should be pleasurable before the player engages the hardest systems.
- [x] psychological difficulty may be high without requiring academic expertise; the player should often feel challenged by uncertainty, intuition, spatial reasoning, timing, pressure and interpretation.
- [x] mastery may be demanding; basic intelligibility must remain generous.

### Four-layer difficulty precedence

Difficulty should generally compound in this order:

1. **Embodied arena difficulty — Tomb Raider inside Quake**
   - [x] first difficulty layer = read physical space, traverse it, manipulate mechanisms, discover routes and solve embodied spatial problems while retaining Quake-style movement freedom.
   - [x] difficulty comes from geometry, timing, orientation, route memory, momentum, affordance recognition and physical sequencing before abstract puzzle logic.
   - [x] skill floor should remain approachable; skill ceiling may be very high through route optimization and movement mastery.

2. **Predatory interruption — Tomb Raider while stalked like Alien: Isolation**
   - [x] second layer = preserve the same traversal/puzzle problem while stalking, ambush risk, sound uncertainty, Cattler evidence or Rival pressure changes the cost of attention and hesitation.
   - [x] the puzzle should usually remain valid under pressure rather than disappear into a separate combat mode.
   - [x] predatory difficulty is driven by uncertainty, persistence, route denial, sensory interpretation and forced improvisation more than raw HP.

3. **Psychological / intuitive physical puzzle difficulty**
   - [x] third layer = problems solvable primarily through embodied intuition, pattern recognition, environmental reading, analogy, memory, timing, perspective, cause/effect and “this feels physically right” reasoning.
   - [x] this is the preferred high-difficulty puzzle layer because it preserves first-person embodiment and does not require specialist academic knowledge.
   - [x] clues should exist materially in the environment; difficulty may come from noticing and interpreting them, not from withholding all evidence.

4. **Cognitive / formal physical puzzle difficulty**
   - [x] fourth layer = optional or higher-tier problems using explicit logic, arithmetic, spatial combinatorics, sequencing, Sudoku-like constraint satisfaction, code-breaking or other formal reasoning embedded into physical mechanisms.
   - [x] cognitive puzzles should remain materially grounded and diegetic rather than becoming detached worksheet screens.
   - [x] these may guard optional shortcuts, secrets, artifact advantages, high-value knowledge or specialist routes more often than mandatory basic progression.

Difficulty principle:

> **Embodiment first; predation second; intuition third; formal cognition fourth.**

### Difficulty settings recommendation

- [x] use **presets plus advanced custom axes**, rather than one global “enemy HP” slider.
- [x] preset names may be thematic, but every preset must resolve to transparent underlying variables.
- [x] custom difficulty should independently expose at least:
  - traversal forgiveness / movement assistance
  - player and enemy lethality
  - Rival coordination/aim/perception pressure
  - Cattler stalking persistence / sensory acuity / ambush pressure
  - encounter endurance / recovery generosity
  - intuitive-puzzle hint cadence
  - formal-puzzle hint/simplification level
  - recurrence/grind pacing where progression systems exist
- [x] difficulty must not secretly change faction morality, world truth or core causal rules.
- [x] accessibility options are not “easy mode penalties”; control assistance, readable clues, puzzle hints and sensory accommodations should remain separable from combat challenge.

Recommended preset philosophy:

- [x] **Explorer / Story:** broad traversal forgiveness, lower lethality and stalker pressure, generous intuitive/formal hints; preserves systemic world and story.
- [x] **Street / Standard:** intended baseline; Tomb Raider-style traversal and Quake fire remain readable, Cattler stalking matters, puzzle hints are restrained.
- [x] **Temple / Hunter:** higher lethality, more persistent stalking, tighter traversal forgiveness, fewer intuitive hints, longer sustained encounter pressure.
- [x] **Apophis / Expert:** severe but legible; high lethality/endurance/stalker pressure, minimal automatic hints, formal puzzles remain unsimplified; never relies on arbitrary HP inflation as the main difficulty mechanism.

### Endurance / boss rule

- [x] long encounters are legitimate when the **problem changes while duration accumulates**.
- [x] Cattler Goliaths may require dismantling infestation supports, changing topography, exploiting locomotor/ecological weaknesses, solving mechanisms under pursuit, coordinating with Rivals or surviving multiple behavioral phases.
- [x] “effective HP” may therefore be distributed across structures, access conditions, phases and ecological dependencies rather than stored entirely in one health number.
- [x] if the player has already solved the encounter and only repetition remains, shorten it or introduce a genuinely new state.

### Grind / recurrence rule

- [x] preferred grind = **mastery grind + social grind + ecological grind + route/knowledge grind**, with statistical gear grind secondary.
- [x] repeated Crew work can strengthen relationships and local habitat; repeated Rival contact can alter trust/compact history; repeated routes can build player mastery and world traces; repeated Cattler pressure can reshape ecology.
- [x] progression should reward continued inhabitation of the world rather than make the world feel like a resource treadmill.
- [x] avoid mandatory low-information repetition merely to reach a numerical gate.

### Psychological difficulty rule

- [x] psychological difficulty should often arise from **attention competition**: moving while listening, solving while hunted, deciding whether a Rival can be trusted, reading incomplete evidence, remembering routes, judging whether a habitat is safe and choosing when to disengage.
- [x] the player should frequently know enough to make a reasoned choice without knowing enough to feel certain.
- [x] uncertainty must remain fair: hidden information should have legitimate evidence channels even when those channels are incomplete.

Maxims:

- **Post-scarcity does not mean post-danger.**
- **The horror is not that there is nothing; the horror is that something is converting what already exists into a predatory order.**
- **Lethality over sponge HP; endurance through changing problems; grind through meaningful recurrence.**
- **Embodiment first; predation second; intuition third; formal cognition fourth.**
- **Make mastery difficult; make the world intelligible.**
- **A Goliath should be hard to uproot, not merely slow to subtract.**
