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
 event.authorized=1;event.applied=1;
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
 CK(nf_retc_publish(&publisher,f));
 CK(nf_retc_read(&publisher)==f&&nf_retc_read(&publisher)->epoch==1);
 CK(!nf_retc_publish(&publisher,f)); /* no replay */
 p->world.tick=2;event.tick=2;event.action=92;
 NfRetcFrame *other=nf_retc_stage(p,&event,2,0,&obligation,1);CK(other);
 CK(!nf_retc_publish(&publisher,other)); /* stale epoch */
 CK(nf_retc_read(&publisher)==f);free(other);
 other=nf_retc_stage(p,&event,2,1,&obligation,1);CK(other);
 CK(nf_retc_publish(&publisher,other));
 CK(nf_retc_read(&publisher)->epoch==2);
 CK(nf_retc_read(&publisher)->event.action==92);
 nf_retc_dispose(&publisher);free(p);
 CK(nf_retc_read(&publisher)==NULL);
 puts("RET-C immutable frame stage/publish/replay and obligation copy PASS");
 return 0;
}
