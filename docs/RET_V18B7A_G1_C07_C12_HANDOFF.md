# nightfall!punk 1.8B.7A Group 1 — C07–C12 Combined First-Test Handoff
Dated 2026-10-18. Branch: audit/v18b7a-g1-authority-contracts. Owner: PRAXIS/SIGNAL/STEWARD.
Baseline: Group 1 PR #53 already contained C01 H2 conditional adoption, C02 executable map (tests/CI still not accepted), C03 M4/M3 tripartite research lock, C04 lifecycle lock, C05 simple physical eligibility plus selective fine checks, and C06 relational consistency / simultaneous update priority. No preceding decisions superseded.
## Design pattern portfolio — ranked on suitability, not empirical engineering performance
1. Existing A6 single writer + immutable S_t proposals (C07)
2. staged copy and atomic publication callback (C08)
3. resource-keyed conflict grouping and deterministic ordering (C09)
4. typed idempotent immutable request identity and bounded receipts (C10)
5. command/query separation for client projections (C11)
6. deterministic replay and conservation assertions (C12)
7. relation keys and generation-checked revisions (C06 input dependency)
8. fail closed on unknown, stale, overflow and capacity (C05/C06 support)
Do not infer integrated A6 ownership or B5 disk durability from a standalone C library.

## New source
- src/shared/nf_authority18b7.h: bounded API 16 objects, 32 simultaneous proposals, 64 receipts; two policy modes LOW_ACTOR and ROTATING_ACTOR.
- src/shared/nf_authority18b7.c: validate shared snapshot, reject alias action IDs in same batch, reject contradictory replay, group contenders by target, stage receipts/resources/owner, sort by object ID for permutation-invariant publication, reject overflow, invoke trusted persistence callback before replacing local RAM.
- src/tests/test_authority18b7.c: positive/blocked/pending/stale/replay/invalidation tests, two ordering policies, deterministic permutation, debit conservation and five strata.
- .github/workflows/v18b7a-g1-authority.yml: GCC, Clang and sanitizer CI intended to run exact GitHub source.

## Fresh evidence actually executed locally
Compiled source and test suite with:
- GCC: gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -Isrc/shared src/shared/nf_authority18b7.c src/tests/test_authority18b7.c -o /tmp/nf7 && /tmp/nf7
- Clang same strict flags.
- GCC -O1 -g with -fsanitize=address,undefined -fno-omit-frame-pointer; ASAN_OPTIONS=detect_leaks=0.
ALL THREE locally PASS: **5248 cases, 17556 assertions** each, zero asserted mismatches.
Canonical 128, randomized 2048, pathological 512 with competing claims and duplicate IDs, boundary 512 with revision/tick exhaustion and receipt capacity, heldout 2048 with separate seed. Single-request sampled stratum had 113 positive results across canonical/random/heldout (4+65+44), but pathological/boundary have additional valid commits not included in that tally. Independent status oracle in single-request generator compares explicit simple expected outcomes to implementation and shares the intended specification; not a separate physical truth engine.
CPU time recorded by C clock() only for tiny laboratory process: approximate 0.001–0.005 sec whole run across toolchains; **NOT comparative model speed, 60 Hz performance, or independent actor fairness evidence**.
Local source git blob IDs were checked against GitHub:
- src/shared/nf_authority18b7.c 93c114988ed995866d6a9801207045d5f8a94a84
- src/shared/nf_authority18b7.h f46af7493067a9f32fb0727457bcbbd5b830bc7e
- src/tests/test_authority18b7.c 7f914702cf0a025409d2488e49ed72cd470f4743
These matched actual GitHub fetch_file reported blob IDs, establishing exact-source-byte equivalence between local compiled files and committed sources. Workflow CI status must be checked separately and must not be called PASS from configuration alone.

## Incumbent/challenger
Local **C09 comparator**: low-actor-ID deterministic winner incumbent; rotating-cursor ordering experimental challenger. One controlled example demonstrates different winners while respecting one grant per object and resource conservation. Neither is independently implemented M3/M4 eligibility and **cross-tick fairness not measured**. M4 versus independent M3 remain held under C03.

## Gate assessments
C07: PARTIAL model single-writer API; real A6 tie-in OPEN.
C08: PARTIAL mock persistence-before-RAM; atomic durable disk plus real A4/A5 consequences OPEN.
C09: PARTIAL deterministic local arbitration; cross-tick fairness, complex relations, real actors OPEN.
C10: PARTIAL idempotent local receipts; reconnect, cancellation and compensation OPEN.
C11: SPECIFIED only for live engine; no actual UI/observer noninterference test.
C12: PASS within authored laboratory for determinism/conservation; independent integrated engine truth and fuzz/coverage still OPEN.
No C07–C12 full acceptance or release gate is closed.

## RET register
A priori: status authorization vs execution, and relation identity vs mutable state are conceptually distinct. Formally specified (not proven globally): single-snapshot decision, one winner per target, exactly one receipt per immutable action identity, fail-closed stale or unknown, copy-before-publication. Empirically validated: local bounded lab corpus above. Highly predictable but unmeasured: selective staging/early rejection reduces unsafe classes when integrated. Uncertain: real A6 handoff, reentrant concurrent owners, persistence atomicity, fairness/starvation, network reorder/cancel/compensation, client noninterference. Unresolved: physical/contractual schema A3–A6, full B3–B5 material realizers, independent M3 truth, original B1 source #37, governance/rights #36, ancestry #30, pending EEC 337/360.

## Public commitment/hold
The 1.8B.7A Group 1 source and specification adopt a **small authoritative transaction seam**, preserving one A6 world owner, fail-closed versioned identity and verified local snapshot semantics; finer checks are reserved for material/geometry ambiguity. This is a **tested laboratory increment**, not a production integration or proven optimum. All release/branch licensing/provenance holds remain in place.
