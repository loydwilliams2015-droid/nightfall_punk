# nightfall!punk 1.8C.1 MA03 — independent 3-body chain and A6-bound MA02 authority
PRAXIS / SIGNAL / STEWARD • 2026-10-20 • draft H1 scientific research, production/release HOLD.

## Source ancestry / implementation
Based on PR #73, head `4ff8cb32b589cf060c4998f26d42e2616d4db6e3`; preserves the corrected 1.8C research branch chain and its original snapshots.

**Independent oracle** `src/shared/nf_multi_ma03.{h,c}`: deterministic one-dimensional equal-mass, perfectly elastic collision-chain reference for **three horizontally separated, equal-radius bodies**. This computes collision times from closure of pairwise gaps and applies a closed-form equal-mass elastic velocity exchange. It does not call the canonical A4 impulse solver, unlike the challenger test. It is an independently formulated limited analytical reference rather than a general 3D continuous physics authority. Eight-event upper bound; explicit incomplete result on invalid or exhausted frontiers.

**Actual A4 comparative test** `src/tests/test_multi_ma03.c`: repeats the same time-ordered three-body events with the repository's real `nf18a4_apply_contact` and checks final velocity, positions, and hit count against the independent closed-form oracle. Five categories ×64 seeds = **320 authored scenarios**, including starting-overlap refusal, no collision, rapid impacts, and distant bodies. This comparison does **not** retrofit `nf_ma01c2_integrate` to solve three bodies: its existing general-three-body pending and completion limitations survive. The comparator is intentionally the established A4 pair solver.

**MA02 source binding**: `nf_ma03_b2_owner_evaluate` takes the **real** `NfWorld` and `Nf18a6Runtime` and refuses any call lacking the actual A6 tick owner/bound context and revision agreement. It checks live actor identity, activity, body pose and capsule profile, and forces a supplied B2 scene's material pointer to be the runtime-owned A5 world before calling the actual `nf18b2_evaluate`. The callback still provides credentials, inventory, membership and object data; it must be trusted and server-owned, and this H1 module does **not** prove credential-provider authenticity or prevent a malicious callback's falsified credentials. This is a substantially tighter A6-state binding, not a genuine full multi-actor A6 publisher. The standalone negative fixture proves rejection of an unbound fabricated provider; no positive authenticated real-provider full-game trace has yet been established by this test.

## RET
- **A priori:** a verified world owner and real material pointer are necessary but not sufficient for granting a contract; independent physical oracles are required for nontrivial solver promotion; pending collision frontiers are not completed.
- **Formally specified:** 1D equal-mass elastic 3-body reference, strict A6 source pointers/revision/pose checks, B2/A5 delegation.
- **Executed empirical evidence:** refer ONLY to exact successfully completed GitHub Actions jobs; do not count broad 3D worlds, hardware-power loss or real network clients.
- **Highly predictable:** the equal-mass elastic chain has analytical velocity-swap predictions under the stated ideal assumptions; actual multi-actor A4 and broadphase source should remain incumbent.
- **Uncertain:** oblique collisions, friction, unequal masses, 3D capsule/capsule, simultaneous multi-contact manifolds, actor count exceeding bounded laboratory size, 1/2/4/8-worker speedup, tail latency, material streaming races.
- **Unresolved P0:** production multi-actor A6 owner, server provider proof, honest B2 credentials and inventory, material + contact history integration into A5, complete B3/B5 durable envelope before authoritative publication, exact field-level portable codec and rights/ancestry issues #30/#36/#37.
- **Production H3 and release HOLD.** Keep PR draft/unmerged, source/rights and original ledger unchanged.

## Next determinations
1. Independently test three-body **unequal mass**, off-axis collision and simultaneous constraints; compare actual **full multi-body scheduling** against reference rather than just same event list plus A4.
2. Move the B2 server scene construction into the owned runtime where possible. Bind provider lifetime/authority and credential provenance explicitly, add a positive real-world actor fixture with denied/pending/eligible conditions, and prevent client-supplied scene substitution.
3. Bring staged A5 material/contact history and B3 receipts into one B5 crash-atomic publication. Strictly distinguish staged/pending/durable.
4. Only then benchmark 60Hz p50/p95/p99 and optional parallel islands under identical seeds, preserving source and release HOLD.
