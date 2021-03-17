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

extern const mat3 mat3_identity;
extern const mat3 mat3_null;
extern const mat4 mat4_identity;
extern const mat4 mat4_null;

extern void FASTCALL mat3_add(mat3* x, mat3* y, mat3* z);
extern void FASTCALL mat3_sub(mat3* x, mat3* y, mat3* z);
extern void FASTCALL mat3_mult(mat3* x, mat3* y, mat3* z);
extern void FASTCALL mat3_mult_vec(mat3* x, vec3* y, vec3* z);
extern void FASTCALL mat3_mult_scalar(mat3* x, float_t y, mat3* z);
extern void FASTCALL mat3_transpose(mat3* x, mat3* z);
extern void FASTCALL mat3_inverse(mat3* x, mat3* z);
extern void FASTCALL mat3_invtrans(mat3* x, mat3* z);
extern void FASTCALL mat3_normalize(mat3* x, mat3* z);
extern void FASTCALL mat3_normalize_rotation(mat3* x, mat3* z);
extern float_t FASTCALL mat3_determinant(mat3* x);
extern void FASTCALL mat3_rotate(float_t x, float_t y, float_t z, mat3* w);
extern void FASTCALL mat3_rotate_x(float_t x, mat3* y);
extern void FASTCALL mat3_rotate_y(float_t x, mat3* y);
extern void FASTCALL mat3_rotate_z(float_t x, mat3* y);
extern void FASTCALL mat3_rotate_mult(mat3* s, float_t x, float_t y, float_t z, mat3* d);
extern void FASTCALL mat3_rotate_x_mult(mat3* x, float_t y, mat3* z);
extern void FASTCALL mat3_rotate_y_mult(mat3* x, float_t y, mat3* z);
extern void FASTCALL mat3_rotate_z_mult(mat3* x, float_t y, mat3* z);
extern void FASTCALL mat3_rotate(float_t x, float_t y, float_t z, mat3* w);
extern void FASTCALL mat3_scale_x(float_t x, mat3* y);
extern void FASTCALL mat3_scale_y(float_t x, mat3* y);
extern void FASTCALL mat3_scale_z(float_t x, mat3* y);
extern void FASTCALL mat3_scale_mult(mat3* s, float_t x, float_t y, float_t z, mat3* d);
extern void FASTCALL mat3_scale_x_mult(mat3* x, float_t y, mat3* z);
extern void FASTCALL mat3_scale_y_mult(mat3* x, float_t y, mat3* z);
extern void FASTCALL mat3_scale_z_mult(mat3* x, float_t y, mat3* z);
extern void FASTCALL mat3_from_quat(quat* quat, mat3* mat);

extern void FASTCALL mat4_add(mat4* x, mat4* y, mat4* z);
extern void FASTCALL mat4_sub(mat4* x, mat4* y, mat4* z);
extern void FASTCALL mat4_mult(mat4* x, mat4* y, mat4* z);
extern void FASTCALL mat4_mult_vec3(mat4* x, vec3* y, vec3* z);
extern void FASTCALL mat4_mult_vec(mat4* x, vec4* y, vec4* z);
extern void FASTCALL mat4_mult_scalar(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_transpose(mat4* x, mat4* z);
extern void FASTCALL mat4_inverse(mat4* x, mat4* z);
extern void FASTCALL mat4_invtrans(mat4* x, mat4* z);
extern void FASTCALL mat4_normalize(mat4* x, mat4* z);
extern void FASTCALL mat4_normalize_rotation(mat4* x, mat4* z);
extern float_t FASTCALL mat4_determinant(mat4* x);
extern void FASTCALL mat4_rotate(float_t x, float_t y, float_t z, mat4* d);
extern void FASTCALL mat4_rotate_x(float_t x, mat4* y);
extern void FASTCALL mat4_rotate_y(float_t x, mat4* y);
extern void FASTCALL mat4_rotate_z(float_t x, mat4* y);
extern void FASTCALL mat4_rotate_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void FASTCALL mat4_rotate_x_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_rotate_y_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_rotate_z_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_rot(mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void FASTCALL mat4_rot_x(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_rot_y(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_rot_z(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_scale(float_t x, float_t y, float_t z, mat4* d);
extern void FASTCALL mat4_scale_x(float_t x, mat4* y);
extern void FASTCALL mat4_scale_y(float_t x, mat4* y);
extern void FASTCALL mat4_scale_z(float_t x, mat4* y);
extern void FASTCALL mat4_scale_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void FASTCALL mat4_scale_x_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_scale_y_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_scale_z_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_scale_rot(mat4* s, float_t x, float_t y, float_t z, mat4* d);;
extern void FASTCALL mat4_scale_x_rot(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_scale_y_rot(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_scale_z_rot(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_translate(float_t x, float_t y, float_t z, mat4* d);
extern void FASTCALL mat4_translate_x(float_t x, mat4* y);
extern void FASTCALL mat4_translate_y(float_t x, mat4* y);
extern void FASTCALL mat4_translate_z(float_t x, mat4* y);
extern void FASTCALL mat4_translate_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d);
extern void FASTCALL mat4_translate_x_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_translate_y_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_translate_z_mult(mat4* x, float_t y, mat4* z);
extern void FASTCALL mat4_from_quat(quat* quat, mat4* mat);
