# 1.8B.2 EEC live-owner handoff — codex nightfall dev chat one

2026-10-11 UTC. PRAXIS, SIGNAL, STEWARD responsibilities executed in this
conversation. **HOLD release; retain M4 provisionally, with M3 admissible.**
These are new-source H2 results, not recovered historical H1 or human H4 proof.

## Source authority and inherited evidence

Fetched and verified PR #42 at `f30ef6b48179afd28f0911b9922cdcf41af6fd33`
and PR #43 at `f377d662c2e4bf70a38fbbf1d8255527022f7fc8`. PR #43 has only
the three handoff documents and is based exactly on f30ef6b. This follow-up
branch, `agent/v18b2-eec-live-tick`, starts from PR #43; neither PR was merged,
closed, or promoted. At baseline verification all 11 PR #42 checks reported
SUCCESS. PR #43 had no checks. Remote reports are distinguished from local runs.

Before modification, the PR #42 six raw CSVs, comparison, machine metadata,
source provenance and targeted output were copied and SHA-256 checked against
the committed ledger. [BASELINE_FREEZE.json](evidence/1.8B2-EEC/BASELINE_FREEZE.json)
identifies those frozen bytes. All prior B2 laboratory, full-engine and exact
PR #41 reproducibility commands were rerun successfully at that baseline.
The baseline logs and raw frozen bytes are included in the executable dossier.

The PR #42 parent contains closed PR #40's experimental eligibility module
(`71cdb3b`). Inspection of its 67-line module and the PR #42 diff confirms
that `ce30679` substantially replaced its implementation and added the new
evaluator/resolver; ancestry is retained, not presented as clean-room history.
No code from PR #40 was newly cherry-picked or merged by this follow-up.
Existing A6, A4, A5 and broader game systems remain inherited source.

Issue #37 remains OPEN. Fresh release enumeration found no releases/assets;
the issue and comments were retrieved again. The exact original archive has
not been recovered or hash-matched. PR #41 is exact copied **new B1 H2 source**
at `3fb792eab6e035e1a9736db7dbcf22fc36bac410`, not original H1.
Its original 6000-seed rejection-only coverage and separate 256-case supplement
(three commits, 253 refusals) were rerun under GCC, Clang and ASan/UBSan.
Historical 2304/2304, 2176/2304 and 55/55 remain inherited reports; RP-01's
38/38 discrepancy remains unresolved. No historical-source promotion approved.

## PRAXIS — delivered encounter boundary

`nf_encounter18b2` attaches begin, prepublication and finish hooks to the
already-bound A6 `nf_world_step_checked` owner. It installs no movement owner.
The dedicated server's opt-in `--b2-encounter-proof` path binds the real owner;
the default multiplayer server retains its existing scheduler. Authenticated
client input conveys an attempt; actor identity is taken from its session,
and object/credential/inventory facts come from server authority.

Snapshots combine actual NfWorld actors/colliders, the physical A5 world, and
a bounded server registry. World, material, pose, object, contract and geometry
revisions invalidate stale witnesses. Solid and ladder geometry changes count.
Physical-object registration binds its immutable material identity epoch at
registration; object-state revision changes do not replace it. Typed
cell/nexus/slot aliases are rejected. A3 clearance/support/reach and full A5
canonical/fine sweeps are reused; unknown material stays pending.

Up to 64 queued proposals, 16 shared slots per object, 64 retained receipts
and 128 traces are explicit bounds. Withdrawal releases prospective contention;
negative assessments require a fresh proposal. Resolution orders actor/action
IDs, then stages slots, token debits and consumable inventory. Same-tick
conflicts and exhausted storage cannot duplicate grants or evict receipts.
Prepublication revalidation runs under the existing single tick owner.
Successful A5 body/history publication is followed by prevalidated, nonfailing
in-memory registry assignment. Failed/stale/denied/full publication leaves
body, resources, slots and physical history unchanged. Diagnostic revision
refresh and failure traces can change; there is no claim of byte-identical
metadata or thread-safe arbitrary external setters.

Eligibility leads to COMMITTED authorization and an EXECUTING handoff, never
automatic physical completion. Only an actual applied A4 receipt completes the
scoped push encounter. A receipt is attributed to one active handoff, not
every pending attempt. Post-commit interruption preserves physical history and
records COMPENSATION_REQUIRED; it does not invent a refund or physical reversal.
The observer copies traces without authorizing action; `visual_correction` is
a diagnostic flag, not an implemented client UI or human observation.

The optional A5 WAL persists physical state only. Failed writes and torn files
fail closed. Recovery of a physical-only checkpoint is refused while B2 is
attached, because it cannot recover registry/debit/receipt state atomically.
This is a minimal B5 interface and an OPEN durability gate, not crash-proof B2
transactions. Full B3 realization, B4 multiplayer scheduling/fairness and B5
registry persistence were deliberately deferred as instructed.

## SIGNAL — new executed results

The [registered comparison plan](V18B2_ENCOUNTER_COMPARISON_PLAN.md) preceded
the corpus. M3/M4 receive the same 672 indexed fixtures: 32 canonical, 256
randomized, 64 pathological, 64 boundary and 256 held-out. Separate seeds are
`0xa6b218b2` and `0xeec018b2`. Boundary configurations repeat; three timing
repeats are dependent observations. **1,344 model-case IDs per repeat, 4,032
rows total; zero authored mismatches, unsafe publications, conservation
violations or semantic replay/pair differences.** No held-out failures discarded.

All ten EEC fixtures enter the actual owner. The independent suite now has
**120 checks, zero failures**, including actual A4 contact/history, cancellation,
contract/object/material changes, full receipts, failed publication, a contract
change injected immediately before publication, WAL write failure and torn
physical snapshot, retry, 64-proposal conflict, withdrawal during contention,
ladder geometry revision, and observer noninterference.
EEC03 proves two prospective grants from three real body snapshots, then
asserts the inherited A6 multi-actor execution guard. It does **not** execute
two shared live grants. The separate inherited 66-check suite tests 2/4/8/16
snapshot contenders and permutations. Live multi-actor acceptance stays OPEN.

For each model, EEC05 demonstrates one positive attributed physical contact,
zero fabricated initial receipts, preserved history on interruption and no
second debit on replay. On these two narrow model-fixture observations,
attribution precision/recall are 1/1. The one-frame corpus has zero expected
contacts: its absence-of-fabrication result cannot establish positive recall.

Strict C11 `-Wall -Wextra -Wpedantic -Werror`: GCC 14.2 and Clang 19 pass all
10 offline CTests; GCC ASan/UBSan passes all 10. Full Linux graphical
client/server and tools compile under both GCC and Clang; **29 CTests pass
under each**. Local host is Debian 13, not native Pop!_OS. Two inherited files
lacked final newlines and failed Clang `-Werror`; only their final newline was
added. Compiler failures and initial missing-executable CTest output are retained.

The first EEC fixture run failed two checks because its assumed material epoch
was 1; actual canonical load advances the grid epoch. The fixture now binds
the actual input epoch rather than weakening the check. The initial 84-check
failure log and subsequent 102/120-check passes remain in the dossier. A server
static-link order failure was also corrected before the successful full builds.

Local authenticated M3/M4 network runs inject 20ms transport latency, 10ms
jitter and 2% loss, plus duplicate packets. Each publishes one grant, debits
10 to 9, reaches actual A4/A5 contact/history, and records interruption without
erasure. Final runs recorded server receipt-to-disposition **5ms M3 / 3ms M4**;
these are two observations at integer-ms resolution, not comparative latency
statistics or end-to-end player latency. Each attempts 360 owner frames at a
60Hz wall schedule, but commits 23 and leaves 337 pending. Actual movement
reaches x=1.9605; the small canonical fixture does not sustain six seconds of
committed physical ticks. The precise pending frontier needs broader diagnosis;
do not interpret it as demonstrated realtime gameplay throughput.
The separate legacy four-client combat/network smoke passes; it does not
prove multiplayer EEC integration. Remote CI results are recorded separately.

## PAR — isolated CPU measurements and exit candidate

Timing includes proposal and a complete A6 owner tick at dt=1/60, with fixture
construction and CSV output excluded. Final timing repeats ran sequentially
after all compilation and network work finished; preliminary overlapping
build observations are excluded from this comparison. These are headless CPU
measurements on the recorded Xeon host, not wall-clock display frame timings.

| Measured total CPU scope | M3 p50 / p95 / p99 | M4 p50 / p95 / p99 |
|---|---:|---:|
| All cases | 2.493 / 11.869 / 16.082 microseconds | 2.496 / 11.879 / 16.319 microseconds |
| Held-out | 2.500 / 11.817 / 12.502 microseconds | 2.503 / 11.835 / 13.170 microseconds |
| Published positive subset | 11.837 / 14.764 / 67.443 microseconds | 11.857 / 16.876 / 76.017 microseconds |
| CPU 16.667ms overruns | 0 / 2,016 rows | 0 / 2,016 rows |

Both pass the authored correctness/causality/conservation/replay gates. M3 has
the lower measured positive-subset p95 here; M4's earlier library-call speedup
does not establish an integrated advantage. Differences at this scale, these
dependent repeats and this narrow domain do not justify switching the
incumbent or claiming general superiority. **Retain M4 provisionally; M3 stays
mandatory and admissible.** Optimize the programme by leaving later-build
responsibilities explicit rather than constructing an unvalidated scheduler.

## STEWARD — RET, acceptance and unresolved questions

| RET class | Claim and evidence limits |
|---|---|
| A priori | Physical and contractual eligibility are necessary conditions; they do not logically imply a contingent action succeeds. This is the EEC-01 design contract. |
| Formally established | Inherited narrow LCG-parity and bounded grid arithmetic arguments only. No formal proof of the live engine, atomic crash recovery, concurrency or all-world safety was performed. |
| Empirically validated | Only the executed baselines, 120 fixtures, 672 indexed paired cases, listed compiler/tests and authenticated network/legacy smoke results. |
| Highly predictable | Cheap refusal gates can avoid expensive physical staging on invalid proposals; general savings depend on encounter distribution. |
| Uncertain | General causal fidelity, mixed-world performance, stable player latency, the pending network frontier, fairness and observer presentation outside these constructed cases. |
| Unresolved | Original H1 archive/38-versus-55 erratum, shared live multi-actor integration, B3 realization, B5 persistent registry transaction/recovery, native Pop!_OS and human H4 acceptance. |

| Acceptance gate | Status |
|---|---|
| Pin/freeze prior code, contracts, data and execute reproducibility | PASS |
| Single real A6 owner; authoritative fresh snapshots and local publication | PASS in declared single-physical-actor scope |
| Ten EEC falsification fixtures, paired independent truth, compiler/sanitizer/full Linux tests | PASS in listed authored domain |
| Actual physical effects/history, failed-publication no-effects, replay/debit/slot conservation | PASS for executed cases |
| Live shared multi-actor EEC, sustained fairness/committed 60Hz gameplay | OPEN; inherited owner rejects multiple active actors |
| Atomic durable registry/body recovery after crash/reboot | OPEN; B5 minimal fail-closed interface only |
| Graphical client encounter correction and human input-to-result acceptance | OPEN; no new EEC client UI/H4 evidence |
| Native Pop!_OS compatibility | OPEN; Debian local and Ubuntu CI are separate evidence |
| Historical-source recovery or explicitly approved new lineage | OPEN; issue #37 remains open |
| Human release acceptance/merge | OPEN; no approval recorded |

**Recommendation: HOLD.** This source is reviewable as an H2 engineering
increment, not an accepted B2 release. No main merge, lineage promotion or
notification to another chat was performed. Other chats can retrieve this same
GitHub ledger; their private message histories cannot be synchronized remotely.
The questions above remain in their owning milestones; no unexecuted prediction
is substituted for a passed gate.

Reproduce: `bash v18b2.sh all`, `bash v18b2.sh engine`,
`bash tools/check_v18b1_pr41.sh`, `python3 tools/check_v18b2_network.py`,
`bash nightfall.sh combat-smoke`. For isolated profiling, stop compilation and
network work before `bash v18b2.sh encounter-benchmark`.
Raw CSV hashes, source hashes, compact comparison, network logs and targeted
output are in [evidence/1.8B2-EEC](evidence/1.8B2-EEC/encounter-comparison.json).
The executable dossier also includes baseline frozen raw data and failed logs.
