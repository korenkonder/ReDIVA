/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "face.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void light_param_face_read_inner(light_param_face* face, stream& s);
static void light_param_face_write_inner(light_param_face* face, stream& s);
static const char* light_param_face_read_line(char* buf, int32_t size, const char* src);
static void light_param_face_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
static void light_param_face_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);

light_param_face::light_param_face() : ready(), offset(), scale(), position(), direction() {

}

light_param_face::~light_param_face() {

}

void light_param_face::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            light_param_face_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_face::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            light_param_face_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_face::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    light_param_face_read_inner(this, s);
}

void light_param_face::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        light_param_face_write_inner(this, s);
    free_def(path_txt);
}

void light_param_face::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        light_param_face_write_inner(this, s);
    free_def(path_txt);
}

void light_param_face::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    light_param_face_write_inner(this, s);
    s.copy(data, size);
}

bool light_param_face::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    light_param_face* face = (light_param_face*)data;
    face->read(s.c_str());

    return face->ready;
}

static void light_param_face_read_inner(light_param_face* face, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

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
            vec3& position = face->position;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f",
                &position.x, &position.y, &position.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "direction", 10)) {
            vec3& direction = face->direction;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f %f %f",
                &direction.x, &direction.y, &direction.z) != 3)
                goto End;
        }
    }

    free_def(data);
    face->ready = true;
    return;

End:
    free_def(data);
}

static void light_param_face_write_inner(light_param_face* face, stream& s) {
    char buf[0x200];

    s.write("offset", 6);
    light_param_face_write_float_t(s, buf, sizeof(buf), face->offset);
    s.write_char('\n');

    s.write("scale", 5);
    light_param_face_write_float_t(s, buf, sizeof(buf), face->scale);
    s.write_char('\n');

    vec3& position = face->position;
    s.write("position", 8);
    light_param_face_write_float_t(s, buf, sizeof(buf), position.x);
    light_param_face_write_float_t(s, buf, sizeof(buf), position.y);
    light_param_face_write_float_t(s, buf, sizeof(buf), position.z);
    s.write_char('\n');

    vec3& direction = face->direction;
    s.write("direction", 9);
    light_param_face_write_float_t(s, buf, sizeof(buf), direction.x);
    light_param_face_write_float_t(s, buf, sizeof(buf), direction.y);
    light_param_face_write_float_t(s, buf, sizeof(buf), direction.z);
    s.write_char('\n');

    s.write("EOF", 3);
    s.write_char('\n');
}

static const char* light_param_face_read_line(char* buf, int32_t size, const char* src) {
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

inline static void light_param_face_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    s.write_utf8_string(buf);
}

inline static void light_param_face_write_float_t(stream& s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    s.write_utf8_string(buf);
}
