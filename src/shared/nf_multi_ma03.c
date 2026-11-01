#include "nf_multi_ma03.h"
#include <math.h>
#include <string.h>
#include <float.h>
static Nf18b2UseDecision reject(void){
 Nf18b2UseDecision d={0};d.status=d.physical=d.contract=NF18B2_INVALID;
 d.reason=NF18B2_BAD_INPUT;return d;
}
NfMa03Chain nf_ma03_chain_oracle(const float x[3],const float v[3],
 float radius,float dt){
 NfMa03Chain o={0};
 if(!x||!v||!isfinite(radius)||radius<=0||!isfinite(dt)||dt<=0)return o;
 for(int i=0;i<3;i++){
  if(!isfinite(x[i])||!isfinite(v[i]))return o;
  o.x[i]=x[i];o.v[i]=v[i];
 }
 if(!(o.x[0]+2*radius<o.x[1]&&o.x[1]+2*radius<o.x[2]))return o;
 double t=0;
 for(unsigned step=0;step<8;++step){
  double soon=(double)dt-t;int pair=-1;
  for(int i=0;i<2;i++){
   double approach=(double)o.v[i]-o.v[i+1];
   if(approach<=0)continue;
   double gap=(double)o.x[i+1]-o.x[i]-2*radius;
   double arrival=gap/approach;
   if(arrival>=0&&arrival<soon){soon=arrival;pair=i;}
  }
  for(int i=0;i<3;i++)o.x[i]+=(float)(soon*o.v[i]);
  t+=soon;
  if(pair<0){o.complete=true;return o;}
  /* 1D equal-mass perfectly elastic solution: exchange velocities.
   * Derived independently from closed-form momentum/energy conservation. */
  float temp=o.v[pair];o.v[pair]=o.v[pair+1];o.v[pair+1]=temp;
  o.impacts++;
 }
 return o;
}
Nf18b2UseDecision nf_ma03_b2_owner_evaluate(const NfWorld *world,
 const Nf18a6Runtime *runtime,NfMa03Provider provider,void *ctx,
 uint32_t action,Nf18b2Model model){
 if(!world||!runtime||!provider||!action||!runtime->actor_id||
   world->tick_in_progress||world->tick_owner_context!=runtime||
   world->tick!=runtime->last_commit_tick)return reject();
 const NfActor *actor=nf_world_find_actor_const(world,runtime->actor_id);
 if(!actor||!actor->active||!actor->combat.alive)return reject();
 Nf18b2Scene scene={0};Nf18b2Use use={0};
 if(!provider(ctx,world,runtime,action,&scene,&use))return reject();
 if(scene.material!=&runtime->physical.world||!scene.actors||!scene.objects||
    scene.actor_count!=1||scene.object_count!=1||
    use.actor_id!=runtime->actor_id||use.action_id!=action||
    use.object_id!=runtime->physical.object.id||
    scene.actors[0].id!=runtime->actor_id||
    scene.objects[0].id!=runtime->physical.object.id||
    use.world_revision!=runtime->physical.world.revision||
    use.material_epoch!=runtime->physical.world.grid.global_revision||
    use.tick!=world->tick+1u||
    scene.geometry.tick!=use.tick||
    scene.geometry.world_version!=use.world_revision||
    fabsf(scene.actors[0].feet.x-actor->transform.position.x)>0.0001f||
    fabsf(scene.actors[0].feet.y-actor->transform.position.y)>0.0001f||
    fabsf(scene.actors[0].feet.z-actor->transform.position.z)>0.0001f||
    fabsf(scene.actors[0].radius-runtime->physical.actor.radius)>0.0001f||
    fabsf(scene.actors[0].height-runtime->physical.actor.height)>0.0001f)
    return reject();
 return nf18b2_evaluate(&scene,use,model);
}

/* H76: actor enumeration belongs to the actual NfWorld; this deterministic
 * roster is an input to a future A6-owned arbitration stage, not a commit. */
static bool roster_capture(const NfWorld *world,NfMa03Roster *out,bool in_tick){
 if(!world||!out||world->tick_in_progress!=in_tick)return false;
 NfMa03Roster candidate={0};
 candidate.source_tick=world->tick;
 for(size_t i=0;i<NF_MAX_ENTITIES;++i){
  const NfActor *a=&world->actors[i];
  if(!a->active)continue;
  if(!a->id||candidate.count>=NF_MA03_ROSTER_MAX)return false;
  const NfVec3 p=a->transform.position,v=a->transform.velocity;
  const NfMoveInput in=a->input;
  if(!isfinite(p.x)||!isfinite(p.y)||!isfinite(p.z)||
     !isfinite(v.x)||!isfinite(v.y)||!isfinite(v.z)||
     !isfinite(in.forward)||!isfinite(in.strafe)||!isfinite(in.yaw_radians))
    return false;
  uint32_t index=candidate.count;
  for(uint32_t j=0;j<candidate.count;++j){
   if(candidate.actors[j].id==a->id)return false;
   if(candidate.actors[j].id>a->id){index=j;break;}
  }
  for(uint32_t k=candidate.count;k>index;--k)
   candidate.actors[k]=candidate.actors[k-1u];
  candidate.actors[index]=(NfMa03Participant){a->id,p,v,in};
  ++candidate.count;
 }
 if(!candidate.count)return false;
 *out=candidate;
 return true;
}

bool nf_ma03_roster_capture(const NfWorld *w,NfMa03Roster *out){return roster_capture(w,out,false);}
bool nf_ma03_roster_capture_in_tick(const NfWorld *w,NfMa03Roster *out){return roster_capture(w,out,true);}
