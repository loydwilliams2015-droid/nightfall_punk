# 1.8A — Paired Arcade Counterpoint / Scientific Verification

## What is actually instantiated

These are **two new, original 2D X11 Pac-Man-style games using nightfall!punk code**, not GZDoom or LibreQuake source ports. Cinder Circuit tests narrow industrial corridors, fixed key/door authority, attrition and pursuit. Slipgate Circuit adds local traversal authorization and pushable material. The executable calls existing 1.8A3/4/5/6 C routines rather than reimplementing all physical-contact logic.

## Five a priori (conditional) contracts

1. **No false material clearance.** The full upright capsule sweep must not accept an occupied geometric corridor because one point query was free.
2. **No authority by affordance alone.** Traversal requires both geometric passage and a present key/switch permission; nearby ladder symbols cannot create arbitrary passage.
3. **No fictitious Purple.** Only an applied, reciprocal material impulse with successful history commit may be tagged Purple. An encounter, observation or one-way veto is not enough.
4. **No silent pending→free.** Missing canonical cache results in PENDING; the authored level witness supplies real data, after which the query is repeated. Mixed/fine resolution is not modeled by these exact per-tile demo chunks.
5. **No conflation of test populations.** The 2 fixed levels and randomized AI initial states are not a sample of independent FPS worlds; player score and survivor health do not establish H4 play feel.

## Targeted tests

| Test | Negative control | Positive test | Demonstration |
|---|---|---|---|
| Canonical pending | Missing chunk is not FREE | load authoritative 1m cells, then re-adjudicate | `--scenario pending` |
| Wall clearance | known wall must stay solid | empty corridor after fresh load | `--selftest` |
| Ladder authorization | switch missing denies | switch permits with valid path | `--scenario ladder` |
| Door authorization | key missing denies | key permits, canonical material revision | `--scenario door` |
| Dynamic crate | unverified impulse cannot move crate | actual 1.8A.4 applied pairwise impulse | `--scenario crate` |
| Contact history | failed sample cannot commit | 1.8A.5 cumulative history samples and event promotion | `--scenario crate` |
| Purple | cross-target nonreciprocal graph is not purple | applied reciprocal contact with matching target and tick | `--scenario crate` |

## AI-player factorial

Two level maps × two ghost policies × two player policies × 128 seed initializations = **1,024 simulated games**, each at most 300 ticks. The 1,024 digests were replayed in a second complete sweep using the same program/compiler and matched exactly. The policies were compared per paired seed, separately by level and player policy, with a fixed-seed 3,000-replicate nonparametric bootstrap of paired score and health differences. Confidence intervals describe this authored scenario distribution only.

- **Classic ghost control:** full maze goal, BFS movement, a simple energetic regime. This is NOT a faithful Doom monster or Quake monster.
- **Systemic ghost policy:** local occlusion-limited map observation, last-seen actor memory, bounded energy, exploration, repair and recharging. It is not intrinsically easier or harder by design; all observed tradeoffs are reported.
- **Goal player:** deterministic breadth-first routing against a globally known static maze, with local enemy avoidance. This is a strong test-player policy, not one claiming human-level skill.
- **Random player:** deterministic pseudo-random input sequence; a weak control for accidental score gains.

For numerical results, see `results/PAR.md`, `results/PAR.json`, and complete per-run `results/paired_1_8a6_arcade.csv`. Failed or nonreaching scenarios are not erased. Actual Cinder and Slipgate results are compared to **our own baseline AI**, not to code or binaries from GZDoom or LibreQuake.

## Key limitations / falsification agenda

The animation renders discrete cell positions after checking geometry and calculating a bounded motor impulse; it does not use the production 1.8A6 authoritative `NfWorld` tick for multiple moving actors. The 1.8A4 crate calculation is a genuine pairwise velocity exchange, but the resulting box/player **tile translation** is a discrete arcade projection. Collision geometry is upright capsule vs static AABB, not rotating Quake hulls. The 1.8A5 contact-history and grid are in-memory and loaded on demand for 1m parent cells; the engine's durable world WAL is not wired into these games. Neither candidate has renderer/raycast, weapon combat, multiplayer, external asset import, native GZDoom mod compatibility, or LibreQuake PAK/BSP compatibility. For stricter research, implement those as separate, GPL-compliant and appropriately attributed ports, and run **actual upstream binaries** as controls under matched tasks.

**Evidence disposition: H1 executable subsystem demonstration + paired authored AI-player evaluation. Not H3 full FPS, H4 user feel, or actual upstream replacement.**
