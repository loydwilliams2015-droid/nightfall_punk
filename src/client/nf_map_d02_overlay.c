/* NIGHTFALL!PUNK MAP-D02 visual-only mesh overlay, C11 / raylib 5.5.
 * No physics / authoritative world mutations. The parser deliberately
 * accepts only exporter-produced vertices, object labels and triangle faces.
 * This does not establish GIS provenance or production A6 admission. */
#include "nf_map_d02_overlay.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NF_MESH_MAX_VERTICES 256u
#define NF_MESH_MAX_TRIANGLES 512u
#define NF_MESH_MAX_FILE_BYTES (1024u * 1024u)

bool nf_map_d02_overlay_load(NfMapD02Overlay *overlay, const char *path) {
    if (overlay == NULL || path == NULL || path[0] == '\0' || overlay->ready ||
        !IsWindowReady()) return false;
    FILE *file = fopen(path, "rb");
    if (!file) return false;
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return false; }
    long length = ftell(file);
    if (length <= 0 || length > (long)NF_MESH_MAX_FILE_BYTES ||
        fseek(file, 0, SEEK_SET) != 0) { fclose(file); return false; }
    Vector3 positions[NF_MESH_MAX_VERTICES];
    unsigned position_count = 0, triangle_count = 0;
    float *triangles = MemAlloc(NF_MESH_MAX_TRIANGLES * 9u * sizeof(float));
    if (!triangles) { fclose(file); return false; }
    bool valid = true;
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        size_t line_len = strlen(line);
        if (!line_len || (line[line_len - 1] != '\n' && !feof(file))) {
            valid = false; break;
        }
        if (line[0] == 'v' && line[1] == ' ') {
            float x,y,z;char extra;
            if (position_count >= NF_MESH_MAX_VERTICES ||
                sscanf(line, "v %f %f %f %c", &x,&y,&z,&extra) != 3 ||
                !isfinite(x)||!isfinite(y)||!isfinite(z)) { valid = false; break; }
            positions[position_count++] = (Vector3){x,y,z};
        } else if (line[0] == 'f' && line[1] == ' ') {
            unsigned a,b,c;char extra;
            if (triangle_count >= NF_MESH_MAX_TRIANGLES ||
                sscanf(line, "f %u %u %u %c", &a,&b,&c,&extra) != 3 ||
                !a||!b||!c||a>position_count||b>position_count||c>position_count||
                a==b||a==c||b==c) { valid = false; break; }
            const Vector3 points[3] = {positions[a-1u],positions[b-1u],positions[c-1u]};
            for (unsigned j=0;j<3u;j++) {
                const unsigned k=triangle_count*9u+j*3u;
                triangles[k]=points[j].x;
                triangles[k+1u]=points[j].y;
                triangles[k+2u]=points[j].z;
            }
            triangle_count++;
        } else if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' ||
                   (line[0] == 'o' && line[1] == ' ')) {
            continue;
        } else { valid = false; break; }
    }
    if (ferror(file)) valid=false;
    fclose(file);
    if (!valid || triangle_count == 0u) { MemFree(triangles); return false; }
    Mesh mesh={0};
    mesh.vertices=triangles;
    mesh.vertexCount=(int)(triangle_count*3u);
    mesh.triangleCount=(int)triangle_count;
    UploadMesh(&mesh,false);
    if (mesh.vaoId == 0u) { UnloadMesh(mesh); return false; }
    overlay->model=LoadModelFromMesh(mesh);
    if (overlay->model.meshCount != 1) {
        UnloadModel(overlay->model);
        memset(overlay,0,sizeof(*overlay));
        return false;
    }
    overlay->ready=true;
    overlay->triangles=triangle_count;
    return true;
}
void nf_map_d02_overlay_draw(const NfMapD02Overlay *overlay) {
    if (!overlay || !overlay->ready) return;
    /* MAP-D02 export uses outward/upward triangle winding; preserve caller GL state. */
    DrawModel(overlay->model,(Vector3){0,0,0},1.0f,(Color){140,189,191,255});
}
void nf_map_d02_overlay_unload(NfMapD02Overlay *overlay) {
    if (!overlay) return;
    if (overlay->ready) UnloadModel(overlay->model);
    memset(overlay,0,sizeof(*overlay));
}
