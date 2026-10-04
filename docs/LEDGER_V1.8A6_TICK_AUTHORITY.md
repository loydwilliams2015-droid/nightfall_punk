# nightfall!punk 1.8A.6 — Single-authority tick and scoped Purple (H1 addendum)

**Status:** opt-in grounded single-actor headless laboratory, **not** general multiplayer/AI scheduling or production physics. Source reconciled only upon an explicit matching Git commit and CI run; the commit SHA must be recorded once verified. Historical 1.8A model selections remain H1.

## The architectural contradiction and repair

Previously `nf_world_step()` unconditionally ran `nf_movement_step_actor()` for each actor and advanced the world tick, while `nf18a6_step()` independently staged an A5 dynamic/world history transaction and also advanced the world tick. Calling both in the same gameplay frame could grant two authorities over a single actor.

The current `NfWorld` contains an optional `NfWorldTickOwner`. `nf_world_step_checked()` routes a tick to **exactly one** owner; the original legacy path is used only if no owner is installed. A `PENDING`, `REJECTED`, or invalid opt-in result never falls back to legacy movement. The old `nf_world_step()` remains ABI-compatible as a wrapper and exposes the outcome in `world->last_tick_status`. Double binding and recursive dispatch are rejected; direct `nf18a6_step()` while an opt-in callback is bound but idle fails closed. The opt-in A6 bridge rejects any additional active actor, even outside the interaction neighborhood, rather than silently starving their combat/movement/energy advancement. One actor/one dynamic crate is the entire scope of the current bridge.

**Major retained limitation:** The opt-in H1 tick does not yet execute the legacy `nf_combat_step_actor`, contamination and energy lifecycle as part of one atomic physics transaction. This must be integrated before a full production FPS exit; merely routing actor movement does not certify total game-world stepping.

## Red/Blue/Purple diagnostic interpretation

- **Red:** proposed actor motor action, typed `NF17B_DOMAIN_ACTOR` (a domain-specific view, not a new ontological domain).
- **Blue:** material geometry/contact constraint or dynamic response, typed `NF17B_DOMAIN_MATERIAL`.
- **Purple:** only a **proven reciprocal** actor ↔ material dependency on the same material target and tick. It is reconstructed with `nf17c_build_purple_envelopes()` using actual `Nf17cDomainDependency` edges and overlapping nonzero material channels. The envelope is temporary and dissolves after the authoritative tick. A mere camera query, proximity, one-way material veto, or differing targets is **not Purple**.
- The two colors are diagnostic projections over a *seven-domain* 1.7B ontology. Geometry/traversal agreement is independently required by 1.8A.3; Purple does not supersede or shortcut those jurisdictions.
- A successful applied reciprocal impulse is sufficient for the *limited* A6 runtime to mark `PURPLE_COMMITTED`. When canonical data is missing, a returned `PENDING` is **not** automatically labeled Purple because reciprocal causation is not established. Deep unresolved coupled dynamics will require an explicit `PURPLE_PENDING` trace once typed pending reasons and full multi-body island dependencies are available. Never guess Purple or arbitrarily select Red/Blue when coupled constraints cannot converge.
- `nf18a6_classify_tick()` is a pure dependency-graph classifier; its booleans are explicit test inputs. Only the authoritative applied-contact result in the tick owner supplies these booleans for committed runtime Purple. It is not a causal-oracle or security capability.

## Conditional a priori rules

1. Exactly one movement authority per actor tick; no double movement or fallback on pending.
2. A canonical missing material parent is not a free physical path.
3. A failed disk/world transaction cannot be upgraded to history or camera authority.
4. A Purple envelope requires shared target, reciprocal material edges, a nonzero channel intersection, and one tick; otherwise it decomposes.
5. A material contact that is only predicted, or an epistemic observation, cannot be recorded as an **applied** world impulse.

## Native tests and falsifiability

- `test_v18a6_tick_owner.c`: 29 named PASS/FAIL assertions, including missing geometry and pending with unchanged world tick, native `nf_world_step` actual actor movement, direct-bypass rejection, recursion prevention, single-owner binding, rejection of a distant second active actor, and actual committed reciprocal crate impulses → contact history → Purple. Dependency negatives test one-way, cross-target, disjoint channel and next-tick ephemeral envelope.
- `test_v18a6_embodiment.c`: existing 54 fixture assertions for real actor/camera and A5 integration; inherited A1–A5 regressions retained.
- `sample_v18a6_embodiment.c`: 4 policies × 6 scenario strata × 200 seeds = 4,800 paired model rows. **Unchanged** by opt-in scheduler-only addition; its identical SHA-256 is a regression oracle, not new full-world scheduling evidence.
- Confirm GCC, Clang, sanitizer and GitHub CI independently. Distinguish source-verified compile from full ENet/raylib client and server, which remain unproven until built and played.

## Next falsifying gate for production

Run a **joint world scheduler** that performs combat, AI, energy, contamination, player input and material solve as one ordered authoritative transaction, with no remote actors starved. Instrument multi-body physical SCCs independently of semantic/contract propagation; add explicit `PENDING_PURPLE` and physical-support-cascade budget telemetry for real nonconvergence. Validate ladder/jump/crouch, moving supports, map chunks, network reconciliation and H4 playability. Do not overwrite the H1 exit label to claim these have already passed.
