/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glow.hpp"
#include "../io/path.h"
#include "../io/stream.h"
#include "../str_utils.h"

static void light_param_glow_read_inner(light_param_glow* glow, stream* s);
static void light_param_glow_write_inner(light_param_glow* glow, stream* s);
static const char* light_param_glow_read_line(char* buf, int32_t size, const char* src);
static void light_param_glow_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value);
static void light_param_glow_write_float_t(stream* s, char* buf, size_t buf_size, float_t value);

light_param_glow::light_param_glow() : ready(), has_exposure(), exposure(), has_gamma(), gamma(),
has_saturate_power(), saturate_power(), has_saturate_coef(), saturate_coef(), has_flare(), flare(),
has_sigma(), sigma(), has_intensity(), intensity(), has_auto_exposure(), auto_exposure(),
has_tone_map_method(), tone_map_method(), has_fade_color(), fade_color(), fade_color_blend_func(),
has_tone_transform(), tone_transform_start(), tone_transform_end() {

}

light_param_glow::~light_param_glow() {

}

void light_param_glow::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        stream s;
        io_open(&s, path_txt, "rb");
        if (s.io.stream)
            light_param_glow_read_inner(this, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_glow::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        stream s;
        io_open(&s, path_txt, L"rb");
        if (s.io.stream)
            light_param_glow_read_inner(this, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_glow::read(const void* data, size_t size) {
    stream s;
    io_open(&s, data, size);
    light_param_glow_read_inner(this, &s);
    io_free(&s);
}

void light_param_glow::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    stream s;
    io_open(&s, path_txt, "wb");
    if (s.io.stream)
        light_param_glow_write_inner(this, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_glow::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    stream s;
    io_open(&s, path_txt, L"wb");
    if (s.io.stream)
        light_param_glow_write_inner(this, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_glow::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    stream s;
    io_open(&s);
    light_param_glow_write_inner(this, &s);
    io_copy(&s, data, size);
    io_free(&s);
}

bool light_param_glow::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    light_param_glow* glow = (light_param_glow*)data;
    glow->read(s.c_str());

    return glow->ready;
}

static void light_param_glow_read_inner(light_param_glow* glow, stream* s) {
    char* data = force_malloc_s(char, s->length + 1);
    io_read(s, data, s->length);
    data[s->length] = 0;

    char buf[0x100];
    const char* d = data;

    while (d = light_param_glow_read_line(buf, sizeof(buf), d)) {
        if (!str_utils_compare_length(buf, sizeof(buf), "exposure", 8)) {
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f", &glow->exposure) != 1)
                goto End;

            glow->has_exposure = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "gamma", 5)) {
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f", &glow->gamma) != 1)
                goto End;

            glow->has_gamma = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "saturate_power", 14)) {
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%d", &glow->saturate_power) != 1)
                goto End;

            glow->has_saturate_power = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "saturate_coef", 13)) {
            if (buf[13] != ' ' || sscanf_s(buf + 14, "%f", &glow->saturate_coef) != 1)
                goto End;

            glow->has_saturate_coef = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "flare", 5)) {
            vec3* flare = &glow->flare;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f",
                &flare->x, &flare->y, &flare->z) != 3)
                goto End;

            glow->has_flare = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "sigma", 5)) {
            vec3* sigma = &glow->sigma;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f",
                &sigma->x, &sigma->y, &sigma->z) != 3)
                goto End;

            glow->has_sigma = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "intensity", 9)) {
            vec3* intensity = &glow->intensity;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f %f %f",
                &intensity->x, &intensity->y, &intensity->z) != 3)
                goto End;

            glow->has_intensity = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "auto_exposure", 13)) {
            int32_t auto_exposure = 0;
            if (buf[13] != ' ' || sscanf_s(buf + 14, "%d", &auto_exposure) != 1)
                goto End;

            glow->auto_exposure = auto_exposure ? true : false;
            glow->has_auto_exposure = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "tone_map_method", 15)) {
            if (buf[15] != ' ' || sscanf_s(buf + 16, "%d", (int32_t*)&glow->tone_map_method) != 1)
                goto End;

            glow->has_tone_map_method = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "fade_color", 10)) {
            vec4u* fade_color = &glow->fade_color;
            int32_t* blend_func = &glow->fade_color_blend_func;
            if (buf[10] != ' ' || sscanf_s(buf + 11, "%f %f %f %f %d",
                &fade_color->x, &fade_color->y, &fade_color->z, &fade_color->w, blend_func) != 5)
                goto End;

            glow->has_fade_color = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "tone_transform", 14)) {
            vec3* start = &glow->tone_transform_start;
            vec3* end = &glow->tone_transform_end;
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%f %f %f %f %f %f",
                &start->x, &start->y, &start->z, &end->x, &end->y, &end->z) != 6)
                goto End;

            glow->has_tone_transform = true;
        }
    }

    free(data);
    glow->ready = true;
    return;

End:
    free(data);
}

static void light_param_glow_write_inner(light_param_glow* glow, stream* s) {
    char buf[0x100];

    if (glow->has_exposure) {
        io_write(s, "exposure", 8);
        light_param_glow_write_float_t(s, buf, sizeof(buf), glow->exposure);
        io_write_char(s, '\n');
    }

    if (glow->has_gamma) {
        io_write(s, "gamma", 5);
        light_param_glow_write_float_t(s, buf, sizeof(buf), glow->gamma);
        io_write_char(s, '\n');
    }

    if (glow->has_saturate_power) {
        io_write(s, "saturate_power", 14);
        light_param_glow_write_int32_t(s, buf, sizeof(buf), glow->saturate_power);
        io_write_char(s, '\n');
    }

    if (glow->has_saturate_coef) {
        io_write(s, "saturate_coef", 13);
        light_param_glow_write_float_t(s, buf, sizeof(buf), glow->saturate_coef);
        io_write_char(s, '\n');
    }

    if (glow->has_flare) {
        vec3* flare = &glow->flare;
        io_write(s, "flare", 5);
        light_param_glow_write_float_t(s, buf, sizeof(buf), flare->x);
        light_param_glow_write_float_t(s, buf, sizeof(buf), flare->y);
        light_param_glow_write_float_t(s, buf, sizeof(buf), flare->z);
        io_write_char(s, '\n');
    }

    if (glow->has_sigma) {
        vec3* sigma = &glow->sigma;
        io_write(s, "sigma", 5);
        light_param_glow_write_float_t(s, buf, sizeof(buf), sigma->x);
        light_param_glow_write_float_t(s, buf, sizeof(buf), sigma->y);
        light_param_glow_write_float_t(s, buf, sizeof(buf), sigma->z);
        io_write_char(s, '\n');
    }

    if (glow->has_intensity) {
        vec3* intensity = &glow->intensity;
        io_write(s, "intensity", 9);
        light_param_glow_write_float_t(s, buf, sizeof(buf), intensity->x);
        light_param_glow_write_float_t(s, buf, sizeof(buf), intensity->y);
        light_param_glow_write_float_t(s, buf, sizeof(buf), intensity->z);
        io_write_char(s, '\n');
    }

    if (glow->has_auto_exposure) {
        io_write(s, "exposure", 8);
        light_param_glow_write_int32_t(s, buf, sizeof(buf), glow->auto_exposure ? 1 : 0);
        io_write_char(s, '\n');
    }

    if (glow->has_tone_map_method) {
        io_write(s, "tone_map_method", 15);
        light_param_glow_write_int32_t(s, buf, sizeof(buf), (int32_t)glow->tone_map_method);
        io_write_char(s, '\n');
    }

    if (glow->has_fade_color) {
        vec4u* fade_color = &glow->fade_color;
        int32_t blend_func = glow->fade_color_blend_func;
        io_write(s, "fade_color", 10);
        light_param_glow_write_float_t(s, buf, sizeof(buf), fade_color->x);
        light_param_glow_write_float_t(s, buf, sizeof(buf), fade_color->y);
        light_param_glow_write_float_t(s, buf, sizeof(buf), fade_color->z);
        light_param_glow_write_float_t(s, buf, sizeof(buf), fade_color->w);
        light_param_glow_write_int32_t(s, buf, sizeof(buf), blend_func);
        io_write_char(s, '\n');
    }

    if (glow->has_tone_transform) {
        vec3* start = &glow->tone_transform_start;
        vec3* end = &glow->tone_transform_end;
        io_write(s, "tone_transform", 14);
        light_param_glow_write_float_t(s, buf, sizeof(buf), start->x);
        light_param_glow_write_float_t(s, buf, sizeof(buf), start->y);
        light_param_glow_write_float_t(s, buf, sizeof(buf), start->z);
        light_param_glow_write_float_t(s, buf, sizeof(buf), end->x);
        light_param_glow_write_float_t(s, buf, sizeof(buf), end->y);
        light_param_glow_write_float_t(s, buf, sizeof(buf), end->z);
        io_write_char(s, '\n');
    }

    io_write(s, "EOF", 3);
    io_write_char(s, '\n');
}

static const char* light_param_glow_read_line(char* buf, int32_t size, const char* src) {
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

    if (!str_utils_compare(buf, "EOF"))
        return 0;
    return src;
}

inline static void light_param_glow_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    io_write_utf8_string(s, buf);
}

inline static void light_param_glow_write_float_t(stream* s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    io_write_utf8_string(s, buf);
}
