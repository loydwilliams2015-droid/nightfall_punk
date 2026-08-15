# nightfall!punk — v1.5 AI Belief Integration Q869–Q873

Status: **LOCKED DESIGN CONTRACTS — ACTIVE BUILD**
Branch: `build/v1.5-combat-ai-communications`

## Q869 — KEEP: Lightweight first-class anonymous threat hypothesis

Allow DAMAGE_RECEIVED, AUDIBLE_GUNFIRE, IMPACT, TRACE, or similar evidence to create a bounded `UNKNOWN_THREAT` hypothesis when identity has not been legitimately earned.

The anonymous hypothesis may carry:
- threat/source type where inferable;
- coarse region / direction;
- confidence;
- precision;
- provenance/channel;
- age;
- compatible/contradictory evidence.

It must not invent actor identity.

When later independent evidence supports identity, the anonymous threat may bind/merge with an identified actor hypothesis.

Canonical relation:

`UnknownThreat(region, direction, confidence)`
`+ compatible independent identity evidence`
`-> identified actor hypothesis`

not:

`DamageReceived -> attacker identity`.

Keep anonymous tracks tightly capped so they remain an epistemic primitive rather than a second target system.

## Q870 — KEEP: Structural familiarity without current-state omniscience

Human Rivals may begin with authored familiarity with the stable geometry of the proof block:
- permanent walls;
- major routes;
- stairs/platforms;
- broad cover/vantage possibilities;
- stable traversal grammar.

They do **not** begin with perfect knowledge of:
- current occupants;
- dynamic route/object revisions;
- resource state;
- contamination/hazards;
- current tactical claims;
- recent traces;
- actor positions or intentions.

Invariant:

`KnownStableStructure != KnownCurrentSituation`

This preserves competent FPS navigation while keeping dynamic world state actor-relative.

## Q871 — KEEP: Uncertainty-responsive interaction appraisal

Conflicting high-quality hypotheses should have context-sensitive visible consequences rather than a generic scripted confusion state.

If additional information is cheap and decision-relevant, uncertainty may raise the value of:
- investigate;
- vantage acquisition;
- cautious reposition;
- search.

If time/objective pressure is high, the actor may commit to the leading hypothesis while preserving cover, escape, or hedge options.

If competing hypotheses imply the same strategic relation, guard/continue-objective may dominate both.

Canonical relation:

`CompetingHypotheses -> UncertaintyCost + ValueOfInformation -> InteractionAppraisal`

not:

`Contradiction -> random action`.

The actor remains capable of rational commitment under uncertainty.

## Q872 — KEEP: Persistent historical relation + discontinuous tactical horizon through STASIS/re-entry

STASIS/re-entry does not erase all actor history, but it also does not preserve exact short-lived tactical knowledge as if agency had never been interrupted.

Prefer persistence of:
- relationship history;
- stable structural knowledge;
- known object/route history;
- major recent strategic facts;
- coarse encounter memory where still relevant.

Strongly decay or clear:
- exact target tracks;
- short-lived auditory/impact hypotheses;
- narrow last-known position estimates;
- active tactical commitment;
- fleeting search state.

Governing relation:

`Persistent = Relation + History + StableWorldKnowledge`

`Degraded/Cleared = ExactTargetPosition + ShortStimuli + TacticalCommitment`

This preserves continuity without post-reentry wallhacks.

## Q873 — KEEP: Selectable F4 epistemic perspective mode

F4 human proof should make actor belief inspectable without showing every actor's cognition simultaneously.

Select one AI actor at a time and expose a compact causal view:

```text
SUBJECT: PLAYER
H1 DIRECT_VISUAL  conf=.84  precision=.5m  age=.4s
H2 REPORT         conf=.39  precision=8m   age=1.7s
SELECTED: H1
LAST UPDATE: direct visual reacquisition
APPRAISAL: reposition > attack > investigate
COMMITMENT: reposition
```

Combine this with restrained world-space uncertainty markers/regions corresponding to the selected actor's hypotheses.

The diagnostic chain is:

`Evidence -> Competing Hypotheses -> Selected Belief -> Appraisal -> Commitment -> Embodied Action`

The normal HUD does not expose these internals.

## Closure consequence

Q869–Q873 close the remaining AI-belief integration design choices.

The active v1.5 epistemic architecture is now:

`Authoritative Event`
`-> channel-specific actor evidence`
`-> actor-local bounded hypotheses`
`-> contradiction / merge / decay / uncertainty growth`
`-> selected situated belief`
`-> nested-account appraisal`
`-> commitment`
`-> physical action`
`-> new evidence`

Reports remain social evidence rather than cognition copies; stable map familiarity remains distinct from current-state knowledge; anonymous threats prevent illicit identity inference; STASIS preserves history without preserving exact tactical omniscience; F4 can human-prove the full causal chain.

Vertical Integration Pass II remains deferred until the remaining weak-point sweeps are completed.