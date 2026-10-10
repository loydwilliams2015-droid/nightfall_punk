#ifndef NF_ELIGIBILITY18B2_H
#define NF_ELIGIBILITY18B2_H
/* Clean 1.8B.2 reproduction, NOT the lost 1.8B.1 tested source.
 * Uses real A3 traversal geometry/contract authority and A5 material epoch.
 * Callers must supply live server snapshots, not presentation/client proposals.
 */
#include "nf_contact18a5_close.h"

typedef enum Nf18b2Status {
    NF18B2_ELIGIBLE = 0, NF18B2_BLOCKED = 1, NF18B2_PENDING = 2,
    NF18B2_STALE = 3, NF18B2_INVALID = 4,
    NF18B2_DUPLICATE = 5, NF18B2_PUBLISH_FAILED = 6
} Nf18b2Status;

typedef struct Nf18b2Witness {
    Nf18a3Decision traversal;
    uint32_t material_epoch;
    uint32_t world_revision;
    uint32_t tick;
    uint32_t actor_id;
    uint32_t feature_id;
    uint32_t witness_hash;
    Nf18b2Status status;
} Nf18b2Witness;

typedef struct Nf18b2Authority {
    uint32_t committed_tick;
    uint32_t committed_actor;
    uint32_t committed_feature;
    uint32_t committed_hash;
    uint8_t has_commit;
} Nf18b2Authority;

/* The material probe must be a trusted point on the intended passage.
 * This is necessary but not sufficient for full swept-volume safety:
 * A3 must independently approve the complete geometry. */
Nf18b2Witness nf18b2_adjudicate(Nf18a5World *material,
        Nf18a3Geometry geometry, Nf18a3Contract contract,
        Nf18a3Request request, NfVec3 material_probe);

/* Publisher must provide an atomic durable-before-visible transaction:
 * false => NO externally visible effects. A positive callback alone does
 * not prove durability; it is a caller obligation. The authority token
 * updates only if publication succeeds. No locomotion is initiated here. */
typedef bool (*Nf18b2Publish)(void *context, const Nf18b2Witness *w);
Nf18b2Status nf18b2_publish(Nf18b2Authority *authority,
        const Nf18b2Witness *w, Nf18b2Publish publisher, void *context);
#endif
