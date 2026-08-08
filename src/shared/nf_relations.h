#ifndef NF_RELATIONS_H
#define NF_RELATIONS_H

#include "nf_world.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum NfRelationship {
    NF_RELATION_NEUTRAL = 0,
    NF_RELATION_COOPERATIVE,
    NF_RELATION_TRUCE,
    NF_RELATION_CONTESTED,
    NF_RELATION_HOSTILE,
    NF_RELATION_NONNEGOTIABLE_HOSTILE
} NfRelationship;

typedef enum NfNegotiationPolicy {
    NF_NEGOTIATION_ALLOWED = 0,
    NF_NEGOTIATION_NEVER
} NfNegotiationPolicy;

typedef enum NfRancherDisposition {
    NF_RANCHER_PREDATORY = 0,
    NF_RANCHER_HOSTILE_NONPREDATORY
} NfRancherDisposition;

typedef struct NfRancherProfile {
    NfNegotiationPolicy negotiation;
    NfRancherDisposition disposition;
    float predation_drive;
    float territorial_drive;
    float threat_sensitivity;
    float pursuit_persistence;
    float avoidance_preference;
} NfRancherProfile;

NfRelationship nf_relation_between(NfFaction a, NfFaction b, NfRelationship human_rival_relation);
bool nf_relation_can_damage(NfRelationship relation, bool friendly_fire);
bool nf_relationship_is_negotiable(NfRelationship relation);
NfRancherProfile nf_rancher_profile_from_roll(uint32_t roll, uint32_t nonpredatory_threshold, uint32_t scale);
const char *nf_relationship_name(NfRelationship relation);

#endif
