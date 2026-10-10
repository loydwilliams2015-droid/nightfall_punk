#ifndef NF_SMART18B1_H
#define NF_SMART18B1_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
/* Clean 2026-10-10 reproduction. Not the lost historical source. */
#define NF_B1_MAX_OBJECTS 32u
#define NF_B1_MAX_EVENTS 64u
typedef enum {NF_B1_OK=0,NF_B1_BLOCKED,NF_B1_PENDING,NF_B1_STALE,NF_B1_INVALID,NF_B1_FULL,NF_B1_DUPLICATE} NfB1Status;
typedef enum {NF_B1_OPEN=1,NF_B1_CLOSE=2,NF_B1_LADDER=3,NF_B1_PUSH=4} NfB1Action;
typedef struct {uint32_t id,material_identity_epoch,material_revision,owner;uint8_t locked,open,material_known,support_valid;} NfB1Object;
typedef struct {uint32_t actor_id,object_id,tick,expected_revision,observed_material_epoch;NfB1Action action;uint8_t physically_reachable,clearance,has_contract,actor_observed;} NfB1Intent;
typedef struct {uint32_t tick,actor_id,object_id,revision_before,revision_after,digest;NfB1Action action;} NfB1Event;
typedef struct {NfB1Object objects[NF_B1_MAX_OBJECTS];NfB1Event events[NF_B1_MAX_EVENTS];uint32_t tick,world_revision;size_t object_count,event_count;} NfB1World;
typedef struct {NfB1Status status;uint32_t witness,material_identity_epoch;} NfB1Decision;
void nf_b1_init(NfB1World *w);
bool nf_b1_add(NfB1World *w,NfB1Object obj);
NfB1Decision nf_b1_evaluate(const NfB1World *w,const NfB1Intent *intent);
NfB1Status nf_b1_commit(NfB1World *w,const NfB1Intent *intent,NfB1Decision prior);
uint32_t nf_b1_event_digest(const NfB1Event *e);
#endif
