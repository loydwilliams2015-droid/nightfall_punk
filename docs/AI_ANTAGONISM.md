# nightfall!punk — Antagonism Logic

## Core distinction

Human Rivals and Nightmare Ranchers are different kinds of antagonists and must not share the same motivational policy.

### Human Rivals

- Secondary antagonism.
- Temporary terrain antagonism.
- Social/tactical actors.
- Negotiation is possible.
- Relations may move among hostility, contest, truce, cooperation, and renewed conflict.
- Their conflict is contingent rather than structurally permanent.

### Nightmare Ranchers

- Primary antagonism.
- Semi-permanent environmental antagonism.
- Non-negotiable in all cases.
- Their relationship model must never expose truce, bargaining, alliance, surrender, or social reconciliation states.
- Their behavioral disposition is usually predatory, but **predatory** and **non-negotiable** are separate dimensions.

## Rancher disposition rule

A Rancher normally receives a predatory/non-negotiable disposition: it treats player, Human Rival, and other valid actors as prey/threat/resource pressure and may actively stalk, pursue, isolate, attack, or contest habitat.

A rare seeded statistical disposition may instead be **hostile/non-negotiable but non-predatory**.

This rare Rancher:

- remains hostile;
- cannot be negotiated with;
- does not become friendly, neutral, cooperative, or socially receptive;
- is not required to actively hunt a valid target merely because that target exists;
- may prefer distance, territorial avoidance, observation, withdrawal, route denial, or defensive hostility;
- can therefore permit an **avoidant detente** to emerge behaviorally when both sides maintain separation;
- will escalate if its threat, territorial, resource, proximity, damage, or habitat thresholds are crossed;
- must never signal that this detente is a negotiated agreement.

The avoidant detente is an emergent equilibrium, not diplomacy.

## Game-logic axes

Rancher logic should keep at least these axes independent:

- `negotiation_policy = NEVER`
- `predation_drive`
- `territorial_drive`
- `threat_sensitivity`
- `resource_pressure`
- `pursuit_persistence`
- `avoidance_preference`
- `aggression_threshold`

This prevents `hostile` from becoming synonymous with `always pursue`.

A future seeded profile can therefore produce distributions such as:

- strongly predatory / strongly territorial;
- predatory / roaming;
- ambush-predatory / low pursuit persistence;
- hostile / non-predatory / territorial;
- hostile / non-predatory / avoidant.

Exact probabilities remain a tuning parameter and should not be hard-coded until Rancher behavior is playtested.

## Architectural consequence

Human Rivals and Nightmare Ranchers may share lower-level systems—world semantics, perception, memory, navigation, affordances, Fuzzy Rail, combat, and spatial pressure fields—but must use different motivational policies.

Human Rival policy asks questions such as:

- Is conflict advantageous?
- Can we negotiate?
- Should we contest, cooperate, betray, retreat, or disengage?

Rancher policy asks questions such as:

- Is this actor prey, threat, pressure, or habitat intrusion?
- Should I stalk, attack, deny, observe, avoid, withdraw, or defend territory?

The rare hostile/non-predatory Rancher is therefore not a friendly exception. It is a non-negotiable environmental antagonist whose locally stable solution may be mutual avoidance.
