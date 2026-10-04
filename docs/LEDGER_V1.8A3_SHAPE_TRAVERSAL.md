# nightfall!punk 1.8A.3 — Shape & Traversal: Dual Jurisdiction Laboratory

**Candidate**: capsule CCD + rounded-box-like foot support, with simultaneous geometry and traversal adjudication.

## Contract ontology (conditional a priori entailments)

- A legitimate traversal state transition requires **both** geometric permission G and traversal authorization T. `commit_eligible = G && T`. Neither permission overrides the other.
- G is a read-only query over actual material shape, clearance, swept path, stable landing support, feature identity and geometry revision.
- T is a separate read-only check against the server's actor/feature/tick/world-version/collider-size/origin/grounding/ladder/support/step/reach authority snapshot.
- The result is only **eligibility**. Atomic world-state commit, motion work and dynamic contacts still belong to the world/motor solver.
- A ladder is a trigger (affordance), not a solid barrier or self-grounding support. Ladder exits need a distinct physical landing.
- The bounded flat-foot proxy constrains support acceptance but **does not replace** the actual physical capsule with a larger/smaller solid body.
- Unsafe policies are available only in test binaries compiled with `NF18A3_TEST_CONTROLS`; production calls reject them.

## Highly predictable but defeasible priors

- Capsule CCD should reduce false side-edge hits compared with swept upright box, while retaining materially accurate TOI on AABB surfaces.
- A finite flat support patch should reject marginal edge contacts that otherwise appear grounded.
- A dual-authority policy will reject material permission without actor authority and actor permission without viable geometry.
- Stair negotiation should remain conservative on unsupported/dynamic platforms until an integrated dynamic contact island exists.
- Static capsule/AABB exact sweep here does not imply exact mesh/rotating-body CCD, arbitrary slopes, or GPU/network evidence.

## Controlled five-way study

- A: swept box + dual jurisdiction, retained incumbent;
- B: true swept capsule + dual jurisdiction;
- C: true swept capsule + independent flat-foot support patch + dual jurisdiction, selected H1 exit candidate;
- D: permissive climb policy, negative control (intentionally violates measured step limit);
- E: contract-bypass policy, negative control (intentionally ignores valid authorization).

All candidates are tested on shared seed/world/input fixtures. First corpus: 4,000 distinct stratified cases (20,000 model evaluations); separate held-out corpus: 1,200 near-edge landing cases (6,000 evaluations); independent long-double numerical trajectory oracle: 2,400 cases. Fixture classes and measured outcomes reside in `build/v18a3/PAR_RESULTS.md` and CSVs, not guesses.

## Exit disposition and restrictions

**C is H1 laboratory-exit eligible**, not gameplay-branch/production physics certified. There is no integrated dynamic motor, contact island, network replay, moving-platform 6-DOF solve, general mesh CCD, validated human FPS movement feel, or durable event-history integration in this subsystem. The candidate must pass end-to-end 1.8A.4 dynamic-body integration and 1.8A.6 human embodiment trials before unqualified adoption.

## Known safety implementation boundary

The `Nf18a3Contract` is a **server-provided C snapshot**, not an authenticated capability if constructed from untrusted client bytes. Reject origin/body-state/step-limit/reach changes against authoritative snapshot, then let the world transaction arbiter finalize. Unsafe controls compiled into test target only. The diagnostic witness hash is reproducibility evidence, not a security signature.

## Technical and industry correspondence

- PhysX's capsule constrained-climb caveat motivates independently measured stair-height gates.
- Jolt and Rapier separate stair and floor support logic from generic collision sweeps.
- This project introduces an explicit geometry-versus-contract dual-jurisdiction gate, without forcing semantic relations to manufacture physical contact.
