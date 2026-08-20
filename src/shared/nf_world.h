#ifndef NF_WORLD_H
#define NF_WORLD_H

#include "nf_energy.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_MAX_ENTITIES 1024u
#define NF_MAX_COLLIDERS 128u
#define NF_MAX_RAMPS 16u
#define NF_MAX_CONTAMINATION_TRACES 64u
#define NF_TICK_RATE 60u

typedef uint32_t NfEntityId;

typedef enum NfFaction {
    NF_FACTION_NONE = 0,
    NF_FACTION_PLAYER,
    NF_FACTION_TEAMMATE,
    NF_FACTION_RIVAL,
    NF_FACTION_CATTLER,
    /* Temporary v1.5 compatibility alias. New code must use CATTLER. */
    NF_FACTION_RANCHER = NF_FACTION_CATTLER
} NfFaction;

typedef enum NfMovementMode {
    NF_MOVE_GROUND = 0,
    NF_MOVE_SPRINT,
    NF_MOVE_CROUCH,
    NF_MOVE_AIR,
    NF_MOVE_LADDER,
    NF_MOVE_VAULT,
    NF_MOVE_MANTLE,
    NF_MOVE_PLATFORM
} NfMovementMode;

typedef enum NfColliderKind {
    NF_COLLIDER_SOLID = 0,
    NF_COLLIDER_LADDER,
    NF_COLLIDER_MOVING_PLATFORM
} NfColliderKind;

typedef enum NfTraversalType {
    NF_TRAVERSAL_NONE = 0,
    NF_TRAVERSAL_STEP,
    NF_TRAVERSAL_VAULT,
    NF_TRAVERSAL_MANTLE,
    NF_TRAVERSAL_LADDER
} NfTraversalType;

typedef enum NfRampAxis {
    NF_RAMP_POS_X = 0,
    NF_RAMP_NEG_X,
    NF_RAMP_POS_Z,
    NF_RAMP_NEG_Z
} NfRampAxis;

typedef enum NfWeaponId {
    NF_WEAPON_NONE = 0,
    NF_WEAPON_CARBINE = 1,
    NF_WEAPON_PISTOL = 2,
    NF_WEAPON_COUNT = 3
} NfWeaponId;

typedef enum NfWeaponState {
    NF_WEAPON_READY = 0,
    NF_WEAPON_RECOVERING,
    NF_WEAPON_RELOADING,
    NF_WEAPON_SWITCHING,
    NF_WEAPON_EMPTY
} NfWeaponState;

typedef enum NfHitZone {
    NF_HIT_NONE = 0,
    NF_HIT_BODY,
    NF_HIT_HEAD,
    NF_HIT_KNEE,
    NF_HIT_FOOT
} NfHitZone;

typedef enum NfAgencyState {
    NF_AGENCY_ACTIVE = 0,
    NF_AGENCY_CRITICAL,
    NF_AGENCY_STASIS,
    NF_AGENCY_REVIVING,
    NF_AGENCY_RETURNING
} NfAgencyState;

typedef enum NfContaminationPhenotype {
    NF_CONTAM_PHENOTYPE_NONE = 0,
    NF_CONTAM_PHENOTYPE_SURFACE = 1u << 0,
    NF_CONTAM_PHENOTYPE_DISSOLVED = 1u << 1,
    NF_CONTAM_PHENOTYPE_AIRBORNE = 1u << 2,
    NF_CONTAM_PHENOTYPE_FILAMENTOUS = 1u << 3,
    NF_CONTAM_PHENOTYPE_SIGNAL = 1u << 4
} NfContaminationPhenotype;

typedef enum NfInventoryFate {
    NF_INVENTORY_FATE_NONE = 0,
    NF_INVENTORY_FATE_BASE_RESTOCK,
    NF_INVENTORY_FATE_SITE_PERSIST,
    NF_INVENTORY_FATE_ECO_ABSORB
} NfInventoryFate;

typedef enum NfCombatEventType {
    NF_COMBAT_EVENT_NONE = 0,
    NF_COMBAT_EVENT_GUNFIRE,
    NF_COMBAT_EVENT_DAMAGE,
    NF_COMBAT_EVENT_STASIS,
    NF_COMBAT_EVENT_REVIVAL,
    NF_COMBAT_EVENT_RELOAD,
    NF_COMBAT_EVENT_WEAPON_SWITCH,
    /* Compatibility aliases: v1.1 player-facing semantics are STASIS/REVIVAL. */
    NF_COMBAT_EVENT_DEATH = NF_COMBAT_EVENT_STASIS,
    NF_COMBAT_EVENT_RESPAWN = NF_COMBAT_EVENT_REVIVAL
} NfCombatEventType;

typedef struct NfVec3 { float x, y, z; } NfVec3;
typedef struct NfTransform { NfVec3 position; NfVec3 velocity; } NfTransform;

typedef struct NfBodyContamination {
    float systemic;
    float locomotor;
    float manipulator;
    float sensory;
    float surface;
    float dissolved;
    float airborne;
    uint32_t phenotype_mask;
    uint64_t last_change_tick;
    uint64_t stasis_tick;
    float agency_timer;
    bool trace_pending;
} NfBodyContamination;

typedef struct NfStasisInventoryDisposition {
    bool valid;
    NfWeaponId site_weapon;
    NfWeaponId restock_weapon;
    uint16_t absorbed_ammo_units;
    NfInventoryFate site_fate;
    NfInventoryFate restock_fate;
    NfInventoryFate ammo_fate;
} NfStasisInventoryDisposition;

typedef struct NfContaminationTrace {
    bool active;
    uint32_t id;
    NfEntityId source;
    NfFaction source_faction;
    NfVec3 position;
    float surface;
    float dissolved;
    float airborne;
    uint32_t phenotype_mask;
    uint64_t created_tick;
    uint64_t last_change_tick;
    NfWeaponId site_weapon;
    uint16_t absorbed_ammo_units;
} NfContaminationTrace;

typedef struct NfContaminationSystem {
    uint32_t next_trace_id;
    float drained_sink;
    float decayed_sink;
    float absorbed_inventory_sink;
    NfContaminationTrace traces[NF_MAX_CONTAMINATION_TRACES];
} NfContaminationSystem;

typedef struct NfMoveInput {
    float forward;
    float strafe;
    float yaw_radians;
    bool jump_pressed;
    bool crouch_held;
    bool sprint_held;
    bool interact_held;
} NfMoveInput;

typedef struct NfCombatInput {
    bool fire_held;
    bool fire_pressed;
    bool reload_pressed;
    bool focus_held;
    uint8_t weapon_slot;
    float aim_pitch_radians;
} NfCombatInput;

typedef struct NfTraversalCandidate {
    NfTraversalType type;
    int feature_index;
    NfVec3 point;
    NfVec3 normal;
    float score;
    bool active;
} NfTraversalCandidate;

typedef struct NfMovementState {
    NfMovementMode mode;
    bool grounded;
    bool crouched;
    uint8_t jumps_used;
    float body_height;
    float eye_height;
    float traversal_timer;
    int attached_collider;
    int ground_collider;
    NfTraversalCandidate candidate;
} NfMovementState;

typedef struct NfCombatState {
    /* v1.1 compatibility projection: true means ordinary gameplay agency is available. */
    bool alive;
    NfWeaponId weapon;
    NfWeaponId pending_weapon;
    NfWeaponState state;
    uint16_t ammo_mag[NF_WEAPON_COUNT];
    uint16_t reserve_ammo[NF_WEAPON_COUNT];
    float action_timer;
    float reload_total;
    bool reload_committed;
    float respawn_timer;
    uint32_t last_fire_sequence;
    uint64_t last_fire_tick;
} NfCombatState;

/* v1.6B: actor-local authoritative causes. Focus is actor attention; recoil is per weapon.
   accepted_shot_sequence is material ancestry and is deliberately distinct from input sequence. */
typedef struct NfWeaponAuthorityState {
    float focus_amount;
    float instability_deg[NF_WEAPON_COUNT];
    uint32_t accepted_shot_sequence;
    NfVec3 previous_velocity;
    NfMovementMode previous_mode;
    bool previous_grounded;
    bool kinematic_initialized;
    float redirect_stress_deg;
    float support_stress_deg;
} NfWeaponAuthorityState;

typedef struct NfCombatEvent {
    uint32_t sequence;
    uint64_t server_tick;
    NfCombatEventType type;
    NfEntityId source;
    NfEntityId target;
    NfWeaponId weapon;
    NfHitZone hit_zone;
    float amount;
    NfVec3 position;
    uint16_t rewind_ms;
    /* v1.6B firearm provenance; zero for non-shot events. */
    uint32_t shot_sequence;
    NfVec3 shot_direction;
    bool shot_blocked;
} NfCombatEvent;

typedef struct NfActor {
    NfEntityId id;
    bool active;
    NfFaction faction;
    NfTransform transform;
    NfMoveInput input;
    NfMovementState movement;
    NfCombatState combat;
    NfWeaponAuthorityState weapon_authority;
    NfAgencyState agency;
    NfBodyContamination contamination;
    NfStasisInventoryDisposition stasis_inventory;
    /* Temporary protocol/debug projection: 100 * (1 - systemic contamination). */
    float health;
} NfActor;

typedef struct NfCollider {
    NfColliderKind kind;
    NfVec3 min;
    NfVec3 max;
    NfVec3 previous_min;
    NfVec3 previous_max;
    NfVec3 base_min;
    NfVec3 base_max;
    NfVec3 velocity;
    float motion_amplitude;
    float motion_period;
} NfCollider;

typedef struct NfRamp { NfVec3 min; NfVec3 max; NfRampAxis axis; } NfRamp;

typedef struct NfMovementConfig {
    float walk_speed;
    float sprint_speed;
    float crouch_speed;
    float ground_accel;
    float air_accel;
    float ground_brake;
    float gravity;
    float jump_speed;
    float second_jump_speed;
    float radius;
    float stand_height;
    float crouch_height;
    float stand_eye_height;
    float crouch_eye_height;
    float step_height;
    float vault_height;
    float mantle_height;
    float ladder_speed;
    float candidate_distance;
    float candidate_acquire_score;
    float candidate_retain_score;
} NfMovementConfig;

typedef struct NfWorld {
    uint64_t tick;
    uint32_t seed;
    NfEntityId next_entity_id;
    NfActor actors[NF_MAX_ENTITIES];
    NfCollider colliders[NF_MAX_COLLIDERS];
    size_t collider_count;
    NfRamp ramps[NF_MAX_RAMPS];
    size_t ramp_count;
    NfMovementConfig movement;
    NfEnergySystem energy;
    NfContaminationSystem contamination;
} NfWorld;

void nf_world_init(NfWorld *world, uint32_t seed);
void nf_world_build_movement_lab(NfWorld *world);
NfEntityId nf_world_spawn_actor(NfWorld *world, NfFaction faction, NfVec3 position);
NfEntityId nf_world_spawn_actor_with_id(NfWorld *world, NfEntityId id, NfFaction faction, NfVec3 position);
bool nf_world_despawn_actor(NfWorld *world, NfEntityId id);
NfActor *nf_world_find_actor(NfWorld *world, NfEntityId id);
const NfActor *nf_world_find_actor_const(const NfWorld *world, NfEntityId id);
void nf_world_set_input(NfWorld *world, NfEntityId id, NfMoveInput input);
void nf_world_step(NfWorld *world, float dt);
void nf_world_sync_dynamic_geometry(NfWorld *world);
size_t nf_world_active_actor_count(const NfWorld *world);

int nf_world_add_collider(NfWorld *world, NfColliderKind kind, NfVec3 min, NfVec3 max);
int nf_world_add_moving_platform(NfWorld *world, NfVec3 min, NfVec3 max, NfVec3 velocity_axis, float amplitude, float period);
int nf_world_add_ramp(NfWorld *world, NfVec3 min, NfVec3 max, NfRampAxis axis);

const char *nf_movement_mode_name(NfMovementMode mode);
const char *nf_traversal_type_name(NfTraversalType type);

#endif
