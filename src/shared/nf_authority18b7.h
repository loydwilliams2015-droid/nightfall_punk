#ifndef NF_AUTHORITY18B7_H
#define NF_AUTHORITY18B7_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Standalone 1.8B.7A laboratory: no A6 integration, physical effects, or disk WAL. */
#define NF7_OBJECTS 16u
#define NF7_REQUESTS 32u
#define NF7_RECEIPTS 64u

typedef enum { NF7_DENIED=0, NF7_ELIGIBLE=1, NF7_UNKNOWN=2 } Nf7Evidence;
typedef enum {
 NF7_COMMITTED=0, NF7_DUPLICATE, NF7_INVALID, NF7_STALE, NF7_PENDING,
 NF7_REFUSED, NF7_CONTESTED, NF7_FULL, NF7_PERSIST_FAIL
} Nf7Status;
typedef enum { NF7_LOW_ACTOR=0, NF7_ROTATING_ACTOR=1 } Nf7Policy;

typedef struct {
 uint32_t id, generation, revision, material_identity_epoch, owner;
 int64_t quantity;
} Nf7Object;
typedef struct {
 uint32_t actor, action, object, generation, epoch, revision;
 int32_t debit;
 Nf7Evidence physical, contractual;
} Nf7Request;
typedef struct {
 Nf7Request request;
 uint32_t committed_epoch;
} Nf7Receipt;
typedef struct {
 uint32_t epoch, tick, cursor;
 size_t object_count, receipt_count;
 Nf7Object objects[NF7_OBJECTS];
 Nf7Receipt receipts[NF7_RECEIPTS];
} Nf7World;
/* A true callback promises full durable acceptance. This library cannot verify it. */
typedef bool (*Nf7Persist)(void *, const Nf7World *);
void nf7_init(Nf7World *world);
bool nf7_add_object(Nf7World *world, Nf7Object object);
bool nf7_world_valid(const Nf7World *world);
/* Mutations occur only after a successful persistence callback, never in the
   request-validation loop. All requests inspect the same original snapshot.
   The owner must hold a lock and must NOT reenter from the callback. */
bool nf7_batch(Nf7World *world, const Nf7Request *requests, size_t count,
 Nf7Policy policy, Nf7Persist persist, void *context, Nf7Status *results);
#endif
