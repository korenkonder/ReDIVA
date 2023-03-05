/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "clear_color.hpp"

vec4u8 clear_color;
bool set_clear_color;

vec4 get_clear_color() {
    if (!set_clear_color)
        return 0.0f;

    vec4 _clear_color;
    vec4u8_to_vec4(clear_color, _clear_color);
    return _clear_color * (float_t)(1.0 / 255.0);
}

