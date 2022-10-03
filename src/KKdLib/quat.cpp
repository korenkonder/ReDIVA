/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "quat.hpp"
#include <pmmintrin.h>
#include <xmmintrin.h>

inline void quat_mult(const quat* x, const quat* y, quat* z) {
    __m128 xt;
    __m128 yt;
    quat zt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 zt3;

    xt = _mm_loadu_ps((const float*)x);
    yt = _mm_loadu_ps((const float*)y);
    zt0 = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0x1B));
    zt1 = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0x4E));
    zt2 = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0xB1));
    zt3 = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0xE4));
    zt0 = _mm_xor_ps(zt0, __m128( {  0.0f,  0.0f, -0.0f, 0.0f } ));
    zt1 = _mm_xor_ps(zt1, __m128( { -0.0f,  0.0f,  0.0f, 0.0f } ));
    zt2 = _mm_xor_ps(zt2, __m128( {  0.0f, -0.0f,  0.0f, 0.0f } ));
    zt3 = _mm_xor_ps(zt3, __m128( { -0.0f, -0.0f, -0.0f, 0.0f } ));
    zt0 = _mm_hadd_ps(zt0, zt0);
    zt1 = _mm_hadd_ps(zt1, zt1);
    zt2 = _mm_hadd_ps(zt2, zt2);
    zt3 = _mm_hadd_ps(zt3, zt3);
    zt.x = _mm_cvtss_f32(_mm_hadd_ps(zt0, zt0));
    zt.y = _mm_cvtss_f32(_mm_hadd_ps(zt1, zt1));
    zt.z = _mm_cvtss_f32(_mm_hadd_ps(zt2, zt2));
    zt.w = _mm_cvtss_f32(_mm_hadd_ps(zt3, zt3));
    *z = zt;
}

void quat_from_mat3(float_t m00, float_t m01, float_t m02, float_t m10,
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

inline void quat_from_axis_angle(const vec3* axis, const float_t angle, quat* quat) {
    *(vec3*)quat = vec3::normalize(*axis) * sinf(angle * 0.5f);
    quat->w = cosf(angle * 0.5f);
}
