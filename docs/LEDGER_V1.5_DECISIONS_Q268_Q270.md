# nightfall!punk — v1.5 Decisions Q268–Q270

Status: **LOCKED DESIGN CONTRACTS — ACTIVE BUILD**
Branch: `build/v1.5-combat-ai-communications`

## Q268 — KEEP: Integrated Causal Combat–Information Proof

Use staged engineering verification, but make the human-proof target one integrated causal encounter:

`Player visible weapon -> authoritative fire -> hit/miss/contamination evidence -> Rival A direct knowledge -> bounded report -> Rival B delayed/coarser knowledge -> divergent beliefs -> nested-account utility -> continue/cover/reposition/withdraw -> embodied action -> new world relation`

The proof must demonstrate that the receiving actor does not inherit the source actor's exact knowledge unless the causal channel legitimately preserves that precision.

## Q269 — KEEP: Plural Human Communications

Player Crew and Human Rival Crews use the same bounded report/provenance machinery while retaining different evidence, relations, beliefs and situated utility.

`SharedMachinery != SharedInformation`

Dream Cattlers remain on an ecologically justified perception/signaling path rather than being homogenized into human squad communications.

## Q270 — KEEP: Canonical Cattler Rename + Temporary Compatibility Alias

`NF_FACTION_CATTLER` is canonical beginning in v1.5. A temporary `NF_FACTION_RANCHER = NF_FACTION_CATTLER` compatibility alias remains only to protect inherited code while references are migrated.

Rules:

- all new v1.5 code uses `NF_FACTION_CATTLER`;
- all new diagnostics/player-facing strings use Cattler terminology;
- inherited `RANCHER` identifiers are migrated incrementally;
- the alias is removed once inherited references reach zero;
- this naming cleanup must not become a gameplay-semantic change.

## Current implementation state

The canonical enum change is applied on the v1.5 build branch. No compile has been requested or run for this change.
