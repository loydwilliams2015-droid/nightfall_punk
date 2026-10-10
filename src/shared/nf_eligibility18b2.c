#include "nf_eligibility18b2.h"
#include <string.h>
#include <math.h>

static uint32_t mix(uint32_t h,uint32_t v) {
    h ^= v; h *= UINT32_C(16777619); return h;
}
Nf18b2Witness nf18b2_adjudicate(Nf18a5World *material,
        Nf18a3Geometry geometry, Nf18a3Contract contract,
        Nf18a3Request request, NfVec3 probe) {
    Nf18b2Witness w;
    memset(&w,0,sizeof(w));
    w.status=NF18B2_INVALID;
    if (!material || !isfinite(probe.x) || !isfinite(probe.y) || !isfinite(probe.z)
        || request.actor_id==0 || request.feature_id==0) return w;
    w.tick=geometry.tick;
    w.world_revision=material->revision;
    w.material_epoch=material->grid.global_revision;
    w.actor_id=request.actor_id;
    w.feature_id=request.feature_id;
    if (geometry.world_version != contract.world_version ||
        geometry.tick != contract.tick ||
        request.actor_id != contract.actor_id ||
        request.feature_id != contract.feature_id) {
        w.status=NF18B2_STALE; return w;
    }
    /* Never resolve pending canonical/fine data to positive clearance.
       No provider is trusted implicitly; streaming must occur beforehand. */
    const Nf18a5Query q=nf18a5_query(&material->grid,probe.x,probe.y,probe.z,
                                      geometry.tick,true);
    if (q==NF18A5_Q_PENDING) { w.status=NF18B2_PENDING; return w; }
    if (q==NF18A5_Q_INVALID) return w;
    if (q==NF18A5_Q_SOLID) { w.status=NF18B2_BLOCKED; return w; }
    /* Reject deliberately unsafe scientific negative-control policies. */
    if (request.policy==NF18A3_CAPSULE_EASY_NEGATIVE ||
        request.policy==NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE) return w;
    w.traversal=nf18a3_adjudicate(geometry,contract,request);
    w.status=w.traversal.commit_eligible ? NF18B2_ELIGIBLE : NF18B2_BLOCKED;
    uint32_t hash=UINT32_C(2166136261);
    hash=mix(hash,w.world_revision); hash=mix(hash,w.material_epoch);
    hash=mix(hash,w.tick); hash=mix(hash,w.actor_id);
    hash=mix(hash,w.feature_id);
    hash=mix(hash,nf18a3_decision_hash(&w.traversal));
    w.witness_hash=hash;
    return w;
}
Nf18b2Status nf18b2_publish(Nf18b2Authority *authority,
        const Nf18b2Witness *w, Nf18b2Publish publisher,void *context) {
    if (!authority || !w || !publisher) return NF18B2_INVALID;
    if (w->status!=NF18B2_ELIGIBLE || !w->traversal.commit_eligible
        || !w->witness_hash) return w->status==NF18B2_ELIGIBLE
                               ? NF18B2_INVALID : w->status;
    if (authority->has_commit && authority->committed_tick==w->tick
        && authority->committed_actor==w->actor_id
        && authority->committed_feature==w->feature_id) {
        return NF18B2_DUPLICATE;
    }
    /* Caller must ensure server revisions are still current immediately
       before publish, ideally under a tick/transaction owner lock. */
    if (!publisher(context,w)) return NF18B2_PUBLISH_FAILED;
    authority->committed_tick=w->tick;
    authority->committed_actor=w->actor_id;
    authority->committed_feature=w->feature_id;
    authority->committed_hash=w->witness_hash;
    authority->has_commit=1;
    return NF18B2_ELIGIBLE;
}
