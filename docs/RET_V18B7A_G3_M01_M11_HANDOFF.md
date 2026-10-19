# RET 1.8B.7A Group 3 — M01–M11 Multi-Actor and Client H0 Handoff
Date: 2026-10-19. Baseline: `build/v18b7a-g2-physical-interactions-h0` (PR #54, head `fef7593c7c16ad8ee1231dddfdf64c21421a4951` at intake); Group 1 PR #53 remains its unmerged ancestor. This is **not** an actual multiplayer A6 or A4 physics implementation. Maintain global release HOLD.

## Experimental question
Can a minimal, deterministic, copy-before-publication multi-actor arbiter safely distinguish exclusive from shared prospective grants, honor actor/session/action identity and voluntary consent, refuse stale or unknown inputs, and remain logically replayable while preserving a strictly nonauthoritative client view?

## Selected design patterns
1. **One writer, many proposals (M01):** S_t immutable throughout eligibility and arbitration; only existing A6 owner would be permitted to publish.
2. **Typed capacity and explicit relational identity (M02/M06):** each resource scope has stable ID and revision, capacity of one or two in H0; the actor has authenticated-by-caller session and unique action ID.
3. **Deterministic arbitration and rotating priority challenger (M03):** compare fixed actor order with cursor-based rotation; do not claim cross-tick fairness merely from a few favorable rounds.
4. **Cooperation does not generate force (M04/M10):** shared grants are permission, never applied A4 impulses or actual B3 carry dynamics; consent is required.
5. **Safe release and interruption boundary (M05):** only a current holder may release; actor disconnect, session rotation and compensation require a real server adapter.
6. **Read-only, monotonic presentation (M07–M09):** reuse HF-01, ignore late contradictory messages after applied, preserve non-color labels; no actual client integration or usability evidence.
7. **Human evaluation with consent (M11):** defer participant observation until real multiplayer H4; do not invent users, accessibility results or telemetry.

## Implemented code and actual scope
- `src/shared/nf_multi18b7.h` / `.c`: up to 8 actors, 4 scopes, capacity 1–2 each, 32 intents per batch, 64 retained receipts. New C11 standalone owner-model entry point `nf_m7_step`, trusted-snapshot input flags, typed actor/session ID, slot revision/epoch checks, fail-closed unknown and absence of consent, key-based replay and altered-payload rejection, same-tick duplicate/alias rejection, scope arbitration, immutable baseline capacity, stable sorted publication receipt ordering, no publication on failed mock persistence callback, capacity and UINT32_MAX guards.
- `src/tests/test_multi18b7.c`: true/exclusive vs shared capacity, speculative same-tick release/claim rejected, duplicate/conflicting replay, unknown/denied/stale/session mismatch, mock publication failure, replay under input permutations, eight alternating claim/release rounds using rotating priority, and existing HF-01 sequence-bound nonauthoritative feedback. Five strata included with a deliberately simple *separately written expected-status evaluator* for initial empty-capacity worlds, plus pathological and boundary properties. This oracle shares the declared domain assumptions, and is NOT an independent physics engine.
- `.github/workflows/v18b7a-g3-multiactor.yml`: GCC, Clang, GCC ASan/UBSan, exact source SHA logging for review.
- `docs/RET_V18B7A_G3_M01_LOCK.md` through `M11_LOCK.md`: individually assigned commitments, selected mechanisms, evidence boundaries and remaining gates.

## RET classification and test integrity
A priori: eligibility, prospective slot grant, applied impulse, durably published resource change and client view are different types of events.
Formal/defined (no whole-engine proof): unique action per actor+session, epoch/revision freshness, simultaneous same-actor/slot alias exclusion, at most scope capacity grants based on S_t, no speculative reuse of freed capacity, deterministic tie-break, bounded retained receipts, copy-before-publish.
Empirical: only actual compiler and GitHub CI run outcomes recorded after execution. Test vector counts alone are **not** proof until workflows complete.
Highly predictable: rotation may reduce fixed-ID starvation under symmetric repeated contention; not general or guaranteed fairness.
Uncertain: algorithm fairness over real actor schedules, latency and throughput, shared inventory aliases, held-out domain representativeness, multi-scope conflicts.
Unresolved: actual A6 world-owner hookup, real B3 push/carry impulse cooperation, A5 material effects and history, B5 crash-atomic receipts/replay, actor authentication, reconnect cancellation and cleanup, human interface/H4 results, historical source issue #37, governance #36, ancestry #30.

## Mandatory acceptance
1. **SOURCE:** pin/verify commit and exact file hashes, strict Linux GCC/Clang/ASan/UBSan execution and full engine CI.
2. **CORRECTNESS:** independently grounded eligibility/physical truth, zero unsafe positive grants or physical force claims under unknown/stale states; conservation, isolation and unique scoped receipts.
3. **MULTI-USER:** multiple physical actors in one actual A6 tick, valid shared/exclusive object ownership, current actor consent and authenticated session.
4. **TEMPORAL:** cross-tick fairness, disconnect/interrupt and delayed packets, replay and compensation with actual authoritative B5 durable storage.
5. **PHYSICS:** material support, real A4 reciprocal forces and A5 contact history, actual carry constraints and cooperatively applied actions.
6. **PERFORMANCE:** real tick p50/p95/p99, backlog/starvation and input-to-render metrics under representative game worlds.
7. **HUMANS/RIGHTS:** user privacy/consent, accessibility/device testing, provenance and maintainer review.

## Decision
**M01–M11 design choices LOCKED on a public research branch.** H0 standalone arbiter and client unit tests are admissible only within their measured scope; no B4/Group 3 full promotion or merge. Keep the preceding C/F-series locks and release holds unchanged.
