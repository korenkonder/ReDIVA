/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "half_t.h"
#include <pmmintrin.h>
#include <xmmintrin.h>

#define v2(t) \
typedef struct vec2_##t { \
    t x; \
    t y; \
} vec2_##t;

#define v3(t) \
typedef struct vec3_##t { \
    t x; \
    t y; \
    t z; \
} vec3_##t;

#define v4(t) \
typedef struct vec4_##t { \
    t x; \
    t y; \
    t z; \
    t w; \
} vec4_##t;

typedef struct vec2i8 {
    int8_t x;
    int8_t y;
} vec2i8;

typedef struct vec3i8 {
    int8_t x;
    int8_t y;
    int8_t z;
} vec3i8;

typedef struct vec4i8 {
    int8_t x;
    int8_t y;
    int8_t z;
    int8_t w;
} vec4i8;

typedef struct vec2u8 {
    uint8_t x;
    uint8_t y;
} vec2u8;

typedef struct vec3u8 {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} vec3u8;

typedef struct vec4u8 {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t w;
} vec4u8;

typedef struct vec2i16 {
    int16_t x;
    int16_t y;
} vec2i16;

typedef struct vec3i16 {
    int16_t x;
    int16_t y;
    int16_t z;
} vec3i16;

typedef struct vec4i16 {
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t w;
} vec4i16;

typedef struct vec2u16 {
    uint16_t x;
    uint16_t y;
} vec2u16;

typedef struct vec3u16 {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} vec3u16;

typedef struct vec4u16 {
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint16_t w;
} vec4u16;

typedef struct vec2h {
    half_t x;
    half_t y;
} vec2h;

typedef struct vec3h {
    half_t x;
    half_t y;
    half_t z;
} vec3h;

typedef struct vec4h {
    half_t x;
    half_t y;
    half_t z;
    half_t w;
} vec4h;

typedef struct vec2 {
    float_t x;
    float_t y;
} vec2;

typedef struct vec3 {
    float_t x;
    float_t y;
    float_t z;
} vec3;

typedef union vec4 {
    struct {
        float_t x;
        float_t y;
        float_t z;
        float_t w;
    };
    __m128 data;
} vec4;

typedef struct vec2i {
    int32_t x;
    int32_t y;
} vec2i;

typedef struct vec3i {
    int32_t x;
    int32_t y;
    int32_t z;
} vec3i;

typedef union vec4i {
    struct {
        int32_t x;
        int32_t y;
        int32_t z;
        int32_t w;
    };
    __m128i data;
} vec4i;

typedef struct vec2d {
    struct {
        double_t x;
        double_t y;
    };
    __m128d data;
} vec2d;

extern const __m128 vec2_negate;
extern const __m128 vec3_negate;
extern const __m128 vec4_negate;

extern const __m128 vec4_negate;

extern const vec2 vec2_identity;
extern const vec3 vec3_identity;
extern const vec4 vec4_identity;

extern const vec2 vec2_null;
extern const vec3 vec3_null;
extern const vec4 vec4_null;

extern const vec2i vec2i_null;
extern const vec3i vec3i_null;
extern const vec4i vec4i_null;

extern const vec2d vec2d_identity;

extern const vec2d vec2d_null;

#define vec2i8_to_vec2(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y;

#define vec3i8_to_vec3(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y; \
(dst).z = (float_t)(src).z;

#define vec4i8_to_vec4(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y; \
(dst).z = (float_t)(src).z; \
(dst).w = (float_t)(src).w;

#define vec2_to_vec2i8(src, dst) \
(dst).x = (int8_t)(src).x; \
(dst).y = (int8_t)(src).y;

#define vec3_to_vec3i8(src, dst) \
(dst).x = (int8_t)(src).x; \
(dst).y = (int8_t)(src).y; \
(dst).z = (int8_t)(src).z;

#define vec4_to_vec4i8(src, dst) \
(dst).x = (int8_t)(src).x; \
(dst).y = (int8_t)(src).y; \
(dst).z = (int8_t)(src).z; \
(dst).w = (int8_t)(src).w;

#define vec2u8_to_vec2(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y;

#define vec3u8_to_vec3(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y; \
(dst).z = (float_t)(src).z;

#define vec4u8_to_vec4(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y; \
(dst).z = (float_t)(src).z; \
(dst).w = (float_t)(src).w;

#define vec2_to_vec2u8(src, dst) \
(dst).x = (uint8_t)(src).x; \
(dst).y = (uint8_t)(src).y;

#define vec3_to_vec3u8(src, dst) \
(dst).x = (uint8_t)(src).x; \
(dst).y = (uint8_t)(src).y; \
(dst).z = (uint8_t)(src).z;

#define vec4_to_vec4u8(src, dst) \
(dst).x = (uint8_t)(src).x; \
(dst).y = (uint8_t)(src).y; \
(dst).z = (uint8_t)(src).z; \
(dst).w = (uint8_t)(src).w;

#define vec2i16_to_vec2(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y;

#define vec3i16_to_vec3(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y; \
(dst).z = (float_t)(src).z;

#define vec4i16_to_vec4(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y; \
(dst).z = (float_t)(src).z; \
(dst).w = (float_t)(src).w;

#define vec2_to_vec2i16(src, dst) \
(dst).x = (uint16_t)(src).x; \
(dst).y = (uint16_t)(src).y;

#define vec3_to_vec3i16(src, dst) \
(dst).x = (uint16_t)(src).x; \
(dst).y = (uint16_t)(src).y; \
(dst).z = (uint16_t)(src).z;

#define vec4_to_vec4i16(src, dst) \
(dst).x = (uint16_t)(src).x; \
(dst).y = (uint16_t)(src).y; \
(dst).z = (uint16_t)(src).z; \
(dst).w = (uint16_t)(src).w;

#define vec2u16_to_vec2(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y;

#define vec3u16_to_vec3(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y; \
(dst).z = (float_t)(src).z;

#define vec4u16_to_vec4(src, dst) \
(dst).x = (float_t)(src).x; \
(dst).y = (float_t)(src).y; \
(dst).z = (float_t)(src).z; \
(dst).w = (float_t)(src).w;

#define vec2_to_vec2u16(src, dst) \
(dst).x = (uint16_t)(src).x; \
(dst).y = (uint16_t)(src).y;

#define vec3_to_vec3u16(src, dst) \
(dst).x = (uint16_t)(src).x; \
(dst).y = (uint16_t)(src).y; \
(dst).z = (uint16_t)(src).z;

#define vec4_to_vec4u16(src, dst) \
(dst).x = (uint16_t)(src).x; \
(dst).y = (uint16_t)(src).y; \
(dst).z = (uint16_t)(src).z; \
(dst).w = (uint16_t)(src).w;

#define vec2h_to_vec2(src, dst) \
(dst).x = half_to_float((src).x); \
(dst).y = half_to_float((src).y);

#define vec3h_to_vec3(src, dst) \
(dst).x = half_to_float((src).x); \
(dst).y = half_to_float((src).y); \
(dst).z = half_to_float((src).z);

#define vec4h_to_vec4(src, dst) \
(dst).x = half_to_float((src).x); \
(dst).y = half_to_float((src).y); \
(dst).z = half_to_float((src).z); \
(dst).w = half_to_float((src).w);

#define vec2_to_vec2h(src, dst) \
(dst).x = float_to_half((src).x); \
(dst).y = float_to_half((src).y);

#define vec3_to_vec3h(src, dst) \
(dst).x = float_to_half((src).x); \
(dst).y = float_to_half((src).y); \
(dst).z = float_to_half((src).z);

#define vec4_to_vec4h(src, dst) \
(dst).x = float_to_half((src).x); \
(dst).y = float_to_half((src).y); \
(dst).z = float_to_half((src).z); \
(dst).w = float_to_half((src).w);

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
    __m128 b; \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    *(vec2*)&b = (blend); \
    zt = _mm_add_ps(xt, _mm_mul_ps(_mm_sub_ps(yt, xt), b)); \
    (z) = *(vec2*)&zt; \
}

#define vec2_lerp_scalar(x, y, z, blend) \
{ \
    __m128 b; \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    b = _mm_set_ss(blend); \
    b = _mm_shuffle_ps(b, b, 0); \
    zt = _mm_add_ps(xt, _mm_mul_ps(_mm_sub_ps(yt, xt), b)); \
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

#define vec2_rcp(x, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = vec2_identity; \
    zt = _mm_div_ps(yt, xt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_min(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_min_ps(xt, yt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_max(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    zt = _mm_max_ps(xt, yt); \
    (z) = *(vec2*)&zt; \
}

#define vec2_clamp(x, y, z, w) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    *(vec2*)&zt = (z); \
    wt = _mm_min_ps(_mm_max_ps(xt, yt), zt); \
    (w) = *(vec2*)&wt; \
}

#define vec2_clamp_scalar(x, y, z, w) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    *(vec2*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_set_ss(z); \
    wt = _mm_min_ps(_mm_max_ps(xt, _mm_shuffle_ps(yt, yt, 0)), _mm_shuffle_ps(zt, zt, 0)); \
    (w) = *(vec2*)&wt; \
}

#define vec2_mult_min_max(x, y, z, w) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    *(vec2*)&xt = (x); \
    *(vec2*)&yt = (y); \
    *(vec2*)&zt = (z); \
    yt = _mm_xor_ps(yt, vec2_negate); \
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps(xt, vec4_null.data)), \
        _mm_and_ps(zt, _mm_cmpgt_ps(xt, vec4_null.data))); \
    wt = _mm_mul_ps(xt, wt); \
    (w) = *(vec2*)&wt; \
}

#define vec2_mult_min_max_scalar(x, y, z, w) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    *(vec2*)&xt = (x); \
    yt = _mm_set_ss(y); \
    yt = _mm_shuffle_ps(yt, yt, 0); \
    zt = _mm_set_ss(z); \
    zt = _mm_shuffle_ps(zt, zt, 0); \
    yt = _mm_xor_ps(yt, vec2_negate); \
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps(xt, vec4_null.data)), \
        _mm_and_ps(zt, _mm_cmpgt_ps(xt, vec4_null.data))); \
    wt = _mm_mul_ps(xt, wt); \
    (w) = *(vec2*)&wt; \
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
    xt.m128_f32[3] = 0.0f; \
    *(vec3*)&yt = (y); \
    yt.m128_f32[3] = 0.0f; \
    zt = _mm_mul_ps(xt, yt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec3_length(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    xt.m128_f32[3] = 0.0f; \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec3_length_squared(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    xt.m128_f32[3] = 0.0f; \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec3_distance(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    xt.m128_f32[3] = 0.0f; \
    *(vec3*)&yt = (y); \
    yt.m128_f32[3] = 0.0f; \
    zt = _mm_sub_ps(xt, yt); \
    zt = _mm_mul_ps(zt, zt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec3_distance_squared(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    xt.m128_f32[3] = 0.0f; \
    *(vec3*)&yt = (y); \
    yt.m128_f32[3] = 0.0f; \
    zt = _mm_sub_ps(xt, yt); \
    zt = _mm_mul_ps(zt, zt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec3_lerp(x, y, z, blend) \
{ \
    __m128 b; \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    *(vec3*)&b = (blend); \
    zt = _mm_add_ps(xt, _mm_mul_ps(_mm_sub_ps(yt, xt), b)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_lerp_scalar(x, y, z, blend) \
{ \
    __m128 b; \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    b = _mm_set_ss(blend); \
    b = _mm_shuffle_ps(b, b, 0); \
    zt = _mm_add_ps(xt, _mm_mul_ps(_mm_sub_ps(yt, xt), b)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_normalize(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    xt.m128_f32[3] = 0.0f; \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    zt = _mm_sqrt_ss(_mm_hadd_ps(zt, zt)); \
    if (zt.m128_f32[0] != 0.0f) \
        zt.m128_f32[0] = 1.0f / zt.m128_f32[0]; \
    zt = _mm_mul_ps(xt, _mm_shuffle_ps(zt, zt, 0)); \
    (z) = *(vec3*)&zt; \
}

#define vec3_rcp(x, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = vec3_identity; \
    zt = _mm_div_ps(yt, xt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_min(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_min_ps(xt, yt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_max(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_max_ps(xt, yt); \
    (z) = *(vec3*)&zt; \
}

#define vec3_clamp(x, y, z, w) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    *(vec3*)&zt = (z); \
    wt = _mm_min_ps(_mm_max_ps(xt, yt), zt); \
    (w) = *(vec3*)&wt; \
}

#define vec3_clamp_scalar(x, y, z, w) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    *(vec3*)&xt = (x); \
    yt = _mm_set_ss(y); \
    zt = _mm_set_ss(z); \
    wt = _mm_min_ps(_mm_max_ps(xt, _mm_shuffle_ps(yt, yt, 0)), _mm_shuffle_ps(zt, zt, 0)); \
    (w) = *(vec3*)&wt; \
}

#define vec3_mult_min_max(x, y, z, w) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    *(vec3*)&zt = (z); \
    yt = _mm_xor_ps(yt, vec3_negate); \
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps(xt, vec4_null.data)), \
        _mm_and_ps(zt, _mm_cmpgt_ps(xt, vec4_null.data))); \
    wt = _mm_mul_ps(xt, wt); \
    (w) = *(vec3*)&wt; \
}

#define vec3_mult_min_max_scalar(x, y, z, w) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    *(vec3*)&xt = (x); \
    yt = _mm_set_ss(y); \
    yt = _mm_shuffle_ps(yt, yt, 0); \
    zt = _mm_set_ss(z); \
    zt = _mm_shuffle_ps(zt, zt, 0); \
    yt = _mm_xor_ps(yt, vec3_negate); \
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps(xt, vec4_null.data)), \
        _mm_and_ps(zt, _mm_cmpgt_ps(xt, vec4_null.data))); \
    wt = _mm_mul_ps(xt, wt); \
    (w) = *(vec3*)&wt; \
}

#define vec3_cross(x, y, z) \
{ \
    __m128 xt; \
    __m128 yt; \
    __m128 zt; \
    *(vec3*)&xt = (x); \
    *(vec3*)&yt = (y); \
    zt = _mm_sub_ps( \
        _mm_mul_ps(_mm_shuffle_ps(xt, xt, 0x09), _mm_shuffle_ps(yt, yt, 0x12)), \
        _mm_mul_ps(_mm_shuffle_ps(xt, xt, 0x12), _mm_shuffle_ps(yt, yt, 0x09)) \
    ); \
    (z) = *(vec3*)&zt; \
}

#define vec4_add(x, y, z) \
{ \
    (z).data = _mm_add_ps((x).data, (y).data); \
}

#define vec4_add_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    (z).data = _mm_add_ps((x).data, _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_sub(x, y, z) \
{ \
    (z).data = _mm_sub_ps((x).data, (y).data); \
}

#define vec4_sub_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    (z).data = _mm_sub_ps((x).data, _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_mult(x, y, z) \
{ \
    (z).data = _mm_mul_ps((x).data, (y).data); \
}

#define vec4_mult_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    (z).data = _mm_mul_ps((x).data, _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_div(x, y, z) \
{ \
    (z).data = _mm_div_ps((x).data, (y).data); \
}

#define vec4_div_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    (z).data = _mm_div_ps((x).data, _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_and(x, y, z) \
{ \
    (z).data = _mm_and_ps((x).data, (y).data); \
}

#define vec4_and_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    (z).data = _mm_and_ps((x).data, _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_xor(x, y, z) \
{ \
    (z).data = _mm_xor_ps((x).data, (y).data); \
}

#define vec4_xor_scalar(x, y, z) \
{ \
    __m128 yt; \
    yt = _mm_set_ss(y); \
    (z).data = _mm_xor_ps((x).data, _mm_shuffle_ps(yt, yt, 0)); \
}

#define vec4_negate(x, z) \
{ \
    (z).data = _mm_xor_ps((x).data, vec4_negate); \
}

#define vec4_dot(x, y, z) \
{ \
    __m128 zt; \
    zt = _mm_mul_ps((x).data, (y).data); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec4_length(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    xt = (x).data; \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec4_length_squared(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    xt = (x).data; \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec4_distance(x, y, z) \
{ \
    __m128 zt; \
    zt = _mm_sub_ps((x).data, (y).data); \
    zt = _mm_mul_ps(zt, zt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(zt, zt))); \
}

#define vec4_distance_squared(x, y, z) \
{ \
    __m128 zt; \
    zt = _mm_sub_ps((x).data, (y).data); \
    zt = _mm_mul_ps(zt, zt); \
    zt = _mm_hadd_ps(zt, zt); \
    (z) = _mm_cvtss_f32(_mm_hadd_ps(zt, zt)); \
}

#define vec4_lerp(x, y, z, blend) \
{ \
    (z).data = _mm_add_ps((x).data, _mm_mul_ps(_mm_sub_ps((y).data, (x).data), (blend).data)); \
}

#define vec4_lerp_scalar(x, y, z, blend) \
{ \
    __m128 b; \
    b = _mm_set_ss(blend); \
    b = _mm_shuffle_ps(b, b, 0); \
    (z).data = _mm_add_ps((x).data, _mm_mul_ps(_mm_sub_ps((y).data, (x).data), b)); \
}

#define vec4_normalize(x, z) \
{ \
    __m128 xt; \
    __m128 zt; \
    xt = (x).data; \
    zt = _mm_mul_ps(xt, xt); \
    zt = _mm_hadd_ps(zt, zt); \
    zt = _mm_sqrt_ss(_mm_hadd_ps(zt, zt)); \
    if (zt.m128_f32[0] != 0.0f) \
        zt.m128_f32[0] = 1.0f / zt.m128_f32[0]; \
    (z).data = _mm_mul_ps(xt, _mm_shuffle_ps(zt, zt, 0)); \
}

#define vec4_rcp(x, z) \
{ \
    (z).data = _mm_div_ps(vec4_identity.data, (x).data); \
}

#define vec4_min(x, y, z) \
{ \
    (z).data = _mm_min_ps((x).data, (y).data); \
}

#define vec4_max(x, y, z) \
{ \
    (z).data = _mm_max_ps((x).data, (y).data); \
}

#define vec4_clamp(x, y, z, w) \
{ \
    (w).data = _mm_min_ps(_mm_max_ps((x).data, (y).data), (z).data); \
}

#define vec4_clamp_scalar(x, y, z, w) \
{ \
    __m128 yt; \
    __m128 zt; \
    yt = _mm_set_ss(y); \
    zt = _mm_set_ss(z); \
    (w).data = _mm_min_ps(_mm_max_ps((x).data, _mm_shuffle_ps(yt, yt, 0)), _mm_shuffle_ps(zt, zt, 0)); \
}

#define vec4_mult_min_max(x, y, z, w) \
{ \
    __m128 yt; \
    __m128 zt; \
    __m128 wt; \
    yt = _mm_xor_ps((y).data, vec4_negate); \
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps((x).data, vec4_null.data)), \
        _mm_and_ps((z).data, _mm_cmpgt_ps((x).data, vec4_null.data))); \
    (w).data = _mm_mul_ps((x).data, wt); \
}

#define vec4_mult_min_max_scalar(x, y, z, w) \
{ \
    __m128 wt; \
    yt = _mm_set_ss(y); \
    yt = _mm_shuffle_ps(yt, yt, 0); \
    zt = _mm_set_ss(z); \
    zt = _mm_shuffle_ps(zt, zt, 0); \
    yt = _mm_xor_ps(yt, vec4_negate); \
    wt = _mm_or_ps(_mm_and_ps(yt, _mm_cmplt_ps((x).data, vec4_null.data)), \
        _mm_and_ps(zt, _mm_cmpgt_ps((x).data, vec4_null.data))); \
    (w).data = _mm_mul_ps((x).data, wt); \
}

#define vec2i_add(x, y, z) \
{ \
    __m128i xt; \
    __m128i yt; \
    __m128i zt; \
    *(vec2i*)&xt = (x); \
    *(vec2i*)&yt = (y); \
    zt = _mm_add_epi32(xt, yt); \
    (z) = *(vec2i*)&zt; \
}

#define vec2i_add_scalar(x, y, z) \
{ \
    __m128i xt; \
    __m128i yt; \
    __m128i zt; \
    *(vec2i*)&xt = (x); \
    yt = _mm_set1_epi32(y); \
    zt = _mm_add_epi32(xt, _mm_shuffle_epi32(yt, yt, 0)); \
    (z) = *(vec2i*)&zt; \
}

#define vec2i_sub(x, y, z) \
{ \
    __m128i xt; \
    __m128i yt; \
    __m128i zt; \
    *(vec2i*)&xt = (x); \
    *(vec2i*)&yt = (y); \
    zt = _mm_sub_epi32(xt, yt); \
    (z) = *(vec2i*)&zt; \
}

#define vec2i_sub_scalar(x, y, z) \
{ \
    __m128i xt; \
    __m128i yt; \
    __m128i zt; \
    *(vec2i*)&xt = (x); \
    yt = _mm_set1_epi32(y); \
    zt = _mm_sub_epi32(xt, _mm_shuffle_epi32(yt, yt, 0)); \
    (z) = *(vec2i*)&zt; \
}

#define vec2_to_vec2i(x, z) \
{ \
    __m128 xt; \
    __m128i zt; \
    *(vec2*)&xt = (x); \
    zt = _mm_cvtps_epi32(xt); \
    (z) = *(vec2i*)&zt; \
}

#define vec2i_to_vec2(x, z) \
{ \
    __m128i xt; \
    __m128 zt; \
    *(vec2i*)&xt = (x); \
    zt = _mm_cvtepi32_ps(xt); \
    (z) = *(vec2*)&zt; \
}

#define vec3i_add(x, y, z) \
{ \
    __m128i xt; \
    __m128i yt; \
    __m128i zt; \
    *(vec3i*)&xt = (x); \
    *(vec3i*)&yt = (y); \
    zt = _mm_add_epi32(xt, yt); \
    (z) = *(vec3i*)&zt; \
}

#define vec3i_add_scalar(x, y, z) \
{ \
    __m128i xt; \
    __m128i yt; \
    __m128i zt; \
    *(vec3i*)&xt = (x); \
    yt = _mm_set1_epi32(y); \
    zt = _mm_add_epi32(xt, _mm_shuffle_epi32(yt, yt, 0)); \
    (z) = *(vec3i*)&zt; \
}

#define vec3i_sub(x, y, z) \
{ \
    __m128i xt; \
    __m128i yt; \
    __m128i zt; \
    *(vec3i*)&xt = (x); \
    *(vec3i*)&yt = (y); \
    zt = _mm_sub_epi32(xt, yt); \
    (z) = *(vec3i*)&zt; \
}

#define vec3i_sub_scalar(x, y, z) \
{ \
    __m128i xt; \
    __m128i yt; \
    __m128i zt; \
    *(vec3i*)&xt = (x); \
    yt = _mm_set1_epi32(y); \
    zt = _mm_sub_epi32(xt, _mm_shuffle_epi32(yt, yt, 0)); \
    (z) = *(vec3i*)&zt; \
}

#define vec3_to_vec3i(x, z) \
{ \
    __m128 xt; \
    __m128i zt; \
    *(vec3*)&xt = (x); \
    zt = _mm_cvtps_epi32(xt); \
    (z) = *(vec3i*)&zt; \
}

#define vec3i_to_vec3(x, z) \
{ \
    __m128i xt; \
    __m128 zt; \
    *(vec3i*)&xt = (x); \
    zt = _mm_cvtepi32_ps(xt); \
    (z) = *(vec3*)&zt; \
}

#define vec4i_add(x, y, z) \
{ \
    (z).data = _mm_add_epi32((x).data, (y).data); \
}

#define vec4i_add_scalar(x, y, z) \
{ \
    __m128i yt; \
    yt = _mm_set1_epi32(y); \
    (z).data = _mm_add_epi32((x).data, _mm_shuffle_epi32(yt, yt, 0)); \
}

#define vec4i_sub(x, y, z) \
{ \
    (z).data = _mm_sub_epi32((x).data, (y).data); \
}

#define vec4i_sub_scalar(x, y, z) \
{ \
    __m128i yt; \
    yt = _mm_set1_epi32(y); \
    (z).data = _mm_sub_epi32((x).data, _mm_shuffle_epi32(yt, yt, 0)); \
}

#define vec4_to_vec4i(x, z) \
{ \
    (z).data = _mm_cvtps_epi32((x).data); \
}

#define vec4i_to_vec4(x, z) \
{ \
    (z).data = _mm_cvtepi32_ps((x).data); \
}

#define vec2d_add(x, y, z) \
{ \
    (z).data = _mm_add_pd((x).data, (y).data); \
}

#define vec2d_add_scalar(x, y, z) \
{ \
    __m128d yt; \
    yt = _mm_set_sd(y); \
    (z).data = _mm_add_pd((x).data, _mm_shuffle_pd(yt, yt, 0)); \
}

#define vec2d_sub(x, y, z) \
{ \
    (z).data = _mm_sub_pd((x).data, (y).data); \
}

#define vec2d_sub_scalar(x, y, z) \
{ \
    __m128d yt; \
    yt = _mm_set_sd(y); \
    (z).data = _mm_sub_pd((x).data, _mm_shuffle_pd(yt, yt, 0)); \
}

#define vec2d_mult(x, y, z) \
{ \
    (z).data = _mm_mul_pd((x).data, (y).data); \
}

#define vec2d_mult_scalar(x, y, z) \
{ \
    __m128d yt; \
    yt = _mm_set_sd(y); \
    (z).data = _mm_mul_pd((x).data, _mm_shuffle_pd(yt, yt, 0)); \
}

#define vec2d_div(x, y, z) \
{ \
    (z).data = _mm_div_pd((x).data, (y).data); \
}

#define vec2d_div_scalar(x, y, z) \
{ \
    __m128d yt; \
    yt = _mm_set_sd(y); \
    (z).data = _mm_div_pd((x).data, _mm_shuffle_pd(yt, yt, 0)); \
}

#define vec2d_and(x, y, z) \
{ \
    (z).data = _mm_and_pd((x).data, (y).data); \
}

#define vec2d_and_scalar(x, y, z) \
{ \
    __m128d yt; \
    yt = _mm_set_sd(y); \
    (z).data = _mm_and_pd((x).data, _mm_shuffle_pd(yt, yt, 0)); \
}

#define vec2d_xor(x, y, z) \
{ \
    (z).data = _mm_xor_pd((x).data, (y).data); \
}

#define vec2d_xor_scalar(x, y, z) \
{ \
    __m128d yt; \
    yt = _mm_set_sd(y); \
    (z).data = _mm_xor_pd((x).data, _mm_shuffle_pd(yt, yt, 0)); \
}

#define vec2d_negate(x, z) \
{ \
    (z).data = _mm_xor_pd((x).data, vec2d_negate); \
}

#define vec2d_dot(x, y, z) \
{ \
    __m128d zt; \
    zt = _mm_mul_pd((x).data, (y).data); \
    (z) = _mm_cvtsd_f64(_mm_hadd_pd(zt, zt)); \
}

#define vec2d_length(x, z) \
{ \
    __m128d zt; \
    zt = _mm_mul_pd((x).data, (x).data); \
    (z) = _mm_cvtsd_f64(_mm_sqrt_sd(_mm_hadd_pd(zt, zt))); \
}

#define vec2d_length_squared(x, z) \
{ \
    __m128d zt; \
    zt = _mm_mul_pd((x).data, (x).data); \
    (z) = _mm_cvtsd_f64(_mm_hadd_pd(zt, zt)); \
}

#define vec2d_distance(x, y, z) \
{ \
    __m128d zt; \
    zt = _mm_sub_pd((x).data, (y).data); \
    zt = _mm_mul_pd(zt, zt); \
    (z) = _mm_cvtsd_f64(_mm_sqrt_sd(_mm_hadd_pd(zt, zt))); \
}

#define vec2d_distance_squared(x, y, z) \
{ \
    __m128d zt; \
    zt = _mm_sub_pd((x).data, (y).data); \
    zt = _mm_mul_pd(zt, zt); \
    (z) = _mm_cvtsd_f64(_mm_hadd_pd(zt, zt)); \
}

#define vec2d_lerp(x, y, z, blend) \
{ \
    (z).data = _mm_add_pd((x).data, _mm_mul_pd(_mm_sub_pd((y).data, (z).data), (blend).data)); \
}

#define vec2d_lerp_scalar(x, y, z, blend) \
{ \
    __m128d b; \
    b = _mm_set_sd(blend); \
    b = _mm_shuffle_pd(b, b, 0); \
    (z).data = _mm_add_pd((x).data, _mm_mul_pd(_mm_sub_pd((y).data, (z).data), b)); \
}

#define vec2d_normalize(x, z) \
{ \
    __m128d zt; \
    zt = _mm_mul_pd((x).data, (x).data); \
    zt = _mm_sqrt_sd(_mm_hadd_pd(zt, zt)); \
    if (zt.m128d_f64[0] != 0.0) \
        zt.m128d_f64[0] = 1.0 / zt.m128d_f64[0]; \
    (z).data = _mm_mul_pd((x).data, _mm_shuffle_pd(zt, zt, 0)); \
}

#define vec2d_rcp(x, z) \
{ \
    (z).data = _mm_div_pd(vec2d_identity.data, (x).data); \
}

#define vec2d_min(x, y, z) \
{ \
    (z).data = _mm_min_pd((x).data, (y).data); \
}

#define vec2d_max(x, y, z) \
{ \
    (z).data = _mm_max_pd((x).data, (y).data); \
}

#define vec2d_clamp(x, y, z, w) \
{ \
    (w).data = _mm_min_pd(_mm_max_pd((x).data, (y).data), (z).data); \
}

#define vec2d_clamp_scalar(x, y, z, w) \
{ \
    __m128d yt; \
    __m128d zt; \
    __m128d wt; \
    yt = _mm_set_sd(y); \
    zt = _mm_set_sd(z); \
    wt = _mm_min_pd(_mm_max_pd((x).data, _mm_shuffle_pd(yt, yt, 0)), _mm_shuffle_pd(zt, zt, 0)); \
    (w) = *(vec2d*)&wt; \
}

#define vec2d_mult_min_max(x, y, z, w) \
{ \
    __m128d yt; \
    __m128d zt; \
    __m128d wt; \
    yt = (y).data; \
    zt = (z).data; \
    yt = _mm_xor_pd(yt, vec2d_negate); \
    wt = _mm_or_pd(_mm_and_pd(yt, _mm_cmplt_pd((x).data, vec2d_null.data)), \
        _mm_and_pd(zt, _mm_cmpgt_pd((x).data, vec2d_null.data))); \
    (w).data = _mm_mul_pd((x).data, wt); \
}

#define vec2d_mult_min_max_scalar(x, y, z, w) \
{ \
    __m128d yt; \
    __m128d zt; \
    __m128d wt; \
    yt = _mm_set_sd(y); \
    yt = _mm_shuffle_pd(yt, yt, 0); \
    zt = _mm_set_sd(z); \
    zt = _mm_shuffle_pd(zt, zt, 0); \
    yt = _mm_xor_pd(yt, vec2d_negate); \
    wt = _mm_or_pd(_mm_and_pd(yt, _mm_cmplt_pd((x).data, vec2d_null.data)), \
        _mm_and_pd(zt, _mm_cmpgt_pd((x).data, vec2d_null.data))); \
    (w).data = _mm_mul_pd((x).data, wt); \
}
