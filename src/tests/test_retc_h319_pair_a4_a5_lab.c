/* H319 bounded SOURCE A4/A5 contact lab, not an A6-world acceptance. */
#include "nf_contact18a5.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { \
  fprintf(stderr,"H319 LAB FAIL line %d: %s\n",__LINE__,#x);return 1; \
} } while(0)

static Nf18a4Body actor(uint32_t id, float x, float speed) {
    Nf18a4Body a={0};
    a.id=id; a.center=(NfVec3){x,.9f,1.5f};
    a.velocity=(NfVec3){speed,0,0};
    a.inverse_mass=1.f/80.f;
    a.inverse_inertia=(NfVec3){1,1,1};
    a.radius=.3f; a.height=1.8f;
    return a;
}
static Nf18a4Body crate(void) {
    Nf18a4Body b={0};
    b.id=2010u; b.center=(NfVec3){2.f,.9f,1.5f};
    b.box_half=(NfVec3){.2f,.5f,.35f};
    b.inverse_mass=1.f/24.f;
    b.inverse_inertia=(NfVec3){.3f,.3f,.3f};
    return b;
}
int main(void) {
    Nf18a5Grid *grid=malloc(sizeof *grid);
    if (!grid) return 1;
    const Nf18a2ShapePolicy shape={.3f,1.8f,0.f,0.f,1.f};
    const Nf18a4Body target=crate();
    const uint32_t tick=17u, version=4u;
    unsigned accepted=0,applied=0,noncontact=0;
    for(unsigned i=0;i<20u;++i) {
        const float gap=.52f+.02f*(float)(i%10u);
        const float speed=2.f+(float)(i/10u)*3.f;
        Nf18a4Body a=actor(10u,2.f-gap,speed);
        Nf18a4Body b=actor(20u,2.f+gap,-speed);
        const Nf18a4Motor motora={a.velocity,0.f,0.f};
        const Nf18a4Motor motorb={b.velocity,0.f,0.f};
        Nf18a4PairResult left=nf18a4_pair_step(a,target,shape,motora,
          NF18A4_RECIPROCAL_LINEAR,.125f,tick,version,0.f,0.f);
        Nf18a4PairResult right=nf18a4_pair_step(b,target,shape,motorb,
          NF18A4_RECIPROCAL_LINEAR,.125f,tick,version,0.f,0.f);
        const Nf18a4PairResult p[2]={left,right};
        for(unsigned j=0;j<2u;++j) {
            const Nf18a4PairResult *r=&p[j];
            const bool contact=r->status==NF18A4_ACCEPTED &&
               r->swept_hit && r->receipt.status==NF18A4_ACCEPTED &&
               r->receipt.impulses_applied && r->receipt.contact_id!=0u &&
               r->receipt.tick==tick && r->receipt.world_version==version &&
               isfinite(r->toi) && r->toi>=0.f && r->toi<=1.f;
            if(contact) {
                Nf18aContact sample={0};
                REQUIRE(nf18a4_pair_to_sample(r,&sample));
                REQUIRE(sample.body_a==(j?20u:10u));
                REQUIRE(sample.body_b==2010u);
                REQUIRE(sample.contact_id==r->receipt.contact_id);
                REQUIRE(sample.normal_impulse>0.f);
                ++applied;
            } else ++noncontact;
            printf("H319 A4_PAIR case=%u actor=%u target=2010 status=%u swept=%u impulse_applied=%u contact_id=%u toi=%.6f source_tick=%u source_version=%u\n",
              i,j?20u:10u,(unsigned)r->status,(unsigned)r->swept_hit,
              (unsigned)r->receipt.impulses_applied,r->receipt.contact_id,
              (double)r->toi,tick,version);
        }
        if(left.receipt.impulses_applied && right.receipt.impulses_applied) ++accepted;
    }
    nf18a5_grid_init(grid,1u,NF18A5_SELECTIVE_FINE);
    uint8_t voxels[NF18A5_CANON_VOXELS]={0};
    REQUIRE(nf18a5_load_canonical(grid,0,0,0,voxels,tick));
    REQUIRE(nf18a5_query(grid,1.5f,.5f,1.5f,tick,true)==NF18A5_Q_FREE);
    REQUIRE(nf18a5_query(grid,2.5f,.5f,1.5f,tick,true)==NF18A5_Q_FREE);
    memset(voxels,NF18A5_MIXED,sizeof voxels);
    REQUIRE(nf18a5_load_canonical(grid,0,0,0,voxels,tick+1u));
    REQUIRE(nf18a5_query(grid,1.5f,.5f,1.5f,tick+1u,true)==NF18A5_Q_PENDING);
    memset(voxels,NF18A5_SOLID,sizeof voxels);
    REQUIRE(nf18a5_load_canonical(grid,0,0,0,voxels,tick+2u));
    REQUIRE(nf18a5_query(grid,2.5f,.5f,1.5f,tick+2u,true)==NF18A5_Q_SOLID);
    REQUIRE(nf18a5_query(grid,NAN,.5f,1.5f,tick+2u,true)==NF18A5_Q_INVALID);
    printf("H319 LAB_SOURCE_COMPLETE configurations=20 applied_pairs=%u dual_impulse_candidates=%u noncontact=%u material_controls=4 A6_owner=0 B2_grants=0 B3_effects=0\n",
       applied,accepted,noncontact);
    free(grid);
    return 0;
}
