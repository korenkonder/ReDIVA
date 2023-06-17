/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/prj/shared_ptr.hpp"
#include "../KKdLib/prj/stack_allocator.hpp"
#include "file_handler.hpp"

#define FONT_COUNT 18

struct font_character {
    bool init;
    bool halfwidth;
    uint8_t tex_column;
    uint8_t tex_row;
    uint8_t glyph_offset;
    uint8_t glyph_width;
};

struct font {
    int32_t sprite_id;
    uint8_t glyph_width;
    uint8_t glyph_height;
    uint8_t glyph_box_width;
    uint8_t glyph_box_height;
    uint8_t column_scale_num;
    uint8_t column_scale_denom;
    int32_t map_id;
    float_t column_scale;
    size_t characters_count_per_row;
    std::vector<font_character> characters;
    bool disable_glyph_spacing;

    font();
    ~font();

    const font_character* get_character(wchar_t c) const;
    void init(int32_t sprite_id, uint8_t glyph_width, uint8_t glyph_height,
        uint8_t glyph_box_width, uint8_t glyph_box_height, uint8_t column_scale_num,
        uint8_t column_scale_denom, int32_t map_id);
    void set_column_scale(uint8_t num, uint8_t denom);
};

struct font_handler {
    const font* font_ptr;
    int16_t glyph_width;
    int16_t glyph_height;
    int16_t glyph_horizontal_spacing;
    int16_t glyph_vertical_spacing;
    bool disable_glyph_spacing;

    void set_font(const font* f, bool disable_glyph_spacing);
};

extern void fontmap_data_init();
extern const font_handler* fontmap_data_get_font_handler(int32_t index);
extern bool fontmap_data_load_file();
extern void fontmap_data_read_file();
extern void fontmap_data_free();
