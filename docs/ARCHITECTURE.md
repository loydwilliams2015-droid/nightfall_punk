# nightfall!punk architecture contract

## Durable stack

- C + raylib
- Linux-first
- fixed 60 Hz simulation
- authoritative dedicated server as eventual gameplay truth
- data-oriented ECS-lite direction
- presentation observes simulation; it does not own gameplay state

## v0.2 movement contract

The player camera is presentation. The actor body is simulation.

Movement input is expressed as local forward/strafe intent plus yaw and action buttons. This shape is intentionally suitable for later network command serialization.

The movement state machine currently exposes GROUND, SPRINT, CROUCH, AIR, LADDER, VAULT, MANTLE and PLATFORM.

## Fuzzy Rail

Fuzzy Rail is local geometric movement intelligence, not a global AI route. It detects nearby steps, edges, vaults, mantles and ladders and scores possible local transitions from distance + approach alignment. Candidate retention uses hysteresis so a usable edge feels crisp without becoming a hard rail.

Constraints:
1. It never chooses the global destination.
2. It only assists the next local geometric transition.
3. Player and AI should query the same physical affordances.
4. Assistance is bounded and momentum-preserving.
5. Multiple routes remain viable so AI does not carve deterministic ruts.

## Dynamic Affordance Graph

This is a later semantic/tactical layer and is distinct from Fuzzy Rail.

Planned composition: world affordances -> influence/pressure fields -> faction-specific utility -> squad blackboard -> desired action/destination -> Fuzzy Rail local traversal -> physics authority.

## Scope discipline

v0.2 does not implement network transport, combat AI, round objectives, or aesthetic assets. Those systems may rely on movement later; movement may not depend on them.
