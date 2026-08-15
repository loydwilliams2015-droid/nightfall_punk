#include "nf_route.h"

#include "nf_contamination.h"

#include <math.h>
#include <string.h>

static float distance_xz(NfVec3 a, NfVec3 b) {
    const float dx = a.x-b.x;
    const float dz = a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

static void place_gate(NfRouteSystem *route, NfWorld *world, bool open) {
    if (route == NULL || world == NULL || route->gate_collider < 0 ||
        (size_t)route->gate_collider >= world->collider_count) return;
    NfCollider *gate = &world->colliders[route->gate_collider];
    gate->min = open ? route->open_min : route->closed_min;
    gate->max = open ? route->open_max : route->closed_max;
    gate->previous_min = gate->min;
    gate->previous_max = gate->max;
    gate->base_min = gate->min;
    gate->base_max = gate->max;
}

void nf_route_init_v12_slice(NfRouteSystem *route, NfWorld *world) {
    if (route == NULL || world == NULL) return;
    memset(route,0,sizeof(*route));
    route->gate_key = NF_ROUTE_GATE_KEY;
    route->revision = 1u;
    route->gate_collider = -1;
    route->closed_min = (NfVec3){-6.0f,0.0f,4.0f};
    route->closed_max = (NfVec3){6.0f,2.2f,5.0f};
    route->open_min = (NfVec3){8.0f,0.0f,4.0f};
    route->open_max = (NfVec3){20.0f,2.2f,5.0f};
    route->gate_center = (NfVec3){0.0f,1.1f,4.5f};
    route->route_a_entry = (NfVec3){-13.0f,0.05f,-2.0f};
    route->route_a_reconnect = (NfVec3){-13.0f,0.05f,18.0f};
    route->route_b_entry = (NfVec3){0.0f,0.05f,1.2f};
    route->route_b_reconnect = (NfVec3){0.0f,0.05f,18.0f};
    route->goal = (NfVec3){0.0f,0.05f,29.0f};

    route->gate_collider = nf_world_add_collider(
        world,NF_COLLIDER_SOLID,route->closed_min,route->closed_max);
    if (route->gate_collider < 0) return;

    /* 80x80 m causal sub-slice: relational density, not decorative density. */
    nf_world_add_collider(world,NF_COLLIDER_SOLID,(NfVec3){-15.0f,0.0f,-7.0f},(NfVec3){-11.0f,1.15f,-4.5f});
    nf_world_add_collider(world,NF_COLLIDER_SOLID,(NfVec3){-15.0f,0.0f,6.5f},(NfVec3){-11.0f,1.45f,9.0f});
    nf_world_add_collider(world,NF_COLLIDER_SOLID,(NfVec3){-15.0f,0.0f,18.0f},(NfVec3){-11.0f,1.05f,21.0f});
    nf_world_add_collider(world,NF_COLLIDER_SOLID,(NfVec3){12.0f,2.0f,16.0f},(NfVec3){20.0f,2.35f,24.0f});
    nf_world_add_ramp(world,(NfVec3){7.0f,0.0f,16.0f},(NfVec3){12.0f,2.0f,24.0f},NF_RAMP_POS_X);

    route->configured = true;
    place_gate(route,world,false);
}

bool nf_route_try_alter(
    NfRouteSystem *route, NfWorld *world, NfEntityId actor_id) {
    if (route == NULL || world == NULL || !route->configured || route->open) return false;
    NfActor *actor = nf_world_find_actor(world,actor_id);
    if (actor == NULL || !actor->active || !actor->combat.alive ||
        actor->faction != NF_FACTION_PLAYER || !actor->input.interact_held) return false;
    if (distance_xz(actor->transform.position,route->gate_center) > NF_V12_ROUTE_INTERACT_RADIUS) return false;
    if (nf_contamination_manipulator_scale(actor) < 0.35f) return false;

    route->open = true;
    ++route->revision;
    if (route->revision == 0u) route->revision = 1u;
    route->changed_tick = world->tick;
    route->changed_by = actor_id;
    place_gate(route,world,true);
    return true;
}

void nf_route_apply_replica(
    NfRouteSystem *route, NfWorld *world, bool open,
    uint64_t changed_tick, NfEntityId changed_by) {
    if (route == NULL || world == NULL || !route->configured) return;
    if (route->open != open) {
        place_gate(route,world,open);
        ++route->revision;
        if (route->revision == 0u) route->revision = 1u;
    }
    route->open = open;
    route->changed_tick = changed_tick;
    route->changed_by = changed_by;
}

float nf_route_distance_to_gate(const NfRouteSystem *route, NfVec3 position) {
    if (route == NULL || !route->configured) return 1000000.0f;
    return distance_xz(position,route->gate_center);
}

const char *nf_route_state_name(const NfRouteSystem *route) {
    return route != NULL && route->configured && route->open ? "OPEN" : "CLOSED";
}
