# nightfall!punk v1.6B — Weapon Authority / Mobility Ledger

## Status / basis

- Build branch: `build/v1.6b-weapon-authority-mobility`
- Exact basis: v1.6A candidate `23a791a23c132ba6dadd9e6f1ad54f709e666d2b`
- Build thesis: **the player owns aim intention; the server owns the physically realized firearm event; mobility changes the actor–weapon relation without becoming aim assistance.**
- Acceptance rule: compile/CI/data proof is implementation evidence, **not** human acceptance. Native video remains required for feel, readability, aim ownership and mastery.
- Movement–Combat Initiative (MCI) is deliberately **not** a v1.6B acceptance metric; reserve it for the Pac-Man comparative proof.

## FROM → TO

### FROM — v1.6A

- `NfWeaponEnvelope` correctly represented sequence-free CURRENT capability and `nf_weapon_realize_shot()` represented accepted-shot history.
- The focused graphical proof used the new law, but the production server still constructed an eye-origin center ray.
- Production protocol v5 had no Focus intent.
- Input command sequence still doubled as the production fire uniqueness/ancestry handle.
- Client prediction recorded movement/combat actor state but not Weapon Authority state.
- Physical muzzle and obstruction logic were proof-local rather than one shared production owner.
- Native evidence established strong CURRENT/LAST SHOT correspondence and Focus operation, but sustained RMB hold and the complete blocked accepted-shot sequence remained useful regression proofs.
- No persistent automated gameplay-sample database or OITOA soil-sampling pipeline existed.

### TO — v1.6B candidate

- Actor-local `NfWeaponAuthorityState` owns Focus, per-weapon instability, accepted-shot ancestry and named mobility stress.
- Protocol v6 sends **Focus held intention**, never client Focus amount/reticle/solved direction.
- Owner-only snapshot authority state supports prediction/reconciliation while keeping a 16-actor maximum packet at **1186 bytes ≤ 1200**.
- Accepted-shot sequence is separate from input sequence; rejected/dry/cooldown attempts do not consume ballistic ancestry.
- Server and predicting client use the same pure envelope/realization law.
- Q76 is explicit: **integrate newly commanded mobility first; then evaluate Weapon Authority/fire.**
- Shared `nf_weapon_geometry` owns body-relative muzzle origin and current-world first obstruction.
- Default lag compensation is **current authoritative shooter/muzzle + current authoritative world + bounded target rewind**; full shooter/world rollback is deferred unless falsified.
- Reload Focus remains legal but acquisition is slowed to **55%** of normal; release remains fast.
- Held movement/Focus state expires after **350 ms** without fresh accepted input.
- Sprint Focus retains the ~0.35 starting cap; engaging Focus does not hard-brake earned momentum.
- Mantle/vault/ladder firearm use remains legal; difficulty is represented through named embodied handling causes rather than an arbitrary fire prohibition.
- Redirect stress is a bounded kinematic relation (direction/speed change) plus support-transition stress; affordance-candidate score and camera turn are not ballistic causes.
- Focus recoverability is channel-specific: recoil highest; redirect/motion next; air/support lower; manipulator contamination least recoverable.
- Reticle presentation uses elapsed-time exponential smoothing; CURRENT remains sequence-free and LAST SHOT remains realized history.
- Deterministic scripted play can generate a persistent CSV sample database, OITOA report, credibility score and representative data “soil samples”; graphical scenarios can be captured automatically for selected samples.

## Stable coherence / authority owners

| Concept | Authoritative owner | Forbidden duplicate |
|---|---|---|
| aim intention | human/AI control intent | server camera correction / target magnetism |
| CURRENT firearm capability | shared pure Weapon Solution envelope | reticle-owned or demo-only formula |
| accepted shot ancestry | actor `accepted_shot_sequence` | input command sequence / global event sequence |
| material shot result | server | client solved direction/damage |
| Focus request | input command `focus_held` | client-authored Focus amount |
| Focus / recoil state | actor Weapon Authority | client/server sidecar formulas |
| physical muzzle | shared weapon geometry | eye/camera origin shortcut |
| world obstruction | shared current-world ray query | proof-only collider logic |
| prediction | client hypothesis + replay | authority replacement by presentation |
| Snap affordance selection | movement system | weapon cone / aim system |

## C implementation contract

- C11, fixed-capacity data, explicit ownership.
- No allocation in per-tick/per-shot Weapon Authority paths.
- Shared pure envelope + deterministic realization remain the only firearm mathematics.
- Causes are networked; presentation products are derived locally.
- Fixed 60 Hz simulation remains authoritative; render rate may only affect presentation interpolation.
- Weapon Authority replay uses the same movement-first phase ordering as the server candidate.
- Accepted fire applies recoil **after** realizing the shot that caused it.
- Per-weapon recoil may recover while inactive; switching does not magically erase it.

## v1.6B coding-contract priors

| Contract | Candidate |
|---|---:|
| stale held-input expiry | 350 ms |
| normal Focus ~95% acquisition | 180–260 ms target band |
| reload Focus acquisition scale | 0.55× normal |
| reload Focus ~95% acquisition | ~350–500 ms target band |
| Focus release | ≤200 ms target; >250 ms fail candidate |
| full-sprint Focus target | 0.35 starting prior |
| positional correction precedent | 0.035 m trigger |
| max protocol packet | 1200 bytes |
| v1.6B worst-case owner snapshot | 1186 bytes |

These are frozen **priors**, not sacred values. Retune only against a named falsifying test.

## Quantitative B PAR draft

MCI is excluded here and belongs to the Pac-Man test.

| Dimension | Candidate PASS | Warning | Candidate FAIL |
|---|---|---|---|
| input → predicted visible response | p95 ≤33 ms; p99 ≤50 ms | p95 33–50 ms | p95 >50 ms or repeated >80 ms |
| prediction position error | median ≤2 cm; p95 ≤5 cm; p99 ≤10 cm | p95 5–10 cm | p95 >10 cm or p99 >20 cm |
| normal Focus 95% acquisition | 180–260 ms | 260–300 ms | >300 ms / unstable |
| reload Focus 95% acquisition | 350–500 ms | 500–600 ms | >600 ms |
| Focus release | ≤200 ms | 200–250 ms | >250 ms |
| visible positional corrections | ≤2% snapshots in standard impairment | 2–5% | >5% |
| structural resyncs | ≤0.5% snapshots | 0.5–1% | >1% |
| correction p99 | ≤10 cm | 10–20 cm | >20 cm |
| worst ordinary correction | ≤25 cm | 25–50 cm | >50 cm |

Hard-zero invariants: duplicate authoritative damage; duplicate accepted-shot ancestry; client-authored damage; camera rotation caused by reconcile; indefinite stale Focus; through-obstruction target damage; future-shot reticle foreknowledge.

## OITOA evidence plan

**Observability** — sample rows expose scenario, seed, Focus, cone, redirect, support, recoil, speed and shot counts; network bot exposes prediction/authority errors and corrections.

**Inspectability** — named components replace opaque “movement accuracy” scores; debug presentation distinguishes INPUT / PREDICTED NOW / SERVER AUTH / LAST SHOT.

**Testability** — calm, Focus hold, reload Focus, sprint Focus, redirect Focus, pathological redirect and randomized controls run across repeated fixed seeds.

**Organizationality** — duplicate same-seed/script samples must be bit/text-identical at the recorded metric layer; server/prediction share Weapon Authority owners.

**Assessability** — positive/negative/pathological controls must discriminate; representative median rows are extracted as “soil samples” for close reading rather than cherry-picked anecdotes.

Credibility ordering remains:
`accepted project premise > source truth > deterministic invariant / negative control > native human evidence > typed PAR precedent > intuition`.

## Small automated sample database

`nightfall_v16b_sample_db` creates 42 deterministic samples by default:

- 7 scenarios: calm, Focus hold, reload Focus, sprint Focus, redirect Focus, pathological redirect, random control;
- 3 fixed seeds;
- 2 exact duplicate repeats per seed/scenario;
- 360 ticks per sample;
- strata: baseline, median, transition, pathological, random control.

`tools/analyze_v16b_samples.py` calculates executable OITOA status, an explicit diagnostic credibility score and one median-cone representative per stratum in `soil.csv`.

The database is **simulation evidence**, not human play evidence. Automated video generation selects representative/pathological scenarios; it does not manufacture outcomes.

## PAR sheet

| Reference / project benchmark | Dimension borrowed | nightfall synthesis | Current candidate disposition |
|---|---|---|---|
| nightfall v1.6A | present capability vs accepted history | productionize the proven semantic seam | **ADVANCING: production authority graft** |
| Quake / ioquake3 family | immediate mouse/hip-fire authority | immediate predicted intention + server material result; no mandatory ADS | **TARGET DEFINED; native/network proof pending** |
| Titanfall 2 | movement/combat coupling | movement changes firing opportunity through embodied state; no copied movement verbs | **DIFFERENT-BUT-COMPARABLY-VALID candidate; MCI separately Pac-Man-tested** |
| Jet Set Radio / Sunset-style motion guidance | high-authority continuity / affordance capture | Snap Locomotion remains non-exclusive and never becomes target aim assistance | **ARCHITECTURAL PASS; human stress pending** |
| Oxygen Not Included | interacting systemic physical state | same current world geometry constrains mobility, muzzle and material outcome | **ARCHITECTURAL DIRECTION CONSISTENT** |
| Prey (2017) | systemic material consequence | current-world obstruction and explicit impact provenance | **ADVANCING; blocked native proof pending** |
| F.E.A.R. | readable combat causality | shot provenance + diegetic world consequence | **ADVANCING** |
| Halo / Griesemer design method | controlled small-parameter tuning | acquisition/release/caps/recoverability remain named calibration variables | **MEETS METHOD PAR** |
| TLOU-II ranged-combat/debug methodology | camera/body/weapon/outcome correspondence | INPUT / PREDICTED / AUTH / HISTORY remain separately inspectable | **A method carried into network layer** |

External titles are typed precedents, never deductive premises and never imitation targets.

## EXISTING / ADVANCING / LATER

### EXISTING — preserve

- v1.6A CURRENT envelope / accepted-shot realization separation.
- actor-local AI beliefs, reports, claims, tactical candidates, metabolism, contamination/stasis and lifeworld systems.
- Snap Locomotion / traversal candidate ownership.
- server truth, bounded target rewind, fixed tick and input redundancy.
- competent hip fire; Focus does not increase damage.

### ADVANCING — v1.6B

- production Weapon Authority state and protocol v6.
- owner prediction/reconciliation and accepted-shot ancestry.
- physical muzzle + material obstruction.
- mobility-first shot phase ordering.
- named redirect/support handling causes.
- automated scenario/sample/video evidence pipeline.
- network impairment and correction-tail measurement.

### LATER / deliberately not smuggled into B

- Movement–Combat Initiative acceptance metric: Pac-Man comparative proof.
- autonomous AI strategic Focus policy.
- full shooter/world physics rollback.
- projectile conversion, destruction and recoil impulses on world/player body.
- new traversal verbs merely to imitate reference games.
- communication-causality / independent-appraisal social-AI layer.

## Acceptance gates

1. strict compile with `-Wall -Wextra -Wpedantic -Werror`;
2. all inherited tests + v1.6/v1.6A/v1.6B contract tests;
3. protocol v6 Focus and 1186-byte worst-case packet proof;
4. deterministic sample DB + duplicate-repeat determinism;
5. OITOA positive/negative/pathological discrimination;
6. network bot under 100 ms ±25 ms / 2% loss;
7. automated graphical sample capture;
8. native human recording for sustained RMB, movement/Focus correspondence, reload Focus, traversal fire, recoil ownership and blocked-shot material readability;
9. no merge on CI alone.
