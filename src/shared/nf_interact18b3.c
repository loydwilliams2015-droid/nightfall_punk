#include "nf_interact18b3.h"
Nf18b3Result nf18b3_authorize(Nf18b3Event *e,bool p,bool c,bool unknown){
 if(!e||!e->actor||!e->object||!e->action||e->kind>=NF18B3_KIND_COUNT||
    e->phase!=NF18B3_PROPOSED)return NF18B3_INVALID;
 if(unknown){e->phase=NF18B3_WAITING;return NF18B3_PENDING;}
 if(!p||!c){e->phase=NF18B3_REJECTED;return NF18B3_DENIED;}
 e->authorized=1;e->phase=NF18B3_WAITING;return NF18B3_OK;
}
Nf18b3Result nf18b3_realize(Nf18b3Event *e,uint32_t w,uint32_t m,uint32_t c,
 Nf18b3AppliedReceipt apply,void *ctx){
 if(!e||!apply||!e->authorized)return NF18B3_INVALID;
 if(e->applied)return NF18B3_DUPLICATE;
 if(e->phase!=NF18B3_WAITING)return NF18B3_INVALID;
 if(w!=e->world_epoch||m!=e->material_epoch||c!=e->contract_epoch){
  e->phase=NF18B3_REJECTED;return NF18B3_DENIED;
 }
 /* Integration obligation: callback must be A6-owner-bound, checked A3/A5,
    and publish true A4/material effects. H0 does not supply that backend. */
 if(!apply(ctx,e))return NF18B3_PENDING;
 e->applied=1;e->phase=NF18B3_APPLIED;return NF18B3_OK;
}
Nf18b3Result nf18b3_interrupt(Nf18b3Event *e){
 if(!e||e->phase==NF18B3_INTERRUPTED||e->phase==NF18B3_REJECTED)
  return NF18B3_INVALID;
 if(e->phase!=NF18B3_WAITING&&e->phase!=NF18B3_APPLIED)return NF18B3_INVALID;
 e->phase=NF18B3_INTERRUPTED;return NF18B3_OK;
}
