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
    mat4 inv_projection;
    mat4 inv_view;
    mat3 view_mat3;
    mat3 inv_view_mat3;
    vec3 forward;
    vec3 rotation;
    vec3 view_point;
    vec3 interest;
    double_t yaw;
    double_t pitch;
    double_t roll;
    double_t aspect;
    double_t fov;
    double_t max_distance;
    double_t min_distance;
    bool changed_proj;
    bool changed_view;
    bool updated;
} camera;

extern camera* camera_init();
extern void camera_initialize(camera* c, double_t aspect, double_t fov);
extern double_t camera_get_min_distance(camera* c);
extern void camera_set_min_distance(camera* c, double_t value);
extern double_t camera_get_max_distance(camera* c);
extern void camera_set_max_distance(camera* c, double_t value);
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
extern void camera_get_view_point(camera* c, vec3* value);
extern void camera_set_view_point(camera* c, vec3* value);
extern void camera_get_interest_point(camera* c, vec3* value);
extern void camera_set_interest_point(camera* c, vec3* value);
extern void camera_reset(camera* c);
extern void camera_move(camera* c, vec2d* move);
extern void camera_rotate(camera* c, vec2d* rotate);
extern void camera_roll(camera* c, double_t roll);
extern void camera_set(camera* c, vec3* view_point, vec3* interest,
    vec3* trans, vec3* rot, vec3* scale, double_t roll, double_t fov);
extern void camera_set_position(camera* c, vec3* pos);
extern void camera_update(camera* c);
extern void camera_dispose(camera* c);