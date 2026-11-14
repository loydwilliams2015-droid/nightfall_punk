#ifndef NF_RETC_CONSUMER_AUTHORITY_H
#define NF_RETC_CONSUMER_AUTHORITY_H
/* H295 research contract: read-only admission for MAP/AUDIO/RENDER.
 * Does NOT publish A6, grant B2, commit B3, journal, or operate devices.
 * A6 must supply actual accepted immutable evidence in production; until
 * connected, these are adversarial contract tests only. */
#include <stdbool.h>
#include <stdint.h>

typedef enum NfRetcConsumer {
 NF_RETC_CONSUMER_MAP=1,
 NF_RETC_CONSUMER_AUDIO=2,
 NF_RETC_CONSUMER_RENDER=3
} NfRetcConsumer;
typedef struct NfRetcAcceptedWitness {
 uint64_t tick, revision, constitution_revision, owner_generation, event_id;
 bool published;
} NfRetcAcceptedWitness;
typedef struct NfRetcConsumerCursor {
 uint64_t last_tick, last_revision, last_event_id;
} NfRetcConsumerCursor;
/* Consuming an immutable world-event cursor never changes the witness.
 * Local UI-only sounds do not use this authoritative world event contract.
 * An actual A6 renderer/event bus must supply the already-published witness. */
static inline bool nf_retc_consumer_admit(
 NfRetcConsumer role, const NfRetcAcceptedWitness *accepted,
 uint64_t expected_constitution, uint64_t expected_owner_generation,
 NfRetcConsumerCursor *cursor)
{
 if(!accepted||!cursor||
    (role!=NF_RETC_CONSUMER_MAP &&
     role!=NF_RETC_CONSUMER_AUDIO &&
     role!=NF_RETC_CONSUMER_RENDER)||
    !accepted->published||
    !accepted->tick||!accepted->revision||
    !accepted->constitution_revision||!accepted->owner_generation||
    !accepted->event_id||!expected_constitution||
    !expected_owner_generation||
    accepted->constitution_revision!=expected_constitution||
    accepted->owner_generation!=expected_owner_generation||
    accepted->tick<=cursor->last_tick||
    accepted->revision<=cursor->last_revision||
    accepted->event_id<=cursor->last_event_id)
   return false;
 *cursor=(NfRetcConsumerCursor){
   accepted->tick,accepted->revision,accepted->event_id};
 return true;
}
#endif
