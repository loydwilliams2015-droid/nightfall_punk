# nightfall!punk — v1.0 Evolutionary Strategy Persistence Addendum

Status: **accepted design insight / pre-implementation**

This addendum extends the v1.0 Lifeworld/parametric contracts. It does **not** authorize a compile.

## Governing evolutionary rule

> **Natural selection is modeled as the differential persistence and reproduction of heritable strategy-generating capacities across generations, not as a fixed biological script for behavior.**

A strategy is viable when the actors carrying its heritable reaction rules persist and reproduce sufficiently for those rules to remain or increase in the population under the current ecological game. Immediate damage, dominance, resource capture, or short-term survival alone are not fitness.

Preferred causal loop:

`heritable reaction norms -> development + environment + history -> plastic phenotype -> situated behavior -> ecological/social game -> lifetime consequences -> differential persistence/reproduction -> changed population -> changed game`

## Anti-essentialist phenotypic plasticity

- Do not encode aggression, cooperation, selfishness, altruism, retreat, or violence as immutable actor essences.
- Encode **constraints, sensitivities, reaction norms, plasticity and learning parameters** that generate context-sensitive phenotypes and behavior.
- The same inherited configuration may produce different realized phenotypes under different developmental, ecological and social histories.
- Plasticity is itself conditional and costly; more plasticity is not automatically more adaptive.

Conceptual form:

```c
phenotype = develop(genotype, developmental_history, environment);
behavior  = react(phenotype, body_state, social_state, ecology, memory);
```

## Evolutionary game-theory rule

Fitness is relational and frequency-dependent:

```c
fitness = payoff(expressed_behavior,
                 population_state,
                 ecology,
                 interaction_history);
```

Do **not** treat fitness as an intrinsic scalar stored in the genome. A strategy may prosper when rare, decline when common, or coexist with competitors because changing strategy frequencies alter the payoff landscape.

Therefore:

`successful strategy -> becomes more represented -> changes the game -> changes its own and others' payoffs`

There is no universal requirement that aggression, cooperation, defection, or restraint be globally optimal.

## Persistence and reproduction of strategies

For the Lifeworld, the relevant evolutionary criterion is:

> **Does this strategy-generating configuration sustain viable carriers and reproduce its behavioral potential across generations under the ecology it helps create?**

Evaluate long-run consequence accounts such as:

- energy maintenance and surplus;
- survival and injury burden;
- reproductive opportunity and offspring viability;
- reciprocal support and coalition value;
- territory/resource quality and depletion;
- retaliation, conflict and opportunity costs;
- environmental changes created by the strategy itself.

Violence and cooperation are therefore **conditional tactics**, not terminal evolutionary goals. Selection acts on the consequences of situated behavior, not on moral labels or actor intentions.

## Implementation invariant

```c
for (generation = 0; generation < GENERATIONS; ++generation) {
    develop_plastic_phenotypes();
    run_ecological_game();
    accumulate_lifetime_consequences();
    reproduce_weighted_by_realized_fitness();
    recombine_and_mutate_reaction_norms();
    update_population_ecology(); /* new population = new game */
}
```

Keep the distinction explicit:

`genes constrain possibilities; development expresses possibilities; behavior plays the game; consequences determine differential persistence/reproduction; population change changes the next game.`

This contract is compatible with the existing parametric rule: prefer observable relations and accumulated consequences over fixed behavioral outcome constants.