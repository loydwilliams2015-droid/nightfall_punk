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
    /* Bounded replay memory; never silently evict an irreversible receipt. */
    uint32_t ticks[64], actors[64], features[64];
    uint8_t receipt_count;
    uint8_t publishing;
} Nf18b2Authority;

/* The material probe must be a trusted point on the intended passage.
 * This is necessary but not sufficient for full swept-volume safety:
 * A3 must independently approve the complete geometry. */
Nf18b2Witness nf18b2_adjudicate(Nf18a5World *material,
        Nf18a3Geometry geometry, Nf18a3Contract contract,
        Nf18a3Request request, NfVec3 material_probe);

/* Publisher must provide an atomic durable-before-visible transaction:
 * false => NO externally visible effects. A positive callback alone does
 * not prove durability; it is a caller obligation. No locomotion is initiated.
 * The legacy unbound function is fail-closed; use publish_checked. */
typedef bool (*Nf18b2Publish)(void *context, const Nf18b2Witness *w);
Nf18b2Status nf18b2_publish(Nf18b2Authority *authority,
        const Nf18b2Witness *w, Nf18b2Publish publisher, void *context);
/* Recompute from server-owned state immediately before publication. The
   callback receives the freshly adjudicated witness, never caller-supplied
   authority. Caller holds the world owner's lock across validation/callback.
   Callback durability/rollback and distributed replay remain B.5 obligations. */
Nf18b2Status nf18b2_publish_checked(Nf18b2Authority *authority,
    const Nf18b2Witness *w,Nf18a5World *material,Nf18a3Geometry geometry,
    Nf18a3Contract contract,Nf18a3Request request,NfVec3 probe,
    Nf18b2Publish publisher,void *context);

#define NF18B2_MAX_REQUESTS 64u
#define NF18B2_MAX_MEMBERS 16u
typedef enum Nf18b2Model { NF18B2_M3_EXHAUSTIVE=3, NF18B2_M4_MATERIAL_FIRST=4 } Nf18b2Model;
typedef enum Nf18b2Reason {
    NF18B2_OK=0, NF18B2_BAD_INPUT, NF18B2_FRAME_CHANGED,
    NF18B2_POSE, NF18B2_REACH, NF18B2_BODY_BLOCKED, NF18B2_OCCLUDED,
    NF18B2_MATERIAL_PENDING, NF18B2_SUPPORT, NF18B2_CREDENTIAL,
    NF18B2_INVENTORY, NF18B2_PRECONDITION, NF18B2_MEMBERSHIP,
    NF18B2_CAPACITY, NF18B2_REPLAY
} Nf18b2Reason;
/* These are immutable SERVER snapshots, not client-asserted facts. B.2
   screens and resolves proposals; it neither drives bodies nor debits items. */
typedef struct Nf18b2Actor {
    uint32_t id,revision,credentials,inventory,support_id;
    NfVec3 feet;
    float radius,height,eye_height;
    uint8_t active,grounded;
} Nf18b2Actor;
typedef struct Nf18b2Object {
    uint32_t id,revision,cell_id,nexus_id,slot_id;
    NfVec3 target;
    float max_reach;
    uint32_t required_credentials,required_inventory;
    uint32_t members[NF18B2_MAX_MEMBERS],occupants[NF18B2_MAX_MEMBERS];
    uint8_t member_count,occupant_count,capacity,preconditions,requires_support;
} Nf18b2Object;
typedef struct Nf18b2Scene {
    Nf18a5World *material;
    Nf18a3Geometry geometry;
    const Nf18b2Actor *actors;
    const Nf18b2Object *objects;
    size_t actor_count,object_count;
} Nf18b2Scene;
typedef struct Nf18b2Use {
    uint32_t actor_id,object_id,action_id,tick,world_revision,material_epoch;
    uint32_t actor_revision,object_revision;
} Nf18b2Use;
typedef struct Nf18b2UseDecision {
    Nf18b2Status status,physical,contract;
    Nf18b2Reason reason;
    uint32_t geometry_queries,material_queries;
} Nf18b2UseDecision;
/* M3 visits all physical jurisdictions even after a cheap failure; M4 stops
   at established physical failure. Both use real A3/A5 queries, no cost proxy.
   Decisions do not constitute animation/traversal completion or item transfer. */
Nf18b2UseDecision nf18b2_evaluate(const Nf18b2Scene *scene,
    Nf18b2Use use,Nf18b2Model model);
/* Deterministic same-frame slot resolution. Lower actor ID, then action ID,
   wins NEW competing claims; existing authorized occupants retain membership.
   Results preserve input order. This is not a cross-tick fairness guarantee. */
bool nf18b2_resolve(const Nf18b2Scene *scene,const Nf18b2Use *requests,
    size_t count,Nf18b2Model model,Nf18b2UseDecision *out);
const char *nf18b2_reason_name(Nf18b2Reason reason);
#endif
