#include "nf_encounter18b2.h"
#include <assert.h>
#include <stdio.h>
int main(void){NfEncounter e;NfEncounterEpochs n={4,8,16},changed={5,8,16};unsigned checks=0;
#define CHECK(x) do{assert(x);checks++;}while(0)
nf_encounter_init(&e,1,2,3,9,n);CHECK(nf_encounter_eligible(&e,true,true,false,n)==NF_E_OK);CHECK(nf_encounter_reserve(&e,n)==NF_E_OK);CHECK(nf_encounter_commit(&e,changed,true)==NF_E_STALE);CHECK(!e.published&&e.phase==NF_E_EXPIRED);
nf_encounter_init(&e,1,2,3,9,n);CHECK(nf_encounter_eligible(&e,true,false,false,n)==NF_E_DENIED);CHECK(nf_encounter_reserve(&e,n)==NF_E_BAD_INPUT);
nf_encounter_init(&e,1,2,3,9,n);CHECK(nf_encounter_eligible(&e,true,true,true,n)==NF_E_DENIED);CHECK(e.phase==NF_E_PENDING);CHECK(nf_encounter_cancel(&e)==NF_E_OK);
nf_encounter_init(&e,1,2,3,9,n);CHECK(nf_encounter_eligible(&e,true,true,false,n)==NF_E_OK);CHECK(nf_encounter_reserve(&e,n)==NF_E_OK);CHECK(nf_encounter_commit(&e,n,false)==NF_E_DENIED);CHECK(!e.published);
nf_encounter_init(&e,1,2,3,9,n);CHECK(nf_encounter_eligible(&e,true,true,false,n)==NF_E_OK);CHECK(nf_encounter_reserve(&e,n)==NF_E_OK);CHECK(nf_encounter_commit(&e,n,true)==NF_E_OK);CHECK(nf_encounter_commit(&e,n,true)==NF_E_DUPLICATE);CHECK(nf_encounter_cancel(&e)==NF_E_DENIED);CHECK(nf_encounter_execute(&e)==NF_E_OK);CHECK(nf_encounter_complete(&e,false)==NF_E_OK);CHECK(e.published&&e.phase==NF_E_INTERRUPTED);
printf("encounter unit checks=%u PASS\n",checks);return 0;}
