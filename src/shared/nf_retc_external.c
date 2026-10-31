#include "nf_retc_external.h"
#include <math.h>
bool nf_retc_external_valid(const NfRetcExternalRecord *r){
 if(!r||r->prior_epoch==UINT32_MAX||r->epoch!=r->prior_epoch+1u||
    !r->transaction_id||!r->actor_id||!r->object_id||!r->action_id||
    !r->contract_epoch||!r->committed_receipts||
    !r->account_count||r->account_count>NF18B2_MAX_REQUESTS||
    r->first_account_actor!=r->actor_id)return false;
 return isfinite(r->actor_center.x)&&isfinite(r->actor_center.y)&&
        isfinite(r->actor_center.z)&&isfinite(r->object_center.x)&&
        isfinite(r->object_center.y)&&isfinite(r->object_center.z);
}

bool nf_retc_external_capture(const NfRetcPublisher *publisher,NfRetcExternalRecord *out){
 if(!out)return false;
 *out=(NfRetcExternalRecord){0};
 NfRetcReadView v={0};
 if(!nf_retc_read_joint_view(publisher,&v)||!v.accepted||
    v.contract->account_count==0||v.contract->account_count>NF18B2_MAX_REQUESTS||
    v.contract->accounts[0].actor_id!=v.accepted->owner_actor)return false;
 const NfRetcFrame *f=v.accepted;
 NfRetcExternalRecord candidate={0};
 candidate.epoch=v.epoch;candidate.prior_epoch=f->prior_epoch;
 candidate.transaction_id=v.transaction_id;
 candidate.actor_id=f->owner_actor;candidate.object_id=f->object_id;
 candidate.action_id=v.event->action;candidate.contract_epoch=v.event->contract_epoch;
 candidate.world_revision=v.physical->world.revision;
 candidate.material_epoch=v.physical->world.grid.global_revision;
 candidate.committed_receipts=v.physical->committed_receipts;
 candidate.contract_revision=v.contract->revision;
 candidate.account_count=(uint32_t)v.contract->account_count;
 candidate.first_account_actor=v.contract->accounts[0].actor_id;
 candidate.first_account_balance=v.contract->accounts[0].balance;
 candidate.actor_center=v.physical->actor.center;
 candidate.object_center=v.physical->object.center;
 if(!nf_retc_external_valid(&candidate))return false;
 *out=candidate;
 return true;
}
