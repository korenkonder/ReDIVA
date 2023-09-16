/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "litproj.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void stage_param_litproj_read_inner(stage_param_litproj* litproj, stream& s);
static void stage_param_litproj_write_inner(stage_param_litproj* litproj, stream& s);
static const char* stage_param_litproj_read_line(char* buf, int32_t size, const char* src);
static void stage_param_litproj_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value);

stage_param_litproj::stage_param_litproj() : ready() {

}

stage_param_litproj::~stage_param_litproj() {

}

void stage_param_litproj::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            stage_param_litproj_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_litproj::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            stage_param_litproj_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_litproj::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    stage_param_litproj_read_inner(this, s);
}

void stage_param_litproj::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        stage_param_litproj_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_litproj::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        stage_param_litproj_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_litproj::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    stage_param_litproj_write_inner(this, s);
    s.copy(data, size);
}

bool stage_param_litproj::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    stage_param_litproj* litproj = (stage_param_litproj*)data;
    litproj->read(s.c_str());

    return litproj->ready;
}

static void stage_param_litproj_read_inner(stage_param_litproj* litproj, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    while (d = stage_param_litproj_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "tex_name", 8)) {
            if (buf[8] != ' ')
                goto End;

            litproj->tex_name.assign(buf + 9);
        }
    }

    free_def(data);
    litproj->ready = true;
    return;

End:
    free_def(data);
}

static void stage_param_litproj_write_inner(stage_param_litproj* litproj, stream& s) {
    char buf[0x200];

    s.write("tex_name", 8);
    stage_param_litproj_write_string(s, buf, sizeof(buf), litproj->tex_name);
    s.write_char('\n');
}

static const char* stage_param_litproj_read_line(char* buf, int32_t size, const char* src) {
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

inline static void stage_param_litproj_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value) {
    s.write_char(' ');
    s.write_string(value);
}
