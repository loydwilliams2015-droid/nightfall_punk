#include "nf_camera.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static int nearf_test(float a, float b, float eps) {
    return fabsf(a - b) <= eps;
}

int main(void) {
    NfCameraState camera;
    nf_camera_init(&camera, 82.0f);
    assert(camera.mode == NF_CAMERA_FIRST_PERSON);
    assert(nearf_test(camera.current_vertical_fov_deg, 82.0f, 0.0001f));

    nf_camera_apply_look_delta(&camera, 10.0f, -5.0f, 0.0022f);
    assert(camera.yaw < 0.0f);
    assert(camera.pitch > 0.0f);
    const float yaw_after = camera.yaw;
    nf_camera_set_mode(&camera, NF_CAMERA_DEBUG_OBSERVER);
    nf_camera_apply_look_delta(&camera, 100.0f, 100.0f, 0.0022f);
    assert(nearf_test(camera.yaw, yaw_after, 0.0001f));
    nf_camera_set_mode(&camera, NF_CAMERA_FIRST_PERSON);

    NfWorld world;
    nf_world_init(&world, 20260813u);
    NfEntityId actor_id = nf_world_spawn_actor(&world, NF_FACTION_PLAYER, (NfVec3){0.0f, 0.05f, -18.0f});
    NfActor *actor = nf_world_find_actor(&world, actor_id);
    assert(actor != NULL);

    nf_camera_follow_actor(&camera, actor, &world, 1.0f / 60.0f);
    assert(camera.initialized);
    assert(nearf_test(camera.presented_anchor.x, actor->transform.position.x, 0.0001f));
    assert(nearf_test(camera.presented_anchor.y, actor->transform.position.y + actor->movement.eye_height, 0.0001f));

    const float standing_y = camera.presented_anchor.y;
    actor->movement.mode = NF_MOVE_CROUCH;
    actor->movement.eye_height = world.movement.crouch_eye_height;
    nf_camera_follow_actor(&camera, actor, &world, 1.0f / 60.0f);
    assert(camera.presented_anchor.y < standing_y);
    assert(fabsf(camera.desired_anchor.y - camera.presented_anchor.y) <= 0.301f);
    assert(camera.correction_class == NF_CAMERA_CORRECTION_PRESENTATION || camera.correction_class == NF_CAMERA_CORRECTION_NONE);

    actor->agency = NF_AGENCY_RETURNING;
    actor->transform.position = (NfVec3){25.0f, 0.05f, 25.0f};
    nf_camera_follow_actor(&camera, actor, &world, 1.0f / 60.0f);
    assert(camera.correction_class == NF_CAMERA_CORRECTION_RELOCATION);
    assert(nearf_test(camera.presented_anchor.x, 25.0f, 0.0001f));
    assert(nearf_test(camera.presented_anchor.z, 25.0f, 0.0001f));

    actor->agency = NF_AGENCY_ACTIVE;
    actor->movement.mode = NF_MOVE_GROUND;
    const float hfov = nf_camera_horizontal_fov_deg(&camera, 16.0f / 9.0f);
    assert(hfov > 113.0f && hfov < 115.0f);

    nf_camera_apply_impulse(&camera, (NfVec3){0.0f, 1.0f, 0.0f}, (NfVec3){1.0f, 0.0f, 0.0f}, 1.0f);
    assert(camera.impulse_translation.y <= 0.1601f);
    assert(camera.impulse_rotation.x <= 0.0801f);
    const float impulse_before = fabsf(camera.impulse_translation.y);
    nf_camera_step_presentation(&camera, 1.0f / 60.0f);
    assert(fabsf(camera.impulse_translation.y) < impulse_before);

    printf("camera relational perceptual authority tests: PASS\n");
    printf("FOV vertical=%.1f horizontal16:9=%.1f\n", camera.current_vertical_fov_deg, hfov);
    printf("LOOK=IMMEDIATE BODY=TRUTH PRESENTATION=BOUNDED RELOCATION=DISCONTINUITY\n");
    return 0;
}
