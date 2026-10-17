# 1.8B.7 / 7A Group 1 — partial evidence handoff

2026-10-17. Decision: HOLD; C01–C12 remain OPEN / unmapped. No engine fixes or determination closures are claimed.

## Source and scope

Tested source: b5870171f3d2ed6794c349944b3ec2eda70e6219, build/v18b5-persistence-h0, PR #51. Default main is bb377ee8569548eab180778d5bdc9d3c0b16296c and is an older development line. This B5 laboratory branch is the provisional audit basis, not an accepted integrated 7A baseline.

Read B1 H1 handoff, B2 clean reproduction handoff, B3 H0 seam ledger, B4 RET-HK closeout and B5 H0 ledger plus B4 feedback/B5 persistence source and corpus. GitHub PR #47 describes a separate more developed physical B3 line at f2e5fa6ed786115b0ed73ea0138814dea8d40a18; those reported tests were not rerun or merged here. B5's inherited B3 H0 limitations must not erase or subsume that separate evidence.

Exact 7A C01–C12 definitions were not located in current branch files, branch/commit listings, GitHub issue search, retrieved conversation context or Library searches. Older C12 labels are not substituted. The coordinator must supply the exact checklist and designated integration baseline before scoped fixes or closure can be justified.

## Actually executed

GCC 13.3 strict C11, -O1, -Wall -Wextra -Wpedantic -Werror, ASan+UBSan with ASAN_OPTIONS=detect_leaks=0:

| Suite | Result |
|---|---|
| B3 typed H0 unit | 71 assertions PASS |
| B4 HF unit, including terminal-state regressions | 22 checks PASS |
| B5 snapshot unit | 76 checks PASS |
| B5 canonical | 128 cases, 0 mismatches |
| B5 randomized | 1024 cases, 0 mismatches |
| B5 pathological | 256 cases, 0 mismatches |
| B5 boundary | 256 cases, 0 mismatches |
| B5 previously designated held-out | 1024 cases, 0 mismatches |

All four compile and execution exits were zero in this scoped rerun. The previously designated held-out seed is inherited and visible; this is a reproduction, not a fresh independent holdout. No independent challenger was executed; B5 C1/C2 remain unimplemented. No performance winner.

First orchestration attempt aborted on unavailable Clang before saving its results; no pass claim is based on that attempt. The recorded default sanitizer run compiled all four binaries but each exited 1 because LeakSanitizer could not access /proc task data. That failed run is preserved in leaksanitizer-blocked.json. Address/undefined checks subsequently ran with only leak detection disabled. Clang and leak checking remain BLOCKED, not PASS.

Reproduce from repository root: `ASAN_OPTIONS=detect_leaks=0 python3 tools/check_v18b7_group1.py`. JSON records commands, compiler, platform, tested source SHA and exits. The runner records individual outcomes; its own successful exit alone is not a suite gate.

## RET / RET-HK disposition

- A priori within contracts: intent, authorization, material application and notification are distinct; presentation cannot grant authority.
- Formally specified, not formally proved: bounded receipt identities, epochs and callback-before-visible ordering.
- Empirically validated here: only the isolated tests listed above at the stated source SHA.
- Highly predictable: terminal feedback retention suppresses late-message regression within this state machine; end-to-end UX is untested.
- Uncertain: payload collisions sharing actor/action identity, reconnect namespaces, concurrent owners, real storage failures and integrated consequence publication.
- Unresolved: exact C01–C12 mapping, baseline reconciliation, independent challengers, full A6/B3/B4 physical integration, durable recovery, network/fairness, latency and human acceptance.

No archived ledger rewritten; no source relabeled as original B1. Preserve #37 original-source provenance, #36 licensing, #30 main reconciliation, #45 owner/integration and all inherited release holds. No merge, promotion, human acceptance, or cross-chat delivery is claimed.

## Coordinator handoff

Supply C01–C12 text/acceptance gates and identify whether 7A starts from PR #51 laboratory ancestry or requires reconciliation with PR #47 physical lineage. Then map each determination to source, independent oracle, five-stratum cases and named closure evidence. This handoff closes only the narrow local B4 regression rerun gap for GCC ASan/UBSan without leak detection; it does not close B4 integration or any unmapped C determination.
