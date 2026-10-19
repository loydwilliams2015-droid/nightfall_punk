# Group 2 1.8B.7A — seven-family discrete physical interaction laboratory

**Research prototype only.** Does not implement the real A3/A4/A5/A6 continuous physics, production inventory, multi-actor arbitration, disk WAL or networking.

## Build on Pop!_OS / any C11 Linux with GCC or Clang
From repository root:

```sh
cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
   -Isrc/shared src/shared/nf_physical18b7.c src/tests/test_physical18b7.c \
   -o /tmp/nf7_g2 && /tmp/nf7_g2
```

GCC sanitizers: replace `-O2` with `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer` and run with `ASAN_OPTIONS=detect_leaks=0`.

Expected on test fixtures: 128 canonical, 2048 randomized, 512 pathological, 512 boundary, 2048 heldout; total **5248** and **18153 assertions**, **1524** application cases. Do not extrapolate these to production error rates.

## Source organization
- `src/shared/nf_physical18b7.h`: C11 typed discrete world/request/result and mock publishing callback.
- `src/shared/nf_physical18b7.c`: bounded effects for B3's seven kinds; no new A6 owner.
- `src/tests/test_physical18b7.c`: multi-step fixture plus five reproducible seeded strata.
- `docs/RET_V18B7A_G2_F01_F09_HANDOFF.md`: full evidence, RET and release holds.
- `docs/RET_V18B7A_G2_F01_LOCK.md` ... `F09_LOCK.md`: individual feature commitments.
- `.github/workflows/v18b7a-g2-physical.yml`: exact-GitHub source GCC, Clang, sanitizer checks.

## Trusted input boundary
`Nf7fRequest.physical_ok` and `contract_ok` are **mock authoritative inputs**, not remote-client assertions. A real A6 integration must derive these from checked A3/A5/B2 witnesses at publication time; the callback alone does not prove durable write. All outputs labelled APPLIED mean only that the **toy next-state** was accepted by its callback. The 1D momentum update cannot be recorded as a physical A4 impulse or A5 contact.

## Focus
Simple-by-default integer state machines, bounded operations, stale epoch and duplicate identity rejection, no state publication after error, preserved receipts and deterministic repair/impulse accounting. Refinement islands of 0.25–0.5m relative to canonical 1m spatial cells remain a future A5 live-workload requirement, **not part of this model**.

Retain open-source source attribution and H1/H2 historical separation; do not assert rights beyond verified contributors and files. Release HOLD.
