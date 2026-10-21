# MA03E / MA02-6 research evidence — 2026-10-20
PRAXIS / SIGNAL / STEWARD. Based on draft PR #77 at `0988901fc82672f2230892587ca9d75c745c8156`. H1 only. Production HOLD.

## MA03E result
`nf_ma03e_offaxis` invokes existing A3-derived horizontal capsule-to-capsule CCD, advances to TOI, applies a real A4 reciprocal normal impulse at the verified off-axis contact, and advances an isolated two-body remainder of tick. Five scenario groups ×128 tests cover oblique contact, misses, initial overlap, invalid vertical motion, immutability of input copies. This is **not a general three-body or simultaneous manifold solution**, does not integrate A5 material constraints, and is not the active A6 scheduler. Multi-body collision grouping remains a mandatory gate.

## MA02-6 result and critical blocker
`nf_ma026_prepublish` has the true A6 prepublish callback signature and verifies candidate tick, physical material revision and owner binding. **It cannot currently grant a successful in-progress publication**, because the earlier `nf_ma024_record` calls `nf_ma03_b2_owner_evaluate`, which explicitly refuses `world->tick_in_progress`. A6 sets that field during prepublication. This is an unresolved interface contract and requires an owner-lock-aware internal revalidation path, not a fabricated bypass. The code is not installed as a live hook; its tested scope is fail-closed rejection. No authenticated source-bound positive live prepublish trace was executed.

## RET
A priori: unverified simultaneous contact groups and unevidenced material/contract consequences cannot publish. A false-positive projected hit is not actual world contact.
Formal: narrow TOI-supported two-body off-axis domain; fail-closed incomplete A6 hook.
Empirical: source-exact GCC, Clang and sanitized headless CTests only, with 640 scripted off-axis fixtures. No complete independent general 3D oracle.
Highly predictable: A4 reusable contact impulse supports horizontal oblique normal interaction; solver contact history, TOI ordering and material witness still required.
Uncertain: CCD angle tolerance, simultaneous clusters, A5 support, arbitrary vertical motion, friction, dynamic orientation, 1/2/4/8 workers, throughput and frame latency.
HOLD: positive true A6 prepublication, immutable server B2 credential proof, unified A5/B3/B5 durable transaction, distributed network authority, replay under hardware power loss, source ancestry and legal #30/#36/#37.

## Next engineering task
Implement an explicit, owner-locked B2 snapshot transaction revalidator that is permitted inside A6 prepublish while checking full state and dependencies without duplicate movement authority. Demonstrate positive/denied/stale decisions using genuine runtime state. Do not remove existing A6 rejection guard for arbitrary external calls. Independently test three-body off-axis simultaneous contacts before promoting generalized physical scheduling. No release promotion.
