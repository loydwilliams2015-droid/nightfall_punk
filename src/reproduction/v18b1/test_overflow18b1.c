#include "nf_smart18b1.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static void check(uint32_t world_revision,uint32_t material_revision) {
 NfB1World w;nf_b1_init(&w);w.tick=7;w.world_revision=world_revision;
 NfB1Object o={.id=10,.material_identity_epoch=27,.material_revision=material_revision,.material_known=1,.support_valid=1};
 assert(nf_b1_add(&w,o));
 NfB1Intent i={.actor_id=7,.object_id=10,.tick=7,.expected_revision=world_revision,.observed_material_epoch=27,.action=NF_B1_OPEN,.physically_reachable=1,.clearance=1,.has_contract=1,.actor_observed=1};
 NfB1Decision d=nf_b1_evaluate(&w,&i);assert(d.status==NF_B1_OK);
 NfB1World before=w;
 assert(nf_b1_commit(&w,&i,d)==NF_B1_FULL);
 assert(memcmp(&before,&w,sizeof(w))==0);
}
int main(void){check(UINT32_MAX,1);check(1,UINT32_MAX);check(UINT32_MAX,UINT32_MAX);puts("overflow-guard: 3/3 PASS");return 0;}
