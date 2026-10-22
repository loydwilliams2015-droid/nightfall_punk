# MA03G / MA02-8 H1 research handoff — 2026-10-21

PRAXIS / SIGNAL / STEWARD. Ancestor PR #79. All production and rights HOLD.

## Executed local evidence
Standalone reference compiled with `cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -fsanitize=undefined -fno-omit-frame-pointer -lm` and run successfully in the current execution environment. 20,000 seeded simultaneous event-arrival cases, 4 collision classification negative/other controls, and 9 owner-state positive/negative controls passed. The committed test at `src/tests/test_ma03g_ma028_gates.c` implements the same checking rules with a slightly shorter harness; **source-exact committed-file CI execution remains unverified**.

## MA03G disposition
The tested classifier detects near-simultaneous arrival pairs and refuses invalid/overlap evidence in a restricted one-dimensional comparison. It is **not** an installed A4 MA03D/MA01C2 solver path and neither proves three-body impulse arbitration nor general off-axis manifold completeness. Promotion requires source-exact integration, independent conservation and order invariants, mixed/unequal mass, contact ambiguity, and 1/2/4/8 workers.

## MA02-8 disposition
A separate owner-state gate prototype checks expected actor, owner, tick, world revision, material epoch, action, and active tick. It is **not a private capability or immutable server-issued B2 record**, so its positive test is NOT a positive real A6 owner-hook publication. Current nf_ma03_b2_owner_evaluate rejects world->tick_in_progress; old MA02-6 adapter cannot authorize a positive live prepublication. Do not remove external safety guard. Next require an A6-private owner-lock verifier with complete B2 scene/material validation, an immutable server-owned record, genuine positive/denied/stale owner-hook tests and an atomic journal binding.

## Release gates
All previously established holds remain: A4 real multibody truth, B2 owner-issued authority, A5/B3/B5 crash atomicity, replay, networking and 60Hz latency, plus issues #30/#36/#37. No release promotion.
