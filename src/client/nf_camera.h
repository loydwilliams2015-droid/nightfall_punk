#ifndef NF_CAMERA_H
#define NF_CAMERA_H

#include "nf_world.h"

#include <stdbool.h>

typedef enum NfCameraMode {
    NF_CAMERA_FIRST_PERSON = 0,
    NF_CAMERA_DEBUG_OBSERVER,
    NF_CAMERA_DEBUG_TOP
} NfCameraMode;

typedef enum NfCameraCorrectionClass {
    NF_CAMERA_CORRECTION_NONE = 0,
    NF_CAMERA_CORRECTION_PRESENTATION,
    NF_CAMERA_CORRECTION_BOUNDARY,
    NF_CAMERA_CORRECTION_RELOCATION
} NfCameraCorrectionClass;

typedef struct NfCameraState {
    NfCameraMode mode;
    float yaw;
    float pitch;
    float base_vertical_fov_deg;
    float current_vertical_fov_deg;
    NfVec3 desired_anchor;
    NfVec3 presented_anchor;
    NfVec3 impulse_translation;
    NfVec3 impulse_rotation;
    float anchor_error;
    bool initialized;
    NfCameraCorrectionClass correction_class;
} NfCameraState;

void nf_camera_init(NfCameraState *camera, float base_vertical_fov_deg);
void nf_camera_set_mode(NfCameraState *camera, NfCameraMode mode);
void nf_camera_apply_look_delta(NfCameraState *camera, float delta_x, float delta_y, float sensitivity);
void nf_camera_follow_actor(NfCameraState *camera, const NfActor *actor, const NfWorld *world, float dt);
void nf_camera_apply_impulse(NfCameraState *camera, NfVec3 translation, NfVec3 rotation, float consequence);
void nf_camera_step_presentation(NfCameraState *camera, float dt);
NfVec3 nf_camera_render_anchor(const NfCameraState *camera);
NfVec3 nf_camera_forward(const NfCameraState *camera);
float nf_camera_horizontal_fov_deg(const NfCameraState *camera, float aspect_ratio);
const char *nf_camera_mode_name(NfCameraMode mode);
const char *nf_camera_correction_name(NfCameraCorrectionClass correction);

#endif
