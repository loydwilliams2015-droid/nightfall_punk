# H319 physics engineering benchmark — Oxygen Not Included / Red Eclipse / Xonotic

**Assessment scope:** review of source interfaces, not measured game-to-game performance. Historical research releases do not prove the current branch's playable physics. ONI here means Klei Entertainment's **Oxygen Not Included**, not the 2001 action game.

## Three orthogonal benchmarks

1. **Oxygen Not Included — simulation ecology.** Klei publicly describes complex gas, liquid, temperature and power-network simulations. This benchmarks propagation, environmental mass/energy accounting, material causality, deterministic update budgets, and long-lived worlds. Our A5 voxel material queries and history summaries are promising prerequisites, *not* a flowing gas/liquid, thermal conduction or colony-scale simulation. Reference: https://store.steampowered.com/app/457140/Oxygen_Not_Included/
2. **Red Eclipse — locomotion and parkour.** Public movement documentation describes impulse jumps, dashes, slides, wall runs, climbs, momentum chaining and meter limits. Our A3/A4 character capsule, motor, collision sweep and traversal adjudication are lower-level foundations, not evidence of polished parkour with competitive controls. Reference: https://redeclipse.net/docs/gameplay/Parkour-Guide
3. **Xonotic — arena movement and gameplay/physics code integration.** Public guide documents air control, bunny hopping and blaster jumps; player physics is in QuakeC gamecode as of 0.8.5. Our current experimental solvers cannot claim comparable playable motion, prediction quality or online combat feel. References: https://xonotic.org/guide/ and https://xonotic.org/posts/2022/xonotic-0-8-5-release/

## Source-exact baseline

- A4: capsule/AABB swept response, bounded reciprocal impulses, angular/friction policies and up to 8 bodies / 16 constraints via `nf18a4_solve_island`; deterministic ordering by body IDs and feature. No general rigid-body stack or multi-contact geometry manifold claimed.
- A5: 1m canonical grid, selective refinement, ambiguous material queries fail as PENDING, event/history summaries. No fluid/gas flow or temperature diffusion observed in this scope.
- A6: existing runtime tick ownership and source-bound multi-actor A4 staging research, but source-independent 2-actor/1-crate **joint authorized publication** remains OPEN.
- B2/B3: prototype intent and event contracts exist; source-shared contested-object dual grant and typed dual consequences not proven in accepted H319-H322 chain.
- H319 standalone pair laboratory: two independent A4 actor→crate simulations, not a coupled shared crate.
- H319 three-body island laboratory (new): two supplied contact constraints applied to one shared crate, 20 deterministic cases, reversed constraint-order reproduction and duplicate-contact rejection. Synthetic contact geometry and no A6/B2/B3. This improves *coupled solver* evidence but does not discover contacts or represent a live world.
- A6 integration, network playability, rendering/audio and long-term recovery are **not inferred** from the isolated H319 test results.

## Suggested near-term evaluation gates

| Gate | Reference | Measurable requirement | Current assessment |
| --- | --- | --- | --- |
| E1 | shared core | exact-SHA GCC and Clang tests; no undefined/sanitizer findings | Pending exact-head CI |
| E2 | A4 multi-body | real source geometry discovers two impacts on one object; deterministic, conserved impulse within tolerance; no staged-world mutation on failure | OPEN |
| E3 | ONI | 3D material region update and multi-tick propagation with explicitly balanced quantities, bounded memory/time | NOT IMPLEMENTED in inspected scope |
| E4 | Red Eclipse | measured walking, jumping, air control and 3+ chained traversal maneuvers at frame-rate targets | UNVERIFIED |
| E5 | Xonotic | movement consistency, input latency and client/server correction through a two-player playtest | UNVERIFIED |
| E6 | H320–H322 | two independent B2 grants, typed consequences and one immutable A6 joint commit or complete rollback | OPEN |
| E7 | M18C.3 | crash at each commit boundary, restart, exactly-once recovery, no ghost results | OPEN |

A **good hybrid** is not a full imitation of all three titles. Use ONI for environment/resource causality, Red Eclipse for embodied parkour, and Xonotic for precise arena inputs and networking, while distinguishing baseline production MVP from aspirational ecology. A measured 60 Hz full-loop cost and representative simultaneous-actor session are mandatory before public performance claims.

## Verification policy

The new island fixtures do not belong in any official H319 acceptance ledger as authoritative geometry; they are explicit *test controls*. Maintain PR as DRAFT until source-exact compiler and sanitizer results are complete. Do not backfill success from historical SHA workflows.