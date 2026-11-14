# RET-C constitutional democratic centralism / engine authority audit — H294 research instrument

**Status: audit / draft, NOT engine-law enforcement and NOT C2 or C3 production acceptance.**
**Source baseline:** H293 PR #246 `5a1fd494ecdf9d30f66bf8e0fcc7e7ba4ad533e1`, 48/48 historical GitHub CI workflows successful; this is a source-exact regression checkpoint, not evidence that this governance constitution exists in the engine.
**Scope:** 50 distinct recommended safeguards with independently auditable acceptance oracles. None is marked implemented by creating this document.

## Constitutional model (proposed, not ratified)
- Assembly defines and amends rights, broad rules and officers through public/recorded democratic procedure and constitutional limits.
- Elected/appointed delegation executes *within* a bounded, revocable mandate; maintainers review code. No founder, executive, or AI agent can waive rights, rewrite passed source evidence, or bypass mandatory safety gates.
- A6 is centralized **runtime technical execution** over authorized actors, NOT an institution entitled to change the constitution or a real-world political authority. B2 authorizes concrete actor actions; A4/A5 and B3 stage physical/contractual consequences; A6 publishes a single accepted immutable frame.
- Appeal, whistleblowing, minority rights and dissent are independent rights, not prohibited by post-decision implementation discipline. Ruleset changes require revision provenance and valid next-tick admission; never silently rewrite the accepted historical frame.
- No code evaluates a contributor's political beliefs or ideological adherence; no mandatory political conformity is a software capability. Refuse discriminatory or coercive applications.

## Cross-engine comparators — avoid false equivalence
1. **Godot governance:** non-profit financial support, open proposals, technical leadership and area owners with merge responsibility; community consultation is sought but merges are not decided by simple popular votes. Source https://godotengine.org/governance/ . Relevance: transparent proposals + bounded maintainers + human review. Not a claim of democratic centralism.
2. **Godot high-level multiplayer:** server-authoritative critical gameplay, client inputs untrusted. Source https://docs.godotengine.org/en/stable/tutorials/networking/high_level_multiplayer.html . Relevance: A6 authority boundary, independent of human governance.
3. **Bevy:** maintainer/subject matter review precedent; its historical RFC repository became archived in June 2026, so do not assume the old repository is a current submission channel. Sources https://github.com/bevyengine/rfcs and https://github.com/bevyengine/bevy . Relevance: technical review quorum and scoped RFCs, not constitution ratification.
4. **Unreal Engine:** gameplay framework distinguishes GameMode, GameState, PlayerState, Controllers/Pawns and replicated state. Source https://dev.epicgames.com/documentation/unreal-engine/gameplay-framework-in-unreal-engine . Relevance: partition gameplay command from published state; not evidence of democratic governance.
5. **Unity Netcode:** server authority provides centralized state/conflict handling; separate distributed ownership model is also supported. Sources https://mp-docs.dl.it.unity3d.com/netcode/2.2.0/terms-concepts/authority/ and https://mp-docs.dl.it.unity3d.com/netcode/current/basics/ownership/ . Relevance: compare deterministic sole-writer and controlled grants, not producer sovereignty.
6. **Review pressure precedent:** Godot's 2026 contribution update explicitly notes AI-generated PR review overload and seeks human accountability. Source https://godotengine.org/article/contribution-policy-2026/ . Relevance: require human source comprehension and accountable reviewer signoff for release-critical work.

## X=50 prospective acceptance determinations
### C01 Constitutional supremacy
- [ ] GOV-01: Record assembly-ratified charter identifier and immutable revision. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-02: Reject execution under an unknown constitutional revision. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-03: Separate charter revisions from ordinary gameplay rulesets. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-04: Require quorum and proposal history for charter amendments. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-05: Provide deterministic rollback to last ratified valid charter. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C02 Member sovereignty
- [ ] GOV-06: Publish equal standing membership eligibility rules. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-07: Provide a discoverable proposal and consultation process. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-08: Record member voting entitlement without exposing secrets. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-09: Enforce bounded executive delegation and expiration. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-10: Test proposal rejection and appeal without retaliation. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C03 Fundamental rights
- [ ] GOV-11: Codify anti-discrimination and privacy as nonoverrideable constraints. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-12: Ensure participation does not require ideological conformity. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-13: Provide documented due-process and appeal mechanisms. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-14: Enforce conflict-of-interest disclosure and recusal. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-15: Make emergency restrictions time-limited and reviewable. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C04 Democratic oversight of maintainers
- [ ] GOV-16: Define appointment/election, terms and recall for delegated officers. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-17: Separate contributor discussion rights from merge credentials. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-18: Require distinct code-author and reviewer for critical changes. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-19: Log dissent and independent technical objections alongside acceptance. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-20: Require explicit human ratification for constitutional changes. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C05 Release and code governance
- [ ] GOV-21: Attach approved rule manifest and responsible reviewers to each release. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-22: Check source-exact compiler and deterministic test provenance. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-23: Block release on missing critical tests without majority override. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-24: Record compatibility and migration decisions as public change logs. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-25: Version effective date and lawful sunset of each ruleset. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C06 A6 execution separation
- [ ] GOV-26: Prove one runtime owner controls accepted immutable tick publication. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-27: Reject gameplay requests that attempt to edit rule or owner authority. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-28: Require explicit source tick and authorized ruleset identifiers. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-29: Keep research/private A4-A5 stages unable to publish. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-30: Reject unauthorized handoff while preserving prior reader snapshot. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C07 B2 eligibility and conflict
- [ ] GOV-31: Bind per-actor grant to owner issued identity, scope, epoch and geometry. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-32: Exercise simultaneous independently authorized actor contention. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-33: Reject stale, revoked, transferred or consumed capabilities. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-34: Test deterministic fairness and starvation metrics. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-35: Log arbitration reason without revealing private player data. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C08 A4/A5/B3 joint transaction
- [ ] GOV-36: Integrate production swept contact and material history. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-37: Validate two actor physical consequences under one source epoch. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-38: Stage two independently typed B3 actions and compensations. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-39: Publish all authorized consequences atomically or none. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-40: Verify independent readback against accepted immutable frame. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C09 Durable continuity
- [ ] GOV-41: Specify portable versioned journal plus independent checkpoint. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-42: Separate journal writer from an independent cold reader. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-43: Recover with a newly created A6 owner and trusted predecessor. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-44: Reject torn, tampered, foreign-revision or replayed envelopes. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-45: Prove no ghost side effects or double-credit across crashes. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

### C10 Independent playable accountability
- [ ] GOV-46: Run GCC/Clang ASan UBSan and independent TSAN source-exact gates. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-47: Run canonical randomized pathological boundary and held-out cases. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-48: Measure native Linux 1080p frame p95 and p99 and input latency. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-49: Complete three independent >=10 minute P1 play sessions. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.
- [ ] GOV-50: Publish release verdict with dissent unresolved risks and human review. **Oracle:** independently inspect source-exact evidence, rejection control, or governance record; retain OPEN unless demonstrated.

## Release litmus
**C2 P\***: AND(single A6 owner, production swept A4/A5, >=2 independently B2-authorized competing actors, B3 typed consequences and compensation, atomic accepted frame, independent TSAN and lifecycle, GCC/Clang sanitizer and held-out suites, representative measured play). Any missing: HOLD.
**C3 P-recovery**: newly created A6 owner restores or refuses full A4/A5/B2/B3 durable transaction using independent checkpoint and versioned journal with no ghost/duplicate effects. Any missing: HOLD.
**P1**: 3 real Linux human sessions >=10 min, third-person gameplay, objective, clean exit, 1080p p95 <=33.3ms/p99 <=50ms, input-visible p95 <=150ms, zero critical faults.
**P2**: >=8 active representative actors, 15–30 minute sessions, p95 <=16.7ms/p99 <=33.3ms, response p95 <=100ms, 3 independent observations.
Targets are proposed until measured. Historical estimate C2 7–14 five-objective sweeps, C3 4–10 possible overlapping; no completion percent.
**Constitutional audit HOLD:** No ratified charter, versioned technical rule manifest, independent appeal evidence or complete production C2/C3 acceptance proved by H293. A 48/48 CI pass cannot substitute for human democratic ratification or production transaction tests.
