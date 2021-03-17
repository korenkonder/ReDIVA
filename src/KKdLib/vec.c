/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vec.h"

const __m128 vec2_negate = { .m128_u32 = { 0x80000000, 0x80000000, 0x00000000, 0x00000000 } };
const __m128 vec3_negate = { .m128_u32 = { 0x80000000, 0x80000000, 0x80000000, 0x00000000 } };
const __m128 vec4_negate = { .m128_u32 = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 } };
