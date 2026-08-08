# nightfall_punk

Open-source copy-left arena FPS developed in C + raylib.

**Current milestone: v0.4 — Combat.** Completed v0.3 Networked Movement is preserved at `archive/v0.3`.

nightfall!punk is being rebuilt graybox-first: gameplay, physics, networking, combat and AI architecture must become coherent before original aesthetics return.

v0.4 keeps the v0.3 authoritative movement/network substrate and adds a server-authored graybox combat loop: carbine + pistol, health, ammo, reload/switch states, body/head hitscan, bounded lag compensation, death/respawn, faction damage rules, combat events, primitive weapon/target presentation, combat diagnostics and four-client combat smoke testing.

Combat events are the first concrete producers for the future World Semantic Alerts layer. The full semantic routing/AI/objective consumer system is intentionally later.

## Build and test

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh combat-smoke
```

## Demo: server + graphical combat client

```bash
./nightfall.sh local
```

Controls: WASD move, mouse look, Shift sprint, Ctrl crouch, Space jump, E interact/ladder, left mouse fire, R reload, 1 carbine, 2 pistol, F10 toggle/reacquire mouse capture.

The v0.4 client uses focus-aware mouse capture: it releases the cursor when the game loses focus and automatically reacquires relative input shortly after focus returns. The HUD reports `mouse FOCUSED`, capture state, mouse delta and absolute-delta fallback use so Linux/X11/XWayland input failures are visible in recordings instead of silent.

The server spawns simple rival target dummies so the combat loop can be tested with one graphical client. Additional clients receive alternating PLAYER/RIVAL factions for human duel testing while friendly fire remains off by default.

## Impairment testing

Terminal 1:

```bash
./nightfall.sh server --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Terminal 2:

```bash
./nightfall.sh client --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Completed versions are preserved as immutable `archive/v0.X` branches. See `docs/LEDGER.md` and `docs/ARCHITECTURE.md`.
