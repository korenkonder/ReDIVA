/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern int32_t glitter_random_get_value(glitter_random* random);
extern float_t glitter_random_get_float(GLT,
    glitter_random* random, float_t value);
extern float_t glitter_random_get_float_min_max(GLT,
    glitter_random* random, float_t min, float_t max);
extern void glitter_random_get_vec3(GLT,
    glitter_random* random, vec3* src, vec3* dst);
extern int32_t glitter_random_get_int(GLT,
    glitter_random* random, int32_t value);
extern int32_t glitter_random_get_int_min_max(GLT,
    glitter_random* random, int32_t min, int32_t max);
extern int32_t glitter_random_get_max(GLT);
extern void glitter_random_set_value(glitter_random* random, int32_t value);
