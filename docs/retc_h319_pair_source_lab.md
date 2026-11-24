# H319 A4/A5 real-source pair laboratory — isolated review addendum

**Scope:** strict C11 invocation of production `nf18a4_pair_step`, `nf18a4_pair_to_sample`, `nf18a5_grid_init`, `nf18a5_load_canonical`, and `nf18a5_query` through the real `nightfall_contact_lab` library, for actor IDs 10 and 20 against crate ID 2010.

Twenty off-owner geometry configurations (two independent actor→crate pair calls each), recording genuine solver result status, swept hit, receipt, impulse, contact ID, TOI, tick/version; no fabricated positive proof. Test asserts observed applied contact sample source identities when contact is accepted. Separate canonical material controls exercise FREE, MIXED/PENDING, SOLID and INVALID with the actual source A5 library.

**Limit:** Each actor is tested against an independent copy of one crate. This is *not* simultaneous coupled-object integration in one authoritative A6 world, nor an independent CCD oracle or B2 live grant. Counterexamples or absence of positive contact are recorded rather than forged. Running this code cannot satisfy H319 full acceptance, H320, H321, H322 or M18C.2. The Python H319 evidence-schema gate remains separate. Follow-up required: a real A6-controlled three-entity scene, genuine two actor→object impacts from one source tick, both complete A5 swept paths FREE, verified visibility and a held-out CCD oracle.

**CI command:** the `retc-h319-pair-lab.yml` workflow builds the real `nightfall_contact_lab` static library via CMake, compiles this test with GCC/Clang `-std=c11 -Wall -Wextra -Wpedantic -Werror`, executes it at the exact checked-out SHA. Treat any pending/red workflow as not accepted. The original H55 sanitizer workflows also have failures on PR #275 and require separate triage; do not report whole-branch green.

**Release:** PR remains DRAFT/HOLD. Neither this test nor its artifacts are a production B2 issuer, B3 effect, immutable A6 publication, WAL/recovery or playability certificate.
