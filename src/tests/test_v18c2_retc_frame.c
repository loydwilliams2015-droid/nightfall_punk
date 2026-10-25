#include "nf_c2_retc_frame.h"
#include "nf_retc_joint_provider.h"
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
 Nf18a6Runtime *owner=calloc(1,sizeof *owner);CK(owner);
 Nf18b2EncounterRuntime *encounter=calloc(1,sizeof *encounter);CK(encounter);
 owner->actor_id=10;owner->physical.world.tick=0;
 encounter->owner=owner;encounter->prepared=1;
 encounter->staged.contract_epoch=7;
 encounter->staged.revision=3;
 encounter->staged.account_count=1;
 encounter->staged.accounts[0].actor_id=10;
 encounter->staged.accounts[0].balance=41;
 CK(nf_retc_bind_encounter(f,encounter));
 CK(!nf_retc_bind_encounter(f,encounter));
 encounter->staged.accounts[0].balance=1000;
 CK(f->encounter_registry.accounts[0].balance==41);
 /* Composite provider requires one A6 lease and both source domains. */
 NfWorld *live=calloc(1,sizeof *live);CK(live);
 live->tick=0;live->tick_in_progress=true;live->tick_owner_context=owner;
  encounter->world=live;
 Nf18b6Push push={0};push.runtime=owner;push.armed=true;
 push.staged_commit_valid=true;push.contract_epoch=7;push.staged_commit=f->event;
 NfRetcJointContext joint={0};
 joint.owner=owner;joint.encounter=encounter;joint.push=&push;
 joint.use=use;joint.use.tick=1;joint.use.action_id=91;
 joint.use.world_revision=f->physical.world.revision;
 joint.use.material_epoch=f->physical.world.grid.global_revision;
 joint.decision=decision;joint.actor=actor;joint.object=object;
 joint.authority=authority;joint.transaction_id=1;
 owner->retc_publisher=&publisher;
 owner->retc_journal="/tmp/nightfall_retc_joint_owner_test.bin";
 encounter->prepared=0;
 CK(nf_retc_joint_stage(live,&f->physical,&joint)==NULL); /* unprepared */
 encounter->prepared=1;
 NfRetcFrame *combined=nf_retc_joint_stage(live,&f->physical,&joint);
 CK(combined&&combined->has_b2_contract&&combined->has_encounter_registry);
 CK(combined->encounter_registry.accounts[0].balance==1000);
 /* The owner-derived API rejects missing actual encounter grant state. */
 CK(!nf_retc_joint_stage_owned(live,&f->physical,&joint));
 encounter->queue_count=1;encounter->granted[0]=1;
 encounter->queue[0].intent=(Nf18b2Intent){10,2010,91,NF18B2_PUSH};
 encounter->queue[0].witness=joint.use;
 encounter->queue[0].trace_index=0;encounter->trace_count=1;
 encounter->traces[0].revalidation=decision;
 encounter->frame.scene.actor_count=1;
 encounter->frame.scene.object_count=1;
 encounter->frame.scene.actors=&encounter->frame.actors[0];
 encounter->frame.scene.objects=&encounter->frame.objects[0];
 encounter->frame.actors[0]=actor;encounter->frame.objects[0]=object;
 NfRetcFrame *owned=nf_retc_joint_stage_owned(live,&f->physical,&joint);
 CK(owned&&owned->has_encounter_registry&&owned->b2_actor.id==10);
 free(owned);
 encounter->queue[0].intent.action_id=999;
 CK(!nf_retc_joint_stage_owned(live,&f->physical,&joint)); /* forged action */
 push.staged_commit_valid=false;
 CK(!nf_retc_joint_stage(live,&f->physical,&joint)); /* missing B3 */
 free(combined);
 live->tick_in_progress=false;
 CK(nf_retc_joint_attach(live,&joint));
 CK(owner->owner_hooks.begin!=NULL&&owner->owner_hooks.stage_joint!=NULL);
 CK(owner->owner_hooks.finish==NULL); /* no split finish authority */
 CK(!nf_retc_joint_attach(live,&joint));
 CK(nf_retc_joint_detach(live,&joint));
 CK(owner->owner_hooks.context==NULL);
 free(live);
 CK(nf_retc_publish(&publisher,f));
 free(encounter);free(owner);
 CK(nf_retc_read(&publisher)==f&&nf_retc_read(&publisher)->epoch==1);
 CK(!nf_retc_publish(&publisher,f)); /* no replay */
 p->world.tick=2;event.tick=2;event.action=92;
 p->world.tick=1;event.tick=1;
 NfRetcFrame *other=nf_retc_stage(p,&event,2,0,&obligation,1);CK(other);
 p->world.tick=2;event.tick=2;
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
 char journal[160];snprintf(journal,sizeof journal,"/tmp/nightfall_retc_frame_%u.bin",(unsigned)other->transaction_id);
 remove(journal);
 CK(nf_retc_commit_file(&publisher,other,journal));
 NfRetcFrame *disk=malloc(sizeof *disk);CK(disk);
 CK(nf_retc_restore_file(journal,disk));
 CK(disk->epoch==other->epoch&&disk->event.action==other->event.action&&
    disk->has_b2_contract&&disk->b2_actor.inventory==other->b2_actor.inventory);
 CK(!nf_retc_commit_file(&publisher,other,journal)); /* replay */
 free(disk);CK(remove(journal)==0);
 CK(nf_retc_read(&publisher)->epoch==2);
 CK(nf_retc_read(&publisher)->event.action==92);
 nf_retc_dispose(&publisher);free(p);
 CK(nf_retc_read(&publisher)==NULL);
 puts("RET-C immutable frame stage/publish/replay and obligation copy PASS");
 return 0;
}
