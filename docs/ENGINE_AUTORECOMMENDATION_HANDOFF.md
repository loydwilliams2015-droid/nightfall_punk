# Engine Autorecommendation Handoff

Base candidate remains frozen at `e65e19e931794fbe2f7e7f359b982df6cc10d4de` on `build/v1.2-teammate-causality`.

This design branch adds policy only. It does not change the human-video candidate.

## Automatic recommendation status

### AUTO KEEP

- Relational Action Selection above steering.
- Semantic Steering distinct from pathfinding and physics.
- `snap_locomotion` as canonical replacement for retired Fuzzy Rail terminology.
- `snap_locomotion` is a regime inside Embodied Arcade Locomotion, not the whole locomotion layer.
- High-authority arcade intention must still pass legitimate affordance, capability, energy/contamination/momentum, collision, and material truth.
- Existing navigation/physics machinery remains primary; relational layers extend rather than duplicate it.
- Material impossibility breaks commitment.
- World truth does not imply actor knowledge.
- Sparse/event-driven material simulation preferred over universal continuous simulation.
- Persistent consequences and compressed expectations preferred over decorative micro-history.
- Negative-control testing is mandatory for non-omniscience claims.
- Standard geometric machinery first; novelty budget goes to relational interpretation, semantic affordances, energy, memory, Crew negotiation, and consequence.

### AUTO KEEP + MEASURE

- bounded snap-capture windows;
- path-corridor/local-steering compatibility;
- shared abstract steering vocabulary with body-specific realization;
- selective rigid-body/contact physics on materially consequential objects;
- object/surface/volume local state plus sparse fields for material systems;
- local reroute before wider reroute;
- Priority Hotspots as both attention and computation regulators.

### QUESTION SUBROUTINE

Current mid/high-uncertainty topics:

1. humanoid-to-dynamic-object force coupling;
2. continuous versus hotspot-activated material-field simulation;
3. snap-capture strength/permissiveness and failure behavior.

These should be decided before broad implementation beyond bounded prototypes.
