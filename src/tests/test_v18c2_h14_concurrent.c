#include "nf_v18c2_h9_auth.h"
#include <threads.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {NfWorld *w;Nf18a6Runtime *r;atomic_uint *done;} Worker;
static int shutdown_worker(void *arg){
 Worker *x=arg;
 nf_c2_h9_owner_destroy(x->r);
 return 0;
}
static int work(void *arg){
 Worker *x=arg;
 for(unsigned i=0;i<3000;i++){
  nf_c2_h9_revoke(x->r);
  if(nf_c2_h9_valid(x->w,x->r,71))return 1;
  if(nf_c2_h9_consume(x->w,x->r,71))return 2;
 }
 atomic_fetch_add(x->done,1);return 0;
}
int main(void){
 NfWorld *w=calloc(1,sizeof *w);Nf18a6Runtime *r=calloc(1,sizeof *r);
 if(!w||!r)return 1;
 nf_world_init(w,42);
 if(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})!=10)return 2;
 Nf18a4Body ob={0};ob.id=2010;ob.center=(NfVec3){2,1,1};
 ob.inverse_mass=1.f/24.f;ob.box_half=(NfVec3){.2f,.5f,.35f};
 if(!nf18a6_init(r,w,10,ob,3,NF18A6_WORLD_BRIDGE)||
    !nf18a6_bind_world_tick(w,r,NULL,NULL,NULL)||
    !nf_c2_h9_owner_init(r))return 3;
 atomic_uint done=0;Worker data={w,r,&done};thrd_t threads[4];
 for(unsigned i=0;i<4;i++)if(thrd_create(&threads[i],work,&data)!=thrd_success)return 4;
 thrd_t closer;
 if(thrd_create(&closer,shutdown_worker,&data)!=thrd_success)return 11;
 for(unsigned i=0;i<4;i++){int result=-1;
  if(thrd_join(threads[i],&result)!=thrd_success||result)return 5;
 }
 {int result=-1;if(thrd_join(closer,&result)!=thrd_success||result)return 12;}
 if(atomic_load(&done)!=4||r->auth_state)return 6;
 /* World mutation/destruction is permitted only AFTER worker joins.
    Authorization shutdown above does not mutate the shared world. */
 if(w->tick!=0||r->physical.world.tick!=0)return 13;
 /* Destruction is permitted only AFTER all worker threads are joined.
    A6 unbinding during active tick is rejected by existing owner API. */
 w->tick_in_progress=true;
 if(nf18a6_unbind_world_tick(w,r))return 7;
 w->tick_in_progress=false;
 if(!nf18a6_unbind_world_tick(w,r)||r->auth_state)return 8;
 if(!nf18a6_bind_world_tick(w,r,NULL,NULL,NULL)||r->auth_state)return 9;
 if(nf_c2_h9_valid(w,r,71)||nf_c2_h9_consume(w,r,71))return 14;
 if(!nf18a6_unbind_world_tick(w,r))return 10;
 free(r);free(w);
 puts("H14 4-thread credential stress 12000 ops + active-tick unbind denial PASS");
 return 0;
}
