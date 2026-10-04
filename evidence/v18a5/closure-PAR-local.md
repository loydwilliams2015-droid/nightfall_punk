# nightfall!punk 1.8A.5 — Integrated Closure PAR

**Evidence level: H1 laboratory only.** Seven deliberately adversarial categories × 200 perturbations = 1,400 query/step cases. These are not 1,400 randomly sampled real game levels.

| Scenario | Cases | Point-only FREE | Integrated COMMITTED | Integrated BLOCKED | PENDING | STALE | Real crate impulses |
|---|---|---|---|---|---|---|---|
| Missing fine/provider | 200 | 0 | 0 | 0 | 200 | 0 | 0 |
| Loaded clear + dynamic impact | 200 | 200 | 200 | 0 | 0 | 0 | 200 |
| Clear point / obstructed capsule sweep | 200 | 200 | 0 | 200 | 0 | 0 | 0 |
| Malformed fine response | 200 | 0 | 0 | 0 | 200 | 0 | 0 |
| Solid neighbor / clear point | 200 | 200 | 0 | 200 | 0 | 0 | 0 |
| Stale revision | 200 | 200 | 0 | 0 | 0 | 200 | 0 |
| Preloaded fine + dynamic impact | 200 | 200 | 200 | 0 | 0 | 0 | 200 |

## Mandatory interpretation

- Point-only clearance incorrectly authorizes **400/400 known obstructed** cases across scenarios 2 and 4; those cases do not prove a real-world penetration rate.
- The integrated swept-capsule/static-voxel gate blocks **400/400** known obstruction cases.
- A valid resident or successfully loaded geometry witness leads to **400/400** dynamic pair commits with real crate impulse in scenarios 1 and 6.
- Missing or malformed fine data yields **400/400 PENDING**, not false clearance (scenarios 0 and 3).
- Stale revision yields **200/200 STALE** (scenario 5).
- Zero sampled material authority revision changes due solely to fine-cache fills.

## Component regression

- 1.8A.5 closure: 46/46 strict C fixtures, including restored world WAL, outbox failure rollback, actual gravity-normal support impulse provenance and adversarial swept-path clearance.
- Original 1.8A.5: 83/83 component fixtures + 9/9 POSIX sink assertions; A1–A4 inherited regressions run separately.
- Current closure uses a **single-process, same-ABI binary snapshot write-ahead journal** and acknowledged event sink. No claim of portable WAL encoding, multiwriter ordering, cross-machine deterministic replay or operational client/server integration.
- The static voxel sweep is conservative; it can block safe movement and performs a bounded voxel frontier, not a general optimized broadphase or multi-body/rotating collider CCD.
- History promotion retains threshold-triggered cumulative/force-time events; it cannot reconstruct discarded raw time-series detail.
- Exact summary cadence (15 ticks), load thresholds, and pending latency remain provisional.

## Industry correspondence

Box2D provides post-step begin/end/hit events; Rapier exposes contact force events computed after solver impulses and thresholded per contact pair. Our additional rule is that **only committed, physically applied contact receipts** enter an outbox carrying material and actor provenance.

## Exit determination

**YES: close 1.8A.5 at H1 Integrated Physical-World Laboratory level**, conditional on the tested local assumptions. **NO: production player/body/physics/network promotion.** 1.8A.6 inherits the H1 integration adapter for hardware-scale streaming, orientation, multi-contact runtime, server trust and native camera/network proof.
