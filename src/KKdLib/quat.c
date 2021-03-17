/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "quat.h"

void quat_mult(quat* x, quat* y, quat* z) {
    quat xt;
    quat yt;
    quat zt;
    xt = *x;
    yt = *y;
    zt.x = xt.x * yt.w + xt.y * yt.z - xt.z * yt.y + xt.w * yt.x;
    zt.y = -xt.x * yt.z + xt.y * yt.w + xt.z * yt.x + xt.w * yt.y;
    zt.z = xt.x * yt.y - xt.y * yt.x + xt.z * yt.w + xt.w * yt.z;
    zt.w = -xt.x * yt.x - xt.y * yt.y - xt.z * yt.z + xt.w * yt.w;
    *z = zt;
}

void quat_slerp(quat* x, quat* y, quat* z, float_t blend) {
    quat xt;
    quat yt;
    quat zt;
    quat_normalize(x, &xt);
    quat_normalize(y, &yt);

    float_t dot = quat_dot(&xt, &yt);
    if (dot < 0.0f) {
        zt.x = -yt.x;
        zt.y = -yt.y;
        zt.z = -yt.z;
        zt.w = -yt.w;
        dot = -dot;
    }
    else
        zt = yt;

    const float_t DOT_THRESHOLD = 0.9995f;
    float_t s0, s1;
    if (dot <= DOT_THRESHOLD) {
        float_t theta_0 = acosf(dot);
        float_t theta = theta_0 * blend;
        float_t sin_theta = sinf(theta);
        float_t sin_theta_0 = sinf(theta_0);

        s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
        s1 = sin_theta / sin_theta_0;
    }
    else {
        s0 = (1.0f - blend);
        s1 = blend;
    }
    zt.x = s0 * x->x + s1 * zt.x;
    zt.y = s0 * x->y + s1 * zt.y;
    zt.z = s0 * x->z + s1 * zt.z;
    zt.w = s0 * x->w + s1 * zt.w;
    quat_normalize(&zt, &zt);
    *z = zt;
}

void FASTCALL quat_from_mat3(float_t m00, float_t m01, float_t m02, float_t m10,
    float_t m11, float_t m12, float_t m20, float_t m21, float_t m22, quat* quat) {
    float_t sq;

    if (m00 + m11 + m22 >= 0.0f) {
        sq = sqrtf(m00 + m11 + m22 + 1.0f);
        quat->w = 0.5f * sq;
        sq = 0.5f / sq;
        quat->x = (m12 - m21) * sq;
        quat->y = (m20 - m02) * sq;
        quat->z = (m01 - m10) * sq;
    }
    else if (m00 > m11 && m00 > m22) {
        sq = sqrtf(m00 - m11 - m22 + 1.0f);
        quat->x = 0.5f * sq;
        sq = 0.5f / sq;
        quat->y = (m10 + m01) * sq;
        quat->z = (m20 + m02) * sq;
        quat->w = (m12 - m21) * sq;
    }
    else if (m11 > m22) {
        sq = sqrtf(m11 - m00 - m22 + 1.0f);
        quat->y = 0.5f * sq;
        sq = 0.5f / sq;
        quat->x = (m10 + m01) * sq;
        quat->z = (m21 + m12) * sq;
        quat->w = (m20 - m02) * sq;
    }
    else {
        sq = sqrtf(m22 - m00 - m11 + 1.0f);
        quat->z = 0.5f * sq;
        sq = 0.5f / sq;
        quat->x = (m20 + m02) * sq;
        quat->y = (m21 + m12) * sq;
        quat->w = (m01 - m10) * sq;
    }
}

void FASTCALL quat_from_axis_angle(vec3* axis, float_t angle, quat* quat) {
    float_t angle_sin;
    float_t angle_cos;
    vec3 _axis;

    angle_sin = sinf(angle * 0.5f);
    angle_cos = cosf(angle * 0.5f);
    vec3_normalize(*axis, _axis);
    quat->w = angle_cos;
    vec3_mult_scalar(_axis, angle_sin, *(vec3*)quat);
}
