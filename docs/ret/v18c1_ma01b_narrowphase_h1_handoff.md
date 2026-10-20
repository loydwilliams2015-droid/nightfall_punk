# nightfall!punk 1.8C.1 MA01B — verified collision narrowphase (H1)
PRAXIS / SIGNAL / STEWARD, 2026-10-20. Corrected MA01 source parent PR #70 @ `8bf4b3557c98365b5e82caf82002728336d94701`. **Review-only H1; production HOLD.**

## Scope and implementation
- Added `src/shared/nf_multi_narrow18c1.{h,c}`: consumes canonical swept-AABB broadphase candidate pair IDs, checks actual A3 `nf18a3_capsule_sweep` under relative linear motion, and stages `Nf18a4Constraint` only for the supported translating capsule-vs-axis-aligned moving box body combination. It preserves deterministic candidate ordering and reads the input world without mutation.
- **No unsupported fabricated contacts:** capsule–capsule, box–box, malformed shape and initial-overlap pairs remain `NF_NARROW_PENDING`, with zero published constraint count if any included candidate is unresolved. False-positive broadphase candidates are harmless and produce zero constraints.
- Added source-level physical integration **test**: a high-speed capsule crosses the dynamic box's swept volume, the existing A3 narrowphase reports contact, then the actual A4 island solver applies reciprocal impulses to both bodies. This does not yet place the end-of-tick bodies at TOI under a full multi-body CCD advancement schedule; no claim of real entire-tick material causality.
- Added CMake/CTest test and strict GCC, Clang, ASan/UBSan source-exact CI.
- Existing multi-actor staging kernel (PR #69), broadphase (PR #70), single-actor A6 and physical/contractual journal paths remain unchanged.

## Industry grounding
The design follows the standard *broadphase candidates → narrowphase exact/shape-specific geometry → solver constraints* separation of Box2D and modern real-time rigid-body systems. Box2D collision documentation: https://box2d.org/documentation/md_collision.html ; dynamic AABB tree: https://box2d.org/documentation/group__tree.html . True multi-body continuous collision requires ordered time-of-impact and remaining-time integration; the current A3 routine handles only translating vertical capsules versus AABBs and is NOT a general convex/moving-body collision solver. Full actor–actor capsule CCD and complicated world support are separate research questions.

## RET classification
- **A priori:** candidate broadphase overlap alone cannot certify actual contact; unsupported combinations cannot be silently counted as resolved.
- **Formally specified:** pending semantics, shape eligibility, shared read-only snapshot and bounded deterministic constraints.
- **Empirically validated H1:** five-stratum ×64 = 320 scenarios with positive moving collision, no-contact, unsupported and initial-overlap cases, including A3-derived A4 reciprocal impulse test; full inherited CTest and sanitized compiler validation. The test is a **scripted fixture**, not an independent exhaustive 3D geometric oracle.
- **Highly predictable:** reusing A3 CCD and A4 impulse solver reduces duplication; a real world needs accurate multi-contact TOI ordering and strict dependency resolution, whose performance is not yet measured.
- **Uncertain:** capsule–capsule CCD, box–box contact manifolds, rotating contacts, initial-overlap recovery, real material A5 witnesses, fairness, benchmark 60Hz p95/p99, concurrency and hardware recovery.
- **Unresolved mandatory gates:** live A6 multi-actor tick substitution, full A5 and B2/B3 transaction witness, durable B5 single boundary, authenticated networking, independent whole-engine oracle, source ancestry/licensing/governance.
- **Release status:** HOLD. No canonical ledger modification or accepted production link.

## Source-exact CI and limitation
First build failed under Werror because the code named the nonexistent `min_support_normal_y` instead of the real A2 field `walkable_normal_y`. The source bug was corrected and green strict compiler/sanitizer verification was obtained. Afterward, a positive high-speed A3-to-A4 impulse assertion was added and rerun. Do not infer correctness outside the tested fixture, especially continuous full-tick physical state advancement.
