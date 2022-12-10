/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "vec.hpp"

struct quat {
    float_t x;
    float_t y;
    float_t z;
    float_t w;

    static float_t dot(const quat& left, const quat& right);
    static float_t length(const quat& left);
    static float_t length_squared(const quat& left);
    static float_t distance(const quat& left, const quat& right);
    static float_t distance_squared(const quat& left, const quat& right);
    static quat slerp(const quat& left, const quat& right, const float_t blend);
    static quat normalize(const quat& left);
    static quat rcp(const quat& left);
    static quat min(const quat& left, const quat& right);
    static quat max(const quat& left, const quat& right);
    static quat clamp(const quat& left, const quat& min, const quat& max);
    static quat clamp(const quat& left, const float_t min, const float_t max);
    static quat mult_min_max(const quat& left, const quat& min, const quat& max);
    static quat mult_min_max(const quat& left, const float_t min, const float_t max);
    static quat div_min_max(const quat& left, const quat& min, const quat& max);
    static quat div_min_max(const quat& left, const float_t min, const float_t max);
};

static const quat quat_identity = { 0.0f, 0.0f, 0.0f, 1.0f };

inline quat operator +(const quat& left, const quat& right) {
    __m128 yt;
    quat z;
    *(quat*)&yt = right;
    _mm_storeu_ps((float*)&z, _mm_add_ps(_mm_loadu_ps((const float*)&left), yt));
    return z;
}

inline quat operator +(const quat& left, const float_t right) {
    __m128 yt;
    quat z;
    yt = _mm_set_ss(right);
    _mm_storeu_ps((float*)&z, _mm_add_ps(_mm_loadu_ps((const float*)&left), _mm_shuffle_ps(yt, yt, 0)));
    return z;
}

inline quat operator -(const quat& left, const quat& right) {
    __m128 yt;
    quat z;
    *(quat*)&yt = right;
    _mm_storeu_ps((float*)&z, _mm_sub_ps(_mm_loadu_ps((const float*)&left), yt));
    return z;
}

inline quat operator -(const quat& left, const float_t right) {
    __m128 yt;
    quat z;
    yt = _mm_set_ss(right);
    _mm_storeu_ps((float*)&z, _mm_sub_ps(_mm_loadu_ps((const float*)&left), _mm_shuffle_ps(yt, yt, 0)));
    return z;
}

inline quat operator *(const quat& left, const quat& right) {
    __m128 yt;
    quat z;
    *(quat*)&yt = right;
    _mm_storeu_ps((float*)&z, _mm_mul_ps(_mm_loadu_ps((const float*)&left), yt));
    return z;
}

inline quat operator *(const quat& left, const float_t right) {
    __m128 yt;
    quat z;
    yt = _mm_set_ss(right);
    _mm_storeu_ps((float*)&z, _mm_mul_ps(_mm_loadu_ps((const float*)&left), _mm_shuffle_ps(yt, yt, 0)));
    return z;
}

inline quat operator /(const quat& left, const quat& right) {
    __m128 yt;
    quat z;
    *(quat*)&yt = right;
    _mm_storeu_ps((float*)&z, _mm_div_ps(_mm_loadu_ps((const float*)&left), yt));
    return z;
}

inline quat operator /(const quat& left, const float_t right) {
    __m128 yt;
    quat z;
    yt = _mm_set_ss(right);
    _mm_storeu_ps((float*)&z, _mm_div_ps(_mm_loadu_ps((const float*)&left), _mm_shuffle_ps(yt, yt, 0)));
    return z;
}

inline quat operator &(const quat& left, const quat& right) {
    __m128 yt;
    quat z;
    *(quat*)&yt = right;
    _mm_storeu_ps((float*)&z, _mm_and_ps(_mm_loadu_ps((const float*)&left), yt));
    return z;
}

inline quat operator &(const quat& left, const float_t right) {
    __m128 yt;
    quat z;
    yt = _mm_set_ss(right);
    _mm_storeu_ps((float*)&z, _mm_and_ps(_mm_loadu_ps((const float*)&left), _mm_shuffle_ps(yt, yt, 0)));
    return z;
}

inline quat operator ^(const quat& left, const quat& right) {
    __m128 yt;
    quat z;
    *(quat*)&yt = right;
    _mm_storeu_ps((float*)&z, _mm_xor_ps(_mm_loadu_ps((const float*)&left), yt));
    return z;
}

inline quat operator ^(const quat& left, const float_t right) {
    __m128 yt;
    quat z;
    yt = _mm_set_ss(right);
    _mm_storeu_ps((float*)&z, _mm_xor_ps(_mm_loadu_ps((const float*)&left), _mm_shuffle_ps(yt, yt, 0)));
    return z;
}

inline quat operator -(const quat& left) {
    quat z;
    _mm_storeu_ps((float*)&z, _mm_xor_ps(_mm_loadu_ps((const float*)&left), vec4_neg));
    return z;
}

inline float_t quat::dot(const quat& left, const quat& right) {
    __m128 zt;
    zt = _mm_mul_ps(_mm_loadu_ps((const float*)&(left)), _mm_loadu_ps((const float*)&(right)));
    zt = _mm_hadd_ps(zt, zt);
    return _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

inline float_t quat::length(const quat& left) {
    __m128 xt;
    __m128 zt;
    xt = _mm_loadu_ps((const float*)&left);
    zt = _mm_mul_ps(xt, xt);
    zt = _mm_hadd_ps(zt, zt);
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt)));
}

inline float_t quat::length_squared(const quat& left) {
    __m128 xt;
    __m128 zt;
    xt = _mm_loadu_ps((const float*)&left);
    zt = _mm_mul_ps(xt, xt);
    zt = _mm_hadd_ps(zt, zt);
    return _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

inline float_t quat::distance(const quat& left, const quat& right) {
    __m128 zt;
    zt = _mm_sub_ps(_mm_loadu_ps((const float*)&(left)), _mm_loadu_ps((const float*)&(right)));
    zt = _mm_mul_ps(zt, zt);
    zt = _mm_hadd_ps(zt, zt);
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt)));
}

inline float_t quat::distance_squared(const quat& left, const quat& right) {
    __m128 zt;
    zt = _mm_sub_ps(_mm_loadu_ps((const float*)&(left)), _mm_loadu_ps((const float*)&(right)));
    zt = _mm_mul_ps(zt, zt);
    zt = _mm_hadd_ps(zt, zt);
    return _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

inline quat quat::slerp(const quat& left, const quat& right, const float_t blend) {
    quat x_t;
    quat y_t;
    quat z_t;
    x_t = quat::normalize(left);
    y_t = quat::normalize(right);

    float_t dot = quat::dot(x_t, y_t);
    if (dot < 0.0f) {
        z_t = -y_t;
        dot = -dot;
    }
    else
        z_t = y_t;

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
    return quat::normalize(left * s0 + z_t * s0);
}

inline quat quat::normalize(const quat& left) {
    __m128 xt;
    __m128 zt;
    quat z;
    xt = _mm_loadu_ps((const float*)&left);
    zt = _mm_mul_ps(xt, xt);
    zt = _mm_hadd_ps(zt, zt);
    zt = _mm_sqrt_ss(_mm_hadd_ps(zt, zt));
    if (zt.m128_f32[0] != 0.0f)
        zt.m128_f32[0] = 1.0f / zt.m128_f32[0];
    _mm_storeu_ps((float*)&z, _mm_mul_ps(xt, _mm_shuffle_ps(zt, zt, 0)));
    return z;
}

inline quat quat::rcp(const quat& left) {
    quat z;
    _mm_storeu_ps((float*)&z, _mm_div_ps(_mm_loadu_ps((const float*)&(quat_identity)), _mm_loadu_ps((const float*)&left)));
    return z;
}

inline quat quat::min(const quat& left, const quat& right) {
    quat z;
    _mm_storeu_ps((float*)&z, _mm_min_ps(_mm_loadu_ps((const float*)&(left)), _mm_loadu_ps((const float*)&(right))));
    return z;
}

inline quat quat::max(const quat& left, const quat& right) {
    quat z;
    _mm_storeu_ps((float*)&z, _mm_max_ps(_mm_loadu_ps((const float*)&(left)), _mm_loadu_ps((const float*)&(right))));
    return z;
}

inline quat quat::clamp(const quat& left, const quat& min, const quat& max) {
    quat w;
    _mm_storeu_ps((float*)&w, _mm_min_ps(_mm_max_ps(_mm_loadu_ps((const float*)&left),
        _mm_loadu_ps((const float*)&(min))), _mm_loadu_ps((const float*)&(max))));
    return w;
}

inline quat quat::clamp(const quat& left, const float_t min, const float_t max) {
    __m128 yt;
    __m128 zt;
    quat w;
    yt = _mm_set_ss(min);
    zt = _mm_set_ss(max);
    _mm_storeu_ps((float*)&w, _mm_min_ps(_mm_max_ps(_mm_loadu_ps((const float*)&left),
        _mm_shuffle_ps(yt, yt, 0)), _mm_shuffle_ps(zt, zt, 0)));
    return w;
}

inline quat quat::mult_min_max(const quat& left, const quat& min, const quat& max) {
    __m128 xt;
    __m128 yt;
    __m128 wt;
    quat w;
    xt = _mm_loadu_ps((const float*)&left);
    yt = _mm_xor_ps(_mm_loadu_ps((const float*)&(min)), vec4_neg);
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps(xt, vec4::load_xmm(0.0f))),
        _mm_and_ps(_mm_loadu_ps((const float*)&(max)), _mm_cmpge_ps(xt, vec4::load_xmm(0.0f))));
    _mm_storeu_ps((float*)&w, _mm_mul_ps(xt, wt));
    return w;
}

inline quat quat::mult_min_max(const quat& left, const float_t min, const float_t max) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    __m128 wt;
    quat w;
    xt = _mm_loadu_ps((const float*)&left);
    yt = _mm_set_ss(min);
    yt = _mm_shuffle_ps(yt, yt, 0);
    zt = _mm_set_ss(max);
    zt = _mm_shuffle_ps(zt, zt, 0);
    yt = _mm_xor_ps(yt, vec4_neg);
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps(xt, vec4::load_xmm(0.0f))),
        _mm_and_ps(zt, _mm_cmpge_ps(xt, vec4::load_xmm(0.0f))));
    _mm_storeu_ps((float*)&w, _mm_mul_ps(xt, wt));
    return w;
}

inline quat quat::div_min_max(const quat& left, const quat& min, const quat& max) {
    __m128 xt;
    __m128 yt;
    __m128 wt;
    quat w;
    xt = _mm_loadu_ps((const float*)&left);
    yt = _mm_xor_ps(_mm_loadu_ps((const float*)&(min)), vec4_neg);
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps(xt, vec4::load_xmm(0.0f))),
        _mm_and_ps(_mm_loadu_ps((const float*)&(max)), _mm_cmpge_ps(xt, vec4::load_xmm(0.0f))));
    _mm_storeu_ps((float*)&w, _mm_div_ps(xt, wt));
    return w;
}

inline quat quat::div_min_max(const quat& left, const float_t min, const float_t max) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    __m128 wt;
    quat w;
    xt = _mm_loadu_ps((const float*)&left);
    yt = _mm_set_ss(min);
    yt = _mm_shuffle_ps(yt, yt, 0);
    zt = _mm_set_ss(max);
    zt = _mm_shuffle_ps(zt, zt, 0);
    yt = _mm_xor_ps(yt, vec4_neg);
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps(xt, vec4::load_xmm(0.0f))),
        _mm_and_ps(zt, _mm_cmpge_ps(xt, vec4::load_xmm(0.0f))));
    _mm_storeu_ps((float*)&w, _mm_div_ps(xt, wt));
    return w;
}

extern void quat_mult(const quat* x, const quat* y, quat* z);
extern void quat_from_mat3(float_t m00, float_t m01, float_t m02, float_t m10,
    float_t m11, float_t m12, float_t m20, float_t m21, float_t m22, quat* quat);
extern void quat_from_axis_angle(const vec3* axis, const float_t angle, quat* quat);
