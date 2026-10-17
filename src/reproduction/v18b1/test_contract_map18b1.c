#include "nf_smart18b1.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static NfB1Intent intent(const NfB1World *w){NfB1Intent i={.actor_id=7,.object_id=10,.tick=w->tick,.expected_revision=w->world_revision,.observed_material_epoch=27,.action=NF_B1_OPEN,.physically_reachable=1,.clearance=1,.has_contract=1,.actor_observed=1};return i;}
static NfB1World world(void){NfB1World w;nf_b1_init(&w);w.tick=11;w.world_revision=2;NfB1Object o={.id=10,.material_identity_epoch=27,.material_revision=3,.owner=7,.material_known=1,.support_valid=1};assert(nf_b1_add(&w,o));return w;}
int main(void){unsigned checks=0;
#define CK(x) do{assert(x);checks++;}while(0)
 NfB1World w=world();NfB1Intent i=intent(&w);NfB1Decision d=nf_b1_evaluate(&w,&i);
 CK(d.status==NF_B1_OK);CK(nf_b1_commit(&w,&i,d)==NF_B1_OK);
 CK(w.events[0].digest==nf_b1_event_digest(&w.events[0]));CK(w.objects[0].material_identity_epoch==27);
 CK(w.objects[0].material_revision==4&&w.world_revision==3);
 CK(nf_b1_commit(&w,&i,d)!=NF_B1_OK);CK(w.event_count==1);
 w=world();i=intent(&w);i.has_contract=0;CK(nf_b1_evaluate(&w,&i).status==NF_B1_BLOCKED);
 w=world();i=intent(&w);i.clearance=0;CK(nf_b1_evaluate(&w,&i).status==NF_B1_BLOCKED);
 w=world();i=intent(&w);w.objects[0].material_known=0;CK(nf_b1_evaluate(&w,&i).status==NF_B1_PENDING);
 w=world();i=intent(&w);i.expected_revision++;CK(nf_b1_evaluate(&w,&i).status==NF_B1_STALE);
 w=world();i=intent(&w);d=nf_b1_evaluate(&w,&i);d.witness^=1u;NfB1World before=w;
 CK(nf_b1_commit(&w,&i,d)==NF_B1_STALE);CK(memcmp(&w,&before,sizeof(w))==0);
 w=world();i=intent(&w);d=nf_b1_evaluate(&w,&i);w.world_revision=UINT32_MAX;i.expected_revision=UINT32_MAX;d=nf_b1_evaluate(&w,&i);before=w;
 CK(nf_b1_commit(&w,&i,d)==NF_B1_FULL);CK(memcmp(&w,&before,sizeof(w))==0);
 w=world();i=intent(&w);w.objects[0].material_revision=UINT32_MAX;d=nf_b1_evaluate(&w,&i);before=w;
 CK(nf_b1_commit(&w,&i,d)==NF_B1_FULL);CK(memcmp(&w,&before,sizeof(w))==0);
 puts("C02 B1 H2 local contract fixtures PASS");printf("assertions=%u\n",checks);return 0;
}
