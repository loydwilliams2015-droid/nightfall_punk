/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "brio_snap_contract.h"
#include <limits.h>
static int allowed(nf_brio_gate x) { return x == NF_BRIO_ALLOW; }
int nf_brio_choose(const nf_brio_actor *a, const nf_brio_surface *s, size_t n, nf_brio_choice *out) {
    if (!a || !out || (n && !s) || a->snap_radius_mm > 10000u || a->min_forward_dot < 0) return -1;
    int found = 0;
    nf_brio_choice best = {0, 0, 0};
    const int64_t radius2 = (int64_t)a->snap_radius_mm * a->snap_radius_mm;
    for (size_t i=0; i<n; ++i) {
        const nf_brio_surface *v = &s[i];
        if (!v->surface_id || !allowed(v->body_clear) || !allowed(v->material_clear) || !allowed(v->grant_current)) continue;
        if (v->tangent_x < -1000 || v->tangent_x > 1000 || v->tangent_z < -1000 || v->tangent_z > 1000 ||
            (!v->tangent_x && !v->tangent_z)) continue;
        int64_t dx = (int64_t)a->x_mm - v->x_mm;
        int64_t dz = (int64_t)a->z_mm - v->z_mm;
        /* Bound differences before multiplication: subtraction itself uses int64. */
        if (dx > 10000 || dx < -10000 || dz > 10000 || dz < -10000) continue;
        int64_t d2 = dx*dx + dz*dz;
        if (d2 > radius2) continue;
        int64_t dot = (int64_t)a->vx_mm_s*v->tangent_x + (int64_t)a->vz_mm_s*v->tangent_z;
        /* int32 * 1000 * 2 fits int64; min_forward_dot provided in same units. */
        if (dot < a->min_forward_dot || dot > INT32_MAX || dot < INT32_MIN) continue;
        if (!found || d2 < best.distance_sq_mm || (d2 == best.distance_sq_mm && v->surface_id < best.surface_id)) {
            best = (nf_brio_choice){v->surface_id,d2,(int32_t)dot}; found=1;
        }
    }
    if (found) *out=best;
    return found;
}
