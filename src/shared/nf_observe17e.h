#ifndef NF_OBSERVE17E_H
#define NF_OBSERVE17E_H

#include "nf_cleave17d.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF17E_MAX_OVERLAY_CONTRIBUTIONS 32u
#define NF17E_MAX_OVERLAY_OBJECTS 32u

typedef enum Nf17eOverlayDimension {
    NF17E_DIM_WORLD = 0,
    NF17E_DIM_MATERIAL = 1,
    NF17E_DIM_EVIDENCE = 2,
    NF17E_DIM_BELIEF = 3,
    NF17E_DIM_CONFIDENCE = 4,
    NF17E_DIM_CONTRACT = 5,
    NF17E_DIM_TRANSACTION = 6,
    NF17E_DIM_CONFLICT = 7,
    NF17E_DIM_PURPLE = 8,
    NF17E_DIM_FRONTIER = 9,
    NF17E_DIM_REFINEMENT = 10,
    NF17E_DIM_PENDING = 11,
    NF17E_DIM_INVARIANT = 12,
    NF17E_DIM_COUNT = 13
} Nf17eOverlayDimension;

typedef enum Nf17eCompositeKind {
    NF17E_COMPOSITE_SINGLE = 0,
    NF17E_COMPOSITE_PRIORITY_SUPPRESSED = 1,
    NF17E_COMPOSITE_EQUAL_PRIORITY_MULTI = 2,
    NF17E_COMPOSITE_RAINBOW = 3
} Nf17eCompositeKind;

typedef enum Nf17eAnchorKind {
    NF17E_ANCHOR_OBJECT = 0,
    NF17E_ANCHOR_CELL = 1,
    NF17E_ANCHOR_NEXUS = 2,
    NF17E_ANCHOR_BOUNDARY = 3,
    NF17E_ANCHOR_REGION = 4
} Nf17eAnchorKind;

typedef struct Nf17eOverlayContribution {
    uint32_t anchor_id;
    uint32_t source_id;
    uint32_t authority_hash;
    uint32_t payload_hash;
    uint16_t priority;
    uint8_t anchor_kind;
    uint8_t dimension;
    uint8_t committed;
    uint8_t pending;
    uint16_t reserved;
} Nf17eOverlayContribution;

typedef struct Nf17eOverlayObject {
    uint32_t anchor_id;
    uint32_t authority_hash;
    uint32_t dimension_mask;
    uint32_t visible_primary_mask;
    uint32_t suppressed_mask;
    uint32_t composite_signature;
    uint16_t highest_priority;
    uint16_t source_count;
    uint8_t anchor_kind;
    uint8_t composite_kind;
    uint8_t has_pending;
    uint8_t has_committed;
} Nf17eOverlayObject;

typedef struct Nf17eOverlayPolicy {
    uint16_t base_priority[NF17E_DIM_COUNT];
    uint8_t equal_priority_compounds;
    uint8_t preserve_suppressed_metadata;
    uint8_t pending_priority_boost;
    uint8_t invariant_priority_boost;
} Nf17eOverlayPolicy;

void nf17e_overlay_policy_default(Nf17eOverlayPolicy *policy);

uint16_t nf17e_effective_priority(
    const Nf17eOverlayPolicy *policy,
    const Nf17eOverlayContribution *contribution);

size_t nf17e_compose_overlays(
    const Nf17eOverlayPolicy *policy,
    const Nf17eOverlayContribution *contributions,
    size_t contribution_count,
    Nf17eOverlayObject *out,
    size_t out_capacity);

bool nf17e_overlay_object_contains(
    const Nf17eOverlayObject *object,
    Nf17eOverlayDimension dimension);

bool nf17e_overlay_object_primary(
    const Nf17eOverlayObject *object,
    Nf17eOverlayDimension dimension);

const char *nf17e_overlay_dimension_name(Nf17eOverlayDimension dimension);
const char *nf17e_composite_kind_name(Nf17eCompositeKind kind);

#endif
