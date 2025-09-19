/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "splash.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void stage_param_splash_read_inner(stage_param_splash* splash, stream& s);
static void stage_param_splash_write_inner(stage_param_splash* splash, stream& s);
static const char* stage_param_splash_read_line(char* buf, int32_t size, const char* src);
static void stage_param_splash_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
static void stage_param_splash_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);
static void stage_param_splash_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value);

stage_param_splash::stage_param_splash() : ready(), particle_size(), emit_num(),
emission_ratio_attn(), emission_velocity_scale(), ripple_emission(), in_water(), blink() {

}

stage_param_splash::~stage_param_splash() {

}

void stage_param_splash::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            stage_param_splash_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_splash::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            stage_param_splash_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_splash::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    stage_param_splash_read_inner(this, s);
}

void stage_param_splash::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        stage_param_splash_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_splash::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        stage_param_splash_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_splash::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    stage_param_splash_write_inner(this, s);
    s.copy(data, size);
}

bool stage_param_splash::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    stage_param_splash* splash = (stage_param_splash*)data;
    splash->read(path.c_str());

    return splash->ready;
}

static void stage_param_splash_read_inner(stage_param_splash* splash, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    while (d = stage_param_splash_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "splash_tex_name", 15)) {
            if (buf[15] != ' ')
                goto End;

            splash->splash_tex_name.assign(buf + 16);
        }
        if (!str_utils_compare_length(buf, sizeof(buf), "splash_obj_name", 15)) {
            if (buf[15] != ' ')
                goto End;

            splash->splash_obj_name.assign(buf + 16);
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "particle_size", 13)) {
            if (buf[13] != ' ' || sscanf_s(buf + 14, "%f", &splash->particle_size) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emit_num", 8)) {
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%d", &splash->emit_num) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emission_ratio_attn", 19)) {
            if (buf[19] != ' ' || sscanf_s(buf + 20, "%f", &splash->emission_ratio_attn) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emission_velocity_scale", 23)) {
            if (buf[23] != ' ' || sscanf_s(buf + 24, "%f", &splash->emission_velocity_scale) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "ripple_emission", 15)) {
            if (buf[15] != ' ' || sscanf_s(buf + 16, "%f", &splash->ripple_emission) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "color", 5)) {
            vec4& color = splash->color;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f %f",
                &color.x, &color.y, &color.z, &color.w) != 4)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "in_water", 8)) {
            int32_t in_water = 0;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%d", &in_water) != 1)
                goto End;

            splash->in_water = !!in_water;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "blink", 5)) {
            int32_t blink = 0;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%d", &blink) != 1)
                goto End;

            splash->blink = !!blink;
        }
    }

    free_def(data);
    splash->ready = true;
    return;

End:
    free_def(data);
}

static void stage_param_splash_write_inner(stage_param_splash* splash, stream& s) {
    char buf[0x200];

    s.write("splash_tex_name", 15);
    stage_param_splash_write_string(s, buf, sizeof(buf), splash->splash_tex_name);
    s.write_char('\n');

    s.write("splash_obj_name", 15);
    stage_param_splash_write_string(s, buf, sizeof(buf), splash->splash_obj_name);
    s.write_char('\n');

    s.write("particle_size", 13);
    stage_param_splash_write_float_t(s, buf, sizeof(buf), splash->particle_size);
    s.write_char('\n');

    s.write("emit_num", 8);
    stage_param_splash_write_int32_t(s, buf, sizeof(buf), splash->emit_num);
    s.write_char('\n');

    s.write("emission_ratio_attn", 19);
    stage_param_splash_write_float_t(s, buf, sizeof(buf), splash->emission_ratio_attn);
    s.write_char('\n');

    s.write("emission_velocity_scale", 23);
    stage_param_splash_write_float_t(s, buf, sizeof(buf), splash->emission_velocity_scale);
    s.write_char('\n');

    s.write("ripple_emission", 15);
    stage_param_splash_write_float_t(s, buf, sizeof(buf), splash->ripple_emission);
    s.write_char('\n');

    vec4& color = splash->color;
    s.write("color", 5);
    stage_param_splash_write_float_t(s, buf, sizeof(buf), color.x);
    stage_param_splash_write_float_t(s, buf, sizeof(buf), color.y);
    stage_param_splash_write_float_t(s, buf, sizeof(buf), color.z);
    stage_param_splash_write_float_t(s, buf, sizeof(buf), color.w);
    s.write_char('\n');

    s.write("in_water", 8);
    stage_param_splash_write_int32_t(s, buf, sizeof(buf), splash->in_water ? 1 : 0);
    s.write_char('\n');

    s.write("blink", 5);
    stage_param_splash_write_int32_t(s, buf, sizeof(buf), splash->blink ? 1 : 0);
    s.write_char('\n');
}

static const char* stage_param_splash_read_line(char* buf, int32_t size, const char* src) {
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

inline static void stage_param_splash_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_splash_write_float_t(stream& s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_splash_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value) {
    s.write_char(' ');
    s.write_string(value);
}
