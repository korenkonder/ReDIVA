/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/spr.hpp"
#include "../KKdLib/vec.hpp"

struct ScreenParam {
    SCREEN_MODE mode;
    int32_t xoffset;
    int32_t yoffset;
    int32_t width;
    int32_t height;
    double_t aspect;
    int32_t wide_flag;

    inline ScreenParam() : ScreenParam(SCREEN_MODE_MAX) {

    }

    ScreenParam(SCREEN_MODE screen_mode);
};

struct screen_mode_scale_data {
    vec2 scale;
    vec2 src_res;
    vec2 dst_res;
    SCREEN_MODE src_mode;
    SCREEN_MODE dst_mode;

    screen_mode_scale_data(SCREEN_MODE src_mode, SCREEN_MODE dst_mode);
};

extern ScreenParam& get_screen_param();
extern ScreenParam& get_render_screen_param();
extern vec2 get_screen_conv_scale(const SCREEN_MODE dst_mode, const SCREEN_MODE src_mode);
extern void get_screen_conv_pos(vec2& dst_pos, const SCREEN_MODE dst_mode,
    const vec2& src_pos, const SCREEN_MODE src_mode);
extern void init_screen(SCREEN_MODE screen_mode);
