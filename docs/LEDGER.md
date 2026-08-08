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
- [x] Nightmare Ranchers = primary antagonism / semi-permanent environmental antagonism / always non-negotiable
- [x] Rancher `predatory` and `non-negotiable` are independent axes; a rare seeded hostile/non-predatory Rancher may permit avoidant detente without diplomacy
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
- [x] Pop!_OS human combat recording accepted: mouse/camera, authoritative handshake, movement, firing, ammo, weapon switching, damage, death/respawn and reload behavior considered good for current graybox
- [x] `archive/v0.4`
- [x] v0.4 correction-counter discrepancy carried forward for semantic cleanup: raw resync passes had been presented as if every pass were a visible correction

# v0.5 — Agent Intelligence Framework — CURRENT BUILD

## Contract thesis

Four server-controlled Human Rivals should perceive the player through limited evidence, remember imperfectly, share bounded reports, select tactical intent with utility scoring, query local world affordances, move through the same movement/Fuzzy Rail system, fire through the same combat system, damage/kill the player, die and respawn. The framework must remain reusable by teammates and later Ranchers without forcing their motivational policies to become the same.

## Existing / regression protection

- [x] v0.2 movement remains the single physical movement implementation
- [x] v0.3 ENet transport remains behind `nf_net`
- [x] v0.4 combat remains the single weapon/damage/death implementation
- [x] network clients still submit intent, never transforms/hits/damage
- [x] server AI does not impersonate a network client
- [x] shared `NfControlFrame` converges network and AI control onto `NfMoveInput` + `NfCombatInput`
- [x] server AI cannot teleport, bypass collision, bypass ammo, bypass reload, manufacture damage, or skip authoritative respawn
- [x] v0.4 alternating duel-faction scaffold removed: network slot 0 is PLAYER, additional network slots are TEAMMATE, and RIVAL is reserved for server-controlled Human Rival agents
- [x] remote actor presentation now colors by relationship semantics rather than simple faction inequality
- [x] project-owned code remains under `-Wall -Wextra -Wpedantic -Werror`

## Advancing — relationships / antagonism

- [x] explicit relationship states: NEUTRAL / COOPERATIVE / TRUCE / CONTESTED / HOSTILE / NONNEGOTIABLE_HOSTILE
- [x] Player <-> Teammate defaults cooperative
- [x] Human Rival relationship is separate from faction identity and defaults HOSTILE in v0.5
- [x] relationship policy gates damage in server hit validation
- [x] `--rival-truce` / `NF_RIVAL_TRUCE=1` debug path proves hostility can be suppressed without changing faction identity
- [x] Rancher relation always resolves to NONNEGOTIABLE_HOSTILE
- [x] Rancher profile API keeps non-predatory rarity externally tunable instead of hard-coding a final probability
- [x] rare hostile/non-predatory Rancher profile remains non-negotiable and biases avoidance/territorial behavior over pursuit
- [ ] polished player-facing negotiation UX is later
- [ ] live Rancher ecological behavior is later

## Advancing — World Semantic Alerts

- [x] fixed-capacity semantic event bus; no allocation in tick path
- [x] combat facts map into bounded semantic alerts
- [x] GUNFIRE carries hearing radius/lifetime
- [x] DAMAGE / DEATH / RESPAWN have explicit semantic forms
- [x] AI hearing consumes semantic alerts rather than transport packets
- [x] semantic query is local/radius bounded
- [ ] broader mechanisms/objectives/route/resource semantic vocabulary later

## Advancing — perception / memory / knowledge

- [x] Human Rival vision is bounded by range, facing and world occlusion
- [x] perception is staggered and runs below simulation frequency
- [x] reaction delay is deterministic/seeded and roughly human-scale rather than zero-latency
- [x] AI records last-seen and last-heard positions
- [x] knowledge confidence decays when evidence goes stale
- [x] AI can hear gunfire it cannot currently see
- [x] direct damage semantics can strengthen attacker knowledge
- [x] no authoritative hidden player transform is copied into Rival knowledge
- [x] squad reports are secondhand, lower-confidence knowledge rather than telepathy
- [ ] teammate controller behavior beyond shared relation/perception substrate remains later

## Advancing — utility / tactical intent

- [x] bounded utility modes: IDLE / INVESTIGATE / ADVANCE / ENGAGE / SEEK_COVER / RETREAT / RELOAD / TRUCE_HOLD
- [x] utility inputs include visibility/confidence, range, health and ammunition
- [x] action hysteresis reduces tactical thrashing
- [x] ordinary decisions do not require GOAP/planning
- [x] selective GOAP/planning remains architecturally reserved for later compound goals
- [x] AI updates are multi-rate rather than one giant 60 Hz brain

## Advancing — Dynamic Affordance Graph first slice

- [x] world geometry generates bounded cover candidates at server startup
- [x] affordance queries are local and capacity-limited rather than whole-world brute-force scans
- [x] cover evaluation considers travel cost, world occlusion and reservation
- [x] cover slots can be reserved by one AI actor
- [x] reservation prevents all agents selecting the same exclusive cover candidate
- [x] selected tactical goal feeds ordinary movement instead of bypassing physics
- [x] Fuzzy Rail candidate state can trigger AI ladder interaction / vault / mantle input
- [ ] coarse region/A* route graph beyond direct tactical goal steering remains a later expansion if human footage proves necessary
- [ ] full spatial influence fields (enemy threat / ally support / Rancher pressure / objective value) remain later

## Advancing — squad intelligence

- [x] four Human Rival AI by default; `NF_AI_COUNT=1` supports isolated debugging
- [x] ephemeral roles: PRESSURE / FLANK_LEFT / FLANK_RIGHT / HOLD
- [x] shared blackboard reports target position with reduced confidence
- [x] flanking roles bias lateral combat movement differently
- [x] cover reservation handles first occupancy conflict
- [x] per-agent updates are staggered to avoid synchronized CPU spikes
- [ ] sophisticated role bidding / formation / negotiated squad planning later

## Advancing — combat behavior

- [x] AI uses the existing carbine combat state
- [x] imperfect deterministic aim jitter
- [x] reaction delay before first fire
- [x] role-dependent burst cadence instead of uninterrupted aimbot fire
- [x] AI reload requests go through `nf_combat_start_reload`
- [x] AI shots go through the same hitscan/world occlusion/damage validation as network players
- [x] AI can damage/kill Player/Teammate actors only when current relationship permits
- [x] AI can be killed and respawns through the same combat truth
- [x] stuck detection can trigger bounded jump/strafe recovery

## Advancing — diagnostics

- [x] v0.4 prediction ambiguity resolved structurally: `resyncs` counts reconciliation/state rebuild passes; `corrections` counts threshold-crossing positional corrections
- [x] graphical HUD now labels `visible corrections` separately from `resyncs`
- [x] server prints each AI actor's id / role / mode / target / confidence / visibility / health / cover selection at a bounded cadence
- [x] automated AI-smoke server trace demonstrates mode transitions among ENGAGE / INVESTIGATE / RETREAT as line-of-sight/confidence changes
- [ ] richer in-client per-AI utility/knowledge overlay is deferred until after first human v0.5 behavior recording

## Automated contracts

- [x] relationship/damage/truce policy tests added
- [x] Rancher non-negotiable + tunable rare non-predatory disposition tests added
- [x] semantic hearing radius/lifetime tests added
- [x] AI perception/engage/fire-output test added
- [x] truce suppresses AI firing in deterministic test
- [x] legacy simulation/combat/network tests retained
- [x] exact v0.5 implementation full compile green in GitHub CI
- [x] 4/4 CTest contracts green: simulation / combat / network / agent intelligence
- [x] four-network-client + four-AI combat smoke green; automated network actors now choose targets through relationship permissions rather than `different faction`
- [x] corrected Player/Teammate/Rival topology passed the full compile, 4/4 tests, combined combat smoke and passive AI-kill smoke
- [x] passive human-shaped client + four-AI smoke green: AI alone produced 12 incoming damage/death events and killed the client twice in nine seconds
- [x] libsodium active in GitHub verification environment
- [ ] Pop!_OS clean build/tests/smokes green
- [ ] human v0.5 demo reviewed

## Stable-coherence gate

- [x] AI library has no raylib dependency
- [x] AI library has no ENet dependency
- [x] semantic and relationship layers have no raylib/ENet dependency
- [x] transport does not decide AI intent or hit outcome
- [x] AI does not apply damage directly
- [x] AI and network actors converge at shared control intent before movement/combat
- [x] AI knowledge is not server omniscience
- [x] Rancher and Human Rival motivational policies remain distinct even though lower layers are shareable
- [x] network-controlled teammates and server-controlled Human Rivals now have distinct faction identities consistent with their relationship semantics
- [x] no dynamic allocation required in AI tick path
- [x] implementation tree passed full strict-warning CI before merge
- [ ] target-machine behavior accepted before `archive/v0.5`

## Par / Compare-10 synthesis

Behavioral/design references, not claims about proprietary internals.

- [x] F.E.A.R. — legible tactical intent and reusable action vocabulary
- [x] Halo 2/3 — partial knowledge, scalable agent architecture, readable combat spaces
- [x] Alien: Isolation — future Rancher uncertainty/persistence model; not copied into Human Rival policy
- [x] Left 4 Dead — higher-level pressure/director logic remains separate from individual brains
- [x] The Last of Us — grounded search/reaction behavior and distinct AI families over shared technology
- [x] Killzone lineage — tactical position evaluation informs affordance scoring
- [x] Dishonored lineage — search/chase spatial reasoning informs stale-knowledge investigation
- [x] Prey 2017 — world possibilities/objects inform agent affordances
- [x] Quake / Unreal Tournament bots — combat remains compatible with arena movement
- [x] S.T.A.L.K.E.R. / A-Life lineage — future persistence/ecology inspiration for Ranchers, not an immediate v0.5 simulation target

## Deliberate v0.5 boundaries

- [ ] no final Rancher brain yet
- [ ] no polished negotiation/dialogue interface yet
- [ ] no full teammate tactical combat controller yet
- [ ] no global tactical influence-map stack yet
- [ ] no general GOAP planner yet
- [ ] no rounds/objectives/reward mechanisms yet
- [ ] no final original art/audio requirement
- [ ] no claim of production-final cover/pathfinding; first human footage determines the next navigation investment

## Human demo acceptance checklist

For the first v0.5 recording, verify visually rather than trying to prove every internal unit contract:

- [ ] client reaches `AUTHORITATIVE + PREDICTED`
- [ ] red Human Rivals are moving rather than stationary dummies
- [ ] Rivals acquire/approach/strafe instead of always bee-lining
- [ ] Rivals fire in bounded bursts and miss sometimes
- [ ] player takes authoritative damage from Rival fire
- [ ] at least one Rival can kill the player; 3-second player respawn still works
- [ ] player can kill a Rival; Rival respawn still works
- [ ] hiding behind solid geometry interrupts direct fire/line-of-sight behavior
- [ ] reappearing produces reacquisition
- [ ] no obvious wall shooting / teleportation / collision bypass
- [ ] movement/mouse/combat v0.4 regressions absent
- [ ] note anything surprisingly excellent or catastrophically wrong; silence on a requested behavior is not automatically interpreted as success

## Current tuning watch

- [x] correction diagnostic semantics separated before v0.5 human review
- [x] automated passive-client smoke proves Human Rival AI can independently damage and kill a player actor through normal server combat truth
- [ ] inspect whether four Rival default pressure is fun or instantly overwhelming
- [ ] inspect whether aim jitter/reaction/burst timing reads human rather than robotic
- [ ] inspect cover-selection usefulness versus direct-goal navigation failures
- [ ] inspect AI clumping and stuck recovery around ladders/platforms/obstacles
- [ ] inspect whether semantic hearing/search is visible enough to deserve richer debug overlay
- [ ] inspect prediction max-error spikes around death/respawn discontinuities; automated passive-client run reached ~1.285 m while ordinary visible correction counts stayed low
- [ ] use human footage to decide whether v0.6 should deepen navigation/affordances, social/teammate behavior, or broader world semantics first
