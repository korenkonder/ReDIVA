/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "face.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../str_utils.h"

static void light_param_face_read_inner(light_param_face* face, stream* s);
static void light_param_face_write_inner(light_param_face* face, stream* s);
static char* light_param_face_read_line(char* buf, int32_t size, char* src);
static void light_param_face_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value);
static void light_param_face_write_float_t(stream* s, char* buf, size_t buf_size, float_t value);

light_param_face::light_param_face() : ready(), offset(), scale(), position(), direction() {

}

void light_param_face::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        stream s;
        io_open(&s, path_txt, "rb");
        if (s.io.stream)
            light_param_face_read_inner(this, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_face::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_wadd(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        stream s;
        io_open(&s, path_txt, L"rb");
        if (s.io.stream)
            light_param_face_read_inner(this, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_face::read(const void* data, size_t length) {
    stream s;
    io_open(&s, data, length);
    light_param_face_read_inner(this, &s);
    io_free(&s);
}

void light_param_face::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    stream s;
    io_open(&s, path_txt, "wb");
    if (s.io.stream)
        light_param_face_write_inner(this, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_face::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_wadd(path, L".txt");
    stream s;
    io_open(&s, path_txt, L"wb");
    if (s.io.stream)
        light_param_face_write_inner(this, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_face::write(void** data, size_t* length) {
    if (!data || !ready)
        return;

    stream s;
    io_open(&s);
    light_param_face_write_inner(this, &s);
    io_copy(&s, data, length);
    io_free(&s);
}

light_param_face::~light_param_face() {

}

bool light_param_face::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    light_param_face* face = (light_param_face*)data;
    face->read(string_data(&s));

    string_free(&s);
    return face->ready;
}

static void light_param_face_read_inner(light_param_face* face, stream* s) {
    char* data = force_malloc_s(char, s->length + 1);
    io_read(s, data, s->length);
    data[s->length] = 0;

    char buf[0x100];
    char* d = data;

    while (d = light_param_face_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "offset", 6)) {
            if (buf[6] != ' ' || sscanf_s(buf + 7, "%f", &face->offset) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "scale", 5)) {
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f", &face->scale) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "position", 8)) {
            vec3* position = &face->position;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f",
                &position->x, &position->y, &position->z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "direction", 10)) {
            vec3* direction = &face->direction;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f %f %f",
                &direction->x, &direction->y, &direction->z) != 3)
                goto End;
        }
    }

    free(data);
    face->ready = true;
    return;

End:
    free(data);
}

static void light_param_face_write_inner(light_param_face* face, stream* s) {
    char buf[0x100];

    io_write(s, "offset", 6);
    light_param_face_write_float_t(s, buf, sizeof(buf), face->offset);
    io_write_char(s, '\n');

    io_write(s, "scale", 5);
    light_param_face_write_float_t(s, buf, sizeof(buf), face->scale);
    io_write_char(s, '\n');

    vec3* position = &face->position;
    io_write(s, "position", 8);
    light_param_face_write_float_t(s, buf, sizeof(buf), position->x);
    light_param_face_write_float_t(s, buf, sizeof(buf), position->y);
    light_param_face_write_float_t(s, buf, sizeof(buf), position->z);
    io_write_char(s, '\n');

    vec3* direction = &face->direction;
    io_write(s, "direction", 9);
    light_param_face_write_float_t(s, buf, sizeof(buf), direction->x);
    light_param_face_write_float_t(s, buf, sizeof(buf), direction->y);
    light_param_face_write_float_t(s, buf, sizeof(buf), direction->z);
    io_write_char(s, '\n');

    io_write(s, "EOF", 3);
    io_write_char(s, '\n');
}

static char* light_param_face_read_line(char* buf, int32_t size, char* src) {
    char* b = buf;
    if (!src || !*src)
        return 0;

    for (int32_t i = 0; i < size - 1; i++, b++) {
        char c = b[0] = *src++;
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

inline static void light_param_face_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    io_write_utf8_string(s, buf);
}

inline static void light_param_face_write_float_t(stream* s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %g", value);
    io_write_utf8_string(s, buf);
}
