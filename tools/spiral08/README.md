# Spiral 08 — MAP/RENDER consumer handoff (review candidate)

This is a **separate offline C11 research fixture**, not a modification of the active NIGHTFALL!PUNK raylib client. It ports five conservative checks from the local Spiral 08 experiments into a small, reviewable, heap-free C interface:

1. RENDER — same-frame, unique-actor batch.
2. ANIMATION — same-actor, same-generation bounded temporal motion.
3. AUDIO — strictly increasing event IDs and fixed cue budget; no playback.
4. MAP — checked tile memory and count budgets; no asynchronous streaming.
5. AUTHORITY — monotonic read cursors bound to a supplied digest; **not** A6 owner-issued lease validation.

The tests prove only these contracts. They do not grant B2 eligibility, enact B3 effects, commit A6 world frames, read authoritative PHYSICS shapes, verify real GIS provenance, or certify the production renderer. The synthetic MAP-D02 and standalone raster artifacts remain local until reviewed separately.

## Reproduce

From this directory:

```sh
cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic test_spiral08_consumer.c -lm -o /tmp/spiral08_consumer_test
/tmp/spiral08_consumer_test
cc -std=c11 -O1 -g -Wall -Wextra -Werror -pedantic -fsanitize=undefined -fno-sanitize-recover=all test_spiral08_consumer.c -lm -o /tmp/spiral08_consumer_ubsan
/tmp/spiral08_consumer_ubsan
```

## Evidence / provenance

The independent local staging suite reported **174/174 PASS** (164 inherited, 10 added). Its native MAP-D02 raster was 1024×640 and repeated byte-identically between normal and UBSan compilations. That local evidence does **not** validate this GitHub commit until the exact committed source is compiled in CI.

Required production gates: source-exact raylib linkage and capture; source-exact actor geometry; real audio device; tile residency/load recovery; real A6 accepted-frame/lease integration. All remain **OPEN**. Profile A remains the preferred gameplay-readability baseline.

No modification to mainline client or production A6/PHYSICS code is proposed in this PR.
