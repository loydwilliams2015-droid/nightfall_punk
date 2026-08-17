# nightfall!punk — v1.5 AI Belief Integration Q724–Q873

Status: **PRE-BUILD WEAK-POINT CONSOLIDATION — NO COMPILE**
Branch: `build/v1.5-combat-ai-communications`

## Result

150 dependency-ordered questions were reviewed.

- 145 automatically resolved, invalidated, or deferred.
- 5 remain open: Q869–Q873.

## Governing distinction

`WORLD TRUTH != ACTOR EVIDENCE != ACTOR BELIEF != ACTOR ACTION`

Belief is server-authoritative actor-epistemic state. It must not become a global blackboard or an ordinary replicated world field.

## Canonical belief loop

`AUTHORITATIVE EVENT`
`-> LEGITIMATE CHANNEL-SPECIFIC EVIDENCE`
`-> ACTOR-LOCAL INGESTION`
`-> MERGE OR COMPETING HYPOTHESIS`
`-> DECAY / UNCERTAINTY / NEGATIVE EVIDENCE`
`-> SELECTED SITUATED BELIEF`
`-> NESTED ACCOUNT PROJECTION`
`-> INTERACTION APPRAISAL`
`-> COMMITMENT`
`-> CONTROL INTENT`
`-> SNAP LOCOMOTION / COMBAT`
`-> PHYSICAL CONSEQUENCE`
`-> NEW EVIDENCE`

## Locked implementation consequences

1. Add a server-side actor-local fixed-capacity belief/hypothesis store keyed by actor ID.
2. Prefer AI/controller-side storage rather than putting full cognition in replicated `NfActor` state.
3. A belief hypothesis carries subject kind/key, estimated state/position, confidence, precision, provenance/channel, age and bounded update history.
4. Preserve 3–4 competing hypotheses per subject as the likely initial cap; measure before expanding.
5. Reports are evidence inputs, not cognition. `nf_report_best()` remains a helper/diagnostic query, not the tactical belief seam.
6. Same-ancestry relays cannot independently increase confidence or precision.
7. Independent direct evidence may strengthen, contradict, merge with, or displace report-derived hypotheses.
8. Refine the current coarse information-channel enum toward the canonical causal channels: DIRECT_VISUAL, AUDIBLE_GUNFIRE, IMPACT, DAMAGE_RECEIVED, TRACE, REPORT, and AUDIBLE_SPEECH where used.
9. Direct visual evidence is fine-grained but does not reveal hidden ammo, energy, intentions, or utility.
10. DAMAGE_RECEIVED guarantees harm, not attacker identity.
11. Anonymous/coarse threats are legitimate when source identity has not been earned.
12. Line-of-sight loss converts exact/fine last-known position into a growing uncertainty/reachable-region envelope.
13. Reachability constrains spatial hypotheses, but navigation does not define epistemic meaning.
14. Negative evidence only counts when a legitimate observation opportunity existed.
15. Newest evidence does not automatically win; channel quality, independence, confidence, precision, age and observation opportunity matter.
16. Memory/hypothesis pruning is deterministic: invalid/expired first, then lowest-value or redundant stale material.
17. Crew/Rival reports do not create common knowledge or copy sender cognition into recipients.
18. Sender confidence is input evidence; recipient belief weight is locally recomputed.
19. Report-bus simple ring overwrite is a placeholder incompatible with the stronger closure contract; later implementation should evict expired first, then lowest-value/oldest redundant reports deterministically.
20. Tactical utility reads actor-local beliefs for uncertain external relations; it must not use exact hidden target state after the belief seam.
21. Uncertainty can raise the value of investigation, vantage, guard, reposition or withdrawal instead of merely lowering attack accuracy.
22. Belief update is event-driven/multi-rate, fixed-capacity and heap-free in the hot path.
23. F4 should expose channel/provenance, competing hypotheses, selected belief, confidence, precision, age, update cause, appraisal and embodied action.
24. Tests must prove rationally wrong belief, same-ancestry non-inflation, independent confirmation, negative-evidence opportunity, uncertainty growth, and divergent actor beliefs/actions.

## Literary / philosophical / game-theoretic interpretation

- **Focalization:** every actor has a bounded horizon; viewpoint is not ontology.
- **Polyphony:** incompatible actor accounts may coexist without global reconciliation.
- **Unreliable narration as engineering metaphor:** an internally coherent actor account may be objectively wrong without being irrational.
- **Situated phenomenology:** tactical meaning comes from the world as legitimately encountered by the actor.
- **Count-as-one:** belief selection provisionally organizes heterogeneous traces without abolishing competing evidence.
- **Incomplete-information games:** best response is conditioned on actor-relative belief, not server omniscience.
- **Signaling:** communication has sender, receiver, channel, cost, latency and interpretation.
- **Value of information:** uncertainty may make information acquisition strategically preferable to attack.

## Main implementation mismatches currently visible

The existing `nf_report` primitive is useful and should be extended rather than replaced: it already contains fixed capacity, stable report/ancestry IDs, origin/reporter, confidence, precision, hops, delivery and expiry. However, it currently lacks an actor-local competing-hypothesis layer, uses a coarser evidence-channel vocabulary than the later Q271 contract, and its capacity overwrite is simpler than the later deterministic value-based eviction contract.

## Acceptance target

Two Human Rivals occupy the same authoritative world state. Rival A receives direct evidence; Rival B receives delayed/coarse social evidence. Sight changes, uncertainty expands, at least one contradiction occurs, and their selected beliefs diverge. Nested appraisal then produces physically different actions. Diagnostics must demonstrate that no hidden exact-target query bypassed the belief layer.

## Open Q869–Q873

- Q869: first-class anonymous/unknown-threat track vs simpler directional-region alert.
- Q870: amount of initial static map knowledge.
- Q871: how high-quality contradictory hypotheses should become visible behavior.
- Q872: belief persistence through STASIS/re-entry.
- Q873: F4 visualization of competing hypotheses.

Vertical Integration Pass II remains deferred until these and the remaining weak-point sweeps are closed.