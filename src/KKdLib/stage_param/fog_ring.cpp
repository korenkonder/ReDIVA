/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog_ring.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void stage_param_fog_ring_read_inner(stage_param_fog_ring* fog_ring, stream& s);
static void stage_param_fog_ring_write_inner(stage_param_fog_ring* fog_ring, stream& s);
static const char* stage_param_fog_ring_read_line(char* buf, int32_t size, const char* src);
static void stage_param_fog_ring_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
static void stage_param_fog_ring_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);
static void stage_param_fog_ring_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value);

stage_param_fog_ring::stage_param_fog_ring() : ready(),
num_ptcls(), ring_size(), ptcl_size(), density(), density_offset() {

}

stage_param_fog_ring::~stage_param_fog_ring() {

}

void stage_param_fog_ring::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            stage_param_fog_ring_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_fog_ring::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            stage_param_fog_ring_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_fog_ring::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    stage_param_fog_ring_read_inner(this, s);
}

void stage_param_fog_ring::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        stage_param_fog_ring_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_fog_ring::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        stage_param_fog_ring_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_fog_ring::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    stage_param_fog_ring_write_inner(this, s);
    s.copy(data, size);
}

bool stage_param_fog_ring::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    stage_param_fog_ring* fog_ring = (stage_param_fog_ring*)data;
    fog_ring->read(path.c_str());

    return fog_ring->ready;
}

static void stage_param_fog_ring_read_inner(stage_param_fog_ring* fog_ring, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;
    while (d = stage_param_fog_ring_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "num_ptcls", 9)) {
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%d", &fog_ring->num_ptcls) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "ring_size", 9)) {
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f", &fog_ring->ring_size) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "wind_dir", 8)) {
            vec3& wind_dir = fog_ring->wind_dir;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f",
                &wind_dir.x, &wind_dir.y, &wind_dir.z) != 3)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "tex_name", 8)) {
            if (buf[8] != ' ')
                goto End;

            fog_ring->tex_name.assign(buf + 9);
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "color", 5)) {
            vec4& color = fog_ring->color;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f %f",
                &color.x, &color.y, &color.z, &color.w) != 4)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "ptcl_size", 9)) {
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f", &fog_ring->ptcl_size) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "density", 7)) {
            if (!str_utils_compare_length(buf, sizeof(buf), "density_offset", 14)) {
                if (buf[14] != ' ' || sscanf_s(buf + 15, "%f", &fog_ring->density_offset) != 1)
                    goto End;
            }
            else {
                if (buf[7] != ' ' || sscanf_s(buf + 8, "%f", &fog_ring->density) != 1)
                    goto End;
            }
        }
    }

    free_def(data);
    fog_ring->ready = true;
    return;

End:
    free_def(data);
}

static void stage_param_fog_ring_write_inner(stage_param_fog_ring* fog_ring, stream& s) {
    char buf[0x200];

    s.write("num_ptcls", 9);
    stage_param_fog_ring_write_int32_t(s, buf, sizeof(buf), fog_ring->num_ptcls);
    s.write_char('\n');

    s.write("ring_size", 9);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), fog_ring->ring_size);
    s.write_char('\n');

    vec3& wind_dir = fog_ring->wind_dir;
    s.write("wind_dir", 8);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), wind_dir.x);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), wind_dir.y);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), wind_dir.z);
    s.write_char('\n');

    s.write("tex_name", 8);
    stage_param_fog_ring_write_string(s, buf, sizeof(buf), fog_ring->tex_name);
    s.write_char('\n');

    vec4& color = fog_ring->color;
    s.write("color", 5);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), color.x);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), color.y);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), color.z);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), color.w);
    s.write_char('\n');

    s.write("ptcl_size", 9);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), fog_ring->ptcl_size);
    s.write_char('\n');

    s.write("density", 7);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), fog_ring->density);
    s.write_char('\n');

    s.write("density_offset", 14);
    stage_param_fog_ring_write_float_t(s, buf, sizeof(buf), fog_ring->density_offset);
    s.write_char('\n');
}

static const char* stage_param_fog_ring_read_line(char* buf, int32_t size, const char* src) {
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

inline static void stage_param_fog_ring_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_fog_ring_write_float_t(stream& s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_fog_ring_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value) {
    s.write_char(' ');
    s.write_string(value);
}
