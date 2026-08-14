# nightfall!punk — v1.0 Parametric Events & Squad Cognition Addendum

Status: **accepted design contract / pre-implementation**

This addendum extends `docs/LEDGER_V1.0.md`. It does **not** supersede current v0.8/v0.9/v1.0 dependency, compile, CI, or human-proof gates. No compile is implied by this documentation update.

## Governing parametric rule

> **Whenever practical, gameplay behavior should be computed from observable world-state relationships rather than assigned as a fixed gameplay outcome constant.**

Preferred design chain:

`observable variables -> normalized relations -> expression/algorithm -> behavioral result -> bounded invariants`

Before accepting a fixed gameplay parameter, ask:

> **Can this quantity instead be derived from observable game-state variables?**

If yes, derive it. Fixed values remain legitimate for machinery, measurement, numerical stability, physical scale, protocol limits, and explicit invariants; they are suspect when used to directly author behavior that can be explained relationally.

Examples of suspicious direct-outcome constants include fixed revive duration, panic duration, retreat HP threshold, universal aggro range, or universal threat-memory TTL. Prefer expressions whose inputs are authoritative world facts or, for cognition, legitimately perceived/believed facts.

Generic form:

`Y = f(X1, X2, ... Xn)`

where each `Xi` is observable, authoritative, or legitimately inferred data appropriate to the system using it.

Time may participate in an expression, but elapsed time alone should rarely be the explanation for a world-state change.

### Healing / revival example

Do not define healing primarily as a constant such as `heal_duration_ms = 80`.

Prefer a rate relation such as:

`R_heal = R0 * F(healer_agency, patient_state, tool_affordance, environmental_stability, contamination, interruption_pressure, access)`

and:

`delta_heal = R_heal * dt`

The same injured actor can therefore recover differently in a protected clean room, in a contaminated stairwell, or under Dream Cattler pressure because the world-character relation differs.

## Build-question parametrics

Recommended build answers should, whenever applicable, identify:

1. **Observable inputs** — what facts exist in the simulation or legitimate actor belief?
2. **Relation/expression** — how are those facts combined?
3. **Behavioral output** — what does the relation control?
4. **Bounds/invariants** — what prevents numerical, network, or gameplay instability?

Questions should prefer relational invariants over arbitrary constants. Ask what evidence causes threat confidence to decay before asking for a threat-memory millisecond value.

Accepted workflow remains:

`3 consequential questions -> accept/override recommendations -> freeze contract -> implement -> subsystem checks -> explicit compile gate -> human evaluation -> ledger/archive`

## Event authority stack

### Lifeworld — ordinary systemic truth

Ordinary ecology, actors, objects, topology, contamination, resources, factions, and relationships produce consequences through normal rules.

### Ghost Static — limited Director authority

Ghost Static **is the Director in a bounded capacity**. Ghost Static may:

- notice or anticipate eligible situations through legitimate information,
- promote or announce events,
- weight already-lawful possibilities,
- organize circumstances through legitimate world mechanisms,
- invite or redirect participants,
- allocate bounded event pressure,
- curate Crescendos,
- amplify salience,
- frame outcomes afterward.

Ghost Static does **not** possess arbitrary omniscience and does not directly rewrite actor cognition.

> **Ghost Static can organize the party; Ghost Static cannot dictate what everyone does once they arrive.**

### Apophis — explicit deus ex machina scripting authority

Apophis owns exceptional omniscient scripting when story mode or Destiny-style event authorship requires it. Apophis may establish otherwise-impossible or narratively mandatory invariants, including major public/world events, guaranteed set pieces, authored arrivals/departures, extraordinary transitions, mythic coincidences, and campaign continuity requirements.

Architectural distinction:

`SYSTEMIC -> Lifeworld determines consequence`

`GHOST_STATIC -> bounded Director intervention selects/weights lawful possibilities`

`APOPHIS -> explicit authored intervention may establish necessary invariants`

Prefer:

`Apophis establishes an invariant -> Ghost Static curates the resulting situation -> Lifeworld actors determine most of what actually happens`

Stable necessity should open variable possibility rather than eliminate it.

## Panic Event contract

Accepted Panic Event principles:

- Panic is a **systemic crisis**, not merely an enemy-spawn command.
- Panic eligibility arises from ordinary world-state pressure, not a random Director declaration from nothing.
- Candidate inputs include hostile pressure, Player Crew vulnerability, contested traversal, Cattler occupation/infestation, resource/objective stress, contamination, and recent disturbance.
- The Director/Ghost Static may select among **already eligible** compositions using deterministic seeded variation.
- One **dominant local Panic composition** should combine multiple contributing pressures; distant regions may later sustain separate simultaneous Panics.
- Panic pressure should compose/decompose continuously. Diagnostic thresholds may label eligible/active/severe/recovering states, but actors respond to underlying relations rather than a magic `PANIC=true` truth.
- Recovery uses **hysteresis grounded in world state**. A small technical anti-thrashing interval may exist, but recovery is primarily caused by reduced threat, restored traversal, reduced contamination, stabilized Crew agency, resource recovery, etc.
- Panic resolves through world-state resolution, not a primary countdown timer; any maximum duration is a failsafe only.
- Resolution leaves modest lawful causal residue where appropriate.

Canonical causal chain:

`ordinary world conditions -> accumulating pressure -> Panic eligibility -> seeded composition -> ordinary actor response -> pressure decomposition -> causal residue`

## Crescendo Event contract

A Crescendo is an **authored possibility-space** in which a systemic crisis is deliberately made likely and legible.

### Stable identity + variable possibilities

> **Stable structure should generate variable possibilities, not merely variable decoration.**

Avoid:

`same encounter + randomized details`

Prefer:

`same systemic premise + genuinely different possible histories`

The stable layer is the event grammar: place, stakes, semantic objects, broad resolution problem, timing envelope, and lawful eligibility conditions. The variable layer includes routes, alliances, hazards, occupations, priorities, resource states, actor decisions, tactical solutions, and consequences.

Two runs should be recognizably the same Crescendo while being capable of materially different tactical and ecological histories.

### Authorship boundary

Designers may author:

- geometry and semantic objects,
- eligibility conditions,
- timing ranges,
- pressure opportunities,
- possible entry/approach routes,
- environmental affordances,
- escape/resolution conditions.

Do not ordinarily author exact actor behavior, exact attack timestamps, mandatory AI positions, or hidden cognition overrides.

Temporary Crescendo rules should normally be ordinary systems activated at unusual intensity or combinations. Rare special mechanisms should themselves be legitimate world objects/systems unless explicitly owned by Apophis.

### Scale, abandonment, failure

- One Crescendo should usually occupy a **connected local region/corridor with several interacting spaces**, not one room and not the entire map by default.
- The Player Crew may physically abandon a Crescendo. Abandonment is an action in the world, not an escape from simulation.
- Abandonment may leave infrastructure broken, resources lost, Rival gains, Cattler occupation, altered contamination, transformed objectives, or a harder/easier later return.
- Failure normally becomes **another valid continuing world state**, not an automatic encounter reload.

### Resolution semantics

Crescendo outcomes are graded and materially distinct rather than merely binary score variants. Possible classes include decisive success, costly success, partial stabilization, tacit/shared outcome, abandonment, local failure, and catastrophic failure.

Primary objective communication should be explicit; secondary systemic conditions remain discoverable through the Lifeworld.

Authoritative world-state predicates determine whether an event is resolved. Ghost Static may interpret/present the result. Apophis may establish story-mode invariants when explicitly invoked.

> **Success is not “did the player perform the authored sequence?” Success is “what stable-enough world did the player and other actors actually produce?”**

## Player information / epistemic contract

HUD information should be clear and actionable without exposing hidden systemic truth.

The HUD may present legitimate knowledge such as primary objectives, known teammate state, confirmed threats, known hazards, communicated Crew intentions, and visibly contested objects. It should not expose exact Director pressure, unseen Cattler coordinates, unobserved Rival intentions, hidden event predicates, or exact outcome probabilities.

Uncertainty should be represented through confidence, freshness, and source rather than erased. Conceptual statuses include confirmed, likely, stale, reported, and unknown; final presentation may use icon treatment, opacity, age, or other compact visual language instead of literal labels.

Maintain:

`server truth != individual knowledge != group report`

and:

`world truth -> actor perception -> communication -> shared Crew knowledge -> HUD presentation`

## Player Crew command contract

Player pings/orders are **strong utility commitments, not absolute overrides**.

The player primarily commands semantic tactical intentions such as:

- ATTACK / PRESSURE
- DEFEND / HOLD
- REGROUP / FOLLOW
- COLLECT / INTERACT
- REVIVE / ASSIST
- later: AVOID / WITHDRAW

The target supplies context. `DEFEND + doorway` means preserve Crew control of that affordance, not stand at an exact coordinate.

A teammate may break an order only when a bounded exception becomes materially stronger than the command commitment, including immediate lethal danger, severe agency loss, inaccessible route, nearby Crew stasis, catastrophic environmental change, or invalidated objective.

Breaks should be legible (`BREAKING — CONTAMINATION`, `BREAKING — REVIVE`, `ROUTE LOST`, etc.) rather than silent disobedience.

## Basic teammate AI contract

First implementation supports **one fully functional AI teammate** through the complete perception -> utility -> claim -> action -> revive loop before generalizing to multiple AI teammates.

Future squad expansion uses **independent actor utility plus a lightweight shared blackboard**, not centralized puppeteering.

The blackboard carries bounded shared commitments/evidence such as threat sightings, revive claims, cover/route claims, objective intent, regroup points, resource claims, and temporary danger.

## Squad cognition / communication contract

### Propagation

Crew information propagation is derived from observable connectivity rather than squad telepathy.

Conceptual relation:

`P_share = f(signal_quality, distance/topology, communication_access, sender_agency, receiver_availability, interference)`

Under ordinary good conditions this may be effectively immediate; damaged communication, topology, contamination, or incapacitation may degrade it.

### Report uncertainty

Shared reports preserve uncertainty and source reliability. A receiver's confidence may be derived conceptually from:

`C_receiver = C_source * R_source * Q_channel * F_freshness`

Independent corroborating evidence can increase confidence. Reports never become authoritative merely because they entered the squad blackboard.

### Evidence-sensitive staleness

Shared knowledge decays according to how quickly that kind of fact can plausibly change, not through one universal TTL.

Conceptual form:

`dC_k/dt = -lambda_k * C_k + E_k`

where `lambda_k` itself derives from observables such as target mobility, occlusion, report reliability, sensor coverage, environmental volatility, and contestability.

A building location may decay negligibly; a moving Rival position may decay rapidly; contamination boundaries or objective ownership decay according to their actual dynamics.

## Accepted near-term implementation chain

`Relational Physics Beta -> one-teammate AI -> down/stasis + situated revival -> lightweight squad cognition -> Panic Event Alpha -> persistent consequences -> Crescendo Event Alpha`

Current accepted revival behavior:

- critically defeated teammate enters downed/stasis rather than immediate ordinary respawn;
- revival risk is situated: proximity/time/exposure matter, but covering, interruption, pressure diversion, contamination, access, and tools modify the actual rate and viability;
- no universal fixed revive-duration outcome should define the interaction if the observable relation can be computed.

## Complexity guard

Do not introduce for this slice:

- giant global planner,
- full GOAP for basic teammate behavior,
- squad telepathy,
- omniscient AI access to server truth,
- fixed event timers as primary causal explanations,
- universal threat-memory TTL,
- exact scripted actor choreography for ordinary Crescendos,
- silent hidden ordinary-system cheating that should instead be explicitly labeled Apophis intervention.

Prefer small inspectable expressions over one opaque master scalar. Diagnostics should expose meaningful input contributions where useful.

## Cross-system social/economic parametric rule

The accepted social-economy closeout is recorded in `docs/LEDGER_SOCIAL_ECONOMY_CLOSEOUT.md`, with implementation-facing shapes in `docs/LEDGER_SOCIAL_ECOLOGY_CODE_SKETCH.md`.

The governing parametric rule extends to social and economic relations:

`observable world/account facts -> situated evidence/belief -> disagreement alternatives -> action/bargain -> material consequence -> trace/memory -> changed gradient`

Examples of suspicious fixed social/economic outcome constants now include:

- universal `trust_score`;
- permanent class assignment from Crew membership;
- authoritative structural-violence score;
- automatic Rawlsian/non-Rawlsian truth flag;
- universal property/commons ownership state that substitutes for access/stewardship relations;
- fixed exploitation/rent/debt morality thresholds independent of alternatives, contribution, risk, reproduction costs and contestability.

Prefer decomposed relations over master scalars.

### Gradient / memory bridge

Social and economic reproduction uses the same accepted stochastic-Lifeworld logic:

`action -> consequence -> memory -> changed gradient -> future action`

A present gradient conditions practical likelihood and cost; memory records what prior relations materially changed. Neither implies deterministic behavior.

### Surplus value bridge

Surplus is accepted as **surplus value of energy accounting**:

`surplus value = productive output - reproduction/maintenance/repair costs`

Distribution, claims and reinvestment then alter future reserves, access, infrastructure, disagreement points and gradients.

Do not create a second incompatible universal economy currency merely to represent surplus.

### Crew / class boundary

Human factions are Crews: pirate/maroon/union/guild/club/gang/sports-team/cultural-worker/mercenary-like social formations. Crew identity is not class identity.

Working design shorthand:

- equality or Rawlsian inequality on a relation -> no class struggle on that relation;
- non-Rawlsian inequality for a situated person -> micro-economic class struggle;
- reproduced materially similar non-Rawlsian relations across a structural position -> macro-economic class struggle.

This is a design/analysis contract, not an authoritative gameplay classifier.

### Commons boundary

A commons may be spontaneous/self-existing through its own ecological/material recurrence or methodically stewarded by an interested union, guild, Crew, cooperative or local association responsible for that segment of the commons.

Restriction is not automatically enclosure; evaluate whether stewardship reproduces common sustainable availability or primarily reproduces controller leverage/exclusion.

## SIGNAL pause marker

This question round is complete and paused. Do not reopen accepted contracts on resume unless a contradiction appears.

Next three questions are frozen in `docs/LEDGER_SOCIAL_ECONOMY_CLOSEOUT.md`:

1. money relative to universal energy accounting;
2. dues/taxes/levies as stewardship versus extraction;
3. bankruptcy/restructuring/debt forgiveness versus strict repayment.

No compile is authorized by this docs-only closeout.