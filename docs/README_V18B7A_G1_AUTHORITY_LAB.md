# 1.8B.7A Group 1 — Small Authority Transaction Laboratory
**Scope: standalone, bounded C11 reference; NOT an A6 production owner, real A4 physics, A5 material publication, B5 durable WAL or client network server.**

This laboratory implements six simple engineering patterns:
1. **Single writer:** all accepted changes enter through one caller-owned nf7_batch function.
2. **Immutable snapshot:** proposals validate against the original S_t, not other provisional requests.
3. **Copy and commit:** candidate world staged separately; live RAM changes after the persistence callback returns true.
4. **Conflict grouping:** for one exclusive object, one actor wins per tick. Object ID sorts receipt publication independent of input order.
5. **Idempotent receipts:** an identical retry yields DUPLICATE; same action identity with a different payload yields INVALID.
6. **Conservation and explicit uncertainty:** resource debits match recorded receipts; unknown evidence yields PENDING, stale references STALE and exhausted counters/capacity FULL.

## Reproduce from repository root
```sh
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    -Isrc/shared src/shared/nf_authority18b7.c src/tests/test_authority18b7.c \
    -o /tmp/nf7_group1
/tmp/nf7_group1
```
Use clang in place of gcc; or include `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer` for GCC sanitizer execution. GitHub CI: `.github/workflows/v18b7a-g1-authority.yml`.

## Minimal API guide
- `nf7_init(world)`: initialize a fresh laboratory state with epoch and tick = 1.
- `nf7_add_object(world,object)`: fixture/configuration setup ONLY; do not call as a competing runtime owner.
- `nf7_batch(world,requests,count,policy,persist,context,status_out)`: batch one immutable snapshot, stage multiple disjoint approved changes, and publish after a trusted acceptance callback.
- `NF7_LOW_ACTOR`: deterministic lowest-actor selection; `NF7_ROTATING_ACTOR`: deterministic selection starting at the current cursor. These are **ordering policies**, not M3/M4 eligibility.
- `Nf7Status`: COMMITTED only means this local trusted callback accepted the candidate; it **does not assert physical realization or actual disk durability**.
- `Nf7Object.material_identity_epoch`: stable material identity through object revision and owner changes. `generation` guards stale object reuse but this laboratory does not implement destruction/reuse.

## Limits and safety obligations
- Maximum 16 registered objects, 32 requests per batch, 64 retained receipts. Exhaustion is explicit, not silent eviction.
- Caller must hold the one existing A6 owner's lock; callback must not reenter and must satisfy the advertised all-or-nothing side-effect contract. API does not enforce those external obligations.
- Physical and contractual evidence are caller-supplied flags in H0; *never* accept them from untrusted clients in a game. The real adapter must query checked A3/A5/B2 server-side witnesses and revalidate under A6 ownership.
- No atomically durable storage or rollback across physics and ledger exists here. No player-visible applied effects, completed traversal, actual A4 impulses, A5 history, distributed exactly-once transport or H4 human acceptance.
- Do not equate a fast isolated process run with sustained 60 Hz frame time. Treat source-level regressions, CI and reproducibility separately from full-engine promotion.

## Scientific evidence
Five controlled strata: 128 canonical, 2,048 randomized, 512 pathological contested/aliases, 512 boundary exhaustion/identity tests, 2,048 held-out from a separate seed family. Tests include permutations, failure-atomic mock callbacks, receipt conservation and two winner policies. Group 1 evidence and C07–C12 lock docs under `docs/RET_V18B7A_*`. Main user-selected decisions and global release HOLD retained.
