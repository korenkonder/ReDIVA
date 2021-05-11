/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"

typedef struct render_init_struct {
    vec2i res;
    vec2i internal_res;
    float_t scale;
} render_init_struct;

extern int32_t render_main(void* arg);
extern void render_set_scale(double_t value);
