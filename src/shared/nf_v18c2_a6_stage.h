#ifndef NF_V18C2_A6_STAGE_H
#define NF_V18C2_A6_STAGE_H
/* Research-only read/stage guard; never publishes a second world tick. */
#include "nf_embody18a6.h"
#include "nf_multi18c1.h"
#include "nf_interact18b3.h"
typedef struct {
 uint32_t source_tick,source_revision,material_epoch;
 NfEntityId actor; uint32_t action;
 Nf18b3Kind kind;
 NfMulti18c1Candidate physical;
} Nf18c2A6Stage;
typedef enum {NF18C2_STAGE_READY,NF18C2_STAGE_PENDING,
 NF18C2_STAGE_STALE,NF18C2_STAGE_DENIED} Nf18c2StageStatus;
/* Calls real A4 staging; requires explicit externally certified B2 grant.
 * The grant parameter is only an H3 diagnostic and NOT a durable credential.
 * No mutation of world or runtime, and no B3 realization. */
Nf18c2StageStatus nf18c2_a6_stage(const NfWorld *w,const Nf18a6Runtime *r,
 const NfMulti18c1World *island,const NfMulti18c1Proposal *p,size_t n,
 const Nf18a4Constraint *contacts,size_t nc,float dt,
 uint32_t action,Nf18b3Kind kind,bool b2_granted,
 Nf18c2A6Stage *out);
#endif
