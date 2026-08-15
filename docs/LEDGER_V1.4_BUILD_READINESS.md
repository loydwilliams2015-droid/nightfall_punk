# nightfall!punk — v1.4 Build Readiness / FROM -> TO

Date: 2026-08-14
Branch: `build/v1.4-high-medium-integration`
Status: **BUILD CANDIDATE — HUMAN VIDEO GATE REQUIRED**

## FROM

Canonical coded base:

- v1.2c branch: `build/v1.2c-integrated-causality`
- exact green base: `27af894fb9148be11447b8e26320bf18307645d6`
- v1.2c CI #362 PASS.

Human v1.2c recording established:

- Player 1;
- teammate 1;
- Human Rivals 2;
- Dream Cattlers 2;
- first-person camera retained;
- teammate produces measurable embodied displacement;
- Rival and Cattler bodies exist in the same shared slice.

The positive ALTER_ROUTE -> reroute sequence was not cleanly captured, so v1.4 adds a bounded proof setup rather than pretending that human proof was complete.

v1.3 remains audit/design-only and has no compiled gameplay build.

## Roadmap discrepancy resolved

A temporary branch `build/v1.4-relational-combat-communications` implemented a green communications prototype under an older roadmap assumption. A newer canonical state index assigns communications + fuller combat integration to v1.5.

Disposition:

- do not merge that stale branch as v1.4;
- preserve it only as experimental reference;
- canonical v1.4 is this branch and contains high/medium-degree integration only.

## TO — bounded v1.4 code contract

### Stable object revision

ALTER_ROUTE now owns:

- stable `gate_key`;
- monotonic `revision`;
- existing authoritative `changed_tick` / `changed_by`.

### Temporal claims

New fixed-capacity claim table provides:

- COVER;
- TRAVERSAL;
- INTERACTION;
- REVIVE;
- SOFT shareability;
- HARD exclusivity;
- expiry/release.

The human proof consumes INTERACTION claims; deterministic tests cover shared/hard/expiry semantics for the generalized contract.

### Graded exposure / situated utility

New tactical helpers expose:

- `nf_tactical_exposure()` — three-height line-of-sight exposure in `[0,1]`;
- `nf_tactical_cover_utility()` — bounded exposure + travel + future-option utility.

The v1.4 demo displays this gradient without replacing existing AI/navigation/combat authority.

### Integrated proof surface

New `nightfall_v14_demo` preserves:

- Player;
- real teammate;
- Human Rivals;
- Dream Cattlers;
- camera modes;
- route/evidence/utility/commitment diagnostics.

F4 is a clearly labeled debug proof setup:

`rev1 CLOSED -> no route evidence -> interaction claim -> rev2 OPEN -> DIRECT evidence -> belief/gradient/utility/commitment -> embodied action`.

## EXISTING — regression checklist

- [ ] fixed-rate authoritative world simulation;
- [ ] ENet dedicated-server/client networking;
- [ ] prediction/reconciliation;
- [ ] authoritative combat substrate;
- [ ] contamination/STASIS/revival;
- [ ] Snap Locomotion and movement authority;
- [ ] Relational Perceptual Authority Camera;
- [ ] teammate evidence/memory/gradient/commitment;
- [ ] teammate physical-displacement proof;
- [ ] Human Rival perception/utility/spatial ecology;
- [ ] Dream Cattler ecology/weakpoint truth;
- [ ] topographical energy;
- [ ] Lifeworld/epistemic boundaries;
- [ ] v1.2c integrated population.

## ADVANCING — v1.4 checklist

- [ ] stable object key;
- [ ] monotonic object revision;
- [ ] route replica revision behavior;
- [ ] generalized temporal-claim C module;
- [ ] soft-share proof;
- [ ] hard-block proof;
- [ ] claim expiry proof;
- [ ] live interaction claim in graphical proof;
- [ ] graded exposure primitive;
- [ ] exposure affects bounded utility helper;
- [ ] integrated direct-evidence positive proof;
- [ ] F4 controlled human proof setup;
- [ ] dedicated v1.4 CTest/smoke;
- [ ] dedicated v1.4 CI;
- [ ] final exact-head compile/tests green;
- [ ] human video review before merge.

## PAR / next frontier

- [ ] AI communications/provenance/latency — v1.5;
- [ ] player-visible weapon/viewmodel — v1.5;
- [ ] fuller reciprocal playable conflict — v1.5;
- [ ] deliberate deception;
- [ ] physical communications channels/jamming;
- [ ] generalized trust;
- [ ] Cattler communication refactor;
- [ ] universal mutable-object replication;
- [ ] full Dynamic Affordance Graph regeneration;
- [ ] mixed-resource production/replenishment proof;
- [ ] causal waterline/trace expansion;
- [ ] distributed authority/cell scaling;
- [ ] general GOAP;
- [ ] broad destructibility/ragdoll/fluid expansion.

## Compile gate

Do not mark v1.4 green until the final documentation/code head passes the dedicated `nightfall v1.4 CI` workflow end-to-end.

## Human acceptance

Minimum recording:

1. title says v1.4;
2. integrated population visible;
3. F4 proof setup;
4. gate rev1 CLOSED and teammate EVIDENCE NONE;
5. hold E -> interaction claim -> gate rev2 OPEN;
6. teammate obtains legitimate DIRECT evidence;
7. belief/gradient/utility/commitment change;
8. teammate visibly moves/changes trajectory;
9. optional F2/F3/F1 camera regression.
