/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "ripple.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void stage_param_ripple_read_inner(stage_param_ripple* ripple, stream& s);
static void stage_param_ripple_write_inner(stage_param_ripple* ripple, stream& s);
static const char* stage_param_ripple_read_line(char* buf, int32_t size, const char* src);
static void stage_param_ripple_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
static void stage_param_ripple_write_size_t(stream& s, char* buf, size_t buf_size, size_t value);
static void stage_param_ripple_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);
static void stage_param_ripple_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value);

stage_param_ripple::stage_param_ripple() : ready(), rain_ripple_num(), rain_ripple_min_value(),
rain_ripple_max_value(), ground_y(), emit_pos_scale(), emit_pos_ofs_x(), emit_pos_ofs_z(),
wake_attn(), speed(), use_float_ripplemap(), rob_emitter_size(), emitter_num(), emitter_size() {

}

stage_param_ripple::~stage_param_ripple() {

}

void stage_param_ripple::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            stage_param_ripple_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_ripple::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            stage_param_ripple_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_ripple::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    stage_param_ripple_read_inner(this, s);
}

void stage_param_ripple::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        stage_param_ripple_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_ripple::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        stage_param_ripple_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_ripple::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    stage_param_ripple_write_inner(this, s);
    s.copy(data, size);
}

bool stage_param_ripple::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    stage_param_ripple* ripple = (stage_param_ripple*)data;
    ripple->read(s.c_str());

    return ripple->ready;
}

static void stage_param_ripple_read_inner(stage_param_ripple* ripple, stream& s) {
    char* data = force_malloc_s(char, s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    while (d = stage_param_ripple_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "rain_ripple_num", 15)) {
            if (buf[15] != ' ' || sscanf_s(buf + 16, "%llu", &ripple->rain_ripple_num) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "rain_ripple_min_value", 21)) {
            if (buf[21] != ' ' || sscanf_s(buf + 22, "%f", &ripple->rain_ripple_min_value) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "rain_ripple_max_value", 21)) {
            if (buf[21] != ' ' || sscanf_s(buf + 22, "%f", &ripple->rain_ripple_max_value) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "ground_y", 8)) {
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f", &ripple->ground_y) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emit_pos_scale", 14)) {
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%f", &ripple->emit_pos_scale) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emit_pos_ofs_x", 14)) {
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%f", &ripple->emit_pos_ofs_x) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emit_pos_ofs_z", 14)) {
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%f", &ripple->emit_pos_ofs_z) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "wake_attn", 9)) {
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f", &ripple->wake_attn) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "speed", 5)) {
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f", &ripple->speed) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "ripple_tex_name", 15)) {
            if (buf[15] != ' ')
                goto End;

            ripple->ripple_tex_name.assign(buf + 16);
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "use_float_ripplemap", 19)) {
            int32_t use_float_ripplemap = 0;
            if (buf[19] != ' ' || sscanf_s(buf + 20, "%d", &use_float_ripplemap) != 1)
                goto End;

            ripple->use_float_ripplemap = !!use_float_ripplemap;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "rob_emitter_size", 16)) {
            if (buf[16] != ' ' || sscanf_s(buf + 17, "%f", &ripple->rob_emitter_size) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emitter_size", 12)) {
            if (buf[12] != ' ' || sscanf_s(buf + 13, "%f", &ripple->emitter_size) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emitter_num", 11)) {
            if (buf[11] != ' ' || sscanf_s(buf + 12, "%llu", &ripple->emitter_num) != 1)
                goto End;

            ripple->emitter_list.resize(ripple->emitter_num);
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emitter_list", 12)) {
            size_t index = 0;
            vec3 pos;
            if (buf[12] != ' ' || sscanf_s(buf + 13, "%llu %f %f %f",
                &index, &pos.x, &pos.y, &pos.z) != 4)
                goto End;

            if (index >= ripple->emitter_num)
                goto End;

            ripple->emitter_list[index] = pos;
        }
    }

    free_def(data);
    ripple->ready = true;
    return;

End:
    free_def(data);
}

static void stage_param_ripple_write_inner(stage_param_ripple* ripple, stream& s) {
    char buf[0x200];

    size_t emitter_num = ripple->emitter_list.size();

    s.write("rain_ripple_num", 15);
    stage_param_ripple_write_size_t(s, buf, sizeof(buf), ripple->rain_ripple_num);
    s.write_char('\n');

    s.write("rain_ripple_min_value", 21);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->rain_ripple_min_value);
    s.write_char('\n');

    s.write("rain_ripple_max_value", 21);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->rain_ripple_max_value);
    s.write_char('\n');

    s.write("ground_y", 8);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->ground_y);
    s.write_char('\n');

    s.write("emit_pos_scale", 14);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->emit_pos_scale);
    s.write_char('\n');

    s.write("emit_pos_ofs_x", 14);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->emit_pos_ofs_x);
    s.write_char('\n');

    s.write("emit_pos_ofs_z", 14);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->emit_pos_ofs_z);
    s.write_char('\n');

    s.write("wake_attn", 9);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->wake_attn);
    s.write_char('\n');

    s.write("speed", 5);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->speed);
    s.write_char('\n');

    s.write("ripple_tex_name", 15);
    stage_param_ripple_write_string(s, buf, sizeof(buf), ripple->ripple_tex_name);
    s.write_char('\n');

    s.write("use_float_ripplemap", 19);
    stage_param_ripple_write_int32_t(s, buf, sizeof(buf), ripple->use_float_ripplemap ? 1 : 0);
    s.write_char('\n');

    s.write("rob_emitter_size", 16);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->rob_emitter_size);
    s.write_char('\n');

    s.write("emitter_size", 12);
    stage_param_ripple_write_float_t(s, buf, sizeof(buf), ripple->emitter_size);
    s.write_char('\n');

    s.write("emitter_num", 11);
    stage_param_ripple_write_size_t(s, buf, sizeof(buf), emitter_num);
    s.write_char('\n');

    vec3* emitter_list = ripple->emitter_list.data();
    for (int32_t i = 0; i < emitter_num; i++, emitter_list++) {
        s.write("emitter_list", 12);
        stage_param_ripple_write_size_t(s, buf, sizeof(buf), i);
        stage_param_ripple_write_float_t(s, buf, sizeof(buf), emitter_list->x);
        stage_param_ripple_write_float_t(s, buf, sizeof(buf), emitter_list->y);
        stage_param_ripple_write_float_t(s, buf, sizeof(buf), emitter_list->z);
        s.write_char('\n');
    }
}

static const char* stage_param_ripple_read_line(char* buf, int32_t size, const char* src) {
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

inline static void stage_param_ripple_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_ripple_write_size_t(stream& s, char* buf, size_t buf_size, size_t value) {
    sprintf_s(buf, buf_size, " %llu", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_ripple_write_float_t(stream& s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_ripple_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value) {
    s.write_char(' ');
    s.write_string(value);
}
