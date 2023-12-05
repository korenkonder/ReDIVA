/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "clear_color.hpp"
#include "gl_state.hpp"

color4u8 clear_color;
bool set_clear_color;

void clear_color_set_gl() {
    vec4 color = 0.0f;
    if (set_clear_color)
        color = clear_color;
    glClearColor(color.x, color.y, color.z, color.w);
}

