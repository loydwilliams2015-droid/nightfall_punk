# nightfall_punk

Open-source copy-left systemic arena FPS developed in C + raylib.

**Current candidate milestone: v0.7 — Spatial Ecology / Situated Agency.** Completed v0.5 Agent Intelligence is preserved at `archive/v0.5`; v0.6 Encounter Intelligence is merged on `main` and remains the regression-protected combat/AI baseline while its separate human archive-acceptance gate is still open.

nightfall!punk is being rebuilt graybox-first: gameplay, physics, networking, combat and agent intelligence must become coherent before original aesthetics return.

v0.7 extends the v0.5–v0.6 Human Rival stack with a bounded spatial layer. Agents can roam across a coarse region graph, choose a meaningful area, adopt a local situated task, remember recent regional history, react to server-authored semantic events, and then hand ordinary movement back through the same shared movement/Fuzzy Rail and authoritative combat systems. The design rule is **large world != large per-agent thought**.

The current spatial hierarchy is:

`server/world semantics -> regional context -> local task/sample -> ordinary AI control -> v0.6 encounter governance -> shared movement/combat -> authoritative truth`

The v0.7 demo graybox is deliberately **0.40 km² (500 m x 800 m)**, or 5% of the eventual 8 km² map-area target. It contains **24 coarse regions** and bounded local samples; there is no whole-map high-resolution influence grid.

Local spatial preference currently follows the locked order: **route congestion -> ally support -> enemy threat -> exposure -> pressure -> objective value**. Objectives choose the problem; spatial reasoning chooses a sensible way to solve it. High threat or low objective value bias choices, while only hard physical/logical impossibilities veto them.

The v0.6 encounter layer remains live underneath: finite angular tracking, aim settling, movement-sensitive firing readiness, two soft pressure-authorized Rival slots by default, contextual role rebidding, and damage-driven suppression/retreat. Human Rivals still use the same authoritative movement, Fuzzy Rail traversal, weapons, ammo, hit validation, health, death and respawn truth as network actors.

The epistemic contract is unchanged: server truth is not automatically individual AI knowledge, and secondhand squad reports remain lower-confidence evidence rather than telepathy.

Faction identity remains separate from relationship state. Human Rivals are secondary/temporary terrain antagonists and are negotiable in principle. **Dream Cattlers** are the primary semi-permanent ecological antagonism: always non-negotiable, usually predatory, with a rare hostile/non-predatory disposition that can permit avoidant detente without diplomacy. Some implementation identifiers still use the earlier `Rancher` technical name pending a coordinated rename.

## Build and test

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh combat-smoke
./nightfall.sh encounter-smoke
./nightfall.sh spatial-smoke
```

## Demo: spatial ecology / situated agency

```bash
./nightfall.sh local
```

The graphical client starts an isolated-port dedicated server with **four Human Rival AI and two pressure slots** by default. Red actors are Human Rivals.

Press **F4** to toggle the coarse 24-region graph overlay. The client intentionally does not receive each Rival's full tactical scratch state; authoritative server diagnostics in `build/server.log` report current/target/next region, local task, interruption class, and the six spatial field values.

Controls: WASD move, mouse look, Shift sprint, Ctrl crouch, Space jump, E interact/ladder, left mouse fire, R reload, 1 carbine, 2 pistol, F4 spatial graph, F10 toggle/reacquire mouse capture.

Useful debug variants:

```bash
NF_AI_COUNT=1 ./nightfall.sh local
NF_AI_PRESSURE_SLOTS=1 ./nightfall.sh local
NF_RIVAL_TRUCE=1 ./nightfall.sh local
```

`NF_AI_COUNT=1` isolates one Rival. `NF_AI_PRESSURE_SLOTS=1` narrows lethal squad pressure without weakening the underlying agent brain. `NF_RIVAL_TRUCE=1` is especially useful for observing spatial roaming/task behavior without combat continually interrupting it.

## Human acceptance target

The v0.7 question is not simply whether an AI can cross a larger map. The demo should show that Rivals can **leave one local situation, travel through the region graph, inhabit a bounded local task, distribute rather than clump, and return to combat competence when meaningful evidence appears**.

After a recording, also capture:

```bash
tail -n 160 build/server.log
```

The video shows outward behavior; the server trace shows region/task/field reasoning.

## Impairment testing

Terminal 1:

```bash
./nightfall.sh server --ai-count 4 --pressure-slots 2 --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Terminal 2:

```bash
./nightfall.sh client --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Completed versions are preserved as immutable `archive/v0.X` branches only after their acceptance gate is closed. See `docs/LEDGER.md`, `docs/ARCHITECTURE.md`, and `docs/AI_ANTAGONISM.md`.
