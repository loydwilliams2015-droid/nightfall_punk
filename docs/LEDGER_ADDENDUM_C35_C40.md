# nightfall!punk ledger addendum — C35–C40

Status: LOCKED design handoff for SIGNAL / PRAXIS / STEWARD. No compile implied by this documentation update.

This addendum follows the existing C34 non-exclusive semantic-affordance contract and the separate `SEED_MAP_CONTRACT.md`.

---

## C35 — epistemic privacy / bounded information game — LOCKED

Core rule:

> **Server truth is not actor entitlement. Existence of information does not imply legitimate access to information.**

Privacy, security, confidentiality, anonymity/pseudonymity, telemetry, surveillance, provenance and retention remain distinct concepts.

### Real-player boundary

- [x] copyleft/open-source code does not make player data a commons.
- [x] prefer local-first state where network persistence is unnecessary.
- [x] prefer pseudonymous multiplayer identity over compulsory real-world identity.
- [x] collect only operationally required data; optional analytics must remain optional.
- [x] logs must avoid unnecessary personal information and credentials.
- [x] mods receive scoped interfaces/capabilities rather than arbitrary access to private account state.
- [x] no gameplay advantage should require surrender of unrelated real-world privacy.

### Diegetic information game

Every strategically meaningful belief should have a causal information path and provenance.

```c
typedef enum NfKnowledgeSource {
    NF_KNOW_DIRECT = 0,
    NF_KNOW_REPORT,
    NF_KNOW_PUBLIC_RECORD,
    NF_KNOW_AUTHORIZED_SENSOR,
    NF_KNOW_INFERENCE,
    NF_KNOW_INTERCEPT
} NfKnowledgeSource;

typedef struct NfKnowledgeRecord {
    NfEntityId subject;
    NfKnowledgeSource source;
    NfEntityId provenance;
    float confidence;
    float age_s;
    uint32_t scope;
} NfKnowledgeRecord;
```

Game-theoretic information valuation:

```c
typedef struct NfInformationOpportunity {
    double expected_return;
    double acquisition_cost;
    double exposure_risk;
    double trust_damage;
    double retaliation_risk;
    double institutional_cost;
} NfInformationOpportunity;

static double nf_information_payoff(const NfInformationOpportunity *x)
{
    return x->expected_return
         - x->acquisition_cost
         - x->exposure_risk
         - x->trust_damage
         - x->retaliation_risk
         - x->institutional_cost;
}
```

The payoff model governs **diegetic strategic acquisition**, never whether the project may ignore real-player privacy rules.

Information path:

```text
WORLD / SERVER TRUTH
        ↓
legitimate causal channel
        ↓
EVIDENCE + PROVENANCE
        ↓
ACTOR BELIEF
        ↓
STRATEGY
```

Surveillance must be physical/informational causation: powered sensor + range/LOS + functioning signal path + recipient access + interpretation. A camera existing does not grant omniscience.

Maxims:

- **Copyleft the game, not the player.**
- **World truth is not common knowledge.**
- **Truth may be global; knowledge is situated.**
- **Knowing that a person exists is not the same as being entitled to know them.**

---

## C36 — Las Flores consolidated Southern California urban morphology — LOCKED

Core formulation:

> **Las Flores is a consolidated alternate Southern California whose physical memory remains recognizably Los Angeles while its metropolitan development history went laterally elsewhere.**

Planning lineage:

- City Beautiful civic/monumental planning.
- CCC/New Deal greenbelt, park, watershed and public-land stewardship.
- late-1960s onward environmentalism, anti-sprawl planning and strong urban-growth boundaries.
- 2038 outcome: roughly half-sized urbanized footprint, especially reduced eastern sprawl; coastal/core Manhattanization; powerful transit; sharp urban/wild interfaces.

Spatial conditions include:

- continuity districts retaining old LA street memory;
- Manhattanized coastal/core districts with Tokyo/Taipei-like density/economic habits layered over traditional LA entertainment, port/logistics, technology, service and creative economies;
- civic-monument districts;
- transit/interchange districts;
- mixed residential commons;
- industrial/logistical metabolism districts;
- greenbelt/rural/wilderness reserves;
- urban-wild interfaces;
- Twilight/shattered social geographies;
- Cattler infestations and Deep Nightfall conditions.

The city is semi-utopian/in-crisis, not a failed-utopia ruin. Abundance and functioning transit/public life must remain visible enough for Nightfall to threaten something real.

Key formula:

> **Old Los Angeles street memory + half-sized metropolitan footprint + Manhattanized coastal corridors + preserved eastern/wild landscapes + extremely strong transit + radically sharper urban/wild interfaces.**

Key image:

> **You should be able to recognize Los Angeles from Mulholland Drive—and then look toward Hollybrook and realize that history went somewhere completely different.**

---

## C37 — humane choice / traversal-first reward / gore-light horror — LOCKED

Moral motif:

> **Being human is not the same as being humane. Moral grayness does not abolish humane choice; it makes humane choice matter.**

- [x] Crews and Human Rivals may be mercenary, abrasive, violent, self-interested or contradictory while retaining the capacity for humane action.
- [x] humane action may cost time, position, energy, reputation or opportunity and need not always return a mechanical reward.
- [x] Cattler horror is sharpened because wrangling treats life as throughput; humane action recognizes life as exceeding usefulness.
- [x] healing, maintenance, restraint, permitting exit, preserving common infrastructure, protecting living systems and bounded cooperation can be strategically meaningful without becoming a morality meter.

Reward/pacing rule:

> **Traversal is the rush. Horror is the interruption. Violence is one possible resolution.**

Primary pleasure loop:

```text
MOVEMENT
  ↓
DISCOVERY / AESTHETIC PAYOFF
  ↓
SCORE / MASTERY / ROUTE EXPRESSION
  ↓
MEMORY / REPLAY VALUE
```

Violence supplies pressure and consequence but is not the principal dopamine/scoring spectacle.

Enemy fright grammar:

- paranormal wrongness;
- cyberpunk/body-incorporation horror;
- physical scale, mass, reach and territorial intimidation;
- stalking, prowling, route interruption and environmental wrongness;
- restrained blood/gore; implication and morphology over viscera.

Maxims:

- **In an inhumane world, humanity survives wherever someone refuses to treat another life as merely useful.**
- **The player feels exhilaration from moving through the world; the enemy makes that same world frightening to move through.**
- **Fright through presence, scale, implication and pursuit—not viscera.**

---

## C38 — lateral futurism / hybrid PBR–cel neo-noir rendering — LOCKED

Working term:

> **Lateral Futurism = a future reached by plausible sideways historical development rather than nostalgic revival, technological rupture, or extrapolation only along our own timeline.**

Las Flores is not principally retrofuturist. It is adjacent to alternate modernity, lost futurism and Fisherian hauntology, but its future **did arrive**—along another historical branch.

Visual rule:

> **PBR supplies material truth; cel-shading supplies value structure; neo-noir supplies composition; selective SSR/planar reflection supplies reflective punctuation.**

- [x] no character/object outlines as the default style.
- [x] retain PBR material identity: albedo/base color, normals, roughness, metallic, AO, emissive and plausible specular behavior.
- [x] stylize diffuse/value response more strongly than specular/reflection response.
- [x] use soft/broad tonal organization rather than hard two-tone cel bands.
- [x] reflections remain comparatively physically credible so wet streets, glass, metal and contamination anchor the image materially.
- [x] neo-noir comes primarily from composition, negative space, practical lights, silhouette and controlled shadow values rather than a universal gimmick filter.

Temporal balance:

- Day: PBR-dominant, restrained graphic compression.
- Twilight: signature equilibrium of PBR credibility and graphic shadow/value organization.
- Nightfall: stronger midtone compression and negative space, but not simply darker or less legible.

Maxim:

> **The present remains physically believable while the future begins to draw itself differently.**

---

## C39 — dual shadow architecture — LOCKED

Use two cooperating shadow systems with one shared world/light truth.

### A. Projected Footprint Shadow — comic/readability layer

Purpose: replace outlines and circle/blob shadows with an intentionally simple projected actor/object footprint.

- derive from coarse actor bounds/footprint + dominant light direction/elevation + supporting surface;
- use a small tapered polygon/wedge/trapezoid, not detailed silhouette rasterization;
- preserve obvious actor mass/elevation/landing relation;
- Cattlers naturally generate heavier/larger graphic occupation shadows;
- apply mainly to diffuse/readability rather than crushing specular/reflections/emissives.

```c
typedef struct NfGraphicShadow {
    Vector3 point[8];
    uint8_t point_count;
    float opacity;
    float length;
    float edge_softness;
} NfGraphicShadow;
```

### B. Bounded physical shadow system — credibility layer

Target architecture:

- primary sun/moon/key light: approximately two cascades, not a large cascade stack;
- modest filtering such as restrained PCF;
- very small budget of important shadow-casting local lights;
- most decorative city lights illuminate/specularize without dynamic shadow maps;
- static architecture leans on baked/static occlusion where appropriate;
- dynamic physical shadow effort reserved for actors, mechanisms and strategically meaningful moving/changing elements.

Conceptual composition:

```text
PBR MATERIAL
   ↓
DYNAMIC LIGHT + PHYSICAL SHADOW
   ↓
PBR DIFFUSE / SPECULAR
   ↓
PROJECTED GRAPHIC FOOTPRINT SHADOW
   ↓
SOFT VALUE COMPRESSION
   ↓
SELECTIVE SSR / PLANAR REFLECTION
   ↓
ATMOSPHERE / NEO-NOIR GRADE
```

Maxims:

- **Real light casts the world. Comic shadow draws the action.**
- **We do not draw around the object; we let its occupation of light and space define it.**
- **The comic layer exaggerates legibility, not world truth.**

---

## C40 — passive→active systemic street transition reference — LOCKED

Reference scene: a Twilight Las Flores street/intersection begins in a socially awkward but passive state and crosses toward active danger.

Initial conditions:

- player + two teammates and one Human Rival crew member are mingling after an awkward encounter;
- weapons/tools may be present but no one is yet committed to violence;
- social geometry communicates distrust, curiosity and provisional restraint rather than a faction-flip alliance;
- functioning tram/transit, wet street, mixed old-LA/new-dense architecture and ordinary urban life establish that the city still works.

Transition:

- actors notice two Cattlers stalking/prowling rather than charging immediately;
- Cattlers are behaviorally distinct individuals and should not read as matched zombie units;
- noticing occurs through legitimate sense-data/evidence, not HUD omniscience;
- Crew and Rival strategy sets change before anyone necessarily fires.

Decision grammar:

```text
PASSIVE MINGLING / AWKWARD RESTRAINT
               ↓
CATTLER EVIDENCE ENTERS SHARED BUT UNEQUAL PERCEPTION
               ↓
ATTENTION / POSTURE / POSITION CHANGES
               ↓
PLAYER: stay / move / climb / help / investigate / disengage / prepare
TEAMMATES: cover / maintain / watch / regroup / reposition
RIVAL: hold / withdraw / tacitly cooperate / contest / exploit opening
CATTLERS: stalk / observe / deny route / converge / occupy / pressure
               ↓
ACTIVE SCENE MAY EMERGE WITHOUT A SCRIPTED COMBAT SWITCH
```

Scene rule:

> **A systemic screenshot should show what everyone might decide next, not merely what they are shooting now.**

This scene is a concrete visual/behavioral reference for C26 eventality at encounter scale: the arrival/recognition of stalking Cattlers changes future strategy space before spectacle necessarily occurs.

---

# Three-role handoff — UPDATE FROM / TO

## SIGNAL → PRAXIS

SIGNAL supplies:

- Las Flores alternate-history urbanism and lateral-futurist interpretation;
- privacy/cultural/GIS research criteria;
- humane-in-moral-grayness thematic frame;
- traversal-first pleasure and gore-light paranormal/cyberpunk horror frame;
- visual/art-reference language for PBR/cel/neo-noir balance.

PRAXIS converts only accepted implications into bounded systems. No theme term should become a magic manager/stat.

## PRAXIS → SIGNAL

PRAXIS returns:

- exact names/boundaries for `NfSeedKey`, fixed atlas truth, information provenance, sensor causation and shadow/render contracts;
- feasibility constraints so public comparisons do not promise machinery not implemented;
- distinction between concept renderer target and current code reality.

SIGNAL translates these without changing technical meaning.

## SIGNAL → STEWARD

SIGNAL flags what is cultural thesis, working terminology or external-facing language. STEWARD preserves provenance/status and prevents a working phrase such as **Lateral Futurism** from being misrepresented as an established movement unless research later supports that claim.

## PRAXIS → STEWARD

PRAXIS hands off testable invariants:

- fixed atlas features cannot reroll with numeric seed;
- actor knowledge cannot bypass causal evidence/provenance;
- Projected Footprint Shadow remains cheap and non-circular/non-outline;
- serious shadows remain bounded rather than growing into an unlimited dynamic-light system;
- passive→active encounter transition emerges from changed evidence/payoffs rather than a hard combat-mode switch.

## STEWARD → PRAXIS

STEWARD protects scope:

- no compile is implied by this addendum;
- renderer concepts are contracts for the appropriate future rendering/lighting build, not claims of present implementation;
- keep the simple comic shadow and serious physical shadow systems separate enough to profile/test;
- do not add complexity unless a human render/playtest can demonstrate the value.

## STEWARD → SIGNAL

STEWARD preserves the public/core distinctions:

- Las Flores is semi-utopian/in-crisis, not failed-utopian;
- Nightfall/infestation is a condition/history layered over place, not a synonym for poverty or eastern geography;
- Cattlers frighten through predatory presence and territorialization, not gore spectacle;
- the game's niche is aesthetic/systemic identity, not ideological agreement.

## Shared triad

```text
SIGNAL: WHAT DOES IT MEAN / WHAT CAN WE LEARN?
                 ↓
PRAXIS: HOW DOES IT WORK / WHAT IS THE MINIMUM SYSTEM?
                 ↓
STEWARD: WHAT IS CANONICAL / WHAT PROVES IT / WHAT IS DEFERRED?
                 ↓
USER: ACCEPT / REVISE / REJECT / PLAYTEST
                 ↺
```

Shared maxims:

- **SIGNAL translates. PRAXIS defines. STEWARD preserves.**
- **Research the real things the fiction is made from.**
- **Minimum sufficient machinery; maximum relational consequence.**
- **Nothing becomes project truth merely because it sounded good in conversation.**
