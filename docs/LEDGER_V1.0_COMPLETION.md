# nightfall!punk — v1.0 Topographic Lifeworld completion ledger

Status: **CODE-COMPLETE CANDIDATE; final Pop!_OS human acceptance gate remains.**

This file is the compact completion/readiness ledger for v1.0. The cumulative design record remains in `docs/LEDGER.md`; `docs/LEDGER_V1.0.md` records the first v1.0 implementation pass; this file governs the final completion candidate where older readiness text conflicts.

## Dependency closure

- [x] v0.8 Dream Cattler Habitat Ecology accepted after repaired lower-body human proof.
- [x] `archive/v0.8` created at repaired candidate `3448e3b26165d94943e6527eee4a93a13886c119`.
- [x] v0.8 merged to main; repaired weak-point truth is in accepted history.
- [x] v0.9 Topographical Energy Accounting reconciled onto repaired v0.8 history.
- [x] exact reconciled v0.9 CI green.
- [x] `archive/v0.9` created at `f5d4d4ee77a7aabdd390782aedb73943b98772a2`.
- [x] v0.9 merged to main at `0e0b47758c3d8ca0318169a56c885614657c58bd`.
- [x] v1.0 branch ancestry reconciled onto accepted v0.9/main while preserving the v1.0 semantic tree.
- [x] v1.0 PR #13 retargeted to main and mergeable.
- [ ] do not create `archive/v1.0` or merge PR #13 until the final human gate below passes.

## Governing completion thesis

> **A monster belongs to an ecology when its presence, absence, return and traces are consequences of the world rather than of its respawn timer.**

> **The player should fear not only the Cattler, but the possibility that a place has learned how to host Cattlers.**

v1.0 closes the first Topographic Lifeworld slice by making the contested Cattler landbase capable of real local absence, ecological return, haunted environmental expression, bounded epistemic attention and gradual recovery without adding a second planner or leaking hidden world truth to actors/clients.

# EXISTING — regression-protected and green

- [x] fixed 60 Hz authoritative simulation.
- [x] dedicated server truth + client prediction/reconciliation.
- [x] ENet networking/reconnect/session scaffolding.
- [x] libsodium path when available.
- [x] Fuzzy Rail step/vault/mantle/ladder/local physical traversal.
- [x] authoritative carbine/pistol combat, reload, rewind hitscan, damage, death and Human respawn.
- [x] faction/relationship damage policy.
- [x] Human Rival limited perception, memory/confidence, semantic hearing, utility/cover and ordinary `NfControlFrame` output.
- [x] bounded pressure/flank/hold encounter governance.
- [x] 24-region / 0.40 km² spatial ecology and bounded next-hop reasoning.
- [x] distinct Dream Cattler AI family, habitat logic, imperfect pack evidence, locomotor integrity and infestation ecology.
- [x] repaired visible knee/foot -> authoritative hit-zone -> locomotor consequence truth boundary.
- [x] universal topographical `NfEnergySystem`, accounting invariants, history-sensitive metabolism, helicopter ranking and actor-safe belief facade.
- [x] ~800 m² interdependent landbase proof with generator + pump process objects.
- [x] tiny exact quantized primary landbase memory; disposable secondary/client waterline.
- [x] mobile epistemic hotspots: contested occupation, access change, recovery edge.

# ADVANCING — completed in the final v1.0 pass

## A. Ecological Dream Cattler recurrence

- [x] ordinary Cattler combat death no longer uses the generic local respawn-position path.
- [x] Cattler death produces genuine local absence after the combat respawn timer expires unless an ecological return is eligible.
- [x] per-Cattler recurrence memory records death observation, death region and death tick.
- [x] living Cattler population is derived from authoritative actor bodies.
- [x] ordinary return requires at least one other living Cattler as ecological source continuity.
- [x] ordinary return waits a deterministic seeded 8–16 second minimum window.
- [x] ordinary return candidate must be player-clear, connected within <=2 region graph steps of a living source and may not simply reuse the death region.
- [x] return candidate is selected from habitat continuity/resource/quiet/source-fit relationships rather than a fixed spawn point.
- [x] returned Cattler receives new home/current/target/next region at the ecological source and resumes ordinary Cattler AI/Fuzzy Rail embodiment from there.
- [x] server distinguishes returns with `[cattler-return] kind=RECOLONIZE ...`.

## B. Global-zero extinction failsafe

- [x] global-zero Cattler presence is tracked separately from ordinary recurrence.
- [x] zero population does **not** trigger immediate reseeding.
- [x] exceptional reseed requires a viable habitat candidate.
- [x] candidate must remain outside live Player/Teammate observation/occupation.
- [x] candidate and global-zero condition must persist for a deterministic seeded ~45–90 second absence window.
- [x] exceptional return is logged distinctly as `EXTINCTION_RESEED` so debugging never confuses infrastructure with normal ecology.
- [x] deterministic runtime test proves a player observing the candidate blocks reseed beyond the nominal wait.
- [x] deterministic runtime test proves prolonged player absence can permit reseed.

Boundary:

> **Ordinary recurrence is ecology. Zero-population reseeding is infrastructure protecting the possibility of ecology.**

## C. Haunted infestation waterline

- [x] client remains epistemically bounded: no hidden infestation score is replicated for presentation.
- [x] predicted local haunted-state intensity derives from replicated living Cattler presence in the landbase.
- [x] environmental expression remains place/topography-bound rather than actor-centered aura geometry.
- [x] previous near-pure-black stain revised toward midnight blue-black / petroleum violet so infestation can be distinguished from ordinary void/black geometry.
- [x] slow charcoal/violet smoke wisps provide PG-13 cyberpunk haunting without obscuring combat silhouettes.
- [x] restrained geometry-clinging dark filaments imply habitat occupation.
- [x] gore-free violet/cyan synthetic splatter traces provide splatterpunk aggression without blood/gore.
- [x] tiny cold-cyan spatial/static anomaly supplies restrained cosmic-horror wrongness.
- [x] Cattler absence causes client-predicted waterline to recede slowly rather than snap clean.
- [x] generator/pump remain readable and visually suppressed by the haunted state.

Presentation contract:

> **The darkness is not the infestation. The darkness, smoke and synthetic residue are what infestation does to the perceptible lifeworld.**

## D. Diminishing cooperative epistemic coverage

- [x] allied attention no longer uses only proximity plus a linear coverage penalty.
- [x] bounded marginal information gain uses first-observer / corroboration / diminishing / mostly-redundant tiers: `1000 / 650 / 300 / 120` quantized gain.
- [x] deterministic uniform second-choice exploration among near-equivalent hotspots remains the blunt minimal-difference mechanism.
- [x] immediate visible combat/safety takes precedence over epistemic economy.
- [x] no fixed Scout/Worker personality class, global optimizer, opaque RL or literal genetic simulation added.

Maxim:

> **Randomness proposes; situated selection disposes.**

## E. Epistemic attention becomes physical without a second planner

- [x] `NfSpatialAgentState` accepts a bounded, expiring attention hint.
- [x] contested-occupation attention maps into existing CONTEST / FLANK spatial tasks according to existing squad role.
- [x] recovery/maintenance attention maps into existing OCCUPY behavior.
- [x] access/other investigation maps into existing INVESTIGATE behavior.
- [x] hint changes existing regional candidate bias/commitment only; it does not emit movement directly.
- [x] local goal is invalidated so existing `nf_spatial_tick()` + Fuzzy Rail still own physical traversal.
- [x] ENGAGE / SEEK_COVER / RETREAT / RELOAD / current visible threat / low-health safety clear the epistemic hint.
- [x] deterministic runtime test proves attention can bias region choice while combat subsequently overrides it.

Causal path:

```text
EPISTEMIC HOTSPOT
      ↓
bounded attention choice
      ↓
existing spatial-task bias
      ↓
INVESTIGATE / OCCUPY / CONTEST / FLANK
      ↓
NfControlFrame
      ↓
Fuzzy Rail
      ↓
physical behavior
```

# AUTOMATED COMPLETION GATE

Current code candidate before this docs-only completion ledger:

`34b4f74aa850fe40dad7b21923f3edcb884338c5`

GitHub Actions run `31287162401`:

- [x] standard-check.
- [x] strict graphical Linux compile.
- [x] 8/8 CTests.
- [x] four-client combat/network smoke.
- [x] passive-player Human Rival encounter smoke.
- [x] situated Human Rival spatial ecology smoke.
- [x] Dream Cattler habitat/pack smoke aligned with ecological-return contract.
- [x] topographical energy smoke.
- [x] topographic lifeworld smoke, now including runtime recurrence/reseed/attention proof.

Eight CTests now cover:

1. simulation contract;
2. combat contract;
3. network contract;
4. Dream Cattler ecology;
5. topographical energy;
6. Cattler weak-point truth;
7. topographic lifeworld;
8. lifeworld runtime recurrence/reseed/attention.

- [ ] final exact-head CI must run again after this docs-only ledger commit; do not treat the pre-ledger SHA as final archival head.

# PAR — deliberate v1.0 non-goals

These remain intentionally deferred rather than silently incomplete:

- [x] no general GOAP/economic planner.
- [x] no omniscient actor utility/energy map.
- [x] no client access to private AI beliefs or helicopter truth.
- [x] no raw energy/infestation/ecology HUD meter as primary communication.
- [x] no universal trace TTL.
- [x] no single resilience/hysteresis/eventality/regime master scalar.
- [x] no scripted succession ladder.
- [x] no literal genetic evolution or opaque RL.
- [x] no final art/audio/shader pass.
- [x] no 8 km² production expansion.
- [x] no full Ghost Static economy/director layer.
- [x] no full JSON round-boundary load/restore yet.
- [x] no general multi-round persistence pipeline yet.
- [x] no explicit full Cattler ENTRENCH terminal behavior yet.
- [x] no gradual Hunt Surge composition/decomposition yet.
- [x] no ~500 m² Cattler carrying/mob-cap runtime contract yet; keep distinct from the ~800 m² landbase proof.
- [x] no general C27–C30 resilience/hysteresis/succession/regime engine yet.

# FINAL HUMAN ACCEPTANCE GATE — Pop!_OS

This is the **only remaining v1.0 acceptance gate** after final exact-head CI.

Target a roughly 4–6 minute natural recording. Do not force the intended story if the ecology diverges; divergence is evidence.

## Setup

```bash
git fetch --all
git switch build/v1.0-topographic-lifeworld
git pull --ff-only

./nightfall.sh clean
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh local
```

## Recording checklist

- [ ] reach normal `AUTHORITATIVE + PREDICTED` play.
- [ ] sanity-check movement/traversal/weapons/combat.
- [ ] identify whether the blue-black/petroleum-violet smoke/filament/synthetic-residue locality reads as **haunted Cattler habitat**, not missing floor/ordinary black geometry.
- [ ] observe Cattlers stalking/haunting the locality and Human Rivals contesting them through normal perception/combat.
- [ ] use another approach/flank if the geometry affords it; do not manufacture the sequence.
- [ ] kill/displace one or more local Cattlers.
- [ ] verify the defeated Cattler does **not** simply pop back at the local combat spawn.
- [ ] if local Cattler presence reaches zero, observe a genuine absence interval and the still-present environmental residue.
- [ ] remain nearby or return over ~45–60+ seconds if practical and assess whether the locality begins to recover rather than snapping clean.
- [ ] if a Cattler later returns during the recording, determine whether it appears to arrive/recolonize from elsewhere; server log should distinguish `[cattler-return] kind=RECOLONIZE`.
- [ ] inspect whether Rival movement appears more coherently related to contested/access/recovery hotspots without losing ordinary tactical combat behavior.
- [ ] F4 once if useful for region context.
- [ ] say the quiet part aloud if anything is extremely right or extremely wrong.

The **global-zero 45–90 second extinction failsafe does not need manual human proof** unless desired; deterministic runtime testing is the primary proof because waiting out extinction conditions is not useful gameplay labor.

## After recording

```bash
./nightfall.sh energy-smoke
./nightfall.sh lifeworld-smoke
tail -n 350 build/server.log
cat build/lifeworld-smoke.log
```

## Acceptance thesis

> **The v1.0 world can produce a small place whose Cattler occupation, contest, absence, haunted residue, recovery and possible recolonization are causally continuous; Human Rival attention can enter physical spatial behavior without becoming a second planner; actors and clients remain epistemically situated; and the existing networked FPS remains stable.**

If this human gate passes:

1. create immutable `archive/v1.0` at the exact accepted head;
2. mark PR #13 ready;
3. merge v1.0 to main;
4. begin v1.1 as research/questions first, using `docs/COMPARE5_LEDGER.md` before adding features.

# v1.1 handoff

The reusable Compare-5 shorthand ledger already exists at `docs/COMPARE5_LEDGER.md`. v1.1 must begin with source/video review + questions, not feature accumulation.

Start rule:

`WATCH/READ -> QUESTION -> C5 TAG -> CURRENT CODE -> GAP -> K/A/D/R -> SMALLEST CODE CONSEQUENCE -> COMPILE -> HUMAN VIDEO -> REASSESS`

Maxim:

> **Compare mechanisms, not feature inventories. Record deltas, not repeated essays.**
