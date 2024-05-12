/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "print_work.hpp"
#include "../KKdLib/str_utils.hpp"
#include "../CRE/data.hpp"
#include "../CRE/shader_ft.hpp"

namespace app {
    namespace sap {
        class ContentHandler {
        public:
            ContentHandler();
            virtual ~ContentHandler();

            virtual void Print(const wchar_t* str_begin, const wchar_t* str_end) = 0;
            virtual void Field_10() = 0;
            virtual void NewLine() = 0;
        };

        class DefaultContentHandler : public ContentHandler {
        public:
            DefaultContentHandler();
            virtual ~DefaultContentHandler() override;

            virtual void Print(const wchar_t* str_begin, const wchar_t* str_end) override;
            virtual void Field_10() override;
            virtual void NewLine() override;
        };

        class SimpleContentHandler : public DefaultContentHandler {
        public:
            PrintWork* print_work;
            app::text_flags flags;

            SimpleContentHandler(PrintWork* print_work, app::text_flags flags);
            virtual ~SimpleContentHandler() override;

            virtual void Print(const wchar_t* str_begin, const wchar_t* str_end) override;
            virtual void NewLine() override;
        };

        class SimpleExtentContentHandler : public DefaultContentHandler {
        public:
            PrintWork* print_work;
            vec2 space_size;
            vec2 size;
            bool new_line;

            SimpleExtentContentHandler(PrintWork* print_work);
            virtual ~SimpleExtentContentHandler() override;

            virtual void Print(const wchar_t* str_begin, const wchar_t* str_end) override;
            virtual void NewLine() override;
        };

        class Reader {
        public:
            Reader();
            virtual ~Reader();

            virtual void Print(const wchar_t* str_begin, const wchar_t* str_end) = 0;
            virtual void SetContentHandler(ContentHandler* content_handler) = 0;
            virtual ContentHandler* GetContentHandler() = 0;
        };

        class SimpleReader : public Reader {
        public:
            PrintWork* print_work;
            ContentHandler* content_handler;

            SimpleReader(PrintWork* print_work, ContentHandler* content_handler);
            virtual ~SimpleReader() override;

            virtual void Print(const wchar_t* str_begin, const wchar_t* str_end) override;
            virtual void SetContentHandler(ContentHandler* content_handler) override;
            virtual ContentHandler* GetContentHandler() override;
        };

        class SimpleAutoLineFeedReader : public SimpleReader {
        public:
            SimpleAutoLineFeedReader(PrintWork* print_work, ContentHandler* content_handler);
            virtual ~SimpleAutoLineFeedReader() override;

            virtual void Print(const wchar_t* str_begin, const wchar_t* str_end) override;
        };
    }
}

sprite_text_mesh::sprite_text_mesh() {
    sprite_id = -1;
}

sprite_text_mesh::~sprite_text_mesh() {

}

spr::SpriteVertex* sprite_text_mesh::add_char() {
    size_t size = vertices.size();
    vertices.resize(size + 4);
    return &vertices.data()[size];
}

void sprite_text_mesh::add_set_char(app::text_flags flags, rectangle pos, rectangle uv, color4u8 color) {
    sprite_text_mesh::set_char(add_char(), flags, pos, uv, color);
}

void sprite_text_mesh::apply_scale_offset(vec2 scale, vec2 offset) {
    for (spr::SpriteVertex& i : vertices)
        *(vec2*)&i.pos.x = *(vec2*)&i.pos.x * scale + offset;
}

void sprite_text_mesh::set_char(spr::SpriteVertex* vtx, app::text_flags flags, rectangle pos, rectangle uv, color4u8 color) {
    vec2 uv_size = uv.size;
    vec2 uv_pos = uv.pos;
    if (flags & app::TEXT_FLAG_11) {
        uv_pos += 0.5f;
        uv_size += -1.0f;
    }

    vtx[0].pos.x = pos.pos.x;
    vtx[0].pos.y = pos.pos.y;
    vtx[0].pos.z = 0.0f;
    vtx[0].uv.x = uv_pos.x;
    vtx[0].uv.y = uv_pos.y;
    vtx[0].color = color;

    vtx[1].pos.x = pos.pos.x + pos.size.x;
    vtx[1].pos.y = pos.pos.y;
    vtx[1].pos.z = 0.0;
    vtx[1].uv.x = uv_pos.x + uv_size.x;
    vtx[1].uv.y = uv_pos.y;
    vtx[1].color = color;

    vtx[2].pos.x = pos.pos.x + pos.size.x;
    vtx[2].pos.y = pos.pos.y + pos.size.y;
    vtx[2].pos.z = 0.0f;
    vtx[2].uv.x = uv_pos.x + uv_size.x;
    vtx[2].uv.y = uv_pos.y + uv_size.y;
    vtx[2].color = color;

    vtx[3].pos.x = pos.pos.x;
    vtx[3].pos.y = pos.pos.y + pos.size.y;
    vtx[3].pos.z = 0.0f;
    vtx[3].uv.x = uv_pos.x;
    vtx[3].uv.y = uv_pos.y + uv_size.y;
    vtx[3].color = color;
}

PrintWork::PrintWork() : clip(), layer(), field_28(), line_length() {
    color = 0xFFFFFFFF;
    fill_color = 0xFF808080;
    prio = spr::SPR_PRIO_DEFAULT;
    resolution_mode = RESOLUTION_MODE_HD;
    font = font_info_default_get();
    empty_char = L'\x25A1';
}

void PrintWork::DrawLine(vec2 pos[2]) {
    spr::put_sprite_line(pos[0], pos[1], resolution_mode, prio, color, layer);
}

void PrintWork::DrawPolyLine(vec2* points, size_t count) {
    spr::put_sprite_line_list(points, count, resolution_mode, prio, color, layer);
}

void PrintWork::DrawRectangle(rectangle rect) {
    spr::put_sprite_multi(rect, resolution_mode, prio, color, layer);
}

void PrintWork::DrawTextMesh(app::text_flags flags, sprite_text_mesh& mesh) {
    if (!mesh.vertices.size())
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    spr::SprArgs args;
    args.blend = (field_28 >> 3) & 0x02;
    args.resolution_mode_screen = resolution_mode;
    args.resolution_mode_sprite = args.resolution_mode_screen;
    args.prio = prio;
    args.color = color;
    args.layer = layer;

    if (field_28 & 0x02) {
        for (spr::SpriteVertex& i : mesh.vertices) {
            i.color.r = 0x40;
            i.color.g = 0x40;
            i.color.b = 0x40;
            i.pos += 2.0f;
        }
        args.SetVertexArray(mesh.vertices.data(), mesh.vertices.size());
        args.id.index = mesh.sprite_id;
        spr::put_sprite(args, aft_spr_db);

        for (spr::SpriteVertex& i : mesh.vertices) {
            i.color.r = color.r;
            i.color.g = color.g;
            i.color.b = color.b;
            i.pos.x -= 2.0f;
            i.pos.y -= 2.0f;
        }
    }

    args.SetVertexArray(mesh.vertices.data(), mesh.vertices.size());
    if (flags & app::TEXT_FLAG_FONT)
        args.shader = SHADER_FT_FONT;
    args.id.index = mesh.sprite_id;
    spr::put_sprite(args, aft_spr_db);
}

void PrintWork::FillRectangle(rectangle rect) {
    if (clip) {
        float_t clip_max_pos_x;
        PrintWork::ClampPosToClipBox(rect.pos.x, rect.pos.x + rect.size.x,
            clip_data.pos.x, clip_data.pos.x + clip_data.size.x, rect.pos.x, clip_max_pos_x);
        rect.size.x = clip_max_pos_x - rect.pos.x;

        float_t clip_max_pos_y;
        PrintWork::ClampPosToClipBox(rect.pos.y, rect.pos.y + rect.size.y,
            clip_data.pos.y, clip_data.pos.y + clip_data.size.y, rect.pos.y, clip_max_pos_y);
        rect.size.y = clip_max_pos_y - rect.pos.y;
    }

    spr::put_sprite_rect(rect, resolution_mode, prio, fill_color, layer);
}

font_char PrintWork::GetCharData(wchar_t c) {
    font_char font_c = {};
    if (!font->get_char_data(c, &font_c))
        font->get_char_data(empty_char, &font_c);
    return font_c;
}

rectangle PrintWork::GetClipBox(app::text_flags flags, vec2 glyph_size, vec2 glyph_ratio) {
    glyph_ratio = 1.0f / glyph_ratio;
    rectangle clip_box;
    clip_box.pos = (clip_data.pos - text_current_loc) * glyph_ratio;
    clip_box.size = clip_data.size * glyph_ratio;

    if (flags & app::TEXT_FLAG_ALIGN_FLAG_RIGHT)
        clip_box.pos.x += glyph_size.x;
    else if (flags & app::TEXT_FLAG_ALIGN_FLAG_H_CENTER)
        clip_box.pos.x = (glyph_size.x - clip_data.size.x) * 0.5f * glyph_ratio.x;
    else if (flags & app::TEXT_FLAG_ALIGN_FLAG_LOCATE_H_CENTER)
        clip_box.pos.x += glyph_size.x * 0.5f;

    if (flags & app::TEXT_FLAG_ALIGN_FLAG_V_CENTER)
        clip_box.pos.y = (glyph_size.y - clip_data.size.y) * 0.5f * glyph_ratio.y;
    else if (flags & app::TEXT_FLAG_ALIGN_FLAG_LOCATE_V_CENTER)
        clip_box.pos.y += glyph_size.y * 0.5f;
    return clip_box;
}

std::vector<font_char> PrintWork::GetStringData(const wchar_t* str_begin, const wchar_t* str_end) {
    std::vector<font_char> vec(str_end - str_begin);
    font_char* fc = vec.data();
    for (const wchar_t* i = str_begin; i != str_end; i++)
        *fc++ = PrintWork::GetCharData(*i);
    return vec;
}

vec2 PrintWork::GetStringSize(const wchar_t* str_begin, const wchar_t* str_end) {
    std::vector<font_char> vec = GetStringData(str_begin, str_end);
    float_t spacing = font->spacing.x / font->glyph_ratio.x;
    vec2 size;
    for (font_char& i : vec) {
        if (&i - vec.data())
            size.x += spacing;
        size.x += i.glyph_width;
    }

    size.x *= font->glyph_ratio.x;
    size.y = font->glyph.y;
    return size;
}

vec2 PrintWork::GetTextSize(const wchar_t* str, size_t length) {
    return GetTextSize(str, str + length);
}

vec2 PrintWork::GetTextSize(const wchar_t* str_begin, const wchar_t* str_end) {
    app::sap::SimpleExtentContentHandler content_handler(this);
    app::sap::SimpleAutoLineFeedReader reader(this, &content_handler);
    reader.Print(str_begin, str_end);

    vec2 size;
    size.x = max_def(content_handler.size.x, content_handler.space_size.x);
    size.y = content_handler.space_size.y + content_handler.size.y;
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

void PrintWork::NewLine() {
    text_current_loc.x = line_origin_loc.x;
    float_t loc_y = text_current_loc.y + font->spacing.y + font->glyph.y;
    text_current_loc.y = loc_y;
    line_origin_loc.y = loc_y;
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

    app::sap::SimpleContentHandler content_handler(this, flags);
    app::sap::SimpleAutoLineFeedReader reader(this, &content_handler);
    reader.Print(str_begin, str_end);
}

// It was intended to be DrawText but there's WinAPI macro for that
void PrintWork::PutText(app::text_flags flags, const wchar_t* str_begin, const wchar_t* str_end) {
    if (str_begin == str_end)
        return;

    app::text_flags v5 = (app::text_flags)(flags
        & (app::TEXT_FLAG_ALIGN_FLAG_LOCATE_V_CENTER | app::TEXT_FLAG_ALIGN_FLAG_V_CENTER
            | app::TEXT_FLAG_ALIGN_FLAG_LOCATE_H_CENTER | app::TEXT_FLAG_ALIGN_FLAG_H_CENTER
            | app::TEXT_FLAG_ALIGN_FLAG_RIGHT | app::TEXT_FLAG_ALIGN_FLAG_LEFT));
    app::text_flags v6 = (app::text_flags)(flags & (app::TEXT_FLAG_11 | app::TEXT_FLAG_CLIP));
    bool clip = !!(flags & app::TEXT_FLAG_CLIP);
    if (clip && font->glyph.y + text_current_loc.y <= clip_data.pos.y
        && clip_data.pos.y + clip_data.size.y < text_current_loc.y)
        return;

    std::vector<font_char> v43 = PrintWork::GetStringData(str_begin, str_end);
    float_t spacing = font->spacing.x / font->glyph_ratio.x;

    vec2 glyph_ratio = font->glyph_ratio;
    rectangle clip_box;
    if (clip) {
        float_t glyph_size_x = 0.0f;
        for (font_char& i : v43)
            glyph_size_x += i.glyph_width + spacing;

        vec2 glyph_size;
        glyph_size.x = glyph_size_x - spacing;
        glyph_size.y = font->glyph.y;
        clip_box = GetClipBox(v5, glyph_size, glyph_ratio);
    }
    else
        clip_box = {};

    std::vector<sprite_text_mesh> v44;
    sprite_text_mesh v45;
    v45.sprite_id = font->font_ptr->sprite_id;
    v44.push_back(v45);
    v44.back().vertices.reserve(str_end - str_begin);

    float_t glyph_x = 0.0f;
    if (str_end - str_begin) {
        for (size_t i = 0; i != str_end - str_begin; i++) {
            font_char* fc = &v43.data()[i];
            rectangle v37;
            v37.pos.x = glyph_x;
            v37.pos.y = 0.0f;
            v37.size.x = fc->glyph_width;
            v37.size.y = fc->tex.size.y;
            rectangle v42;
            v42.pos.x = fc->tex.pos.x + fc->glyph_offset;
            v42.pos.y = fc->tex.pos.y;
            v42.size.x = fc->glyph_width;
            v42.size.y = fc->tex.size.y;
            if (!clip || PrintWork::sub_140197D60(clip_box, v37, v42))
                v44.data()[fc->field_18].add_set_char(v6, v37, v42, color);
            glyph_x += fc->glyph_width + spacing;
        }
    }
    else
        glyph_x -= spacing;

    vec2 v35;
    v35.x = glyph_x * glyph_ratio.x;
    v35.y = font->glyph.y;

    vec2 v41 = PrintWork::sub_140197B80((app::text_flags)(v5 | v6), v35);
    if (v5 & app::TEXT_FLAG_ALIGN_FLAG_LEFT)
        text_current_loc.x += font->spacing.x + v35.x;

    for (sprite_text_mesh& i : v44) {
        i.apply_scale_offset(glyph_ratio, v41);
        DrawTextMesh(flags, i);
    }
}

void PrintWork::SetFont(const font_info* value) {
    font = value;
}

void PrintWork::SetResolutionMode(::resolution_mode value) {
    resolution_mode = value;
}

void PrintWork::set_text_position(float_t column, float_t line) {
    line_origin_loc.x = (font->spacing.x + font->glyph.x * font->font_ptr->column_scale) * column;
    line_origin_loc.y = (font->spacing.y + font->glyph.y) * line;
    text_current_loc = line_origin_loc;
}

void PrintWork::printf(app::text_flags flags,
    _In_z_ _Printf_format_string_ const char* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(flags, fmt, args);
    va_end(args);
}

void PrintWork::printf_align_left(_In_z_ _Printf_format_string_ const char* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(app::TEXT_FLAG_ALIGN_FLAG_LEFT, fmt, args);
    va_end(args);
}

void PrintWork::wprintf(app::text_flags flags,
    _In_z_ _Printf_format_string_ const wchar_t* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwprintf(flags, fmt, args);
    va_end(args);
}

void PrintWork::wprintf_align_left(_In_z_ _Printf_format_string_ const wchar_t* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwprintf(app::TEXT_FLAG_ALIGN_FLAG_LEFT, fmt, args);
    va_end(args);
}

void PrintWork::vprintf(app::text_flags flags,
    _In_z_ _Printf_format_string_ const char* const fmt, va_list args) {
    std::string buf = vsprintf_s_string(fmt, args);
    PrintText(flags, buf.data(), buf.size());
}

void PrintWork::vwprintf(app::text_flags flags,
    _In_z_ _Printf_format_string_ const wchar_t* const fmt, va_list args) {
    std::wstring buf = vswprintf_s_string(fmt, args);
    PrintText(flags, buf.data(), buf.data() + buf.size());
}

void PrintWork::ClampPosToClipBox(float_t pos_min, float_t pos_max,
    float_t clip_box_min, float_t clip_box_max, float_t& clip_pos_min, float_t& clip_pos_max) {
    float_t v8 = max_def(pos_min, clip_box_min);
    float_t v10 = min_def(pos_max, clip_box_max);

    if (v8 < v10) {
        clip_pos_min = v8;
        clip_pos_max = v10;
    }
    else {
        clip_pos_min = pos_min;
        clip_pos_max = pos_min;
    }
}

vec2 PrintWork::sub_140197B80(app::text_flags flags, vec2 size) {
    vec2 pos = text_current_loc;
    if (flags & app::TEXT_FLAG_ALIGN_FLAG_RIGHT)
        pos.x -= size.x;
    else if (flags & app::TEXT_FLAG_ALIGN_FLAG_H_CENTER) {
        if (flags & app::TEXT_FLAG_CLIP)
            pos.x = clip_data.pos.x + clip_data.size.x * 0.5f;
        else {
            resolution_struct v10(resolution_mode);
            pos.x = (float_t)v10.width * 0.5f;
        }
        pos.x -= size.x * 0.5f;
    }
    else if (flags & app::TEXT_FLAG_ALIGN_FLAG_LOCATE_H_CENTER)
        pos.x -= size.x * 0.5f;

    if (flags & app::TEXT_FLAG_ALIGN_FLAG_V_CENTER) {
        if (flags & app::TEXT_FLAG_CLIP)
            pos.y = clip_data.pos.y + clip_data.size.y * 0.5f;
        else {
            resolution_struct v10(resolution_mode);
            pos.y = (float_t)v10.height * 0.5f;
        }
        pos.y -= size.y * 0.5f;
    }
    else if (flags & app::TEXT_FLAG_ALIGN_FLAG_LOCATE_V_CENTER)
        pos.y -= size.y * 0.5f;
    return pos;
}

int32_t PrintWork::sub_140197D60(rectangle clip_box, rectangle& pos, rectangle& uv) {
    vec2 pos_pos = pos.pos;
    vec2 pos_size = pos.size;
    vec2 uv_pos = uv.pos;
    vec2 uv_size = uv.size;

    vec2 clip_max_pos;
    vec2 clip_min_pos;
    PrintWork::ClampPosToClipBox(pos_pos.x, pos_pos.x + pos_size.x,
        clip_box.pos.x, clip_box.pos.x + clip_box.size.x, clip_min_pos.x, clip_max_pos.x);
    PrintWork::ClampPosToClipBox(pos_pos.y, pos_pos.y + pos_size.y,
        clip_box.pos.y, clip_box.pos.y + clip_box.size.y, clip_min_pos.y, clip_max_pos.y);

    vec2 clip_size = clip_max_pos - clip_min_pos;

    if (clip_size.x == pos_size.x && clip_size.y == pos_size.y)
        return 2;
    else if (clip_size.x <= 0.0f || clip_size.y <= 0.0f)
        return 0;

    vec2 uv_scale = uv_size / pos_size;
    pos.pos = clip_min_pos;
    pos.size = clip_size;
    uv.pos = (clip_min_pos - pos_pos) * uv_scale + uv_pos;
    uv.size = clip_size * uv_scale;
    return 1;
}

namespace app {
    namespace sap {
        ContentHandler::ContentHandler() {

        }

        ContentHandler:: ~ContentHandler() {

        }

        DefaultContentHandler::DefaultContentHandler() {

        }

        DefaultContentHandler::~DefaultContentHandler() {

        }

        void DefaultContentHandler::Print(const wchar_t* str_begin, const wchar_t* str_end) {

        }

        void DefaultContentHandler::Field_10() {

        }

        void DefaultContentHandler::NewLine() {

        }

        SimpleContentHandler::SimpleContentHandler(PrintWork* print_work, app::text_flags flags) {
            this->print_work = print_work;
            this->flags = flags;
        }

        SimpleContentHandler::~SimpleContentHandler() {

        }

        void SimpleContentHandler::Print(const wchar_t* str_begin, const wchar_t* str_end) {
            print_work->PutText(flags, str_begin, str_end);
        }

        void SimpleContentHandler::NewLine() {
            print_work->NewLine();
        }

        SimpleExtentContentHandler::SimpleExtentContentHandler(PrintWork* print_work) : new_line() {
            this->print_work = print_work;
        }

        SimpleExtentContentHandler::~SimpleExtentContentHandler() {

        }

        void SimpleExtentContentHandler::Print(const wchar_t* str_begin, const wchar_t* str_end) {
            if (new_line) {
                new_line = false;
                space_size.y = print_work->font->spacing.y + space_size.y;
            }

            vec2 str_size = print_work->GetStringSize(str_begin, str_end);
            size.x = str_size.x + size.x;
            size.y = max_def(str_size.y, size.y);
        }

        void SimpleExtentContentHandler::NewLine() {
            space_size.x = max_def(size.x, space_size.x);
            space_size.y += size.y;

            size = 0.0f;
            new_line = true;
        }

        Reader::Reader() {

        }

        Reader::~Reader() {

        }

        SimpleReader::SimpleReader(PrintWork* print_work, ContentHandler* content_handler) {
            this->print_work = print_work;
            this->content_handler = content_handler;
        }

        SimpleReader::~SimpleReader() {

        }

        void SimpleReader::Print(const wchar_t* str_begin, const wchar_t* str_end) {
            for (const wchar_t* i = str_begin, *j = str_begin; i != str_end; j = i) {
                size_t length = str_end - i;
                const wchar_t* new_line_ptr;
                if (length) {
                    new_line_ptr = i;
                    size_t len = length;
                    while (*new_line_ptr != '\n') {
                        new_line_ptr++;

                        if (!--len) {
                            new_line_ptr = 0;
                            break;
                        }
                    }
                }
                else
                    new_line_ptr = 0;

                const wchar_t* _str_begin;
                const wchar_t* _str_end;
                bool new_line = !!new_line_ptr;

                if (new_line) {
                    size_t _length = new_line_ptr - i;

                    _str_begin = i;
                    _str_end = i + _length;
                    if (i != i + _length) {
                        if (i[_length - 1] == '\r') {
                            if (_length <= 1)
                                _str_end = i;
                            else
                                _str_end = i + _length - 1;
                        }
                    }

                    if (_length + 1 < length)
                        i += _length + 1;
                    else
                        i = str_end;
                }
                else {
                    _str_begin = j;
                    _str_end = str_end;

                    // dafuq???
                    size_t v15 = -(int64_t)i >> 1;
                    if (v15 < length)
                        i += v15;
                    else
                        i = str_end;
                }

                content_handler->Print(_str_begin, _str_end);
                if (new_line)
                    content_handler->NewLine();
            }
        }

        void SimpleReader::SetContentHandler(ContentHandler* content_handler) {
            this->content_handler = content_handler;
        }

        ContentHandler* SimpleReader::GetContentHandler() {
            return content_handler;
        }

        SimpleAutoLineFeedReader::SimpleAutoLineFeedReader(PrintWork* print_work,
            ContentHandler* content_handler) : SimpleReader(print_work, content_handler) {

        }

        SimpleAutoLineFeedReader::~SimpleAutoLineFeedReader() {

        }

        void SimpleAutoLineFeedReader::Print(const wchar_t* str_begin, const wchar_t* str_end) {
            if (!str_begin || !str_end || !(str_end - str_begin))
                return;

            size_t line_length = print_work->line_length;
            for (const wchar_t* i = str_begin; i != str_end; ) {
                size_t length = str_end - i;
                if (line_length)
                    length = min_def(length, line_length);

                const wchar_t* new_line_ptr;
                if (length) {
                    new_line_ptr = i;
                    size_t len = length;
                    while (*new_line_ptr != '\n') {
                        new_line_ptr++;

                        if (!--len) {
                            new_line_ptr = 0;
                            break;
                        }
                    }
                }
                else
                    new_line_ptr = 0;

                const wchar_t* _str_begin;
                const wchar_t* _str_end;
                bool new_line = !!new_line_ptr;

                if (new_line) {
                    size_t _length = new_line_ptr - i;

                    _str_begin = i;
                    _str_end = i + _length;
                    if (i != i + _length) {
                        if (i[_length - 1] == '\r') {
                            if (_length > 1)
                                _str_end = &i[_length - 1];
                            else
                                _str_end = i;
                        }
                    }

                    if (_length + 1 < line_length)
                        i += _length + 1;
                    else
                        i = str_end;
                }
                else {
                    _str_begin = i;
                    _str_end = i + length;

                    if (length < line_length)
                        i += length;
                    else
                        i = str_end;

                    if (line_length && line_length <= length) {
                        new_line = true;
                        if (i != i + length && i[length - 1] == '\r' && i != str_end && *i == '\n') {
                            if (length > 1)
                                _str_end = &i[length - 1];
                            else
                                _str_end = i;

                            if (str_end - i > 1)
                                i++;
                            else
                                i = str_end;
                        }
                    }
                }

                content_handler->Print(_str_begin, _str_end);
                if (new_line)
                    content_handler->NewLine();
            }
        }
    }
}
