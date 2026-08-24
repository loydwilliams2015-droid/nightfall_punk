#include "nf_spatial_review.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static NfSpatialLabKind parse_lab(const char *name) {
    for (int lab = 0; lab < NF_SPATIAL_LAB_COUNT; ++lab) {
        if (strcmp(name, nf_spatial_lab_name((NfSpatialLabKind)lab)) == 0) {
            return (NfSpatialLabKind)lab;
        }
    }
    return NF_SPATIAL_LAB_COUNT;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s selections.csv review.csv\n", argv[0]);
        return 2;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL) {
        perror(argv[1]);
        return 2;
    }
    FILE *out = fopen(argv[2], "w");
    if (out == NULL) {
        perror(argv[2]);
        fclose(in);
        return 2;
    }

    fprintf(out,
        "lab,category,seed,selection_score,mode,replicate,success,steps,unique_cells,revisits,retreats,"
        "support_transitions,affordance_accepts,topology_updates,route_discovery_step,cue_disagreement_steps,"
        "path_invalid_steps,health_remaining,exposure_accum,ecology_cost_accum,resource_collected,"
        "contract_context_seen,deterministic_hash\n");

    char line[512];
    if (fgets(line, sizeof(line), in) == NULL) {
        fprintf(stderr, "empty selections file\n");
        fclose(in);
        fclose(out);
        return 2;
    }

    unsigned long long rows = 0u;
    unsigned selected = 0u;
    while (fgets(line, sizeof(line), in) != NULL) {
        char lab_name[32];
        char category[32];
        unsigned seed = 0u;
        double selection_score = 0.0;
        int parsed = sscanf(line, "%31[^,],%31[^,],%u,%lf", lab_name, category, &seed, &selection_score);
        if (parsed != 4) {
            fprintf(stderr, "invalid selection row: %s", line);
            fclose(in);
            fclose(out);
            return 2;
        }
        NfSpatialLabKind lab = parse_lab(lab_name);
        if (lab == NF_SPATIAL_LAB_COUNT) {
            fprintf(stderr, "unknown lab: %s\n", lab_name);
            fclose(in);
            fclose(out);
            return 2;
        }
        ++selected;

        for (int mode = 0; mode < NF_SPATIAL_REVIEW_MODE_COUNT; ++mode) {
            for (uint8_t rep = 0u; rep < 2u; ++rep) {
                NfSpatialReviewResult r = nf_spatial_review_run(
                    lab,
                    (NfSpatialReviewMode)mode,
                    (uint32_t)seed,
                    rep,
                    NULL);
                fprintf(out,
                    "%s,%s,%u,%.6f,%s,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,"
                    "%.6f,%.6f,%.6f,%.6f,%.6f,%u\n",
                    lab_name,
                    category,
                    seed,
                    selection_score,
                    nf_spatial_review_mode_name((NfSpatialReviewMode)mode),
                    rep,
                    r.success,
                    r.steps,
                    r.unique_cells,
                    r.revisits,
                    r.retreats,
                    r.support_transitions,
                    r.affordance_accepts,
                    r.topology_updates,
                    r.route_discovery_step,
                    r.cue_disagreement_steps,
                    r.path_invalid_steps,
                    r.health_remaining,
                    r.exposure_accum,
                    r.ecology_cost_accum,
                    r.resource_collected,
                    r.contract_context_seen,
                    r.deterministic_hash);
                ++rows;
            }
        }
    }

    fclose(in);
    fclose(out);
    printf("nightfall v1.7A graphical AI review: %llu rows from %u selected seeds -> %s\n",
           rows, selected, argv[2]);
    if (selected != 40u || rows != 320u) {
        fprintf(stderr, "expected 40 seeds and 320 deterministic review rows\n");
        return 1;
    }
    return 0;
}
