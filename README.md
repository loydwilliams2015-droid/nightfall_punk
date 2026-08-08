# nightfall_punk

Open-source copy-left arena FPS developed in C + raylib.

**Completed milestone: v0.3 — Networked Movement. Next: v0.4 — Combat.**

nightfall!punk is being rebuilt graybox-first: gameplay, physics, networking and AI architecture must become coherent before original aesthetics return.

v0.3 preserves the v0.2 traversal/Fuzzy Rail simulation and proves it through an authoritative networking path: ENet transport, a versioned binary protocol, 60 Hz input/prediction/server simulation, 30 Hz snapshots, reconciliation, remote interpolation scaffolding, reconnect/session-token machinery, network impairment simulation, libsodium-backed token derivation when available, and automated four-client smoke testing.

The accepted v0.3 tree is preserved at `archive/v0.3`.

World Semantic Alerts are **not** part of v0.3. They are planned as a later server-authored gameplay-event layer for combat, AI, objectives, mechanisms and the Dynamic Affordance Graph.

## Build and test the archived network milestone

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh net-smoke
```

## Demo: server + client

```bash
./nightfall.sh local
```

This launches the dedicated server in the background and the raylib client over the real ENet localhost transport.

## Impairment testing

```bash
./nightfall.sh server --sim-latency 80 --sim-jitter 15 --sim-loss 2
```

Then in another terminal:

```bash
./nightfall.sh client
```

Completed versions are preserved as `archive/v0.X` branches. See `docs/LEDGER.md` and `docs/ARCHITECTURE.md`.
