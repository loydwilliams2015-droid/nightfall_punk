#include "nf_spatial_logic.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t parse_u32(const char *s, uint32_t fallback) {
    char *end = NULL;
    errno = 0;
    unsigned long v = strtoul(s, &end, 10);
    if (errno != 0 || end == s || *end != '\0' || v > 0xfffffffful) {
        return fallback;
    }
    return (uint32_t)v;
}

int main(int argc, char **argv) {
    const char *out_path = argc > 1 ? argv[1] : "v17a-samples.csv";
    uint32_t variants = argc > 2 ? parse_u32(argv[2], 128u) : 128u;
    if (variants < 8u) variants = 8u;
    if (variants > 4096u) variants = 4096u;
    FILE *out = fopen(out_path, "w");
    if (out == NULL) {
        perror(out_path);
        return 2;
    }
    fprintf(out, "seed,lab,policy,replicate,success,steps,unique_cells,revisits,retreats,support_transitions,affordance_accepts,topology_updates,health_remaining,exposure_accum,ecology_cost_accum,resource_collected,contract_context_seen,deterministic_hash\n");
    uint64_t rows = 0u;
    for (int lab = 0; lab < NF_SPATIAL_LAB_COUNT; ++lab) {
        for (uint32_t variant = 0u; variant < variants; ++variant) {
            uint32_t seed = 170000u + (uint32_t)lab * 10000u + variant * 37u;
            for (int policy = 0; policy < NF_SPATIAL_POLICY_COUNT; ++policy) {
                for (uint8_t rep = 0u; rep < 2u; ++rep) {
                    NfSpatialRoundResult r = nf_spatial_run_round((NfSpatialLabKind)lab, (NfSpatialPolicyKind)policy, seed, rep);
                    fprintf(out, "%u,%s,%s,%u,%u,%u,%u,%u,%u,%u,%u,%u,%.6f,%.6f,%.6f,%.6f,%.6f,%u\n",
                            r.seed, nf_spatial_lab_name((NfSpatialLabKind)r.lab), nf_spatial_policy_name((NfSpatialPolicyKind)r.policy),
                            r.replicate, r.success, r.steps, r.unique_cells, r.revisits, r.retreats,
                            r.support_transitions, r.affordance_accepts, r.topology_updates, r.health_remaining,
                            r.exposure_accum, r.ecology_cost_accum, r.resource_collected, r.contract_context_seen,
                            r.deterministic_hash);
                    ++rows;
                }
            }
        }
    }
    fclose(out);
    printf("nightfall v1.7A spatial logic laboratory: %llu AI-player rounds written to %s\n",
           (unsigned long long)rows, out_path);
    return 0;
}
