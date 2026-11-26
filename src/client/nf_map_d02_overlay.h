#ifndef NF_MAP_D02_OVERLAY_H
#define NF_MAP_D02_OVERLAY_H
#include <stdbool.h>
#include "raylib.h"
typedef struct NfMapD02Overlay {
    Model model;
    bool ready;
    unsigned triangles;
} NfMapD02Overlay;
/* Call after InitWindow. Visual-only; never adds collision geometry. */
bool nf_map_d02_overlay_load(NfMapD02Overlay *overlay, const char *obj_path);
/* Must be called between BeginMode3D and EndMode3D. */
void nf_map_d02_overlay_draw(const NfMapD02Overlay *overlay);
void nf_map_d02_overlay_unload(NfMapD02Overlay *overlay);
#endif
