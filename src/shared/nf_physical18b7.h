#ifndef NF_PHYSICAL18B7_H
#define NF_PHYSICAL18B7_H
/* Group 2 H0: discrete interaction model; NOT A4/A5/A6 physical integration. */
#include <stdbool.h>
#include <stdint.h>
#include "nf_interact18b3.h"
#define NF7F_RECEIPTS 64u

typedef enum { NF7F_APPLIED, NF7F_DUPLICATE, NF7F_PENDING, NF7F_DENIED,
               NF7F_STALE, NF7F_INVALID, NF7F_FULL, NF7F_PUBLISH_FAILED } Nf7fStatus;
typedef struct {
 uint32_t actor, action, world_epoch, material_epoch, contract_epoch;
 Nf18b3Kind kind;
 int32_t argument;
 uint8_t physical_ok, contract_ok, uncertain;
} Nf7fRequest;
typedef struct { Nf7fRequest request; uint32_t completed_epoch; } Nf7fReceipt;
typedef struct {
 uint32_t world_epoch, material_epoch, contract_epoch, tick;
 /* Door is an axis-aligned slider, integer position [0,100]. */
 int32_t door_position, door_obstacle_low, door_obstacle_high;
 uint8_t door_latched, door_obstructed;
 /* Material/switch state and coarse deterministic resource accounting. */
 uint8_t power_known, powered, switch_on, floor_supported;
 uint32_t held_by;
 int64_t actor_momentum, object_momentum;
 uint32_t health, supplies;
 uint32_t interrupted_actions;
 uint32_t receipt_count;
 Nf7fReceipt receipts[NF7F_RECEIPTS];
} Nf7fWorld;
typedef bool (*Nf7fPublish)(void *ctx,const Nf7fWorld *candidate);
void nf7f_init(Nf7fWorld *world);
/* Caller owns single writer; publish must be atomic and nonreentrant. */
Nf7fStatus nf7f_apply(Nf7fWorld *world,Nf7fRequest request,Nf7fPublish publish,void *ctx);
#endif
