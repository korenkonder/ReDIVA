/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vec.h"
#include "quat.h"

typedef struct mat3 {
    vec3 row0;
    vec3 row1;
    vec3 row2;
} mat3;

typedef struct mat4 {
    vec4 row0;
    vec4 row1;
    vec4 row2;
    vec4 row3;
} mat4;

typedef struct mat4u {
    vec4u row0;
    vec4u row1;
    vec4u row2;
    vec4u row3;
} mat4u;

extern const mat3 mat3_identity;
extern const mat3 mat3_null;
extern const mat4 mat4_identity;
extern const mat4 mat4_null;
extern const mat4u mat4u_identity;
extern const mat4u mat4u_null;

vector(mat3)
vector(mat4)
vector_ptr(mat3)
vector_ptr(mat4)

extern void mat3_add(mat3* x, mat3* y, mat3* z);
extern void mat3_sub(mat3* x, mat3* y, mat3* z);
extern void mat3_mult(mat3* x, mat3* y, mat3* z);
extern void mat3_mult_vec(mat3* x, vec3* y, vec3* z);
extern void mat3_mult_scalar(mat3* x, float_t y, mat3* z);
extern void mat3_transpose(mat3* x, mat3* z);
extern void mat3_inverse(mat3* x, mat3* z);
extern void mat3_invtrans(mat3* x, mat3* z);
extern void mat3_inverse_normalized(mat3* x, mat3* z);
extern void mat3_normalize(mat3* x, mat3* z);
extern void mat3_normalize_rotation(mat3* x, mat3* z);
extern float_t mat3_determinant(mat3* x);
extern void mat3_rotate(float_t x, float_t y, float_t z, mat3* w);
extern void mat3_rotate_x(float_t x, mat3* y);
extern void mat3_rotate_y(float_t x, mat3* y);
extern void mat3_rotate_z(float_t x, mat3* y);
extern void mat3_rotate_mult(mat3* s, float_t x, float_t y, float_t z, mat3* d);
extern void mat3_rotate_x_mult(mat3* x, float_t y, mat3* z);
extern void mat3_rotate_y_mult(mat3* x, float_t y, mat3* z);
extern void mat3_rotate_z_mult(mat3* x, float_t y, mat3* z);
extern void mat3_rotate(float_t x, float_t y, float_t z, mat3* w);
extern void mat3_scale_x(float_t x, mat3* y);
extern void mat3_scale_y(float_t x, mat3* y);
extern void mat3_scale_z(float_t x, mat3* y);
extern void mat3_scale_mult(mat3* s, float_t x, float_t y, float_t z, mat3* d);
extern void mat3_scale_x_mult(mat3* x, float_t y, mat3* z);
extern void mat3_scale_y_mult(mat3* x, float_t y, mat3* z);
extern void mat3_scale_z_mult(mat3* x, float_t y, mat3* z);
extern void mat3_from_quat(quat* quat, mat3* mat);
extern void mat3_from_axis_angle(vec3* axis, float_t angle, mat3* mat);
extern void mat3_from_mat4(mat4* x, mat3* z);
extern void mat3_from_mat4_inverse(mat4* x, mat3* z);
extern void mat3_get_rotation(mat3* x, vec3* z);
extern void mat3_get_scale(mat3* x, vec3* z);
extern float_t mat3_get_max_scale(mat3* x);

extern void mat4_add(mat4* x, mat4* y, mat4* z);
extern void mat4_sub(mat4* x, mat4* y, mat4* z);
extern void mat4_mult(mat4* x, mat4* y, mat4* z);
extern void mat4_mult_vec3(mat4* x, vec3* y, vec3* z);
extern void mat4_mult_vec3_inv(mat4* x, vec3* y, vec3* z);
extern void mat4_mult_vec3_trans(mat4* x, vec3* y, vec3* z);
extern void mat4_mult_vec3_inv_trans(mat4* x, vec3* y, vec3* z);
extern void mat4_mult_vec(mat4* x, vec4* y, vec4* z);
extern void mat4_mult_scalar(mat4* x, float_t y, mat4* z);
extern void mat4_transpose(mat4* x, mat4* z);
extern void mat4_inverse(mat4* x, mat4* z);
extern void mat4_invtrans(mat4* x, mat4* z);
extern void mat4_invrot(mat4* x, mat4* z);
extern void mat4_inverse_normalized(mat4* x, mat4* z);
extern void mat4_invrot_normalized(mat4* x, mat4* z);
extern void mat4_normalize(mat4* x, mat4* z);
extern void mat4_normalize_rotation(mat4* x, mat4* z);
extern float_t mat4_determinant(mat4* x);
extern void mat4_rotate(float_t x, float_t y, float_t z, mat4* d);
extern void mat4_rotate_x(float_t x, mat4* y);
extern void mat4_rotate_y(float_t x, mat4* y);
extern void mat4_rotate_z(float_t x, mat4* y);
extern void mat4_rotate_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_rotate_x_mult(mat4* x, float_t y, mat4* z);
extern void mat4_rotate_y_mult(mat4* x, float_t y, mat4* z);
extern void mat4_rotate_z_mult(mat4* x, float_t y, mat4* z);
extern void mat4_rotate_x_sin_cos(float_t sin_val, float_t cos_val, mat4* y);
extern void mat4_rotate_y_sin_cos(float_t sin_val, float_t cos_val, mat4* y);
extern void mat4_rotate_z_sin_cos(float_t sin_val, float_t cos_val, mat4* y);
extern void mat4_rotate_x_mult_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rotate_y_mult_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rotate_z_mult_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rot(mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_rot_x(mat4* x, float_t y, mat4* z);
extern void mat4_rot_y(mat4* x, float_t y, mat4* z);
extern void mat4_rot_z(mat4* x, float_t y, mat4* z);
extern void mat4_rot_x_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rot_y_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rot_z_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_scale(float_t x, float_t y, float_t z, mat4* d);
extern void mat4_scale_x(float_t x, mat4* y);
extern void mat4_scale_y(float_t x, mat4* y);
extern void mat4_scale_z(float_t x, mat4* y);
extern void mat4_scale_mult(mat4* s, float_t x, float_t y, float_t z, float_t w, mat4* d);
extern void mat4_scale_x_mult(mat4* x, float_t y, mat4* z);
extern void mat4_scale_y_mult(mat4* x, float_t y, mat4* z);
extern void mat4_scale_z_mult(mat4* x, float_t y, mat4* z);
extern void mat4_scale_w_mult(mat4* x, float_t y, mat4* z);
extern void mat4_scale_rot(mat4* s, float_t x, float_t y, float_t z, mat4* d);;
extern void mat4_scale_x_rot(mat4* x, float_t y, mat4* z);
extern void mat4_scale_y_rot(mat4* x, float_t y, mat4* z);
extern void mat4_scale_z_rot(mat4* x, float_t y, mat4* z);
extern void mat4_translate(float_t x, float_t y, float_t z, mat4* d);
extern void mat4_translate_x(float_t x, mat4* y);
extern void mat4_translate_y(float_t x, mat4* y);
extern void mat4_translate_z(float_t x, mat4* y);
extern void mat4_translate_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_translate_x_mult(mat4* x, float_t y, mat4* z);
extern void mat4_translate_y_mult(mat4* x, float_t y, mat4* z);
extern void mat4_translate_z_mult(mat4* x, float_t y, mat4* z);
extern void mat4_translate_add(mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_translate_x_add(mat4* x, float_t y, mat4* z);
extern void mat4_translate_y_add(mat4* x, float_t y, mat4* z);
extern void mat4_translate_z_add(mat4* x, float_t y, mat4* z);
extern void mat4_from_quat(quat* quat, mat4* mat);
extern void mat4_from_axis_angle(vec3* axis, float_t angle, mat4* mat);
extern void mat4_from_mat3(mat3* x, mat4* z);
extern void mat4_from_mat3_inverse(mat3* x, mat4* z);
extern void mat4_clear_rot(mat4* x, mat4* z);
extern void mat4_clear_trans(mat4* x, mat4* z);
extern void mat4_get_scale(mat4* x, vec3* z);
extern void mat4_get_rotation(mat4* x, vec3* z);
extern void mat4_get_translation(mat4* x, vec3* z);
extern void mat4_set_translation(mat4* x, vec3* z);
extern void mat4_blend_rotation(mat4* x, mat4* y, mat4* z, float_t blend);
extern void mat4_ortho(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far, mat4* mat);
extern void mat4_persp(double_t fov_y, double_t aspect, double_t z_near, double_t z_far, mat4* mat);
extern void mat4_look_at(vec3* eye, vec3* target, vec3* up, mat4* mat);
extern float_t mat4_get_max_scale(mat4* x);

extern void mat4_to_mat4u(mat4* x, mat4u* z);
extern void mat4u_to_mat4(mat4u* x, mat4* z);
extern void mat4_to_mat4u_transpose(mat4* x, mat4u* z);
extern void mat4u_to_mat4_transpose(mat4u* x, mat4* z);
