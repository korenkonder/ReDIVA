/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "clear_color.hpp"
#include "gl_rend_state.hpp"

color4u8 clear_color;
bool set_clear_color;

void clear_color_set_gl(p_gl_rend_state& p_gl_rend_st) {
    vec4 color = 0.0f;
    if (set_clear_color)
        color = clear_color;
    p_gl_rend_st.clear_color(color.x, color.y, color.z, color.w);
}
