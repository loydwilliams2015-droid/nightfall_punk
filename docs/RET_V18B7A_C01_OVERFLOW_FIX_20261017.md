# C01 H2 revision-overflow fix — executed 2026-10-17
Source ancestry: original H2 PR #41 SHA 3fb792eab6e035e1a9736db7dbcf22fc36bac410; patch on Group 1 PR #53 branch.
Problem: nf_b1_commit incremented world_revision and material_revision at UINT32_MAX, wrapping to zero while reporting OK. Corrected source checks both counters after object lookup and before *any* event/body state mutation, returning NF_B1_FULL (bounded version exhaustion) with unchanged world.
## Fresh local execution (corrected source)
- GCC and Clang strict C11 -O2 -Wall -Wextra -Wpedantic -Werror original 6000 fixture+capacity: PASS each.
- GCC and Clang same strict flags separate overflow fixtures at (world MAX,material 1), (world 1,material MAX), (both MAX): 3/3 PASS each, byte-identical pre/post NfB1World.
- GCC -O1 -g ASan/UBSan overflow: 3/3 PASS.
- GCC, Clang strict C11 and GCC ASan/UBSan previously authored 13312-case acceptance suite: 13312 indexed cases per execution; 59 positive commits, 0 authored oracle mismatches; edge print now status=5 (NF_B1_FULL) and both revisions remain UINT32_MAX; acceptance harness source records diagnostic rather than asserting the boundary, separate new overflow test asserts it.
- The tests are dependent runs of same corpus, not independent replicates. No full A3–A6 engine CI, M3/M4, multi-actor or disk/network claim.
Local SHA-256 corrected source 8eba6d8271f60a0bc59861692fcb47b7b924033e21b3121450a7523cf7fde448; overflow-test cbde10f7a0a744290458df5a56898de9359a8a867fc33e86dea23d724c3f7530.
## RET status
Empirically validated: fail-closed wrap guard on declared three boundary cases; exact standalone corpus passes as scoped. Formal narrow reasoning: rejecting either max value before writes prevents these two increments wrapping; not a comprehensive formal model. A priori: revision identity must be reliable under versioned witness semantics. Highly predictable: callers may need to distinguish exhaustion as a terminal refusal rather than retry forever. Unresolved: 32-bit lifetime migration strategy, full contract reconciliation C02, historical original H1 recovery, A3–A6 integration, authorized maintainer acceptance. Return to coordinating Group 1 with no release promotion.
## Disposition
C01 overflow defect PATCHED and freshly tested locally. Option 3 H2 new-source forward-reference adoption remains CONDITIONAL; historical issue #37 and full release HOLD remain OPEN. Do not overwrite H1 historical SHA with corrected H2 SHA.
