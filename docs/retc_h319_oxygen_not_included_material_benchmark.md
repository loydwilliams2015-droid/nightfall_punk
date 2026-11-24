# H319 materials and physics: Oxygen Not Included reference

**Only external game benchmark currently in scope:** Klei Entertainment's *Oxygen Not Included* (ONI). The objective is to learn from its environmental causality, material inventories, propagation, and time-budget discipline, not reproduce its full simulation in M18C.2.

## Precise source baseline
- Existing A4: finite capsule collision, bounded deterministic rigid-body island impulse solver, contact receipts.
- Existing A5: canonical voxel FREE/SOLID/MIXED, epoch-constrained fine detail, fail-closed PENDING and history/outbox. These are occupancy/material-contact systems, **not ONI gas/liquid/temperature simulation**.
- A6 and B2/B3: an accepted two-actor/shared-object material consequence in one authoritative owner tick has **not** been demonstrated under H319-H322.
- Newly introduced `src/shared/nf_retc_h319_material_ledger.h`: **research-only** integer accounting of one direct transfer between two A5-queried cells. Two conserved quantities: mass units and *abstract thermal inventory*. The latter is **not** a temperature, conduction solver, phase change or specific heat system.
- `src/tests/test_retc_h319_material_ledger.c`: 128 alternating transfers across a single canonical chunk, exact conservation each step, refusal of insufficient and zero transfers, self-transfer, MIXED/PENDING and SOLID; refused transfers preserve ledger values. Explicit no A6/B2/B3 authority.
- Strict C11 GCC/Clang workflow `.github/workflows/retc-h319-material-ledger.yml` compiles against the actual A5 lab library, asserts expected completion text, and does not claim production physics acceptance.

## ONI-oriented development sequence

| Milestone | Scientific target | Acceptance criterion |
| --- | --- | --- |
| O0, current lab | source-linked conservation / gating | Every applied transfer balances; every refusal is non-mutating; actual A5 queries gate both endpoints; GCC and Clang green |
| O1 | deterministic multi-cell transport | same tick order and source snapshot replay to exactly identical outputs, independent conservation oracle, 2+ materials |
| O2 | bounded environmental diffusion | explicit flux law, conservative exchange, stable time step, mixed/solid barriers and no unsafe borrowing |
| O3 | energy/heat semantics | energy inventory separate from temperature; specific heat and capacity, conservation and thermal equilibration tests |
| O4 | full A6 ownership | all material transfers and physical contacts published atomically in one tick or completely rolled back |
| O5 | runtime efficiency | 60 Hz measured under realistic actors/regions, workload scaling, bounded memory and graceful degradation |

O0 remains **pending exact-head CI** until actual GCC/Clang execution. O1-O5 remain OPEN. A positive laboratory result must not be labeled ONI parity, gameplay readiness or H319/M18C.2 release acceptance.

## Guidance
ONI is a simulation-ecology reference, not a requirement to simulate every gas particle or cellular behavior. Prioritize accounting correctness, stable data ownership, explicit propagation, and no partial effects; only then add realism and optimization.
