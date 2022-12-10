/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "resolution_mode.hpp"

const resolution_table_struct resolution_table[] = {
    {  320,  240, 0, 0,   0,  320,  225 },
    {  640,  480, 0, 0,  60,  640,  360 },
    {  800,  600, 0, 0,  75,  800,  450 },
    { 1024,  768, 0, 0,  96, 1024,  576 },
    { 1280, 1024, 1, 0, 152, 1280,  720 },
    { 1400, 1050, 0, 0, 131, 1400,  788 },
    { 1600, 1200, 0, 0, 150, 1600,  900 },
    {  800,  480, 2, 0,  15,  800,  450 },
    { 1024,  600, 2, 0,  12, 1024,  576 },
    { 1280,  768, 2, 0,  24, 1280,  720 },
    { 1360,  768, 3, 0,   2, 1360,  765 },
    { 1920, 1200, 2, 0,  60, 1920, 1200 },
    { 2560, 1536, 2, 0,  48, 2560, 1440 },
    { 1280,  720, 3, 0,   0, 1280,  720 },
    { 1920, 1080, 3, 0,   0, 1920, 1080 },
    { 2560, 1440, 3, 0,   0, 2560, 1440 },
    {  480,  272, 3, 0,   0,  480,  272 },
    {  960,  544, 3, 0,   0,  960,  544 },
};

vec2 resolution_mode_get_scale(const resolution_mode dst_mode, const resolution_mode src_mode) {
    if (dst_mode == RESOLUTION_MODE_END
        || src_mode == RESOLUTION_MODE_END
        || dst_mode < RESOLUTION_MODE_QVGA
        || src_mode < RESOLUTION_MODE_QVGA
        || dst_mode >= RESOLUTION_MODE_END
        || src_mode >= RESOLUTION_MODE_END)
        return 1.0f;

    float_t scale_x = (float_t)resolution_table[dst_mode].width_full / (float_t)resolution_table[src_mode].width_full;
    float_t scale_y = (float_t)resolution_table[dst_mode].height_full / (float_t)resolution_table[src_mode].height_full;
    int32_t v9 = resolution_table[src_mode].field_8;
    int32_t v10 = resolution_table[dst_mode].field_8;
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

    if (src_mode == RESOLUTION_MODE_qHD && dst_mode != RESOLUTION_MODE_qHD && v10 == 3)
        return scale_x;
    else
        return scale_y;
}

void resolution_mode_scale_pos(vec2& dst_pos, const resolution_mode dst_mode,
    const vec2& src_pos, const resolution_mode src_mode) {
    if (dst_mode == RESOLUTION_MODE_END
        || src_mode == RESOLUTION_MODE_END
        || dst_mode < RESOLUTION_MODE_QVGA
        || src_mode < RESOLUTION_MODE_QVGA
        || dst_mode >= RESOLUTION_MODE_END
        || src_mode >= RESOLUTION_MODE_END) {
        dst_pos = src_pos;
        return;
    }

    const resolution_table_struct* src = &resolution_table[src_mode];
    const resolution_table_struct* dst = &resolution_table[dst_mode];

    vec2 src_size((float_t)src->width_full, (float_t)src->height_full);
    vec2 dst_size((float_t)dst->width_full, (float_t)dst->height_full);
    vec2 scale = resolution_mode_get_scale(dst_mode, src_mode);
    dst_pos = dst_size * 0.5f + (src_pos - src_size * 0.5f) * scale;
}