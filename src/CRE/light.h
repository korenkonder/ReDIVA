/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"

typedef struct light_dir {
    vec3 dir;
    vec3 color;
} light_dir;

typedef struct light_point {
    vec3 position;
    vec3 color;
    union {
        struct {
            float_t constant;
            float_t linear;
            float_t quadratic;
            float_t radius;
        };
        vec4 clqr;
    };
} light_point;

typedef struct light_dir_update {
    vec3 dir;
    vec4 color;
} light_dir_update;

typedef struct light_point_update {
    vec3 position;
    vec4 color;
    float_t constant;
    float_t linear;
    float_t quadratic;
} light_point_update;

extern float_t light_point_calculate_radius(vec3* color,
    float_t constant, float_t linear, float_t quadratic);
