# nightfall_punk

Open-source copy-left systemic arena FPS developed in C + raylib.

**Current milestone: v0.6 — Encounter Intelligence.** Completed v0.5 Agent Intelligence is preserved at `archive/v0.5`.

nightfall!punk is being rebuilt graybox-first: gameplay, physics, networking, combat and agent intelligence must become coherent before original aesthetics return.

v0.6 keeps the v0.5 server-side perception / memory / utility / affordance architecture intact and adds a separate **encounter-governance layer** over AI control intent. Human Rivals still use the same authoritative movement, Fuzzy Rail traversal, weapons, ammo, hit validation, health, death and respawn truth as network actors, but their aggregate combat pressure is now bounded before those controls reach the combat system.

The encounter layer adds finite angular tracking, aim settling, movement-sensitive firing readiness, two soft pressure-authorized squad slots by default, contextual role rebidding, and damage-driven suppression/retreat. Non-pressure Rivals can still take rare opportunistic shots or defend themselves at close range; this is deliberately a soft pressure economy rather than a hard one-enemy-at-a-time shooter token system.

The v0.5 epistemic substrate remains live: staggered perception, imperfect memory/confidence, World Semantic Alert hearing, secondhand squad reports, utility action selection, generated cover affordances and reservations. Server truth remains distinct from individual AI knowledge and squad reports.

Faction identity remains separate from relationship state. Human Rivals are secondary/temporary terrain antagonists and are negotiable in principle. **Dream Cattlers** are the primary semi-permanent ecological antagonism: always non-negotiable, usually predatory, with a rare hostile/non-predatory disposition that can permit avoidant detente without diplomacy. Some implementation identifiers still use the earlier `Rancher` technical name pending a coordinated rename.

## Build and test

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh combat-smoke
./nightfall.sh encounter-smoke
```

## Demo: encounter intelligence

```bash
./nightfall.sh local
```

The graphical client starts an isolated-port dedicated server with **four Human Rival AI and two pressure slots** by default. Red actors are Human Rivals. The v0.6 acceptance question is no longer merely whether they can kill the player; it is whether four capable agents create readable, survivable pressure rather than behaving like four independent aimbots.

Controls: WASD move, mouse look, Shift sprint, Ctrl crouch, Space jump, E interact/ladder, left mouse fire, R reload, 1 carbine, 2 pistol, F10 toggle/reacquire mouse capture.

Useful debug variants:

```bash
NF_AI_COUNT=1 ./nightfall.sh local
NF_AI_PRESSURE_SLOTS=1 ./nightfall.sh local
NF_RIVAL_TRUCE=1 ./nightfall.sh local
```

`NF_AI_COUNT=1` isolates one Rival. `NF_AI_PRESSURE_SLOTS=1` narrows squad lethal pressure without weakening the underlying agent brain. `NF_RIVAL_TRUCE=1` proves relationship policy can suppress offensive action without changing faction identity.

Server diagnostics now report each Rival's role, mode, target confidence, visibility, cover choice, pressure authorization, aim-settle value, suppression value and current aim error. The graphical HUD continues to distinguish **visible positional corrections** from prediction **resyncs**.

## Impairment testing

Terminal 1:

```bash
./nightfall.sh server --ai-count 4 --pressure-slots 2 --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Terminal 2:

```bash
./nightfall.sh client --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Completed versions are preserved as immutable `archive/v0.X` branches. See `docs/LEDGER.md`, `docs/ARCHITECTURE.md`, and `docs/AI_ANTAGONISM.md`.
