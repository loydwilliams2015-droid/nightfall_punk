# 1.8C.x exit demonstration proposal — minimalist 3D Pong (VPC-01 descendant)

**Status:** H63 executable research prototype and acceptance charter; not certification of all 1.8C.x gates, production release or independently validated high-speed wall-riding.

## User observation (qualitative, single observer)
User reports successfully playing a default-settings VPC-01 interaction that felt like simple Pong. They describe **positive primordial responsiveness and delicacy**, and a **promising prospective direction** for eventual high-velocity grinding/wall-riding/momentum traversal.

Evidence class: first-hand, non-blinded qualitative playtest observation; build referenced as VPC-01, no source SHA independently captured in the observation. Neither an effect size nor a validated wall-riding system; no p-value. Preserve the original observation separately from source-exact performance and mechanics tests.

## Planned final 1.8C.x player-facing demonstration
Produce a **single minimalist 3D Pong** game, rather than overextending a research slice into an untested shooter:
- 3D raylib court with spatially legible ball, left/right independently controlled paddles, two reflective rails, scoring, deterministic serve, and restart.
- WASD's vertical pair (W/S) controls left paddle. Right paddle uses a simple AI by default; TAB toggles manual arrow-key control. R resets; ESC exits.
- Fixed 60-Hz physics. Moving paddles affect tangential contact through their bounded velocity, using real `nf18a4_apply_contact` physical impulses, not scripted ball teleports.
- Swept event tests for paddle face, court walls and goals to avoid high-speed missed collision. Event ties, unsupported simultaneous contacts and too many collisions per tick **fail closed**. Simple 3D meshes only; no external art required.
- Deterministic local player interaction; clear score and observable collision counts, with graphical performance observations. No multiplayer/anti-cheat or network authenticity claim.

## H63 files and actual scope
- Core `src/shared/nf_retc_pong.[ch]`
- Test `src/tests/test_vpc01_3d_pong.c`
- Graphics `src/demo/vpc01_3d_pong.c`
- CMake target `nightfall_vpc01_3d_pong`; CTest `vpc01_minimal_3d_pong_v18cx`.

This **A4 stand-alone Pong** is a candidate for the eventual final 1.8C.x *showcase*, not a replacement for independent acceptance of authoritative A6 two-actor A5 material, B2 eligibility, B3 obligations, durable recovery, concurrent publisher lifetime, network reader consistency, source ownership/rights, and independent whole-engine verification. The current Pong paddle contacts are bounded XZ plane impulses; full 6-DOF wall riding/grinding is absent.

## Acceptance
1. Source-exact GCC and Clang debug/full-game compilation and complete inherited CTest suites.
2. Canonical and randomized deterministic replay at fixed 60 Hz, independently staged/published worlds; no mutation before accepted tick.
3. Paddle and rail impulses have real A4 receipts; high-speed swept test verifies contact at 1,000 world-units/s; scoring and serve/scoreboard correctness.
4. Negative cases: invalid and non-finite inputs, stale candidate refusal, pathological too-many-events refusal, no mutation on rejection.
5. Captured Linux Xvfb graphical screenshots from actual built binary; executable package SHA, GPL license, corresponding source and third-party notices verified before distributing.
6. Human followup: neutral observation prompt first, no priming to expected responsiveness or wall-riding, then reproduce reported problems; do not treat one qualitative review as a statistical trial.

**Exit decision:** choose to present this minimalist game as the late 1.8C.x intentional demonstration *once its own mandatory evidence passes*. Keep production / full-program 1.8C.x finalization gates independent; serious unresolved authority defects must not be waived to make Pong look complete.
