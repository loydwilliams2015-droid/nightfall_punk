#ifndef NF_INTERACTION18B3_H
#define NF_INTERACTION18B3_H
#include "nf_encounter18b2.h"

#define NF18B3_MAX_OBJECTS 16u
#define NF18B3_MAX_ACTIONS 64u
#define NF18B3_MAX_EVENTS 128u
typedef enum Nf18b3Kind {
    NF18B3_DOOR_OPEN=0, NF18B3_SWITCH, NF18B3_PICKUP,
    NF18B3_PUSH, NF18B3_CARRY, NF18B3_REPAIR, NF18B3_DOOR_CLOSE,
    NF18B3_TRAVERSAL
} Nf18b3Kind;
typedef enum Nf18b3Status {
    NF18B3_PROPOSED=0, NF18B3_EXECUTING, NF18B3_COMPLETED,
    NF18B3_BLOCKED, NF18B3_PENDING, NF18B3_EXPIRED, NF18B3_JAMMED,
    NF18B3_CANCELLED, NF18B3_COMPENSATION_REQUIRED, NF18B3_UNSUPPORTED,
    NF18B3_INVALID, NF18B3_DUPLICATE, NF18B3_PUBLISH_FAILED
} Nf18b3Status;
typedef struct Nf18b3Object {
    uint32_t id,revision,collider_index;
    Nf18b3Kind kind;
    Nf18a4Body body;
    NfVec3 origin;
    float travel,speed,max_force,max_carry_mass;
    uint32_t units,health,max_health,carried_by;
    uint8_t available,on,jammed,rotated,requires_power;
} Nf18b3Object;
typedef struct Nf18b3Action {
    uint32_t actor_id,object_id,action_id,start_tick,completion_tick,authorization_trace;
    uint32_t expected_object_revision,contract_epoch,motor_receipts,contact_receipts,material_effects;
    Nf18b3Kind kind;Nf18b3Status status,stop_reason;
    double motor_work;
    uint8_t published,cancel_requested,release_requested,return_stroke;
} Nf18b3Action;
/* Distinct causal records: motor work, actual contact and discrete material
   transfer. An ownership/material event is never fabricated as an A4 impulse. */
typedef struct Nf18b3Event {
    uint64_t sequence;
    uint32_t tick,actor_id,object_id,action_id;
    Nf18b3Kind kind;
    NfVec3 before,after;
    Nf18a4MotorReceipt motor;
    Nf18a4Receipt contact;
    NfVec3 external_actor_impulse,external_object_impulse;
    int32_t inventory_delta,health_delta;
    uint8_t ownership_transfer;
    uint8_t switch_changed,switch_state,power_state;
} Nf18b3Event;
typedef struct Nf18b3State {
    Nf18b3Object objects[NF18B3_MAX_OBJECTS];size_t object_count;
    Nf18b3Action actions[NF18B3_MAX_ACTIONS];size_t action_count;
    Nf18b3Event events[NF18B3_MAX_EVENTS];size_t event_count;
    uint64_t next_event,acknowledged_event;
    uint32_t active,inventory_units,salvage_units;
    double power_joules,spent_joules;
    uint8_t power_enabled;
} Nf18b3State;
typedef struct Nf18b3Runtime {
    NfWorld *world;Nf18a6Runtime *owner;Nf18b2EncounterRuntime *authority;
    Nf18b2Model model;
    NfWorldTickOwner physical_owner;
    Nf18b3State state,staged,before_state;
    /* Reused private scratch: no allocation or second LIVE owner during ticks. */
    NfWorld trial_world,before_world;
    Nf18a6Runtime trial_owner,before_owner;
    Nf18b2EncounterRuntime trial_authority;
    Nf18b2Registry before_registry;
    uint64_t owner_calls,geometry_queries,material_queries;
    bool (*publication_validator)(const NfWorld *,const Nf18b3State *,void *);
    void *publication_context;
    uint8_t bound;
} Nf18b3Runtime;

bool nf18b3_init(Nf18b3Runtime *,NfWorld *,Nf18a6Runtime *,Nf18b2EncounterRuntime *,double,uint32_t);
/* Registers server-authored physical objects and B2 descriptors. No client
   permission, physics pose, cost or resource state is accepted by request(). */
bool nf18b3_register(Nf18b3Runtime *,Nf18b3Object,Nf18b2Object);
bool nf18b3_bind(Nf18b3Runtime *);
Nf18b3Status nf18b3_request(Nf18b3Runtime *,uint32_t,uint32_t,uint32_t,Nf18b3Kind);
bool nf18b3_cancel(Nf18b3Runtime *,uint32_t,uint32_t,bool);
bool nf18b3_observe(const Nf18b3Runtime *,size_t,Nf18b3Action *);
bool nf18b3_ack_events(Nf18b3Runtime *,uint64_t);
const char *nf18b3_status_name(Nf18b3Status);
uint32_t nf18b3_semantic_hash(const Nf18b3Runtime *);
#endif
