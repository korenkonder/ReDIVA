/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vec.h"

const __m128 vec2_negate = { .m128_u32 = { 0x80000000, 0x80000000, 0x00000000, 0x00000000 } };
const __m128 vec3_negate = { .m128_u32 = { 0x80000000, 0x80000000, 0x80000000, 0x00000000 } };
const __m128 vec4_negate = { .m128_u32 = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 } };

const vec2 vec2_identity = { 1.0f, 1.0f };
const vec3 vec3_identity = { 1.0f, 1.0f, 1.0f };
const vec4 vec4_identity = { 1.0f, 1.0f, 1.0f, 1.0f };

const vec2 vec2_null = { 0.0f, 0.0f };
const vec3 vec3_null = { 0.0f, 0.0f, 0.0f };
const vec4 vec4_null = { 0.0f, 0.0f, 0.0f, 0.0f };
