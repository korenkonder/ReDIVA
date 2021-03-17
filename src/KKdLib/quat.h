/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vec.h"

typedef struct quat {
    float_t x;
    float_t y;
    float_t z;
    float_t w;
} quat;

static const quat quat_identity = { 0.0f, 0.0f, 0.0f, 1.0f };

static inline float_t quat_dot(quat* x, quat* y) {
    float_t z = x->x * y->x + x->y * y->y + x->z * y->z + x->w * y->w;
    return z;
}

static inline float_t quat_length(quat* x) {
    float_t z = x->x * x->x + x->y * x->y + x->z * x->z + x->w * x->w;
    return sqrtf(z);
}

static inline float_t quat_length_squared(quat* x) {
    float_t z = x->x * x->x + x->y * x->y + x->z * x->z + x->w * x->w;
    return z;
}

static inline void quat_normalize(quat* x, quat* z) {
    float_t length = quat_length(x);
    if (length != 0)
        length = 1.0f / length;

    z->x = x->x * length;
    z->y = x->y * length;
    z->z = x->z * length;
    z->w = x->w * length;
}

extern void quat_mult(quat* x, quat* y, quat* z);
extern void quat_slerp(quat* x, quat* y, quat* z, float_t blend);
extern void FASTCALL quat_from_mat3(float_t m00, float_t m01, float_t m02, float_t m10,
    float_t m11, float_t m12, float_t m20, float_t m21, float_t m22, quat* quat);
extern void FASTCALL quat_from_axis_angle(vec3* axis, float_t angle, quat* quat);
