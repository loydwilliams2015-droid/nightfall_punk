#include "nf_relations.h"

NfRelationship nf_relation_between(NfFaction a, NfFaction b, NfRelationship human_rival_relation) {
    if (a == NF_FACTION_NONE || b == NF_FACTION_NONE) return NF_RELATION_NEUTRAL;
    if (a == b) return NF_RELATION_COOPERATIVE;
    if (a == NF_FACTION_RANCHER || b == NF_FACTION_RANCHER) return NF_RELATION_NONNEGOTIABLE_HOSTILE;
    if ((a == NF_FACTION_PLAYER && b == NF_FACTION_TEAMMATE) ||
        (a == NF_FACTION_TEAMMATE && b == NF_FACTION_PLAYER)) return NF_RELATION_COOPERATIVE;
    if ((a == NF_FACTION_RIVAL && (b == NF_FACTION_PLAYER || b == NF_FACTION_TEAMMATE)) ||
        (b == NF_FACTION_RIVAL && (a == NF_FACTION_PLAYER || a == NF_FACTION_TEAMMATE))) return human_rival_relation;
    return NF_RELATION_NEUTRAL;
}

bool nf_relation_can_damage(NfRelationship relation, bool friendly_fire) {
    if (friendly_fire) return relation != NF_RELATION_NEUTRAL;
    return relation == NF_RELATION_HOSTILE || relation == NF_RELATION_NONNEGOTIABLE_HOSTILE;
}

bool nf_relationship_is_negotiable(NfRelationship relation) {
    return relation != NF_RELATION_NONNEGOTIABLE_HOSTILE;
}

NfRancherProfile nf_rancher_profile_from_roll(uint32_t roll, uint32_t nonpredatory_threshold, uint32_t scale) {
    if (scale == 0u) scale = 1u;
    if (nonpredatory_threshold > scale) nonpredatory_threshold = scale;
    const bool rare_nonpredatory = (roll % scale) < nonpredatory_threshold;
    NfRancherProfile profile = {
        .negotiation = NF_NEGOTIATION_NEVER,
        .disposition = rare_nonpredatory ? NF_RANCHER_HOSTILE_NONPREDATORY : NF_RANCHER_PREDATORY,
        .predation_drive = rare_nonpredatory ? 0.05f : 0.90f,
        .territorial_drive = rare_nonpredatory ? 0.72f : 0.62f,
        .threat_sensitivity = rare_nonpredatory ? 0.78f : 0.70f,
        .pursuit_persistence = rare_nonpredatory ? 0.22f : 0.82f,
        .avoidance_preference = rare_nonpredatory ? 0.82f : 0.18f
    };
    return profile;
}

const char *nf_relationship_name(NfRelationship relation) {
    switch (relation) {
        case NF_RELATION_NEUTRAL: return "NEUTRAL";
        case NF_RELATION_COOPERATIVE: return "COOPERATIVE";
        case NF_RELATION_TRUCE: return "TRUCE";
        case NF_RELATION_CONTESTED: return "CONTESTED";
        case NF_RELATION_HOSTILE: return "HOSTILE";
        case NF_RELATION_NONNEGOTIABLE_HOSTILE: return "NONNEGOTIABLE_HOSTILE";
        default: return "UNKNOWN";
    }
}
