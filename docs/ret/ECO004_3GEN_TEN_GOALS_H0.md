# ECO-004 / 3GEN ten-goal isolated engineering checkpoint

Status: **isolated headless candidate tested; no production AI source modification or A6 integration**.

This research branch starts from the latest inspected 1.8C.1 MA03H branch commit `b8af036883b36499b5ae080e21676e292c4e7646`.

## Local candidate

The source package accompanying the November 19 engineering conversation contains `nf_energy_c01.{c,h}`, `nf_threegen_lab.{c,h}`, `test_threegen_lab.c`, `run_tests.sh`, SHA-256 manifest and execution logs. The executable source files are **not yet committed in this branch**; retrieve the reviewed package from the conversation before attempting integration.

## Executed isolated results

Strict C11 build with `-Wall -Wextra -Wpedantic -Werror`, ASan+UBSan and an optimized release build: **12/12 PASS** with identical test outputs. Ten checks: fixed 60 Hz simulation clock; 100-E C01 ledger initialization; wall-occluded perception; hidden resource invariance; actor-local observations; energy-cost movement; single atomic harvest posting; cultivation affecting delayed regeneration; delayed lower-confidence reports; and fail-closed unimplemented combat. Additional checks: replay hash and journal capacity failure.

## Required before source-exact acceptance

- Commit and review the exact source package as an opt-in test target.
- Re-run native tests against this source-exact branch and existing CMake/CTest suite.
- Bind ecological mutations to A6/B2 owner-issued authority and transaction-wide publication.
- Make regeneration's external-input and resource transfer atomic on one world transaction.
- Add true tactical combat, pathfinding oracle, communication authenticity, actor access control, population-scale endurance, and durable recovery.

Do not interpret the local isolated tests as certified X-COM, StarCraft, or Thief AI behavior, or as evidence of full-engine correctness.
