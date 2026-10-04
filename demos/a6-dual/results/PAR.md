# nightfall!punk 1.8A6 — Dual Arcade Scientific PAR

- Cohorts: 1024 deterministic AI-player runs (128 seeds × 2 authored arenas × 2 player policies × 2 ghost policies).
- Replay: 0 mismatches across all 1024 replayed output digests.
- Hard gates: no observed history commit failure or unresolved cache miss in sampled runs; targeted scenarios independently test crate impulses, authorization and pending loading.
- Evidence: H1 synthetic paired game comparison; NOT GZDoom or LibreQuake measured performance; NOT a 3D player physics benchmark.

| Arena | Player | Classic mean score | Systemic mean score | Score delta [bootstrap 95% CI] | Classic health | Systemic health |
|---|---|---:|---:|---|---:|---:|
| cinder | goal | 1931.875 | 2062.812 | 130.938 [92.344, 166.445] | 0.297 | 2.227 |
| cinder | random | 55.859 | 173.047 | 117.188 [105.312, 129.453] | -0.789 | 3.305 |
| slipgate | goal | 1373.047 | 1839.297 | 466.25 [432.578, 498.281] | 0.0 | 1.75 |
| slipgate | random | 55.938 | 206.172 | 150.234 [137.578, 163.203] | -0.32 | 3.156 |

## Scope and falsification discipline

The baseline policy is a deliberately simple, omniscient BFS pursuer; the systemic policy observes local cells, retains last-seen actor position, spends/regenerates energy and retreats/recharges. The policies do not represent the actual GZDoom/LibreQuake AI.
Cohort policy, player policy, initial RNG state and map are paired. Branch-specific random draws are not synchronized after divergence.
Scores, health, contact count, history events and energy use are reported separately. Survival, lower contact or higher score is not by itself proof of a better FPS experience.
The 1.8A authoritative tick scheduler, 3D physics, normal engine camera and server-client reconciliation are outside these small 2D executables; selected authoritative geometric, motor, impulse, and contact-history subroutines are used directly.
No original Doom/Quake maps, proprietary art, GZDoom code, or LibreQuake assets are shipped in this experiment.

## Per-cohort observations

### cinder / goal

- classical: ticks=272.914; score=1931.875; health=0.297; won=0.117; contacts=0.0; purple=0.0; cache_loads=37.344; cache_pending=37.344; energy_retreats=0.0; stale_chases=0.0; history_events=0.0; blocked_moves=0.0
- systemic: ticks=286.828; score=2062.812; health=2.227; won=0.758; contacts=0.0; purple=0.0; cache_loads=38.312; cache_pending=38.312; energy_retreats=72.578; stale_chases=43.188; history_events=0.0; blocked_moves=0.0
- Paired systemic − classic mean surviving health [bootstrap 95% CI]: [1.93, 1.672, 2.18]

### cinder / random

- classical: ticks=38.352; score=55.859; health=-0.789; won=0.0; contacts=0.0; purple=0.0; cache_loads=2.0; cache_pending=2.0; energy_retreats=0.0; stale_chases=0.0; history_events=0.0; blocked_moves=15.961
- systemic: ticks=271.906; score=173.047; health=3.305; won=0.0; contacts=0.0; purple=0.0; cache_loads=3.891; cache_pending=3.891; energy_retreats=70.688; stale_chases=0.867; history_events=0.0; blocked_moves=113.102
- Paired systemic − classic mean surviving health [bootstrap 95% CI]: [4.094, 3.797, 4.367]

### slipgate / goal

- classical: ticks=180.695; score=1373.047; health=0.0; won=0.0; contacts=0.234; purple=0.234; cache_loads=24.609; cache_pending=24.609; energy_retreats=0.0; stale_chases=0.0; history_events=0.125; blocked_moves=0.0
- systemic: ticks=294.109; score=1839.297; health=1.75; won=0.0; contacts=1.109; purple=1.109; cache_loads=30.219; cache_pending=30.219; energy_retreats=74.766; stale_chases=43.016; history_events=0.82; blocked_moves=0.0
- Paired systemic − classic mean surviving health [bootstrap 95% CI]: [1.75, 1.57, 1.93]

### slipgate / random

- classical: ticks=35.812; score=55.938; health=-0.32; won=0.0; contacts=0.0; purple=0.0; cache_loads=2.203; cache_pending=2.203; energy_retreats=0.0; stale_chases=0.0; history_events=0.0; blocked_moves=14.445
- systemic: ticks=257.547; score=206.172; health=3.156; won=0.0; contacts=0.0; purple=0.0; cache_loads=5.266; cache_pending=5.266; energy_retreats=65.805; stale_chases=2.008; history_events=0.0; blocked_moves=93.844
- Paired systemic − classic mean surviving health [bootstrap 95% CI]: [3.477, 3.18, 3.758]

