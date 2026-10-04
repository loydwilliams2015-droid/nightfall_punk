# nightfall!punk 1.8A.4 — Dynamic Interaction: experimental policy ledger

## Prior lock carried unchanged
1.8A.3 **Model C** is the H1 shape/traversal exit candidate: upright swept capsule, independently tested rounded-box-like foot support, and simultaneous geometry AND traversal authority. It is NOT production physics. The old swept box remains a diagnostic comparator. In particular, a successful stair/ladder adjudication grants the authority to *attempt* a bounded physical motor action, never instantaneous position relocation.

## New 1.8A.4 laboratory law
1. Ordinary input feeds a bounded force/acceleration motor acting on an explicitly mass-bearing body. Motor work (including negative braking work) is accounted independently of contact impulses.
2. A genuine dynamic contact modifies the participating body velocities and, for off-centre contacts, their angular velocities; normal and tangential impulses must have equal-and-opposite body effects.
3. A contact-only estimate without actual reciprocal state change is a negative control and inadmissible as the dynamic player physics implementation.
4. All physical collisions remain subject to material geometry. The 1.8A.4 swept-capsule bridge uses the 1.8A.3 capsule/AABB narrowphase before attempting an actual pairwise impulse exchange.
5. Contact manifolds are sorted by stable body/feature identity and have explicit capacity limits. No duplicate contact can separately claim a fresh impulse.
6. Adaptive iterations: cheap 2-pass reserve up to 4 and then 6, with explicit pending and NO partial island commit on exhaustion. Test residual is provisionally 0.005 m/s, not a general collision-fidelity certification.
7. Stage requests call the original trusted 1.8A.3 adjudicator, verify body position/shape, tick/version and once-per-tick staging before authorizing motor input. This still requires a separate dynamic trajectory CCD and actual transaction commit.
8. Only applied contact impulses from an accepted pair result may enter a condensed-history sample. The world transaction manager must still commit the sample authoritatively; the in-memory ring is not durable long-term history.

## Unweighted competing models
A. Estimate-only kinematic-style contact report — negative control, NO object momentum transfer.
B. Reciprocal linear normal impulse — material two-body response; lacks torque and friction.
C. Reciprocal linear plus off-center angular normal impulse — supports torque; lacks tangential friction.
D. Same with Coulomb-bounded tangent impulse and fixed six solver passes — admissible comparator.
E. Same contact physics as D, with a bounded 2→4→6 adaptive solver — **H1 laboratory exit candidate**.

## Native evidence / observations
- 56/56 named strict-C assertions, including replay, normal/elastic/friction mechanics, model differences, angular momentum, contact order invariance, 3-body adaptive reserve and an eight-body pending island with no partial commit.
- Inherited 1.8A.1 27/27, 1.8A.2 46/46, and 1.8A.3 38/38 + production-negative-controls guard all passed locally.
- 8,000 matched model evaluations of 1,000 shared pair-contact fixtures, 1,000 shared single-contact island fixtures, and 500 additional chain fixtures; 1,500 distinct parameter sets, not 8,000 independent worlds.
- Max tested direct-pair linear momentum residual 0.00024414 kg*m/s; no contact kinetic energy increase in these tested pairs; 0 within-process replay mismatches.
- For the deliberately defined 1,000 simple + 500 three-body scenario mix, fixed six required 8.0 mean contact evaluations and adaptive 4.424: 44.7% fewer *constraint evaluations*, not a measured wall-clock frame improvement. Both correctly accept all in this limited corpus; maximum B-crate-speed disagreement in chains about 0.001245 m/s.
- A deep 8-body chain exceeded reserve and returned explicit PENDING with unchanged original body states.

## Evidential grade and outstanding build gates
This is an **H1 laboratory**, not a complete 3D rigid-body world. Principal-axis inverse inertia is evaluated in the current world frame but orientations are not integrated. A single physical sweep handles upright capsule vs translating axis-aligned box; general multibody CCD, arbitrary-mesh contact, rotating geometry, friction at long-lived resting contacts, restitution thresholds, warm starts, high mass ratios, continuous motor trajectory verification, dynamic stair support, authority-owned geometry provenance, broadphase spatial indexing, full Cell/Nexus substrate, chunk loading, authoritative transaction commit, network replay, durable history and human H4 movement feel remain to be built.

## Reproduction
Run `bash ./v18a4.sh all` in the complete source package. It runs strict C tests and emits `build/v18a4/samples.csv` and `build/v18a4/PAR_RESULTS.md`. `bash ./v18a4.sh regression` runs the historical A.1–A.3 fixture gates plus the A.4 suite. The offline container's global CMake configure attempted to download ENet, which is unavailable in its network sandbox. That is an unverified integration gate—not a passed or a source failure.

## Scientific adjudication
- **Conditional a priori:** G∧T jurisdiction; internal impulses reciprocal; insufficient solve must not claim contact-free traversal.
- **Certain H1 fixture tests:** compiler warnings, explicit invariants, concrete numerical checks and repeatable declared corpus.
- **Highly predictable but defeasible:** adaptive solver saves constraint evaluations with similar accepted state at our tested tolerance; persistence/generalized CCD/human feel not established.
- **Quine:** divide responsibility between witness shape, motor, mass, inertia, timestep, and solver.
- **Kuhn:** the estimate-only controller's lack of a material reaction is an explicit anomaly, not semantics to be relabeled.
- **Lakatos:** preserve authority/conservation while allowing motor gains, restitution, friction and iteration budgets to evolve.
- **Feyerabend:** keep competitor implementations on exactly matched conditions.
- **Popper:** actively challenge the exit candidate with deep constraints, rotating bodies, false supports, low ceiling traversal and malicious client requests.

**Decision: ACCEPT E as the 1.8A.4 H1 experimental exit candidate, NOT as production gameplay physics.**
