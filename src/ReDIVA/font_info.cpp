/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "font_info.hpp"

font_info* font_info_default;

font_info::font_info() : font_ptr(), type() {
    font_handler_index = -1;
    init_font_data(0);
}

font_info::font_info(int32_t font_handler_index) : font_ptr(), type() {
    this->font_handler_index = -1;
    init_font_data(font_handler_index);
}

void font_info::init_font_data(int32_t font_handler_index) {
    this->font_handler_index = font_handler_index;

    const font_handler* fh = fontmap_data_get_font_handler(font_handler_index);
    const font* f = fh->font_ptr;
    font_ptr = f;
    font_glyph.x = (float_t)f->glyph_width;
    font_glyph.y = (float_t)f->glyph_height;
    font_glyph_box.x = (float_t)f->glyph_box_width;
    font_glyph_box.y = (float_t)f->glyph_box_height;

    set_glyph_size((float_t)fh->glyph_width, (float_t)fh->glyph_height);

    spacing.x = (float_t)fh->glyph_horizontal_spacing;
    spacing.y = (float_t)fh->glyph_vertical_spacing;

    type = 0;
    if (f->column_scale_num != f->column_scale_denom)
        type = 1;
    if (fh->disable_glyph_spacing && f->disable_glyph_spacing)
        type = 2;
}

font_char* font_info::get_char_data(wchar_t c, font_char* font_c) const {
    const font_character* fc = font_ptr->get_character(c);
    if (!fc->init)
        return 0;

    vec2 glyph_size = font_glyph;
    if (fc->halfwidth)
        glyph_size.x *= 0.5f;

    float_t glyph_offset = 0.0f;
    float_t glyph_width = glyph_size.x;
    if (type == 2) {
        glyph_offset = (float_t)fc->glyph_offset;
        glyph_width = (float_t)fc->glyph_width;
    }

    font_c->tex.pos = vec2((float_t)fc->tex_column, (float_t)fc->tex_row) * font_glyph_box;
    font_c->tex.size = glyph_size;
    font_c->glyph_offset = glyph_offset;
    font_c->glyph_width = glyph_width;
    font_c->field_18 = false;
    return font_c;
}

void font_info::set_glyph_size(float_t glyph_width, float_t glyph_height) {
    glyph.x = glyph_width;
    glyph.y = glyph_height;
    glyph_ratio.x = glyph_width / font_glyph.x;
    glyph_ratio.y = glyph_height / font_glyph.y;
}

bool font_info::sub_140199170() const {
    return font_glyph != glyph && font_glyph == font_glyph_box;
}

void font_info_default_init() {
    if (!font_info_default)
        font_info_default = new font_info;
}

const font_info* font_info_default_get() {
    return font_info_default;
}

void font_info_default_free() {
    if (font_info_default) {
        delete font_info_default;
        font_info_default = 0;
    }
}
