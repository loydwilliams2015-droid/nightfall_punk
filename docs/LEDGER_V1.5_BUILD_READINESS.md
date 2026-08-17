# nightfall!punk — v1.5 Combat + AI Communications Build Readiness

Status: **ACTIVE BUILD — FIRST CODE SLICE, NOT YET COMPILED**
Branch: `build/v1.5-combat-ai-communications`

## FROM

Exact v1.4 code base:

`e16ab0e7a6d12e0382403e007b37755ae56099f8`

That exact head passed `nightfall v1.4 CI` run **#427** / `31862660880` end-to-end. PR #22 remains draft and human-video gated; v1.5 development does not merge or bypass that gate.

The stale branch `build/v1.4-relational-combat-communications` is **counterfactual reference only**. v1.5 is re-derived from the canonical v1.4 line under the locked Q250-Q267 contracts.

## v1.5 thesis

> **Graft where the grammar is solved; invent where the relation is new.**

Combat remains immediately legible as FPS combat while AI communication, information topography, nested-account tactical individuality, resource/energy consequence, and reproductive conflict appraisal supply the distinctive causal layer.

## Canonical causal spine

`WORLD / COMBAT EVENT -> LEGITIMATE EVIDENCE -> INFORMATION TOPOGRAPHY -> REPORT / MEMORY -> ACTOR BELIEF -> NESTED ACCOUNT PROJECTION -> SITUATED UTILITY -> REPRODUCTIVE CONFLICT APPRAISAL -> COMMITMENT -> CONTROL / COMBAT -> PHYSICAL CONSEQUENCE -> ACCOUNT / WORLD REVISION`

## First implemented slice — information report primitive

New shared files:

- `src/shared/nf_report.h`
- `src/shared/nf_report.c`
- `src/tests/test_v15_report.c`

CMake project version advanced to **1.5** and the report module/test are wired into the build graph.

### Contract

A report carries:

- stable report ID and ancestry ID;
- subject key and semantic kind;
- scope;
- origin evidence channel;
- origin actor and current reporter;
- position/value;
- confidence;
- origin precision and current precision;
- hop count;
- origin / issue / delivery / expiry ticks.

### Information-topography invariant

**Information may lose precision through transmission; it must not gain unearned precision.**

Relay therefore enforces:

`relay_precision >= parent_precision >= origin_precision`

Position estimates are quantized at the current legitimate precision. Direct firsthand evidence can remain fine-grained; relayed/delayed information can become coarser.

Report weight is bounded by confidence, freshness, hop count, and current spatial precision.

### Test intent

`test_v15_report.c` checks:

- delayed delivery;
- direct fine-grained evidence;
- attempted illicit precision gain is rejected;
- coarse relay is legitimately degraded;
- relay confidence decreases;
- faction/scope access;
- best-live-report selection.

This slice is intentionally **not yet compiled** because compile remains an explicit user gate.

## Existing combat substrate inherited

v1.4 already provides authoritative:

- carbine and pistol specifications;
- magazine/reserve accounting;
- automatic/semi-automatic fire state;
- cadence/recovery;
- reload and late commit;
- weapon switching;
- friendly-fire eligibility;
- gunfire/damage/STASIS/revival events;
- contamination as authoritative damage;
- STASIS and revival;
- Human Rival combat smoke;
- Dream Cattler combat/ecology substrate.

v1.5 does not need to reinvent these mechanisms. It must integrate them into the player-visible and actor-relative causal loop.

## Immediate advancing frontier

1. connect legitimate perception/combat events to report provenance;
2. replace omniscient/shared-blackboard implications with recipient-specific report consumption;
3. derive individual Rival tactical utility through nested actor/relation/Crew/locality accounts;
4. add bounded continue / withdraw / reposition appraisal;
5. connect player-visible fire/reload/hit/viewmodel to existing authoritative combat;
6. demonstrate information topography materially changing different AI actions in the same encounter;
7. expose strategic consequences without exposing optimizer internals.

## PAR / later

- deliberate deception;
- generalized trust model;
- physical radio propagation/jamming;
- universal information-field simulation;
- Cattler communications refactor beyond ecologically justified signals;
- general GOAP;
- broad weapon roster;
- full production viewmodel art;
- final network/distributed scaling of report traffic.
