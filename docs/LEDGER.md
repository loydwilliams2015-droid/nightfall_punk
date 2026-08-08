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
