#include "nf_spatial.h"

#include "nf_ai.h"

#include <math.h>

static float distance_xz(NfVec3 a, NfVec3 b) {
    const float dx = a.x-b.x;
    const float dz = a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

void nf_spatial_filter_controls(
    const NfSpatialSystem *spatial,
    const struct NfAiSystem *ai,
    const NfWorld *world,
    NfControlFrame *controls,
    size_t control_count) {
    if (spatial == NULL || ai == NULL || world == NULL || controls == NULL) return;

    for (size_t i = 0u; i < control_count; ++i) {
        const NfSpatialAgentState *state =
            nf_spatial_agent_state_const(spatial,controls[i].actor);
        const NfAiAgent *agent = nf_ai_find_agent_const(ai,controls[i].actor);
        const NfActor *body = nf_world_find_actor_const(world,controls[i].actor);
        if (state == NULL || agent == NULL || body == NULL || !body->combat.alive ||
            !state->local_goal_valid) {
            continue;
        }

        const bool spatially_free = agent->mode == NF_AGENT_IDLE ||
            agent->mode == NF_AGENT_TRUCE_HOLD ||
            (agent->mode == NF_AGENT_INVESTIGATE && agent->knowledge.confidence < 0.20f);
        if (!spatially_free) continue;

        const float distance = distance_xz(body->transform.position,state->local_goal);
        if (distance <= 1.8f) {
            controls[i].move.forward = 0.0f;
            controls[i].move.strafe = 0.0f;
            controls[i].move.sprint_held = false;
            continue;
        }

        const float yaw = atan2f(
            state->local_goal.x-body->transform.position.x,
            state->local_goal.z-body->transform.position.z);
        controls[i].move.yaw_radians = yaw;
        controls[i].move.forward = state->task == NF_SPATIAL_TASK_WATCH ? 0.55f : 0.90f;
        controls[i].move.strafe = 0.0f;
        controls[i].move.sprint_held = distance > 16.0f;

        if (body->movement.candidate.active) {
            if (body->movement.candidate.type == NF_TRAVERSAL_LADDER) {
                controls[i].move.interact_held = true;
            } else if (body->movement.candidate.type == NF_TRAVERSAL_VAULT ||
                       body->movement.candidate.type == NF_TRAVERSAL_MANTLE) {
                controls[i].move.jump_pressed = true;
            }
        }
    }
}
