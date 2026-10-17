#ifndef NF_INTERACT18B3_H
#define NF_INTERACT18B3_H
/* 1.8B.3 H0: typed realization seam. Not an independent physics authority. */
#include <stdbool.h>
#include <stdint.h>
typedef enum {
 NF18B3_DOOR=0,NF18B3_SWITCH,NF18B3_PICKUP,NF18B3_PUSH,
 NF18B3_CARRY,NF18B3_REPAIR,NF18B3_INTERRUPT,NF18B3_KIND_COUNT
} Nf18b3Kind;
typedef enum {NF18B3_PROPOSED=0,NF18B3_WAITING,NF18B3_APPLIED,NF18B3_INTERRUPTED,
 NF18B3_REJECTED} Nf18b3Phase;
typedef enum {NF18B3_OK=0,NF18B3_PENDING,NF18B3_DENIED,NF18B3_INVALID,
 NF18B3_DUPLICATE} Nf18b3Result;
typedef struct {uint32_t actor,object,action,tick,world_epoch,material_epoch,contract_epoch;
 Nf18b3Kind kind;Nf18b3Phase phase;uint8_t authorized,applied;
} Nf18b3Event;
/* Provider is held within the existing A6 owner. A true result must refer
 * to a real, successfully published material consequence. A false result
 * must have no external side effect. This interface cannot prove that.
 */
typedef bool (*Nf18b3AppliedReceipt)(void *ctx,const Nf18b3Event *event);
Nf18b3Result nf18b3_authorize(Nf18b3Event *event,bool physical,bool contract,bool uncertain);
Nf18b3Result nf18b3_realize(Nf18b3Event *event,uint32_t world_epoch,
 uint32_t material_epoch,uint32_t contract_epoch,Nf18b3AppliedReceipt applied,void *ctx);
Nf18b3Result nf18b3_interrupt(Nf18b3Event *event);
#endif
