# nightfall!punk v1.8A.5 — Closure source snapshot

This source archive builds on the previous 1.8A.1–1.8A.5 H1 code with a new *opt-in* physical-world integration laboratory.

- `src/shared/nf_contact18a5_close.h/.c`: selective fine-load callback, full swept-capsule static-voxel safety gate, motor-to-dynamic-pair-to-history atomic staged commit, verified real support-load bridge.
- `src/shared/nf_contact18a5_wal.c`: local same-ABI snapshot WAL with validation, fsync-before-publication, parent directory sync and replay; not a networked portable production WAL.
- `src/tests/test_v18a5_close.c`: 46 strict fail/pass checks including adversarial static obstacles, fake support, disk/recovery interruptions and backlog overflow.
- `src/tests/sample_v18a5_close.c`: 7×200 common-input perturbation scenarios.
- `tools/analyze_v18a5_closure.py`: graded, honest-denominator PAR.
- `docs/LEDGER_V1.8A5_CLOSURE_H1.md`: locks, caveats, reproduction and next gate.

## Commands

```bash
bash ./v18a5_close.sh all
cmake -S . -B build/v18a5/closure-cmake -DNF_CONTACT_LAB_ONLY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/v18a5/closure-cmake --parallel
ctest --test-dir build/v18a5/closure-cmake --output-on-failure
```

This closes **H1 integration evidence**, not the full live FPS client. The source exists in this archive; the GitHub branch handoff is not a source graft. Reproduction requires a C11 toolchain and POSIX filesystem; the full game CMake path separately requires ENet and the optional raylib client dependency.
