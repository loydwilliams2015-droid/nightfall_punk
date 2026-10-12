# 1.8B.3 physical interaction — PRAXIS / SIGNAL / STEWARD

2026-10-11 (America/Chicago). **New executable H2 source. HOLD release; M4
provisional authored-domain exit candidate, M3 admissible mandatory challenger.**
B2's locked HOLD and historical provenance remain unchanged. No main merge,
historical source promotion or human H4 acceptance is authorized by this result.

## Contracts and source

The user-pinned B2 ancestor is `f30ef6b48179afd28f0911b9922cdcf41af6fd33`.
The required live bridge is PR44 at `16949c490ab10aa65edcdcbd15ddb22803ab44d2`,
verified as a descendant; its nine check runs reported SUCCESS when retrieved.
The [B2 ledger](LEDGER_V18B2_EEC_LIVE_TICK.md) and
[EEC-01](V18B2_ELIGIBILITY_EXECUTION_ENCOUNTER.md) were read. PR34's B1–B5
contract was retrieved at `78c7917c9a1198f22fd6d3d0dd7f77908d037c0b`.
B2 strict compiler/sanitizer checks were independently rerun before B3 changes.
Exact PR41 new B1 source and the separate positive supplement were also rerun.

Historical H1 source remains unrecovered; issue37 and the 38/55 erratum remain
OPEN. Historical 2304/2304, 2176/2304, 55/55 and 6000-seed reports are inherited,
not new historical-source executions. PR41's original 6000 fixtures have no
positive commits; its separate 256-case supplement has three commits and 253
refusals. B3 success does not promote that lineage.

The [registered plan](V18B3_EXPERIMENT_PLAN.md) was committed as `baf02c6`
before corpus execution. Later wording clarifies the existing CLOCK_MONOTONIC
timer; no seeds or acceptance gates were silently retuned. Final raw CSVs,
initial raw failure rows, pre-fix source snapshot, source/data SHA-256,
machine/ABI metadata and comparisons are committed in
[evidence/1.8B3](evidence/1.8B3/comparison.json).
Branch `agent/v18b3-physical-interaction` is stacked on PR44.

## PRAXIS — implemented physical realization

`nf_interaction18b3` stages A6/B2 on private reused transaction scratch, then
manipulates and revalidates before **one live NfWorld publication**. Private
simulation is not a second live movement authority. World, physics, registry,
resources and causal events publish under the single dispatcher. A read-only
publication validator may refuse; deliberate live contract mutation is caught
by an exact state fence. This is single-threaded in-memory atomicity. A
physical-only WAL is refused; B5 durable aggregate recovery is not fabricated.
The inherited one-active-physical-actor guard remains; B4 is separate.

| Interaction | Realized mechanism |
|---|---|
| Door open/close | Guided sliding box, finite A4 motor, bounded stopping velocity, full material/collision sweep and actual mirrored collider. Jam retains previously applied motion. |
| Switch | Forward/reverse mechanical stroke; switch/power state changes only after actual travel and settling, with a distinct event. |
| Pickup | Server-derived hand proximity plus material/obstacle sweep; conserved world-unit to inventory transfer and collider removal. No fabricated impulse. |
| Push | Actual A6/A4 capsule–box response and applied-only A5 history. Known first denial precedes the actor motor. |
| Carry/release | Finite powered hand coupling, reciprocal impulse, separately recorded external weight impulses, actual floor support/load, capacity and swept coverage. Release preserves pose/velocity. |
| Repair | Actual powered tool strokes consume one salvage unit and token per completed stroke. Partial health and history persist after interruption. |
| Interruption | Before-effect cancellation leaves no physical/resource change. Partial effects persist, terminal slots release, hand constraints clear and compensation is explicitly required. |

Collision and actuators are axis-aligned. The enclosing capsule includes
endcaps and may conservatively refuse a tight fit. Rotating geometry and
unimplemented ladder/ledge/vault/mantle return UNSUPPORTED. Repair changes a
modeled health component, not arbitrary voxel topology/fracture. Carry introduces
a local carried-assembly weight model; A6 outside it does not promise general
world gravity/falling. Positive signed kinetic motor work consumes power;
external gravity and dissipative braking are distinct, without a claimed exact
global energy-integration proof. General compensating repair/refund/rollback
policy remains OPEN; prior facts are never silently erased.

A5 actor contacts and added carry support form one per-tick contact set.
Motor/ownership/switch/repair events are not fictional contact samples. Failed,
stale or full-storage publication cannot expose staged body/resource/history
writes. Retained action IDs prevent replay. Explicit trusted event acknowledgements
free consumed storage; longer-run A5 outbox management remains a requirement.
Current fixed prototype ABI: world 350,656 bytes, A6 runtime 65,952, B2 105,256,
B3 **1,039,056 bytes**, including reused scratch. This reviewable copy transaction
is not an accepted multi-actor scheduler design.

## SIGNAL — executed results and retained failures

**216 targeted checks pass**: all requested actions; body/collider correspondence;
real force/support; load; inventory/power/salvage/token conservation; replay;
early/late interruption; failed publication and retry; late contract change;
unknown material; full events; physical-only WAL refusal; actual obstruction;
invalidated lease; observation and camera ON/OFF equivalence. Scene construction
shares authored inputs with the native viewer, never the production oracle.

**752 indexed fixtures**: 48 canonical, 256 randomized, 96 pathological,
96 boundary, 256 held-out. M3/M4 paired, M0 laboratory ablation. Three dependent
repeats yield **2,256 model-case IDs per repeat, 6,768 rows**, including 4,512
admissible rows. Seeds `0xb318cafe` / held-out `0xb3eec019`. Repeated boundaries
and timing repeats are not independent representative game worlds.

Final M3/M4: **zero authored mismatches, unsafe effects, conservation violations
or semantic replay/pair differences**. Authored case-level effect detection per
model across dependent repeats: 1,038 true positives, zero false positives or
false negatives, precision/recall 1.0. This measures effect presence, not general
physics or per-contact accuracy; actual receipts are separately falsified by
fixtures. M0 has 2,256 authored errors and 1,218 unsafe effects across repeats;
its immediate mutations omit authority/realization and are not production policy.

Retained failures are part of this result:

- First targeted run had six failures from an invalid hand capsule. The box
  enclosure was corrected to include endcaps/valid height; carry inputs include
  clearance for that conservative volume.
- A tiny floor-solver velocity sampled an unloaded below-floor chunk. Projection
  is restricted to the verified normal constraint's numerical tolerance, without
  moving bodies or inventing receipts. The later explicit weight model records
  external force separately and applies actual support impulses.
- First corpus: **12 calibration/pathological failures, zero held-out failures**.
  Jammed/unsupported push reached A6's actor motor before refusal and moved the
  target. Refusal now precedes staging. All first rows, identified mismatch IDs
  and SHA-matched pre-fix source remain committed; final repeats pass.
- CMake compiler-alias reconfiguration dropped the offline flag from a reused
  cache. `cmake --fresh` now supplies the intended configuration (CMake >=3.24;
  verified 3.31 locally). Failed logs remain in the dossier; zero-test runs are
  never counted as success.

Strict GCC14.2 and Clang19 C11 `-Wall -Wextra -Wpedantic -Werror -O2` pass
**11 offline CTests each**; ASan/UBSan passes 11. Full client, server and native
B3 viewer compile under both compilers; **30 full CTests pass under each**.
Local platform is Debian13. The inherited B2 authenticated replay/contact proof
still passes after the shared server gained the opt-in B3 route.

Network: **14 authenticated action/model runs** with 20ms transport latency,
10ms jitter, 2% loss and deliberate duplicates. Each has one grant, realized
completion, conserved resources and a released slot. Server receipt-to-disposition
was 0–4ms at integer resolution; zero is below logging resolution, not zero
network latency. Scripted completion: doors 544–554ms, switches 171ms, pickup
3ms, push 0–3ms, repair 653–655ms, commanded carry/release 1203–1204ms. These
are observations, not latency distributions or human feedback. See the committed
[network comparison](evidence/1.8B3/network-comparison.json).

`nightfall_v18b3_demo` reads actual bodies/colliders/resources and provides local
use, cancel/release, reset and jam injection. It is an H2 native fixture viewer;
the legacy network FPS client does not yet replicate B3 object/ability snapshots
into its UI. Engineering head `b56bd9da8a628f769c5c348749c8e820bfbe34d8` passed all
**ten GitHub checks**, including all seven native M4 Xvfb scenes and 14 B3
network action/model cases. The retrieved seven screenshots were visually
inspected by the assistant. [CI_ENGINEERING.md](evidence/1.8B3/CI_ENGINEERING.md)
records exact source head and results; this is automated/H2 evidence. No local
display or human H4 trial was performed. The separate B3 corpus artifact ZIP
remains blocked at an added Azure hostname; local raw data are committed and
system evidence/logs were successfully retrieved.

## PAR — balanced selection

Final timing ran after compiler/network work stopped. CLOCK_MONOTONIC includes
scheduling; construction, output and semantic hashing are excluded. Calls use
dt=1/60, following actual action trajectories. Elapsed processing is distinct
from action wall duration and from thread CPU utilization.

| Native elapsed statistic | M3 | M4 |
|---|---:|---:|
| Held-out maximum owner call per fixture, p50 | 67.080 microseconds | 65.328 microseconds |
| Same statistic, p95 | 158.547 microseconds | 121.412 microseconds |
| Same statistic, p99 | 1.904123ms | 1.437073ms |
| Positive complete action owner total, p50 | 0.712577ms | 0.685176ms |
| Same statistic, p95 | 2.615598ms | 2.501836ms |
| Same statistic, p99 | 4.041205ms | 3.563809ms |
| Owner calls above 16.667ms | 0 | 0 |

Maximum-per-fixture percentiles are not percentiles of all individual frames.
Positive mechanics require door 34 ticks, switch 11, pickup/push 1, repair 40,
carry 25 including commanded release. M3 and M4 meet the authored binary gates;
M4 has lower measured tails here, so **retain M4 provisionally**. Three dependent
repeats and this narrow corpus establish neither universal superiority nor
population-zero failure. M0 cannot be rescued by cheap immediate mutation.

## RET and release gates

| RET class | Claim and practical limits |
|---|---|
| A priori | Eligibility is necessary, not realization; interruption preserves applied facts; refund requires a new transaction. Adopted contracts. |
| Formally established | No new machine-checked end-to-end proof. Narrow array-bound/enclosure arguments apply under finite-input/single-owner premises; executed tests are not proofs. |
| Empirically validated | Listed source-linked tests, corpora, compiler/sanitizer/engine/network runs and later explicitly recorded CI executions only. |
| Highly predictable | Equivalent conservative broad-phase omission saves disjoint collision queries; general gain depends on scene distribution/copy cost. |
| Uncertain | Production tails, tight-fit refusal cost, extended carry stability, scheduler/frame behavior, feedback and perceived responsiveness. |
| Unresolved | Historical lineage, rotation/general falling/traversal, B4 fairness, B5 durability, native Pop!_OS and H4. |

| Acceptance gate | Status |
|---|---|
| Pinned ancestry/contracts/source/data/reproducibility | PASS in declared new-source scope |
| Requested interactions, interruption, actual receipt/material causality | PASS in authored grounded/axis-aligned scope |
| No denied/stale effects, duplicate transfers/debits, orphan terminal slots or observer differences | PASS in executed cases |
| Independent five-stratum comparison, negative-control sensitivity, replay and resource conservation | PASS |
| Strict compilers/sanitizers/full Linux engine and authenticated network proof | PASS |
| Native automated rendering and engineering-head remote CI | PASS at b56bd9d; exact-head records linked above; no H4 inferred |
| General scene PAR, rotation/falling/traversal realization | OPEN; explicit declared limits/unsupported domains |
| Full FPS client B3 replication and human play-feel | OPEN |
| B4 shared actors/fairness, B5 durable aggregate recovery | OPEN / deferred |
| Historical/new-lineage approval, native Pop!_OS and human release approval | OPEN |

**HOLD release.** This is reviewable 1.8B.3 H2 source with a provisional M4 exit
candidate. It does not accept B2's open lineage gates or authorize a main merge.
Reproduce: `bash v18b3.sh all`, `bash v18b3.sh engine`,
`python3 tools/check_v18b3_network.py`; isolated timing: `bash v18b3.sh benchmark`
after other work stops. With a working display, `build/full/nightfall_v18b3_demo`
uses keys 1–7, E, Space, Right arrow, R and J. The dossier distinguishes first
failures, final local results, inherited reports and exact-head remote CI.
