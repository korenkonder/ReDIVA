/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern int32_t FASTCALL glitter_random_get();
extern float_t FASTCALL glitter_random_get_float_clamp(float_t min, float_t max);
extern float_t FASTCALL glitter_random_get_float_clamp_min_max(float_t value);
extern void FASTCALL glitter_random_get_float_vec3_clamp(vec3* src, vec3* dst);
extern int32_t FASTCALL glitter_random_get_int(int32_t value);
extern int32_t FASTCALL glitter_random_get_int_clamp(int32_t min, int32_t max);
extern int32_t FASTCALL glitter_random_get_max();
extern void FASTCALL glitter_random_set(int32_t value);
