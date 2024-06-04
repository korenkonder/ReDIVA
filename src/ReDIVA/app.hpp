/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/vec.hpp"

enum render_scale {
    RENDER_SCALE_25 = 0,
    RENDER_SCALE_33,
    RENDER_SCALE_50,
    RENDER_SCALE_67,
    RENDER_SCALE_75,
    RENDER_SCALE_83,
    RENDER_SCALE_100,
    RENDER_SCALE_117,
    RENDER_SCALE_125,
    RENDER_SCALE_133,
    RENDER_SCALE_150,
    RENDER_SCALE_167,
    RENDER_SCALE_175,
    RENDER_SCALE_183,
    RENDER_SCALE_200,
    RENDER_SCALE_MAX,
};

struct app_init_struct {
    vec2i res;
    int32_t scale_index;

    inline app_init_struct() : scale_index() {

    }

    inline app_init_struct(vec2i res, int32_t scale_index) {
        this->res = res;
        this->scale_index = scale_index;
    }
};

extern const double_t render_scale_table[];

extern int32_t app_main(const app_init_struct& ris);
extern double_t app_get_render_scale();
extern int32_t app_get_render_scale_index();
extern void app_set_render_scale_index(int32_t index);
