#include "nf_multi_ma03e.h"
#include <math.h>
NfMa03ePair nf_ma03e_offaxis(Nf18a4Body a,Nf18a4Body b,float dt,uint32_t tick,uint32_t version){
 NfMa03ePair o={0};o.status=NF_NARROW_PENDING;
 NfMultiCcd hit=nf_multi_capsule_ccd18c1(&a,&b,dt);
 if(hit.status!=NF_NARROW_OK)return o;
 if(!hit.hit){
  a.center.x+=a.velocity.x*dt;a.center.z+=a.velocity.z*dt;
  b.center.x+=b.velocity.x*dt;b.center.z+=b.velocity.z*dt;
  o.a=a;o.b=b;o.status=NF_NARROW_OK;return o;
 }
 const float t=hit.toi*dt;
 a.center.x+=a.velocity.x*t;a.center.z+=a.velocity.z*t;
 b.center.x+=b.velocity.x*t;b.center.z+=b.velocity.z*t;
 Nf18a4Receipt r=nf18a4_apply_contact(&a,&b,hit.contact,NF18A4_RECIPROCAL_LINEAR,tick,version);
 if(r.status!=NF18A4_ACCEPTED||!r.impulses_applied)return o;
 float left=dt-t;
 a.center.x+=a.velocity.x*left;a.center.z+=a.velocity.z*left;
 b.center.x+=b.velocity.x*left;b.center.z+=b.velocity.z*left;
 o.a=a;o.b=b;o.toi=hit.toi;o.impulses=1;o.status=NF_NARROW_OK;
 return o;
}
bool nf_ma026_prepublish(const NfWorld *world,const Nf18a5Integrated *candidate,void *context){
 NfMa026Gate *gate=context;
 if(!gate||!gate->runtime||!world||!candidate||!gate->action)return false;
 if(world->tick_owner_context!=gate->runtime||!world->tick_in_progress||
    candidate->world.tick!=world->tick+1||
    candidate->world.grid.global_revision!=gate->expected.material_epoch||
    gate->runtime->physical.world.revision!=gate->expected.world_revision)return false;
 /* Previous research recheck prohibited in-progress: owner callback must
  * retain exclusive tick and perform direct bound record comparison here. */
 NfMa024Receipt current={0};
 return nf_ma024_record(world,gate->runtime,gate->provider,gate->provider_ctx,
   gate->action,gate->model,&current) &&
   current.fingerprint==gate->expected.fingerprint &&
   current.action==gate->expected.action &&
   current.tick==gate->expected.tick &&
   current.material_epoch==gate->expected.material_epoch;
}
