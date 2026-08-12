# nightfall!punk ledger addendum — C34

## C34 — non-exclusive semantic affordance game / systemic-physics locomotion — LOCKED

### STEWARD — canonical contract

Core formulation:

> **nightfall!punk uses semantic high-authority arcade locomotion with smoothed affordance capture inside a world whose physics priority is Oxygen Not Included first and Exanima second. Intentions and actions are bounded and biased by affordances, but affordance capture is not exclusive: viable alternatives, aborts, combinations, disturbances and emergent physical outcomes remain possible.**

Physics hierarchy:

1. **ONI-first world systemics** — material state, energy, temperature, fluids/gases where modeled, surfaces, infrastructure, damage, environment and persistent causal state determine the physical opportunity landscape.
2. **Exanima-second embodiment** — actors and objects possess momentum, collision, mass, contact, balance/impulse consequences and recoverable physical disturbance.
3. **High-authority semantic locomotion** — player/AI intention is interpreted generously and immediately; movement assistance serves intention rather than replacing it.
4. **Smoothed affordance capture** — nearby valid ladders, mantles, vaults, cover positions, landings and other traversal relations exert bounded guidance/alignment rather than binary teleporting or universal hard-locking.
5. **Non-exclusive strategy space** — capture raises the value/probability of a coherent action but normally does not erase other feasible actions. Hard exclusion is reserved for genuine physical/logical impossibility.
6. **Authoritative consequence** — the actor chooses locally from situated belief; the server/world resolves globally. Chosen action and realized outcome are distinct.

Game-theoretic loop:

```text
BELIEF + INTENTION + LOCAL WORLD STATE
                ↓
      BOUNDED STRATEGY SET
                ↓
AFFORDANCE CAPTURE REWEIGHTS PAYOFFS
      (does not normally delete alternatives)
                ↓
PLAYER AUTHORITY / AI MIXED STRATEGY
                ↓
       PHYSICAL EXECUTION
                ↓
ONI-LIKE WORLD + EXANIMA-LIKE BODY RESOLUTION
                ↓
OUTCOME / PAYOFF / NEW EVIDENCE / NEW WORLD STATE
                ↺
```

Locked rules:

- [x] **intent is preference, not prophecy.**
- [x] **affordance is an opportunity relation, not an exclusive animation state.**
- [x] **capture is a bounded payoff/control bias, not ownership of the actor.**
- [x] retain an alternative action set while captured whenever those actions remain physically viable.
- [x] allow abort/release, counter-steer, jump-off, interruption, knockback, collision, route substitution and other legitimate departures.
- [x] player locomotion remains high-authority and deterministic from input; do not randomize player intent to imitate AI mixed strategy.
- [x] AI may use a mixed/soft strategy over several viable actions to prevent brittle deterministic routing and permit game-theoretic adaptation.
- [x] physics may transform the realized outcome without retroactively changing what the actor intended.
- [x] semantic assistance must be capped in acceleration, angular correction, positional correction and capture time so the player can feel the physical world underneath it.
- [x] environmental state may change both feasibility and payoff: e.g. wetness, damage, heat, flow, obstruction, occupancy, pressure, exposure or material condition can alter traversal value.
- [x] shared affordance machinery may be used by player and AI, but player input and AI strategy selection remain different decision sources.
- [x] server truth remains authoritative; client prediction may predict the same semantic capture/controller rules and reconcile to authoritative physical outcomes.

Maxims:

- **Crisp intention; plural action; physical consequence.**
- **Affordances bound the game without solving it.**
- **Capture attracts; it does not possess.**
- **Only impossibility should make an alternative impossible.**
- **The actor selects a strategy; the world selects the consequence.**
- **ONI supplies the systemic world; Exanima supplies embodied consequence; arcade authority keeps intention legible.**

### PRAXIS — game-theoretic C translation

The locomotion problem is represented as a repeated situated game. The actor evaluates a bounded set of physically feasible strategies. Intent and semantic capture change expected payoff, while other actors and environmental state change interaction cost. Capture normally biases rather than deletes alternatives.

```c
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_ACTION_COUNT 10

typedef enum NfActionKind {
    NF_ACT_RUN = 0,
    NF_ACT_JUMP,
    NF_ACT_VAULT,
    NF_ACT_MANTLE,
    NF_ACT_LADDER,
    NF_ACT_SLIDE,
    NF_ACT_COVER,
    NF_ACT_BRAKE,
    NF_ACT_ABORT,
    NF_ACT_WAIT
} NfActionKind;

typedef struct NfStrategyOption {
    NfActionKind action;

    /* Bounded feasibility: 0 = physically/logically unavailable, 1 = clean. */
    float feasibility;

    /* Actor side. */
    float intent_fit;
    float continuity_value;
    float energy_delta;
    float survival_value;
    float positional_value;
    float information_value;
    float coordination_value;

    /* Affordance side: smooth capture is a bonus, not exclusivity. */
    float capture_fit;
    float alignment_cost;
    float switching_cost;

    /* Game/environment side. */
    float opponent_response;
    float environmental_risk;
    float uncertainty_cost;

    bool hard_forbidden;
} NfStrategyOption;

typedef struct NfStrategySet {
    NfStrategyOption option[NF_ACTION_COUNT];
    size_t count;
} NfStrategySet;

typedef struct NfMixedStrategy {
    float p[NF_ACTION_COUNT];
} NfMixedStrategy;

static float nf_strategy_payoff(const NfStrategyOption *s)
{
    if (s->hard_forbidden || s->feasibility <= 0.0f)
        return -INFINITY;

    /* Weights are tuning data, not universal truth. */
    return 2.40f * s->intent_fit
         + 1.10f * s->capture_fit
         + 0.90f * s->continuity_value
         + 0.80f * s->energy_delta
         + 1.20f * s->survival_value
         + 0.80f * s->positional_value
         + 0.35f * s->information_value
         + 0.55f * s->coordination_value
         + 0.75f * s->opponent_response
         - 0.70f * s->alignment_cost
         - 0.45f * s->switching_cost
         - 1.00f * s->environmental_risk
         - 0.40f * s->uncertainty_cost;
}

/* AI only: bounded rationality / mixed strategy over viable actions. */
static NfMixedStrategy nf_ai_mixed_strategy(const NfStrategySet *set,
                                            float temperature)
{
    NfMixedStrategy mix = {0};
    float max_u = -INFINITY;
    float sum = 0.0f;

    if (temperature < 0.05f)
        temperature = 0.05f;

    for (size_t i = 0; i < set->count; ++i) {
        float u = nf_strategy_payoff(&set->option[i]);
        if (u > max_u)
            max_u = u;
    }

    for (size_t i = 0; i < set->count; ++i) {
        const NfStrategyOption *s = &set->option[i];
        if (s->hard_forbidden || s->feasibility <= 0.0f)
            continue;

        /* Feasibility bounds the strategy; capture merely reweights it. */
        float u = nf_strategy_payoff(s);
        float w = s->feasibility * expf((u - max_u) / temperature);
        mix.p[s->action] += w;
        sum += w;
    }

    if (sum > 0.0f) {
        for (size_t a = 0; a < NF_ACTION_COUNT; ++a)
            mix.p[a] /= sum;
    }

    return mix;
}
```

The important property is that `capture_fit` appears inside payoff while `hard_forbidden` alone removes a strategy. A ladder capture can strongly favor `NF_ACT_LADDER` while `NF_ACT_JUMP`, `NF_ACT_ABORT`, or a lateral move remain available when physically possible.

Player control uses the same affordance evaluation but does **not** sample a mixed strategy. The player's explicit command retains authority and semantic capture only adds bounded guidance:

```c
typedef struct NfCaptureGuidance {
    float confidence;          /* semantic certainty [0,1] */
    float max_linear_accel;    /* assistance cap */
    float max_angular_speed;   /* assistance cap */
    float release_threshold;   /* counter-intent needed to break capture */
} NfCaptureGuidance;

typedef struct NfControlIntent {
    float move_x;
    float move_y;
    bool jump;
    bool interact;
    bool abort;
} NfControlIntent;

void nf_resolve_high_authority_locomotion(
    NfActor *actor,
    const NfControlIntent *intent,
    const NfAffordanceQuery *affordances,
    const NfWorldPhysicsContext *physics,
    float dt)
{
    NfDesiredMotion desired = nf_motion_from_intent(actor, intent);

    NfAffordanceCandidate capture =
        nf_best_semantic_capture(actor, intent, affordances, physics);

    if (capture.valid) {
        NfCaptureGuidance g = nf_capture_guidance(actor, &capture, physics);

        /* Smoothly bias trajectory/orientation, never teleport. */
        nf_apply_bounded_capture(&desired, &capture, &g, dt);

        /* Explicit counter-intent/abort can release a non-mandatory capture. */
        if (intent->abort || nf_counter_intent(actor, intent, &capture) >
                             g.release_threshold)
            nf_release_capture(actor, &capture);
    }

    /* High-authority command enters embodied physics. */
    nf_body_drive_toward(actor, &desired, physics, dt);
}
```

The world then resolves the attempt. This keeps **intention**, **selected action**, and **outcome** separate:

```c
typedef struct NfActionOutcome {
    NfActionKind intended_action;
    NfActionKind executed_action;
    bool completed;
    bool interrupted;
    float energy_cost;
    float damage_cost;
    float positional_gain;
} NfActionOutcome;

NfActionOutcome nf_execute_strategy(
    NfWorld *world,
    NfActor *actor,
    NfActionKind intended,
    const NfControlIntent *control,
    float dt)
{
    /* ONI-first: authoritative systemic environment defines conditions. */
    NfWorldPhysicsContext physics =
        nf_world_physics_context(world, actor->position);

    /* Semantic/high-authority layer expresses intention within those conditions. */
    nf_resolve_high_authority_locomotion(
        actor, control, &world->affordances, &physics, dt);

    /* Exanima-second: embodied collision/momentum/contact resolve the attempt. */
    NfBodyResult body = nf_body_physics_step(world, actor, &physics, dt);

    /* Systemic exchanges feed consequences back into the persistent world. */
    nf_world_apply_actor_exchange(world, actor, &body, dt);

    return nf_outcome_from_body_result(intended, &body);
}
```

For AI, repeated-game adaptation occurs after the world resolves the action:

```c
void nf_actor_learn_from_outcome(NfActorBelief *belief,
                                 const NfActionOutcome *outcome)
{
    nf_belief_update_action_cost(belief,
                                 outcome->intended_action,
                                 outcome->energy_cost,
                                 outcome->damage_cost,
                                 outcome->completed,
                                 outcome->interrupted);
}
```

This produces the desired recurrent game:

```text
choose among several viable strategies
        ↓
semantic capture biases local execution
        ↓
physical world may confirm, perturb or defeat the attempt
        ↓
outcome changes energy / position / risk / evidence
        ↓
next strategy is revalued
```

### PRAXIS — implementation constraints

- Reuse existing **Fuzzy Rail** for local traversal intelligence; do not create a second global movement planner.
- Reuse **Dynamic Affordance Graph** and situated energy/belief infrastructure for strategy valuation.
- Player and Human Rival/Cattler controllers may share feasibility/capture queries but must not share omniscient strategy state.
- Prefer continuous `capture_fit` / bounded assistance over `if (near_rail) state = RAIL_LOCK` except where the physical relation genuinely requires a constraint.
- A hard constraint may exist while actually climbing/grinding/attached, but acquisition and release remain semantically forgiving and non-exclusive whenever physically plausible.
- Preserve fixed-step/server-authoritative prediction/reconciliation contracts.
- Do not compile merely to record this design contract; implementation belongs in the appropriate future movement/physics build.

### SIGNAL — comparative/communication frame

Use this wording when comparing the design externally or internally:

> **nightfall!punk aims for arcade-clear intention in a systemic physical world: Oxygen Not Included is the first reference for persistent environmental causation; Exanima is the second reference for embodied physical consequence; semantic movement systems such as Jet Set Radio / Mirror's Edge clarify how intention can be assisted without turning the world into canned traversal.**

SIGNAL guardrails:

- Do not describe the project as “physics-driven controls.” The **world is physics-rich; control remains high-authority**.
- Do not describe affordance snapping as an exclusive animation state machine. It is primarily a **smooth, bounded bias over several possible actions**.
- Do not imply ONI means reproducing every fluid/gas/thermal simulation at full fidelity everywhere. The governing lesson is **persistent systemic causation with appropriate simulation LOD**.
- Do not imply Exanima-like embodiment means intentionally cumbersome input. Borrow **consequence and embodiment**, not loss of legible player authority.
- Emphasize that player and AI inhabit the same causal world while differing in intention source, knowledge and strategy selection.

### STEWARD — regression tests / acceptance questions

- [ ] Does a near-valid traversal intention succeed generously without visible teleportation?
- [ ] Can the player counter-steer, abort or select another still-viable action during capture?
- [ ] Does capture preserve meaningful incoming momentum unless the physical affordance requires otherwise?
- [ ] Can environmental state change action cost/feasibility without directly dictating actor intention?
- [ ] Can physical collision or disturbance alter the realized outcome after a valid intention?
- [ ] Do AI actors retain more than one viable strategy when several are genuinely available?
- [ ] Does AI strategy use situated belief rather than authoritative future/world knowledge?
- [ ] Does the same physical event produce consistent authoritative consequences for player and AI?
- [ ] Can client prediction reproduce capture guidance and reconcile cleanly to server physical truth?
- [ ] Does the system remain readable and fun at ordinary FPS speed before deeper systemic effects are noticed?

Acceptance thesis:

> **The movement system succeeds when intention feels crisp, affordances feel generous, alternatives remain alive, and the systemic world still has the power to make consequences surprising.**
