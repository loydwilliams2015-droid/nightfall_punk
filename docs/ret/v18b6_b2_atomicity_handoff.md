# Corrected nightfall!punk 1.8B.6 — B2/A6/B3 authority hardening and next frontier
PRAXIS / SIGNAL / STEWARD, 2026-10-20. **Review-only H1 engineering**; no release, no historical ledger mutation.

## Verified lineage, baseline and source-of-truth
- 1.8B.5 reference branch `build/v18b5-persistence-h0` @ `b5870171f3d2ed6794c349944b3ec2eda70e6219`.
- Corrected B6 parent: draft physics H1 PR #60 @ `0618dc898e9a94f4f8cf345760f1d83455831e8d`; upstream draft source/rights/RET PR #59, migration #58.
- This branch adds to that exact lineage; original 1.8B.7 experiments retain original filenames, hashes and labels, not relabeled as new B6 results.
- Verified candidate research ledger `docs/evidence/v18b6_master_ledger.json` is inherited unedited; older `docs/LEDGER.md` and `docs/PROJECT_STATE_INDEX.md` retain their historic canonical claims. No permission to promote those records simply from GitHub push access. Legal/ancestry holds #30, #36, #37 persist.

## Implemented B2 authoritative adapter, actual code boundaries
- `src/shared/nf_b6_push18b6.{h,c}` adds `nf18b6_push_require_b2`, a borrowed trusted-server snapshot callback, and `nf18b6_push_arm_b2`. With this option installed, legacy Boolean arming fails closed; no silent fallback from the verified B2 path to unverified booleans.
- At proposal, A6 tick begin and A5 prepublication, checks B2 M4 (or explicit M3) `nf18b2_evaluate` over the **actual live A5 world pointer**, actor feet/height/radius/eye height, static-world A3 geometry (exact bound-to-A6 collider coordinates/count), dynamic-crate handle location, action, entity and material/world revisions, tick and server contract state. B2 independently evaluates material swept clearance, reach, credentials, inventory, occupancy/membership, support, line of sight and preconditions.
- A server snapshot callback must be genuinely server-owned; this module cannot independently authenticate its provider. The caller owns credential/inventory/permission truth. Source design is intentionally NOT an API for remote clients to pass asserted Boolean eligibility.
- B3 `PUSH` semantic receipt still requires an actually committed A6 tick, real A4 reciprocal impulse and growing A5 contact history; does not drive a second force or physics owner.
- This is opt-in single-actor/one-crate H1. Earlier Boolean arming remains for the historic unpromoted H1 laboratory when B2 mode has not been enabled; the new checked pathway disallows it. Source has not been wired into general networked runtime targets; no production security claim.

## Freshly executed scientific evidence
**Exactly tested source commit:** `e60728711bf486bc3b16baea3b7d202642e315c7`. GitHub Actions run `39479304422`: strict GCC, Clang and GCC ASan/UBSan **all SUCCESS**; **11/11 CTest, 22,143 total checks per direct test execution**. The three compiler configurations repeat the same deterministic corpora; do not triple the test sample size.

Proved within authored fixture scope:
- **17 B2-authorized A6 commits** and **five physically realized reciprocal A4/A5 contacts with five B3 applied receipts**; no receipt fabricated from appearance or intention. Occluded/pending outcomes do not increment committed tick or B3 receipt count.
- 256 same-server-snapshot comparisons `NF18B2_M3_EXHAUSTIVE` versus `NF18B2_M4_MATERIAL_FIRST`, both consistent with 512 geometry queries in this positive/credential-denial fixture. They share infrastructure, and this is NOT an independent M3 oracle or full performance winner.
- 2,048 malformed/denied/uncertain inputs; 256 forged/stale tick identity attempts; 2,048 conservation cases in the isolated inelastic A4 two-body contact model.
- 32 paired seeds × 50 owner-tick comparisons between unarmed incumbent and hooked candidate: identical physical world tick, hash, contact history and receipt count on this static dataset.
- The first B2 compile run **failed** due to a variable typo in the new prepublication gate, then a source-corrected new CI was executed. Keep failed run and correction in GitHub history. Earlier A6 invariant initially failed at tick 18 because the real owner became PENDING; this should never be bypassed for throughput.
- No actual full multiplayer, power-cut, 60Hz p95/p99, or synthetic-snapshot-independent workload measurements performed here.

## Rational Engineering Toolbox (RET)
**A priori:** server intent and client visual correspondence do not entail material/contractual authorization; B2 admissibility is a prerequisite for an action, not proof that the action happened; no authorized semantic receipt without committed A4/A5 consequences.
**Specified:** exact snapshot/identity/revision relation; fail-closed unknown/denied/stale; bounded receipts, single existing owner, no physical solve island enlargement through semantic relations.
**Empirical:** exact scoped CI counts above; no claim of 1.8B.5 FPS improvement or broad failure-rate reduction.
**Highly predictable but unmeasured:** reusing B2 and A6 avoids independent shadow authorities; additional validation costs a bounded number of scene/geometry queries but no representative runtime cost measured.
**Unresolved:** independent standalone M3 oracle, actual production contract provider authenticated to server, multi-actor A6 scheduling, application-level B5 crash atomicity, real networking/human/rights, and sustained frame latency under load.

## Atomic A6/B3/B5 publication — fundamental unclosed gap
The current `nf18a5_integrated_pair_step_checked` journals A5 physical state before publishing A6 body; **B3 action ID and contractual receipt are NOT included in that record**. Current adapter marks B3 `APPLIED` after the physical publication. If the process crashes between these steps, the material effect may survive without its B3 record. The H0 B5 mock `nf_b5_apply` and the independent POSIX N01 debit WAL do not fix multi-domain atomicity. Do NOT claim exactly-once semantics, durability or release.

**Required future contract, not implemented:** one A6 owner stages the complete material+contract+action-ID candidate, B2 revalidates same-snapshot read/write witnesses, B3's semantic outcome is derived from the real A4/A5 applied receipt, and one versioned B5 durable envelope is written/synced and replayable **before** public tick/receipt acknowledgement. On crash between any two boundaries, restore either the complete prior state or the complete new state, never half a commit. Recheck duplicate action IDs after recovery, release reservations/cancel with compensating transactions after material commit and enforce source/version/checksum integrity. Audit implementation and portable power-loss behavior separately. Existing A5 journal is a same-ABI test path and must not be silently rewritten.

## Retained open engineering determinations (handoff to this chat)
1. **B6-OPEN01 P0**: bind authenticated server inventory/credentials, B2 geometry ownership and canonical revision source; independently test real (not supplied-by-test) witness origin.
2. **B6-OPEN02 P0**: choose versioned full-world A6/A4/A5/B3/B5 durable envelope and reversible failure semantics with no parallel writer.
3. **B6-OPEN03 P0**: complete crash-fault matrix (before write, torn write, before sync, after sync, missing ACK, reconnect) and prove conservation/exactly-once durable effects; independent oracle.
4. **B6-OPEN04 P0**: extend current one-actor A6 owner to integrated same-tick multi-actor scheduling with actual actor–actor/actor–crate impulses, physical conflict arbitration, no starvation.
5. **B6-OPEN05 P0**: integrate legacy combat, AI, contamination, energy, ladder/step transitions and missing other B3 kinds without creating a second owner.
6. **B6-OPEN06 P1**: representative 60Hz frame/tick/IO/network p50/p95/p99, worst pathological/stress/held-out seeds, long-horizon fairness, backlog and adaptive precision baseline.
7. **B6-OPEN07 P0**: recover original B1 H1 source/erratum and independent M3-v-M4 oracle; resolve source owner and protected branch #30.
8. **B6-OPEN08 P0**: integrate GPL/REUSE audits #36 from stewardship branch into approved production ancestry, review contribution rights, licensing and H4 privacy/accessibility.
9. **B6-OPEN09 P1**: production compilation/linking for checked B2 path, code review of old H1-only Boolean API and safe isolation from client requests.
10. **B6-OPEN10 P1**: end-to-end deterministic identity/version reuse, replay windows, canonical source schema and disaster-recovery rehearsals.

**Selection:** Retain existing A6+A4+A5 physics incumbent and the newly B2-checked H1 semantics; do not optimize by weakening authoritative proof. Those ten open determinations belong to later engineering steps and remain explicitly OPEN. Source/provenance and release **HOLD**. Do not promote beyond the validated single-actor H1 scope.
