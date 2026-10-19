# Group 3 M01–M11 — Executed CI Evidence and Disposition (2026-10-19)
PR #55, source head before this evidence entry: `d8b9195e7edc0bd898b85682a4ad064aa15f0830` (GitHub Actions run `39324058293`). Baseline PR #54 and upstream PR #53 remain draft / unmerged.

## Actual executed GitHub jobs
- GCC strict C11 `-O2 -Wall -Wextra -Wpedantic -Werror`: job `118305200467` **SUCCESS**.
- Clang same flags: job `118305200871` **SUCCESS**.
- GCC AddressSanitizer+UndefinedBehaviorSanitizer `-O1 -g -fsanitize=address,undefined`: job `118305200957` **SUCCESS**.
- Each job actually ran: **128 canonical, 2048 randomized, 512 pathological, 512 boundary, 2048 held-out = 5248 cases**. Total **46,303 assertion checks** per job; **422 granted proposals, 15,322 non-granted proposals** across those cases. The sample includes multiple requests per case, so grants+non-grants = 15,744, not 5,248. All three compiler jobs reproduced the same counts; they are repeat executions of the *same authored corpus*, not statistically independent populations.
- Measured `clock()` whole-test process numbers varied approximately 0.0016–0.0059 seconds across jobs. **These are not representative multiplayer tick costs or end-to-end latency evidence**.

## Cryptographic exact-source identification from GCC job
- `src/shared/nf_multi18b7.h`: SHA-256 `66876722146d90f3141d8e2dc54edf709b623d92e69f19fcdd2683a4b6f450ca`
- `src/shared/nf_multi18b7.c`: SHA-256 `7bf4a0a58949c5cadf4d7e0a7cdc0dc8cb254c66cd16c3ae9686ff7778ecc5a7`
- `src/tests/test_multi18b7.c`: SHA-256 `1f95b5cbb210897bfb2edc4d59c229e596305fc70ac8f05f89c8b0627c378029`
- `src/shared/nf_hf18b4.c`: SHA-256 `cc6c5bb561a8b7194ed4d843340cd71f6d9212f45e73b7708c42b3cf94523bfd`
- `src/shared/nf_hf18b4.h`: SHA-256 `696830e7e199be40d674af8be8df79943f5977ac12534b1ee16954f8e1b3d524`

## Relevant local-model results, not real-world physics
- M01 snapshot and deterministic batch publication: PASS in standalone mock callback model, real A6 OPEN.
- M02 exclusive and shared capacity: PASS in scoped 1/2-capacity model, real resource/contract integration OPEN.
- M03 rotating fairness: eight alternating claim/release trials PASS, long-term fairness/strategic behavior OPEN.
- M04 actual cooperative push/carry A4/A5 physics: NOT IMPLEMENTED / OPEN; shared scope grants are not forces.
- M05 release tests PASS in bounded model; disconnect/incapacitation and compensation OPEN.
- M06 session, action, alias and stale checks PASS in bounded model; network authentication and reconnect OPEN.
- M07/08 HF-01 read-only sequence feedback regression PASS in harness; real packet/UI OPEN.
- M09/M10 accessible controls and voluntary consent: source contract and consent-bit checks only; real UI/mapping/consent OPEN.
- M11 human H4 usability and anthropological observation: NOT EXECUTED / OPEN.

## Scientific conclusion
The bounded arbitration **H0 code and five-stratum acceptance tests PASS**. This supports the public M01–M11 **design locks** and selection of a minimal actor/session/scope contract under one intended A6 owner. It does not constitute a completed multiplayer engine or an independent physical interaction oracle; under RET, claims of A4 impulse, A5 material consequence, B5 disk durability, full A6 ownership, network security and human acceptance remain uncertain/unresolved.
**Release/promotion: HOLD**, PR draft. C01–C12, F01–F09 and open #30/#36/#37 remain unchanged. No default-branch merge.
