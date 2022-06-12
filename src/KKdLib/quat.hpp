/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "vec.hpp"

union quat {
    struct {
        float_t x;
        float_t y;
        float_t z;
        float_t w;
    };
    __m128 data;
};

static const quat quat_identity = { 0.0f, 0.0f, 0.0f, 1.0f };

extern void quat_mult(const quat* x, const quat* y, quat* z);
extern void quat_slerp(const quat* x, const quat* y, quat* z, const float_t blend);
extern void quat_from_mat3(float_t m00, float_t m01, float_t m02, float_t m10,
    float_t m11, float_t m12, float_t m20, float_t m21, float_t m22, quat* quat);
extern void quat_from_axis_angle(const vec3* axis, const float_t angle, quat* quat);
