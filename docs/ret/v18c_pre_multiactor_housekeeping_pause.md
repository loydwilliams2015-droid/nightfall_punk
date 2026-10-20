# UCW pre–multi-actor housekeeping and stop checkpoint

PRAXIS / SIGNAL / STEWARD. Date 2026-10-20. Research increment following 1.8C.3 draft PR #67. **Pause after this checkpoint; the next session starts the originally planned 1.8C.1 Multi-Actor World Authority.** This numbering is a return to the functional roadmap, not a rollback of 1.8C.2/1.8C.3 research.

## Changes
- New `src/shared/nf_ucw_codec18c.{h,c}`: portable 32-bit big-endian TLV header (mechanic ID, schema version, mandatory/optional flag, payload length) plus a bounded 16-codec registry and caller-supplied codec functions. Reject ID collision, unknown mandatory types, versions, duplicates, incomplete required mechanics, impossible sizes, unknown flag bits and truncated records. Unknown optional sections may be skipped.
- Metadata is preflighted for all sections before invoking decode callbacks. **Limitation:** codec callbacks own their data and may mutate it; if a later callback fails, rollback is not guaranteed. A truly atomic caller must decode into disposable/staged state and publish only on full success.
- New `src/tests/test_ucw_codec18c.c`: five case strata × 256 = 1,280 tests for identity/roundtrip/version/error handling and malformed required records, with strict C and sanitizer CI.
- CMake headless lab integrates the codec and inherited A6/B2/B3/A5 plus UCW C1–C3 regression suite. The source is *a separate codec kernel*. Neither live A6 nor current C3 raw A5 snapshot has been migrated to it. Actual universal self-serialization remains OPEN.

## Architecture and compatibility
- **Incumbent:** typed C3 envelope stores real `Nf18a5Integrated` as an ABI-local native snapshot and the bounded UCW state together in one frame; no live A6 owner integration.
- **Challenger:** a reusable versioned per-mechanic codec registry, capable of producing portable mechanic payloads when each mechanic provides a verified schema-specific encoder/decoder; no schema migration yet.
- Separate A5 and UCW journal writers must NOT be simultaneously used as a supposed full-world atomic transaction.
- No direct code changes to physics, contracts, network, UI or accepted ledgers. No production release, no permission to assume CC0 extends to GPL code or third-party materials.

## RET classification and mandatory gates
**A priori:** stable per-field wire encoding is necessary for cross-ABI persistence; private mechanics must not publish partially decoded authoritative state.
**Formally specified:** ID/version/flags/length and bounded registry semantics.
**Empirical:** record successful exact-source GitHub CI only, with the previous failure history preserved.
**Highly predictable:** future mechanic schemas can reuse TLV without modifying the outer section framing, but encoding every real A5 material/physics field will require substantial implementation and independent cross-platform tests.
**Unresolved:** transactional decode staging for arbitrary callbacks; portable complete A5 and B3 codecs; field migrations; unknown mandatory mechanics at reload; independent crash oracle and hardware tests; single live A6 publication gate; source/licensing #30/#36/#37; representative 60Hz p95/p99; real multi-actor physics. **H3 and release HOLD.**

## Next-session handoff — the originally scheduled 1.8C.1
Start **Multi-Actor World Authority**, not another unbounded journal feature series. Preserve the one A6 authoritative world and same-tick snapshot. First implement the deterministic multi-actor proposal/read-write conflict grouping without extra physics owner, then stage real actor–actor/actor–object impulses and fairness testing. Require canonical/random/pathological/boundary/heldout with independent authority and conservation oracles, 1/2/4/8-worker invariance where applicable; do not claim full multiplayer while incomplete A5+B3+B5 atomic publication and C3 portable serialization remain OPEN. Preserve the existing replay test corpus.

## Stop
This note is the agreed engineering pause and handoff. No future execution is scheduled and no further changes are implied without a new user message.
