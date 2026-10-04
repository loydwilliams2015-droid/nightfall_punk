#include "nf_contact18a.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static unsigned total=0u,failed=0u;
static void gate(bool ok,const char *name) {
    ++total;
    printf("%s,%s\n",name,ok?"PASS":"FAIL");
    if(!ok)++failed;
}
static Nf18aCollider box(unsigned id,float x0,float y0,float z0,float x1,float y1,float z1) {
    Nf18aCollider b={0};b.body_id=id;
    b.min=(NfVec3){x0,y0,z0};b.max=(NfVec3){x1,y1,z1};b.material_channel=1u;
    return b;
}
static Nf18aSolveResult solve(NfVec3 p,NfVec3 velocity,const Nf18aCollider *b,size_t n,
                              Nf18aConfig cfg) {
    return nf18a_solve(17u,1u,p,velocity,(Nf18aShape){0.3f,1.8f,80.0f},1.0f/60.0f,b,n,cfg);
}
static bool near(float a,float b,float eps){return fabsf(a-b)<eps;}
int main(void) {
    const Nf18aConfig cfg=nf18a_default_config();
    const Nf18aCollider wall=box(22u,1.0f,-10.0f,-2.0f,1.02f,10.0f,2.0f);
    const Nf18aSolveResult a=solve((NfVec3){0.0f,0.0f,0.0f},(NfVec3){240.0f,0.0f,0.0f},&wall,1,cfg);
    gate(a.status==NF18A_BLOCKED && a.feet.x<=0.701f,"D01_thin_wall_CCD");
    gate(a.contact_count==1u && a.contacts[0].normal.x < -0.99f &&
         a.contacts[0].toi>0.0f && a.contacts[0].toi<1.0f,"D02_normal_TOI");
    gate(a.contacts[0].normal_impulse>0.0f && a.velocity.x==0.0f,"D03_impulse_restricts_normal_speed");
    gate(a.broadphase_candidates>=a.narrowphase_tests && a.narrowphase_tests>=1u,"D04_conservative_candidates");
    const Nf18aSolveResult replay=solve((NfVec3){0,0,0},(NfVec3){240,0,0},&wall,1,cfg);
    gate(nf18a_result_hash(&a)==nf18a_result_hash(&replay),"D05_duplicate_replay");
    const Nf18aSolveResult sliding=solve((NfVec3){0,0,0},(NfVec3){240,0,60},&wall,1,cfg);
    gate(sliding.feet.x<=0.701f && sliding.feet.z>0.1f,"D06_tangent_slide");
    const Nf18aCollider floor=box(33u,-5,-1,-5,5,0,5);
    const Nf18aSolveResult landing=solve((NfVec3){0,1,0},(NfVec3){0,-180,0},&floor,1,cfg);
    gate(landing.feet.y>=-0.001f && landing.support && landing.contacts[0].normal.y>0.99f,
         "D07_floor_support");
    const Nf18aCollider roof=box(44u,-5,2,-5,5,3,5);
    const Nf18aSolveResult jump=solve((NfVec3){0,0,0},(NfVec3){0,200,0},&roof,1,cfg);
    gate(jump.feet.y<=0.201f && !jump.support,"D08_ceiling_non_support");
    const Nf18aCollider pair[]={wall,box(23u,-2,-10,1,2,10,1.02f)};
    const Nf18aSolveResult corner=solve((NfVec3){0,0,0},(NfVec3){240,0,240},pair,2,cfg);
    gate(corner.feet.x<=0.701f && corner.feet.z<=0.701f,"D09_corner_multi_constraint");
    gate(corner.contact_count>=2u,"D10_two_contact_manifold");
    const Nf18aSolveResult reversed=solve((NfVec3){0,0,0},(NfVec3){240,0,240},(Nf18aCollider[]){pair[1],pair[0]},2,cfg);
    gate(near(corner.feet.x,reversed.feet.x,0.0002f)&&near(corner.feet.z,reversed.feet.z,0.0002f)
         && nf18a_result_hash(&corner)==nf18a_result_hash(&reversed),"D11_order_independence");
    const Nf18aSolveResult rot=solve((NfVec3){0,0,0},(NfVec3){0,0,240},
                                    (Nf18aCollider[]){box(22u,-2,-10,1,2,10,1.02f)},1,cfg);
    gate(near(a.feet.x,rot.feet.z,0.0002f),"D12_axis_rotation_metamorphism");
    Nf18aConfig limited=cfg;limited.iteration_budget=1u;
    const Nf18aSolveResult budget=solve((NfVec3){0,0,0},(NfVec3){240,0,60},&wall,1,limited);
    gate(budget.status==NF18A_PENDING_BUDGET && budget.feet.x<=0.701f,"D13_pending_is_nonpenetrating");
    const Nf18aSolveResult overlap=solve((NfVec3){1.01f,0,0},(NfVec3){0,0,1},&wall,1,cfg);
    gate(overlap.status==NF18A_INVALID_START,"D14_initial_overlap_explicit");
    const Nf18aSolveResult free=solve((NfVec3){0,0,0},(NfVec3){10,0,0},NULL,0,cfg);
    gate(free.status==NF18A_SOLVED && near(free.feet.x,10.0f/60.0f,0.0001f),"D15_free_movement");
    Nf18aHistory h;nf18a_history_init(&h);
    nf18a_history_record(&h,&a,17u);
    gate(h.active_count==1u && h.events[0].kind==NF18A_EVENT_BEGIN,"D16_history_begin");
    nf18a_history_record(&h,&a,18u);
    gate(h.active_count==1u && h.events[2].kind==NF18A_EVENT_PERSIST,"D17_history_persist");
    nf18a_history_record(&h,&free,19u);
    gate(h.active_count==0u && h.events[4].kind==NF18A_EVENT_END,"D18_history_end");
    for(unsigned i=0u;i<80u;++i)nf18a_history_record(&h,&a,20u+i);
    gate(h.event_count<=NF18A_MAX_HISTORY && h.cold_events>0,"D19_bounded_history");
    const Nf18aCollider mover=(Nf18aCollider){.body_id=51u,.min={1,-10,-2},.max={1.02f,10,2},
        .velocity={-100,0,0},.material_channel=1};
    const Nf18aSolveResult moving=solve((NfVec3){0,0,0},(NfVec3){0,0,0},&mover,1,cfg);
    /* Relatively moving collider can be queried by CCD, but no dynamic impulse propagation yet. */
    Nf18aContact moving_hit;
    gate(nf18a_sweep_aabb((NfVec3){0,0,0},(Nf18aShape){.radius=.3f,.height=1.8f,.mass=80},
          (NfVec3){0,0,0},mover,1.0f/60.0f,&moving_hit)&&moving_hit.toi>0,
         "D20_relative_sweep_detection");
    (void)moving;
    NfWorld world={0};
    world.collider_count=2u;
    world.colliders[0].kind=NF_COLLIDER_SOLID;
    world.colliders[0].min=(NfVec3){1,-1,0};
    world.colliders[0].max=(NfVec3){2,2,1};
    world.colliders[1].kind=NF_COLLIDER_LADDER;
    Nf18aCollider converted[2];
    gate(nf18a_extract_world_colliders(&world,converted,2u)==1u &&
         converted[0].body_id==1u,"D21_world_collider_adapter");
    gate(nf18a_extract_world_colliders(&world,converted,0u)==SIZE_MAX,
         "D22_no_silent_collider_truncation");
    Nf18aHistory authority;nf18a_history_init(&authority);
    gate(nf18a_history_commit(&authority,&a,20u,2u,1u),"D23_first_authoritative_history_commit");
    gate(!nf18a_history_commit(&authority,&a,20u,2u,1u),"D24_duplicate_same_tick_commit_rejected");
    gate(!nf18a_history_commit(&authority,&a,21u,3u,2u),"D25_cross_actor_history_rejected");
    gate(nf18a_history_commit(&authority,&free,21u,3u,1u),"D26_monotone_authoritative_history_commit");
    Nf18aCollider crowded[9];
    for(unsigned i=0u;i<9u;++i) crowded[i]=box(100u+i,1,-10,-2,1.02f,10,2);
    const Nf18aSolveResult overfull=solve((NfVec3){0,0,0},(NfVec3){240,0,0},crowded,9u,cfg);
    gate(overfull.status==NF18A_PENDING_BUDGET && overfull.feet.x<0.701f,
         "D27_simultaneous_contact_overflow_is_pending");
    printf("TOTAL,%u,PASS,%u,FAIL,%u\n",total,total-failed,failed);
    return failed?1:0;
}
