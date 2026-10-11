#define _POSIX_C_SOURCE 200809L
#include "v18b2_fixtures.h"
#include <math.h>
#include <stdio.h>
#include <unistd.h>
static unsigned checks,failures,publishes;
static void check(bool ok,const char *name){
    ++checks;if(!ok)++failures;printf("%s,%s\n",name,ok?"PASS":"FAIL");
}
static bool publish_ok(void *ctx,const Nf18b2Witness *w){
    (void)ctx;if(!w->traversal.commit_eligible)return false;++publishes;return true;
}
static bool publish_fail(void *ctx,const Nf18b2Witness *w){(void)ctx;(void)w;return false;}
static Nf18b2Status eligible(const Nf18b2Scene *s,Nf18b2Use u){
    return nf18b2_evaluate(s,u,NF18B2_M4_MATERIAL_FIRST).status;
}
typedef struct GateContext {Nf18b2Scene *scene;Nf18b2Use use;} GateContext;
static bool gate(const Nf18a5Integrated *candidate,void *ctx){
    GateContext *g=ctx;
    return candidate->world.revision==g->scene->material->revision+1&&
        eligible(g->scene,g->use)==NF18B2_ELIGIBLE;
}
int main(void){
    B2Fixture f;unsigned mismatches=0;
    for(unsigned flags=0;flags<4096;++flags){
        fixture(&f,flags);
        for(unsigned m=3;m<=4;++m){
            Nf18b2UseDecision d=nf18b2_evaluate(&f.scene,f.use,(Nf18b2Model)m);
            if(d.status!=expected_status(flags)||d.reason!=expected_reason(flags))++mismatches;
        }
    }
    check(mismatches==0,"independent_4096_physical_contract_combinations");
    fixture(&f,0);f.objects[0].max_reach=2;
    check(eligible(&f.scene,f.use)==NF18B2_ELIGIBLE,"exact_reach_boundary");
    f.objects[0].max_reach=nextafterf(2,0);
    check(eligible(&f.scene,f.use)==NF18B2_BLOCKED,"one_ulp_beyond_reach");
    fixture(&f,0);f.actors[0].feet.x=NAN;
    check(eligible(&f.scene,f.use)==NF18B2_INVALID,"nan_actor_fails_closed");
    fixture(&f,0);f.objects[0].max_reach=INFINITY;
    check(eligible(&f.scene,f.use)==NF18B2_INVALID,"infinite_reach_fails_closed");
    fixture(&f,0);f.actors[1]=f.actors[0];f.scene.actor_count=2;
    check(eligible(&f.scene,f.use)==NF18B2_INVALID,"ambiguous_actor_ids");
    fixture(&f,0);f.objects[0].occupant_count=2;
    check(eligible(&f.scene,f.use)==NF18B2_INVALID,"corrupt_capacity");
    fixture(&f,0);
    check(nf18b2_evaluate(&f.scene,f.use,(Nf18b2Model)0).status==NF18B2_INVALID,"runtime_control_separation");
    const Nf18a2ShapePolicy huge={100000000,200000000,0,0,1};
    check(nf18a5_sweep_query(&f.material.grid,(NfVec3){0,0,0},
        (NfVec3){100000000,100000000,100000000},huge,1,10,NULL,NULL,NULL)==NF18A5_Q_PENDING,
        "overflow_hull_has_bounded_pending_exit");
    f.material.grid.chunks[0].last_access_tick=11;
    check(eligible(&f.scene,f.use)==NF18B2_PENDING,"future_cache_never_grants_past_tick");
    fixture(&f,F_UNKNOWN);uint8_t fine[512]={0};fine[(1*8+3)*8+3]=NF18A5_SOLID;
    check(nf18a5_load_fine(&f.material.grid,0,0,0,2,fine,512,
        f.material.grid.chunks[0].canonical_epoch,10),"real_fine_payload_loaded");
    check(eligible(&f.scene,f.use)==NF18B2_ELIGIBLE,"resident_fine_free_body");
    fine[(0*8+2)*8+2]=NF18A5_SOLID;
    check(nf18a5_load_fine(&f.material.grid,0,0,0,2,fine,512,
        f.material.grid.chunks[0].canonical_epoch,10),"neighbor_fine_obstruction_loaded");
    check(eligible(&f.scene,f.use)==NF18B2_BLOCKED,"fine_obstruction_blocks_whole_body");
    f.material.grid.chunks[0].fine[(0*8+2)*8+2]=NF18A5_MIXED;
    check(eligible(&f.scene,f.use)==NF18B2_INVALID,"malformed_fine_cache_never_false_clear");
    check(nf18a5_release_fine(&f.material.grid,0,0,0),"fine_eviction");
    check(eligible(&f.scene,f.use)==NF18B2_PENDING,"eviction_cannot_create_clearance");
    fixture(&f,0);f.solids[0]=(Nf18aCollider){.body_id=88,.min={2,1,0},.max={2.1f,2,2}};
    f.scene.geometry.solids=f.solids;f.scene.geometry.solid_count=1;
    check(eligible(&f.scene,f.use)==NF18B2_BLOCKED,"geometry_occlusion_with_free_material_grid");
    fixture(&f,F_SUPPORT);f.solids[0].dynamic_body=0;
    check(eligible(&f.scene,f.use)==NF18B2_ELIGIBLE,"verified_stationary_support");
    f.solids[0].dynamic_body=1;
    check(eligible(&f.scene,f.use)==NF18B2_BLOCKED,"moving_support_explicitly_blocked");
    f.solids[1]=f.solids[0];f.solids[1].dynamic_body=0;f.scene.geometry.solid_count=2;
    check(eligible(&f.scene,f.use)==NF18B2_INVALID,"ambiguous_support_identity_rejected");
    fixture(&f,0);f.actors[0].revision=f.use.actor_revision=0;
    check(eligible(&f.scene,f.use)==NF18B2_INVALID,"unknown_actor_revision_rejected");
    fixture(&f,0);Nf18b2Use uses[16];Nf18b2UseDecision out[16];
    for(unsigned i=0;i<16;++i){f.actors[i]=f.actors[0];f.actors[i].id=i+1;
        uses[i]=f.use;uses[i].actor_id=i+1;uses[i].action_id=i+1;}
    f.scene.actor_count=16;
    for(unsigned m=3;m<=4;++m)for(unsigned n=2;n<=16;n*=2){
        check(nf18b2_resolve(&f.scene,uses,n,(Nf18b2Model)m,out),"contested_batch_resolved");
        unsigned granted=0;for(unsigned i=0;i<n;++i)granted+=out[i].status==NF18B2_ELIGIBLE;
        check(granted==1&&out[0].status==NF18B2_ELIGIBLE,"pickup_one_deterministic_claim");
        Nf18b2Use reverse[16];for(unsigned i=0;i<n;++i)reverse[i]=uses[n-1-i];
        check(nf18b2_resolve(&f.scene,reverse,n,(Nf18b2Model)m,out)&&
            out[n-1].status==NF18B2_ELIGIBLE,"permutation_preserves_winner");
    }
    f.objects[0].capacity=2;f.objects[0].members[0]=1;f.objects[0].members[1]=2;
    f.objects[0].member_count=2;
    check(nf18b2_resolve(&f.scene,uses,2,NF18B2_M4_MATERIAL_FIRST,out)&&
        out[0].status==NF18B2_ELIGIBLE&&out[1].status==NF18B2_ELIGIBLE,"two_authorized_joint_operators");
    uses[1].actor_id=1;
    check(nf18b2_resolve(&f.scene,uses,2,NF18B2_M4_MATERIAL_FIRST,out)&&
        out[1].status==NF18B2_DUPLICATE,"actor_cannot_take_two_slots");
    uses[1].actor_id=2;f.objects[1]=f.objects[0];f.objects[1].id=13;
    f.objects[0].capacity=f.objects[1].capacity=1;f.scene.object_count=2;uses[1].object_id=13;
    check(nf18b2_resolve(&f.scene,uses,2,NF18B2_M4_MATERIAL_FIRST,out)&&
        out[0].status==NF18B2_ELIGIBLE&&out[1].status==NF18B2_ELIGIBLE,"disjoint_transactions_both_granted");
    uses[1].actor_id=uses[0].actor_id;uses[1].action_id=uses[0].action_id;
    check(nf18b2_resolve(&f.scene,uses,2,NF18B2_M4_MATERIAL_FIRST,out)&&
        out[0].status==NF18B2_INVALID&&out[1].status==NF18B2_INVALID,"contradictory_replay_targets_rejected");
    check(!nf18b2_resolve(&f.scene,uses,65,NF18B2_M4_MATERIAL_FIRST,out),"oversized_batch_rejected");
    fixture(&f,0);
    Nf18aCollider ladders[2]={{.body_id=12,.min={2,0,.5f},.max={2.1f,3,1.5f}},
        {.body_id=13,.min={2,0,.5f},.max={2.1f,3,1.5f}}};
    Nf18a3Geometry g={.ladders=ladders,.ladder_count=2,.world_version=f.material.revision,.tick=10};
    Nf18a3Request r={.transition=NF18A3_LADDER_ATTACH,.policy=NF18A3_HYBRID_DUAL,
        .actor_id=7,.feature_id=12,.feet={1,0,1},.destination={1.6f,0,1},.facing={1,0,0},
        .body_radius=.3f,.body_height=1.8f,.foot_flat_radius=.1f,.max_ladder_reach=2,
        .min_facing_cosine=.5f,.min_landing_width=.1f,.grounded=1};
    Nf18a3Contract c={.actor_id=7,.feature_id=12,.world_version=g.world_version,.tick=10,
        .may_attach_ladder=1,.material_support_valid=1,.support_stable=1,
        .authoritative_reach_limit=2,.authoritative_body_radius=.3f,.authoritative_body_height=1.8f,
        .authoritative_foot_flat_radius=.1f,.authoritative_min_facing_cosine=.5f,
        .authoritative_min_landing_width=.1f,.authoritative_feet={1,0,1},.actor_grounded=1};
    const NfVec3 probe={1.3f,.5f,1};
    Nf18b2Witness w=nf18b2_adjudicate(&f.material,g,c,r,probe);
    check(w.status==NF18B2_ELIGIBLE,"actual_A3_A5_ladder_eligibility");
    Nf18b2Authority auth={0};
    check(nf18b2_publish(&auth,&w,publish_ok,NULL)==NF18B2_INVALID&&publishes==0,"unbound_publication_closed");
    Nf18b2Witness forged=w;forged.witness_hash++;
    check(nf18b2_publish_checked(&auth,&forged,&f.material,g,c,r,probe,publish_ok,NULL)==NF18B2_STALE,"forged_witness_rejected");
    check(nf18b2_publish_checked(&auth,&w,&f.material,g,c,r,probe,publish_fail,NULL)==NF18B2_PUBLISH_FAILED&&
        !auth.receipt_count,"failed_callback_no_receipt");
    auth.publishing=1;
    check(nf18b2_publish_checked(&auth,&w,&f.material,g,c,r,probe,publish_ok,NULL)==NF18B2_PENDING&&
        !auth.receipt_count,"reentrant_publisher_has_no_second_authority");
    auth.publishing=0;
    check(nf18b2_publish_checked(&auth,&w,&f.material,g,c,r,probe,publish_ok,NULL)==NF18B2_ELIGIBLE,"fresh_checked_publication");
    r.feature_id=c.feature_id=13;
    Nf18b2Witness other=nf18b2_adjudicate(&f.material,g,c,r,probe);
    check(nf18b2_publish_checked(&auth,&other,&f.material,g,c,r,probe,publish_ok,NULL)==NF18B2_ELIGIBLE,"second_disjoint_receipt");
    r.feature_id=c.feature_id=12;
    check(nf18b2_publish_checked(&auth,&w,&f.material,g,c,r,probe,publish_ok,NULL)==NF18B2_DUPLICATE&&publishes==2,
        "interleaved_A_B_A_replay_rejected");
    ++f.material.revision;
    check(nf18b2_publish_checked(&auth,&w,&f.material,g,c,r,probe,publish_ok,NULL)==NF18B2_STALE&&publishes==2,"stale_before_publication");
    --f.material.revision;
    for(unsigned i=auth.receipt_count;i<64;++i){g.tick=c.tick=11+i;w=nf18b2_adjudicate(&f.material,g,c,r,probe);
        if(nf18b2_publish_checked(&auth,&w,&f.material,g,c,r,probe,publish_ok,NULL)!=NF18B2_ELIGIBLE)++failures;}
    g.tick=c.tick=100;w=nf18b2_adjudicate(&f.material,g,c,r,probe);
    check(nf18b2_publish_checked(&auth,&w,&f.material,g,c,r,probe,publish_ok,NULL)==NF18B2_PENDING,"receipt_capacity_no_eviction");
    fixture(&f,F_BODY);g.tick=c.tick=10;
    w=nf18b2_adjudicate(&f.material,g,c,r,(NfVec3){2.5f,.5f,1.5f});
    check(w.status==NF18B2_BLOCKED,"clear_point_cannot_bypass_full_capsule");
    /* Real A5 physics staging and WAL prepublication gate. */
    fixture(&f,0);Nf18a5Integrated physical={0};physical.world=f.material;
    physical.actor=(Nf18a4Body){.id=7,.center={1,.9f,1},.radius=.3f,.height=1.8f,
        .inverse_mass=1.0f/80,.inverse_inertia={1,1,1},.box_half={.5f,.9f,.8f}};
    physical.object=(Nf18a4Body){.id=12,.center={2.2f,.9f,1},.radius=.3f,.height=1.8f,
        .inverse_mass=1.0f/40,.inverse_inertia={1,1,1},.box_half={.5f,.9f,.8f}};
    f.scene.material=&physical.world;GateContext ctx={&f.scene,f.use};
    Nf18a5Integrated before=physical;
    char journal[]="/tmp/nf18b2_gate_XXXXXX";int fd=mkstemp(journal);
    if(fd<0)return 2;
    close(fd);f.actors[0].credentials=0;
    const Nf18a2ShapePolicy shape={.3f,1.8f,.1f,.4f,.7f};
    const Nf18a4Motor motor={{120,0,0},200000,5000};
    Nf18a5CloseResult result=nf18a5_integrated_pair_step_checked(&physical,1,10,
        (NfVec3){1,.1f,1},NULL,NULL,shape,motor,1.0f/60,0,0,journal,gate,&ctx);
    check(result.status==NF18A5_CLOSE_GATED&&memcmp(&physical,&before,sizeof(physical))==0,"denied_no_body_or_history_write");
    Nf18a5Integrated recovered={0};
    check(!nf18a5_restore(journal,&recovered),"denied_no_durable_snapshot");
    f.actors[0].credentials=1;
    result=nf18a5_integrated_pair_step_checked(&physical,1,10,
        (NfVec3){1,.1f,1},NULL,NULL,shape,motor,1.0f/60,0,0,journal,gate,&ctx);
    check(result.status==NF18A5_CLOSE_COMMITTED&&result.dynamic_contact&&
        physical.world.history.raw_samples==1,"authorized_real_contact_history");
    check(nf18a5_restore(journal,&recovered)&&recovered.world.revision==physical.world.revision&&
        recovered.object.velocity.x==physical.object.velocity.x,"actual_A5_WAL_recovery");
    unlink(journal);printf("checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
