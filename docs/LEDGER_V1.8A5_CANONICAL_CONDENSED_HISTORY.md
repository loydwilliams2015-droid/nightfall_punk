# nightfall!punk 1.8A.5 — Canonical Grid + Condensed Contact History

## Inherited lock
1.8A.3 Model C remains the H1 capsule/flat-foot/dual-jurisdiction exit candidate, **not** production player physics. 1.8A.4 Model E (motor-driven reciprocal angular+friction impulses with adaptive bounded contacts) is now the **locked H1 predecessor**; no other model is silently substituted.

## Conditional a priori contract
- Material truth is independent of cache residency. `canonical_epoch` changes only upon authoritative canonical material update; `cache_revision` changes upon loading/evicting fine samples. A grid material revision does not change because the cache warmed.
- A canonical 1 m voxel is FREE, SOLID, or MIXED. FREE and SOLID cannot be contradicted by fine evidence. MIXED requires a valid, epoch-matched fine witness (0.5 m or 0.25 m) before an exact FREE can be concluded. Otherwise: explicitly PENDING, or conservatively SOLID under coarse policy.
- Actual space is three-dimensional: each 4 m canonical chunk stores 4×4×4 canonical voxels. 0.5 m supplies 8×8×8 voxels; 0.25 m supplies 16×16×16. Capacity and pinning cannot silently drop an active chunk.
- An actor's intent/semantics cannot author a physical passage. This module supplies material occupancy evidence; the 1.8A.3 geometry AND traversal adjudicator remains separate.
- A contact sample entering the journal must be applied and **world-committed**, carry material/world epoch, and belong to the owner. The history interface is not proof the gameplay world has committed a speculative body impulse.
- Each owner-tick gives the complete contact set. Stable contact-ID order, changes of material epoch and missing contact samples must preserve discontinuity rather than invent support across missing ticks.
- Each sample contributes to a 15-tick window summary. Peak impact, cumulative impulse, contact duration, and integrated resting load are separate observables; long-term consequential events derive from closed summaries, with source range/digest.
- Consequential events enter a bounded outbox. A full outbox refuses further promotion with rollback; no silent overwrite. The POSIX laboratory sink writes event payloads and checksums, `fflush`/`fsync`, then ACKs a contiguous prefix; if power fails after append but before ACK, the same event can be replayed without duplicate append. A torn record causes fail-closed behavior and requires explicit repair/recovery.
- This is **not** an atomic multi-process persistent-world write-ahead log. The current `nf18a5_world_material_commit` stages a canonical material edit plus journal into one local copy-on-write commit, but persistence of the entire dynamic world is still future work.

## Comparative H1 outcome
Five spatial models × 1,000 paired scenario parameters × 12 observations; five history models × 1,000 episodes. 10,000 model evaluations / 60,000 spatial observations. See `build/v18a5/PAR_RESULTS.md` for exact tables and source assumptions.

**Conditional GRID exit candidate: hysteretic selective fine refinement**, with canonical material witness, explicit PENDING and optional pinned hot chunks. In this synthetic workload it gives 9,000/12,000 exact results, zero false FREE, 3,000 explicit PENDING, average 2,794 bytes of resident voxel samples vs eager fine's 4,160 bytes and zero PENDING. It is **not** a strict winner: eager fine offers better responsiveness on this corpus at a data-residency cost, and on-demand escalation must be tested in full gameplay. Coarse-only stays a conservative control; an unsafe cache-miss return-FREE fails hard truth (4,000/12,000 wrong FREE). Unsafe mode is available only with `NF18A5_TEST_CONTROLS`.

**Conditional HISTORY exit candidate: cumulative periodic summaries + threshold promotion + acknowledged durable-event append**, with a full raw-history comparator. 1,000/1,000 episode classifications; 800/800 consequential episodes detected; zero false-positive classifications. Peak-only and isolated-window controls each reach only 400/1,000 correct and miss distributed impulse/duration/load consequences. The overwritten-event control performs acceptably below capacity but fails the explicit full-outbox negative control. History H1 field counts are not yet actual game storage throughput.

## Tests and source provenance
- 83/83 strict-C new fixtures; 9/9 POSIX sink assertions; normal-build unsafe-grid guard; inherited 1.8A.1–1.8A.4 strict-C checks.
- Offline CMake `-DNF_CONTACT_LAB_ONLY=ON` and CTest 3/3 PASS; no ENet or raylib dependence in lab mode.
- ASan/UBSan 83/83; repeat CSV hash matches within the same host/compiler.
- No declaration of full standard CMake, game runtime H3, cross-platform bit identity, network replay, mesh-to-voxel correspondence or human H4 proof.
- The GitHub handoff branch may contain only this ledger until source is explicitly uploaded/merged from the tested local archive. Treat local archive as the actual tested source snapshot.

## Next gates
(1) Physical collider/chunk correspondence and indexed broadphase; (2) true stable support loads from motorized dynamic contacts, not fabricated nominal forces; (3) full authoritative world WAL with crash/recovery and multiwriter; (4) medium/large map streaming, memory/latency profiling, world revision distributed consistency; (5) 1.7E GRAPHICAL contact+chunk+history layers; (6) end-to-end server/client prediction and H4 human play.
