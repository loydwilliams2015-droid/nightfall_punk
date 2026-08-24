#include "nf_spatial_review.h"

#include <stdio.h>

static int failures = 0;

static void expect_true(bool value, const char *name) {
    if (!value) {
        fprintf(stderr, "FAIL: %s\n", name);
        ++failures;
    }
}

static void expect_int(int actual, int expected, const char *name) {
    if (actual != expected) {
        fprintf(stderr, "FAIL: %s actual=%d expected=%d\n", name, actual, expected);
        ++failures;
    }
}

int main(void) {
    NfSpatialReviewTrace a;
    NfSpatialReviewTrace b;
    NfSpatialReviewResult ia = nf_spatial_review_run(
        NF_SPATIAL_LAB_COVER, NF_SPATIAL_REVIEW_INSTRUMENTED, 200001u, 0u, &a);
    NfSpatialReviewResult ib = nf_spatial_review_run(
        NF_SPATIAL_LAB_COVER, NF_SPATIAL_REVIEW_INSTRUMENTED, 200001u, 1u, &b);
    expect_int((int)ia.deterministic_hash, (int)ib.deterministic_hash, "instrumented duplicate deterministic");
    expect_true(a.path_len > 1u, "instrumented trace records path");
    expect_int((int)ia.path_invalid_steps, 0, "instrumented path remains material-valid");

    NfSpatialReviewResult pa = nf_spatial_review_run(
        NF_SPATIAL_LAB_ECOLOGY, NF_SPATIAL_REVIEW_PERCEPTUAL, 240001u, 0u, &a);
    NfSpatialReviewResult pb = nf_spatial_review_run(
        NF_SPATIAL_LAB_ECOLOGY, NF_SPATIAL_REVIEW_PERCEPTUAL, 240001u, 1u, &b);
    expect_int((int)pa.deterministic_hash, (int)pb.deterministic_hash, "perceptual duplicate deterministic");
    expect_int((int)pa.path_invalid_steps, 0, "perceptual path remains material-valid");

    NfSpatialReviewResult ga = nf_spatial_review_run(
        NF_SPATIAL_LAB_LOOP, NF_SPATIAL_REVIEW_GREEDY, 210001u, 0u, NULL);
    NfSpatialReviewResult ra = nf_spatial_review_run(
        NF_SPATIAL_LAB_LOOP, NF_SPATIAL_REVIEW_RANDOM, 210001u, 0u, NULL);
    expect_true(ga.steps > 0u, "greedy control runs");
    expect_true(ra.steps > 0u, "random negative control runs");

    if (failures != 0) {
        fprintf(stderr, "nightfall v1.7A graphical review contracts: FAIL (%d)\n", failures);
        return 1;
    }
    printf("nightfall v1.7A graphical review contracts: PASS\n");
    printf("REVIEW instrumented=PASS perceptual=PASS greedy=PASS random=PASS deterministic=PASS material_path=PASS\n");
    return 0;
}
