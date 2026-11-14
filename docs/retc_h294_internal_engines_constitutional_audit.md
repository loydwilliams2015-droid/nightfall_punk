# RET-C H294 — internal constitutional audit: physics/A6, map, audio, render

**Supersedes closed PR #247** (external-engine audit). This is an internal four-engine cross-contract audit, not an implementation of the remaining integration functions. **No 50 production tests have been executed merely by writing this ledger.**

## Source and evidence hierarchy (2026-11-14)
- GitHub H293 source `5a1fd494ecdf9d30f66bf8e0fcc7e7ba4ad533e1`: 48/48 configured PR workflows successful; private two-body A4/A5 research validation. **Not** two independently B2-authorized committed actors, portable new-owner world recovery or native playable acceptance. Source: https://github.com/loydwilliams2015-droid/nightfall_punk/pull/246
- GitHub source paths explicitly inspected: `src/shared/nf_world.h` (`NF_TICK_RATE 60`), `src/shared/nf_world.c`, `src/client/main.c` raylib client and `CMakeLists.txt`. Expected production standalone `src/shared/nf_map.c`, `src/shared/nf_audio.c`, `src/client/nf_render.c` were **not found at these paths**; this is NOT proof equivalent implementations are absent elsewhere.
- **Map** prior local M0.6K/L/N: 48 cases, 1,152 executions per documented sweep; M0.6N partial-world codec 256 bytes, covers selected actors/colliders/energy/epoch/constitution. Full `NfWorld` and real A6 integration explicitly NOT accepted; M0.6O full same-build world image under investigation; local reported evidence, not verified GitHub source-exact on H293.
- **Audio** prior local M0-RC1/M1 director: synthetic unit/sanitizer evidence; 8 events, 7 local UI dispatches, 1 refused world cue. Genuine A6 publication binding, native audio device and 128-second full-loop human audition NOT established at H293 GitHub SHA.
- **Render** prior local M0.x graybox and later input sweeps: read-only `NfRenderSnapshot`, mock frames and compiled tests, including reported 629 tests in a local later sweep. Native raylib GPU, reliable human input/1080p measurements and full A6 integration not source-exact verified on H293.
- The constitutional approach here means democratically ratified rules; bounded delegation; protected review/appeal and human safety approval; **technical** single-writer A6 runtime authority. Software must never test political loyalty. No ratified constitutional manifest or audited enforcement is established by the research evidence.

## Architecture contract (target)
```
Constitution / rights / release approvals [human, revisioned]
                           |
                 A6 accepted immutable frame
                  /         |         \
        Map read model  Audio event bus  Render snapshot
          spatial        accepted cues     interpolated view
                  \         |         /
            audit / read-only witness and provenance
```
Map, audio, and render may independently schedule their presentation and caches but must not alter accepted simulation history. B2 authorizes actor intents; A4/A5 solves material consequences; B3 realizes typed actions; A6 alone publishes an immutable accepted frame. An uncommitted event may be a local UI cue but never an authoritative world effect.

## X=50 recommendations with proposed executable/human acceptance tests

### 01 Shared constitution and authority
- [ ] INT-01 Record one ratified or explicitly provisional constitution/ruleset ID in every snapshot — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-02 A6 remains sole writer of accepted ticks; specialized engines cannot advance world epoch — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-03 Version and verify delegated capability scopes for map, audio and render readers — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-04 Reject mismatched constitution, tick, epoch and owner-generation in every boundary adapter — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-05 Require independently reviewable charter amendments, dissent and release holds — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 02 Map engine world projection
- [ ] INT-06 Use authoritative NfWorld/A6 state rather than an independently writable map clock — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-07 Validate complete actor/collider/energy topology and canonical material epochs — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-08 Accept only signed/verified source-tick map projection and reject fabricated witnesses — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-09 Explicitly distinguish M0.6N partial 256-byte codec from full NfWorld serialization — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-10 Reconstruct identical map view after a genuinely restarted A6 world owner — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 03 Map streaming and lifecycle
- [ ] INT-11 Reject missing, stale, corrupt or incompatible chunk/fine material data — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-12 Guarantee deterministic cell load/unload without granting phantom free-space clearance — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-13 Demonstrate map generation and world snapshot correspondence at the same epoch — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-14 Test 60 Hz authoritative tick versus delayed map-reader catchup and resynchronization — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-15 Prove spatial map changes cannot silently bypass A4/A5 collision or B2 eligibility — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 04 Audio event authority
- [ ] INT-16 Consume accepted A6 event/receipt stream rather than speculative uncommitted world cues — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-17 Bind event ID, accepted tick and ruleset revision to every world playback request — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-18 Deduplicate reconnect/replay to exactly-once logical cues where prescribed — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-19 Forbid audio device callbacks from modifying A6 state, B2 rights or accepted receipts — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-20 Verify denial of uncommitted world cues while authorized UI events remain local — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 05 Audio reliability and perception
- [ ] INT-21 Prove deterministic event ordering under burst, delay and rewind/replay — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-22 Test audio queue overflow, underrun, focus loss, device reset and callback quiescence — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-23 Carry prior M0 recovery-barrier and persistent-cursor laboratory guarantees into native client — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-24 Measure audio event-to-audible onset and synchronized audio-to-visual skew on target hardware — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-25 Human-check A.2 menu audio, fade/loop continuity and interruption without gameplay lockup — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 06 Render snapshot and authority
- [ ] INT-26 Render from immutable accepted NfWorld/A6 snapshots, never writable simulation internals — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-27 Enforce monotonically valid source tick, interpolation pair and topology/epoch checks — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-28 Reject invalid draw plans without altering the last accepted reader frame — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-29 Prove renderer does not acquire command, arbitration, contract or simulation clock authority — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-30 Retain deterministic camera/object mappings when map topology or actor roster changes — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 07 Render performance and user input
- [ ] INT-31 Integrate real raylib/Linux GPU client rather than solely headless/mock-raylib tests — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-32 Verify third-person controls, focus/recovery, mouse capture and clean exit — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-33 Measure frame p50/p95/p99 at 1080p with both CPU and GPU telemetry — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-34 Measure input-to-visible response p95 and retained frame pacing during map/audio load — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-35 Run three independently observed ten-minute sessions with no critical faults — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 08 Cross-engine atomic encounter
- [ ] INT-36 Execute two genuinely competing independently B2-authorized actors in the same A6 tick — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-37 Resolve production H275F swept contact and correct A5 material consequences — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-38 Stage typed B3 actions and one atomic immutable A6 acceptance or complete denial — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-39 Derive same-tick map projection, authorized audio cue and render snapshot from that acceptance — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-40 Falsify inconsistent partial publication, stale map, ghost sound and rendered ghost interaction — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 09 Durability and disconnected recovery
- [ ] INT-41 Define versioned portable transaction and independent trusted checkpoint for all effects — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-42 Restart new A6 owner from committed A4/A5/B2/B3 state without duplicate or ghost effects — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-43 Restore map projection without independent unauthorized map mutation — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-44 Restart/reconnect audio and render readers from accepted cursor/epoch without replay leakage — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-45 Test kill/restart and corrupted/torn/stale checkpoint rejection with source-exact oracles — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

### 10 Independent verification and ratification
- [ ] INT-46 Require real source-exact full-engine GCC/Clang, ASan, UBSan and independent TSAN — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-47 Run canonical/randomized/pathological/boundary/held-out cross-engine acceptance — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-48 Prove no experimental/local fixture pass is silently counted as production integration — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-49 Obtain authorized human review of ruleset, authority delegation, privacy and rights — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN
- [ ] INT-50 Publish separate C2 P*, C3 recovery, map M0/M1, audio M1, render M0/M1, P1/P2 verdicts — **oracle:** exact-SHA executable evidence or documented human ratification; classify PASS / FAIL / OPEN

## Litmus boundaries (conjunctive, not percentage)
- **C2 P\***: actual production A6 owner, full H275F contact/A5 effects, 2+ independently B2-authorized competing actors, typed B3 effects with denial/compensation, atomic accepted frame, independent TSAN/lifecycle and held-out GCC/Clang/sanitizer regressions, measured representative play. Missing => HOLD.
- **C3 P-recovery**: a freshly started A6 world owner independently reconstructs/rejects complete A4/A5/B2/B3 after crash using trusted checkpoint plus portable versioned journal without ghost or duplicate credits. Missing => HOLD.
- **MAP-M0/M1**: complete real world/admission, deterministic spatial/collision consistency, true recovery and client player-visible map transitions. Partial-world 256-byte codec is not sufficient.
- **AUDIO-M1**: accepted world event-to-audible cue, dedup/restart, device recovery, latency and human listening, with no audio-to-A6 write-back.
- **RENDER-M0/M1**: real raylib/GPU client reading accepted frames only; smooth TPS controls and input capture, measured latency/frame time.
- **P1 playable**: 3x >=10min Linux sessions; third-person controls/objective/exit; 1080p p95 <=33.3ms p99 <=50ms input-to-visible p95 <=150ms no critical faults.
- **P2 alpha**: representative 8 actors, 15-30min sessions, p95 <=16.7ms p99 <=33.3ms input-to-visible p95 <=100ms, >=3 independent observations.
- **Constitutional acceptance**: ratified governance mandate, revisioned executable rulesets, limits on delegation, non-discrimination and privacy, audit/appeal and accountable release votes. Not satisfied merely by A6 being authoritative.
- Existing C2 estimate 7–14 five-substantive implementation sweeps, C3 4–10 possibly overlapping; revised joint map/audio/render integration effort cannot be quantified without authoritative same-SHA pipeline/benchmarks. Do not report a completion percentage or calendar date.

## Priority implementation order
1. Source inventory of map/audio/render local artifacts and their exact compile SHA, surface integration prototypes and ABI drift.
2. Freeze shared accepted-frame contract: tick, revision, constitution, authority owner generation, typed B2/B3, map geometry and audio event IDs.
3. Wire real MAP read model and A4/A5 spatial checks to A6; prove no write-back.
4. Wire AUDIO accepted event bus and replay cursor, with safe UI-local exception.
5. Wire RENDER immutable snapshot and native input/viewer; frame-time benchmark.
6. Exercise all three consumers on a genuinely competing two-actor accepted event.
7. Restart process and owner; reconstruct all read models and event cursors.
8. Run independent full-source acceptance and three human play sessions.
**Keep HOLD/draft until these are directly evidenced.**
