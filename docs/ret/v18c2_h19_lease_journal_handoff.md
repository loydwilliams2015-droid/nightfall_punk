# H19 RET: lease and logical frame reference gates
H18 source SHA: 7364003787726e07e2f0bccad9989524b9a7473e; H18 status must be checked separately before promotion.
## New experiments
- Four C11 worker threads attempt registration; atomic close rejects new leases and drain is verified after join.
- 2048 source-distinct logical A4/A5/B3 frames: precommit recovery must reject, postcommit full checksum recovers, a corrupted A5 digest must invalidate the entire frame.
- Independent Valgrind Helgrind workflow on Ubuntu 22.04 offers an alternate detector because independent TSAN smoke itself crashed on runner. Actual results must be observed.
## Important limits
This NfC2Lease is a separately implemented **reference**, not yet integrated into real nf_world_unbind_tick_owner or Nf18a6Runtime destruction, and the callers must keep lease storage alive until workers join. Closing rejects new acquisition but does not itself free the world.
This NfC2Journal holds an **in-memory** committed frame: it provides logical indivisibility for reference tests only, not durable storage, fsync, cross-machine replay, crash recovery, or actual A4/A5/B3 state restoration. Never label it persistent physical-contractual atomicity.
## Acceptance gates
(1) Full CTest GCC/Clang strict/ASan/UBSan, all on one source SHA.
(2) Helgrind no race reports on test; do not hide failures.
(3) Real world owner lease integration: explicit hold/close/drain, reject tick and mutation on shutdown, exercise creator/destructor epoch reuse.
(4) Durable journal integration: source exact A4 impulse + A5 history + B3 receipt in a single owner transaction frame, then forced crash/torn frame recovery and replay proof.
Production & 1.8C.3 HOLD until (3-4) actually pass.
