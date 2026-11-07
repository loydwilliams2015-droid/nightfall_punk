# RET-C M2.4 / M2.5 closeout acceptance contract (draft)

Status: RESEARCH-ONLY. This document is a closeout *framework*, not evidence that M2.5 is implemented or that 1.8C.3/production can be promoted.

## Baseline and provenance
- M2.4 bounded incumbent: H268-H272, PR #204.
- Verified baseline SHA: `583a26eff132525038b9b21ea8399b283ea0a1d8`.
- GitHub Actions associated with baseline: 45/45 successful, including H268-H272 sanitizer matrix, H263-H267, H59 cumulative and B2 authoritative eligibility.
- Exact baseline evidence must be re-established for any new candidate SHA; CI pass on an ancestor does not automatically qualify the descendant.

## X=5 next independent determinations
1. **H273: Arrival-order reversal.** Build an actual source-backed first/second event scheduler with reversed order, not an input-supplied schedule. Verify TOI, A4 impulse, A5 identity, deterministic replay and rejection at unsupported geometry or tick boundaries. Preserve incumbent results.
2. **H274: Simultaneous and near-simultaneous events.** Compare deterministic canonical constraint ordering with an independent mass-weighted momentum/energy-loss oracle for perfectly inelastic collinear contacts. Randomize source roster order and event-time perturbations. Reject unresolved event ambiguity, no partial A5 ancestry.
3. **H275: Geometric generalization.** Replace collinear assumptions incrementally with real 2D/3D A4 contact normals, source-backed collider geometry, A5 static swept clearance and coupled impulse solver. Verify momentum along contact normals, nonpenetration, off-axis and high-speed boundary cases. Do not call a 1D result 3D acceptance.
4. **M2.5-A / H276: Authoritative integration.** Execute source-exact B2 eligibility checks and B3 contractual consequences together with A4/A5 under the A6 sole world-tick owner. The accepted immutable frame must be the single visibility decision; failures before commit leave physical and contractual projections unchanged. No diagnostic grants or fabricated witnesses.
5. **M2.5-B / H277: Durable recovery and performance.** Introduce persisted joint envelope/recovery with fault injection before and after stage, synchronization, durable commit, projection and restart. Verify complete reconstruction or rejection, deterministic continued ticking and replay suppression. Measure representative frame-time/throughput against the pre-M2.5 incumbent. Require independent race, record-lifetime, world-lifetime and transaction-safety checks.

## Mandatory evidence matrix (each candidate SHA)
- Canonical positives; randomized; pathological; boundary; held-out tests.
- Independent physical oracle where applicable and real source-backed A4/A5 receipt/history witness.
- GCC and Clang sanitizer builds with explicitly reported test counts, exclusions and known sanitizer gaps.
- H59, M2.4 regressions, B2/B3 authority and WAL/recovery; exact CI run IDs and SHA.
- Performance comparison: scene, actor/object counts, sample sizes, confidence intervals, p95/p99 frame time and determinism hashes.
- Provenance ledger: code diff, tests actually run, results, unresolved counterexamples, reproducible commands.

## Mandatory release holds
- M2.4 research acceptance != M2.5 integration acceptance.
- No A6 live publication, B2/B3 contract promotion, 1.8C.3 claim, or production release without the relevant real source-exact implementation and evidence.
- A rejected tick must not publish an actor, object, material history or contractual receipt in isolation.
- Keep PR #204 draft; create a separate M2.5 candidate/PR only after independent gates H273-H275 pass.

## Closeout report shape
For each determination report: hypothesis, incumbent/challenger, exact SHA, CI links, tests actually executed with totals, invariants, status PASS/FAIL/HOLD/NOT RUN, uncertainty, regression and next handoff.
