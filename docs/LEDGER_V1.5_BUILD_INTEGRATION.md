# nightfall!punk v1.5 — Build Integration Ledger

Date: 2026-08-15
Branch: `build/v1.5-combat-ai-communications`
Basis: v1.4 green `e16ab0e7a6d12e0382403e007b37755ae56099f8`
Verified code candidate: `74fd07a2bb6d7ffcfcdf06fd6691bca2a1cffb36`
Compile/CI proof: GitHub Actions run `31908280120`, run #479 — SUCCESS
Status: **CODE/CI PASS — HUMAN GRAPHICAL PROOF PENDING**

## FROM

v1.5 is a strict descendant of the v1.4 green integration base. v1.4 already supplied integrated population, authoritative movement/camera, route revision, generalized temporal claims, exposure/cover geometry, teammate embodied causality, Human Rivals, Dream Cattlers, and the fixed-tick world substrate.

Before this build integration, the v1.5 design had advanced substantially beyond the active Rival C code. The branch had a useful report primitive, but the Rival path still used single-target `NfAiKnowledge`, a shared target `NfAiBlackboard`, audible events that could recover authoritative source identity, cover `reserved_by` ownership parallel to `nf_claim`, simple health/ammo tactical scoring, and no integrated v1.5 graphical demo target.

## TO

Canonical integrated spine:

`PLAYER INTENT -> CAMERA / VIEWMODEL -> AUTHORITATIVE COMBAT -> HIT/MISS / IMPACT / CONTAMINATION / SOUND -> SANITIZED CHANNEL EVIDENCE -> ACTOR-LOCAL BELIEF -> REPORT / MEMORY -> NESTED TACTICAL PROJECTION -> COMMITMENT -> CONTROL INTENT -> MOVEMENT / COMBAT / INTERACTION -> PHYSICAL CONSEQUENCE -> WORLD / ACCOUNT REVISION -> COMMUNICATION PRESENTATION -> NEW EVIDENCE`

Implemented and compiled in the candidate:

- Actor-local fixed-capacity belief stores with bounded competing hypotheses and first-class anonymous threats.
- Refined information channels: direct visual, audible gunfire, impact, damage received, trace, report, audible speech.
- Same-ancestry reports cannot multiply certainty; report overflow uses deterministic expiry/redundancy/value-aware replacement.
- Shared Human Rival cognition blackboard removed from the active v1.5 authority path.
- Audible events no longer grant recipient belief the authoritative source actor identity.
- Fresh direct perception outranks stale competing target belief; stale spatial precision expands after sight is lost.
- Fixed 12-candidate nested tactical layer with viability gate, relation/Crew/locality/future-option terms, information uncertainty, opportunity costs and role bias.
- Clear sustainable firing opportunity can beat guard/option preservation when information, ammunition and viability are strong; that advantage disappears under uncertainty or poor condition.
- Rival cover exclusivity uses `nf_claim`; geometric cover candidate generation is retained.
- Bounded metabolic source/sink/restitution links extend `nf_energy` without duplicating physical stocks.
- Sanitized fixed-capacity communication-presentation events separate UI from report/belief internals.
- Graphical `nightfall_v15_demo` with provisional eco-tech-guard carbine, player-visible authoritative hit/miss consequences, restrained hit confirmation, minimal Snap-Guided Focus bobble/jostle and recapture, Rival belief/action diagnostics, WATCH/SEEN presentation, Cattlers, and F6/F7 restitution proof controls.
- New deterministic `vertical_integration_v15` contract test and full v1.5 CI wrapper.

## Discrepancies resolved during compile

1. **Fresh perception vs stale belief:** fresh direct evidence now has bounded priority; stale precision grows after loss of sight.
2. **Cattler relation regression:** inherited truce test was deconfounded from the old Rival path endpoint; production hysteresis now breaks `TRUCE_HOLD` when the selected relation becomes damageable/non-negotiable again.
3. **Guard-heavy tactical bias:** ATTACK receives a bounded decisive-opportunity contribution only under high information quality, usable ammunition and viable body state.
4. **Utility/reflex clock mismatch:** aim-settle is preserved across compatible visible-target tactical modes and is calibrated to produce a firing opportunity within the slower nested appraisal cadence.
5. **Bounded-pressure dead slot:** pressure slots constrain actual simultaneous firing capacity rather than permanently privileged identities. If preferred pressure actors are not offering a valid shot, an otherwise valid Rival can temporarily occupy an unused slot for that tick. The inherited bounded-pressure smoke then passed.

These fixes preserve the architecture rather than restoring a global target or bypassing the evidence/belief layer.

## EXISTING — preserved and green

- Server/world authority and fixed-tick simulation.
- Movement, collision, traversal and camera authority.
- Authoritative weapon state, ammunition, cadence, reload/switch, contamination, STASIS and revival.
- Route/object revision and teammate causal route response.
- Temporal-claim substrate.
- Energy stock/flow/capacity/regeneration/absorption accounting plus actor-local energy beliefs.
- Human Rival encounter/spatial systems and distinct Dream Cattler ecology.
- v1.2/v1.4 regression tests and graphical proof targets.
- Networking/client/server build substrate.

## ADVANCING — coded and CI-green; human proof still required

- Actor-local competing belief runtime and anonymous-threat binding.
- Human Rival reports without shared cognition.
- Nested-account tactical candidate selection and commitment interaction.
- Claim-authoritative Rival cover contention.
- Restitution-gap / cycle-continuity runtime.
- Sanitized communication-presentation event seam.
- Player-visible v1.5 combat greybox, eco-tech-guard weapon identity and bounded Focus presentation.
- Modern sparse WATCH marker / Crew `SEEN` receipt presentation.
- Integrated two-Rival / teammate / Cattler graphical causal proof.

## PAR / LATER — deliberately not claimed complete

- Full six-slot ping wheel and complete ping -> teammate independent tactical reappraisal/action pipeline. The demo proves WATCH + `SEEN` presentation only; receipt is not compliance.
- Finished voice assets, subtitle corpus and full Rival audible-speech presentation.
- Finished weapon art and fine Focus/weapon-jostle tuning.
- Generalized three-carrier gameplay economy (metabolic reserve, ammunition, repair material); the first restitution carrier is proof-level rather than a completed inventory economy.
- Fully diegetic source/use/recovery object interactions; F6/F7 remain explicit diagnostic proof controls.
- Complete network replication/prediction for every new v1.5 presentation element.
- Long-horizon ecological generalization beyond the bounded restitution-link model.
- Human acceptance and full longitudinal H3 sweep.

## Green checklist

- [x] v1.5 source standard-check.
- [x] strict graphical compile with `-Wall -Wextra -Wpedantic -Werror`.
- [x] all inherited + v1.5 CTests.
- [x] bounded-pressure Human Rival combat smoke.
- [x] passive Human Rival encounter smoke.
- [x] situated Human Rival spatial ecology smoke.
- [x] Dream Cattler ecology smoke.
- [x] topographical energy accounting smoke.
- [x] topographic lifeworld / epistemic-boundary smoke.
- [x] contamination/STASIS transport smoke.
- [x] relational camera proof.
- [x] inherited integrated-population proof.
- [x] v1.4 object/claim/exposure proof.
- [x] v1.5 belief/nested/metabolic/communication proof.
- [ ] human graphical recording and assessment.
- [ ] explicit acceptance/merge authorization.

## Acceptance gate

Do **not** merge this candidate because CI is green. Record the graphical proof, assess the video against this ledger, then explicitly authorize acceptance/merge. The full H3 longitudinal sweep remains deferred until after that human proof unless concrete regression/drift evidence appears earlier.
