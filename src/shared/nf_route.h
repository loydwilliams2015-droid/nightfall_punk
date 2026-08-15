#ifndef NF_ROUTE_H
#define NF_ROUTE_H

#include "nf_world.h"

#include <stdbool.h>
#include <stdint.h>

#define NF_V12_SLICE_HALF_EXTENT 40.0f
#define NF_V12_ROUTE_INTERACT_RADIUS 3.4f
#define NF_ROUTE_GATE_KEY 0x140001u

typedef struct NfRouteSystem {
    bool configured;
    bool open;
    uint32_t gate_key;
    uint32_t revision;
    int gate_collider;
    NfVec3 closed_min;
    NfVec3 closed_max;
    NfVec3 open_min;
    NfVec3 open_max;
    NfVec3 gate_center;
    NfVec3 route_a_entry;
    NfVec3 route_a_reconnect;
    NfVec3 route_b_entry;
    NfVec3 route_b_reconnect;
    NfVec3 goal;
    uint64_t changed_tick;
    NfEntityId changed_by;
} NfRouteSystem;

void nf_route_init_v12_slice(NfRouteSystem *route, NfWorld *world);
bool nf_route_try_alter(NfRouteSystem *route, NfWorld *world, NfEntityId actor_id);
void nf_route_apply_replica(NfRouteSystem *route, NfWorld *world, bool open, uint64_t changed_tick, NfEntityId changed_by);
float nf_route_distance_to_gate(const NfRouteSystem *route, NfVec3 position);
const char *nf_route_state_name(const NfRouteSystem *route);

#endif
