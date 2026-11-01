# RET-C H71 — component operability, interoperability and durability gates

**Source baseline:** H70 accepted SHA `34b8381395fc3aaf039e858939a415d43d4e6d7d`, 32/32 full game and 51/51 GCC/Clang research tests. H71 adds the bounded-history capacity refusal test. Execute full source-exact CI before accepting H71; green on older SHA is not proof for H71.

## Three sequential acceptance layers

### Layer A: component operability — prove local contracts before coupling
- **A4 contact**: reciprocal deterministic impulses, swept high-speed/pathological tests, conservation/residual metrics, unsupported TOI fail closed.
- **A5 material**: revision-exact material witnesses, grid/history provenance and restore consistency.
- **A6 world**: sole tick owner and deterministic complete-or-refuse update; no unauthorized physical-first publication.
- **B2 eligibility**: source-issued rights, revoked/stale/missing material refusal, actual encounter state.
- **B3 physical–contractual consequences**: typed receipts and source-bound PUSH; no invented transfer/repair/carry permissions.
- **Bounded frame history**: `NF_RETC_HISTORY=8` is an explicit capacity. Test that full history refuses additional persist/publish without changing `current` or erasing a pinned frame; implement retention/reclamation only under a proven reader lifetime. An arbitrary larger fixed buffer does **not** close the operational problem.
- **Diagnostic transport**: 92-byte NFEX corruption and replay failure; checksum is not authenticity.
- **Resource performance**: report memory per immutable frame, maximum retained bytes, accepted/rejected tick latency, spikes, throughput and reclamation; define workload and budget before threshold certification.

### Layer B: actual interoperability — source-exact whole engine
- Two genuine independently movable actors and movable objects share the **same real A6 tick**, including actor–actor and actor–material contact arbitration.
- Eligibility from the actual B2 encounter registry and A5 material witness feeds the same staged A4/B3 consequence, without externally visible partial success.
- A single committed immutable frame is consumed by genuine gameplay network, camera, AI, rendering and prediction readers. Source inventory must rule out legacy live-world bypasses.
- Race detector / shutdown and reinitialization tests cover reader/writer, unbind, transport, frame retirement, world destruction and interruption.
- Compare incumbent and challenger under canonical, randomized, pathological, boundary and held-out seeds. The source SHA and parameter set must match the result.

### Layer C: interoperability durability — conditional entry to 1.8C.3
- Replace native in-memory C struct journaling with portable, explicitly versioned canonical serialization with endianness, field count, bounds and migration/compatibility policy.
- Co-commit A4/A5/B2/B3 material and contractual state with a single durable decision and source-backed replay identity.
- Fault-injection at write, flush, fsync, rename, containing-directory fsync and publication boundaries, then independent-process recovery and continued deterministic ticking.
- Install a retention strategy (checkpoint + bounded replay/compaction, or equivalent) with lease-aware reclamation and measured memory/disk overhead; prove that it cannot invalidate readers.
- Acceptance: power-loss survival under specified filesystem/hardware assumptions, no split successes, idempotent replay, bounded leak-free lifetime, predeclared 8-actor baseline and 16-actor exploratory load/latency budgets.

## Current source-backed constraints
1. H70 proved bounded joint-frame identity, not a complete shared two-actor A6 runtime.
2. `NfRetcPublisher.owned[8]` and `count` enforce limited cumulative publications with no general history retirement. H71 explicitly regression-tests refusal.
3. `nf_retc_persist_file` writes `sizeof(NfRetcFrame)` directly, version 1, checksum and rename. There is no canonical cross-ABI wire format or containing-directory synchronization. That journal is a **research prototype**, not final production crash safety.
4. H65's leased pointer stress uses a simulated owner. It does not prove actor/world shutdown lifecycle in production.
5. The H63 Pong derivative demonstrates real A4 impulse in a bounded research scene but not independently validated grinding/wall-riding or the full B2/A5/B3 joint authority.

## Priority and exit state
- **P0 block:** real dual actor A6 authority, exact A5/B2/B3 single-tick publication, source-exact concurrent lifetime, accepted-frame reader migration.
- **P1 block:** contact chains, support/carry/interruption, throughput under representative load, world-level fault-injection / continued replay.
- **P2 hardening:** history capacity exhaustion, journal portability/compatibility documentation and stable schema, failure-matrix completeness, measurement automation. These must eventually close too, not be mislabeled harmless.
- **Current phase:** 1.8C.2 HOLD until layer A and B mandatory gates pass. 1.8C.3 durability implementation may be designed now but **cannot inherit unproved interoperability**.

**Do not call any hardware/software system universally guaranteed.** State the assumptions, invariants, exact test evidence, remaining undecidable hazards and constraints explicitly. No production/public Pong release is authorized by this ledger. The GPL-3.0-or-later software choice and deferred copyright attribution persist.
