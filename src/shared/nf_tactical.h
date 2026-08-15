#ifndef NF_TACTICAL_H
#define NF_TACTICAL_H

#include "nf_world.h"

float nf_tactical_exposure(const NfWorld *world, NfVec3 threat, NfVec3 candidate);
float nf_tactical_cover_utility(float exposure, float travel_m, float option_distance_m);

#endif
