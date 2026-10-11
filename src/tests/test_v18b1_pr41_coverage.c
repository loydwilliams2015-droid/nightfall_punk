/* Supplemental coverage for the exact PR #41 copy. Not its original test. */
#include "../reproduction/v18b1/nf_smart18b1.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static unsigned next(unsigned *state) { *state = 1664525u * *state + 1013904223u; return *state; }
int main(void) {
    unsigned accepted = 0, refused = 0, historical_positive = 0;
    for (unsigned seed = 1; seed <= 6000; ++seed) {
        unsigned state = seed;
        unsigned locked = next(&state) & 1u, known = next(&state) & 1u;
        unsigned support = next(&state) & 1u, owner = next(&state) & 1u;
        unsigned reach = next(&state) & 1u, clearance = next(&state) & 1u;
        unsigned contract = next(&state) & 1u, observed = next(&state) & 1u;
        historical_positive += known && support && reach && clearance && contract && observed && (!locked || owner);
    }
    CHECK(historical_positive == 0);
    for (unsigned bits = 0; bits < 256; ++bits) {
        NfB1World world; nf_b1_init(&world); world.tick = 5; world.world_revision = 10;
        NfB1Object object = {.id=10, .material_identity_epoch=27, .material_revision=1, .owner=7,
            .locked=(bits>>6)&1u, .material_known=bits&1u, .support_valid=(bits>>1)&1u};
        NfB1Intent intent = {.actor_id=(bits&128u)?7u:8u, .object_id=10, .tick=5,
            .expected_revision=10, .observed_material_epoch=27, .action=NF_B1_OPEN,
            .physically_reachable=(bits>>2)&1u, .clearance=(bits>>3)&1u,
            .has_contract=(bits>>4)&1u, .actor_observed=(bits>>5)&1u};
        CHECK(nf_b1_add(&world, object));
        NfB1World before = world;
        NfB1Decision decision = nf_b1_evaluate(&world, &intent);
        NfB1Status expected = !object.material_known || !intent.actor_observed ? NF_B1_PENDING :
            (!object.support_valid || !intent.physically_reachable || !intent.clearance || !intent.has_contract ||
             (object.locked && intent.actor_id != object.owner)) ? NF_B1_BLOCKED : NF_B1_OK;
        CHECK(decision.status == expected);
        if (expected == NF_B1_OK) {
            NfB1Decision forged = decision; forged.witness ^= 1u;
            CHECK(nf_b1_commit(&world, &intent, forged) == NF_B1_STALE);
            CHECK(memcmp(&world, &before, sizeof world) == 0);
            CHECK(nf_b1_commit(&world, &intent, decision) == NF_B1_OK);
            CHECK(world.event_count == 1 && world.objects[0].open == 1 && world.world_revision == 11);
            CHECK(nf_b1_commit(&world, &intent, decision) == NF_B1_STALE);
            CHECK(world.event_count == 1); ++accepted;
        } else {
            CHECK(nf_b1_commit(&world, &intent, decision) == expected);
            CHECK(memcmp(&world, &before, sizeof world) == 0); ++refused;
        }
    }
    CHECK(accepted == 3 && refused == 253);
    printf("PR41 seed positive cases=%u/6000; supplemental exhaustive cases=256; commits=%u; refusals=%u; PASS\n",
           historical_positive, accepted, refused);
    return 0;
}
