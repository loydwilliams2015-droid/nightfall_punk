#ifndef NF_ENCOUNTER18B2_H
#define NF_ENCOUNTER18B2_H
/* EEC-01 opt-in, local authorization transaction inside the existing A6 owner.
   New source; not the lost B1 code. Single physical actor scope is preserved. */
#include "nf_embody18a6.h"
#include "nf_eligibility18b2.h"

typedef enum Nf18b2EncounterState {
    NF18B2_PROPOSED=0, NF18B2_EVALUATING, NF18B2_ENCOUNTER_BLOCKED,
    NF18B2_ENCOUNTER_PENDING, NF18B2_ENCOUNTER_ELIGIBLE, NF18B2_EXPIRED,
    NF18B2_RESERVED, NF18B2_REVALIDATING, NF18B2_CANCELLED,
    NF18B2_COMMIT_PENDING, NF18B2_COMMITTED, NF18B2_FAILED_NO_EFFECT,
    NF18B2_EXECUTING, NF18B2_COMPLETED, NF18B2_INTERRUPTED,
    NF18B2_COMPENSATION_REQUIRED
} Nf18b2EncounterState;
typedef enum Nf18b2IntentKind { NF18B2_USE=0, NF18B2_PUSH, NF18B2_LADDER } Nf18b2IntentKind;
/* Client proposals contain identifiers only, never permission or material facts.
   actor_id comes from the authenticated server session at the network boundary. */
typedef struct Nf18b2Intent { uint32_t actor_id,object_id,action_id; Nf18b2IntentKind kind; } Nf18b2Intent;
typedef struct Nf18b2Account {
    uint32_t actor_id,revision,credentials,inventory,balance;
    Nf18b2Actor last_pose;
} Nf18b2Account;
typedef struct Nf18b2RegisteredObject {
    Nf18b2Object object;
    uint32_t material_identity_epoch,cost,consume_inventory;
    NfVec3 target_offset;
    uint8_t follows_physical_object;
} Nf18b2RegisteredObject;
typedef struct Nf18b2Registry {
    Nf18b2Account accounts[NF18B2_MAX_REQUESTS];
    Nf18b2RegisteredObject objects[NF18B2_MAX_REQUESTS];
    size_t account_count,object_count;
    uint32_t revision,contract_epoch,geometry_epoch;
} Nf18b2Registry;
typedef struct Nf18b2EncounterTrace {
    Nf18b2Intent intent;
    uint32_t assessed_tick,commit_tick,world_before,world_after;
    uint32_t material_epoch,material_identity_epoch,contract_epoch,actor_revision,object_revision;
    uint32_t history_before,history_after,applied_receipts,balance_before,balance_after;
    NfVec3 feet_before,feet_after;
    Nf18b2UseDecision authorization,revalidation;
    Nf18b2EncounterState state;
    Nf18b2EncounterState transitions[12];
    uint8_t transition_count,published,physical_wal,visual_correction,deferred_milestone;
    Nf18a6Status physical_status;
} Nf18b2EncounterTrace;
typedef struct Nf18b2QueuedIntent {
    Nf18b2Intent intent;
    Nf18b2Use witness;
    uint32_t registry_revision,contract_epoch,geometry_epoch;
    size_t trace_index;
    uint8_t withdrawn;
} Nf18b2QueuedIntent;
typedef struct Nf18b2LiveFrame {
    Nf18b2Actor actors[NF18B2_MAX_REQUESTS];
    Nf18b2Object objects[NF18B2_MAX_REQUESTS];
    Nf18aCollider solids[NF_MAX_COLLIDERS],ladders[NF_MAX_COLLIDERS];
    Nf18b2Scene scene;
} Nf18b2LiveFrame;
#define NF18B2_MAX_TRACES 128u
typedef struct Nf18b2EncounterRuntime {
    NfWorld *world;
    Nf18a6Runtime *owner;
    Nf18b2Model model;
    Nf18b2Registry registry,staged;
    Nf18b2LiveFrame frame;
    Nf18aCollider observed_solids[NF_MAX_COLLIDERS];
    Nf18aCollider observed_ladders[NF_MAX_COLLIDERS];
    size_t observed_count,observed_ladder_count;
    Nf18b2QueuedIntent queue[NF18B2_MAX_REQUESTS];
    Nf18b2EncounterTrace traces[NF18B2_MAX_TRACES];
    size_t queue_count,trace_count,grant_count;
    uint32_t grant_actors[64],grant_objects[64],grant_actions[64];
    uint8_t granted[64],prepared,attached;
    /* Trusted read-only commit validator. False => no publication. It must not
       mutate external state. It is not a distributed/durable registry publisher. */
    bool (*publication_validator)(const Nf18a5Integrated *,void *);
    void *publication_context;
} Nf18b2EncounterRuntime;

bool nf18b2_encounter_init(Nf18b2EncounterRuntime *,NfWorld *,Nf18a6Runtime *,Nf18b2Model);
bool nf18b2_register_actor(Nf18b2EncounterRuntime *,uint32_t,uint32_t,uint32_t,uint32_t);
bool nf18b2_register_object(Nf18b2EncounterRuntime *,Nf18b2Object,uint32_t,uint32_t,uint32_t);
bool nf18b2_set_contract(Nf18b2EncounterRuntime *,uint32_t,uint32_t,uint32_t);
bool nf18b2_touch_object(Nf18b2EncounterRuntime *,uint32_t);
/* Snapshots come from actual NfWorld bodies/colliders and the server ledger.
   Diagnostic revision refresh is not a resource/body/history publication. */
bool nf18b2_live_frame(Nf18b2EncounterRuntime *,Nf18b2LiveFrame *);
Nf18b2Status nf18b2_propose(Nf18b2EncounterRuntime *,Nf18b2Intent);
bool nf18b2_withdraw(Nf18b2EncounterRuntime *,uint32_t,uint32_t);
/* Attach to the already-bound A6 owner; never installs another movement owner. */
bool nf18b2_attach(Nf18b2EncounterRuntime *);
bool nf18b2_detach(Nf18b2EncounterRuntime *);
/* Post-commit interruption records a fact and need for compensation; it neither
   erases history nor invents the B3/B5 compensating physical transaction. */
bool nf18b2_interrupt(Nf18b2EncounterRuntime *,uint32_t,uint32_t);
bool nf18b2_observe(const Nf18b2EncounterRuntime *,size_t,Nf18b2EncounterTrace *);
const char *nf18b2_encounter_state_name(Nf18b2EncounterState);
#endif
