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
- [x] epistemic contract: `server truth != individual knowledge != group report`
- [x] faction identity is distinct from relationship state
- [x] Human Rivals = secondary antagonism / temporary terrain antagonism / negotiable
- [x] Dream Cattlers = primary ecological antagonism / semi-permanent environmental antagonism / always non-negotiable
- [x] Dream Cattler `predatory` and `non-negotiable` are independent axes; rare hostile/non-predatory disposition may permit avoidant detente without diplomacy
- [x] technical `Rancher` identifiers may remain temporarily; player-facing term is **Dream Cattler**
- [x] ecological score topology: Player Crew + teammates contend with Dream Cattlers over habitat/ecology; Player Crew and Rival Crews compete over ecological resources
- [x] score-play eventually measures control, preservation, extraction, access, denial and resource contest as well as direct combat
- [x] formal genre identity = **Neo-Xennial Shooter / Neo-Xennial FPS**
- [x] subtype = **Systemic Arena FPS**
- [x] genre thesis = Quake immediacy + Deus Ex systemic possibility + Halo spatial clarity + F.E.A.R. encounter intelligence + contemporary simulation/network infrastructure
- [x] manifesto = **The neo-xennial shooter continues the FPS future that the early 2000s started: fast, physical, systemic, open, and unpredictable.**
- [x] Ghost Static / Apophis replaces Dragon Master lore: Ghost Static is the public pirate-radio arena-conductor identity; Apophis is the deeper mythic/signal layer
- [x] aesthetic target = PG-13 ambient/paranormal/psychological cyberpunk horror, not splatter/gore-centric horror
- [x] aesthetic engine remains a later boxed-off phase so presentation does not rewrite movement/network/combat/AI truth

## Version / archive status

- [x] **v0.1 Foundation** — `archive/v0.1`
- [x] **v0.2 Movement** — `archive/v0.2`
- [x] **v0.3 Networked Movement** — `archive/v0.3`
- [x] **v0.4 Combat** — `archive/v0.4`
- [x] **v0.5 Agent Intelligence Framework** — `archive/v0.5`
- [x] **v0.6 Encounter Intelligence** — accepted baseline; archived at `archive/v0.6` from merge `a8f52f1c6d91107a0cbe53e255d4bddde1a6a968`
- [x] **v0.7 Spatial Ecology / Situated Agency** — human accepted from Pop!_OS recordings + server trace; PR #10 merged as `f656fb1731271c496076934ed9e850143021aa2c`; archived at `archive/v0.7`
- [ ] **v0.8 Dream Cattler Habitat Ecology** — compiled CI-green candidate on `build/v0.8-dream-cattler-ecology`; PR #11 remains draft/open; do not merge/archive until Pop!_OS human acceptance closes

# Regression-protected baseline through v0.7

## v0.1–v0.4 truth substrate

- [x] fixed simulation and headless/graphical split
- [x] movement/Fuzzy Rail owns step/vault/mantle/ladder/local physical traversal
- [x] ENet transport is behind `nf_net`
- [x] server validates client intent; snapshots + reconciliation remain authoritative
- [x] carbine/pistol ammo/reload/hitscan/rewind/damage/death/respawn are authoritative combat truth
- [x] relationship/faction policy gates damage

## v0.5–v0.6 Human Rival intelligence

- [x] limited perception, imperfect memory/confidence, semantic hearing, utility and cover affordances
- [x] Human Rival AI emits ordinary `NfControlFrame`; no teleport or privileged damage path
- [x] finite yaw/pitch tracking and aim settling
- [x] default two-slot soft pressure economy
- [x] contextual PRESSURE / FLANK_LEFT / FLANK_RIGHT / HOLD roles
- [x] damage-driven suppression/retreat
- [x] AI/encounter code has no raylib/ENet dependency

### v0.7 carry-forward watch

Human footage accepted the v0.7 thesis. One non-blocking follow-up remains:

- [ ] a badly wounded Human Rival can remain in `RETREAT` too long after reaching safety because no post-retreat recovery/withdrawal resolution exists yet
- [x] do **not** hide this with an arbitrary timer; later solve it through a coherent life-state/support affordance

## v0.7 Spatial Ecology / Situated Agency — accepted contract

Thesis:

> Agents can traverse the whole arena, choose meaningful regions, temporarily inhabit local tasks, coordinate occupation, remember recent spatial history, and respond to authoritative events without continuously solving the entire world.

Hierarchy:

`region chooses context -> local task chooses purpose -> action chooses behavior -> Fuzzy Rail handles physical execution -> server validates truth`

- [x] 0.40 km² / 500 m x 800 m graybox = 5% of eventual 8 km² map-area target
- [x] deterministic 24-region coarse graph
- [x] bounded next-hop routing
- [x] five local samples/query
- [x] top-four bounded deep regional candidates
- [x] spatial preference: congestion -> ally support -> enemy threat -> exposure -> pressure -> objective value
- [x] global roaming + local ROAM/WATCH/INVESTIGATE/OCCUPY/PRESSURE/FLANK/REGROUP/CONTEST
- [x] task hysteresis / claims / visitation memory / incident memory / deterministic variation
- [x] F4 region diagnostic overlay
- [x] server-side field reasoning; clients receive consequences rather than whole tactical scratch state
- [x] no giant whole-map influence texture/grid
- [x] no general GOAP
- [x] human recording proved cross-region relocation and maintained v0.6 combat competence

# v0.8 — Dream Cattler Habitat Ecology — ACTIVE BUILD

## Contract thesis

> Dream Cattlers are Goliath-scale ecological predators whose power comes from height, lurking, long-range prey pressure, swooping commitment and hyper-exploitative colonization of space. Their strength is also a weakness: mature infestation creates ecological inertia, while feet/knee damage collapses spatial dominance. Individuals remain epistemically limited; packs coordinate through imperfect evidence; rare Director-conditioned hunt surges create dogpile fights; rare loners hunt independently. Exact ecological scoring stays server-side and reaches the player as sense-data through world behavior, not score numbers.

Pipeline:

`authoritative world + semantic events`

`-> Human Rival perception/utility/spatial/encounter path`

`-> Dream Cattler habitat + pack-predator path`

`-> ordinary NfControlFrame intent`

`-> shared movement/Fuzzy Rail + authoritative combat`

`-> world truth + ecological consequences`

No Cattler subsystem may manufacture hits, damage, hidden transforms, teleportation, or privileged traversal.

## B1 — habitat value order

Locked order:

1. **habitat continuity**
2. **resource / ecological value**
3. **prey evidence**
4. **disturbance**
5. **territorial pressure**
6. **access / travel cost**

- [x] implemented as bounded server-side habitat scoring
- [x] infestation continuity and local territorial inertia influence residency
- [x] predatory/non-predatory dispositions interpret disturbance differently
- [x] travel cost remains subordinate to belonging

Maxim: **Human Rivals optimize occupation. Dream Cattlers optimize belonging.**

## B2 / B6 / B16 — infestation and colonization

- [x] Cattlers linger/lurk and deepen infestation while physically occupying a region
- [x] unoccupied infestation decays slowly instead of disappearing instantly
- [x] infestation creates residency/inertia rather than raw health/damage buffs
- [x] expansion requires physical use of new territory; adjacent regions never become magically infested from a numeric threshold
- [x] crossing a foothold threshold contributes to internal expansion accounting
- [x] killing the current occupant does not instantly erase the region's infestation history
- [ ] future presentation may express mature infestation through richer environmental traces/audio/resources; v0.8 proves behavior first

Natural rhythm:

`LINGER -> LURK -> PREY -> EXPLOIT -> EXPAND -> LINGER`

## B3 / B5 — epistemic contract and pack blackboard

Individual Cattlers may know:

- firsthand sight
- audible/semantic evidence
- last-known prey position
- coarse region activity only when grounded in personal/pack evidence
- imperfect secondhand reports

Pack blackboard may share:

- approximate prey region / position hint
- evidence type, confidence and age
- watched/occupied hunting space
- territorial disturbance
- injury/death/hunt outcomes

Pack blackboard may **not** share:

- exact current unseen prey transform
- exact unseen velocity
- another actor's entire perception state
- global LOS
- perfect route knowledge of prey

- [x] firsthand visual evidence is full-confidence
- [x] secondhand pack evidence is confidence-reduced and expires
- [x] reports can become stale/wrong
- [x] loners ignore ordinary pack-report coordination
- [x] authoritative regional `prey_activity` remains available to Director/event eligibility and diagnostics but no longer directly raises an individual Cattler's habitat preference unless that Cattler has personal/pack evidence for the region
- [x] one lower-commons habitat anchor is placed within normal first-contact perception distance so the human demo can expose the system without injecting hidden knowledge or spawning directly on the player

Maxim: **server truth != individual knowledge != pack report**.

## B4 — predator grammar / ambush

Cattler modes in the v0.8 graybox vocabulary:

`LURK / STALK / SNIPE / SWOOP / INFEST / EXPAND / RECOVER / WITHDRAW / AVOID`

- [x] distant firsthand prey -> long-range SNIPE pressure
- [x] medium-range viable prey -> committed SWOOP opportunity
- [x] secondhand evidence -> LURK/STALK/reposition rather than psychic direct attack
- [x] normal pack behavior distributes through different evidence/regions rather than Human-Rival PRESSURE/FLANK roles
- [x] SWOOP in v0.8 is deliberately a **graybox committed sprint/jump approach**, not a final flight/aerial-physics system
- [ ] human recording must prove whether the proxy reads as swooping enough to earn a dedicated aerial traversal primitive later

Target fear:

> Seeing one Cattler must not answer the question “where is the threat?”

## Goliath morphology / David counterplay

- [x] Dream Cattler combat silhouette is taller than humans in server hit geometry and client presentation
- [x] dedicated `KNEE` and `FOOT` hit zones added
- [x] knee/foot hits reduce separate locomotor integrity as well as normal health
- [x] lower locomotor integrity reduces willingness/range for relocation and suppresses confident swoop behavior
- [x] client graybox gives knee/foot test bands without pretending these are final art
- [ ] movement collision capsule is still shared/human-sized in this candidate; true tall low-clearance navigation remains a deliberate post-compile watch rather than destabilizing shared movement before human proof

David principle:

> Do not out-stat Goliath. Attack its locomotion, break vertical/spatial dominance, deny prey, and make its colonized space stop working for it.

## B8 / B13 / B14 — internal ecological ledger, player sense-data

Authoritative internal channels:

- **INFESTATION**
- **PREY**
- **DISPLACEMENT**
- **EXPANSION**
- **SURVIVAL**

- [x] individual Cattler and aggregate ecology ledgers are distinct
- [x] Cattler-caused damage/death contributes graded prey success; kills are not the only ecological value
- [x] physical occupation/foothold contributes infestation/expansion
- [x] exact values are emitted only in server/debug diagnostics
- [x] ordinary client HUD deliberately does **not** display Cattler score numbers

Player-facing contract:

> **The scoreboard measures the ecology for the simulation; the world expresses the scoreboard for the player.**

Sense-data should eventually include persistent lurking, repeated perch use, territorial recurrence, prey pressure, route abandonment, colonization traces and Ghost Static interpretation—not `+5 INFESTATION` popups.

## B9–B12 — 90 / 5 / 5 social ecology

Target distribution is an ecological/encounter distribution, not a random personality reroll each tick:

- **~90% normal pack-predator ecology** — lurk / stalk / watch / snipe / swoop with imperfect blackboard evidence
- **~5% HUNT_SURGE / dogpile ecology** — rare zombie-horde-style convergence
- **~5% true loners** — persistent seeded solitary disposition

- [x] default v0.8 demo population = **3 Cattlers**
- [x] deterministic seeded ~5% loner disposition in normal profile
- [x] `PACK` debug profile forces pack-linked Cattlers
- [x] `LONER` debug profile forces solitary behavior
- [x] `HORDE` debug profile forces 4–5 Cattlers + HUNT_SURGE
- [x] normal HUNT_SURGE eligibility is rare, ecology-conditioned and followed by cooldown/repose
- [x] Director/event context changes convergence incentives; it does not directly author control inputs or hidden prey coordinates

## B17–B20 — ecological counterplay / defeat semantics

Player/crew may weaken infestation by:

- wounding/killing occupants
- foot/knee locomotor attacks
- denying prey
- reoccupying/contesting valuable regions
- disrupting useful routes/perches/resources later
- forcing relocation away from accumulated colonization value

Hierarchy:

`CATTLER KILLED = individual defeat`

`INFESTATION COLLAPSED = local ecological defeat`

`CATTLERS DISPLACED = regional strategic defeat`

`SPECIES ABSENT = temporary world state, not permanent extermination`

- [x] v0.8 region infestation persists temporarily after occupant death and decays without reoccupation
- [ ] full human reclamation/objective semantics remain later because objective/resource systems are not yet implemented

## Ghost Static / Director boundary

- [x] Director may conceptually access authoritative state for Events
- [x] current HUNT_SURGE scaffold represents event-level convergence authority
- [x] Director may alter context/event eligibility, never inject exact hidden prey transforms into actor cognition
- [ ] no full Ghost Static pacing/story Director is implemented in v0.8

Maxim: **Ghost Static conducts conditions; the ecology performs them.**

## Existing / advancing / par checklist

### Existing systems that must remain green

- [x] v0.1 fixed simulation
- [x] v0.2 movement/Fuzzy Rail
- [x] v0.3 ENet authority/prediction/reconciliation
- [x] v0.4 authoritative combat/hitscan/rewind/death/respawn
- [x] v0.5 Human Rival perception/memory/utility/affordances
- [x] v0.6 finite tracking/pressure/suppression
- [x] v0.7 24-region / 0.40 km² spatial ecology

### Advancing systems targeted in v0.8

- [x] separate `nf_cattler` AI family; not a renamed Human Rival controller
- [x] three default Dream Cattlers; five maximum in current lab
- [x] habitat score + infestation persistence
- [x] imperfect pack evidence
- [x] lurking/stalking/sniping/swoop-proxy behaviors
- [x] pack/loner/horde debug profiles
- [x] rare HUNT_SURGE scaffold
- [x] knee/foot hit zones + locomotor integrity
- [x] internal ecological ledger + player-facing sense-data boundary
- [x] Cattlers emit ordinary `NfControlFrame` and use shared combat/movement truth
- [x] reciprocal Human Rival perception/response now includes Dream Cattlers through the same limited LOS/audible evidence and relationship gate; Human-Rival truce does not neutralize non-negotiable Cattlers
- [x] reciprocal Rival/Cattler perception behavior has an automated contract test; no forced target injection was introduced
- [ ] true aerial swoop physics deferred unless human proof shows the graybox proxy is insufficient
- [ ] true faction-specific tall collision capsule is a watch item after compile/human proof

### Par / Compare-5 for v0.8

Behavioral/design references only; not claims about proprietary internals.

1. **Rain World** — creatures should appear to participate in an ecology whose purposes exceed the player
2. **Alien: Isolation** — stalking uncertainty, reacquisition limits and predator pressure rather than psychic pursuit
3. **S.T.A.L.K.E.R.** — world presence, roaming actors and persistent location-level danger
4. **The Forest** — observation, approach/withdrawal/escalation and territorial hostility rather than permanent charge behavior
5. **Monster Hunter: World** — morphology, habitat and territorial behavior should make the creature feel like it belongs somewhere

Synthesis:

> Rain World autonomy + S.T.A.L.K.E.R. world presence + Alien uncertainty + The Forest escalation + Monster Hunter habitat legibility, filtered through nightfall!punk's bounded deterministic server architecture.

## v0.8 automated compile gate

- [x] CMake version advanced to 0.8
- [x] new `nf_cattler.c/.h` source family
- [x] AI contract extended with Dream Cattler + reciprocal Rival/Cattler tests
- [x] old Human Rival combat/encounter/spatial smokes isolate Cattlers so regression evidence stays interpretable
- [x] `cattler-smoke` isolates server-side three-Cattler pack habitat persistence rather than failing because a passive netbot begins outside prey-sensing range
- [x] local helper supports `NF_CATTLER_COUNT` + `NF_CATTLER_PROFILE`
- [x] full client presents tall orange Cattlers and graybox knee/foot bands without exposing ecology score numbers
- [x] strict graphical compile green
- [x] 4/4 CTest green
- [x] combat smoke green
- [x] encounter smoke green
- [x] spatial smoke green
- [x] Cattler ecology smoke green
- [x] GitHub Actions candidate gate green; this documentation reconciliation is the final exact-head rerun gate before human testing

## v0.8 human acceptance checklist

Record roughly **2–3 minutes** if possible. Default run first.

- [ ] title reads `v0.8 - DREAM CATTLER HABITAT ECOLOGY LAB`
- [ ] client reaches `AUTHORITATIVE + PREDICTED`
- [ ] red Human Rivals and tall orange Dream Cattlers are visually distinguishable
- [ ] ordinary movement/combat/network behavior remains stable
- [ ] observe the deliberately nearby first-contact Cattler acquiring prey through ordinary perception rather than hidden server knowledge
- [ ] observe at least one Cattler lingering/infesting rather than constantly beelining to player
- [ ] observe Cattlers redistributing after prey evidence or LOS changes
- [ ] look for a situation where visible and unseen Cattlers create ambush uncertainty
- [ ] aim low at yellow knee/foot graybox bands and compare mobility/commitment afterward
- [ ] watch whether Human Rivals and Cattlers recognize/attack each other rather than behaving as mutually invisible systems
- [ ] note whether Cattlers ever look like “Human Rivals, but tall”; that is an explicit failure condition
- [ ] note whether three Cattlers are frightening because of behavior rather than raw population pressure
- [ ] kill/escape from at least one Cattler if practical and observe whether regional danger appears to persist
- [ ] allow at least one player death/respawn if practical
- [ ] use F4 briefly to retain v0.7 regional context
- [ ] note anything **extremely right** as well as anything obviously wrong

After default recording, capture:

```bash
tail -n 200 build/server.log
```

Optional diagnostic A/B after default:

```bash
NF_AI_COUNT=0 NF_CATTLER_COUNT=1 NF_CATTLER_PROFILE=loner ./nightfall.sh local
NF_AI_COUNT=0 NF_CATTLER_COUNT=5 NF_CATTLER_PROFILE=horde ./nightfall.sh local
```

Only record these if the default run leaves the social difference ambiguous. The default video + server trace remain the primary acceptance evidence.

## Deliberate v0.8 boundaries

- [ ] no final Cattler art/audio
- [ ] no final flight/aerial navigation system
- [ ] no full Ghost Static Director
- [ ] no autonomous teammate brain
- [ ] no full objective/resource/reclamation loop
- [ ] no final player-facing ecological sense-data presentation beyond behavior itself
- [ ] no general GOAP
- [ ] no giant influence grids
- [ ] no cross-match adaptive ML
- [ ] no 8 km² production map yet

## Standing question subroutine

`WATCH -> QUESTION -> COMPARE WITH CURRENT CODE -> KEEP / ADAPT / DEFER / REJECT -> CODING CONTRACT -> COMPILE -> HUMAN VIDEO -> REASSESS`

Before every compile ask:

1. What did we learn that changes the code?
2. What merely confirms what we already have?
3. What is attractive but premature?
4. What adds complexity without proportionate gameplay value?
5. What is the smallest implementation that lets the next human recording prove/disprove the idea?
