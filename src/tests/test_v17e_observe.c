#include "nf_observe17e.h"

#include <stdio.h>
#include <string.h>

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

static Nf17eOverlayContribution contribution(
    uint32_t anchor,
    Nf17eOverlayDimension dim,
    uint16_t priority,
    uint32_t source) {

    Nf17eOverlayContribution c;
    memset(&c, 0, sizeof(c));
    c.anchor_id = anchor;
    c.anchor_kind = (uint8_t)NF17E_ANCHOR_CELL;
    c.dimension = (uint8_t)dim;
    c.priority = priority;
    c.source_id = source;
    c.authority_hash = 1000u + source;
    c.payload_hash = 2000u + source;
    return c;
}

int main(void) {
    Nf17eOverlayPolicy policy;
    nf17e_overlay_policy_default(&policy);

    Nf17eOverlayContribution stack[4];
    stack[0] = contribution(7u, NF17E_DIM_WORLD, 0u, 1u);
    stack[1] = contribution(7u, NF17E_DIM_BELIEF, 0u, 2u);
    stack[2] = contribution(7u, NF17E_DIM_CONFLICT, 0u, 3u);
    stack[3] = contribution(7u, NF17E_DIM_PENDING, 0u, 4u);
    stack[3].pending = 1u;

    Nf17eOverlayObject objects[NF17E_MAX_OVERLAY_OBJECTS];
    const size_t count = nf17e_compose_overlays(&policy, stack, 4u, objects, NF17E_MAX_OVERLAY_OBJECTS);

    expect_int((int)count, 1, "same anchor composes to one multidimensional object");
    expect_int((int)objects[0].source_count, 4, "all overlay sources retained");
    expect_true(nf17e_overlay_object_contains(&objects[0], NF17E_DIM_WORLD),
                "suppressed world dimension retained");
    expect_true(nf17e_overlay_object_contains(&objects[0], NF17E_DIM_BELIEF),
                "suppressed belief dimension retained");
    expect_true(nf17e_overlay_object_primary(&objects[0], NF17E_DIM_PENDING),
                "pending priority becomes visible primary");
    expect_true((objects[0].suppressed_mask & (1u << NF17E_DIM_CONFLICT)) != 0u,
                "nonpriority conflict is visually suppressed but retained");
    expect_int((int)objects[0].composite_kind,
               (int)NF17E_COMPOSITE_PRIORITY_SUPPRESSED,
               "different priorities form suppressed composite");

    Nf17eOverlayContribution equal[3];
    equal[0] = contribution(9u, NF17E_DIM_BELIEF, 40u, 10u);
    equal[1] = contribution(9u, NF17E_DIM_CONTRACT, 35u, 11u);
    equal[2] = contribution(9u, NF17E_DIM_WORLD, 0u, 12u);

    const size_t equal_count =
        nf17e_compose_overlays(&policy, equal, 3u, objects, NF17E_MAX_OVERLAY_OBJECTS);
    expect_int((int)equal_count, 1, "equal-priority stack composes");
    expect_true(nf17e_overlay_object_primary(&objects[0], NF17E_DIM_BELIEF),
                "belief remains co-primary at equal effective priority");
    expect_true(nf17e_overlay_object_primary(&objects[0], NF17E_DIM_CONTRACT),
                "contract remains co-primary at equal effective priority");
    expect_int((int)objects[0].composite_kind,
               (int)NF17E_COMPOSITE_RAINBOW,
               "equal primary plus lower dimensions yields rainbow composite");

    Nf17eOverlayContribution separate[2];
    separate[0] = contribution(100u, NF17E_DIM_WORLD, 0u, 20u);
    separate[1] = contribution(101u, NF17E_DIM_WORLD, 0u, 21u);
    expect_int((int)nf17e_compose_overlays(
                   &policy, separate, 2u, objects, NF17E_MAX_OVERLAY_OBJECTS),
               2, "different anchors remain different objects");

    Nf17eOverlayContribution invariant = contribution(33u, NF17E_DIM_INVARIANT, 0u, 30u);
    Nf17eOverlayContribution purple = contribution(33u, NF17E_DIM_PURPLE, 100u, 31u);
    Nf17eOverlayContribution high_stack[2] = {purple, invariant};
    nf17e_compose_overlays(&policy, high_stack, 2u, objects, NF17E_MAX_OVERLAY_OBJECTS);
    expect_true(nf17e_overlay_object_primary(&objects[0], NF17E_DIM_INVARIANT),
                "invariant semantic boost outranks decorative/manual priority");

    if (failures != 0) {
        fprintf(stderr, "nightfall v1.7E overlay compositor: FAIL (%d)\n", failures);
        return 1;
    }

    printf("nightfall v1.7E overlay compositor: PASS\n");
    printf("OBSERVE compose=PASS suppression=PASS rainbow=PASS anchors=PASS invariant=PASS\n");
    return 0;
}
