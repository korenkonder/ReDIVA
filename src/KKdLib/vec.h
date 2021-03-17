/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include <pmmintrin.h>
#include <xmmintrin.h>

#define v2(t) typedef struct vec2_##t { \
    t x; \
    t y; \
} vec2_##t;

#define v3(t) typedef struct vec3_##t { \
    t x; \
    t y; \
    t z; \
} vec3_##t;

#define v4(t) typedef struct vec4_##t { \
    t x; \
    t y; \
    t z; \
    t w; \
} vec4_##t;

typedef struct vec2i {
    int32_t x;
    int32_t y;
} vec2i;

typedef struct vec3i {
    int32_t x;
    int32_t y;
    int32_t z;
} vec3i;

typedef struct vec4i {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t w;
} vec4i;

typedef struct vec2 {
    float_t x;
    float_t y;
} vec2;

typedef struct vec3 {
    float_t x;
    float_t y;
    float_t z;
} vec3;

typedef struct vec4 {
    float_t x;
    float_t y;
    float_t z;
    float_t w;
} vec4;

extern const __m128 vec2_negate;
extern const __m128 vec3_negate;
extern const __m128 vec4_negate;

extern const vec2 vec2_identity;
extern const vec3 vec3_identity;
extern const vec4 vec4_identity;

extern const vec2 vec2_null;
extern const vec3 vec3_null;
extern const vec4 vec4_null;

#define vec2_add(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_add_ps(xt, yt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_add_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_add_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec2*)&zt; \
}

#define vec2_sub(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_sub_ps(xt, yt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_sub_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_sub_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec2*)&zt; \
}

#define vec2_mult(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_mul_ps(xt, yt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_mult_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec2*)&zt; \
}

#define vec2_div(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_div_ps(xt, yt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_div_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_div_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec2*)&zt; \
}

#define vec2_and(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_and_ps(xt, yt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_and_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_and_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec2*)&zt; \
}

#define vec2_xor(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_xor_ps(xt, yt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_xor_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_xor_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec2*)&zt; \
}

#define vec2_negate(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    zt = _mm_xor_ps(xt, vec2_negate); \
    (z) = *(vec2*)&zt; \
}

#define vec2_dot(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_mul_ps(xt, yt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec2_length(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec2_length_squared(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec2_distance(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_sub_ps(xt, yt); \
    zt = _mm_mul_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec2_distance_squared(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_sub_ps(xt, yt); \
    zt = _mm_mul_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec2_lerp(x, y, z, blend) \
{ \
    __m128 b0; \
    __m128 b1; \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    b0 = _mm_set_ss(1.0f - blend); \
    b0 = _mm_shuffle_ps(b0, b0, 0); \
    b1 = _mm_set_ss(blend); \
    b1 = _mm_shuffle_ps(b1, b1, 0); \
    zt = _mm_add_ps(_mm_mul_ps(xt, b0), _mm_mul_ps(yt, b1)); \
    (z) = *(vec2*)&zt; \
}

#define vec2_normalize(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_sqrt_ss(_mm_hadd_ps(zt, zt)); \
    if (zt.m128_f32[0] != 0.0f) \
        zt.m128_f32[0] = 1.0f / zt.m128_f32[0]; \
    zt = _mm_mul_ps(xt, _mm_shuffle_ps(zt, zt, 0)); \
    (z) = *(vec2*)&zt; \
}

#define vec3_add(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_add_ps(xt, yt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_add_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_add_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_sub(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_sub_ps(xt, yt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_sub_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_sub_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_mult(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_mul_ps(xt, yt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_mult_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_div(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_div_ps(xt, yt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_div_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_div_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_and(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_and_ps(xt, yt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_and_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_and_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_xor(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_xor_ps(xt, yt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_xor_scalar(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_xor_ps(xt, _mm_shuffle_ps(yt, yt, 0)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_negate(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    zt = _mm_xor_ps(xt, vec3_negate); \
    (z) = *(vec3*)&zt; \
}

#define vec3_dot(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_mul_ps(xt, yt); \
    zt.m128_f32[3] = 0.0f; \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec3_length(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    zt.m128_f32[3] = 0.0f; \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec3_length_squared(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    zt.m128_f32[3] = 0.0f; \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec3_distance(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_sub_ps(xt, yt); \
    zt = _mm_mul_ps(zt, zt); \
    zt.m128_f32[3] = 0.0f; \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec3_distance_squared(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_sub_ps(xt, yt); \
    zt = _mm_mul_ps(zt, zt); \
    zt.m128_f32[3] = 0.0f; \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec3_lerp(x, y, z, blend) \
{ \
    __m128 b0; \
    __m128 b1; \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    b0 = _mm_set_ss(1.0f - blend); \
    b0 = _mm_shuffle_ps(b0, b0, 0); \
    b1 = _mm_set_ss(blend); \
    b1 = _mm_shuffle_ps(b1, b1, 0); \
    zt = _mm_add_ps(_mm_mul_ps(xt, b0), _mm_mul_ps(yt, b1)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_normalize(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    zt.m128_f32[3] = 0.0f; \
    zt = _mm_hadd_ps(zt, zt); \
    zt = _mm_sqrt_ss(_mm_hadd_ps(zt, zt)); \
    if (zt.m128_f32[0] != 0.0f) \
        zt.m128_f32[0] = 1.0f / zt.m128_f32[0]; \
    zt = _mm_mul_ps(xt, _mm_shuffle_ps(zt, zt, 0)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_cross(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_sub_ps( \
        _mm_mul_ps(_mm_shuffle_ps(xt, xt, 0b001001), _mm_shuffle_ps(yt, yt, 0b010010)), \
        _mm_mul_ps(_mm_shuffle_ps(xt, xt, 0b010010), _mm_shuffle_ps(yt, yt, 0b001001)) \
    ); \
    (z) = *(vec3*)&zt; \
}

#define vec4_add(x, y, z) \
{ \
    *(__m128*)&(z) = _mm_add_ps(*(__m128*)&(x), *(__m128*)&(y)); \
}

#define vec4_add_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    *(__m128*)&(z) = _mm_add_ps(*(__m128*)&(x), _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_sub(x, y, z) \
{ \
    *(__m128*)&(z) = _mm_sub_ps(*(__m128*)&(x), *(__m128*)&(y)); \
}

#define vec4_sub_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    *(__m128*)&(z) = _mm_sub_ps(*(__m128*)&(x), _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_mult(x, y, z) \
{ \
    *(__m128*)&(z) = _mm_mul_ps(*(__m128*)&(x), *(__m128*)&(y)); \
}

#define vec4_mult_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    *(__m128*)&(z) = _mm_mul_ps(*(__m128*)&(x), _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_div(x, y, z) \
{ \
    *(__m128*)&(z) = _mm_div_ps(*(__m128*)&(x), *(__m128*)&(y)); \
}

#define vec4_div_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    *(__m128*)&(z) = _mm_div_ps(*(__m128*)&(x), _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_and(x, y, z) \
{ \
    *(__m128*)&(z) = _mm_and_ps(*(__m128*)&(x), *(__m128*)&(y)); \
}

#define vec4_and_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    *(__m128*)&(z) = _mm_and_ps(*(__m128*)&(x), _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_xor(x, y, z) \
{ \
    *(__m128*)&(z) = _mm_xor_ps(*(__m128*)&(x), *(__m128*)&(y)); \
}

#define vec4_xor_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    *(__m128*)&(z) = _mm_xor_ps(*(__m128*)&(x), _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_negate(x, z) \
{ \
    *(__m128*)&(z) = _mm_xor_ps(*(__m128*)&(x), vec4_negate); \
}

#define vec4_dot(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec4*)&xt = (x); \
    *(vec4*)&yt = (y); \
    zt = _mm_mul_ps(xt, yt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec4_length(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec4*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec4_length_squared(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec4*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec4_distance(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec4*)&xt = (x); \
    *(vec4*)&yt = (y); \
    zt = _mm_sub_ps(xt, yt); \
    zt = _mm_mul_ps(zt, zt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec4_distance_squared(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec4*)&xt = (x); \
    *(vec4*)&yt = (y); \
    zt = _mm_sub_ps(xt, yt); \
    zt = _mm_mul_ps(zt, zt); \
    zt.m128_f32[3] = 0.0f; \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec4_lerp(x, y, z, blend) \
{ \
    __m128 b0; \
    __m128 b1; \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec4*)&xt = (x); \
    *(vec4*)&yt = (y); \
    b0 = _mm_set_ss(1.0f - blend); \
    b0 = _mm_shuffle_ps(b0, b0, 0); \
    b1 = _mm_set_ss(blend); \
    b1 = _mm_shuffle_ps(b1, b1, 0); \
    zt = _mm_add_ps(_mm_mul_ps(xt, b0), _mm_mul_ps(yt, b1)); \
    (z) = *(vec4*)&zt; \
}

#define vec4_normalize(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec4*)&xt = (x); \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    zt = _mm_sqrt_ss(_mm_hadd_ps(zt, zt)); \
    if (zt.m128_f32[0] != 0.0f) \
        zt.m128_f32[0] = 1.0f / zt.m128_f32[0]; \
    zt = _mm_mul_ps(xt, _mm_shuffle_ps(zt, zt, 0)); \
    (z) = *(vec4*)&zt; \
}
