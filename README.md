# nightfall_punk

Open-source copy-left systemic arena FPS developed in C + raylib.

**Current milestone: v0.5 — Agent Intelligence Framework.** Completed v0.4 Combat is preserved at `archive/v0.4`.

nightfall!punk is being rebuilt graybox-first: gameplay, physics, networking, combat and agent intelligence must become coherent before original aesthetics return.

v0.5 keeps the v0.4 server-authoritative movement/combat substrate and adds four server-controlled Human Rival agents by default. AI and network players now converge on a shared control vocabulary (`NfMoveInput` + `NfCombatInput`) and therefore use the same movement, Fuzzy Rail traversal, weapons, ammo, hit validation, health, death and respawn truth.

The first World Semantic Alert bus is live. Combat facts such as gunfire, damage, death and respawn become bounded semantic alerts consumed by AI hearing and memory rather than being inferred from transport packets. Human Rival AI uses staggered perception, imperfect memory/confidence, utility action selection, generated cover affordances, reservations, squad reports, reaction delay, imperfect aim, burst fire and stuck recovery.

Faction identity is now separated from relationship state. Human Rivals are secondary/temporary terrain antagonists and may conceptually move among hostility, contest, truce and cooperation. The v0.5 server defaults to HOSTILE and supports a debug truce start. Nightmare Ranchers remain the primary/semi-permanent environmental antagonism: always non-negotiable, usually predatory, with the documented rare hostile/non-predatory disposition that can produce avoidant detente without diplomacy.

## Build and test

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh combat-smoke
./nightfall.sh ai-smoke
```

## Demo: four Human Rival AI

```bash
./nightfall.sh local
```

The graphical client starts an isolated-port dedicated server with **four Human Rival AI** by default. Red actors are Human Rivals. They should perceive, move, strafe, fire, reload, damage/kill the player, die and respawn through the same authoritative simulation contracts as network-controlled actors.

Controls: WASD move, mouse look, Shift sprint, Ctrl crouch, Space jump, E interact/ladder, left mouse fire, R reload, 1 carbine, 2 pistol, F10 toggle/reacquire mouse capture.

Useful debug variants:

```bash
NF_AI_COUNT=1 ./nightfall.sh local
NF_RIVAL_TRUCE=1 ./nightfall.sh local
```

`NF_AI_COUNT=1` isolates one Rival for behavior inspection. `NF_RIVAL_TRUCE=1` proves relationship policy can suppress offensive action without changing faction identity.

The HUD now distinguishes **visible positional corrections** from internal prediction **resyncs**, resolving the ambiguous v0.4 correction counter that could rise without corresponding rubber-banding.

## Impairment testing

Terminal 1:

```bash
./nightfall.sh server --ai-count 4 --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Terminal 2:

```bash
./nightfall.sh client --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Completed versions are preserved as immutable `archive/v0.X` branches. See `docs/LEDGER.md`, `docs/ARCHITECTURE.md`, and `docs/AI_ANTAGONISM.md`.
