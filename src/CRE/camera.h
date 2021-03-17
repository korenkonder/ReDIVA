/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"

typedef struct camera {
    mat4 view;
    mat4 projection;
    mat4 view_projection;
    vec3 position;
    vec3 target;
    vec3 up;
    vec3 right;
    vec3 forward;
    vec3 rotation;
    double_t yaw;
    double_t pitch;
    double_t roll;
    double_t camera_aperture_w;
    double_t camera_aperture_h;
    double_t aspect;
    double_t focal_length;
    double_t fov;
    bool fov_horizontal;
    double_t max_distance;
    double_t min_distance;
} camera;

extern camera* camera_init();
extern void camera_initialize(camera* c, double_t aspect, double_t fov);
extern double_t camera_get_min_distance(camera* c);
extern void camera_set_min_distance(camera* c, double_t value);
extern double_t camera_get_max_distance(camera* c);
extern void camera_set_max_distance(camera* c, double_t value);
extern double_t camera_get_camera_aperture_w(camera* c);
extern void camera_set_camera_aperture_w(camera* c, double_t value);
extern double_t camera_get_camera_aperture_h(camera* c);
extern void camera_set_camera_aperture_h(camera* c, double_t value);
extern double_t camera_get_focal_length(camera* c);
extern void camera_set_focal_length(camera* c, double_t value);
extern bool camera_get_fov_horizontal(camera* c);
extern void camera_set_fov_horizontal(camera* c, bool value);
extern double_t camera_get_aspect(camera* c);
extern void camera_set_aspect(camera* c, double_t value);
extern double_t camera_get_fov(camera* c);
extern void camera_set_fov(camera* c, double_t value);
extern double_t camera_get_pitch(camera* c);
extern void camera_set_pitch(camera* c, double_t value);
extern double_t camera_get_yaw(camera* c);
extern void camera_set_yaw(camera* c, double_t value);
extern double_t camera_get_roll(camera* c);
extern void camera_set_roll(camera* c, double_t value);
extern void camera_reset(camera* c);
extern void camera_set_point(camera* c, vec3* view_point, vec3* interest_point,
    vec3* trans, vec3* rot, vec3* scale, double_t roll, double_t fov);
extern void camera_set(camera* c, vec3* eye, vec3* target, vec3* up);
extern void camera_rotate_vec2(camera* c, vec2* rotate);
extern void camera_move_vec2(camera* c, vec2* move);
extern void camera_move_vec3(camera* c, vec3* move);
extern void camera_look_at(camera* c, vec3* eye, vec3* target, vec3* up, mat4* view);
extern void camera_dispose(camera* c);
