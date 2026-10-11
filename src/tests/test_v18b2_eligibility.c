#include "nf_eligibility18b2.h"
#include <assert.h>
#include <string.h>
static int publishes;
static bool publish_ok(void *ctx,const Nf18b2Witness *w) {
    (void)ctx; assert(w->actor_id==7); ++publishes; return true;
}
static bool publish_fail(void *ctx,const Nf18b2Witness *w) {
    (void)ctx; (void)w; return false;
}
int main(void) {
    Nf18a5World material;
    nf18a5_world_init(&material,2);
    Nf18a3Geometry geometry={0};
    Nf18a3Contract contract={0};
    Nf18a3Request request={0};
    geometry.tick=10; geometry.world_version=2;
    contract.tick=9; contract.world_version=2;
    request.actor_id=7; request.feature_id=12;
    contract.actor_id=7; contract.feature_id=12;
    NfVec3 probe={0.5f,0.5f,0.5f};
    Nf18b2Witness result=nf18b2_adjudicate(&material,geometry,contract,request,probe);
    assert(result.status==NF18B2_STALE);
    assert(!result.traversal.commit_eligible);
    contract.tick=10;
    result=nf18b2_adjudicate(&material,geometry,contract,request,probe);
    assert(result.status!=NF18B2_ELIGIBLE); /* absent canonical material */
    Nf18b2Authority authority={0};
    assert(nf18b2_publish(&authority,&result,publish_ok,0)!=NF18B2_ELIGIBLE);
    assert(!authority.has_commit && !publishes);
    Nf18b2Witness synthetic={0};
    synthetic.status=NF18B2_ELIGIBLE; synthetic.traversal.commit_eligible=1;
    synthetic.tick=10; synthetic.actor_id=7; synthetic.feature_id=12;
    synthetic.witness_hash=12345; /* publisher protocol/unit test, not proof of physical eligibility */
    assert(nf18b2_publish(&authority,&synthetic,publish_fail,0)==NF18B2_INVALID);
    assert(!authority.has_commit);
    assert(nf18b2_publish(&authority,&synthetic,publish_ok,0)==NF18B2_INVALID);
    assert(!authority.has_commit && publishes==0);
    return 0;
}
