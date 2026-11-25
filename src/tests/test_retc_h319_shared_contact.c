/* H319: nonvacuous positive shared-crate contact and refusal controls.
 * Low-speed simultaneous translating capsule/AABB case; no A6 authority. */
#include "nf_retc_h319_shared_contact.h"
#include <stdio.h>
#define CK(x) do {if(!(x)){fprintf(stderr,"H319 SHARED FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static Nf18a4Body actor(uint32_t id,float x,float v){
    Nf18a4Body a={0};
    a.id=id;a.center=(NfVec3){x,.9f,1.5f};
    a.velocity.x=v;a.inverse_mass=1.f/80.f;
    a.radius=.3f;a.height=1.8f;return a;
}
static Nf18a4Body crate(void){
    Nf18a4Body b={0};b.id=2010u;
    b.center=(NfVec3){2.f,.9f,1.5f};
    b.inverse_mass=1.f/24.f;b.box_half=(NfVec3){.2f,.5f,.35f};
    return b;
}
int main(void){
    unsigned accepted=0,pending=0;
    for(unsigned i=0;i<20u;++i){
        /* Start with a positive gap, not an initial overlap. A bounded
         * low-speed case is intentional; higher speeds remain unresolved
         * unless the existing solver meets its convergence tolerance. */
        const float speed=.01f+.005f*(float)i;
        const float dt=.125f;
        const float distance=.5f+speed*dt*.5f;
        Nf18a4Body src[3]={actor(10u,2.f-distance,speed),
                           actor(20u,2.f+distance,-speed),crate()};
        const Nf18a4Body reverse[3]={src[1],src[0],src[2]};
        NfRetcH319SharedContact a={0},b={0};
        const Nf18a4Status x=nf_retc_h319_shared_contact(src,dt,17u,4u,&a);
        const Nf18a4Status y=nf_retc_h319_shared_contact(reverse,dt,17u,4u,&b);
        CK(x==y);
        if(x==NF18A4_PENDING_CONTACT){++pending;continue;}
        CK(x==NF18A4_ACCEPTED);
        CK(a.island.contact_count==2u&&b.island.contact_count==2u);
        for(size_t k=0;k<2u;++k){
            CK(a.constraints[k].body_a==(k?20u:10u));
            CK(a.constraints[k].body_b==2010u);
            CK(a.hits[k].toi>0.f&&a.hits[k].toi<=1.f);
            CK(a.island.receipts[k].impulses_applied);
            CK(a.island.receipts[k].contact_id==b.island.receipts[k].contact_id);
            CK(a.island.receipts[k].tick==17u&&a.island.receipts[k].world_version==4u);
            CK(a.island.receipts[k].normal_impulse.x==b.island.receipts[k].normal_impulse.x);
        }
        CK(a.island.receipts[0].contact_id!=a.island.receipts[1].contact_id);
        CK(a.island.receipts[0].normal_impulse.x<0.f&&a.island.receipts[1].normal_impulse.x>0.f);
        for(size_t k=0;k<3u;++k){
            CK(a.bodies[k].id==b.bodies[k].id);
            CK(a.bodies[k].velocity.x==b.bodies[k].velocity.x);
        }
        const float momentum=80.f*(a.bodies[0].velocity.x+a.bodies[1].velocity.x)+24.f*a.bodies[2].velocity.x;
        CK(fabsf(momentum)<.001f);
        ++accepted;
    }
    /* Refusals must not alter the caller's output. */
    Nf18a4Body src[3]={actor(10u,1.49f,.1f),actor(20u,2.51f,-.1f),crate()};
    NfRetcH319SharedContact out;
    memset(&out,0x5a,sizeof out);
    unsigned char before[sizeof out];memcpy(before,&out,sizeof out);
    src[1].id=10u;
    CK(nf_retc_h319_shared_contact(src,.125f,17u,4u,&out)==NF18A4_INVALID);
    CK(memcmp(before,&out,sizeof out)==0);
    src[1]=actor(20u,2.51f,.1f);
    CK(nf_retc_h319_shared_contact(src,.125f,17u,4u,&out)==NF18A4_PENDING_CONTACT);
    CK(memcmp(before,&out,sizeof out)==0);
    src[1]=actor(20u,2.505f,-.1f);
    CK(nf_retc_h319_shared_contact(src,.125f,17u,4u,&out)==NF18A4_PENDING_CONTACT);
    CK(memcmp(before,&out,sizeof out)==0);
    src[1].velocity.x=NAN;
    CK(nf_retc_h319_shared_contact(src,.125f,17u,4u,&out)==NF18A4_INVALID);
    CK(memcmp(before,&out,sizeof out)==0);
    printf("H319 SHARED cases=20 accepted=%u pending=%u refusal_controls=4 owner=0\n",accepted,pending);
    CK(accepted>0u&&accepted+pending==20u);
    return 0;
}
