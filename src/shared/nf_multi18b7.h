#ifndef NF_MULTI18B7_H
#define NF_MULTI18B7_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* M01–M11 H0: bounded contract/arbitration experiment, not a live A6 owner.
   Outcomes are prospective grants, never applied A4/A5 effects. */
#define NF_M7_ACTORS 8u
#define NF_M7_SLOTS 4u
#define NF_M7_CAPACITY 2u
#define NF_M7_BATCH 32u
#define NF_M7_RECEIPTS 64u

typedef enum { NF_M7_NO=0, NF_M7_YES=1, NF_M7_UNKNOWN=2 } NfM7Evidence;
typedef enum { NF_M7_CLAIM=1, NF_M7_RELEASE=2 } NfM7Operation;
typedef enum {
 NF_M7_GRANTED=0, NF_M7_DUPLICATE, NF_M7_CONTESTED,
 NF_M7_PENDING, NF_M7_REFUSED, NF_M7_STALE,
 NF_M7_INVALID, NF_M7_FULL, NF_M7_PUBLISH_FAILED
} NfM7Status;
typedef enum { NF_M7_FIXED_ID=0, NF_M7_ROTATING=1 } NfM7Policy;

typedef struct { uint32_t id, session; uint8_t active; } NfM7Actor;
typedef struct {
 uint32_t id, revision;
 uint8_t capacity, occupant_count;
 uint32_t occupant[NF_M7_CAPACITY];
} NfM7Slot;
typedef struct {
 uint32_t actor, session, action, slot, epoch, slot_revision;
 NfM7Operation operation;
 NfM7Evidence physical, contract;
 uint8_t consent;
} NfM7Request;
typedef struct { NfM7Request request; uint32_t committed_epoch; } NfM7Receipt;
typedef struct {
 uint32_t epoch, tick, cursor;
 size_t actor_count, slot_count, receipt_count;
 NfM7Actor actors[NF_M7_ACTORS];
 NfM7Slot slots[NF_M7_SLOTS];
 NfM7Receipt receipts[NF_M7_RECEIPTS];
} NfM7World;
/* Callback promises durable, failure-atomic persistence; H0 uses mocks only.
   The one true A6 owner must lock around the entire call, prohibit callback
   reentry, and derive evidence from trusted server-side state. */
typedef bool (*NfM7Publisher)(void *,const NfM7World *);
void nf_m7_init(NfM7World *);
bool nf_m7_add_actor(NfM7World *,NfM7Actor);
bool nf_m7_add_slot(NfM7World *,NfM7Slot);
bool nf_m7_valid(const NfM7World *);
bool nf_m7_step(NfM7World *,const NfM7Request *,size_t,NfM7Policy,
                NfM7Publisher,void *,NfM7Status *);
#endif
