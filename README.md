# nightfall_punk

Open-source copy-left systemic arena FPS developed in C + raylib.

**Current candidate milestone: v0.8 — Dream Cattler Habitat Ecology.** Accepted v0.6 Encounter Intelligence and v0.7 Spatial Ecology / Situated Agency are preserved at `archive/v0.6` and `archive/v0.7`.

nightfall!punk is being rebuilt graybox-first: gameplay, physics, networking, combat and agent intelligence must become coherent before original aesthetics return.

v0.8 adds a second AI family over the accepted v0.7 0.40 km² / 24-region lab. Human Rivals retain their perception/utility/spatial/encounter stack. Dream Cattlers are taller ecological predators whose behavior is driven by habitat continuity, infestation, prey evidence, territorial pressure and imperfect pack reports rather than Human Rival tactical roles.

The two AI paths converge at the same control/truth boundary:

`world + semantic evidence -> Human Rival intelligence OR Dream Cattler habitat/pack ecology -> NfControlFrame -> shared movement/Fuzzy Rail + authoritative combat -> server truth`

Dream Cattlers are the primary semi-permanent ecological antagonism: always non-negotiable, usually predatory, with a rare hostile/non-predatory disposition. Most behavior is pack-linked lurking/stalking/sniping/swoop pressure; rare seeded loners operate independently, while rare Director-conditioned `HUNT_SURGE` events allow short dogpile/horde episodes followed by repose.

Cattler power is deliberately asymmetric. They are statistically taller and strongest when exploiting colonized space, but dedicated **knee** and **foot** hit zones reduce locomotor integrity and therefore spatial dominance. v0.8's swoop is a graybox committed sprint/jump approach, not final flight physics.

The authoritative Cattler ecology maintains internal channels for infestation, prey, displacement, expansion and survival. **Those exact numbers are not an ordinary player HUD.** Player-facing ecology is intended to arrive as sense-data through recurring territory, stalking, route pressure and later environmental/audio expression. Exact values stay in server/debug diagnostics.

The production map-area target remains 8 km². The current demo intentionally stays at **0.40 km² (500 m x 800 m)**, 5% of that target, so behavior remains testable.

## Build and test

```bash
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh combat-smoke
./nightfall.sh encounter-smoke
./nightfall.sh spatial-smoke
./nightfall.sh cattler-smoke
```

## Demo: Dream Cattler habitat ecology

```bash
./nightfall.sh local
```

Default local configuration: **four Human Rivals, two Human-Rival pressure slots, and three Dream Cattlers**. Red actors are Human Rivals. Tall orange actors are Dream Cattlers; the yellow lower-body bands are temporary graybox markers for the knee/foot locomotor weakness.

Press **F4** to toggle the accepted v0.7 24-region graph. The ordinary client does not receive whole AI scratch maps or the internal Cattler ecology ledger; use `build/server.log` for authoritative reasoning diagnostics.

Controls: WASD move, mouse look, Shift sprint, Ctrl crouch, Space jump, E interact/ladder, left mouse fire, R reload, 1 carbine, 2 pistol, F4 spatial graph, F10 toggle/reacquire mouse capture.

Useful debug variants:

```bash
NF_AI_COUNT=0 NF_CATTLER_COUNT=3 NF_CATTLER_PROFILE=pack ./nightfall.sh local
NF_AI_COUNT=0 NF_CATTLER_COUNT=1 NF_CATTLER_PROFILE=loner ./nightfall.sh local
NF_AI_COUNT=0 NF_CATTLER_COUNT=5 NF_CATTLER_PROFILE=horde ./nightfall.sh local
NF_RIVAL_TRUCE=1 ./nightfall.sh local
```

## Human acceptance target

The v0.8 question is not merely whether tall orange enemies can kill the player. The demo must show that Cattlers **belong to and colonize space, exchange imperfect prey evidence, create lurking/ambush uncertainty, lose spatial dominance when their locomotor weak points are damaged, and remain behaviorally distinct from Human Rivals**.

Record the default run first. Afterward capture:

```bash
tail -n 200 build/server.log
```

The video shows outward behavior and sense-data; the trace exposes habitat, pack-report, locomotor and internal ecology reasoning.

## Impairment testing

Terminal 1:

```bash
./nightfall.sh server --ai-count 4 --pressure-slots 2 --cattler-count 3 --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Terminal 2:

```bash
./nightfall.sh client --sim-latency 40 --sim-jitter 8 --sim-loss 1
```

Completed versions are preserved as immutable `archive/v0.X` branches only after their acceptance gate is closed. See `docs/LEDGER.md`, `docs/ARCHITECTURE.md`, and `docs/AI_ANTAGONISM.md`.
