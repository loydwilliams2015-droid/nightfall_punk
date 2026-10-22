# nightfall!punk 1.8C.1 — MA03F / MA02-7 H1 evidence (2026-10-21)

PRAXIS / SIGNAL / STEWARD. Ancestor: engineering/v18c1-ma03e-ma026-h1, PR #78. Research-only, all production gates HOLD.

## MA03F — executed narrow independent reference

Source: `src/tests/test_multi_ma03f_reference.c`; an independent C11 equal-mass symmetric three-body simultaneous-contact oracle, deliberately NOT an A4 integration. Local command:
```sh
cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer src/tests/test_multi_ma03f_reference.c -lm -o /tmp/ma03f_ref && /tmp/ma03f_ref
```
Executed in local container: PASS 20,000 distinct fixed-seed randomized symmetric cases plus 3 negative controls. Invariants: momentum, kinetic energy, separation velocity order. Earlier incorrect impulse coefficient failed the invariants and was corrected prior to passing rerun. Exact GitHub-committed code was not independently reexecuted after commit; local reference was executed before publishing its corresponding logic.

**Not established:** correct unordered simultaneous collision manifold for unequal masses, angled or vertical contacts, deterministic finite-precision A4 solver integration, A5 history or latency. The existing MA03D chronological A4 scheduler and MA03E off-axis two-body challenger remain distinct; no replacement of active dispatcher.

## MA02-7 — inspected blocker, NOT implemented or pass-tested

Inspected actual `src/shared/nf_embody18a6.c` prepublish flow and `src/shared/nf_multi_ma03.c` evaluator on ancestor branch. `prepublish_world_gate` invokes user hook during an A6 in-progress tick; `nf_ma03_b2_owner_evaluate` rejects `world->tick_in_progress`. Thus the MA02-6 callback cannot succeed using that evaluator; do **not** remove public guard. No positive MA02-7 integration and no B2 authenticated immutable credential proof have been implemented.

Recommended next modification: separate public idle evaluator from internal owner-locked snapshot verifier with a private capability established by the A6 dispatcher. Validate actor/material/world revision, scene identity, action and candidate tick; recompute eligibility on an A6-owned immutable record, check atomic journal coverage, and test allow/deny/stale plus fail closed on provider side effects. Do not let arbitrary caller-provided callbacks grant contract state.

## Governance / acceptance

Physical release HOLD; contract publication HOLD; A5/B3/B5 joint crash-atomic transaction HOLD; disconnected replay/throughput/worker invariance HOLD; ancestry/rights issues #30/#36/#37 remain open. This branch is an evidence/reference change only.
