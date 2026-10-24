#include "nf_c2_retc_frame.h"
#include <stdio.h>
#include <stdlib.h>
#define CK(x) do {if(!(x)){fprintf(stderr,"RET-C fail line %d\n",__LINE__);return 1;}}while(0)
int main(void){
 NfRetcPublisher publisher={0};
 CK(nf_retc_read(&publisher)==NULL);
 Nf18a5Integrated *p=calloc(1,sizeof *p);CK(p);
 p->world.tick=1;p->actor.id=10;p->object.id=2010;
 p->committed_receipts=1;
 Nf18b3Event event={0};
 event.actor=10;event.object=2010;event.tick=1;event.action=91;
 event.contract_epoch=7;event.phase=NF18B3_APPLIED;
 event.authorized=1;event.applied=1;event.kind=NF18B3_PUSH;
 NfRetcObligation obligation={.obligation_id=99,.holder=10,
   .beneficiary=20,.kind=1,.quantity=2,.status=1};
 CK(!nf_retc_stage(p,&event,1,0,&obligation,9));
 event.authorized=0;
 CK(!nf_retc_stage(p,&event,1,0,&obligation,1));
 event.authorized=1;
 NfRetcFrame *f=nf_retc_stage(p,&event,1,0,&obligation,1);CK(f);
 CK(nf_retc_read(&publisher)==NULL); /* staging is invisible */
 CK(f->derived_actor_hash&&f->derived_contract_hash);
 p->committed_receipts=18;event.action=444;obligation.quantity=88;
 CK(f->physical.committed_receipts==1&&f->event.action==91&&
    f->obligations[0].quantity==2); /* immutable source copy */
 Nf18b2Actor actor={.id=10,.revision=5,.credentials=3,.inventory=7};
 Nf18b2Object object={.id=2010,.revision=6,.capacity=1};
 Nf18b2Use use={.actor_id=10,.object_id=2010,.action_id=91,
  .tick=1,.actor_revision=5,.object_revision=6};
 Nf18b2UseDecision decision={.status=NF18B2_ELIGIBLE,
  .physical=NF18B2_ELIGIBLE,.contract=NF18B2_ELIGIBLE};
 Nf18b2Authority authority={0};
 CK(nf_retc_bind_b2(f,&use,&decision,&actor,&object,&authority));
 CK(f->has_b2_contract&&f->b2_actor.inventory==7);
 CK(!nf_retc_bind_b2(f,&use,&decision,&actor,&object,&authority));
 actor.inventory=99;CK(f->b2_actor.inventory==7);
 CK(nf_retc_seal_b2_b3(f));
 f->event.kind=NF18B3_PICKUP;
 CK(!nf_retc_seal_b2_b3(f));
 CK(!nf_retc_publish(&publisher,f));
 CK(nf_retc_read(&publisher)==NULL);
 f->event.kind=NF18B3_PUSH;
 CK(nf_retc_publish(&publisher,f));
 CK(nf_retc_read(&publisher)==f&&nf_retc_read(&publisher)->epoch==1);
 CK(!nf_retc_publish(&publisher,f)); /* no replay */
 p->world.tick=2;event.tick=2;event.action=92;
 NfRetcFrame *other=nf_retc_stage(p,&event,2,0,&obligation,1);CK(other);
 CK(!nf_retc_publish(&publisher,other)); /* stale epoch */
 CK(nf_retc_read(&publisher)==f);free(other);
 other=nf_retc_stage(p,&event,2,1,NULL,0);CK(other);
 CK(!nf_retc_publish(&publisher,other)); /* no generic-only authority */
 CK(nf_retc_read(&publisher)==f);free(other);
 use.tick=2;use.action_id=92;
 other=nf_retc_stage_source_push(p,&event,2,1,
     &use,&decision,&actor,&object,&authority);CK(other);
 CK(other->has_b2_contract&&other->obligation_count==0);
 CK(other->b2_actor.inventory==99);
 CK(nf_retc_publish(&publisher,other));
 CK(nf_retc_read(&publisher)->epoch==2);
 CK(nf_retc_read(&publisher)->event.action==92);
 nf_retc_dispose(&publisher);free(p);
 CK(nf_retc_read(&publisher)==NULL);
 puts("RET-C immutable frame stage/publish/replay and obligation copy PASS");
 return 0;
}
