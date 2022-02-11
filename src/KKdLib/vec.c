/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vec.h"

const __m128 vec2_negate = { -0.0f, -0.0f,  0.0f,  0.0f };
const __m128 vec3_negate = { -0.0f, -0.0f, -0.0f,  0.0f };
const __m128 vec4_negate = { -0.0f, -0.0f, -0.0f, -0.0f };

const vec4i vec4_mask_vec2 = { (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0x00000000, (int32_t)0x00000000 };
const vec4i vec4_mask_vec3 = { (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0x00000000 };

const __m128d vec2d_negate = { -0.0, -0.0 };

const vec2 vec2_identity = { 1.0f, 1.0f };
const vec3 vec3_identity = { 1.0f, 1.0f, 1.0f };
const vec4 vec4_identity = { 1.0f, 1.0f, 1.0f, 1.0f };

const vec2 vec2_null = { 0.0f, 0.0f };
const vec3 vec3_null = { 0.0f, 0.0f, 0.0f };
const vec4 vec4_null = { 0.0f, 0.0f, 0.0f, 0.0f };

const vec2i vec2i_null = { 0, 0 };
const vec3i vec3i_null = { 0, 0, 0 };
const vec4i vec4i_null = { 0, 0, 0, 0 };

const vec2d vec2d_identity = { 1.0f, 1.0f };

const vec2d vec2d_null = { 0.0f, 0.0f };

const vec4u vec4u_identity = { 1.0f, 1.0f, 1.0f, 1.0f };

const vec4u vec4u_null = { 0.0f, 0.0f, 0.0f, 0.0f };

const vec4iu vec4iu_null = { 0, 0, 0, 0 };

vector_func(vec2)
vector_func(vec2i)
vector_func(vec3)
vector_func(vec3i)
vector_func(vec4)
vector_func(vec4i)
vector_ptr_func(vec2)
vector_ptr_func(vec2i)
vector_ptr_func(vec3)
vector_ptr_func(vec3i)
vector_ptr_func(vec4)
vector_ptr_func(vec4i)

// Crutch for vector_func definition
