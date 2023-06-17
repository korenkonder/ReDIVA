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
        TEXT_FLAG_5                          = 0x00010,
        TEXT_FLAG_6                          = 0x00020,
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


struct PrintWork {
    vec4u8 color;
    vec4u8 fill_color;
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

    font_char GetCharData(wchar_t c);
    vec2 GetTextSize(const wchar_t* str, size_t length);
    vec2 GetTextSize(const wchar_t* str_begin, const wchar_t* str_end);
    vec2 GetTextSize(const char* str, size_t length);
    vec2 GetTextSize(const char* str_begin, const char* str_end);
    vec2 GetTextSize(const std::wstring& str);
    void PrintText(app::text_flags flags, const char* str);
    void PrintText(app::text_flags flags, const char* str, size_t length);
    void PrintText(app::text_flags flags, const wchar_t* str_begin, const wchar_t* str_end);
    void SetFont(const font_info* value);
    void SetResolutionMode(::resolution_mode value);

    void printf(app::text_flags flags, const char* fmt, ...);
    void printf_align_left(const char* fmt, ...);
    void wprintf(app::text_flags flags, const wchar_t* fmt, ...);
    void wprintf_align_left(const wchar_t* fmt, ...);
    void vprintf(app::text_flags flags, const char* fmt, va_list args);
    void vwprintf(app::text_flags flags, const wchar_t* fmt, va_list args);
};
