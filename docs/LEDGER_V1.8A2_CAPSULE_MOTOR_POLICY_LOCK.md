# nightfall!punk v1.8A.2 collision-policy lock

**Scope:** The five selected design policies are locked for development. An H1 local-source experiment exists, but the gameplay collision solver has **not** yet been integrated into this GitHub branch. Do not interpret this document as a compiled-production promotion.

1. **Capsule/rounded-box hybrid:** upright capsule in ordinary movement; rounded-box foot/support/ladder contact policy is permitted only when clearance, material relation and affordance authority all validate. Shape changes never authorize physical penetration. Distinguish the static capsule/AABB distance query from the unimplemented full swept hybrid narrowphase.
2. **Motor-driven dynamic:** input supplies velocity targets and bounded motor force/acceleration to a mass-bearing body; collision impulses exchange equal and opposite momentum with other dynamic bodies. Motor work is an explicitly external energy source, not a claim of closed-system energy conservation. Friction, rotation and full dynamic island solve remain challenger work.
3. **Adaptive bounded solve:** initial 2 contact passes, +2 reserve passes as needed, hard max 6, explicit PENDING on exhaustion. The incumbent v1.8A.1 swept AABB resolver remains available for controlled ablation.
4. **Uniform inexpensive canonical grid:** authoritative 1 m semantic/material units; fine data at 0.5 m or 0.25 m is explicitly supplied and loaded only in consequential chunks (provisional physical chunk extent 4 m). Fine-resolution metadata alone does not count as loaded fine data. Physical geometry stays continuous and may be independent of cell resolution.
5. **Condensed contact history:** every *committed* contact sample enters a bounded per-contact accumulator, which closes a periodic summary window every 15 ticks (250 ms at 60 Hz, provisional). Peak, cumulative loading and sampled duration thresholds consolidate summaries into persistent causal event records. Immediate critical-event promotion can be introduced only with explicit commit and provenance. Raw contacts are not all permanently retained; subthreshold summaries may be rolled into cold integrity digests. Event records eventually require durable external append-only persistence and acknowledgment/backpressure.

## Preliminary local native evidence (not yet CI for this branch)
- Strict-C standalone policy tests: 43/43 PASS.
- Inherited v1.8A.1 contact fixtures: 27/27 PASS.
- 1,000 seeded parameter samples, zero within-process replay mismatches and zero numerical/commit failures.
- 86/1,000 static step/edge configurations distinguish capsule from box overlap behavior.
- Maximum observed linear momentum residual in tested normal pair impulses: 0.00024414 kg·m/s.
- All 1,000 sampled adaptive sweeps finished in the cheap pass; reserve performance *not validated by this corpus*.
- Event summarization showed periodic compaction and cumulative/peak promotions in fixture cases.

### Evidence policy
A success in one component does not prove it has been integrated into the real player/world pipeline. H1 components must be followed by compiled full-world simulation, dynamic contact island tests, real chunk loading, true capsule CCD, network replay, hardware timing, and player trials before broader promotions. The local-source package contains the current implementation and an installer; the branch currently records the architecture, not the source graft.

### Scientific gates
No false penetration/grounding, no missing reciprocal impulse in a claimed dynamic collision, no silent drop of solver work or consequential events, no simulated actor omniscience, no false temporal continuity across unsampled contact gaps, and no use of a merely in-memory event ring as evidence of durable long-term storage.
