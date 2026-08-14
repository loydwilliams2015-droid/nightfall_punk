#include "nf_camera.h"

#include <math.h>
#include <stddef.h>

#define NF_CAMERA_PI 3.14159265358979323846f
#define NF_CAMERA_PITCH_LIMIT 1.45f
#define NF_CAMERA_RELOCATION_DISTANCE 4.0f
#define NF_CAMERA_MAX_VERTICAL_LAG 0.30f
#define NF_CAMERA_BOUNDARY_EPSILON 0.01f
#define NF_CAMERA_BOUNDARY_MAX_CORRECTION 0.10f

static float clampf_camera(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static float vec3_distance(NfVec3 a, NfVec3 b) {
    const float x = a.x - b.x;
    const float y = a.y - b.y;
    const float z = a.z - b.z;
    return sqrtf(x * x + y * y + z * z);
}

static bool point_inside(const NfVec3 p, const NfCollider *c) {
    return p.x > c->min.x && p.x < c->max.x &&
           p.y > c->min.y && p.y < c->max.y &&
           p.z > c->min.z && p.z < c->max.z;
}

static bool apply_boundary_envelope(const NfWorld *world, NfVec3 *anchor) {
    if (world == NULL || anchor == NULL) return false;
    for (size_t i = 0; i < world->collider_count; ++i) {
        const NfCollider *c = &world->colliders[i];
        if (!point_inside(*anchor, c)) continue;

        const float dx_min = anchor->x - c->min.x;
        const float dx_max = c->max.x - anchor->x;
        const float dy_min = anchor->y - c->min.y;
        const float dy_max = c->max.y - anchor->y;
        const float dz_min = anchor->z - c->min.z;
        const float dz_max = c->max.z - anchor->z;
        float d = dx_min;
        int face = 0;
        if (dx_max < d) { d = dx_max; face = 1; }
        if (dy_min < d) { d = dy_min; face = 2; }
        if (dy_max < d) { d = dy_max; face = 3; }
        if (dz_min < d) { d = dz_min; face = 4; }
        if (dz_max < d) { d = dz_max; face = 5; }

        const float correction = d + NF_CAMERA_BOUNDARY_EPSILON;
        if (correction > NF_CAMERA_BOUNDARY_MAX_CORRECTION) return false;
        switch (face) {
            case 0: anchor->x -= correction; break;
            case 1: anchor->x += correction; break;
            case 2: anchor->y -= correction; break;
            case 3: anchor->y += correction; break;
            case 4: anchor->z -= correction; break;
            case 5: anchor->z += correction; break;
            default: break;
        }
        return true;
    }
    return false;
}

static float vertical_response_rate(NfMovementMode mode) {
    switch (mode) {
        case NF_MOVE_CROUCH: return 14.0f;
        case NF_MOVE_LADDER: return 15.0f;
        case NF_MOVE_VAULT: return 12.0f;
        case NF_MOVE_MANTLE: return 11.0f;
        case NF_MOVE_PLATFORM: return 18.0f;
        case NF_MOVE_AIR: return 20.0f;
        case NF_MOVE_SPRINT: return 24.0f;
        case NF_MOVE_GROUND:
        default: return 26.0f;
    }
}

void nf_camera_init(NfCameraState *camera, float base_vertical_fov_deg) {
    if (camera == NULL) return;
    *camera = (NfCameraState){0};
    camera->mode = NF_CAMERA_FIRST_PERSON;
    camera->base_vertical_fov_deg = base_vertical_fov_deg;
    camera->current_vertical_fov_deg = base_vertical_fov_deg;
}

void nf_camera_set_mode(NfCameraState *camera, NfCameraMode mode) {
    if (camera == NULL) return;
    camera->mode = mode;
}

void nf_camera_apply_look_delta(NfCameraState *camera, float delta_x, float delta_y, float sensitivity) {
    if (camera == NULL || camera->mode != NF_CAMERA_FIRST_PERSON) return;
    camera->yaw -= delta_x * sensitivity;
    camera->pitch = clampf_camera(camera->pitch - delta_y * sensitivity,
                                  -NF_CAMERA_PITCH_LIMIT, NF_CAMERA_PITCH_LIMIT);
    if (camera->yaw > NF_CAMERA_PI || camera->yaw < -NF_CAMERA_PI) {
        camera->yaw = fmodf(camera->yaw + NF_CAMERA_PI, 2.0f * NF_CAMERA_PI);
        if (camera->yaw < 0.0f) camera->yaw += 2.0f * NF_CAMERA_PI;
        camera->yaw -= NF_CAMERA_PI;
    }
}

void nf_camera_follow_actor(NfCameraState *camera, const NfActor *actor, const NfWorld *world, float dt) {
    if (camera == NULL || actor == NULL) return;
    camera->desired_anchor = (NfVec3){
        actor->transform.position.x,
        actor->transform.position.y + actor->movement.eye_height,
        actor->transform.position.z
    };

    if (!camera->initialized) {
        camera->presented_anchor = camera->desired_anchor;
        camera->initialized = true;
        camera->anchor_error = 0.0f;
        camera->correction_class = NF_CAMERA_CORRECTION_NONE;
    } else {
        const float discontinuity = vec3_distance(camera->desired_anchor, camera->presented_anchor);
        if (actor->agency == NF_AGENCY_RETURNING || discontinuity >= NF_CAMERA_RELOCATION_DISTANCE) {
            camera->presented_anchor = camera->desired_anchor;
            camera->correction_class = NF_CAMERA_CORRECTION_RELOCATION;
        } else {
            camera->presented_anchor.x = camera->desired_anchor.x;
            camera->presented_anchor.z = camera->desired_anchor.z;
            const float rate = vertical_response_rate(actor->movement.mode);
            const float alpha = 1.0f - expf(-rate * clampf_camera(dt, 0.0f, 0.10f));
            camera->presented_anchor.y += (camera->desired_anchor.y - camera->presented_anchor.y) * alpha;
            const float lag = camera->desired_anchor.y - camera->presented_anchor.y;
            if (lag > NF_CAMERA_MAX_VERTICAL_LAG) camera->presented_anchor.y = camera->desired_anchor.y - NF_CAMERA_MAX_VERTICAL_LAG;
            else if (lag < -NF_CAMERA_MAX_VERTICAL_LAG) camera->presented_anchor.y = camera->desired_anchor.y + NF_CAMERA_MAX_VERTICAL_LAG;
            camera->correction_class = fabsf(lag) > 0.002f ? NF_CAMERA_CORRECTION_PRESENTATION : NF_CAMERA_CORRECTION_NONE;
        }
    }

    if (apply_boundary_envelope(world, &camera->presented_anchor)) {
        camera->correction_class = NF_CAMERA_CORRECTION_BOUNDARY;
    }
    camera->anchor_error = vec3_distance(camera->desired_anchor, camera->presented_anchor);
}

void nf_camera_apply_impulse(NfCameraState *camera, NfVec3 translation, NfVec3 rotation, float consequence) {
    if (camera == NULL) return;
    const float k = clampf_camera(consequence, 0.0f, 1.0f);
    camera->impulse_translation.x = clampf_camera(camera->impulse_translation.x + translation.x * k, -0.16f, 0.16f);
    camera->impulse_translation.y = clampf_camera(camera->impulse_translation.y + translation.y * k, -0.16f, 0.16f);
    camera->impulse_translation.z = clampf_camera(camera->impulse_translation.z + translation.z * k, -0.16f, 0.16f);
    camera->impulse_rotation.x = clampf_camera(camera->impulse_rotation.x + rotation.x * k, -0.08f, 0.08f);
    camera->impulse_rotation.y = clampf_camera(camera->impulse_rotation.y + rotation.y * k, -0.08f, 0.08f);
    camera->impulse_rotation.z = clampf_camera(camera->impulse_rotation.z + rotation.z * k, -0.05f, 0.05f);
}

void nf_camera_step_presentation(NfCameraState *camera, float dt) {
    if (camera == NULL) return;
    const float decay = expf(-12.0f * clampf_camera(dt, 0.0f, 0.10f));
    camera->impulse_translation.x *= decay;
    camera->impulse_translation.y *= decay;
    camera->impulse_translation.z *= decay;
    camera->impulse_rotation.x *= decay;
    camera->impulse_rotation.y *= decay;
    camera->impulse_rotation.z *= decay;
    camera->current_vertical_fov_deg += (camera->base_vertical_fov_deg - camera->current_vertical_fov_deg) * (1.0f - decay);
}

NfVec3 nf_camera_render_anchor(const NfCameraState *camera) {
    if (camera == NULL) return (NfVec3){0};
    return (NfVec3){
        camera->presented_anchor.x + camera->impulse_translation.x,
        camera->presented_anchor.y + camera->impulse_translation.y,
        camera->presented_anchor.z + camera->impulse_translation.z
    };
}

NfVec3 nf_camera_forward(const NfCameraState *camera) {
    if (camera == NULL) return (NfVec3){0.0f, 0.0f, 1.0f};
    const float yaw = camera->yaw + camera->impulse_rotation.y;
    const float pitch = camera->pitch + camera->impulse_rotation.x;
    return (NfVec3){cosf(pitch) * sinf(yaw), sinf(pitch), cosf(pitch) * cosf(yaw)};
}

float nf_camera_horizontal_fov_deg(const NfCameraState *camera, float aspect_ratio) {
    if (camera == NULL || aspect_ratio <= 0.0f) return 0.0f;
    const float vertical_rad = camera->current_vertical_fov_deg * NF_CAMERA_PI / 180.0f;
    return 2.0f * atanf(tanf(vertical_rad * 0.5f) * aspect_ratio) * 180.0f / NF_CAMERA_PI;
}

const char *nf_camera_mode_name(NfCameraMode mode) {
    switch (mode) {
        case NF_CAMERA_FIRST_PERSON: return "PLAYER VIEW";
        case NF_CAMERA_DEBUG_OBSERVER: return "OBSERVER / LIVE";
        case NF_CAMERA_DEBUG_TOP: return "DEBUG TOP / LIVE";
        default: return "CAMERA UNKNOWN";
    }
}

const char *nf_camera_correction_name(NfCameraCorrectionClass correction) {
    switch (correction) {
        case NF_CAMERA_CORRECTION_NONE: return "DIRECT";
        case NF_CAMERA_CORRECTION_PRESENTATION: return "PRESENTATION";
        case NF_CAMERA_CORRECTION_BOUNDARY: return "BOUNDARY";
        case NF_CAMERA_CORRECTION_RELOCATION: return "RELOCATION";
        default: return "UNKNOWN";
    }
}
