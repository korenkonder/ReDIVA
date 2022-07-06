/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "interpolation.hpp"

inline float_t interpolate_linear_value(float_t p1, float_t p2, float_t f1, float_t f2, float_t f) {
    float_t t = (f - f1) / (f2 - f1);
    return (1.0f - t) * p1 + t * p2;
}

inline void interpolate_linear_value_vec2(vec2* p1, vec2* p2, vec2* f1, vec2* f2, vec2* f, vec2* value) {
    __m128 _p1;
    __m128 _p2;
    __m128 _f1;
    __m128 _f2;
    __m128 _f;
    *(vec2*)&_p1 = *p1;
    *(vec2*)&_p2 = *p2;
    *(vec2*)&_f1 = *f1;
    *(vec2*)&_f2 = *f2;
    *(vec2*)&_f = *f;

    const __m128 _1 = _mm_set_ps1(1.0f);

    __m128 df = _mm_sub_ps(_f2, _f1);
    __m128 t = _mm_div_ps(_mm_sub_ps(_f, _f1), df);
    __m128 t1 = _mm_sub_ps(_1, t);
    __m128 val = _mm_add_ps(_mm_mul_ps(_p1, t1), _mm_mul_ps(_p2, t));
    *value = *(vec2*)&val;
}

inline void interpolate_linear_value_vec3(vec3* p1, vec3* p2, vec3* f1, vec3* f2, vec3* f, vec3* value) {
    __m128 _p1;
    __m128 _p2;
    __m128 _f1;
    __m128 _f2;
    __m128 _f;
    *(vec3*)&_p1 = *p1;
    *(vec3*)&_p2 = *p2;
    *(vec3*)&_f1 = *f1;
    *(vec3*)&_f2 = *f2;
    *(vec3*)&_f = *f;

    const __m128 _1 = _mm_set_ps1(1.0f);

    __m128 df = _mm_sub_ps(_f2, _f1);
    __m128 t = _mm_div_ps(_mm_sub_ps(_f, _f1), df);
    __m128 t1 = _mm_sub_ps(_1, t);
    __m128 val = _mm_add_ps(_mm_mul_ps(_p1, t1), _mm_mul_ps(_p2, t));
    *value = *(vec3*)&val;
}

inline void interpolate_linear_value_vec4(vec4* p1, vec4* p2, vec4* f1, vec4* f2, vec4* f, vec4* value) {
    __m128 _p1;
    __m128 _p2;
    __m128 _f1;
    __m128 _f2;
    __m128 _f;
    _p1 = _mm_loadu_ps((float*)p1);
    _p2 = _mm_loadu_ps((float*)p2);
    _f1 = _mm_loadu_ps((float*)f1);
    _f2 = _mm_loadu_ps((float*)f2);
    _f = _mm_loadu_ps((float*)f);

    const __m128 _1 = _mm_set_ps1(1.0f);

    __m128 df = _mm_sub_ps(_f2, _f1);
    __m128 t = _mm_div_ps(_mm_sub_ps(_f, _f1), df);
    __m128 t1 = _mm_sub_ps(_1, t);
    _mm_storeu_ps((float*)value, _mm_add_ps(_mm_mul_ps(_p1, t1), _mm_mul_ps(_p2, t)));
}

inline void interpolate_linear(float_t p1, float_t p2,
    size_t f1, size_t f2, float_t** arr, size_t* length) {
    *length = f2 - f1 + 1;
    *arr = force_malloc_s(float_t, *length);

    float_t* a = *arr;
    float_t l = (float_t)*length;
    for (size_t i = 0; i < l; i++, a++)
        *a = interpolate_linear_value(p1, p2,
            (float_t)f1, (float_t)f2, (float_t)(f1 + i));
}

inline float_t interpolate_chs_value(float_t p1, float_t p2,
    float_t t1, float_t t2, float_t f1, float_t f2, float_t f) {
    float_t df = f - f1;
    float_t t = df / (f2 - f1);
    float_t t_1 = t - 1.0f;
    return p1 + t * t * (3.0f - 2.0f * t) * (p2 - p1) + (t_1 * t1 + t * t2) * df * t_1;
}

inline void interpolate_chs_value_vec2(vec2* p1, vec2* p2,
    vec2* t1, vec2* t2, vec2* f1, vec2* f2, vec2* f, vec2* value) {
    __m128 _p1;
    __m128 _p2;
    __m128 _t1;
    __m128 _t2;
    __m128 _f1;
    __m128 _f2;
    __m128 _f;
    *(vec2*)&_p1 = *p1;
    *(vec2*)&_p2 = *p2;
    *(vec2*)&_t1 = *t1;
    *(vec2*)&_t2 = *t2;
    *(vec2*)&_f1 = *f1;
    *(vec2*)&_f2 = *f2;
    *(vec2*)&_f = *f;

    const __m128 _1 = _mm_set_ps1(1.0f);
    const __m128 _2 = _mm_set_ps1(2.0f);
    const __m128 _3 = _mm_set_ps1(3.0f);

    __m128 df = _mm_sub_ps(_f, _f1);
    __m128 t = _mm_div_ps(df, _mm_sub_ps(_f2, _f1));
    __m128 t_1 = _mm_sub_ps(t, _1);

    __m128 delta = _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(t, t), _mm_sub_ps(_3, _mm_mul_ps(_2, t))), _mm_sub_ps(_p2, _p1));
    __m128 tangent = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(t_1, _t1), _mm_mul_ps(t, _t2)), _mm_mul_ps(df, t_1));
    __m128 val = _mm_add_ps(_mm_add_ps(_p1, delta), tangent);
    *value = *(vec2*)&val;
}

inline void interpolate_chs_value_vec3(vec3* p1, vec3* p2, vec3* t1,
    vec3* t2, vec3* f1, vec3* f2, vec3* f, vec3* value) {
    __m128 _p1;
    __m128 _p2;
    __m128 _t1;
    __m128 _t2;
    __m128 _f1;
    __m128 _f2;
    __m128 _f;
    *(vec3*)&_p1 = *p1;
    *(vec3*)&_p2 = *p2;
    *(vec3*)&_t1 = *t1;
    *(vec3*)&_t2 = *t2;
    *(vec3*)&_f1 = *f1;
    *(vec3*)&_f2 = *f2;
    *(vec3*)&_f = *f;

    const __m128 _1 = _mm_set_ps1(1.0f);
    const __m128 _2 = _mm_set_ps1(2.0f);
    const __m128 _3 = _mm_set_ps1(3.0f);

    __m128 df = _mm_sub_ps(_f, _f1);
    __m128 t = _mm_div_ps(df, _mm_sub_ps(_f2, _f1));
    __m128 t_1 = _mm_sub_ps(t, _1);

    __m128 delta = _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(t, t), _mm_sub_ps(_3, _mm_mul_ps(_2, t))), _mm_sub_ps(_p2, _p1));
    __m128 tangent = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(t_1, _t1), _mm_mul_ps(t, _t2)), _mm_mul_ps(df, t_1));
    __m128 val = _mm_add_ps(_mm_add_ps(_p1, delta), tangent);
    *value = *(vec3*)&val;
}

inline void interpolate_chs_value_vec4(vec4* p1, vec4* p2,
    vec4* t1, vec4* t2, vec4* f1, vec4* f2, vec4* f, vec4* value) {
    __m128 _p1;
    __m128 _p2;
    __m128 _t1;
    __m128 _t2;
    __m128 _f1;
    __m128 _f2;
    __m128 _f;
    _p1 = _mm_loadu_ps((float*)p1);
    _p2 = _mm_loadu_ps((float*)p2);
    _t1 = _mm_loadu_ps((float*)t1);
    _t2 = _mm_loadu_ps((float*)t2);
    _f1 = _mm_loadu_ps((float*)f1);
    _f2 = _mm_loadu_ps((float*)f2);
    _f = _mm_loadu_ps((float*)f);

    const __m128 _1 = _mm_set_ps1(1.0f);
    const __m128 _2 = _mm_set_ps1(2.0f);
    const __m128 _3 = _mm_set_ps1(3.0f);

    __m128 df = _mm_sub_ps(_f, _f1);
    __m128 t = _mm_div_ps(df, _mm_sub_ps(_f2, _f1));
    __m128 t_1 = _mm_sub_ps(t, _1);

    __m128 delta = _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(t, t), _mm_sub_ps(_3, _mm_mul_ps(_2, t))), _mm_sub_ps(_p2, _p1));
    __m128 tangent = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(t_1, _t1), _mm_mul_ps(t, _t2)), _mm_mul_ps(df, t_1));
    _mm_storeu_ps((float*)value, _mm_add_ps(_mm_add_ps(_p1, delta), tangent));
}

inline void interpolate_chs(float_t p1, float_t p2,
    float_t t1, float_t t2, size_t f1, size_t f2, float_t** arr, size_t* length) {
    *length = f2 - f1 + 1;
    *arr = force_malloc_s(float_t, *length);

    float_t* a = *arr;
    float_t l = (float_t)*length;
    for (size_t i = 0; i < l; i++, a++)
        *a = interpolate_chs_value(p1, p2, t1, t2, (float_t)f1, (float_t)f2, (float_t)(f1 + i));
}

void interpolate_chs_reverse_value(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2, size_t f) {
    *t1 = *t2 = 0.0f;

    if (!arr || length < 2 || f - f1 + 1 >= length || f < 1 || f < f1 || f + 2 > f2)
        return;

    float_t df_1 = (float_t)(f - f1);
    float_t df_2 = (float_t)(f - f1 + 1);
    float_t _t1 = df_1 / (float_t)(f2 - f1);
    float_t _t2 = df_2 / (float_t)(f2 - f1);
    float_t t1_1 = _t1 - 1.0f;
    float_t t2_1 = _t2 - 1.0f;

    float_t t1_t2_1 = arr[f] - arr[f1] - _t1 * _t1 * (3.0f - 2.0f * _t1) * (arr[f2] - arr[f1]);
    float_t t1_t2_2 = arr[f + 1] - arr[f1] - _t2 * _t2 * (3.0f - 2.0f * _t2) * (arr[f2] - arr[f1]);
    t1_t2_1 /= df_1 * t1_1;
    t1_t2_2 /= df_2 * t2_1;

    *t1 = (t1_t2_1 * _t2 - t1_t2_2 * _t1) / (_t1 - _t2);
    *t2 = (-t1_t2_1 * t2_1 + t1_t2_2 * t1_1) / (_t1 - _t2);
}

void interpolate_chs_reverse(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2) {
    *t1 = *t2 = 0.0f;
    if (f2 - f1 - 2 < 1)
        return;

    float_t _t1 = 0.0f;
    float_t _t2 = 0.0f;
    double_t tt1 = 0.0;
    double_t tt2 = 0.0;
    for (size_t i = f1 + 1; i < f2 - 1; i++) {
        interpolate_chs_reverse_value(arr, length, &_t1, &_t2, f1, f2, i);
        tt1 += _t1;
        tt2 += _t2;
    }
    *t1 = (float_t)(tt1 / (double_t)(f2 - f1 - 2));
    *t2 = (float_t)(tt2 / (double_t)(f2 - f1 - 2));
}

void interpolate_chs_reverse_step_value(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2, size_t f, uint8_t step) {
    *t1 = *t2 = 0.0f;

    if (!arr || length < 2 || f - f1 + 1 >= length || f < 1 || f < f1 || f + 2 > f2)
        return;

    float_t df_1 = (float_t)((f - f1) * step);
    float_t df_2 = (float_t)((f - f1 + 1) * step);
    float_t _t1 = df_1 / (float_t)((f2 - f1) * step);
    float_t _t2 = df_2 / (float_t)((f2 - f1) * step);
    float_t t1_1 = _t1 - 1.0f;
    float_t t2_1 = _t2 - 1.0f;

    float_t t1_t2_1 = arr[f] - arr[f1] - _t1 * _t1 * (3.0f - 2.0f * _t1) * (arr[f2] - arr[f1]);
    float_t t1_t2_2 = arr[f + 1] - arr[f1] - _t2 * _t2 * (3.0f - 2.0f * _t2) * (arr[f2] - arr[f1]);
    t1_t2_1 /= df_1 * t1_1;
    t1_t2_2 /= df_2 * t2_1;

    *t1 = (t1_t2_1 * _t2 - t1_t2_2 * _t1) / (_t1 - _t2);
    *t2 = (-t1_t2_1 * t2_1 + t1_t2_2 * t1_1) / (_t1 - _t2);
}

void interpolate_chs_reverse_step(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2, uint8_t step) {
    *t1 = *t2 = 0.0f;
    if (f2 - f1 - 2 < 1)
        return;

    float_t _t1 = 0.0f;
    float_t _t2 = 0.0f;
    double_t tt1 = 0.0;
    double_t tt2 = 0.0;
    for (size_t i = f1 + 1; i < f2 - 1; i++) {
        interpolate_chs_reverse_step_value(arr, length, &_t1, &_t2, f1, f2, i, step);
        tt1 += _t1;
        tt2 += _t2;
    }
    *t1 = (float_t)(tt1 / (double_t)(f2 - f1 - 2));
    *t2 = (float_t)(tt2 / (double_t)(f2 - f1 - 2));
}
