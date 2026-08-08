# nightfall!punk master ledger

Versioning rule: **one flat version = one coherent development contract.** Git commits record fine-grained work. Completed versions are preserved as immutable `archive/v0.X` branches before `main` advances. No nested subversions.

## Locked project decisions

- [x] C + raylib; Linux-first
- [x] fixed 60 Hz simulation; authoritative dedicated server
- [x] server truth + client prediction/reconciliation
- [x] four-player target for now; possible five later
- [x] round-based story-oriented systemic arena FPS
- [x] placeholder/graybox mechanics before original aesthetics
- [x] direct confrontation remains the gameplay emphasis
- [x] physical puzzle/mechanism play later creates rewards and route advantages under combat pressure
- [x] Fuzzy Rail = shared local geometric traversal intelligence, not strategic pathing
- [x] Dynamic Affordance Graph = utility + bounded affordance queries + spatial pressure + blackboards; selective planning later
- [x] World Semantic Alerts = explicit server-authored gameplay-event layer; AI never infers critical world truth from packet arrival
- [x] AI knowledge is perspectival: server truth != individual knowledge != squad report
- [x] faction identity is distinct from relationship state
- [x] Human Rivals = secondary antagonism / temporary terrain antagonism / negotiable
- [x] **Dream Cattlers** replace the former Nightmare Ranchers in player-facing lore: primary ecological antagonists / semi-permanent environmental antagonism / always non-negotiable
- [x] existing `Rancher` identifiers may remain temporarily in implementation/technical notes until a coordinated rename pass; player-facing terminology is **Dream Cattler**
- [x] Dream Cattler `predatory` and `non-negotiable` are independent axes; a rare seeded hostile/non-predatory Dream Cattler may permit avoidant detente without diplomacy
- [x] ecological conflict topology for score-play: **Player Crew + teammates contend with Dream Cattlers over ecology/habitat; Player Crew and Rival Crews compete with one another over ecological resources**
- [x] score-play should reward/measure ecological control, preservation, extraction, access, denial and resource contest alongside direct combat rather than reducing match value to kill count alone
- [x] player and rival factions are named **“[X] Crew”** from carefully adapted public-domain literary sources with cyberpunk-adjacent, proto-speculative, anti-imperial, colonial/post-colonial or cosmopolitan themes
- [x] individual crew-member names are similarly derived from characters in those same verified public-domain source texts, transformed enough to belong naturally to the nightfall!punk world
- [x] commercial-use naming gate: use only source texts and source-character elements verified public domain for the intended market; do not borrow expression, costume, characterization, translations, illustrations or naming innovations unique to later copyrighted adaptations
- [x] public-domain status and trademark risk are separate checks: before shipping a crew or character name, record source work, author, publication date/edition and public-domain basis, then perform a separate title/name trademark-clearance check
- [x] formal crew-member naming grammar = **public-domain literary given-name variation + Old English-derived modern surname**; example form: `Zora Ashdown — Silver Fleece Crew`
- [x] Old English surname layer should favor naturalized modern English surnames descended from landscape, settlement, occupational and place-name vocabulary rather than conspicuously archaic Anglo-Saxon personal names
- [x] surname vocabulary should subtly reinforce the ecological score-play world through inherited landscape terms such as **wood, field, ford, heath, ash, oak, down, ley/clearing and settlement**
- [x] naming semiotics = **literary given name = inherited cultural memory; Old English-derived surname = inherited ecology/landscape; Crew name = present social organization**
- [x] roster naming should balance recognizability and transformation: names must read as plausible contemporary/cosmopolitan people first and literary references second
- [x] current exemplar naming register includes forms such as **Zora Ashdown, Bles Fenwick, Alwyn Hartley, Wynn Ridley, Sulla Atwood, Radius Langley, Prim Hadley, Alquist Bromley, Jim Beckley, Julia Westwood, Davis Sutton, Jule Easton, Avis Ashford, Ern Woodward, Anna Fordham, Mora Underwood, Vash Oakley, Kuno Whitfield, Vas Alderton and Kunel Radford**; these are working roster examples subject to final public-domain provenance and trademark clearance
- [x] formal genre identity = **Neo-Xennial Shooter / Neo-Xennial FPS**
- [x] design subtype = **Systemic Arena FPS**
- [x] genre thesis = Quake-style immediacy + Deus Ex-style systemic possibility + Halo-style spatial clarity + F.E.A.R.-style encounter intelligence + contemporary simulation/network infrastructure
- [x] manifesto = **The neo-xennial shooter continues the FPS future that the early 2000s started: fast, physical, systemic, open, and unpredictable.**
- [x] Ghost Static / Apophis replaces Dragon Master lore: Ghost Static is the public pirate-radio arena-conductor identity; Apophis is the deeper mythic/signal layer

# v0.1 — Foundation — COMPLETE / ARCHIVED
- [x] fixed-tick C foundation, headless server shell, raylib client shell, smoke test
- [x] `archive/v0.1`

# v0.2 — Movement — COMPLETE / ARCHIVED
- [x] shared movement/traversal/Fuzzy Rail contract
- [x] `archive/v0.2`

# v0.3 — Networked Movement — COMPLETE / ARCHIVED
- [x] ENet abstraction + explicit protocol
- [x] authoritative server, prediction/reconciliation, remote interpolation scaffold
- [x] reconnect/session tokens, impairment simulation, libsodium readiness
- [x] four-client network smoke and Pop!_OS recordings accepted
- [x] `archive/v0.3`

# v0.4 — Combat — COMPLETE / ARCHIVED
- [x] two authoritative weapons: carbine + pistol
- [x] health, ammo, reload/switch state, body/head hitscan, bounded rewind, death/3-second respawn
- [x] client sends combat intent, never hit/damage truth
- [x] combat events: GUNFIRE / DAMAGE / DEATH / RESPAWN / RELOAD / WEAPON_SWITCH
- [x] four-client combat smoke with authoritative damage/death
- [x] Linux mouse-capture/focus defect fixed and isolated-port local launcher hardened
- [x] Pop!_OS human combat recording accepted
- [x] `archive/v0.4`

# v0.5 — Agent Intelligence Framework — COMPLETE / ARCHIVED

## Accepted thesis

Four server-controlled Human Rivals perceive the player through limited evidence, remember imperfectly, share bounded reports, select tactical intent with utility scoring, query local world affordances, move through the same movement/Fuzzy Rail system, fire through the same combat system, damage/kill the player, die and respawn. The framework remains reusable by teammates and later Dream Cattlers without forcing their motivational policies to become the same.

## v0.5 delivered

- [x] `NfControlFrame` unifies network and AI intent onto `NfMoveInput` + `NfCombatInput`
- [x] server AI does not impersonate a network client
- [x] server AI cannot teleport, bypass collision/ammo/reload, manufacture damage or skip authoritative respawn
- [x] network slot 0 = PLAYER; additional network slots = TEAMMATE; server AI = RIVAL
- [x] explicit relationship states: NEUTRAL / COOPERATIVE / TRUCE / CONTESTED / HOSTILE / NONNEGOTIABLE_HOSTILE
- [x] Player <-> Teammate cooperative; Human Rival relation separate from faction identity
- [x] debug Rival truce suppresses intentional damage without changing faction identity
- [x] Dream Cattler policy remains always non-negotiable while predation is an independent tunable disposition axis
- [x] fixed-capacity World Semantic Alert bus maps combat facts into GUNFIRE / DAMAGE / DEATH / RESPAWN semantics
- [x] Human Rival vision is bounded by range/facing/world occlusion
- [x] perception is staggered below simulation frequency
- [x] reaction delay is deterministic/seeded and non-zero
- [x] last-seen / last-heard positions + confidence decay
- [x] semantic hearing consumes gameplay alerts, not transport packets
- [x] no hidden authoritative player transform copied into Rival knowledge
- [x] squad reports are secondhand lower-confidence knowledge rather than telepathy
- [x] bounded utility modes: IDLE / INVESTIGATE / ADVANCE / ENGAGE / SEEK_COVER / RETREAT / RELOAD / TRUCE_HOLD
- [x] utility considers visibility/confidence, range, health and ammo
- [x] action hysteresis reduces tactical thrashing
- [x] ordinary decisions avoid general GOAP/planner cost
- [x] first Dynamic Affordance Graph slice: generated local cover candidates + travel/occlusion scoring + exclusive reservation
- [x] selected tactical goal feeds ordinary movement/Fuzzy Rail instead of bypassing physics
- [x] four Human Rivals by default; isolated one-Rival debug mode
- [x] PRESSURE / FLANK_LEFT / FLANK_RIGHT / HOLD role scaffold + shared blackboard
- [x] imperfect deterministic aim jitter + reaction delay + role-dependent burst cadence
- [x] AI reload/fire/hits/damage/death/respawn pass through existing combat truth
- [x] v0.4 prediction diagnostic ambiguity resolved: visible positional `corrections` separated from internal `resyncs`
- [x] AI library has no raylib or ENet dependency; no allocation required in AI tick path

## v0.5 verification / closure

- [x] strict-warning full build green in GitHub CI
- [x] 4/4 CTest contracts green
- [x] four-network-client + four-Rival combat smoke green
- [x] passive human-shaped client smoke proved Human Rival AI independently damages and kills the player actor
- [x] Pop!_OS graphical human recording reviewed: authoritative client, mobile Human Rivals, lethal combat/death/respawn and no obvious foundational movement/network/client crash regression
- [x] principal human-feedback finding: four individually competent agents created excessive aggregate lethality; this is an encounter-governance problem carried intentionally into v0.6 rather than a v0.5 architecture failure
- [x] `archive/v0.5` frozen at accepted v0.5 tree

# v0.6 — Encounter Intelligence — CURRENT BUILD

## Contract thesis

Preserve v0.5 agent competence while controlling **when, where and how much** of that competence becomes simultaneous lethal pressure. Four Human Rivals should remain dangerous, but movement, line-of-sight loss, target motion, damage, suppression and squad role allocation must create meaningful counterplay. Encounter governance must remain a separate bounded layer over AI intent rather than turning the v0.5 utility system into a giant monolith.

## Existing / regression protection

- [x] v0.2 movement + Fuzzy Rail remain the only physical traversal truth
- [x] v0.3 ENet / server authority / prediction remain intact
- [x] v0.4 weapon/ammo/hitscan/damage/death/respawn remain the only combat truth
- [x] v0.5 perception / memory / semantic hearing / utility / affordance architecture remains intact beneath v0.6
- [x] faction identity / relationship semantics remain distinct
- [x] network Player/Teammate and server Human Rival topology preserved
- [x] AI still emits ordinary shared control intent before movement/combat
- [x] no direct AI damage path added
- [x] no new raylib/ENet dependency introduced into AI layer
- [x] no dynamic allocation required in encounter tick path
- [x] project-owned C remains compiled with `-Wall -Wextra -Wpedantic -Werror`

## Advancing — encounter-governance layer

- [x] new `nf_encounter` module sits between v0.5 AI control generation and authoritative movement/combat execution
- [x] encounter state is fixed-capacity and per-agent
- [x] finite angular yaw/pitch tracking prevents instantaneous aim rotation
- [x] explicit aim-settle value separates target awareness from firing readiness
- [x] target motion raises firing-readiness requirements
- [x] own movement modestly raises firing-readiness requirements
- [x] line-of-sight loss causes aim settling to decay rather than retaining a ready firing solution
- [x] target changes reset aim settling
- [x] existing v0.5 deterministic reaction delay remains in force underneath aim settling

## Advancing — squad pressure economy

- [x] default squad pressure budget = **two pressure-authorized Rivals**
- [x] pressure authorization is a soft tactical budget, not a hard one-enemy-at-a-time shooter token
- [x] non-pressure agents may still take rare opportunistic shots or defend themselves at close range
- [x] pressure bids consider confidence, visibility, health, ammo and range
- [x] pressure authorization rotates as bids change
- [x] pressure authorization is distinct from immediate `ENGAGE` mode; an authorized actor may be investigating/repositioning until it has a viable firing solution
- [x] `--pressure-slots 0..2` server flag
- [x] `NF_AI_PRESSURE_SLOTS=1` local-debug path

## Advancing — contextual squad roles

- [x] role allocation is no longer permanently tied to actor index
- [x] role rebidding occurs at bounded cadence rather than every simulation tick
- [x] strongest tactical bid becomes PRESSURE
- [x] other viable agents distribute into FLANK_LEFT / FLANK_RIGHT / HOLD
- [x] role rebidding uses existing partial knowledge rather than authoritative hidden player state
- [ ] sophisticated formation / negotiated multi-agent planning remains later

## Advancing — suppression / damage response

- [x] recent authoritative health loss raises a bounded suppression value
- [x] suppression decays over time
- [x] suppression slows tracking modestly
- [x] suppression raises required aim settling before fire
- [x] high suppression + reduced health can force a retreat response
- [x] respawn / major health reset clears stale suppression/settling behavior
- [ ] near-miss / incoming-fire suppression without actual damage is deliberately deferred until weapon/projectile/spatial-fire semantics justify it

## Advancing — Dynamic Affordance Graph / spatial reasoning

- [x] v0.5 cover generation/reservation remains active
- [x] v0.6 role/pressure decisions now provide additional context for how existing affordances are used
- [ ] global/local influence-map stack (threat / ally support / Dream Cattler pressure / objective value) deferred pending v0.6 footage
- [ ] coarse region graph / A* routing deferred pending evidence of navigation failure in v0.6 footage
- [ ] full route-congestion field deferred; existing exclusive cover reservations remain the first occupancy primitive

## Advancing — teammate / social AI

- [x] Player/Teammate relationship substrate remains cooperative and generic control architecture remains compatible with a future teammate policy
- [ ] live autonomous Teammate controller deliberately deferred: implementing one cleanly requires shared ally-target/support semantics rather than copying the Rival controller and inverting hostility
- [ ] polished Human Rival negotiation UX remains later; debug truce still proves relationship policy
- [ ] live Dream Cattler ecological brain remains later and must not be built as a renamed Human Rival policy

## Advancing — diagnostics / tuning

- [x] server prints role / mode / target / confidence / visibility / health / cover
- [x] v0.6 server diagnostics add pressure authorization / aim settle / suppression / aim error
- [x] graphical lab title/footer identify v0.6 Encounter Intelligence
- [x] HUD retains visible-correction vs resync distinction
- [ ] richer in-client per-Rival reasoning overlay remains a later ergonomics pass; current authoritative server trace is sufficient for first v0.6 human recording

## Automated contracts / compile gate

- [x] CMake project version advanced to 0.6
- [x] v0.1–v0.5 simulation/combat/network/relation/semantic/truce contracts retained
- [x] encounter test asserts pressure authorization never exceeds two
- [x] encounter test proves pressure authorization becomes active when targets are viable
- [x] encounter test bounds aim-settle and suppression values to [0,1]
- [x] direct-damage test proves suppression rises meaningfully
- [x] filtered encounter AI still eventually fires
- [x] first exact v0.6 CI compile green
- [x] 4/4 CTest contracts green: simulation_v06 / combat_contract_v06 / network_contract_v06 / encounter_intelligence_v06
- [x] four-network-client + four-Rival bounded-pressure combat smoke green: ~210 snapshots/client, 157 combat events, 50 damage events, 8 deaths; max prediction error ~0.095–0.107 m
- [x] passive-player encounter smoke green: 360 snapshots, 38 combat events, 18 incoming damage/death events and 3 incoming deaths in 12 seconds
- [x] passive smoke proves encounter governance did **not** make Rivals harmless
- [x] server trace demonstrates rotating pressure authorization and role changes with no more than two pressure-authorized agents
- [x] libsodium active in GitHub verification environment
- [x] client periodic ping request semantics rechecked after v0.6 presentation edit
- [x] final exact documentation-closed PR head CI green before merge
- [ ] Pop!_OS build/tests/smokes checked by human operator
- [ ] human v0.6 encounter recording reviewed
- [ ] `archive/v0.6` only after target-machine acceptance

## Par / Compare-5 synthesis for v0.6

Behavioral/design references, not claims about proprietary internals.

- [x] **F.E.A.R.** — preserve legible tactical intention while role/pressure allocation makes squad behavior more coherent than four independent shooters
- [x] **Halo 3** — pressure/repose and readable opportunities; dangerous opponents need not output maximum lethality every instant
- [x] **Left 4 Dead** — distinguish individual capability from encounter-level intensity; v0.6 applies that principle locally to a Human Rival squad rather than implementing a global Director
- [x] **The Last of Us** — believable human limitation: acquisition, tracking, uncertainty, damage response and recovery contribute to perceived intelligence
- [x] **Quake III / Unreal Tournament lineage** — preserve player movement as genuine combat counterplay rather than letting AI perfect tracking invalidate strafing/jumping/traversal

## Deliberate v0.6 boundaries

- [ ] no global AI Director / Ghost Static pacing controller yet
- [ ] no full tactical influence-map stack yet
- [ ] no coarse region/A* navigation rewrite yet
- [ ] no autonomous teammate combat brain yet
- [ ] no live Dream Cattler ecological behavior yet
- [ ] no general GOAP planner yet
- [ ] no polished negotiation/dialogue interface yet
- [ ] no rounds/objectives/ecological score system yet
- [ ] no final original art/audio requirement
- [ ] no difficulty menu yet; future difficulty should tune behavioral competence without granting psychic knowledge or bypassing physical rules

## Human v0.6 demo acceptance checklist

For the first v0.6 Pop!_OS recording:

- [ ] graphical title reads `v0.6 - ENCOUNTER INTELLIGENCE LAB`
- [ ] client reaches `AUTHORITATIVE + PREDICTED`
- [ ] movement/mouse/combat from v0.4–v0.5 remain functional
- [ ] four red Human Rivals remain active and capable of killing the player
- [ ] standing exposed is punished severely
- [ ] lateral movement / direction changes / jumping produce visibly better survival opportunities than standing still
- [ ] pressure feels less like four simultaneous perfect guns; look for 1–2 serious pressure sources while others move/hold/investigate
- [ ] hiding behind substantial solid geometry interrupts direct fire and causes investigation/search behavior
- [ ] reappearing requires reacquisition rather than apparent wall tracking
- [ ] shoot one Rival for meaningful damage without immediately killing it; look for reduced firing / retreat / reposition behavior
- [ ] player can kill a Rival and see normal Rival respawn
- [ ] a Rival can still kill player and normal 3-second player respawn remains intact
- [ ] no obvious teleportation / wall shooting / collision bypass
- [ ] note whether encounter rhythm now feels fair, too passive, still overwhelming, or unexpectedly good

## Current tuning watch

- [x] v0.5 excessive aggregate lethality is explicitly addressed by v0.6 encounter governance rather than a uniform accuracy/damage nerf
- [ ] inspect whether two default pressure slots are still too aggressive or have become too permissive
- [ ] inspect whether finite tracking is perceptible enough when the player reverses direction or changes elevation
- [ ] inspect whether non-pressure opportunistic fire reads as believable support rather than violating the pressure economy
- [ ] inspect whether suppression/retreat is visible during ordinary human fire, not only deterministic test injection
- [ ] inspect cover-selection usefulness and clumping around ladders/platforms/obstacles
- [ ] inspect whether stale-knowledge investigation is spatially convincing enough before investing in coarse routing
- [ ] prediction max-error spikes around death/respawn remain a watch item; automated passive encounter run reached ~1.263 m while ordinary visible correction counts remained low
- [ ] use v0.6 human footage to choose v0.7 emphasis among spatial influence/navigation, teammate/social intelligence, or ecological/director systems
