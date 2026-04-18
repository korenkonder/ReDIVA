/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "screen_param.hpp"
#include "color.hpp"

struct ScreenSize {
    int32_t width;
    int32_t height;
    int32_t wide_flag;
    int32_t render_xoffset;
    int32_t render_yoffset;
    int32_t render_width;
    int32_t render_height;
};

struct ScreenInfo {
    ScreenParam screen;
    ScreenParam render;
    color4u8 back_color;

    ScreenInfo();
};

static const ScreenSize screen_size[] = {
    {  320,  240, 0, 0,   0,  320,  225 }, // QVGA
    {  640,  480, 0, 0,  60,  640,  360 }, // VGA
    {  800,  600, 0, 0,  75,  800,  450 }, // SVGA
    { 1024,  768, 0, 0,  96, 1024,  576 }, // XGA
    { 1280, 1024, 1, 0, 152, 1280,  720 }, // SXGA
    { 1400, 1050, 0, 0, 131, 1400,  788 }, // SXGA+
    { 1600, 1200, 0, 0, 150, 1600,  900 }, // UXGA
    {  800,  480, 2, 0,  15,  800,  450 }, // WVGA
    { 1024,  600, 2, 0,  12, 1024,  576 }, // WSVGA
    { 1280,  768, 2, 0,  24, 1280,  720 }, // WXGA
    { 1360,  768, 3, 0,   2, 1360,  765 }, // FWXGA
    { 1920, 1200, 2, 0,  60, 1920, 1200 }, // WUXGA
    { 2560, 1536, 2, 0,  48, 2560, 1440 }, // WQXGA
    { 1280,  720, 3, 0,   0, 1280,  720 }, // HD
    { 1920, 1080, 3, 0,   0, 1920, 1080 }, // FHD
    { 2560, 1440, 3, 0,   0, 2560, 1440 }, // QHD
    {  480,  272, 3, 0,   0,  480,  272 }, // WQVGA
    {  960,  544, 3, 0,   0,  960,  544 }, // qHD

    // MM+
    /*{  320,  240, 0, 0,   0,  320,  225 }, // QVGA
    {  640,  480, 0, 0,  60,  640,  360 }, // VGA
    {  800,  600, 0, 0,  75,  800,  450 }, // SVGA
    { 1024,  768, 0, 0,  96, 1024,  576 }, // XGA
    { 1280, 1024, 1, 0, 152, 1280,  720 }, // SXGA
    { 1400, 1050, 0, 0, 131, 1400,  788 }, // SXGA+
    { 1600, 1200, 0, 0, 150, 1600,  900 }, // UXGA
    {  800,  480, 2, 0,  15,  800,  450 }, // WVGA
    { 1024,  600, 2, 0,  12, 1024,  576 }, // WSVGA
    { 1280,  768, 2, 0,  24, 1280,  720 }, // WXGA
    { 1360,  768, 3, 0,   2, 1360,  765 }, // FWXGA
    { 1920, 1200, 2, 0,  60, 1920, 1200 }, // WUXGA
    { 2560, 1536, 2, 0,  48, 2560, 1440 }, // WQXGA
    { 1280,  720, 3, 0,   0, 1280,  720 }, // HD
    { 1920, 1080, 3, 0,   0, 1920, 1080 }, // FHD
    { 3840, 2160, 3, 0,   0, 3840, 2160 }, // UHD
    { 3840, 2160, 3, 0,   0, 2880, 1620 }, // 3K at UHD
    { 2880, 1620, 3, 0,   0, 2880, 1620 }, // 3K
    { 2560, 1440, 3, 0,   0, 2560, 1440 }, // QHD
    {  480,  272, 3, 0,   0,  480,  272 }, // WQVGA
    {  960,  544, 3, 0,   0,  960,  544 }, // qHD
    { 1152,  864, 0, 0,   0, 1152,  864 }, // XGA+
    { 1176,  664, 3, 0,   0, 1176,  664 }, // 1176x664
    { 1200,  960, 1, 0,   0, 1200,  960 }, // 1200x960
    { 1280,  800, 2, 0,   0, 1280,  800 }, // WXGA1280x900
    { 1280,  960, 0, 0,   0, 1280,  960 }, // SXGA-
    { 1366,  768, 3, 0,   0, 1366,  768 }, // FWXGA1366x766
    { 1440,  900, 2, 0,   0, 1440,  900 }, // WXGA+
    { 1600,  900, 0, 0,   0, 1600,  900 }, // HD+
    { 1600, 1024, 2, 0,   0, 1600, 1024 }, // WSXGA
    { 1680, 1050, 2, 0,   0, 1680, 1050 }, // WSXGA+
    { 1920, 1440, 0, 0,   0, 1920, 1440 }, // 1920x1440
    { 2048, 1152, 0, 0,   0, 2048, 1152 }, // QWXGA*/
};

ScreenInfo screen_info;

ScreenParam::ScreenParam(SCREEN_MODE screen_mode) {
    mode = screen_mode;
    xoffset = 0;
    yoffset = 0;
    if (screen_mode == SCREEN_MODE_MAX) {
        width = screen_info.screen.width;
        height = screen_info.screen.height;
        aspect = screen_info.screen.aspect;
        wide_flag = screen_info.screen.wide_flag;
    }
    else {
        width = screen_size[screen_mode].width;
        height = screen_size[screen_mode].height;
        aspect = (double_t)width / (double_t)height;
        wide_flag = screen_size[screen_mode].wide_flag;
    }
}

screen_mode_scale_data::screen_mode_scale_data(SCREEN_MODE src_mode, SCREEN_MODE dst_mode) {
    scale = 1.0f;
    src_res = 0.0f;
    dst_res = 0.0f;
    this->src_mode = src_mode;
    this->dst_mode = dst_mode;
    scale = get_screen_conv_scale(dst_mode, src_mode);
    if (dst_mode != SCREEN_MODE_MAX && src_mode != SCREEN_MODE_MAX
        && dst_mode >= SCREEN_MODE_QVGA && src_mode >= SCREEN_MODE_MAX
        && dst_mode < SCREEN_MODE_MAX && src_mode < SCREEN_MODE_MAX) {
        src_res.x = (float_t)screen_size[src_mode].width * 0.5f;
        src_res.y = (float_t)screen_size[src_mode].height * 0.5f;
        dst_res.x = (float_t)screen_size[dst_mode].width * 0.5f;
        dst_res.y = (float_t)screen_size[dst_mode].height * 0.5f;
    }
}

ScreenParam& get_screen_param() {
    return screen_info.screen;
}

ScreenParam& get_render_screen_param() {
    return screen_info.render;
}

vec2 get_screen_conv_scale(const SCREEN_MODE dst_mode, const SCREEN_MODE src_mode) {
    if (dst_mode == SCREEN_MODE_MAX || src_mode == SCREEN_MODE_MAX
        || dst_mode < SCREEN_MODE_QVGA || src_mode < SCREEN_MODE_QVGA
        || dst_mode >= SCREEN_MODE_MAX || src_mode >= SCREEN_MODE_MAX)
        return 1.0f;

    float_t scale_x = (float_t)screen_size[dst_mode].width / (float_t)screen_size[src_mode].width;
    float_t scale_y = (float_t)screen_size[dst_mode].height / (float_t)screen_size[src_mode].height;
    int32_t v9 = screen_size[src_mode].wide_flag;
    int32_t v10 = screen_size[dst_mode].wide_flag;
    if (v9 != v10) {
        if (!v9 && 10 == 1)
            return scale_x;

        if (v9 == 2 || v9 == 3)
            switch (v10) {
            case 0:
            case 1:
            case 2:
                return scale_x;
            }
    }

    if (src_mode == SCREEN_MODE_qHD && dst_mode != SCREEN_MODE_qHD && v10 == 3)
        return scale_x;
    else
        return scale_y;
}

void get_screen_conv_pos(vec2& dst_pos, const SCREEN_MODE dst_mode,
    const vec2& src_pos, const SCREEN_MODE src_mode) {
    if (dst_mode == SCREEN_MODE_MAX || src_mode == SCREEN_MODE_MAX
        || dst_mode < SCREEN_MODE_QVGA || src_mode < SCREEN_MODE_QVGA
        || dst_mode >= SCREEN_MODE_MAX || src_mode >= SCREEN_MODE_MAX) {
        dst_pos = src_pos;
        return;
    }

    const ScreenSize* src = &screen_size[src_mode];
    const ScreenSize* dst = &screen_size[dst_mode];

    vec2 src_size((float_t)src->width, (float_t)src->height);
    vec2 dst_size((float_t)dst->width, (float_t)dst->height);
    vec2 scale = get_screen_conv_scale(dst_mode, src_mode);
    dst_pos = dst_size * 0.5f + (src_pos - src_size * 0.5f) * scale;
}

void init_screen(SCREEN_MODE mode) {
    screen_info.screen = ScreenParam(mode);

    int32_t width = screen_size[mode].width;
    int32_t height = screen_size[mode].height;

    screen_info.render.mode = SCREEN_MODE_MAX;
    screen_info.render.xoffset = screen_size[mode].render_xoffset;
    screen_info.render.yoffset = screen_size[mode].render_yoffset;
    screen_info.render.width = width;
    screen_info.render.height = height;
    screen_info.render.aspect = (double_t)width / (double_t)height;
    screen_info.render.wide_flag = 4;

    for (int32_t i = 0; i < SCREEN_MODE_MAX; i++)
        if (screen_size[i].width == width && screen_size[i].height == height) {
            screen_info.render.mode = (SCREEN_MODE)i;
            screen_info.render.wide_flag = screen_size[i].wide_flag;
        }

    screen_info.back_color = color_black;
}

ScreenInfo::ScreenInfo() {

}
