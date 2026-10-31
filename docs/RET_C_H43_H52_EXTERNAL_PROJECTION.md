# RET-C 1.8C.2 H43–H52: bounded external observation programme

**Status:** executable research adapter; not the production multiplayer protocol.
**Parent:** H42 (`b7d80a1112c845a06c842c3b92869b7f0e46780b`).
**Authority:** `NfRetcPublisher`/A6 owner remains authoritative, as in H42.

## Ten cumulative build hypotheses

| Build | Explicit engineering proposition | Evidence |
|---|---|---|
| H43 | Capture owned external telemetry only from one accepted A6/B2/B3 frame | Real dispatcher ticks 1/2 |
| H44 | Reject invalid epochs, identity and nonfinite positions | Malformed-record controls |
| H45 | Require strictly increasing epoch/transaction at observer | Replay rejection and two ticks |
| H46 | Export fixed-length LE V1 record without native ABI padding | Bounded diagnostic encoder |
| H47 | Decode V1 with structural verification | Source-derived roundtrip |
| H48 | Reject damaged, truncated, unsupported and absent records | Negative corruption controls |
| H49 | Reject single-bit corruption across first 88 bytes | Deterministic 88-case sweep per tick |
| H50 | Combine decode + cursor progression without partial success | Duplicate and invalid input |
| H51 | Reject NaN, epoch overflow, zero identifiers and missing accounts | Pathological/boundary checks |
| H52 | Run complete regression suite on GCC and Clang, plus focus | Source-exact GitHub Actions |

## Scope and nonclaims

- External record is an **independently copied** view of a *single*
  authoritative actor/object PUSH and its B2/B3 encounter; it is not the
  whole actor universe or a substitute for `NfWorld` serialisation.
- Codec magic `NFEX`, LE version 1, 92 bytes including a 32-bit noncryptographic
  FNV-style integrity checksum. Byte corruption can be detected but authenticity,
  adversarial tamper resistance and identity are **NOT guaranteed**.
- Do not use incoming diagnostic records as trusted gameplay instructions.
  Network authentication, authorization, encryption and replay authority must
  be provided by a separate production protocol.
- No live `src/server/main.c` networking cutover: it still uses raw-world
  snapshots, and its broader actor, weapon and AI state cannot be reconstructed
  from this single-actor prototype. No camera/prediction migration in this sweep.
- Accepted pointers can outlive later publishes but **cannot** safely outlive
  publisher disposal. Concurrent shutdown requires a separate lifetime gate.
- Production persistent storage format, directory fsync, recovery cutpoints,
  TSAN runtime qualification, all typed B3 operations and global reader
  authority remain open. This is **not** 1.8C.2 closeout.
- VPC-01 remains aesthetic/physics acceptance direction, no graphical claims.

## Evaluation and ML uncertainty

Tests are deterministic correctness/falsification checks, not ML evaluations.
No machine-learning significance level or p-value is established here.
Requests to defer p-values near 1 remain **UNRESOLVED**: any later analysis must
predefine null hypothesis, relevant observations, statistical power, multiple
comparison correction, and whether large p means lack of evidence rather than
positive support for equivalence. No fabricated p-values.

## Holds

Production release, global reader cutover and 1.8C.3 remain HOLD. Next gate:
a real server/prediction reader migration with the *full* required snapshot
semantics, and lifecycle-safe concurrency checking, before claiming adoption.
