# nightfall!punk 1.8B.7A Group 3 — multi-actor arbitration H0
This is a **C11 laboratory for prospective interaction grants**. It does not create actual game actors, forces, collider contacts, inventory transfers, GPU screens, network authentication, persistence WAL or an A6 owner.

## Reproduce locally (Pop!_OS or another Linux)
From repository root:

```sh
cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
  -Isrc/shared src/shared/nf_multi18b7.c src/shared/nf_hf18b4.c \
  src/tests/test_multi18b7.c -o /tmp/nf7_g3
/tmp/nf7_g3
```

For GCC sanitizers use `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer` and `ASAN_OPTIONS=detect_leaks=0`.

GitHub workflow: `.github/workflows/v18b7a-g3-multiactor.yml`.
Expected strata: 128 canonical, 2,048 randomized, 512 pathological, 512 boundary and 2,048 held-out = 5,248 cases; these figures describe authored test sizes, not necessarily passing runs until executed.

## API and limits
- `nf_m7_init`: zero and set initial tick/epoch/cursor.
- `nf_m7_add_actor`, `nf_m7_add_slot`: **fixture or A6-owned setup only**, not separate runtime publishers.
- `nf_m7_step`: evaluate a bounded simultaneous request batch under one immutable snapshot, reject stale/unknown/conflicting proposals, choose at most scope capacity, stage copy and ask trusted callback before exposing updates.
- Actor/session/action tuple is idempotency identity. Session checked against server-configured actor, not cryptographically authenticated by this library.
- Fixed low-ID and rotating cursor arbitration are alternative **ordering policies**, NOT M3 and M4 eligibility models.
- All `NF_M7_GRANTED` results are **prospective permission in an isolated model**—no physical completion.
- Maximum 8 actors, 4 scopes, two concurrent users per scope, 32 requests per batch, 64 retained receipts. Capacity exhaustion fails closed.

## Governing constraints
Retain one existing A6 authority under external lock; callback must be nonreentrant and genuinely failure-atomic if used in production. Derive physical and contractual inputs only from real A3/A5/B2 server snapshots, never from client-supplied flags. Consent must be checked authoritatively. Releasing a slot this tick does not produce speculative capacity for a different claim based on the same original snapshot. No physical effects may be claimed until the real A4/A5 solver actually applies them.

## Evidence and holds
`docs/RET_V18B7A_G3_M01_M11_HANDOFF.md` has RET classifications, tests and full release holds; individually owned M01–M11 lock files are in the same directory. Reuse licensed library code only under verified rights; do not presume CC0 for third-party materials. Preserve all upstream B1 H1/H2 provenance, licensing and release holds.
