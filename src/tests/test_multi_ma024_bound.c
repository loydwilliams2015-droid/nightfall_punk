#include "nf_multi_ma03c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {Nf18b2Actor a;Nf18b2Object object;Nf18aCollider solid;unsigned calls;unsigned change_at;int change;} Fixture;
static bool provided(void *p,const NfWorld *w,const Nf18a6Runtime *r,uint32_t action,Nf18b2Scene *s,Nf18b2Use *u){
 Fixture *f=p;++f->calls;
 const NfActor *actor=nf_world_find_actor_const(w,r->actor_id);
 if(!actor)return false;
 f->a=(Nf18b2Actor){.id=actor->id,.revision=1,.credentials=1,.support_id=101,
   .feet=actor->transform.position,.radius=r->physical.actor.radius,
   .height=r->physical.actor.height,.eye_height=actor->movement.eye_height,
   .active=1,.grounded=1};
 if(f->change&&f->calls>=f->change_at)f->a.credentials=0;
 const Nf18a4Body *b=&r->physical.object;
 f->object=(Nf18b2Object){.id=b->id,.revision=1,.cell_id=1,.nexus_id=1,.slot_id=1,
   .target={b->center.x-b->box_half.x,b->center.y+b->box_half.y-.1f,b->center.z},
   .max_reach=2.5f,.required_credentials=1,.capacity=1,.preconditions=1,.requires_support=1};
 f->solid=(Nf18aCollider){.body_id=101,.min=w->colliders[0].min,.max=w->colliders[0].max};
 *s=(Nf18b2Scene){.material=(Nf18a5World *)&r->physical.world,
  .actors=&f->a,.objects=&f->object,.actor_count=1,.object_count=1,
  .geometry={.solids=&f->solid,.solid_count=1,
   .tick=(uint32_t)w->tick+1,.world_version=r->physical.world.revision}};
 *u=(Nf18b2Use){.actor_id=actor->id,.object_id=b->id,.action_id=action,
  .tick=(uint32_t)w->tick+1,.world_revision=r->physical.world.revision,
  .material_epoch=r->physical.world.grid.global_revision,.actor_revision=1,.object_revision=1};
 return true;
}
int main(void){
 NfWorld *w=calloc(1,sizeof *w);
 Nf18a6Runtime *r=calloc(1,sizeof *r);
 Fixture *f=calloc(1,sizeof *f);
 CK(w&&r&&f);
 nf_world_init(w,42);
 CK(nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4})>=0);
 CK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 NfActor *a=nf_world_find_actor(w,10);CK(a);
 a->movement.grounded=true;a->movement.mode=NF_MOVE_GROUND;
 Nf18a4Body crate={0};
 crate.id=2010;crate.center=(NfVec3){2.2f,.9f,1};
 crate.inverse_mass=1.f/24;crate.inverse_inertia=(NfVec3){.3f,.3f,.3f};
 crate.box_half=(NfVec3){.2f,.5f,.35f};
 CK(nf18a6_init(r,w,10,crate,3,NF18A6_WORLD_BRIDGE));
 uint8_t zero[NF18A5_CANON_VOXELS]={0};
 CK(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,zero,1));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 NfMa024Receipt record={0};
 CK(nf_ma024_record(w,r,provided,f,31,NF18B2_M4_MATERIAL_FIRST,&record));
 CK(record.actor==10&&record.action==31&&record.fingerprint!=0);
 CK(nf_ma024_recheck(w,r,provided,f,31,NF18B2_M4_MATERIAL_FIRST,&record));
 f->change=1;f->change_at=f->calls+1;
 CK(!nf_ma024_recheck(w,r,provided,f,31,NF18B2_M4_MATERIAL_FIRST,&record));
 f->change=0;
 CK(!nf_ma024_recheck(w,r,provided,f,32,NF18B2_M4_MATERIAL_FIRST,&record));
 CK(nf18a6_unbind_world_tick(w,r));
 CK(!nf_ma024_recheck(w,r,provided,f,31,NF18B2_M4_MATERIAL_FIRST,&record));
 puts("MA024_BOUND_RECORD PASS");
 free(f);free(r);free(w);
 return 0;
}
