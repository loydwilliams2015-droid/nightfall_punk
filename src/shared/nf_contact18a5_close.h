#ifndef NF_CONTACT18A5_CLOSE_H
#define NF_CONTACT18A5_CLOSE_H
#include "nf_contact18a5.h"

/* H1 integration harness. Material grid witnesses are for STATIC geometry;
   the dynamic crate is resolved separately by 1.8A.4 capsule CCD. */
typedef bool (*Nf18a5FineProvider)(void *ctx,int32_t cx,int32_t cy,int32_t cz,
                                   uint32_t authoritative_parent_epoch,
                                   uint8_t *scale,uint8_t voxels[NF18A5_FINE_MAX_VOXELS],
                                   size_t *count);
typedef enum Nf18a5CloseStatus {
    NF18A5_CLOSE_COMMITTED=0, NF18A5_CLOSE_BLOCKED,
    NF18A5_CLOSE_PENDING, NF18A5_CLOSE_STALE, NF18A5_CLOSE_INVALID,
    NF18A5_CLOSE_DISK_FAILED, NF18A5_CLOSE_GATED
} Nf18a5CloseStatus;
typedef struct Nf18a5Integrated {
    Nf18a5World world;
    Nf18a4Body actor, object;
    uint32_t committed_receipts;
} Nf18a5Integrated;
typedef struct Nf18a5CloseResult {
    Nf18a5CloseStatus status;
    Nf18a5Query query;
    Nf18a4Receipt applied;
    uint32_t cache_loads;
    uint32_t material_revision_before,material_revision_after;
    uint8_t dynamic_contact;
    NfVec3 external_impulse; /* gravity / environmental acceleration source */
} Nf18a5CloseResult;

void nf18a5_integrated_init(Nf18a5Integrated *s, uint8_t chunk_capacity,
                            Nf18a4Body actor,Nf18a4Body object);
/* Load actual fine voxels only for a currently resident canonical MIXED cell.
   Missing/stale/failed load returns PENDING or INVALID, never clearance. */
Nf18a5Query nf18a5_resolve_exact(Nf18a5Grid *grid,NfVec3 probe,uint32_t tick,
                                  Nf18a5FineProvider provider,void *provider_ctx,
                                  uint32_t *load_count);
/* Bounded static-material coverage, using actual A3 capsule CCD against all
   canonical/fine voxels in the swept hull. Cache fills are not material writes.
   Missing/stale data and a frontier over 4096 voxels return PENDING. */
Nf18a5Query nf18a5_sweep_query(Nf18a5Grid *grid,NfVec3 feet,
    NfVec3 displacement,Nf18a2ShapePolicy shape,float dt,uint32_t tick,
    Nf18a5FineProvider provider,void *context,uint32_t *load_count);
/* The journal is a single-process, same-ABI snapshot WAL: durable before publish.
   Recovery accepts complete snapshots and FAILS CLOSED on torn/corrupt tails.
   This is NOT a portable/replicated production world WAL. */
bool nf18a5_checkpoint(const char *path,const Nf18a5Integrated *state);
bool nf18a5_restore(const char *path,Nf18a5Integrated *state);
/* Opt-in H1 prepublication gate: called after physical and contact-history
   staging but BEFORE the A5 WAL and caller-state publication. A rejecting
   validator cannot leave behind a committed newer snapshot. */
typedef bool (*Nf18a5PrepublishGate)(const Nf18a5Integrated *candidate,void *ctx);
Nf18a5CloseResult nf18a5_integrated_pair_step_checked(
    Nf18a5Integrated *state,uint32_t expected_revision,uint32_t tick,
    NfVec3 static_world_probe,Nf18a5FineProvider provider,void *provider_ctx,
    Nf18a2ShapePolicy shape,Nf18a4Motor motor,float dt,
    float restitution,float friction,const char *world_journal,
    Nf18a5PrepublishGate gate,void *gate_ctx);
Nf18a5CloseResult nf18a5_integrated_pair_step(
    Nf18a5Integrated *state, uint32_t expected_revision, uint32_t tick,
    NfVec3 static_world_probe, Nf18a5FineProvider provider,void *provider_ctx,
    Nf18a2ShapePolicy shape,Nf18a4Motor motor,float dt,
    float restitution,float friction,const char *world_journal);
/* Resting support is a solver APPLIED impulse with verified upward normal.
   Its impulse/dt is a support FORCE; it is not automatically an impact. */
Nf18a5CloseResult nf18a5_integrated_support_step(
    Nf18a5Integrated *state,uint32_t expected_revision,uint32_t tick,
    Nf18a4Constraint support,Nf18a3Contract support_authority,
    NfVec3 authoritative_gravity,
    float dt,const char *world_journal);

#endif
