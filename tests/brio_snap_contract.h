/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef NF_BRIO_SNAP_CONTRACT_H
#define NF_BRIO_SNAP_CONTRACT_H
#include <stdint.h>
#include <stddef.h>
/* Pure, non-authoritative research prefilter. All positions mm, velocities mm/s.
   This never awards B2 grants, issues A4/A5 contact, or commits A6 state. */
typedef enum { NF_BRIO_DENY=0, NF_BRIO_ALLOW=1, NF_BRIO_PENDING=2 } nf_brio_gate;
typedef struct {
    uint32_t surface_id;
    int32_t x_mm, z_mm;
    int32_t tangent_x, tangent_z; /* bounded to [-1000,1000] by admission */
    nf_brio_gate body_clear, material_clear, grant_current;
} nf_brio_surface;
typedef struct {
    int32_t x_mm, z_mm;
    int32_t vx_mm_s, vz_mm_s;
    uint32_t snap_radius_mm;
    int32_t min_forward_dot; /* signed orientation dot in scaled mm/s */
} nf_brio_actor;
typedef struct { uint32_t surface_id; int64_t distance_sq_mm; int32_t forward_dot; } nf_brio_choice;
/* Returns 1 for a candidate, 0 for no candidate, -1 for invalid arguments. */
int nf_brio_choose(const nf_brio_actor *actor, const nf_brio_surface *surfaces,
                   size_t count, nf_brio_choice *out);
#endif
