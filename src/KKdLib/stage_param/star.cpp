/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "star.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"

static void stage_param_star_read_inner(stage_param_star* star, stream& s);
static void stage_param_star_write_inner(stage_param_star* star, stream& s);
static const char* stage_param_star_read_line(char* buf, int32_t size, const char* src);
static void stage_param_star_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
static void stage_param_star_write_size_t(stream& s, char* buf, size_t buf_size, size_t value);
static void stage_param_star_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);
static void stage_param_star_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value);

stage_param_star_utc::stage_param_star_utc() {
    year = 2013;
    month = 1;
    day = 1;
    hour = 12;
    minute = 0;
    second = 0.0f;
}

stage_param_star_modifiers::stage_param_star_modifiers() {
    field_0 = 1.0f;
    field_4 = 1.0f;
    field_8 = 0.0f;
    field_C = 0.0f;
    size_max = 64.0f;
}

stage_param_star::stage_param_star() : ready() {
    rotation_y_deg = 0.0f;
    observer_north_latitude_deg = 0.0f;
    observer_east_longitude_deg = 0.0f;
}

stage_param_star::~stage_param_star() {

}

void stage_param_star::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            stage_param_star_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_star::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            stage_param_star_read_inner(this, s);
    }
    free_def(path_txt);
}

void stage_param_star::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    stage_param_star_read_inner(this, s);
}

void stage_param_star::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        stage_param_star_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_star::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        stage_param_star_write_inner(this, s);
    free_def(path_txt);
}

void stage_param_star::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    stage_param_star_write_inner(this, s);
    s.copy(data, size);
}

bool stage_param_star::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    stage_param_star* star = (stage_param_star*)data;
    star->read(s.c_str());

    return star->ready;
}

static void stage_param_star_read_inner(stage_param_star* star, stream& s) {
    char* data = force_malloc_s(char, s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    while (d = stage_param_star_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "milky_way_texture_name", 22)) {
            if (buf[22] != ' ')
                goto End;

            star->milky_way_texture_name.assign(buf + 23);
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "utc", 3)) {
            stage_param_star_utc& utc = star->utc;
            if (buf[3] != ' ' || sscanf_s(buf + 4, "%d %d %d %d %d %f",
                &utc.year, &utc.month, &utc.day, &utc.hour, &utc.minute, &utc.second) != 6)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "rotation_y_deg", 14)) {
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%f", &star->rotation_y_deg) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "observer_north_latitude_deg", 27)) {
            if (buf[27] != ' ' || sscanf_s(buf + 28, "%f", &star->observer_north_latitude_deg) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "observer_east_longitude_deg", 27)) {
            if (buf[27] != ' ' || sscanf_s(buf + 28, "%f", &star->observer_east_longitude_deg) != 1)
                goto End;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "modifiers", 9)) {
            size_t index = 0;
            stage_param_star_modifiers modifiers;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%llu %f %f %f %f %f",
                &index, &modifiers.field_0, &modifiers.field_4,
                &modifiers.field_8, &modifiers.field_C, &modifiers.size_max) != 6)
                goto End;

            if (index >= 2)
                goto End;

            star->modifiers[index] = modifiers;
        }
    }

    free_def(data);
    star->ready = true;
    return;

End:
    free_def(data);
}

static void stage_param_star_write_inner(stage_param_star* star, stream& s) {
    char buf[0x200];

    s.write("milky_way_texture_name", 22);
    stage_param_star_write_string(s, buf, sizeof(buf), star->milky_way_texture_name);
    s.write_char('\n');

    stage_param_star_utc& utc = star->utc;
    s.write("utc", 3);
    stage_param_star_write_int32_t(s, buf, sizeof(buf), utc.year);
    stage_param_star_write_int32_t(s, buf, sizeof(buf), utc.month);
    stage_param_star_write_int32_t(s, buf, sizeof(buf), utc.day);
    stage_param_star_write_int32_t(s, buf, sizeof(buf), utc.hour);
    stage_param_star_write_int32_t(s, buf, sizeof(buf), utc.minute);
    stage_param_star_write_float_t(s, buf, sizeof(buf), utc.second);
    s.write_char('\n');

    s.write("rotation_y_deg", 14);
    stage_param_star_write_float_t(s, buf, sizeof(buf), star->rotation_y_deg);
    s.write_char('\n');

    s.write("observer_north_latitude_deg", 27);
    stage_param_star_write_float_t(s, buf, sizeof(buf), star->observer_north_latitude_deg);
    s.write_char('\n');

    s.write("observer_east_longitude_deg", 27);
    stage_param_star_write_float_t(s, buf, sizeof(buf), star->observer_east_longitude_deg);
    s.write_char('\n');

    for (size_t i = 0; i < 2; i++) {
        stage_param_star_modifiers& modifiers = star->modifiers[i];
        s.write("modifiers", 9);
        stage_param_star_write_size_t(s, buf, sizeof(buf), i);
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.field_0);
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.field_4);
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.field_8);
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.field_C);
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.size_max);
        s.write_char('\n');
    }
}

static const char* stage_param_star_read_line(char* buf, int32_t size, const char* src) {
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

inline static void stage_param_star_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_star_write_size_t(stream& s, char* buf, size_t buf_size, size_t value) {
    sprintf_s(buf, buf_size, " %llu", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_star_write_float_t(stream& s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    s.write_utf8_string(buf);
}

inline static void stage_param_star_write_string(stream& s,
    char* buf, size_t buf_size, std::string& value) {
    s.write_char(' ');
    s.write_string(value);
}
