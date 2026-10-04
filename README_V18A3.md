# nightfall!punk 1.8A.3 — Shape & Traversal H1 laboratory

This package is the tested source tree from the local standalone 1.8A.3 experiment.
It is deliberately NOT a production integration of motor-driven dynamic movement.

## Verify on Pop!_OS

You need a C11 compiler and Python 3. No graphics card or network required for the standalone tests.

```bash
sudo apt-get install -y build-essential python3
bash ./v18a3.sh all
```

This builds, runs and writes:
- `build/v18a3/fixture_results.csv` — 38 named a priori/contact/traversal assertions.
- `build/v18a3/production_gate.log` — independent compile test that negative-control policies are disabled in production.
- `build/v18a3/shape_samples.csv` — 4,000 distinct world/input fixtures under five matched models (20,000 rows).
- `build/v18a3/heldout_support.csv` — 1,200 independent support-edge fixtures under five models (6,000 rows).
- `build/v18a3/sweep_oracle.csv` — 2,400 capsule/AABB sweeps compared with an independently implemented long-double numerical oracle.
- `build/v18a3/PAR_RESULTS.md` — unweighted comparison and H1 disposition.

An archival local CMakeLists registers `nf_contact18a3.c` and its production gate, but a complete engine CMake/CTest run could not be reproduced in this isolated environment because the ENet dependency was not installed and external package downloads were unavailable. The standalone strict-C lab is the validated evidence.

## Reasoning commitments

- `commit_eligible` = valid physical geometry AND valid authoritative traversal contract.
- `commit_eligible` does NOT apply an authoritative world-state mutation.
- Ladder triggers remain separate from solids and do not provide support without a valid landing.
- Real collision shape stays capsule; support acceptance uses an independently validated flat foot patch.
- Server must generate authority snapshot; arbitrary caller-constructed C structs are not authentication.
- Two unsafe comparison models are compiled ONLY under `NF18A3_TEST_CONTROLS`.

## Technical limits

1. The CCD implementation supports upright capsules translated linearly against translating axis-aligned boxes. It does not support arbitrary slopes, rotating geometry, or arbitrary convex-mesh collision.
2. Rounded-box behavior currently means a **support-contact profile**, not a new rotating body collider.
3. A successful stair or ladder traversal still requires the integrated dynamic motor to realize actual motion under physical constraints, with no disguised teleport.
4. The main comparative strata are constructed adversarial fixtures, not an unbiased sample of final gameplay maps.
5. Edge stability is conditional on an explicitly selected 0.10 m support patch and a 0.0001 m geometric tolerance; re-evaluate those choices with later human movement tests.
