# Corrected nightfall!punk 1.8B.6 — Physics Finalization H1, relative to 1.8B.5
PRAXIS / SIGNAL / STEWARD. Date 2026-10-19. Source PR #60, parent PR #59 (draft), ancestor corrected-migration PR #58 (draft). Engineering review only; DO NOT mark full-engine or release acceptance.

## 1. Exact source parents and reconciliation

**Verified 1.8B.5 baseline branch:** `build/v18b5-persistence-h0` at `b5870171f3d2ed6794c349944b3ec2eda70e6219`. **Corrected 1.8B.6 research parent:** `engineering/v18b6-ret-readiness-h0` at `bff20036c54557ba4eefe917e858a14bb27c5c14`. GitHub compare at intake: B6 parent **139 commits ahead, zero behind** B5; not necessarily accepted/promoted or an inherited end-to-end H3 production build. Public default `main` remains six commits off the experimental ancestry, issue #30 OPEN.

Six critical actual code blobs were compared between B5 and this engineering branch:
- `src/shared/nf_world.c`: **identical** Git blob `c686a14089e38856cdb6d6b9e25f5e148ba7f609`.
- `src/shared/nf_embody18a6.c`: **identical** `a5bc971e3141a652ac72f7035c3d3a2451e27e72`.
- `src/shared/nf_contact18a4.c`: **identical** `859a8a51849e0c611696b20b3febb9f9141bbd0e`.
- `src/shared/nf_contact18a5_close.c`: **identical** `219259a330e1426afa0d72a23a3b3bbd59d0fe1b`.
- `src/shared/nf_interact18b3.c`: **identical** `d252386053d25cf668fe5dfe419c17bf295c7d05`.
- `src/shared/nf_persist18b5.c`: B5 `7bafe7c9e18ced414dc40b7904466c8878c7939c`; B6 `1d99168e4492a8cd8fc44f4932de0ee8faf3fff9`. B6 checks replay payload fields and rejects an altered actor/action duplicate instead of silently saying duplicate. This H0 B5 callback remains a mock-publisher contract; do not infer actual multi-domain crash safety.

**Six main-only commits** net three source-of-truth additions: `docs/LEDGER_C34_SEMANTIC_AFFORDANCE_GAME.md`, `docs/SEED_MAP_CONTRACT.md`, `docs/LEDGER_ADDENDUM_C35_C40.md`. Their direct reviewed content gives these reconciliation rules:
- **C34** high-authority arcade intent and non-exclusive smoothed affordance guidance do not override genuine A3/A4/A5 clearance, momentum, or exclusive contract rights. Actual tick effects remain authoritative and may differ from actor intention. This H1 standing/pushing exercise does not implement ladder/vault/mantle.
- **Seed-map atlas** fixed geography versus seed variation is an authoring/world representation contract; it does not grant a new runtime physics owner. Player/private identifying geographic data must not enter seed contents by default.
- **C35–C40** distinguish server truth from legitimate actor knowledge, real-player privacy and gameplay choices from prediction. No client observations are used as an authoritative physical witness in this adapter.
These three documents have **not been merged** from `main` into this research line; the reviewed compatibility outcomes are provisional until issue #30 and release ancestry are resolved.

## 2. Selected bounded engineering change
The **incumbent** is the existing `nf_world_step_checked` → `nf18a6_tick_dispatch` → `nf18a6_step` → `nf18a5_integrated_pair_step_checked` pipeline. It solves actual A4 actor/crate impulses, stages A5 material/contact history, and publishes the physical owner tick once. In H1 this supports exactly one active actor and one dynamic crate. Non-solved actors are refused, not starved behind a falsely accepted tick.

The selected **challenger/adaptor** is `src/shared/nf_b6_push18b6.h/.c`, using the existing `Nf18a6OwnerHooks` (begin, prepublish, finish). `nf18b3_authorize` creates the typed PUSH proposal. Begin rejects stale actor, object, tick, material and contractual epochs. Prepublish checks the staged material target and world tick before A5 publication. **After** genuine A6 commit, the B3 semantic receipt is marked APPLIED only when `actual_impulse` is true, the A5 contact history grows, and the same physical world tick is committed. It never drives a second body or an additional physics tick. Retains a bounded 64-entry in-memory action receipt set and refuses duplicates/overflow.

The external `physical` and `contract` eligibility booleans passed at arming are **trusted-server-call obligations**, not yet provenance-proof B2 authority. To complete full contractual authority, call B2's independently grounded A3/A5 eligibility and server contract owner from this prepublication path and prove same-tick revisions; **this is an OPEN integration gate**. Likewise, the B3 semantic receipt is emitted AFTER physical publication: an intervening process failure can leave physical A5 applied with no durable B3 receipt. This is an explicit H1 compromise, not full B5 atomicity or exactly-once distributed network semantics.

## 3. RET epistemic record
- **A priori:** intention does not imply physical clearance; a semantic proposal does not prove an A4 impulse; server-owner mutation and durable commit are separate events; client bits cannot establish server truth.
- **Specified / checked by C11 fixtures:** one A6 owner, single active actor plus one crate, identity/epoch matching, fail-closed unknown/denied/stale, post-commit only B3 applied semantics and conservation of physical impulse.
- **Empirical executed on original 1.8B.5:** earlier H1 tests and original Git source blobs, not rerun as if newly produced by this commit.
- **Fresh measured H1 for pre-conservation candidate at commit `cc2dcf95f75589399f317bda323e313e3734aca7`:** GitHub Actions `39344050966` strict GCC, Clang, GCC ASan+UBSan all SUCCESS; CTest 11/11 each, integration test 13,391 assertions, 32 paired seeds × 50 checks = 1,600 compared tick frames; five contact impulses and five B3 applied receipts; 13 no-contact/pending observations; 2,048 malformed inputs and 256 stale/forged tick cases. The first 55-tick assumption FAILED because the real A5 owner became PENDING at tick 18; gate corrected to prove unchanged tick, history and B3 on that boundary. This failed run remains in GitHub evidence.
- **New authored conservation corpus after this run:** 2,048 deterministic two-body impulses/momentum/energy checks, to be counted only after its exact workflow passes.
- **Highly predictable but uncertain:** extra hook checks should add little cost compared with full material/physical solves; do not infer speed from tiny process-CPU measures.
- **Unresolved:** full actor multi-scheduling, B2 definitive server contract witnesses, crash-atomic B3+B5 receipts, actual network auth/lag compensation, separate B1 historical source #37, H4 human play and legal #36.

## 4. Paired incumbent/challenger correctness-first comparison
Correctness gate first: every tested **unarmed** candidate tick must match incumbent tick result, world state hash, committed A5 contact history hash and receipt counts for 32 × 50 controlled frames. All passed on the above exact historical H1 test source.

Diagnostic summed `clock()` CPU costs for 1,600 paired calls on each side:
- GCC: bare A6 `0.010948 s`; unarmed B3 hook `0.010815 s`. Candidate approximately 1.2% lower in this specific host process.
- Clang: bare A6 `0.012622 s`; unarmed B3 hook `0.012739 s`. Candidate approximately 0.9% higher.
**Those opposite tiny effects do not support any true throughput, latency or p95/p99 optimization winner.** Workload is deterministic micro-H1 with no real multiplayer networking, graphics, general physics islands or power failure. Retain the simplest correctness-admissible adapter; no unsafe optimization or claim of real frame-rate improvement. Raw loop wall/CPU depends on CI runner and compile flags.

## 5. Safety and remaining quantified limitations
Known bounded observation: five positive actual dynamic contacts, five corresponding B3 semantic receipts, **zero detected fabricated B3 successes**, 256 forged-frame negatives, 2,048 denied/unknown/malformed proposals, 1,600 paired tick decisions. This corpus cannot establish a field failure probability or zero risk outside the fixtures; the single-actor scope is not representative of production thousands-of-actor/multiplayer workload.

Retained holds:
1. **H3/H4 physics**: full A6 scheduling includes legacy combat, AI, contamination, energy and mobile actors, multi-body support, geometry/refinement and all B3 interaction families. Required real 3D experiences and no double owner.
2. **B2 contract**: same-snapshot independent M3/M4 eligibility oracle, historical B1 H1 recovery, freshness and mutable resource ownership.
3. **B5 durability**: physical A5 and B3/contract semantics remain non-atomic across process crash; attach actual durable WAL and recover idempotently before the receipt becomes authoritatively persistent.
4. **Performance**: representative 60 Hz input-to-world-to-client p50/p95/p99, fairness, backlog, load shedding, memory, sustained stress, 337/360 EEC pending issue.
5. **Security/rights**: network session authentication, code/asset software license graft reviewed from `stewardship/2026-10-10-project-reconciliation`, issues #30, #36, #37 and human/cooperative authorization.

**Decision:** A6/A4/A5 physical core **retained byte-identical** to 1.8B.5, additive B3 PUSH semantic observer H1 selected conditionally on strict CI. No H3/full-B6 production acceptance or master-ledger rewrite. Continue on draft research PR; release HOLD.

## 6. Fresh fully expanded H1 execution — source-exact evidence
The adversarial + independently formulated two-body conservation extension **actually executed and passed** at source commit `9c79ab8b5b3d8f00a5da596a3cb042732e932dd8`, GitHub Actions run `39344311869`:
- strict GCC job `118373117864` — SUCCESS;
- strict Clang job `118373117873` — SUCCESS;
- GCC AddressSanitizer / UndefinedBehaviorSanitizer job `118373117680` — SUCCESS.
- Each compiled offline CMake/CTest suite **11/11 PASS**; direct integration executable **21,583 assertion checks PASS**, with 32 × 50 paired A6 comparisons, 2,048 input mutations, 256 forged-frame cases, 2,048 independent A4 momentum/energy checks, five actual coupled impacts and five B3 receipts. The negative/failure-data limit is unchanged: deterministic configured fixture corpus, not a 95% reliability statement for the live game.
- Paired process `clock()` diagnostics from this expanded run (GCC) bare A6 0.016476 s versus unarmed B3 hook 0.016413 s; Clang bare 0.005886 s versus hook 0.005901 s. **No well-supported speed winner**; noise and runner variance far exceed these differences. No actual FPS/game latency or production throughput was measured.

Exact SHA-256 from the GCC CI log at the executed source:
- `src/shared/nf_b6_push18b6.h` `7293b377b8f74e2b897c6f08af2d10402c0c61e8cab82d99945b6230a33bf04a`
- `src/shared/nf_b6_push18b6.c` `a48d38005ca7c1efcc66aae72e4a6c0b4a460d7d2754b043a23351b949b182c8`
- `src/tests/test_v18b6_a6_b3_push.c` `ec1efd75cde04e3e116954103a7c07c666bd9d2125d7d6e27a93027e19acf8dd`
- `CMakeLists.txt` `55f0f19eea4c66c5950130d14a97337ef8a33455dadd011876948dfb11c07f67`

The apparent early failure at tick 18 remains part of the test history: the true A6 owner **PENDING** state leaves the world tick and A5 history unchanged. The initial test incorrectly demanded uninterrupted 55-tick publication and failed; the revised correctness-first assertion accepts a verified pending/no-receipt boundary. Never convert a stalled physical state into fictitious B3 success just to achieve higher measured FPS.

**Outcome:** Within specified one-actor/one-crate H1 scope, incumbent A6 physics is empirically non-regressed, the B3 semantic event is causally tied to real contact, and conservation/adversarial tests pass. **Production physical engine finalization, independent contract-oracle closure, atomic cross-domain B5 recovery, four-actor networking, performance acceptance and release remain HOLD.**
