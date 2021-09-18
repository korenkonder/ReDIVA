/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vec.h"

typedef union quat {
    struct {
        float_t x;
        float_t y;
        float_t z;
        float_t w;
    };
    __m128 data;
} quat;

static const quat quat_identity = { 0.0f, 0.0f, 0.0f, 1.0f };

extern void quat_mult(quat* x, quat* y, quat* z);
extern void quat_slerp(quat* x, quat* y, quat* z, float_t blend);
extern void quat_from_mat3(float_t m00, float_t m01, float_t m02, float_t m10,
    float_t m11, float_t m12, float_t m20, float_t m21, float_t m22, quat* quat);
extern void quat_from_axis_angle(vec3* axis, float_t angle, quat* quat);
