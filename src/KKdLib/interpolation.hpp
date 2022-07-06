/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "vec.hpp"

extern float_t interpolate_linear_value(float_t p1, float_t p2, float_t f1, float_t f2, float_t f);
extern void interpolate_linear_value_vec2(vec2* p1, vec2* p2, vec2* f1, vec2* f2, vec2* f, vec2* value);
extern void interpolate_linear_value_vec3(vec3* p1, vec3* p2, vec3* f1, vec3* f2, vec3* f, vec3* value);
extern void interpolate_linear_value_vec4(vec4* p1, vec4* p2, vec4* f1, vec4* f2, vec4* f, vec4* value);
extern void interpolate_linear(float_t p1, float_t p2, size_t f1, size_t f2, float_t** arr, size_t* length);
extern float_t interpolate_chs_value(float_t p1, float_t p2,
    float_t t1, float_t t2, float_t f1, float_t f2, float_t f);
extern void interpolate_chs_value_vec2(vec2* p1, vec2* p2,
    vec2* t1, vec2* t2, vec2* f1, vec2* f2, vec2* f, vec2* value);
extern void interpolate_chs_value_vec3(vec3* p1, vec3* p2,
    vec3* t1, vec3* t2, vec3* f1, vec3* f2, vec3* f, vec3* value);
extern void interpolate_chs_value_vec4(vec4* p1, vec4* p2,
    vec4* t1, vec4* t2, vec4* f1, vec4* f2, vec4* f, vec4* value);
extern void interpolate_chs(float_t p1, float_t p2,
    float_t t1, float_t t2, size_t f1, size_t f2, float_t** arr, size_t* length);
extern void interpolate_chs_reverse_value(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2, size_t f);
extern void interpolate_chs_reverse(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2);
extern void interpolate_chs_reverse_step_value(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2, size_t f, uint8_t step);
extern void interpolate_chs_reverse_step(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2, uint8_t step);
