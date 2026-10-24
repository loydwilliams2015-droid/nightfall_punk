# H33 RET: authoritative publication boundary — explicit safety hold
H32 exact SHA 2f637fd25ff32d581a91886f6b0efe3088a5e4bb failed strict compilation: POSIX fileno declaration in nf_b6_push18b6.c. H33 applies a portability correction.

## Current code-order witness
1. nf18a5_integrated_pair_step_checked stages A4/A5 candidate, calls A6 prepublish_world_gate, then publishes A5 via publish(s,&next,journal).
2. nf18a6_step subsequently assigns r->physical=next, writes NfActor, crate collider and w->tick.
3. nf18a6_tick_dispatch invokes owner_hooks.finish AFTER nf18a6_step.
4. nf_b6_push18b6.c finish calls nf18b3_realize and appends the B3 receipt; in H25-H32 it independently rereads the unified envelope.

The committed envelope can be recovered, but the physical runtime projection and B3 realization are still different visibility transitions. The H31 outside-writer fault cutpoints stress these transitions; they do not implement isolation or atomicity.

## Mandatory H34 contract
- A6 exclusively controls a compound transaction containing before/after world state, full B2 obligations and B3 receipts, A4 impulses and A5 history.
- Stage and validate ALL consequences (including B3 realization) without side effects.
- Durable commit decision is one journal record; crash recovery reconstructs BOTH domains from that record.
- Live observers cannot read partially applied runtime projections: use an epoch-gated snapshot/view or other proven atomic publish barrier, with reader tests.
- Test every A5 stage/sync/rename boundary and all runtime/B3 publication cutpoints with terminated writer and *fresh* recovery processes; compare full authoritative state, replay refusal and deterministic continued ticking.
- H33 source-exact CI green is necessary but insufficient. Keep production/1.8C.3 HOLD.
