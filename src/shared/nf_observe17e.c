#include "nf_observe17e.h"

#include <string.h>

static uint32_t mix32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

static uint32_t dimension_bit(uint8_t dimension) {
    return dimension < (uint8_t)NF17E_DIM_COUNT ? (1u << dimension) : 0u;
}

void nf17e_overlay_policy_default(Nf17eOverlayPolicy *policy) {
    if (policy == NULL) return;
    memset(policy, 0, sizeof(*policy));

    policy->base_priority[NF17E_DIM_WORLD] = 10u;
    policy->base_priority[NF17E_DIM_MATERIAL] = 20u;
    policy->base_priority[NF17E_DIM_EVIDENCE] = 30u;
    policy->base_priority[NF17E_DIM_BELIEF] = 35u;
    policy->base_priority[NF17E_DIM_CONFIDENCE] = 32u;
    policy->base_priority[NF17E_DIM_CONTRACT] = 40u;
    policy->base_priority[NF17E_DIM_TRANSACTION] = 50u;
    policy->base_priority[NF17E_DIM_CONFLICT] = 60u;
    policy->base_priority[NF17E_DIM_PURPLE] = 70u;
    policy->base_priority[NF17E_DIM_FRONTIER] = 65u;
    policy->base_priority[NF17E_DIM_REFINEMENT] = 45u;
    policy->base_priority[NF17E_DIM_PENDING] = 80u;
    policy->base_priority[NF17E_DIM_INVARIANT] = 90u;

    policy->equal_priority_compounds = 1u;
    policy->preserve_suppressed_metadata = 1u;
    policy->pending_priority_boost = 1u;
    policy->invariant_priority_boost = 1u;
}

uint16_t nf17e_effective_priority(
    const Nf17eOverlayPolicy *policy,
    const Nf17eOverlayContribution *contribution) {

    if (policy == NULL || contribution == NULL ||
        contribution->dimension >= (uint8_t)NF17E_DIM_COUNT) return 0u;

    uint32_t priority =
        (uint32_t)policy->base_priority[contribution->dimension] +
        (uint32_t)contribution->priority;

    if (contribution->pending != 0u && policy->pending_priority_boost != 0u) {
        priority += 32u;
    }

    if (contribution->dimension == (uint8_t)NF17E_DIM_INVARIANT &&
        policy->invariant_priority_boost != 0u) {
        priority += 64u;
    }

    return priority > UINT16_MAX ? UINT16_MAX : (uint16_t)priority;
}

static bool same_anchor(
    const Nf17eOverlayObject *object,
    const Nf17eOverlayContribution *contribution) {

    return object->anchor_id == contribution->anchor_id &&
           object->anchor_kind == contribution->anchor_kind;
}

static void add_contribution(
    const Nf17eOverlayPolicy *policy,
    Nf17eOverlayObject *object,
    const Nf17eOverlayContribution *contribution) {

    const uint32_t bit = dimension_bit(contribution->dimension);
    const uint16_t priority = nf17e_effective_priority(policy, contribution);

    object->dimension_mask |= bit;
    object->authority_hash ^= mix32(contribution->authority_hash + contribution->source_id);
    object->composite_signature ^= mix32(
        contribution->payload_hash ^
        ((uint32_t)contribution->dimension << 24) ^
        ((uint32_t)priority << 8) ^
        contribution->source_id);
    ++object->source_count;

    if (contribution->pending != 0u) object->has_pending = 1u;
    if (contribution->committed != 0u) object->has_committed = 1u;

    if (object->visible_primary_mask == 0u || priority > object->highest_priority) {
        if (policy->preserve_suppressed_metadata != 0u) {
            object->suppressed_mask |= object->visible_primary_mask;
        }
        object->visible_primary_mask = bit;
        object->highest_priority = priority;
    } else if (priority == object->highest_priority &&
               policy->equal_priority_compounds != 0u) {
        object->visible_primary_mask |= bit;
    } else if (policy->preserve_suppressed_metadata != 0u) {
        object->suppressed_mask |= bit;
    }
}

static uint8_t classify_composite(const Nf17eOverlayObject *object) {
    if (object == NULL || object->source_count <= 1u) return (uint8_t)NF17E_COMPOSITE_SINGLE;

    const bool multi_primary =
        object->visible_primary_mask != 0u &&
        (object->visible_primary_mask & (object->visible_primary_mask - 1u)) != 0u;

    const bool many_dimensions =
        object->dimension_mask != 0u &&
        (object->dimension_mask & (object->dimension_mask - 1u)) != 0u;

    if (multi_primary && object->suppressed_mask != 0u) return (uint8_t)NF17E_COMPOSITE_RAINBOW;
    if (multi_primary) return (uint8_t)NF17E_COMPOSITE_EQUAL_PRIORITY_MULTI;
    if (many_dimensions) return (uint8_t)NF17E_COMPOSITE_PRIORITY_SUPPRESSED;
    return (uint8_t)NF17E_COMPOSITE_SINGLE;
}

size_t nf17e_compose_overlays(
    const Nf17eOverlayPolicy *policy,
    const Nf17eOverlayContribution *contributions,
    size_t contribution_count,
    Nf17eOverlayObject *out,
    size_t out_capacity) {

    if (policy == NULL || contributions == NULL || contribution_count == 0u) return 0u;
    if (contribution_count > NF17E_MAX_OVERLAY_CONTRIBUTIONS) {
        contribution_count = NF17E_MAX_OVERLAY_CONTRIBUTIONS;
    }

    Nf17eOverlayObject objects[NF17E_MAX_OVERLAY_OBJECTS];
    memset(objects, 0, sizeof(objects));
    size_t object_count = 0u;

    for (size_t i = 0u; i < contribution_count; ++i) {
        const Nf17eOverlayContribution *c = &contributions[i];
        if (c->dimension >= (uint8_t)NF17E_DIM_COUNT) continue;

        size_t index = object_count;
        for (size_t j = 0u; j < object_count; ++j) {
            if (same_anchor(&objects[j], c)) {
                index = j;
                break;
            }
        }

        if (index == object_count) {
            if (object_count >= NF17E_MAX_OVERLAY_OBJECTS) break;
            memset(&objects[object_count], 0, sizeof(objects[object_count]));
            objects[object_count].anchor_id = c->anchor_id;
            objects[object_count].anchor_kind = c->anchor_kind;
            ++object_count;
        }

        add_contribution(policy, &objects[index], c);
    }

    for (size_t i = 0u; i < object_count; ++i) {
        objects[i].composite_kind = classify_composite(&objects[i]);
        if (out != NULL && i < out_capacity) out[i] = objects[i];
    }

    return object_count;
}

bool nf17e_overlay_object_contains(
    const Nf17eOverlayObject *object,
    Nf17eOverlayDimension dimension) {

    if (object == NULL || dimension < 0 || dimension >= NF17E_DIM_COUNT) return false;
    return (object->dimension_mask & (1u << dimension)) != 0u;
}

bool nf17e_overlay_object_primary(
    const Nf17eOverlayObject *object,
    Nf17eOverlayDimension dimension) {

    if (object == NULL || dimension < 0 || dimension >= NF17E_DIM_COUNT) return false;
    return (object->visible_primary_mask & (1u << dimension)) != 0u;
}

const char *nf17e_overlay_dimension_name(Nf17eOverlayDimension dimension) {
    static const char *names[] = {
        "world", "material", "evidence", "belief", "confidence",
        "contract", "transaction", "conflict", "purple", "frontier",
        "refinement", "pending", "invariant"
    };
    return dimension >= NF17E_DIM_WORLD && dimension < NF17E_DIM_COUNT
        ? names[dimension] : "unknown";
}

const char *nf17e_composite_kind_name(Nf17eCompositeKind kind) {
    static const char *names[] = {
        "single", "priority-suppressed", "equal-priority-multi", "rainbow"
    };
    return kind >= NF17E_COMPOSITE_SINGLE && kind <= NF17E_COMPOSITE_RAINBOW
        ? names[kind] : "unknown";
}
