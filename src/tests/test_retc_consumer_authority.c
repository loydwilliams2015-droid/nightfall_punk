/* H295: 50 independent negative categories x 3 read-only consumers,
 * each paired with a positive accepted control. Strict C11, no raylib.
 * This is NOT a live A6/Map/Audio/Render integration test. */
#include "nf_retc_consumer_authority.h"
#include <stdio.h>
#include <stdlib.h>
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"H295 failure line=%d case=%u role=%u\n",__LINE__,case_id,(unsigned)role);exit(1);}}while(0)
int main(void){
 unsigned denials=0,controls=0,replays=0;
 for(unsigned case_id=0;case_id<50u;++case_id){
  for(unsigned role_value=1u;role_value<=3u;++role_value){
   NfRetcConsumer role=(NfRetcConsumer)role_value;
   const NfRetcAcceptedWitness good={100u,200u,300u,400u,500u,true};
   NfRetcAcceptedWitness altered=good;
   NfRetcConsumerCursor cursor={0};
   const NfRetcConsumerCursor original=cursor;
   uint64_t law=300u,owner=400u;
   NfRetcConsumer requested=role;
   switch(case_id/5u){
    case 0: altered.published=false;break;
    case 1: altered.tick=0u;break;
    case 2: altered.revision=0u;break;
    case 3: altered.constitution_revision=0u;break;
    case 4: altered.owner_generation=0u;break;
    case 5: altered.event_id=0u;break;
    case 6: law=301u+(case_id%5u);break;
    case 7: owner=401u+(case_id%5u);break;
    case 8: cursor.last_tick=good.tick+(case_id%5u);break;
    case 9: requested=(NfRetcConsumer)(0u+case_id%5u+4u);break;
   }
   const NfRetcConsumerCursor before=cursor;
   REQUIRE(!nf_retc_consumer_admit(requested,&altered,law,owner,&cursor));
   REQUIRE(cursor.last_tick==before.last_tick &&
           cursor.last_revision==before.last_revision &&
           cursor.last_event_id==before.last_event_id);
   ++denials;
   cursor=original;
   REQUIRE(nf_retc_consumer_admit(role,&good,300u,400u,&cursor));
   REQUIRE(cursor.last_tick==100u&&cursor.last_revision==200u&&
           cursor.last_event_id==500u);
   ++controls;
   REQUIRE(!nf_retc_consumer_admit(role,&good,300u,400u,&cursor));
   ++replays;
   printf("H295 consumer=%u case=%u rejected=1 admitted_control=1 replay_denied=1\n",role_value,case_id);
  }
 }
 printf("H295 PASS 50 cases x 3 consumers: %u denials %u positive controls %u replay denials\n",denials,controls,replays);
 return !(denials==150u&&controls==150u&&replays==150u);
}
