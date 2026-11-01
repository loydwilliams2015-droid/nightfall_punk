# Deferred public 3D Pong release requirements (intentional HOLD)

**Scope:** Record user acceptance feedback for the later public VPC-01 Pong release. This is NOT a request to ship or reframe 1.8C.2 now.

## Accepted feedback (Oct 31, 2026, first-person)
The current playable H63 minimal 3D Pong prototype is fun. Before any *public* playtest/release, implement and test all three player-facing changes:

1. **Distinct, legible player score counters:** explicit `PLAYER` and `OPPONENT` labels with spatial separation and sufficient contrast. Verify both sides score and reset independently. Do not bury the scores in debug tick or impulse readouts.
2. **Full court-side WASD locomotion:** W/S travels along player-side court depth, A/D travels left/right *within the player's half* with continuous bounds, configurable paddle collision geometry, and predictable camera-relative movement. Avoid merely relabeling the present one-dimensional W/S paddle as full 2D movement. Independent collision/clearance and high-speed edge cases must pass.
3. **Player camera zoom:** wheel zoom in/out plus conventional keyboard fallback (suggest `+`/`-` and keypad +/-), bounded near/far clipping, no keyboard/scroll interference with play, no dependence on development camera controls. Provide keyboard help, test wheel direction and limits.

## Must-pass public-release gates
- Evidence-backed control integration and player score telemetry; default settings clearly documented.
- Source-exact GCC/Clang/Release and Debug CTests, automated graphical capture under Linux with UI visibility, and a playable owner QA session.
- Linux package reproducibility, GPL-3.0-or-later corresponding source, third-party rights and deferred copyright-holder provenance review.
- Unprimed human pilot prompts first; no implied consensus from one informal observation.
- **Explicit owner authorization** and the separate required A6/B2/A5/B3, durability and concurrency standards for any declaration of full 1.8C.x engine closure.

**H64–H69 boundary:** These public-facing features are deferred. Continue 1.8C.2 authority/durability work instead. A possible later Pong release candidate may be built against an accepted 1.8C.x baseline.
