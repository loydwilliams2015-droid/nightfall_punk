# 1.8B.7A Group 4 — executed CI evidence, exact provenance and hold
Date 2026-10-19, GitHub PR #56. **Tested exact commit** `89a945c18f8b5ab0782b426d0c6a1a385abb126a`, workflow **39326789518**.

## Real CI results (not projected)
- GCC strict C11 job **118314600375**: SUCCESS.
- Clang strict C11 job **118314600815**: SUCCESS.
- GCC ASan/UBSan job **118314601062**: SUCCESS.
- Every job independently executed the same deterministic authored corpus: **704 real-file N01 recovery cases** (64 canonical, 256 randomized, 64 pathological, 64 boundary, 256 held-out) with **6,150 assertion checks** and **557 applied journal operations in the generated five-stratum corpus**. Separate targeted tests exercise 64-record saturation, replay on process restart, same-action duplicate/mutated identity, unknown outcome after injected write-before-fdatasync, torn suffix, one corrupted record, quarantine, stale epochs, insufficient debit balance, and no RAM success on uncertainty.
- Every job also executed **10,000 synthetic governor metric inputs**, **39,056 assertions**. Scoped tested invariants: deterministic admission, zero optional work when unhealthy/unknown/critical backlog, journal uncertainty moves to read-only, four healthy ticks needed for optional workload admission, optional bounded allocation <=250µs *as a granted budget*, **not actually timed execution**.
- **Combined:** 45,206 assertion checks per CI configuration; test runs reuse the same corpora so do NOT count three compilers as three independent populations.
- No reliable 60Hz physics/net/client latency measured. The optional budget value is a policy limit, not evidence of a scheduler meeting it.

## Exact SHA-256 sources from GCC job
- `src/shared/nf_wal18b7.h` f5a82661ea60bdb627554cd4426b04195e63c97c66d9354625a1001aa7e5288b
- `src/shared/nf_wal18b7.c` a37a6526852f1ce05ba32d5c142d795bc3afdd21cfaa669104817b46ed0148d0
- `src/tests/test_wal18b7.c` a11e445ab8a85048ace9c24de61e7dd34e5db77d1191b5fff46cb9ffa6969239
- `src/shared/nf_governor18b7.h` db333943831c37a53a0838d1db373ed9cc9d8b10b424460a96e44560893283d7
- `src/shared/nf_governor18b7.c` cb2a84d752d2a910de509294124a93e1fbd92225c71fb26cfbe0709951313281
- `src/tests/test_governor18b7.c` a684c59ee192f4995d1dda61573bdea0713277beea274265044fb9ec3f6b44b2

## Admissible claims
**N01 PARTIAL PASS**: bounded single-process POSIX WAL write-sync-replay semantics at the tested host and process interruption/corruption level. It is **not** certified hardware power-loss durable, full B5 world transaction safe, or actual physical A6 commit.
**N09 N10 N11 P08 PARTIAL TEST-MODEL SUPPORT**: quarantine after occluded outcome, graceful refusal and low-priority budget gating are implemented/tested in separate H0 reference components.
**N02–N12 / P01–P14 full-engine acceptance OPEN**, including snapshots/compaction, session authentication, packet loss/reorder, real A4/A5 effects, actual background work, independent durable oracles and frame-tail benchmarking.
The group-level *architecture/policy* decisions N01–N12 P01–P14 are LOCKED for research only. No proof of benchmark-optimal policy or M3/M4 algorithm comparison.
**Release, legal/provenance, maintainer merge gates: HOLD.** Historical H1 #37, legal #36, ancestry #30 and upstream PR #53/#54/#55 unchanged.
