/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../str_utils.h"

static void light_param_fog_read_inner(light_param_fog* fog, stream* s);
static void light_param_fog_write_inner(light_param_fog* fog, stream* s);
static const char* light_param_fog_read_line(char* buf, int32_t size, const char* src);
static void light_param_fog_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value);
static void light_param_fog_write_float_t(stream* s, char* buf, size_t buf_size, float_t value);

light_param_fog::light_param_fog() : ready() {

}

light_param_fog::~light_param_fog() {

}

void light_param_fog::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        stream s;
        io_open(&s, path_txt, "rb");
        if (s.io.stream)
            light_param_fog_read_inner(this, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_fog::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        stream s;
        io_open(&s, path_txt, L"rb");
        if (s.io.stream)
            light_param_fog_read_inner(this, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_fog::read(const void* data, size_t length) {
    stream s;
    io_open(&s, data, length);
    light_param_fog_read_inner(this, &s);
    io_free(&s);
}

void light_param_fog::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    stream s;
    io_open(&s, path_txt, "wb");
    if (s.io.stream)
        light_param_fog_write_inner(this, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_fog::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    stream s;
    io_open(&s, path_txt, L"wb");
    if (s.io.stream)
        light_param_fog_write_inner(this, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_fog::write(void** data, size_t* length) {
    if (!data || !ready)
        return;

    stream s;
    io_open(&s);
    light_param_fog_write_inner(this, &s);
    io_copy(&s, data, length);
    io_free(&s);
}

bool light_param_fog::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    light_param_fog* fog = (light_param_fog*)data;
    fog->read(string_data(&s));

    string_free(&s);
    return fog->ready;
}

light_param_fog_group::light_param_fog_group() : has_type(), type(), has_density(),
density(), has_linear(), linear_start(), linear_end(), has_color(), color() {

}

light_param_fog_group::~light_param_fog_group() {

}

static void light_param_fog_read_inner(light_param_fog* fog, stream* s) {
    char* data = force_malloc_s(char, s->length + 1);
    io_read(s, data, s->length);
    data[s->length] = 0;

    char buf[0x100];
    const char* d = data;

    int32_t group_id = -1;
    while (d = light_param_fog_read_line(buf, sizeof(buf), d)) {
        light_param_fog_group* group = &fog->group[group_id];

        if (!str_utils_compare_length(buf, sizeof(buf), "group_start", 11)) {
            if (group_id != -1)
                goto End;

            int32_t index = 0;
            if (buf[11] != ' ' || sscanf_s(buf + 12, "%d", &index) != 1)
                goto End;

            group_id = index;
            continue;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "group_end", 9)) {
            if (group_id == -1)
                goto End;

            int32_t index = 0;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%d", &index) != 1)
                goto End;

            if (group_id != index)
                goto End;

            group_id = -1;
            continue;
        }
        else if (group_id == -1)
            continue;

        if (!str_utils_compare_length(buf, sizeof(buf), "type", 4)) {
            if (buf[4] != ' ' || sscanf_s(buf + 5, "%d", (int32_t*)&group->type) != 1)
                goto End;

            group->has_type = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "density", 7)) {
            if (buf[7] != ' ' || sscanf_s(buf + 8, "%f", &group->density) != 1)
                goto End;

            group->has_density = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "linear", 6)) {
            if (buf[6] != ' ' || sscanf_s(buf + 7, "%f %f",
                &group->linear_start, &group->linear_end) != 2)
                goto End;

            group->has_linear = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "color", 5)) {
            vec4u* color = &group->color;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f %f",
                &color->x, &color->y, &color->z, &color->w) != 4)
                goto End;

            group->has_color = true;
        }
    }

    free(data);
    fog->ready = true;
    return;

End:
    free(data);
}

static void light_param_fog_write_inner(light_param_fog* fog, stream* s) {
    char buf[0x100];

    for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++) {
        light_param_fog_group* group = &fog->group[i];
        io_write(s, "group_start", 11);
        light_param_fog_write_int32_t(s, buf, sizeof(buf), i);
        io_write_char(s, '\n');

        if (group->has_type) {
            io_write(s, "type", 4);
            light_param_fog_write_int32_t(s, buf, sizeof(buf), (int32_t)group->type);
            io_write_char(s, '\n');
        }

        if (group->has_density) {
            io_write(s, "density", 7);
            light_param_fog_write_float_t(s, buf, sizeof(buf), group->density);
            io_write_char(s, '\n');
        }

        if (group->has_linear) {
            io_write(s, "linear", 6);
            light_param_fog_write_float_t(s, buf, sizeof(buf), group->linear_start);
            light_param_fog_write_float_t(s, buf, sizeof(buf), group->linear_end);
            io_write_char(s, '\n');
        }

        if (group->has_color) {
            vec4u* color = &group->color;
            io_write(s, "color", 5);
            light_param_fog_write_float_t(s, buf, sizeof(buf), color->x);
            light_param_fog_write_float_t(s, buf, sizeof(buf), color->y);
            light_param_fog_write_float_t(s, buf, sizeof(buf), color->z);
            light_param_fog_write_float_t(s, buf, sizeof(buf), color->w);
            io_write_char(s, '\n');
        }

        io_write(s, "group_end", 9);
        light_param_fog_write_int32_t(s, buf, sizeof(buf), i);
        io_write_char(s, '\n');
    }

    io_write(s, "EOF", 3);
    io_write_char(s, '\n');
}

static const char* light_param_fog_read_line(char* buf, int32_t size, const char* src) {
    char* b = buf;
    if (!src || !*src)
        return 0;

    for (int32_t i = 0; i < size - 1; i++, b++) {
        char c = *b = *src++;
        if (!c) {
            b++;
            break;
        }
        else if (c == '\n') {
            *b++ = 0;
            break;
        }
        else if (c == '\r' && *src == '\n') {
            *b++ = 0;
            src++;
            break;
        }
    }

    if (!str_utils_compare(buf, "EOF"))
        return 0;
    return src;
}

inline static void light_param_fog_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    io_write_utf8_string(s, buf);
}

inline static void light_param_fog_write_float_t(stream* s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    io_write_utf8_string(s, buf);
}
