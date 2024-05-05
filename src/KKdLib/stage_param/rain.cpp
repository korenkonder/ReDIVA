/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rain.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void stage_param_rain_read_inner(stage_param_rain* rain, stream& s);
static void stage_param_rain_write_inner(stage_param_rain* rain, stream& s);
static const char* stage_param_rain_read_line(char* buf, int32_t size, const char* src);
static void stage_param_rain_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
static void stage_param_rain_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);
static void stage_param_rain_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value);

stage_param_rain::stage_param_rain() : ready(), num_rain() {

}

stage_param_rain::~stage_param_rain() {

}

void stage_param_rain::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            stage_param_rain_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_rain::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            stage_param_rain_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_rain::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    stage_param_rain_read_inner(this, s);
}

void stage_param_rain::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        stage_param_rain_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_rain::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        stage_param_rain_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_rain::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    stage_param_rain_write_inner(this, s);
    s.copy(data, size);
}

bool stage_param_rain::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    stage_param_rain* rain = (stage_param_rain*)data;
    rain->read(path.c_str());

    return rain->ready;
}

static void stage_param_rain_read_inner(stage_param_rain* rain, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    while (d = stage_param_rain_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "tex_name", 8)) {
            if (buf[8] != ' ')
                goto End;

            rain->tex_name.assign(buf + 9);
        }
        if (!str_utils_compare_length(buf, sizeof(buf), "num_rain", 8)) {
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%d", &rain->num_rain) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "color", 5)) {
            vec4& color = rain->color;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f %f",
                &color.x, &color.y, &color.z, &color.w) != 4)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "velocity", 8)) {
            vec3& velocity = rain->velocity;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f",
                &velocity.x, &velocity.y, &velocity.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "psize", 5)) {
            vec2& psize = rain->psize;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f",
                &psize.x, &psize.y) != 2)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "range", 5)) {
            vec3& range = rain->range;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f",
                &range.x, &range.y, &range.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "offset", 6)) {
            vec3& offset = rain->offset;
            if (buf[6] != ' ' || sscanf_s(buf + 7, "%f %f %f",
                &offset.x, &offset.y, &offset.z) != 3)
                goto End;
        }
    }

    free_def(data);
    rain->ready = true;
    return;

End:
    free_def(data);
}

static void stage_param_rain_write_inner(stage_param_rain* rain, stream& s) {
    char buf[0x200];

    s.write("tex_name", 8);
    stage_param_rain_write_string(s, buf, sizeof(buf), rain->tex_name);
    s.write_char('\n');

    s.write("num_rain", 8);
    stage_param_rain_write_int32_t(s, buf, sizeof(buf), rain->num_rain);
    s.write_char('\n');

    vec4& color = rain->color;
    s.write("color", 5);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), color.x);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), color.y);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), color.z);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), color.w);
    s.write_char('\n');

    vec3& velocity = rain->velocity;
    s.write("velocity", 8);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), velocity.x);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), velocity.y);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), velocity.z);
    s.write_char('\n');

    vec3& vel_range = rain->vel_range;
    s.write("vel_range", 9);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), vel_range.x);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), vel_range.y);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), vel_range.z);
    s.write_char('\n');

    vec2& psize = rain->psize;
    s.write("psize", 5);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), psize.x);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), psize.y);
    s.write_char('\n');

    vec3& range = rain->range;
    s.write("range", 5);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), range.x);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), range.y);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), range.z);
    s.write_char('\n');

    vec3& offset = rain->offset;
    s.write("offset", 6);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), offset.x);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), offset.y);
    stage_param_rain_write_float_t(s, buf, sizeof(buf), offset.z);
    s.write_char('\n');
}

static const char* stage_param_rain_read_line(char* buf, int32_t size, const char* src) {
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
    return src;
}

inline static void stage_param_rain_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_rain_write_float_t(stream& s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_rain_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value) {
    s.write_char(' ');
    s.write_string(value);
}
