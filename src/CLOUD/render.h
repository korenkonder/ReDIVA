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
    float_t scale;
} render_init_struct;

extern const double_t render_scale_table[];

extern int32_t render_main(void* arg);
extern double_t render_get_scale();
extern void render_set_scale(double_t value);
extern int32_t render_get_scale_index();
extern void render_set_scale_index(int32_t index);
