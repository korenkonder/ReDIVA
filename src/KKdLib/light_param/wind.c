/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "wind.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../str_utils.h"

static void light_param_wind_read_inner(light_param_wind* wind, stream* s);
static void light_param_wind_write_inner(light_param_wind* wind, stream* s);
static char* light_param_wind_read_line(char* buf, int32_t size, char* src);
static void light_param_wind_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value);
static void light_param_wind_write_float_t(stream* s, char* buf, size_t buf_size, float_t value);

void light_param_wind_init(light_param_wind* wind) {
    memset(wind, 0, sizeof(light_param_wind));
}

void light_param_wind_read(light_param_wind* wind, char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        stream s;
        io_open(&s, path_txt, "rb");
        if (s.io.stream)
            light_param_wind_read_inner(wind, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_wind_wread(light_param_wind* wind, wchar_t* path) {
    wchar_t* path_txt = str_utils_wadd(path, L".txt");
    if (path_wcheck_file_exists(path_txt)) {
        stream s;
        io_wopen(&s, path_txt, L"rb");
        if (s.io.stream)
            light_param_wind_read_inner(wind, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_wind_mread(light_param_wind* wind, void* data, size_t length) {
    stream s;
    io_mopen(&s, data, length);
    light_param_wind_read_inner(wind, &s);
    io_free(&s);
}

void light_param_wind_write(light_param_wind* wind, char* path) {
    if (!wind || !path || !wind->ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    stream s;
    io_open(&s, path_txt, "wb");
    if (s.io.stream)
        light_param_wind_write_inner(wind, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_wind_wwrite(light_param_wind* wind, wchar_t* path) {
    if (!wind || !path || !wind->ready)
        return;

    wchar_t* path_txt = str_utils_wadd(path, L".txt");
    stream s;
    io_wopen(&s, path_txt, L"wb");
    if (s.io.stream)
        light_param_wind_write_inner(wind, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_wind_mwrite(light_param_wind* wind, void** data, size_t* length) {
    if (!wind || !data || !wind->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    light_param_wind_write_inner(wind, &s);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool light_param_wind_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    light_param_wind* wind = (light_param_wind*)data;
    light_param_wind_read(wind, string_data(&s));

    string_free(&s);
    return wind->ready;
}

void light_param_wind_free(light_param_wind* wind) {

}

static void light_param_wind_read_inner(light_param_wind* wind, stream* s) {
    char* data = force_malloc_s(char, s->length + 1);
    io_read(s, data, s->length);
    data[s->length] = 0;

    char buf[0x100];
    char* d = data;

    while (d = light_param_wind_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "scale", 5)) {
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f", &wind->scale) != 1)
                goto End;

            wind->has_scale = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "cycle", 5)) {
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f", &wind->cycle) != 1)
                goto End;

            wind->has_cycle = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "rot", 3)) {
            if (buf[3] != ' ' || sscanf_s(buf + 4, "%f %f", &wind->rot_y, &wind->rot_z) != 2)
                goto End;

            wind->has_rot = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "bias", 4)) {
            if (buf[4] != ' ' || sscanf_s(buf + 5, "%f", &wind->bias) != 1)
                goto End;

            wind->has_bias = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "spc", 3)) {
            int32_t index = 0;
            light_param_wind_spc spc;
            if (buf[3] != ' ' || sscanf_s(buf + 4, "%d %f %f",
                &index, &spc.cos, &spc.sin) != 3)
                goto End;

            if (index >= 16)
                index = 0;

            wind->spc[index] = spc;
            wind->has_spc[index] = true;
        }
    }

    free(data);
    wind->ready = true;
    return;

End:
    free(data);
}

static void light_param_wind_write_inner(light_param_wind* wind, stream* s) {
    char buf[0x100];

    if (wind->has_scale) {
        io_write(s, "scale", 5);
        light_param_wind_write_float_t(s, buf, sizeof(buf), wind->scale);
        io_write_char(s, '\n');
    }

    if (wind->has_cycle) {
        io_write(s, "cycle", 5);
        light_param_wind_write_float_t(s, buf, sizeof(buf), wind->cycle);
        io_write_char(s, '\n');
    }

    if (wind->has_rot) {
        io_write(s, "rot", 3);
        light_param_wind_write_float_t(s, buf, sizeof(buf), wind->rot_y);
        light_param_wind_write_float_t(s, buf, sizeof(buf), wind->rot_z);
        io_write_char(s, '\n');
    }

    if (wind->has_bias) {
        io_write(s, "bias", 4);
        light_param_wind_write_float_t(s, buf, sizeof(buf), wind->bias);
        io_write_char(s, '\n');
    }

    for (int32_t i = 0; i < 16; i++)
        if (wind->has_spc[i]) {
            io_write(s, "spc", 3);
            light_param_wind_write_int32_t(s, buf, sizeof(buf), i);
            light_param_wind_write_float_t(s, buf, sizeof(buf), wind->spc[i].cos);
            light_param_wind_write_float_t(s, buf, sizeof(buf), wind->spc[i].sin);
            io_write_char(s, '\n');
        }
}

static char* light_param_wind_read_line(char* buf, int32_t size, char* src) {
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

    if (!str_utils_compare(buf, "EOF"))
        return 0;
    return src;
}

inline static void light_param_wind_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    io_write_utf8_string(s, buf);
}

inline static void light_param_wind_write_float_t(stream* s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %g", value);
    io_write_utf8_string(s, buf);
}
