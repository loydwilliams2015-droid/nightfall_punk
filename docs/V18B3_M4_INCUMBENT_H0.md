# 1.8B.3 M4 + EEC + typed realizers — H0 implementation submission
Status: **new source candidate, NOT a finished engine build**, no B3 acceptance or promotion.
Base: research/v18b3-ret-build-start, anchored at user-specified fadae2b5a88c1d989febc39a83ab9fc8ec020559.

Files src/shared/nf_interact18b3.h and .c implement a bounded, new-source typed encounter seam for DOOR, SWITCH, PICKUP, PUSH, CARRY, REPAIR and INTERRUPT. They accept authorization predicates, enforce world/material/contract epochs, block duplicate publication within an event, distinguish pending versus applied, and retain applied history on interruption.

They intentionally **do not** simulate doors, pickups or dynamic constraints. The injected applied receipt callback is an integration obligation: real A6-owned A3/A4/A5 physical publication must be supplied and demonstrated independently. A successful callback alone cannot certify correct physical dynamics, durable WAL or network recovery. No source from the lost 1.8B.1 archive is claimed.

Unit test src/tests/test_interact18b3.c covers all seven kinds, stale-epoch denial, pending callback, single applied receipt, duplicate refusal and interruption. These are authored unit fixtures and not B3 real-world physics acceptance. M3 remains the required challenger, M4 the inherited B2 eligibility incumbent.

Local compiler command:
`cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc/shared src/shared/nf_interact18b3.c src/tests/test_interact18b3.c -o /tmp/nf18b3_h0 && /tmp/nf18b3_h0`.

Mandatory follow-up: diagnose 337/360 B2 pending frames, wire real owner/physical adapters for seven interactions, run canonical/randomized/pathological/boundary/held-out M4/M3 comparisons and actual 60Hz timing, pass full engine CI, resolve licensing, obtain reviewer acceptance. Current G0-G8 for B3 remain OPEN.
