# nightfall_punk

Open-source copy-left arena FPS developed in C + raylib.

**Current milestone: v0.3 — Networked Movement.**

nightfall!punk is being rebuilt graybox-first: gameplay, physics, networking and AI architecture must become coherent before original aesthetics return.

v0.3 keeps the v0.2 traversal/Fuzzy Rail simulation and puts it through a real authoritative networking path: ENet transport, a versioned binary protocol, 60 Hz input/prediction/server simulation, 30 Hz snapshots, reconciliation, remote interpolation scaffolding, reconnect, network impairment simulation, and automated clients.

## Build and test

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
```

ENet 1.3.18 is fetched automatically when a system ENet package is unavailable. For strong session-token derivation on Pop!_OS/Ubuntu, install `libsodium-dev`; the build remains usable for localhost testing without it.

## Demo: server + client

```bash
./nightfall.sh local
```

This launches the dedicated server in the background and the raylib client over the real ENet localhost transport.

## Four-client network smoke test

```bash
./nightfall.sh build-headless
./nightfall.sh net-smoke
```

## Impairment testing

```bash
./nightfall.sh server --sim-latency 80 --sim-jitter 15 --sim-loss 2
```

Then in another terminal:

```bash
./nightfall.sh client
```

Completed versions are preserved as `archive/v0.X` branches. See `docs/LEDGER.md` and `docs/ARCHITECTURE.md`.
