#ifndef NF_LIFEWORLD_SPATIAL_H
#define NF_LIFEWORLD_SPATIAL_H

#include "nf_ai.h"
#include "nf_lifeworld.h"
#include "nf_spatial.h"
#include "nf_world.h"

void nf_lifeworld_apply_attention_bias(
    NfSpatialSystem *spatial,
    const NfLifeworldSystem *lifeworld,
    const NfAiSystem *ai,
    const NfWorld *world);

#endif
