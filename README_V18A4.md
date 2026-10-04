# nightfall!punk — v1.8A.4 dynamic-contact research package

Source is a **standalone H1 laboratory** plus inherited full project snapshot. It is not a compiled production player physics replacement.

## Run on Pop!_OS

Install `build-essential python3` (and `cmake libenet-dev` when running full project CMake). From this folder:

```bash
bash ./v18a4.sh all
```

Outputs:
- `build/v18a4/fixtures.csv` — 56 explicit deductive/pass-fail assertions
- `build/v18a4/samples.csv` — 8,000 unweighted comparative model evaluations
- `build/v18a4/PAR_RESULTS.md` — scientific evidence and exit selection

Historical regression with original source scripts:

```bash
bash ./v18a1.sh test
bash ./v18a2.sh test
bash ./v18a3.sh test
```

The test package contains the original v1.8A.3 source (including 1.8A.3 Model C geometry/traversal gate), plus newly implemented `src/shared/nf_contact18a4.{h,c}`, `src/tests/test_v18a4_contact.c`, `src/tests/sample_v18a4_contact.c`, `tools/analyze_v18a4_contact.py`, the script `v18a4.sh` and a CMake registration. Build 1.8A.4 in parallel to the actual gameplay controller; production promotion requires authoritative transaction integration and live native physics validation.

The published GitHub 1.8A.4 design branch may contain only the ledger and handoff, not this C source; the **archive is the reproducible implementation** until full source graft and GitHub CI verification.
