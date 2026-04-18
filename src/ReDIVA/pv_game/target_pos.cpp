/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "target_pos.hpp"
#include "../../CRE/screen_param.hpp"

vec2 target_pos_scale;
vec2 target_pos_offset;

void target_pos_scale_offset_apply(const vec2* src, vec2* dst) {
    if (src && dst)
        *dst = target_pos_scale * *src + target_pos_offset;
}

void target_pos_scale_offset_get() {
    ScreenParam screen_param(SCREEN_MODE_HD);
    target_pos_offset = 0.0f;
    target_pos_scale.x = (float_t)screen_param.width * (float_t)(1.0 / 480.0);
    target_pos_scale.y = (float_t)screen_param.height * (float_t)(1.0 / 272.0);
}
