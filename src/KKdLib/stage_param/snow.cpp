/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "snow.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void stage_param_snow_read_inner(stage_param_snow* snow, stream& s);
static void stage_param_snow_write_inner(stage_param_snow* snow, stream& s);
static const char* stage_param_snow_read_line(char* buf, int32_t size, const char* src);
static void stage_param_snow_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
static void stage_param_snow_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);
static void stage_param_snow_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value);

stage_param_snow_colli_ground::stage_param_snow_colli_ground()
    : min_x(), max_x(), min_z(), max_z(), y() {

}

stage_param_snow::stage_param_snow() : ready(), num_snow(), num_snow_gpu() {

}

stage_param_snow::~stage_param_snow() {

}

void stage_param_snow::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            stage_param_snow_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_snow::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            stage_param_snow_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_snow::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    stage_param_snow_read_inner(this, s);
}

void stage_param_snow::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        stage_param_snow_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_snow::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        stage_param_snow_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_snow::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    stage_param_snow_write_inner(this, s);
    s.copy(data, size);
}

bool stage_param_snow::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    stage_param_snow* snow = (stage_param_snow*)data;
    snow->read(s.c_str());

    return snow->ready;
}

static void stage_param_snow_read_inner(stage_param_snow* snow, stream& s) {
    char* data = force_malloc_s(char, s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    while (d = stage_param_snow_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "tex_name", 8)) {
            if (buf[8] != ' ')
                goto End;

            snow->tex_name.assign(buf + 23);
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "num_snow", 8)) {
            if (!str_utils_compare_length(buf, sizeof(buf), "num_snow_gpu", 12)) {
                if (buf[12] != ' ' || sscanf_s(buf + 13, "%d", &snow->num_snow_gpu) != 1)
                    goto End;
            }
            else {
                if (buf[8] != ' ' || sscanf_s(buf + 9, "%d", &snow->num_snow) != 1)
                    goto End;
            }
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "color", 5)) {
            vec4& color = snow->color;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f %f",
                &color.x, &color.y, &color.z, &color.w) != 4)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "velocity", 8)) {
            vec3& velocity = snow->velocity;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f",
                &velocity.x, &velocity.y, &velocity.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "vel_range", 9)) {
            vec3& vel_range = snow->vel_range;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f %f %f",
                &vel_range.x, &vel_range.y, &vel_range.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "range", 5)) {
            if (!str_utils_compare_length(buf, sizeof(buf), "range_gpu", 9)) {
                vec3& range_gpu = snow->range_gpu;
                if (buf[9] != ' ' || sscanf_s(buf + 10, "%f %f %f",
                    &range_gpu.x, &range_gpu.y, &range_gpu.z) != 3)
                    goto End;
            }
            else {
                vec3& range = snow->range;
                if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f",
                    &range.x, &range.y, &range.z) != 3)
                    goto End;
            }
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "offset", 6)) {
            if (!str_utils_compare_length(buf, sizeof(buf), "offset_gpu", 10)) {
                vec3& offset_gpu = snow->offset_gpu;
                if (buf[10] != ' ' || sscanf_s(buf + 11, "%f %f %f",
                    &offset_gpu.x, &offset_gpu.y, &offset_gpu.z) != 3)
                    goto End;
            }
            else {
                vec3& offset = snow->offset;
                if (buf[6] != ' ' || sscanf_s(buf + 7, "%f %f %f",
                    &offset.x, &offset.y, &offset.z) != 3)
                    goto End;
            }
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "colli_ground", 12)) {
            stage_param_snow_colli_ground& colli_ground = snow->colli_ground;
            if (buf[12] != ' ' || sscanf_s(buf + 13, "%f %f %f %f %f",
                &colli_ground.min_x, &colli_ground.max_x,
                &colli_ground.min_z, &colli_ground.max_z, &colli_ground.y) != 5)
                goto End;
        }
    }

    free_def(data);
    snow->ready = true;
    return;

End:
    free_def(data);
}

static void stage_param_snow_write_inner(stage_param_snow* snow, stream& s) {
    char buf[0x200];

    s.write("tex_name", 8);
    stage_param_snow_write_string(s, buf, sizeof(buf), snow->tex_name);
    s.write_char('\n');

    s.write("num_snow", 8);
    stage_param_snow_write_int32_t(s, buf, sizeof(buf), snow->num_snow);
    s.write_char('\n');

    s.write("num_snow_gpu", 12);
    stage_param_snow_write_int32_t(s, buf, sizeof(buf), snow->num_snow_gpu);
    s.write_char('\n');

    vec4& color = snow->color;
    s.write("color", 5);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), color.x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), color.y);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), color.z);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), color.w);
    s.write_char('\n');

    vec3& velocity = snow->velocity;
    s.write("velocity", 8);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), velocity.x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), velocity.y);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), velocity.z);
    s.write_char('\n');

    vec3& vel_range = snow->vel_range;
    s.write("vel_range", 9);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), vel_range.x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), vel_range.y);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), vel_range.z);
    s.write_char('\n');

    vec3& range = snow->range;
    s.write("range", 5);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), range.x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), range.y);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), range.z);
    s.write_char('\n');

    vec3& range_gpu = snow->range_gpu;
    s.write("range_gpu", 9);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), range_gpu.x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), range_gpu.y);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), range_gpu.z);
    s.write_char('\n');

    vec3& offset = snow->offset;
    s.write("offset", 6);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), offset.x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), offset.y);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), offset.z);
    s.write_char('\n');

    vec3& offset_gpu = snow->offset_gpu;
    s.write("offset_gpu", 10);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), offset_gpu.x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), offset_gpu.y);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), offset_gpu.z);
    s.write_char('\n');

    stage_param_snow_colli_ground& colli_ground = snow->colli_ground;
    s.write("colli_ground", 12);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), colli_ground.min_x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), colli_ground.max_x);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), colli_ground.min_z);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), colli_ground.max_z);
    stage_param_snow_write_float_t(s, buf, sizeof(buf), colli_ground.y);
    s.write_char('\n');
}

static const char* stage_param_snow_read_line(char* buf, int32_t size, const char* src) {
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

inline static void stage_param_snow_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_snow_write_float_t(stream& s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_snow_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value) {
    s.write_char(' ');
    s.write_string(value);
}
