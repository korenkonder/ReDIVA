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

    inline mat3() : row0(), row1(), row2() {

    }

    inline mat3(vec3 row0, vec3 row1, vec3 row2) :
        row0(row0), row1(row1), row2(row2) {

    }
};

struct mat4 {
    vec4 row0;
    vec4 row1;
    vec4 row2;
    vec4 row3;

    inline mat4() : row0(), row1(), row2(), row3() {

    }

    inline mat4(vec4 row0, vec4 row1, vec4 row2, vec4 row3) :
        row0(row0), row1(row1), row2(row2), row3(row3) {

    }
};

extern const mat3 mat3_identity;
extern const mat3 mat3_null;
extern const mat4 mat4_identity;
extern const mat4 mat4_null;

extern void mat3_add(const mat3* x, float_t y, mat3* z);
extern void mat3_add(const mat3* x, const mat3* y, mat3* z);
extern void mat3_sub(const mat3* x, float_t y, mat3* z);
extern void mat3_sub(const mat3* x, const mat3* y, mat3* z);
extern void mat3_mul(const mat3* x, float_t y, mat3* z);
extern void mat3_mul(const mat3* x, const mat3* y, mat3* z);
extern void mat3_transform_vector(const mat3* x, const vec2* y, vec2* z);
extern void mat3_transform_vector(const mat3* x, const vec3* y, vec3* z);
extern void mat3_inverse_transform_vector(const mat3* x, const vec2* y, vec2* z);
extern void mat3_inverse_transform_vector(const mat3* x, const vec3* y, vec3* z);
extern void mat3_transpose(const mat3* x, mat3* z);
extern void mat3_inverse(const mat3* x, mat3* z);
extern void mat3_invtrans(const mat3* x, mat3* z);
extern void mat3_inverse_normalized(const mat3* x, mat3* z);
extern void mat3_normalize(const mat3* x, mat3* z);
extern void mat3_normalize_rotation(const mat3* x, mat3* z);
extern float_t mat3_determinant(const mat3* x);
extern void mat3_rotate_x(float_t rad, mat3* y);
extern void mat3_rotate_y(float_t rad, mat3* y);
extern void mat3_rotate_z(float_t rad, mat3* y);
extern void mat3_rotate_x(float_t s, float_t c, mat3* y);
extern void mat3_rotate_y(float_t s, float_t c, mat3* y);
extern void mat3_rotate_z(float_t s, float_t c, mat3* y);
extern void mat3_rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_mul_rotate_x(const mat3* x, float_t rad, mat3* z);
extern void mat3_mul_rotate_y(const mat3* x, float_t rad, mat3* z);
extern void mat3_mul_rotate_z(const mat3* x, float_t rad, mat3* z);
extern void mat3_mul_rotate_x(const mat3* x, float_t s, float_t c, mat3* z);
extern void mat3_mul_rotate_y(const mat3* x, float_t s, float_t c, mat3* z);
extern void mat3_mul_rotate_z(const mat3* x, float_t s, float_t c, mat3* z);
extern void mat3_mul_rotate_xyz(const mat3* x, float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_mul_rotate_xzy(const mat3* x, float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_mul_rotate_yxz(const mat3* x, float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_mul_rotate_yzx(const mat3* x, float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_mul_rotate_zxy(const mat3* x, float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_mul_rotate_zyx(const mat3* x, float_t rad_x, float_t rad_y, float_t rad_z, mat3* z);
extern void mat3_scale(float_t sx, float_t sy, float_t sz, mat3* z);
extern void mat3_scale_x(float_t s, mat3* y);
extern void mat3_scale_y(float_t s, mat3* y);
extern void mat3_scale_z(float_t s, mat3* y);
extern void mat3_mul_scale(const mat3* x, float_t sx, float_t sy, float_t sz, mat3* z);
extern void mat3_mul_scale_x(const mat3* x, float_t s, mat3* z);
extern void mat3_mul_scale_y(const mat3* x, float_t s, mat3* z);
extern void mat3_mul_scale_z(const mat3* x, float_t s, mat3* z);
extern void mat3_from_quat(const quat* quat, mat3* mat);
extern void mat3_from_axis_angle(const vec3* axis, float_t angle, mat3* mat);
extern void mat3_from_axis_angle(const vec3* axis, float_t s, float_t c, mat3* mat);
extern void mat3_from_mat4(const mat4* x, mat3* z);
extern void mat3_from_mat4_inverse(const mat4* x, mat3* z);
extern void mat3_get_rotation(const mat3* x, vec3* z);
extern void mat3_get_scale(const mat3* x, vec3* z);
extern float_t mat3_get_max_scale(const mat3* x);
extern void mat3_mult_axis_angle(const mat3* x, const vec3* axis, const float_t angle, mat3* z);

extern void mat4_add(const mat4* x, float_t y, mat4* z);
extern void mat4_add(const mat4* x, const mat4* y, mat4* z);
extern void mat4_sub(const mat4* x, float_t y, mat4* z);
extern void mat4_sub(const mat4* x, const mat4* y, mat4* z);
extern void mat4_mul(const mat4* x, float_t y, mat4* z);
extern void mat4_mul(const mat4* x, const mat4* y, mat4* z);
extern void mat4_transform_vector(const mat4* x, const vec2* y, vec2* z);
extern void mat4_transform_vector(const mat4* x, const vec3* y, vec3* z);
extern void mat4_transform_vector(const mat4* x, const vec4* y, vec4* z);
extern void mat4_transform_point(const mat4* x, const vec2* y, vec2* z);
extern void mat4_transform_point(const mat4* x, const vec3* y, vec3* z);
extern void mat4_inverse_transform_vector(const mat4* x, const vec2* y, vec2* z);
extern void mat4_inverse_transform_vector(const mat4* x, const vec3* y, vec3* z);
extern void mat4_inverse_transform_vector(const mat4* x, const vec4* y, vec4* z);
extern void mat4_inverse_transform_point(const mat4* x, const vec2* y, vec2* z);
extern void mat4_inverse_transform_point(const mat4* x, const vec3* y, vec3* z);
extern void mat4_transpose(const mat4* x, mat4* z);
extern void mat4_inverse(const mat4* x, mat4* z);
extern void mat4_invtrans(const mat4* x, mat4* z);
extern void mat4_invrot(const mat4* x, mat4* z);
extern void mat4_inverse_normalized(const mat4* x, mat4* z);
extern void mat4_invrot_normalized(const mat4* x, mat4* z);
extern void mat4_normalize(const mat4* x, mat4* z);
extern void mat4_normalize_rotation(const mat4* x, mat4* z);
extern float_t mat4_determinant(const mat4* x);
extern void mat4_rotate_x(float_t rad, mat4* y);
extern void mat4_rotate_y(float_t rad, mat4* y);
extern void mat4_rotate_z(float_t rad, mat4* y);
extern void mat4_rotate_x(float_t s, float_t c, mat4* y);
extern void mat4_rotate_y(float_t s, float_t c, mat4* y);
extern void mat4_rotate_z(float_t s, float_t c, mat4* y);
extern void mat4_rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_mul_rotate_x(const mat4* x, float_t rad, mat4* z);
extern void mat4_mul_rotate_y(const mat4* x, float_t rad, mat4* z);
extern void mat4_mul_rotate_z(const mat4* x, float_t rad, mat4* z);
extern void mat4_mul_rotate_x(const mat4* x, float_t s, float_t c, mat4* z);
extern void mat4_mul_rotate_y(const mat4* x, float_t s, float_t c, mat4* z);
extern void mat4_mul_rotate_z(const mat4* x, float_t s, float_t c, mat4* z);
extern void mat4_mul_rotate_xyz(const mat4* x, float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_mul_rotate_xzy(const mat4* x, float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_mul_rotate_yxz(const mat4* x, float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_mul_rotate_yzx(const mat4* x, float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_mul_rotate_zxy(const mat4* x, float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_mul_rotate_zyx(const mat4* x, float_t rad_x, float_t rad_y, float_t rad_z, mat4* z);
extern void mat4_scale(float_t sx, float_t sy, float_t sz, mat4* z);
extern void mat4_scale_x(float_t s, mat4* y);
extern void mat4_scale_y(float_t s, mat4* y);
extern void mat4_scale_z(float_t s, mat4* y);
extern void mat4_mul_scale(const mat4* x, float_t sx, float_t sy, float_t sz, float_t sw, mat4* z);
extern void mat4_mul_scale_x(const mat4* x, float_t s, mat4* z);
extern void mat4_mul_scale_y(const mat4* x, float_t s, mat4* z);
extern void mat4_mul_scale_z(const mat4* x, float_t s, mat4* z);
extern void mat4_scale_w_mult(const mat4* x, float_t s, mat4* z);
extern void mat4_scale_rot(const mat4* x, float_t sx, float_t sy, float_t sz, mat4* z);
extern void mat4_scale_x_rot(const mat4* x, float_t s, mat4* z);
extern void mat4_scale_y_rot(const mat4* x, float_t s, mat4* z);
extern void mat4_scale_z_rot(const mat4* x, float_t s, mat4* z);
extern void mat4_translate(float_t tx, float_t ty, float_t tz, mat4* z);
extern void mat4_translate_x(float_t t, mat4* y);
extern void mat4_translate_y(float_t t, mat4* y);
extern void mat4_translate_z(float_t t, mat4* y);
extern void mat4_mul_translate(const mat4* x, float_t tx, float_t ty, float_t tz, mat4* z);
extern void mat4_mul_translate_x(const mat4* x, float_t t, mat4* z);
extern void mat4_mul_translate_y(const mat4* x, float_t t, mat4* z);
extern void mat4_mul_translate_z(const mat4* x, float_t t, mat4* z);
extern void mat4_add_translate(const mat4* x, float_t tx, float_t ty, float_t tz, mat4* z);
extern void mat4_add_translate_x(const mat4* x, float_t t, mat4* z);
extern void mat4_add_translate_y(const mat4* x, float_t t, mat4* z);
extern void mat4_add_translate_z(const mat4* x, float_t t, mat4* z);
extern void mat4_from_quat(const quat* quat, mat4* mat);
extern void mat4_from_two_vectors(const vec3* x, const vec3* y, mat4* mat);
extern void mat4_from_axis_angle(const vec3* axis, float_t angle, mat4* mat);
extern void mat4_from_axis_angle(const vec3* axis, float_t s, float_t c, mat4* mat);
extern void mat4_from_mat3(const mat3* x, mat4* z);
extern void mat4_from_mat3_inverse(const mat3* x, mat4* z);
extern void mat4_clear_rot(mat4* x, mat4* z);
extern void mat4_clear_trans(mat4* x, mat4* z);
extern void mat4_get_scale(const mat4* x, vec3* z);
extern void mat4_get_rotation(const mat4* x, vec3* z);
extern void mat4_get_translation(const mat4* x, vec3* z);
extern void mat4_set_translation(mat4* x, vec3* z);
extern void mat4_blend(const mat4* x, const mat4* y, mat4* z, float_t blend);
extern void mat4_blend_rotation(const mat4* x, const mat4* y, mat4* z, float_t blend);
extern void mat4_lerp_rotation(const mat4* x, const mat4* y, mat4* z, float_t blend);
extern float_t mat4_get_max_scale(const mat4* x);
extern void mat4_mult_axis_angle(const mat4* x, const vec3* axis, const float_t angle, mat4* z);
extern void mat4_frustrum(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far, mat4* mat);
extern void mat4_ortho(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far, mat4* mat);
extern void mat4_persp(double_t fov_y, double_t aspect, double_t z_near, double_t z_far, mat4* mat);
extern void mat4_look_at(const vec3* eye, const vec3* target, const vec3* up, mat4* mat);
extern void mat4_look_at(const vec3* eye, const vec3* target, mat4* mat);

inline void mat3_rotate_xyz(const vec3* rad, mat3* z) {
    mat3_rotate_xyz(rad->x, rad->y, rad->z, z);
}

inline void mat3_rotate_xzy(const vec3* rad, mat3* z) {
    mat3_rotate_xzy(rad->x, rad->y, rad->z, z);
}

inline void mat3_rotate_yxz(const vec3* rad, mat3* z) {
    mat3_rotate_yxz(rad->x, rad->y, rad->z, z);
}

inline void mat3_rotate_yzx(const vec3* rad, mat3* z) {
    mat3_rotate_yzx(rad->x, rad->y, rad->z, z);
}

inline void mat3_rotate_zxy(const vec3* rad, mat3* z) {
    mat3_rotate_zxy(rad->x, rad->y, rad->z, z);
}

inline void mat3_rotate_zyx(const vec3* rad, mat3* z) {
    mat3_rotate_zyx(rad->x, rad->y, rad->z, z);
}

inline void mat3_mul_rotate_xyz(const mat3* x, const vec3* rad, mat3* z) {
    mat3_mul_rotate_xyz(x, rad->x, rad->y, rad->z, z);
}

inline void mat3_mul_rotate_xzy(const mat3* x, const vec3* rad, mat3* z) {
    mat3_mul_rotate_xzy(x, rad->x, rad->y, rad->z, z);
}

inline void mat3_mul_rotate_yxz(const mat3* x, const vec3* rad, mat3* z) {
    mat3_mul_rotate_yxz(x, rad->x, rad->y, rad->z, z);
}

inline void mat3_mul_rotate_yzx(const mat3* x, const vec3* rad, mat3* z) {
    mat3_mul_rotate_yzx(x, rad->x, rad->y, rad->z, z);
}

inline void mat3_mul_rotate_zxy(const mat3* x, const vec3* rad, mat3* z) {
    mat3_mul_rotate_zxy(x, rad->x, rad->y, rad->z, z);
}

inline void mat3_mul_rotate_zyx(const mat3* x, const vec3* rad, mat3* z) {
    mat3_mul_rotate_zyx(x, rad->x, rad->y, rad->z, z);
}

inline void mat3_scale(const vec3* s, mat3* z) {
    mat3_scale(s->x, s->y, s->z, z);
}

inline void mat3_mul_scale(const mat3* x, float_t s, mat3* z) {
    mat3_mul_scale(x, s, s, s, z);
}

inline void mat3_mul_scale(const mat3* x, const vec3* s, mat3* z) {
    mat3_mul_scale(x, s->x, s->y, s->z, z);
}

inline void mat4_rotate_xyz(const vec3* rad, mat4* z) {
    mat4_rotate_xyz(rad->x, rad->y, rad->z, z);
}

inline void mat4_rotate_xzy(const vec3* rad, mat4* z) {
    mat4_rotate_xzy(rad->x, rad->y, rad->z, z);
}

inline void mat4_rotate_yxz(const vec3* rad, mat4* z) {
    mat4_rotate_yxz(rad->x, rad->y, rad->z, z);
}

inline void mat4_rotate_yzx(const vec3* rad, mat4* z) {
    mat4_rotate_yzx(rad->x, rad->y, rad->z, z);
}

inline void mat4_rotate_zxy(const vec3* rad, mat4* z) {
    mat4_rotate_zxy(rad->x, rad->y, rad->z, z);
}

inline void mat4_rotate_zyx(const vec3* rad, mat4* z) {
    mat4_rotate_zyx(rad->x, rad->y, rad->z, z);
}

inline void mat4_mul_rotate_xyz(const mat4* x, const vec3* rad, mat4* z) {
    mat4_mul_rotate_xyz(x, rad->x, rad->y, rad->z, z);
}

inline void mat4_mul_rotate_xzy(const mat4* x, const vec3* rad, mat4* z) {
    mat4_mul_rotate_xzy(x, rad->x, rad->y, rad->z, z);
}

inline void mat4_mul_rotate_yxz(const mat4* x, const vec3* rad, mat4* z) {
    mat4_mul_rotate_yxz(x, rad->x, rad->y, rad->z, z);
}

inline void mat4_mul_rotate_yzx(const mat4* x, const vec3* rad, mat4* z) {
    mat4_mul_rotate_yzx(x, rad->x, rad->y, rad->z, z);
}

inline void mat4_mul_rotate_zxy(const mat4* x, const vec3* rad, mat4* z) {
    mat4_mul_rotate_zxy(x, rad->x, rad->y, rad->z, z);
}

inline void mat4_mul_rotate_zyx(const mat4* x, const vec3* rad, mat4* z) {
    mat4_mul_rotate_zyx(x, rad->x, rad->y, rad->z, z);
}

inline void mat4_scale(const vec3* s, mat4* z) {
    mat4_scale(s->x, s->y, s->z, z);
}

inline void mat4_mul_scale(const mat4* x, float_t s, mat4* z) {
    mat4_mul_scale(x, s, s, s, s, z);
}

inline void mat4_mul_scale(const mat4* x, vec4* s, mat4* z) {
    mat4_mul_scale(x, s->x, s->y, s->z, s->w, z);
}

inline void mat4_scale_rot(const mat4* x, const float_t s, mat4* z) {
    mat4_scale_rot(x, s, s, s, z);
}

inline void mat4_scale_rot(const mat4* x, const vec3* s, mat4* z) {
    mat4_scale_rot(x, s->x, s->y, s->z, z);
}

inline void mat4_translate(const vec3* s, mat4* z) {
    mat4_translate(s->x, s->y, s->z, z);
}

inline void mat4_mul_translate(const mat4* x, const vec3* t, mat4* z) {
    mat4_mul_translate(x, t->x, t->y, t->z, z);
}

inline void mat4_add_translate(const mat4* x, const vec3* t, mat4* z) {
    mat4_add_translate(x, t->x, t->y, t->z, z);
}
