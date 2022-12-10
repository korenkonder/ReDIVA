/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vec.hpp"

const __m128 vec2_neg = { -0.0f, -0.0f,  0.0f,  0.0f };
const __m128 vec3_neg = { -0.0f, -0.0f, -0.0f,  0.0f };
const __m128 vec4_neg = { -0.0f, -0.0f, -0.0f, -0.0f };

const vec4i vec4_mask_vec2 = { (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0x00000000, (int32_t)0x00000000 };
const vec4i vec4_mask_vec3 = { (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0x00000000 };
