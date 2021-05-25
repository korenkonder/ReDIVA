/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern int32_t FASTCALL glitter_x_random_get_value(glitter_random* random);
extern float_t FASTCALL glitter_x_random_get_float(glitter_random* random, float_t value);
extern float_t FASTCALL glitter_x_random_get_float_min_max(glitter_random* random, float_t min, float_t max);
extern void FASTCALL glitter_x_random_get_vec3(glitter_random* random, vec3* src, vec3* dst);
extern int32_t FASTCALL glitter_x_random_get_int(glitter_random* random, int32_t value);
extern int32_t FASTCALL glitter_x_random_get_int_min_max(glitter_random* random, int32_t min, int32_t max);
extern int32_t FASTCALL glitter_x_random_get_max();
extern void FASTCALL glitter_x_random_reset(glitter_random* random);
extern void FASTCALL glitter_x_random_set_step(glitter_random* random, uint8_t step);
extern void FASTCALL glitter_x_random_set_value(glitter_random* random, int32_t value);
extern void FASTCALL glitter_x_random_step_value(glitter_random* random);
