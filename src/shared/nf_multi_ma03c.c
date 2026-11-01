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

/* H82: positive two-actor contact is only an immutable proposal. No
 * source-world tick, A6 owner, material, B2 registry or B3 receipt mutation. */
static bool stage_contact(const NfWorld *world,uint32_t owner_actor,
 float dt,NfMa03cCandidate *out,bool in_tick){
 if(!world||!out||!owner_actor||!isfinite(dt)||dt<=0.f||
    world->tick_in_progress!=in_tick||world->tick>=UINT32_MAX)return false;
 NfMa03cCandidate candidate={0};
 if(!(in_tick?nf_ma03_roster_capture_in_tick(world,&candidate.roster):
                nf_ma03_roster_capture(world,&candidate.roster))||
    candidate.roster.count!=2u)return false;
 Nf18a4Body bodies[2]={0};
 bool owner_found=false;
 for(unsigned i=0;i<2u;++i){
  const NfMa03Participant *p=&candidate.roster.actors[i];
  owner_found|=p->id==owner_actor;
  bodies[i].id=p->id;
  bodies[i].center=p->position;
  bodies[i].velocity=p->velocity;
  bodies[i].inverse_mass=1.f;
  bodies[i].radius=.3f;
  bodies[i].height=1.8f;
 }
 if(!owner_found)return false;
 candidate.contact=nf_ma03c_compare(bodies,2u,dt,(uint32_t)world->tick+1u,1u);
 if(candidate.contact.matched!=1u||candidate.contact.disagreed||
    candidate.contact.unresolved)return false;
 candidate.source_tick=world->tick;
 candidate.owner_actor=owner_actor;
 candidate.eligible=true;
 *out=candidate;
 return true;
}

bool nf_ma03c_stage_contact(const NfWorld *w,uint32_t a,float dt,NfMa03cCandidate *out){return stage_contact(w,a,dt,out,false);}
bool nf_ma03c_stage_contact_in_tick(const NfWorld *w,uint32_t a,float dt,NfMa03cCandidate *out){return stage_contact(w,a,dt,out,true);}

/* H84 validates the presence of independent, source-consistent witnesses;
 * no A6, A5, B2 or B3 state is published by this pure check. */
bool nf_ma03c_joint_certify(const NfMa03cCandidate *c,
 const NfMa03cActorWitness witness[2],uint32_t epoch){
 if(!c||!witness||!c->eligible||c->roster.count!=2u||
    c->contact.matched!=1u||c->contact.unresolved||
    c->contact.disagreed||!epoch||c->source_tick+1u!=epoch)return false;
 for(unsigned i=0;i<2u;++i){
  const NfMa03cActorWitness *w=&witness[i];
  if(w->actor_id!=c->roster.actors[i].id||
     !w->physical_valid||!w->b2_valid||!w->b3_valid||
     w->physical_epoch!=epoch||w->b2_epoch!=epoch||w->b3_epoch!=epoch)
     return false;
 }
 return true;
}

/* H85 requires a source-owned issuer, and rejects any provider without
 * a bound A6 world-tick owner. The issuer is not automatically trustworthy:
 * real B2/A5/B3 source binding is an independently open acceptance gate. */
bool nf_ma03c_joint_certify_source(const NfWorld *world,
 const NfMa03cCandidate *candidate,uint32_t epoch,
 NfMa03cWitnessIssuer issuer,void *context){
 if(!world||!candidate||!issuer||!context||!world->tick_owner||
    !world->tick_owner_context||world->tick_in_progress||
    world->tick!=candidate->source_tick||!epoch||
    candidate->source_tick+1u!=epoch)return false;
 NfMa03cActorWitness witnesses[2]={{0},{0}};
 if(candidate->roster.count!=2u)return false;
 /* H86: revalidate the candidate against the CURRENT source actors.
    A caller cannot replay an old roster after changing live kinematics.
    Binding the issuer to actual A6/B2/A5/B3 ownership is still open. */
 bool owner_present=false;
 for(unsigned i=0;i<2u;++i){
  const NfMa03Participant *p=&candidate->roster.actors[i];
  const NfActor *a=nf_world_find_actor_const(world,p->id);
  if(!a||!a->active||!a->combat.alive||
     memcmp(&a->transform.position,&p->position,sizeof p->position)!=0||
     memcmp(&a->transform.velocity,&p->velocity,sizeof p->velocity)!=0||
     memcmp(&a->input,&p->input,sizeof p->input)!=0)return false;
  if(p->id==candidate->owner_actor)owner_present=true;
 }
 if(!owner_present)return false;
 for(unsigned i=0;i<2u;++i){
  const uint32_t id=candidate->roster.actors[i].id;
  const NfActor *actual=nf_world_find_actor_const(world,id);
  if(!actual||!actual->active||
     !issuer(context,world,id,epoch,&witnesses[i]))return false;
 }
 return nf_ma03c_joint_certify(candidate,witnesses,epoch);
}
