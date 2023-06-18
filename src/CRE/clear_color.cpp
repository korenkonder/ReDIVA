/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "clear_color.hpp"

color4u8 clear_color;
bool set_clear_color;

vec4 get_clear_color() {
    if (!set_clear_color)
        return 0.0f;

    return clear_color;
}

