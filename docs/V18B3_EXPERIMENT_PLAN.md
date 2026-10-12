# 1.8B.3 physical interaction experiment registration

2026-10-11 (America/Chicago). New H2 source experiment, not accepted release.
Pinned B2 lab ancestor: f30ef6b48179afd28f0911b9922cdcf41af6fd33.
Runtime prerequisite: PR44, 16949c490ab10aa65edcdcbd15ddb22803ab44d2.
Contract: PR34, 78c7917c9a1198f22fd6d3d0dd7f77908d037c0b; EEC-01.
Historical H1 source remains unrecovered; B2's HOLD is not promoted.

## Scoped implementation

Exactly one live NfWorld tick publisher. The existing A6/B2 owner runs on a
private transaction snapshot, followed by B3 manipulation and fresh publication
checks; private simulation is not another live movement authority. The whole
world, registry, resource and event assignment is local and single-threaded.
Physical-only WAL is explicitly refused; B5 durable aggregate recovery remains
open. Keep the inherited one-physical-actor guard; B4 is separate.

Sliding doors and mechanical switches use finite-force A4 motors and actual
bounded displacement. Pickups require server-derived hand proximity and material
clearance, then exactly-once conserved ownership transfer. Pushing uses actual
A6/A4 contact. Carrying is a bounded hand coupling with reciprocal motor impulse
and verified floor response to an explicit carried-assembly weight field,
within A6's grounded, axis-aligned model. External gravitational impulses are
recorded separately; general world gravity/falling is not inherited from A6.
Release preserves pose/velocity;
general falling/rotating rigid-body behavior is not claimed. Repair requires
actual tool stroke, finite power and salvage; partial consequences survive
interruption. No animation-derived authority, teleport, invented impulse,
automatic refund or silent unsupported traversal success.

## Models and independent evidence

M4 provisional incumbent: material-first B2 authorization plus conservative
scoped broad-phase rejection of disjoint collision candidates. M3 mandatory
challenger: exhaustive B2 screening and all-collider CCD. Both use the same
real physical solvers; no busy-work timing proxy. M0 is an immediate scripted
laboratory negative control and cannot be selected in production.

Register 48 canonical, 256 randomized, 96 pathological, 96 boundary, 256
held-out indexed fixtures (752), each paired across M3/M4, plus M0 control
rows. Three dependent timing repeats. Calibration seed 0xb318cafe; held-out
seed 0xb3eec019. Random cases vary safe poses and far collider counts; boundary
configurations repeat and are not independent worlds. Model order rotates.
Authored truth covers all seven requested actions, blocked/pending/stale
authority, physical jam/load, interruption before/after repair threshold,
failed publisher, exact hand boundary/one ULP beyond, nonfinite values, event
capacity, replay and observer equivalence. Save failures, including first runs.
No held-out failure may be discarded or silently used to tune an exit claim.

Independent fixtures assert material poses, inventories, consumed salvage,
token/power conservation, released slots and actual receipts. Compare paired
semantic results, replay stability and causal attribution. Monotonic elapsed timing excludes
fixture setup and output; measures complete real owner calls at dt=1/60,
with proposal and completion-tick latency reported separately. Use isolated
timing after compilation and network checks; 16.667ms owner-call overruns are a
headless budget check, not FPS or perceived latency. CLOCK_MONOTONIC includes
scheduling delays; these samples do not measure thread CPU utilization.

## Exit gates

Mandatory: zero unauthorized effects, phantom receipts, duplicate transfer or
debit, orphan terminal slots, stale/pending success, unsupported traversal
success, observer interference or conservation/replay mismatch in authored
tests. M0 must expose sensitivity. Passing these gates precedes balanced cost,
tail latency and responsiveness comparison; correctness cannot be averaged away.
Run strict GCC/Clang, ASan/UBSan, full Linux engine, real network proof and CI.
Human graphical acceptance, native Pop!_OS, broad rigid-body coverage, B4/B5
and historical provenance remain separate OPEN gates. Publish executable source,
raw data/hashes and six RET classifications, with an explicit HOLD/MERGE decision.
