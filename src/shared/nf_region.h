#ifndef NF_REGION_H
#define NF_REGION_H

#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_REGION_MAX 24u
#define NF_REGION_MAX_NEIGHBORS 6u
#define NF_REGION_INVALID 255u

typedef struct NfRegion {
    uint8_t id;
    NfVec3 center;
    uint8_t neighbors[NF_REGION_MAX_NEIGHBORS];
    uint8_t neighbor_count;
    float structural_exposure;
    float objective_value;
} NfRegion;

typedef struct NfRegionGraph {
    NfRegion regions[NF_REGION_MAX];
    size_t count;
} NfRegionGraph;

void nf_region_graph_init_spatial_lab(NfRegionGraph *graph);
uint8_t nf_region_nearest(const NfRegionGraph *graph, NfVec3 position);
int nf_region_path_steps(const NfRegionGraph *graph, uint8_t from, uint8_t to);
uint8_t nf_region_next_hop(const NfRegionGraph *graph, uint8_t from, uint8_t to);
bool nf_region_are_neighbors(const NfRegionGraph *graph, uint8_t a, uint8_t b);
const char *nf_region_name(uint8_t region);

#endif
