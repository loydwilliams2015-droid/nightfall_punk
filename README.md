# nightfall_punk

Open-source copy-left arena FPS developed in C + raylib.

**Current milestone: v0.2 — Movement.**

nightfall!punk is being rebuilt graybox-first: gameplay, physics, networking and AI architecture must become coherent before original aesthetics return.

v0.2 adds a first-person movement laboratory with shared-simulation locomotion, explicit traversal states, stairs/ramps, crouch, sprint, crouch-gated second jump, ladder/vault/mantle scaffolding, a moving platform, and the first Fuzzy Rail local traversal-candidate system.

Build:

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh client
```

For headless validation:

```bash
./nightfall.sh build-headless
./nightfall.sh test
./nightfall.sh server
```

See `docs/LEDGER.md` and `docs/ARCHITECTURE.md`.
