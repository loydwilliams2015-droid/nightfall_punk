#ifndef NF_MOVEMENT_H
#define NF_MOVEMENT_H

#include "nf_world.h"

void nf_movement_step_actor(NfWorld *world, NfActor *actor, float dt);
void nf_movement_update_candidate(const NfWorld *world, NfActor *actor);
bool nf_movement_space_is_free(
    const NfWorld *world, NfVec3 feet_position, float radius, float height);

#endif
