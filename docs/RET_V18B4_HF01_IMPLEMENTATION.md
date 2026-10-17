# B4 HF-01 ergonomic client presentation — H0 evidence / scope
2026-10-17. New, original standalone C11 source; not taken from copyrighted ergonomic guideline prose, game assets, or CC0-disguised sources.

## Provenance and source handling
CC0 is a copyright waiver tool used by a rightsholder, **not a label for all online ergonomic facts**: https://creativecommons.org/publicdomain/zero/1.0/ . Public domain marking and third-party reuse claims require verification. We adopt abstract functional principles (non-color-only labels, predictable feedback, remapping/toggle provision, explicit uncertainty and voluntary cooperation), not copyrighted source text. This contribution follows existing project's GPL licensing direction; no claim of relicensing it to CC0 or of license compatibility for third-party assets.

## Executed
New src/shared/nf_hf18b4.c/.h and src/tests/test_hf18b4.c authored. Local GCC strict C11 -O2 -Wall -Wextra -Wpedantic -Werror -fsanitize=undefined compiled and printed b4_hf_unit=19 PASS. Tested local source SHA256 C b94c342fea9fd9d0a793afc43f09a67b1607fa7c903f19fdb8cd3ca3eb15804e ; header 696830e7e199be40d674af8be8df79943f5977ac12534b1ee16954f8e1b3d524 ; test 6ab4264379077cafd9d31a6b75e552e158f5bcd9ceb9e47e3780de7f32e627b0 . GitHub-copied source must be byte-verified and freshly CI compiled before equivalence asserted.

## Supported / unsupported
Supported isolated client-display state: local ATTEMPT, authoritative sequence-bound PENDING/CONTESTED/REFUSED/APPLIED/INTERRUPTED feedback, toggle/hold input interpretation, non-color textual labels. Client state never changes server grants, resources or physics; input release must be communicated separately to the actual server by future UI integration. Sequence matches only local outstanding action (not globally unique server identity); out-of-order updates for the *same* sequence need more design. This is not a complete game UI or multiplayer transport layer.

## Acceptance checklist
[unit] distinguish pending/contested/refused/applied, no success before server event, ignore stale seq, accessible hold/toggle behavior, descriptive text labels: local PASS.
[open] actor/object/intent input remapping in actual client, motion/target jitter, keyboard/gamepad ergonomics, feedback in display and audio, explicit consent/carrying control, latency/perception under network loss, observer noninterference in live engine, human usability novices and diverse settings, qualitative anthropological field observations with privacy/consent, accessibility validation. Full human testing deferred until multiplayer demo. No HF-01 human acceptance or 1.8B.4 build promotion.
