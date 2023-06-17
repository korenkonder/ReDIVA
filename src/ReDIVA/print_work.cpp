/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "print_work.hpp"
#include "../KKdLib/str_utils.hpp"

PrintWork::PrintWork() : clip(), layer(), field_28(), line_length() {
    color = { 0xFF, 0xFF, 0xFF, 0xFF };
    fill_color = { 0x80, 0x80, 0x80, 0xFF };
    this->clip_data.size = 0i64;
    prio = spr::SPR_PRIO_DEFAULT;
    layer = 0;
    resolution_mode = RESOLUTION_MODE_HD;
    field_28 = 0;
    line_length = 0i64;
    font = font_info_default_get();
    empty_char = '\x25\xA1';
}

font_char PrintWork::GetCharData(wchar_t c) {
    font_char font_c = {};
    if (!font->get_char_data(c, &font_c))
        font->get_char_data(empty_char, &font_c);
    return font_c;
}

vec2 PrintWork::GetTextSize(const wchar_t* str, size_t length) {
    return GetTextSize(str, str + length);
}

vec2 PrintWork::GetTextSize(const wchar_t* str_begin, const wchar_t* str_end) {
    /*app::sap::SimpleExtentContentHandler v10;
    v10.print_work = this;

    app::sap::SimpleAutoLineFeedReader v8;
    v8.print_work = this;
    v8.SimpleAutoLineFeedReader::Print(str_begin, str_end);*/

    vec2 size;
    /*size.x = max_def(v10.size.x, v10.space_size.x);
    size.y = v10.space_size.y + v10.size.y;*/
    return size;
}

vec2 PrintWork::GetTextSize(const char* str, size_t length) {
    return GetTextSize(str, str + length);
}

vec2 PrintWork::GetTextSize(const char* str_begin, const char* str_end) {
    wchar_t* wstr = utf8_to_utf16(str_begin, str_end - str_begin);
    vec2 size = GetTextSize(wstr, wstr + utf16_length(wstr));
    free_def(wstr);
    return size;
}

vec2 PrintWork::GetTextSize(const std::wstring& str) {
    return GetTextSize(str.c_str(), str.c_str() + str.size());
}

void PrintWork::PrintText(app::text_flags flags, const char* str) {
    wchar_t* wstr = utf8_to_utf16(str);
    PrintText(flags, wstr, wstr + utf16_length(wstr));
    free_def(wstr);
}

void PrintWork::PrintText(app::text_flags flags, const char* str, size_t length) {
    wchar_t* wstr = utf8_to_utf16(str, length);
    PrintText(flags, wstr, wstr + utf16_length(wstr));
    free_def(wstr);
}

void PrintWork::PrintText(app::text_flags flags, const wchar_t* str_begin, const wchar_t* str_end) {
    if (clip)
        enum_or(flags, app::TEXT_FLAG_CLIP);

    if (font->sub_140199170())
        enum_or(flags, app::TEXT_FLAG_11);

    /*app::sap::SimpleContentHandler v8;
    v8.data = this;
    v8.flags = flags;

    app::sap::SimpleAutoLineFeedReader v6;
    v6.print_work = this;
    v6.content_handler = &v8;
    v6.Print(str_begin, str_end);*/
}

void PrintWork::SetFont(const font_info* value) {
    font = value;
}

void PrintWork::SetResolutionMode(::resolution_mode value) {
    resolution_mode = value;
}

void PrintWork::printf(app::text_flags flags, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(flags, fmt, args);
    va_end(args);
}

void PrintWork::printf_align_left(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(app::TEXT_FLAG_ALIGN_FLAG_LEFT, fmt, args);
    va_end(args);
}

void PrintWork::wprintf(app::text_flags flags, const wchar_t* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwprintf(flags, fmt, args);
    va_end(args);
}

void PrintWork::wprintf_align_left(const wchar_t* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwprintf(app::TEXT_FLAG_ALIGN_FLAG_LEFT, fmt, args);
    va_end(args);
}

void PrintWork::vprintf(app::text_flags flags, const char* fmt, va_list args) {
    char buf[0x100];
    PrintText(flags, buf, vsprintf_s(buf, sizeof(buf) / sizeof(wchar_t), fmt, args));
}

void PrintWork::vwprintf(app::text_flags flags, const wchar_t* fmt, va_list args) {
    wchar_t buf[0x100];
    PrintText(flags, buf, buf + vswprintf_s(buf, sizeof(buf) / sizeof(wchar_t), fmt, args));
}
