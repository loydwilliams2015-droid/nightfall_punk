# Spiral 09 — read-only five-engine presentation-batch gate

Review-branch candidate. **Not** integrated into the production game executable or A6 publication authority.

The bundle validates five concerns together, with no output mutation until the complete batch is valid:
1. RENDER: actors share a tick/revision/generation and are uniquely identified.
2. ANIMATION: optional paired poses preserve actor identity and bounded movement across ticks.
3. AUDIO: optional accepted-event IDs advance monotonically without duplicates.
4. MAP: optional tile residency fits fixed tile and memory budgets.
5. AUTHORITY: all data is bound to an explicit source digest and monotonic consumer cursor.

It does not authenticate real A6 owner signatures/leases, validate authoritative actor shapes, enqueue device audio, load tiles from disk, or render raylib graphics.

## Reproduce at branch HEAD
```sh
cc -std=c11 -Wall -Wextra -Werror -pedantic -O2 tools/spiral09/test_spiral09_bundle.c -lm -o /tmp/spiral09
/tmp/spiral09
cc -std=c11 -Wall -Wextra -Werror -pedantic -O1 -g -fsanitize=undefined -fno-sanitize-recover=all tools/spiral09/test_spiral09_bundle.c -lm -o /tmp/spiral09_ubsan
/tmp/spiral09_ubsan
```

Local fixture: 44 assertions pass on each binary, including 18 corrupted batches whose failure leaves the output unchanged. Inherited local Python Spiral 08 tree reran 174/174 tests. **Separate suites; do not add them into a fictitious unified count.**

Production gates remain HOLD: real raylib client, source-exact PHYSICS morphology, device playback, tile residency, genuine A6 lease admission and recovery. PR remains draft.
