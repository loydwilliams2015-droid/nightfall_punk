#include "nf_region.h"

#include <float.h>
#include <math.h>
#include <string.h>

static void add_neighbor(NfRegionGraph *graph, uint8_t a, uint8_t b) {
    if (graph == NULL || a >= graph->count || b >= graph->count || a == b) return;
    NfRegion *region = &graph->regions[a];
    for (uint8_t i = 0u; i < region->neighbor_count; ++i) {
        if (region->neighbors[i] == b) return;
    }
    if (region->neighbor_count < NF_REGION_MAX_NEIGHBORS) {
        region->neighbors[region->neighbor_count++] = b;
    }
}

static void connect(NfRegionGraph *graph, uint8_t a, uint8_t b) {
    add_neighbor(graph, a, b);
    add_neighbor(graph, b, a);
}

void nf_region_graph_init_spatial_lab(NfRegionGraph *graph) {
    if (graph == NULL) return;
    memset(graph, 0, sizeof(*graph));

    static const float xs[4] = {-187.0f, -61.0f, 67.0f, 190.0f};
    static const float zs[6] = {-334.0f, -204.0f, -72.0f, 67.0f, 204.0f, 334.0f};
    static const float exposure[24] = {
        0.62f,0.48f,0.55f,0.66f,
        0.44f,0.33f,0.46f,0.52f,
        0.58f,0.38f,0.31f,0.57f,
        0.51f,0.29f,0.36f,0.49f,
        0.47f,0.35f,0.42f,0.55f,
        0.64f,0.46f,0.53f,0.61f
    };
    static const float objective[24] = {
        0.18f,0.22f,0.27f,0.16f,
        0.28f,0.34f,0.40f,0.26f,
        0.24f,0.46f,0.58f,0.31f,
        0.30f,0.52f,0.64f,0.36f,
        0.25f,0.42f,0.49f,0.29f,
        0.17f,0.23f,0.32f,0.19f
    };

    graph->count = NF_REGION_MAX;
    for (uint8_t row = 0u; row < 6u; ++row) {
        for (uint8_t col = 0u; col < 4u; ++col) {
            const uint8_t id = (uint8_t)(row*4u + col);
            NfRegion *region = &graph->regions[id];
            region->id = id;
            region->center = (NfVec3){
                xs[col] + ((row & 1u) ? (float)((int)col-1)*4.0f : 0.0f),
                0.05f,
                zs[row] + ((col & 1u) ? 5.0f : -3.0f)
            };
            region->structural_exposure = exposure[id];
            region->objective_value = objective[id];
        }
    }

    for (uint8_t row = 0u; row < 6u; ++row) {
        for (uint8_t col = 0u; col < 4u; ++col) {
            const uint8_t id = (uint8_t)(row*4u + col);
            if (col + 1u < 4u) connect(graph, id, (uint8_t)(id+1u));
            if (row + 1u < 6u) connect(graph, id, (uint8_t)(id+4u));
        }
    }

    connect(graph, 1u, 6u);
    connect(graph, 5u, 10u);
    connect(graph, 9u, 14u);
    connect(graph, 13u, 18u);
    connect(graph, 6u, 9u);
    connect(graph, 10u, 13u);
    connect(graph, 14u, 17u);
    connect(graph, 18u, 21u);
}

uint8_t nf_region_nearest(const NfRegionGraph *graph, NfVec3 position) {
    if (graph == NULL || graph->count == 0u) return NF_REGION_INVALID;
    float best_distance = FLT_MAX;
    uint8_t best = NF_REGION_INVALID;
    for (size_t i = 0u; i < graph->count; ++i) {
        const float dx = position.x - graph->regions[i].center.x;
        const float dz = position.z - graph->regions[i].center.z;
        const float distance = dx*dx + dz*dz;
        if (distance < best_distance) {
            best_distance = distance;
            best = (uint8_t)i;
        }
    }
    return best;
}

bool nf_region_are_neighbors(const NfRegionGraph *graph, uint8_t a, uint8_t b) {
    if (graph == NULL || a >= graph->count || b >= graph->count) return false;
    const NfRegion *region = &graph->regions[a];
    for (uint8_t i = 0u; i < region->neighbor_count; ++i) {
        if (region->neighbors[i] == b) return true;
    }
    return false;
}

int nf_region_path_steps(const NfRegionGraph *graph, uint8_t from, uint8_t to) {
    if (graph == NULL || from >= graph->count || to >= graph->count) return -1;
    if (from == to) return 0;

    int distance[NF_REGION_MAX];
    uint8_t queue[NF_REGION_MAX];
    for (size_t i = 0u; i < NF_REGION_MAX; ++i) distance[i] = -1;
    size_t head = 0u;
    size_t tail = 0u;
    distance[from] = 0;
    queue[tail++] = from;

    while (head < tail) {
        const uint8_t current = queue[head++];
        const NfRegion *region = &graph->regions[current];
        for (uint8_t i = 0u; i < region->neighbor_count; ++i) {
            const uint8_t next = region->neighbors[i];
            if (next >= graph->count || distance[next] >= 0) continue;
            distance[next] = distance[current] + 1;
            if (next == to) return distance[next];
            queue[tail++] = next;
        }
    }
    return -1;
}

uint8_t nf_region_next_hop(const NfRegionGraph *graph, uint8_t from, uint8_t to) {
    if (graph == NULL || from >= graph->count || to >= graph->count) return NF_REGION_INVALID;
    if (from == to) return to;

    int distance[NF_REGION_MAX];
    uint8_t parent[NF_REGION_MAX];
    uint8_t queue[NF_REGION_MAX];
    for (size_t i = 0u; i < NF_REGION_MAX; ++i) {
        distance[i] = -1;
        parent[i] = NF_REGION_INVALID;
    }

    size_t head = 0u;
    size_t tail = 0u;
    distance[from] = 0;
    queue[tail++] = from;

    while (head < tail) {
        const uint8_t current = queue[head++];
        const NfRegion *region = &graph->regions[current];
        for (uint8_t i = 0u; i < region->neighbor_count; ++i) {
            const uint8_t next = region->neighbors[i];
            if (next >= graph->count || distance[next] >= 0) continue;
            distance[next] = distance[current] + 1;
            parent[next] = current;
            queue[tail++] = next;
        }
    }

    if (distance[to] < 0) return NF_REGION_INVALID;
    uint8_t cursor = to;
    while (parent[cursor] != NF_REGION_INVALID && parent[cursor] != from) {
        cursor = parent[cursor];
    }
    return parent[cursor] == from ? cursor : NF_REGION_INVALID;
}

const char *nf_region_name(uint8_t region) {
    static const char *names[NF_REGION_MAX] = {
        "South Freight", "South Yard", "South Works", "South Tower",
        "Freight Annex", "Lower Works", "Glass Court", "Relay Yard",
        "West Campus", "Lower Commons", "Central Works", "East Campus",
        "West Gantries", "Central Commons", "Signal Court", "East Gantries",
        "North Works", "Upper Commons", "Service Yard", "North Relay",
        "North Freight", "North Court", "North Works Annex", "North Tower"
    };
    return region < NF_REGION_MAX ? names[region] : "INVALID";
}
