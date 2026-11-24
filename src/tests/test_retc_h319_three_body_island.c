/* H319 deterministic three-body A4 island laboratory.
   Contact constraints are a controlled synthetic fixture, NOT A6-world
   contact discovery, a grant, or published physics. */
#include "nf_contact18a4.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CK(c) do { if(!(c)) {fprintf(stderr,"H319 island failure line %d: %s\n",__LINE__,#c);return 1;} } while(0)
static Nf18a4Body make_actor(uint32_t id,float x,float vx) {
    Nf18a4Body a={0};
    a.id=id;a.center=(NfVec3){x,.9f,1.5f};
    a.velocity=(NfVec3){vx,0.f,0.f};
    a.inverse_mass=1.f/80.f;a.radius=.3f;a.height=1.8f;
    return a;
}
static Nf18a4Body make_crate(void) {
    Nf18a4Body b={0};
    b.id=2010u;b.center=(NfVec3){2.f,.9f,1.5f};
    b.inverse_mass=1.f/24.f;
    b.box_half=(NfVec3){.2f,.5f,.35f};
    return b;
}
static int equal_body(const Nf18a4Body *a,const Nf18a4Body *b) {
    return a->id==b->id && a->center.x==b->center.x &&
      a->center.y==b->center.y && a->center.z==b->center.z &&
      a->velocity.x==b->velocity.x && a->velocity.y==b->velocity.y &&
      a->velocity.z==b->velocity.z;
}
int main(void) {
    unsigned completed=0u,pending=0u;
    for(unsigned seed=0;seed<20u;++seed) {
        const float v=1.f+(float)(seed%10u)*.5f;
        Nf18a4Body bodies[3]={
          make_actor(10u,1.5f,v),make_actor(20u,2.5f,-v),make_crate()};
        Nf18a4Body reversed[3];
        memcpy(reversed,bodies,sizeof bodies);
        Nf18a4Constraint constraints[2]={
          {10u,2010u,2010u,{-1.f,0.f,0.f},{1.8f,.9f,1.5f},0.f,0.f},
          {20u,2010u,2010u,{1.f,0.f,0.f},{2.2f,.9f,1.5f},0.f,0.f}
        };
        Nf18a4Constraint swap[2]={constraints[1],constraints[0]};
        const Nf18a4IslandResult a=nf18a4_solve_island(
            bodies,3u,constraints,2u,NF18A4_RECIPROCAL_LINEAR,17u,4u);
        const Nf18a4IslandResult b=nf18a4_solve_island(
            reversed,3u,swap,2u,NF18A4_RECIPROCAL_LINEAR,17u,4u);
        CK(a.status==b.status);
        CK(a.contact_count==b.contact_count);
        CK(a.iterations==b.iterations);
        CK(a.constraint_evaluations==b.constraint_evaluations);
        for(size_t k=0;k<3u;++k)CK(equal_body(&bodies[k],&reversed[k]));
        if(a.status==NF18A4_ACCEPTED) {
            ++completed;
            CK(a.contact_count==2u);
            CK(a.receipts[0].contact_id!=0u && a.receipts[1].contact_id!=0u);
            CK(a.receipts[0].tick==17u && a.receipts[1].tick==17u);
        } else {
            ++pending;
            CK(a.status==NF18A4_PENDING_CONTACT);
        }
        Nf18a4Body invalid[3]={
          make_actor(10u,1.5f,v),make_actor(20u,2.5f,-v),make_crate()};
        Nf18a4Body pristine[3];
        memcpy(pristine,invalid,sizeof invalid);
        const Nf18a4Constraint dup[2]={constraints[0],constraints[0]};
        const Nf18a4IslandResult denied=nf18a4_solve_island(
          invalid,3u,dup,2u,NF18A4_RECIPROCAL_LINEAR,17u,4u);
        CK(denied.status==NF18A4_INVALID);
        for(size_t k=0;k<3u;++k)CK(equal_body(&invalid[k],&pristine[k]));
        printf("H319 ISLAND case=%u status=%u contacts=%u order_invariant=1 duplicate_denied=1\n",
            seed,(unsigned)a.status,(unsigned)a.contact_count);
    }
    CK(completed+pending==20u);
    printf("H319 ISLAND_COMPLETE cases=20 accepted=%u pending=%u order_invariant=1 duplicate_denied=1 A6_owner=0 B2_grants=0 B3_effects=0\n",
       completed,pending);
    return 0;
}
