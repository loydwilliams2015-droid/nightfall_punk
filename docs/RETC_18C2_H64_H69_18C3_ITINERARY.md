# RET-C 1.8C.2 H64–H68 engineering sweep, H69 → tentative 1.8C.3 itinerary

**Planning status:** All H64–H68 changes are committed as evidence candidates, **not promoted on commit alone**. Actual source-exact workflow results, SHA and regressions are the only accepted build evidence. Earlier failures are preserved, not erased. The cumulative green SHA after all changes must be established independently.

## User-facing product direction: defer until public Pong gate
- User played the H63 Pong demo and described it as fun.
- Before a public release, visibly separate **PLAYER** and **OPPONENT** score counters; extend player-side movement to **full 2D WASD court movement** with bounds and real collisions; add **mouse-wheel and +/- zoom**. Source-backed UI acceptance and user approval before any public release.
- These are *documented future requirements*, not merged release features; see [VPC01_PONG_PUBLIC_RELEASE_DEFERRED.md](VPC01_PONG_PUBLIC_RELEASE_DEFERRED.md).
- H63 remains a bounded physical demonstration rather than full 1.8C.2/1.8C.3 gameplay authority.

## 1.8C.2 engineering builds attempted (five stage design)
| Stage | Question / falsifiable claim | Source/evidence change | Exit criterion |
|---|---|---|---|
| **H64** Joint publication eligibility | B2 alone cannot publish if B3/encounter contractual state is missing or contradictory | `nf_c2_retc_frame.c`, source-backed stage/restore/persist/publish regression fixture | No incomplete externally visible frame; prior unchanged |
| **H65** Reader lifetime and coherence | Multiple readers see one coherent accepted frame under atomic pointer handoffs | `test_v18c2_retc_h65_reader_stress.c`, 8 × 6 × 3000 leased-view assertions; normal and lab CTest | Zero mixed epochs and zero acquisition after closure; **simulated publisher** (not live A6) |
| **H66** Diagnostic transport boundary | All 736 possible single-bit modifications of one 92-byte NFEX diagnostic record fail checksum/decoding, with no stale output reuse | Expanded `test_v18c2_retc_real_twotick.c` | Source-bound replay and tamper rejection, not cryptographic authenticity |
| **H67** Whole-frame recovery semantic validity | Even if version, ABI and checksum are valid, forged missing/mismatched encounter obligations are rejected at restart | `test_v18c2_retc_frame.c` checksum-recomputed corrupt full-frame fixtures | No accepted partial material/contractual recovery, no mutation of rejected destination |
| **H68** Bounded epoch/overflow closure | Tick uint32 overflow can never look like a valid next epoch | `nf_c2_retc_frame.c` and epoch-boundary tests | Reject prior UINT32_MAX, epoch 0 and stale publication paths |
| **H69** Governance/handoff | Preserve exact source/CI evidence, open gaps and next programme dependencies | This planning ledger and PR | Do not declare 1.8C.2 final merely because these five bounded checks pass |

### Note about prior-stage failures
The original H64–H65 attempts found (a) an old H42 fixture that explicitly promoted B2-only frames and rejected them only at the reader, now migrated to a stronger prepublication refusal, and (b) Clang C11 `_Atomic` pointer aggregate initialization incompatibility, now corrected. The exact old workflow run failures are retained in GitHub Actions. Cumulative reruns determine actual eligibility.

## Still-open 1.8C.2 gates not closed by these five builds
1. **Real A6 two-actor authority:** both genuinely movable actors and material contacts participate in the same A6-owned tick with deterministic arbitration. Bounded Pong is not source-accepted A6 integration.
2. **Whole material realization:** A4 impulse, exact A5 voxel/material witness and history, B2 authority, B3 typed receipt and encounter ledger commit together; no independent physical-first publication or unsupported contractual side effect.
3. **Complex contacts / wall riding:** simultaneous multi-contact manifolds, actor–object pushing and carrying, vertical support/transition, high-velocity grinding / tangential conservation under moving support, and fairness under contention.
4. **Concurrency:** prove record/world/transaction lifetime safety under real actor readers and shutdown. H65 pins a bounded immutable read frame but does not establish global source-owned reader migration, network/gameplay frame serialization, or writer concurrency.
5. **Durability:** local ABI frame journal uses temp-file fsync and rename but is NOT a portable, power-loss-safe whole-world commit (missing directory sync protocol, decision log integration, semantic repair rules and actual runtime restart).
6. **Workload and usability:** source-exact GCC/Clang/ASan/TSan as applicable, representative frame times, memory footprint, responsiveness under load, Linux package distribution, independent observations, final licencing/provenance review.

**1.8C.2 decision:** treat these as *hardening candidates*. Do not auto-promote to 1.8C.3 or production merely on passing subset tests. Explicitly return to 1.8C.2 where prerequisite gaps remain.

## Tentative 1.8C.3 programme — durability, recovery and whole-engine acceptance

| Phase | Core hypothesis | Implement / measure | Promotion gate |
|---|---|---|---|
| **C3-01 — Authoritative transaction map** | Every side effect has exactly one A6 commit point | Inventory real A6 writers/readers, A4/A5/B2/B3 mutation routes; full event-source graph and fail-closed interfaces | No unmapped publication bypass |
| **C3-02 — Unified immutable decision** | Both material and contractual consequences share a unique durable commit identity | Versioned canonical transaction envelope with actor/object IDs, epoch, B2 grant, A5 material hashes, typed B3 receipt | Strict schema validation; all-or-nothing decision |
| **C3-03 — Crash/power-loss fault matrix** | Interruption at any write/sync/rename/publication boundary cannot create split success | Worker kill at every boundary, fsync staging and containing directory, restart with no in-memory state | Recover complete commit or reject incomplete attempt; no torn A4/A5/B3 |
| **C3-04 — Deterministic replay** | Accepted checkpoint and continued ticking reproduce exact world and contract state | Independent fresh-process recovery, randomized/held-out replay, exactly-once receipt identity and idempotence | Replay protection, monotonic epochs, material and obligation equivalence |
| **C3-05 — Multi-actor and disconnection** | Two actors contend or disconnect without dual authority or starvation | Simultaneous contacts and B2 eligibility revocation, partitions/rejoin, timeout/duplicate/out-of-order messages | Invariants under contention and recovery; fairness measured |
| **C3-06 — Reader cutover and shutdown** | Readers see only committed, immutable, source-complete epochs | Move real camera/AI/render/prediction/transport reads under leased accepted frame; concurrent unbind/close | TSan/Helgrind-style race checks, reclamation and shutdown source-exact passing |
| **C3-07 — Performance and operating envelope** | Durable authority remains useful at representative frame times | 8-actor baseline, 16-actor stretch, large-world cells, disk latency, load/soak and graceful decay | Predeclared latency/throughput limits; failure classified not hidden |
| **C3-08 — Independent G1–G8 exit** | The entire live engine satisfies G1–G8 with independently checkable evidence | Real player client/server, source-exact CI + replay provenance, external test/owner acceptance, retirement of obsolete authority paths | Every mandatory gate PASS; any unsupported case HOLD; release owner decision |

### RET-C acceptance and handoff protocol
- Each build records: exact source SHA, compiler options, source of inputs, deterministic/random/pathological/boundary/held-out samples, numeric outcome, artifacts, failed/open gates and estimated operational uncertainty.
- Incumbent is the last *fully source-accepted* configuration. A challenger is never selected solely by passing new tests while inherited gates fail.
- Human observation uses open prompts and captures provenance separately from instrumented physics; do not assign p-values to a handful of qualitative observations.
- Return to 1.8C.2 for missing material-transaction or actor-owned authority; do not move a failure into 1.8C.3 by reclassifying it.
- GPL-3.0-or-later is the chosen software license; copyright-holder attribution remains provisional pending authorship/provenance viability review.
