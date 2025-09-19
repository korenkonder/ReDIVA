/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "star.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"
#include <time.h>

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

void stage_param_star_utc::get_current_time() {
    time_t time_now;
    struct tm tm;
    time(&time_now);
    gmtime_s(&tm, &time_now);

    year = 1900 + tm.tm_year;
    month = 1 + tm.tm_mon;
    day = tm.tm_mday;
    hour = tm.tm_hour;
    minute = tm.tm_min;
    second = 0.0f;
}

double_t stage_param_star_utc::to_julian_date() const {
    int64_t year = this->year;
    int64_t month = this->month;
    if (month == 1 || month == 2) {
        year--;
        month += 12;
    }

    return (double_t)(day + year / 400 + (int64_t)((double_t)year * 365.25)
            - (int32_t)((double_t)(month - 2) * -30.59) - year / 100)
        + 1721088.5
        + (double_t)hour * (1.0 / 24.0)
        + (double_t)minute * (1.0 / 1440.0)
        + (double_t)second * (1.0 / 86400.0);
}

stage_param_star_modifiers::stage_param_star_modifiers() {
    color_scale = 1.0f;
    pos_scale = 1.0f;
    offset_scale = 0.0f;
    threshold = 0.0f;
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
    if (!path_txt)
        return;

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
    if (!path_txt)
        return;

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
    if (!path_txt)
        return;

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
    if (!path_txt)
        return;

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

void stage_param_star::get_mat(mat4& mat, const float_t observer_north_latitude,
    const float_t observer_east_longitude, const stage_param_star_utc& utc, const float_t rotation_y) {
    double_t v7 = (utc.to_julian_date() - 2440000.5) * 1.0027379094 + 0.671262;

    mat3 rot_y;
    mat3_rotate_y((float_t)(v7 - (double_t)(int64_t)v7) * (float_t)(M_PI * 2.0) + observer_east_longitude, &rot_y);

    float_t latitude_sin = sinf(observer_north_latitude);
    float_t latitude_cos = cosf(observer_north_latitude) * 0.99664718f;

    vec3 x_axis = { 0.0f, latitude_cos, -latitude_sin };
    mat3_transform_vector(&rot_y, &x_axis, &x_axis);

    vec3 y_axis = { 0.0f, latitude_sin, latitude_cos };
    mat3_transform_vector(&rot_y, &y_axis, &y_axis);

    vec3 z_axis = { 1.0f, 0.0f, 0.0f };
    mat3_transform_vector(&rot_y, &z_axis, &z_axis);

    x_axis = vec3::normalize(x_axis);
    y_axis = vec3::normalize(y_axis);

    float_t rot_y_sin = sinf(rotation_y);
    float_t rot_y_cos = cosf(rotation_y);

    mat4 temp = mat4_identity;
    *(vec3*)&temp.row0 = x_axis * -rot_y_sin + z_axis * rot_y_cos;
    *(vec3*)&temp.row1 = y_axis;
    *(vec3*)&temp.row2 = x_axis * -rot_y_cos + z_axis * -rot_y_sin;
    mat4_transpose(&temp, &mat);
}

bool stage_param_star::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    stage_param_star* star = (stage_param_star*)data;
    star->read(path.c_str());

    return star->ready;
}

stage_param_star& stage_param_star::operator=(const stage_param_star& star) {
    ready = star.ready;
    milky_way_texture_name.assign(star.milky_way_texture_name);
    utc = star.utc;
    rotation_y_deg = star.rotation_y_deg;
    observer_north_latitude_deg = star.observer_north_latitude_deg;
    observer_east_longitude_deg = star.observer_east_longitude_deg;
    modifiers[0] = star.modifiers[0];
    modifiers[1] = star.modifiers[1];
    return *this;
}

static void stage_param_star_read_inner(stage_param_star* star, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
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
                &index, &modifiers.color_scale, &modifiers.pos_scale,
                &modifiers.offset_scale, &modifiers.threshold, &modifiers.size_max) != 6)
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
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.color_scale);
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.pos_scale);
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.offset_scale);
        stage_param_star_write_float_t(s, buf, sizeof(buf), modifiers.threshold);
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
