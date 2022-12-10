/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "clear_color.hpp"

const vec4u8 color_black        = { 0x00, 0x00, 0x00, 0xFF};
const vec4u8 color_blue         = { 0x00, 0x00, 0xFF, 0xFF };
const vec4u8 color_red          = { 0xFF, 0x00, 0x00, 0xFF };
const vec4u8 color_magenta      = { 0xFF, 0x00, 0xFF, 0xFF };
const vec4u8 color_green        = { 0x00, 0xFF, 0x00, 0xFF };
const vec4u8 color_cyan         = { 0x00, 0xFF, 0xFF, 0xFF };
const vec4u8 color_yellow       = { 0xFF, 0xFF, 0x00, 0xFF };
const vec4u8 color_white        = { 0xFF, 0xFF, 0xFF, 0xFF };
const vec4u8 color_dark_blue    = { 0x00, 0x00, 0x7F, 0xFF };
const vec4u8 color_dark_red     = { 0x7F, 0x00, 0x00, 0xFF };
const vec4u8 color_dark_magenta = { 0x7F, 0x00, 0x7F, 0xFF };
const vec4u8 color_dark_green   = { 0x00, 0x7F, 0x00, 0xFF };
const vec4u8 color_dark_cyan    = { 0x00, 0x7F, 0x7F, 0xFF };
const vec4u8 color_dark_yellow  = { 0x7F, 0x7F, 0x00, 0xFF };
const vec4u8 color_grey         = { 0x7F, 0x7F, 0x7F, 0xFF };

vec4u8 clear_color;
bool set_clear_color;

vec4 get_clear_color() {
    if (!set_clear_color)
        return 0.0f;

    vec4 _clear_color;
    vec4u8_to_vec4(clear_color, _clear_color);
    return _clear_color * (float_t)(1.0 / 255.0);
}

