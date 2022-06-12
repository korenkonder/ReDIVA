/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "vec.hpp"
#include "quat.hpp"

struct mat3 {
    vec3 row0;
    vec3 row1;
    vec3 row2;
};

struct mat4;
struct mat4u;

struct mat4 {
    vec4 row0;
    vec4 row1;
    vec4 row2;
    vec4 row3;

    operator mat4u() const;
};

struct mat4u {
    vec4u row0;
    vec4u row1;
    vec4u row2;
    vec4u row3;

    operator mat4() const;
};

extern const mat3 mat3_identity;
extern const mat3 mat3_null;
extern const mat4 mat4_identity;
extern const mat4 mat4_null;
extern const mat4u mat4u_identity;
extern const mat4u mat4u_null;

extern void mat3_add(const mat3* x, const mat3* y, mat3* z);
extern void mat3_sub(const mat3* x, const mat3* y, mat3* z);
extern void mat3_mult(const mat3* x, const mat3* y, mat3* z);
extern void mat3_mult_vec(const mat3* x, const vec3* y, vec3* z);
extern void mat3_mult_scalar(const mat3* x, float_t y, mat3* z);
extern void mat3_transpose(const mat3* x, mat3* z);
extern void mat3_inverse(const mat3* x, mat3* z);
extern void mat3_invtrans(const mat3* x, mat3* z);
extern void mat3_inverse_normalized(const mat3* x, mat3* z);
extern void mat3_normalize(const mat3* x, mat3* z);
extern void mat3_normalize_rotation(const mat3* x, mat3* z);
extern float_t mat3_determinant(const mat3* x);
extern void mat3_rotate(float_t x, float_t y, float_t z, mat3* w);
extern void mat3_rotate_x(float_t x, mat3* y);
extern void mat3_rotate_y(float_t x, mat3* y);
extern void mat3_rotate_z(float_t x, mat3* y);
extern void mat3_rotate_mult(const mat3* s, float_t x, float_t y, float_t z, mat3* d);
extern void mat3_rotate_x_mult(const mat3* x, float_t y, mat3* z);
extern void mat3_rotate_y_mult(const mat3* x, float_t y, mat3* z);
extern void mat3_rotate_z_mult(const mat3* x, float_t y, mat3* z);
extern void mat3_rotate(float_t x, float_t y, float_t z, mat3* w);
extern void mat3_scale_x(float_t x, mat3* y);
extern void mat3_scale_y(float_t x, mat3* y);
extern void mat3_scale_z(float_t x, mat3* y);
extern void mat3_scale_mult(const mat3* s, float_t x, float_t y, float_t z, mat3* d);
extern void mat3_scale_x_mult(const mat3* x, float_t y, mat3* z);
extern void mat3_scale_y_mult(const mat3* x, float_t y, mat3* z);
extern void mat3_scale_z_mult(const mat3* x, float_t y, mat3* z);
extern void mat3_from_quat(const quat* quat, mat3* mat);
extern void mat3_from_axis_angle(const vec3* axis, float_t angle, mat3* mat);
extern void mat3_from_mat4(const mat4* x, mat3* z);
extern void mat3_from_mat4(const mat4u* x, mat3* z);
extern void mat3_from_mat4_inverse(const mat4* x, mat3* z);
extern void mat3_from_mat4_inverse(const mat4u* x, mat3* z);
extern void mat3_get_rotation(const mat3* x, vec3* z);
extern void mat3_get_scale(const mat3* x, vec3* z);
extern float_t mat3_get_max_scale(const mat3* x);

extern void mat4_add(const mat4* x, const mat4* y, mat4* z);
extern void mat4_sub(const mat4* x, const mat4* y, mat4* z);
extern void mat4_mult(const mat4* x, const mat4* y, mat4* z);
extern void mat4_mult(const mat4u* x, const mat4u* y, mat4u* z);
extern void mat4_mult_vec3(const mat4* x, const vec3* y, vec3* z);
extern void mat4_mult_vec3_inv(const mat4* x, const vec3* y, vec3* z);
extern void mat4_mult_vec3_trans(const mat4* x, const vec3* y, vec3* z);
extern void mat4_mult_vec3_inv_trans(const mat4* x, const vec3* y, vec3* z);
extern void mat4_mult_vec(const mat4* x, vec4* y, vec4* z);
extern void mat4_mult_scalar(const mat4* x, float_t y, mat4* z);
extern void mat4_transpose(const mat4* x, mat4* z);
extern void mat4_transpose(const mat4* x, mat4u* z);
extern void mat4_transpose(const mat4u* x, mat4* z);
extern void mat4_transpose(const mat4u* x, mat4u* z);
extern void mat4_inverse(const mat4* x, mat4* z);
extern void mat4_invtrans(const mat4* x, mat4* z);
extern void mat4_invrot(const mat4* x, mat4* z);
extern void mat4_inverse_normalized(const mat4* x, mat4* z);
extern void mat4_invrot_normalized(const mat4* x, mat4* z);
extern void mat4_normalize(const mat4* x, mat4* z);
extern void mat4_normalize_rotation(const mat4* x, mat4* z);
extern float_t mat4_determinant(const mat4* x);
extern void mat4_rotate(float_t x, float_t y, float_t z, mat4* d);
extern void mat4_rotate_x(float_t x, mat4* y);
extern void mat4_rotate_y(float_t x, mat4* y);
extern void mat4_rotate_z(float_t x, mat4* y);
extern void mat4_rotate_mult(const mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_rotate_x_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_rotate_y_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_rotate_z_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_rotate_x_sin_cos(float_t sin_val, float_t cos_val, mat4* y);
extern void mat4_rotate_y_sin_cos(float_t sin_val, float_t cos_val, mat4* y);
extern void mat4_rotate_z_sin_cos(float_t sin_val, float_t cos_val, mat4* y);
extern void mat4_rotate_x_mult_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rotate_y_mult_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rotate_z_mult_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rot(const mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_rot_x(const mat4* x, float_t y, mat4* z);
extern void mat4_rot_y(const mat4* x, float_t y, mat4* z);
extern void mat4_rot_z(const mat4* x, float_t y, mat4* z);
extern void mat4_rot_x_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rot_y_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_rot_z_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z);
extern void mat4_scale(float_t x, float_t y, float_t z, mat4* d);
extern void mat4_scale_x(float_t x, mat4* y);
extern void mat4_scale_y(float_t x, mat4* y);
extern void mat4_scale_z(float_t x, mat4* y);
extern void mat4_scale_mult(const mat4* s, float_t x, float_t y, float_t z, float_t w, mat4* d);
extern void mat4_scale_x_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_scale_y_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_scale_z_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_scale_w_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_scale_rot(const mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_scale_x_rot(const mat4* x, float_t y, mat4* z);
extern void mat4_scale_y_rot(const mat4* x, float_t y, mat4* z);
extern void mat4_scale_z_rot(const mat4* x, float_t y, mat4* z);
extern void mat4_translate(float_t x, float_t y, float_t z, mat4* d);
extern void mat4_translate_x(float_t x, mat4* y);
extern void mat4_translate_y(float_t x, mat4* y);
extern void mat4_translate_z(float_t x, mat4* y);
extern void mat4_translate_mult(const mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_translate_x_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_translate_y_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_translate_z_mult(const mat4* x, float_t y, mat4* z);
extern void mat4_translate_add(const mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void mat4_translate_x_add(const mat4* x, float_t y, mat4* z);
extern void mat4_translate_y_add(const mat4* x, float_t y, mat4* z);
extern void mat4_translate_z_add(const mat4* x, float_t y, mat4* z);
extern void mat4_from_quat(const quat* quat, mat4* mat);
extern void mat4_from_quat(const quat* quat, mat4u* mat);
extern void mat4_from_axis_angle(const vec3* axis, float_t angle, mat4* mat);
extern void mat4_from_mat3(const mat3* x, mat4* z);
extern void mat4_from_mat3_inverse(const mat3* x, mat4* z);
extern void mat4_clear_rot(mat4* x, mat4* z);
extern void mat4_clear_trans(mat4* x, mat4* z);
extern void mat4_get_scale(const mat4* x, vec3* z);
extern void mat4_get_scale(const mat4u* x, vec3* z);
extern void mat4_get_rotation(const mat4* x, vec3* z);
extern void mat4_get_rotation(const mat4u* x, vec3* z);
extern void mat4_get_translation(const mat4* x, vec3* z);
extern void mat4_get_translation(const mat4u* x, vec3* z);
extern void mat4_set_translation(mat4* x, vec3* z);
extern void mat4_set_translation(mat4u* x, vec3* z);
extern void mat4_blend(const mat4* x, const mat4* y, mat4* z, float_t blend);
extern void mat4_blend(const mat4u* x, const mat4u* y, mat4u* z, float_t blend);
extern void mat4_blend_rotation(const mat4* x, const mat4* y, mat4* z, float_t blend);
extern void mat4_blend_rotation(const mat4u* x, const mat4u* y, mat4u* z, float_t blend);
extern void mat4_lerp_rotation(mat4* dst, const mat4* src0, const mat4* src1, float_t blend);
extern void mat4_lerp_rotation(mat4u* dst, const mat4u* src0, const mat4u* src1, float_t blend);
extern float_t mat4_get_max_scale(const mat4* x);
extern void mat4_ortho(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far, mat4* mat);
extern void mat4_persp(double_t fov_y, double_t aspect, double_t z_near, double_t z_far, mat4* mat);
extern void mat4_look_at(const vec3* eye, const vec3* target, const vec3* up, mat4* mat);
extern void mat4_look_at(const vec3* eye, const vec3* target, mat4* mat);
