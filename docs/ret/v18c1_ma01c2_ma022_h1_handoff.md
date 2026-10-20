# nightfall!punk MA01C-2 / MA02-2 — H1 scientific engineering handoff
PRAXIS / SIGNAL / STEWARD — 2026-10-20. Based on prior draft PR #72 source `9c39919967c5ffe0988397a42ef44b8c97578526`.

## Implemented bounded candidate
- `src/shared/nf_multi_next2.{h,c}`: serial single-owner time-of-impact search for upright, horizontally moving capsule pairs; stage bodies locally, advance up to first earliest impact, apply the actual A4 normal impulse at impact, advance residual time for an isolated **two-body** no-recollision case. If three or more bodies are present, up to eight chronological iterations can be attempted; any initial overlaps or unresolved causal dependencies lead to `PENDING`, never a completed published world. Source states remain unmodified. No claim of verified general multi-impact event scheduling, secondary collisions, box contacts, arbitrary 3D capsules, friction, stable multi-actor body manifold, or physical A5 publication.
- `nf_ma02_live_evaluate`: uses `NfMa02ServerSnapshot` callback to obtain the actual B2 `Nf18b2Scene` and `Nf18b2Use`; checks submitted actor/action correspondence; delegates eligibility to the **existing** `nf18b2_evaluate`, which evaluates B2 object/actor revision, credentials, membership, geometry and real A5 canonical/fine material sweep with PENDING where data absent. Caller MUST supply an authenticated server-owned provider; this callback alone does not verify a provider's provenance nor grant final authority.
- CMake offline headless real source tests and GCC/Clang/ASan/UBSan CI added. No changes to original A6 single-actor owner, B3 receipt generator or B5 authoritative journal.

## Incumbent and challenger
Incumbent: MA01C fixed-time first-contact analytical capsule TOI with A4 constraint; MA02 mock Boolean research eligibility. Challenger: staged contact-time advance (bounded two-body) plus true B2/A5 evaluation via server-supplied scene rather than caller-controlled eligibility flags.

## RET
**A priori:** no B2 grant on absent A5 voxels; one unclosed physics chain cannot be marked fully resolved; full-world publication and disk acknowledgment are distinct operations.
**Formally specified:** chronological isolated two-body impulse and remaining-time operation; compare actor/action identity before B2 calls; PENDING propagation for unsupported dynamics and missing material witnesses.
**Empirically validated (subject to exact successful CI):** five fixture strata ×128 = 640 scenarios; positive two-body contact through residual tick, missed contact, initial overlap PENDING, unsupported vertical motion and distant body; B2 evaluation of actual A5 world with missing resident chunks cannot authorize. The corpus is primarily fixtures, not independent whole-world analytical verification.
**Highly predictable:** real B2 delegation reduces duplicated eligibility logic; TOI-based advancement avoids equating a contact impulse at end of tick with an actual contact-time physical event.
**Uncertain:** chains of 3+ bodies, exact A5 material support, authenticated live provider, complete B2 M3/M4 agreement, contacts with AABBs/other geometries, numeric degeneracy, cross-worker determinism, 60Hz tail latency, lost acknowledgments and hardware crashes.
**Unresolved P0:** live A6 single authoritative owner replacement; authenticated provenance for B2 server data, full actor clearance and support; independent material and physical oracle; joint A5/B3/B5 crash-atomic production publication; C3 portable schema migration; source and rights holds #30/#36/#37.

## Next
Run a rigorously independent oracle for 3-body collision chains and expose a general event scheduler with deterministic future contact recomputation. Integrate A5 and B2 provider records from the actual A6 owner, not a freely set callback, then stage contact history plus B3 outcome into one durable B5 publication. No H3 promotion or release.
