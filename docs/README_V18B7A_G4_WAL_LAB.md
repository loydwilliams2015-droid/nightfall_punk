# Group 4 POSIX WAL H0 — Pop!_OS/Linux reproduction
Research prototype: actor/action debits only, no real physics, world snapshots, network session authentication, or production release.

From repository root, run:

    cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -DNF4_TESTING -Isrc/shared src/shared/nf_wal18b7.c src/tests/test_wal18b7.c -o /tmp/nf4_wal
    /tmp/nf4_wal

Repeat with clang, or use GCC -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer and ASAN_OPTIONS=detect_leaks=0.
Remove NF4_TESTING for a non-test build. The synthetic lost-ACK case deliberately exits after writing a complete record before syncing; it demonstrates a possible recovered record after process interruption, not a power-failure durability proof.

## API
- nf4_open: advisory nonblocking exclusive file lock; inspect/replay versions, balances, epochs, checksums; quarantine corrupt/torn/unknown outcomes.
- nf4_apply: reject altered duplicate, stale epoch or invalid debit; synchronously append, fdatasync, then update in-memory state and acknowledge. A failed write/sync returns uncertain and prevents further commits until reviewed recovery.
- nf4_close: release the file descriptor.
- Records: 64-byte fixed little-endian records, up to 64 receipts; no silent rollover, background compaction or auto-truncation.
- The FNV checksum detects ordinary corruption, not adversarial forgery.

## Limitations
One intended authoritative writer; caller-owned trusted initial genesis balance; parent directory sync on first path creation; storage hardware determines real power-loss durability. No verified transaction spanning actual A4 impulses, A5 contact history, A6 authority and B5 registry exists here. No automatic quarantine repair mechanism.

See docs/RET_V18B7A_G4_N01_N12_P01_P14_HANDOFF.md and the 26 per-determination G4 lock files for RET, degradation, occluded crash resolution, background stability and holds.
