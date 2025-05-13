/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "font.hpp"
#include "data.hpp"
#include <map>

struct fontmap {
    p_file_handler file_handler;
    bool read;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    font_handler handler[FONT_COUNT];
    font font[FONT_COUNT];

    fontmap();
    ~fontmap();

    const font_handler* get_font_handler(int32_t index);
    bool load_file();
    void parse_file();
    void read_file();
    void set_font_handler(int32_t handler_index, int32_t font_index, bool disable_glyph_spacing = false);
};

struct font_character_file {
    uint16_t id;
    uint8_t halfwidth;
    uint8_t pad;
    uint8_t tex_column;
    uint8_t tex_row;
    uint8_t glyph_offset;
    uint8_t glypth_width;
};

struct font_file {
    int32_t map_id;
    uint8_t character_advance_width;
    uint8_t character_line_height;
    uint8_t glyph_box_width;
    uint8_t glyph_box_height;
    uint8_t flags;
    uint8_t column_scale_num;
    uint8_t column_scale_denom;
    uint8_t field_B;
    uint32_t field_C;
    uint32_t characters_count_per_row;
    uint32_t characters_count;
    uint32_t characters_offset;
};

struct fontmap_header {
    uint32_t signature;
    uint32_t pad;
    uint32_t font_count;
    uint32_t font_offsets_offset;
};

fontmap* fontmap_data;

static const font_character font_character_null = {};

font::font() : glyph_width(), glyph_height(), glyph_box_width(), glyph_box_height(),
column_scale_num(), column_scale_denom(), column_scale(), characters_count_per_row(), disable_glyph_spacing() {
    sprite_id = -1;
    map_id = -1;
}

font::~font() {

}

const font_character* font::get_character(wchar_t c) const {
    if (c >= 0 && c < characters.size())
        return &characters[c];
    return &font_character_null;
}

void font::init(int32_t sprite_id, uint8_t glyph_width, uint8_t glyph_height,
    uint8_t glyph_box_width, uint8_t glyph_box_height, uint8_t column_scale_num,
    uint8_t column_scale_denom, int32_t map_id) {
    this->sprite_id = sprite_id;
    this->glyph_width = glyph_width;
    this->glyph_height = glyph_height;
    this->glyph_box_width = glyph_box_width;
    this->glyph_box_height = glyph_box_height;
    this->map_id = map_id;

    set_column_scale(column_scale_num, column_scale_denom);

    characters_count_per_row = 16;

    if (characters.capacity() < 0x100)
        characters.reserve(0x100);

    font_character font_char = {};
    font_char.init = true;
    font_char.glyph_offset = 0;
    for (int32_t i = 0; i < 0x100; i++) {
        font_char.halfwidth = column_scale_denom != 1;
        font_char.tex_row = (uint8_t)(i / characters_count_per_row);
        font_char.tex_column = (uint8_t)(i % characters_count_per_row);
        font_char.glyph_width = glyph_width;
        characters.push_back(font_char);
    }
}

void font::set_column_scale(uint8_t num, uint8_t denom) {
    column_scale_num = num;
    column_scale_denom = denom;
    column_scale = (float_t)num / (float_t)denom;
}

void font_handler::set_font(const font* f, bool disable_glyph_spacing) {
    font_ptr = f;
    glyph_width = f->glyph_width;
    glyph_height = f->glyph_height;
    glyph_horizontal_spacing = 0;
    glyph_vertical_spacing = 0;
    this->disable_glyph_spacing = disable_glyph_spacing;
}

void fontmap_data_init() {
    if (!fontmap_data)
        fontmap_data = new fontmap;
}

const font_handler* fontmap_data_get_font_handler(int32_t index) {
    return fontmap_data->get_font_handler(index);
}

bool fontmap_data_load_file() {
    return fontmap_data->load_file();
}

void fontmap_data_read_file() {
    fontmap_data->read_file();
}

void fontmap_data_free() {
    if (fontmap_data) {
        delete fontmap_data;
        fontmap_data = 0;
    }
}

fontmap::fontmap() : read(), handler() {
    font[0].init(3348, 10, 16, 10, 16, 1, 1, -1);
    font[1].init(2281, 12, 18, 14, 20, 1, 1, 12);
    font[2].init(211, 24, 24, 26, 26, 1, 2, 0);
    font[3].init(7365, 24, 24, 26, 26, 1, 2, 0);
    font[4].init(1625, 12, 16, 14, 18, 1, 1, 5);
    font[5].init(1390, 14, 20, 16, 22, 1, 1, 1);
    font[6].init(1670, 14, 18, 16, 20, 1, 1, 9);
    font[7].init(1602, 20, 26, 22, 28, 1, 1, 6);
    font[8].init(38527, 20, 22, 22, 24, 1, 1, 14);
    font[9].init(1695, 22, 22, 24, 24, 1, 1, 10);
    font[10].init(1665, 22, 24, 24, 26, 1, 1, 11);
    font[11].init(1391, 24, 30, 26, 32, 1, 1, 2);
    font[12].init(1282, 26, 24, 28, 26, 1, 1, 3);
    font[13].init(2853, 28, 40, 30, 42, 1, 1, 13);
    font[14].init(8827, 28, 40, 30, 42, 1, 1, 13);
    font[15].init(1283, 34, 32, 36, 34, 1, 1, 4);
    font[16].init(1603, 40, 52, 42, 54, 1, 1, 7);
    font[17].init(1604, 56, 46, 58, 48, 1, 1, 8);

    set_font_handler(0, 0);
    set_font_handler(15, 1);
    set_font_handler(16, 2);
    set_font_handler(17, 3);
    set_font_handler(1, 4);
    set_font_handler(2, 5);
    set_font_handler(3, 6);
    set_font_handler(4, 7);
    set_font_handler(5, 8);
    set_font_handler(6, 9);
    set_font_handler(7, 10);
    set_font_handler(8, 11);
    set_font_handler(9, 12);
    set_font_handler(10, 13);
    set_font_handler(11, 14);
    set_font_handler(12, 15);
    set_font_handler(13, 16);
    set_font_handler(14, 17);
}

fontmap::~fontmap() {

}

const font_handler* fontmap::get_font_handler(int32_t index) {
    if (index < 0 || index >= FONT_COUNT)
        index = 0;
    return &handler[index];
}

bool fontmap::load_file() {
    if (read)
        return false;

    if (file_handler.check_not_ready())
        return true;

    parse_file();

    file_handler.reset();
    read = true;
    return false;
}

void fontmap::parse_file() {
    size_t data = (size_t)file_handler.get_data();
    fontmap_header* fmh = (fontmap_header*)data;
    if (fmh->signature != reverse_endianness_uint32_t('FMH3'))
        return;

    std::map<int32_t, font_file*> font_map;

    uint32_t* font_offsets = (uint32_t*)(data + fmh->font_offsets_offset);
    for (uint32_t i = 0; i < fmh->font_count; i++) {
        font_file* ff = (font_file*)(data + font_offsets[i]);
        auto elem = font_map.find(ff->map_id);
        if (elem != font_map.end())
            elem->second = ff;
        else
            font_map.insert({ ff->map_id, ff });
    }

    for (::font& i : font) {
        if (i.map_id == -1)
            continue;

        auto elem = font_map.find(i.map_id);
        if (elem == font_map.end())
            continue;

        font_file* ff = elem->second;
        i.set_column_scale(ff->column_scale_num, ff->column_scale_denom);
        i.characters_count_per_row = ff->characters_count_per_row;
        i.disable_glyph_spacing = (ff->flags & 2) != 0;
        if (!ff->characters_count)
            continue;

        font_character_file* first_char = (font_character_file*)(data + ff->characters_offset);
        font_character_file* last_char = &first_char[ff->characters_count];

        font_character_file* max_char = first_char;
        if (first_char != last_char)
            for (font_character_file* j = first_char + 1; j != last_char; j++)
                if (max_char->id < j->id)
                    max_char = j;

        i.characters.clear();
        i.characters.resize(max_char->id + 1ULL);

        for (size_t k = 0; k < ff->characters_count; k++) {
            font_character_file* c_f = &((font_character_file*)(data + ff->characters_offset))[k];
            if (c_f->id < i.characters.size()) {
                font_character* c = &i.characters.data()[c_f->id];
                c->init = true;
                c->halfwidth = !!c_f->halfwidth;
                c->tex_column = c_f->tex_column;
                c->tex_row = c_f->tex_row;
                c->glyph_offset = c_f->glyph_offset;
                c->glyph_width = c_f->glypth_width;
            }
        }
    }
}

void fontmap::read_file() {
    if (read)
        return;

    file_handler.read_file(&data_list[DATA_AFT], "rom/", "fontmap.farc", "fontmap.bin", false);
    read = false;
}

void fontmap::set_font_handler(int32_t handler_index, int32_t font_index, bool disable_glyph_spacing) {
    handler[handler_index].set_font(&font[font_index], disable_glyph_spacing);
}
