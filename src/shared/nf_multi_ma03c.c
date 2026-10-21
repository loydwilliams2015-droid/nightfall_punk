#include "nf_multi_ma03c.h"
#include <math.h>
#include <string.h>
static uint64_t mix(uint64_t h,uint32_t x){return (h^x)*UINT64_C(1099511628211);}
NfMa03cEvidence nf_ma03c_compare(const Nf18a4Body *b,size_t n,float dt,uint32_t tick,uint32_t rev){
 NfMa03cEvidence e={0};
 if(!b||n<2||n>3)return e;
 NfMa01c2Result result=nf_ma01c2_integrate(b,n,dt,tick,rev);
 if(result.status!=NF_NARROW_OK){e.unresolved=1;return e;}
 if(n!=2){e.unresolved=1;return e;}
 /* Independent two-body zero-restitution 1D momentum oracle:
  * verify only collinear, same-height, stationary target configurations. */
 if(fabsf(b[0].center.z-b[1].center.z)>.00001f||
    fabsf(b[0].velocity.z)>1e-6f||fabsf(b[1].velocity.z)>1e-6f||
    fabsf(b[0].center.y-b[1].center.y)>1e-6f){e.unresolved=1;return e;}
 const double m0=1.0/b[0].inverse_mass,m1=1.0/b[1].inverse_mass;
 const double closing=(double)b[0].velocity.x-b[1].velocity.x;
 if(!(closing>0)){e.unresolved=1;return e;}
 const double arrival=((double)b[1].center.x-b[0].center.x-b[0].radius-b[1].radius)/closing;
 if(arrival<0||arrival>dt){e.unresolved=1;return e;}
 const double shared=(m0*b[0].velocity.x+m1*b[1].velocity.x)/(m0+m1);
 const double expected0=b[0].center.x+b[0].velocity.x*arrival+shared*(dt-arrival);
 const double expected1=b[1].center.x+b[1].velocity.x*arrival+shared*(dt-arrival);
 if(fabs(result.bodies[0].center.x-expected0)<.003&&
    fabs(result.bodies[1].center.x-expected1)<.003&&
    fabs(result.bodies[0].velocity.x-shared)<.003&&
    fabs(result.bodies[1].velocity.x-shared)<.003)e.matched=1;
 else e.disagreed=1;
 return e;
}
static uint64_t fingerprint(const Nf18b2Scene *s,const Nf18b2Use *u){
 uint64_t h=UINT64_C(1469598103934665603);
 const uint32_t fields[]={u->actor_id,u->object_id,u->action_id,u->tick,
 u->world_revision,u->material_epoch,u->actor_revision,u->object_revision,
 s->actors[0].credentials,s->actors[0].inventory,s->actors[0].support_id,
 s->objects[0].required_credentials,s->objects[0].required_inventory,
 s->objects[0].preconditions,s->objects[0].capacity,s->objects[0].occupant_count,
 s->material->grid.global_revision};
 for(size_t i=0;i<sizeof fields/sizeof *fields;i++)h=mix(h,fields[i]);
 return h;
}
bool nf_ma024_record(const NfWorld *w,const Nf18a6Runtime *r,NfMa03Provider source,void *ctx,
 uint32_t action,Nf18b2Model model,NfMa024Receipt *out){
 if(!out||!source||!w||!r||!action)return false;
 memset(out,0,sizeof *out);
 if(nf_ma03_b2_owner_evaluate(w,r,source,ctx,action,model).status!=NF18B2_ELIGIBLE)return false;
 Nf18b2Scene scene={0};Nf18b2Use use={0};
 if(!source(ctx,w,r,action,&scene,&use)||scene.material!=&r->physical.world||
    !scene.actors||!scene.objects||scene.actor_count!=1||scene.object_count!=1||
    use.actor_id!=r->actor_id||use.object_id!=r->physical.object.id||
    use.action_id!=action||use.world_revision!=r->physical.world.revision||
    use.material_epoch!=r->physical.world.grid.global_revision||
    use.tick!=w->tick+1)return false;
 out->actor=use.actor_id;out->action=action;out->tick=use.tick;
 out->world_revision=use.world_revision;out->material_epoch=use.material_epoch;
 out->fingerprint=fingerprint(&scene,&use);
 return true;
}
bool nf_ma024_recheck(const NfWorld *w,const Nf18a6Runtime *r,NfMa03Provider source,void *ctx,
 uint32_t action,Nf18b2Model model,const NfMa024Receipt *expected){
 if(!expected)return false;
 NfMa024Receipt next={0};
 return nf_ma024_record(w,r,source,ctx,action,model,&next)&&
  next.actor==expected->actor&&next.action==expected->action&&
  next.tick==expected->tick&&next.world_revision==expected->world_revision&&
  next.material_epoch==expected->material_epoch&&next.fingerprint==expected->fingerprint;
}
