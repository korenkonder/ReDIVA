/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/rectangle.hpp"
#include "../KKdLib/vec.hpp"
#include "../CRE/font.hpp"

struct font_char {
    rectangle tex;
    float_t glyph_offset;
    float_t glyph_width;
    uint8_t field_18;

    font_char();
};

struct font_info {
    int32_t font_handler_index;
    const font* font_ptr;
    int32_t type;
    vec2 font_glyph;
    vec2 font_glyph_box;
    vec2 glyph;
    vec2 glyph_ratio;
    vec2 spacing;

    font_info();
    font_info(int32_t font_handler_index);

    void init_font_data(int32_t font_handler_index);
    font_char* get_char_data(wchar_t c, font_char* font_c) const;
    void set_glyph_size(float_t glyph_width, float_t glyph_height);

    bool sub_140199170() const;
};

extern void font_info_default_init();
extern const font_info* font_info_default_get();
extern void font_info_default_free();
