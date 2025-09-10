/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include "../CRE/auth_2d.hpp"
#include "font_info.hpp"

namespace app {
    enum text_flags {
        TEXT_FLAG_ALIGN_FLAG_LEFT            = 0x00001,
        TEXT_FLAG_ALIGN_FLAG_RIGHT           = 0x00002,
        TEXT_FLAG_ALIGN_FLAG_H_CENTER        = 0x00004,
        TEXT_FLAG_ALIGN_FLAG_LOCATE_H_CENTER = 0x00008,
        TEXT_FLAG_ALIGN_FLAG_V_CENTER        = 0x00010,
        TEXT_FLAG_ALIGN_FLAG_LOCATE_V_CENTER = 0x00020,
        TEXT_FLAG_7                          = 0x00040,
        TEXT_FLAG_8                          = 0x00080,
        TEXT_FLAG_9                          = 0x00100,
        TEXT_FLAG_CLIP                       = 0x00200,
        TEXT_FLAG_11                         = 0x00400,
        TEXT_FLAG_12                         = 0x00800,
        TEXT_FLAG_13                         = 0x01000,
        TEXT_FLAG_14                         = 0x02000,
        TEXT_FLAG_15                         = 0x04000,
        TEXT_FLAG_16                         = 0x08000,
        TEXT_FLAG_FONT                       = 0x10000,
    };
}

struct sprite_text_mesh {
    int32_t sprite_id;
    std::vector<spr::SpriteVertex> vertices;

    sprite_text_mesh();
    ~sprite_text_mesh();

    spr::SpriteVertex* add_char();
    void add_set_char(app::text_flags flags,
        rectangle pos, rectangle uv, color4u8 color);
    void apply_scale_offset(vec2 scale, vec2 offset);

    static void set_char(spr::SpriteVertex* vtx, app::text_flags flags,
        rectangle pos, rectangle uv, color4u8 color);
};

struct PrintWork {
    color4u8 color;
    color4u8 fill_color;
    bool clip;
    rectangle clip_data;
    spr::SprPrio prio;
    uint32_t layer;
    resolution_mode resolution_mode;
    uint32_t field_28;
    vec2 text_current_loc;
    vec2 line_origin_loc;
    size_t line_length;
    const font_info* font;
    wchar_t empty_char;

    PrintWork();

    void DrawLine(vec2 pos[2]);
    void DrawPolyLine(vec2* points, size_t count);
    void DrawRectangle(rectangle rect);
    void DrawTextMesh(app::text_flags flags, sprite_text_mesh& mesh);
    void FillRectangle(rectangle rect);
    font_char GetCharData(wchar_t c);
    rectangle GetClipBox(app::text_flags flags, vec2 glyph_size, vec2 glyph_ratio);
    std::vector<font_char> GetStringData(const wchar_t* str_begin, const wchar_t* str_end);
    vec2 GetStringSize(const wchar_t* str_begin, const wchar_t* str_end);
    vec2 GetTextOffset(app::text_flags flags, vec2 size);
    vec2 GetTextSize(const wchar_t* str, size_t length);
    vec2 GetTextSize(const wchar_t* str_begin, const wchar_t* str_end);
    vec2 GetTextSize(const char* str, size_t length);
    vec2 GetTextSize(const char* str_begin, const char* str_end);
    vec2 GetTextSize(const std::wstring& str);
    void NewLine();
    void PrintText(app::text_flags flags, const char* str);
    void PrintText(app::text_flags flags, const char* str, size_t length);
    void PrintText(app::text_flags flags, const wchar_t* str_begin, const wchar_t* str_end);
    void PutText(app::text_flags flags, const wchar_t* str_begin, const wchar_t* str_end);
    void set_font(const font_info* value);
    void set_resolution_mode(const ::resolution_mode value);
    void set_text_position(const float_t column, const float_t line);

    void printf(app::text_flags flags,
        _In_z_ _Printf_format_string_ const char* const fmt, ...);
    void printf_align_left(_In_z_ _Printf_format_string_ const char* const fmt, ...);
    void wprintf(app::text_flags flags,
        _In_z_ _Printf_format_string_ const wchar_t* const fmt, ...);
    void wprintf_align_left(_In_z_ _Printf_format_string_ const wchar_t* const fmt, ...);
    void vprintf(app::text_flags flags,
        _In_z_ _Printf_format_string_ const char* const fmt, va_list args);
    void vwprintf(app::text_flags flags,
        _In_z_ _Printf_format_string_ const wchar_t* const fmt, va_list args);

    inline void set_color(const color4u8 value) {
        color = value;
    }

    inline void set_fill_color(const color4u8 value) {
        fill_color = value;
    }

    inline void set_fill_opacity(const uint8_t value) {
        fill_color.a = value;
    }

    inline void set_opacity(const uint8_t value) {
        color.a = value;
    }

    inline void set_position(const vec2 loc) {
        line_origin_loc = loc;
        text_current_loc = line_origin_loc;
    }

    inline void set_position(const float_t loc_x, const float_t loc_y) {
        line_origin_loc.x = loc_x;
        line_origin_loc.y = loc_y;
        text_current_loc = line_origin_loc;
    }

    inline void set_prio(const spr::SprPrio value) {
        prio = value;
    }

    static void ClampPosToClipBox(float_t pos_min, float_t pos_max,
        float_t clip_box_min, float_t clip_box_max, float_t& clip_pos_min, float_t& clip_pos_max);

    static int32_t sub_140197D60(rectangle clip_box, rectangle& pos, rectangle& uv);
};
