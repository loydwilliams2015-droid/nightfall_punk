# 1.8B.1 H2 — Clean C11 Reproduction (NOT the original H1 source)

Parent: integration/v1.8a6-embodiment-h1 (verified antecedent of 1.8B.1 H1). No 1.8B.2 implementation is authorized.

The original H1 tested C source is missing (issue #37). This is *new* simplified smart-object C logic, not recompilation of the original code, not the full historical M0-M4 experiment, not an engine-integrated replacement, and not a production candidate. The local test outcome is an H2 reproduction-development observation only.

Mechanisms: deterministic actor-local intent, world and material-identity epoch comparison, fail-closed material uncertainty, physical reach/clearance/support and contractual gates, checked witness at commit, resource-capacity preflight, no publication on failure, immutable recorded event and anti-double-commit via revised authority version. Material identity remains stable when object state revision changes.

Observed in a local Linux environment: `gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -fsanitize=address,undefined -fno-omit-frame-pointer nf_smart18b1.c test_smart18b1.c -o test_smart18b1 && ./test_smart18b1` succeeded. Output: `clean_reproduction_1.8B.1: 6000 deterministic seed fixtures + capacity gate PASS (not historical M0-M4 corpus)`; `cases=6000`. This is a local test of the reconstruction artifacts, not a verified run of the GitHub copies or inherited A6 engine.

Compile this GitHub copy:
```sh
cd src/reproduction/v18b1
cc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 nf_smart18b1.c test_smart18b1.c -o test_smart18b1
./test_smart18b1
```

Limitations requiring further work: actual A3 geometry, A4 applied impulses, A5 contact-history/WAL and A6 world-tick ownership are **not yet invoked by this isolated library**. No full M0–M4 corpus, recovered historical H1 source matching SHA, same-tick multi-actor fairness, disk durability, production network or graphical acceptance. No 1.8B.2 build.

The historical archive SHA-256 `63c333a16b1a43981156185390517458c3febbc6704cad725d02cf615c4d4644` is NOT a hash for this reproduction. Preserve the historical H1 branch and issue #37.
