# nightfall!punk 1.8A.5 — Canonical Grid / Condensed History (H1 laboratory)

This full source snapshot inherits the previous C collision laboratories and includes:
- `src/shared/nf_contact18a5.{h,c}` three-dimensional canonical/fine grid, material epoch, contact-summary journal and local atomic world update.
- `src/shared/nf_contact18a5_sink.c` single-process POSIX durable event append, replay check, fsync and ACK.
- `src/tests/test_v18a5_contact.c` 83 targeted scientific gates (negative controls test-only).
- `src/tests/test_v18a5_sink.c` POSIX sink recovery checks.
- `src/tests/test_v18a5_production.c` unsafe-policy exclusion check.
- `src/tests/sample_v18a5_contact.c` 10,000 paired model evaluations / 60,000 voxel observations.
- `tools/analyze_v18a5_contact.py` unweighted graded model analysis.
- `docs/LEDGER_V1.8A5_CANONICAL_CONDENSED_HISTORY.md` source status, locked decisions and limits.

Native Pop!_OS / Linux with C11 compiler, CMake >=3.20, Python 3:

```bash
bash ./v18a5.sh all
bash ./v18a5.sh regression
cmake -S . -B build/v18a5/cmake-lab -DNF_CONTACT_LAB_ONLY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/v18a5/cmake-lab --parallel
ctest --test-dir build/v18a5/cmake-lab --output-on-failure
```

The *regular* full game CMake still requires ENet and optionally raylib; the offline lab mode intentionally avoids that dependency so collision logic is independently reproducible. This is not a playable v1.8 demo or general rigid-body physics engine. The selected policies are H1 laboratory exit candidates, and the fine-cache resolver does not replace 1.8A.3 swept capsule CCD.
