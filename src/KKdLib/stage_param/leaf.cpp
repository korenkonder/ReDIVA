/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "leaf.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void stage_param_leaf_read_inner(stage_param_leaf* leaf, stream& s);
static void stage_param_leaf_write_inner(stage_param_leaf* leaf, stream& s);
static const char* stage_param_leaf_read_line(char* buf, int32_t size, const char* src);
static void stage_param_leaf_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
static void stage_param_leaf_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);
static void stage_param_leaf_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value);

stage_param_data_leaf_lie_plane_xz::stage_param_data_leaf_lie_plane_xz() : min_x(), max_x(), min_z(), max_z() {

}

stage_param_leaf::stage_param_leaf() : ready(), psize(),
num_initial_ptcls(), frame_speed_coef(), emit_interval(), split_tex() {

}

stage_param_leaf::~stage_param_leaf() {

}

void stage_param_leaf::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            stage_param_leaf_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_leaf::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            stage_param_leaf_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_leaf::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    stage_param_leaf_read_inner(this, s);
}

void stage_param_leaf::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        stage_param_leaf_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_leaf::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        stage_param_leaf_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_leaf::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    stage_param_leaf_write_inner(this, s);
    s.copy(data, size);
}

bool stage_param_leaf::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    stage_param_leaf* leaf = (stage_param_leaf*)data;
    leaf->read(s.c_str());

    return leaf->ready;
}

static void stage_param_leaf_read_inner(stage_param_leaf* leaf, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    int32_t group_id = -1;
    while (d = stage_param_leaf_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "tex_name", 8)) {
            if (buf[8] != ' ')
                goto End;

            leaf->tex_name.assign(buf + 9);
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "color", 5)) {
            vec4& color = leaf->color;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f %f",
                &color.x, &color.y, &color.z, &color.w) != 4)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "psize", 5)) {
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f", &leaf->psize) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "num_initial_ptcls", 17)) {
            if (buf[17] != ' ' || sscanf_s(buf + 18, "%d", &leaf->num_initial_ptcls) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "frame_speed_coef", 16)) {
            if (buf[16] != ' ' || sscanf_s(buf + 17, "%f", &leaf->frame_speed_coef) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "emit_interval", 13)) {
            if (buf[13] != ' ' || sscanf_s(buf + 14, "%f", &leaf->emit_interval) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "wind", 4)) {
            vec3& wind = leaf->wind;
            if (buf[4] != ' ' || sscanf_s(buf + 5, "%f %f %f",
                &wind.x, &wind.y, &wind.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "range", 5)) {
            vec3& range = leaf->range;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f",
                &range.x, &range.y, &range.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "offset", 6)) {
            vec3& offset = leaf->offset;
            if (buf[6] != ' ' || sscanf_s(buf + 7, "%f %f %f",
                &offset.x, &offset.y, &offset.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "lie_plane_xz", 12)) {
            stage_param_data_leaf_lie_plane_xz& lie_plane_xz = leaf->lie_plane_xz;
            if (buf[12] != ' ' || sscanf_s(buf + 13, "%f %f %f %f",
                &lie_plane_xz.min_x, &lie_plane_xz.max_x, &lie_plane_xz.min_z, &lie_plane_xz.max_z) != 4)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "split_tex", 9)) {
            int32_t split_tex = 0;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%d", &split_tex) != 1)
                goto End;

            leaf->split_tex = !!split_tex;
        }
    }

    free_def(data);
    leaf->ready = true;
    return;

End:
    free_def(data);
}

static void stage_param_leaf_write_inner(stage_param_leaf* leaf, stream& s) {
    char buf[0x200];

    s.write("tex_name", 8);
    stage_param_leaf_write_string(s, buf, sizeof(buf), leaf->tex_name);
    s.write_char('\n');

    vec4& color = leaf->color;
    s.write("color", 5);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), color.x);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), color.y);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), color.z);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), color.w);
    s.write_char('\n');

    s.write("psize", 5);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), leaf->psize);
    s.write_char('\n');

    s.write("num_initial_ptcls", 17);
    stage_param_leaf_write_int32_t(s, buf, sizeof(buf), leaf->num_initial_ptcls);
    s.write_char('\n');

    s.write("frame_speed_coef", 16);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), leaf->frame_speed_coef);
    s.write_char('\n');

    s.write("emit_interval", 13);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), leaf->emit_interval);
    s.write_char('\n');

    vec3& wind = leaf->wind;
    s.write("wind", 4);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), wind.x);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), wind.y);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), wind.z);
    s.write_char('\n');

    vec3& range = leaf->range;
    s.write("range", 5);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), range.x);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), range.y);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), range.z);
    s.write_char('\n');

    vec3& offset = leaf->offset;
    s.write("offset", 6);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), offset.x);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), offset.y);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), offset.z);
    s.write_char('\n');

    stage_param_data_leaf_lie_plane_xz& lie_plane_xz = leaf->lie_plane_xz;
    s.write("lie_plane_xz", 12);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), lie_plane_xz.min_x);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), lie_plane_xz.max_x);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), lie_plane_xz.min_z);
    stage_param_leaf_write_float_t(s, buf, sizeof(buf), lie_plane_xz.max_z);
    s.write_char('\n');

    s.write("split_tex", 9);
    stage_param_leaf_write_int32_t(s, buf, sizeof(buf), leaf->split_tex ? 1 : 0);
    s.write_char('\n');
}

static const char* stage_param_leaf_read_line(char* buf, int32_t size, const char* src) {
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
    return src;
}

inline static void stage_param_leaf_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_leaf_write_float_t(stream& s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_leaf_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value) {
    s.write_char(' ');
    s.write_string(value);
}
