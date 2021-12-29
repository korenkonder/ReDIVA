/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../str_utils.h"

static void light_param_light_read_inner(light_param_light* light, stream* s);
static void light_param_light_write_inner(light_param_light* light, stream* s);
static char* light_param_light_read_line(char* buf, int32_t size, char* src);
static void light_param_light_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value);
static void light_param_light_write_float_t(stream* s, char* buf, size_t buf_size, float_t value);

void light_param_light_init(light_param_light* light) {
    memset(light, 0, sizeof(light_param_light));
}

void light_param_light_read(light_param_light* light, char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        stream s;
        io_open(&s, path_txt, "rb");
        if (s.io.stream)
            light_param_light_read_inner(light, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_light_wread(light_param_light* light, wchar_t* path) {
    wchar_t* path_txt = str_utils_wadd(path, L".txt");
    if (path_wcheck_file_exists(path_txt)) {
        stream s;
        io_wopen(&s, path_txt, L"rb");
        if (s.io.stream)
            light_param_light_read_inner(light, &s);
        io_free(&s);
    }
    free(path_txt);
}

void light_param_light_mread(light_param_light* light, void* data, size_t length) {
    stream s;
    io_mopen(&s, data, length);
    light_param_light_read_inner(light, &s);
    io_free(&s);
}

void light_param_light_write(light_param_light* light, char* path) {
    if (!light || !path || !light->ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    stream s;
    io_open(&s, path_txt, "wb");
    if (s.io.stream)
        light_param_light_write_inner(light, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_light_wwrite(light_param_light* light, wchar_t* path) {
    if (!light || !path || !light->ready)
        return;

    wchar_t* path_txt = str_utils_wadd(path, L".txt");
    stream s;
    io_wopen(&s, path_txt, L"wb");
    if (s.io.stream)
        light_param_light_write_inner(light, &s);
    io_free(&s);
    free(path_txt);
}

void light_param_light_mwrite(light_param_light* light, void** data, size_t* length) {
    if (!light || !data || !light->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    light_param_light_write_inner(light, &s);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool light_param_light_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    light_param_light* light = data;
    light_param_light_read(light, string_data(&s));

    string_free(&s);
    return light->ready;
}

void light_param_light_free(light_param_light* light) {

}

static void light_param_light_read_inner(light_param_light* light, stream* s) {
    char* data = force_malloc(s->length + 1);
    io_read(s, data, s->length);
    data[s->length] = 0;

    char buf[0x100];
    char* d = data;

    int32_t group_id = -1;
    int32_t light_id = -1;
    while (d = light_param_light_read_line(buf, sizeof(buf), d)) {
        light_param_light_group* group = &light->group[group_id];
        light_param_light_data* light = &group->data[light_id];

        if (!str_utils_compare_length(buf, sizeof(buf), "group_start", 11)) {
            if (group_id != -1)
                goto End;

            int32_t index = 0;
            if (buf[11] != ' ' || sscanf_s(buf + 12, "%d", &index) != 1)
                goto End;

            group_id = index;
            continue;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "group_end", 9)) {
            if (group_id == -1)
                goto End;

            int32_t index = 0;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%d", &index) != 1)
                goto End;

            if (group_id != index)
                goto End;

            group_id = -1;
            continue;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "id_start", 8)) {
            if (light_id != -1)
                goto End;

            int32_t index = 0;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%d", &index) != 1)
                goto End;

            light_id = index;
            continue;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "id_end", 6)) {
            if (light_id == -1)
                goto End;

            int32_t index = 0;
            if (buf[6] != ' ' || sscanf_s(buf + 7, "%d", &index) != 1)
                goto End;

            if (light_id != index)
                goto End;

            light_id = -1;
            continue;
        }
        else if (group_id == -1 || light_id == -1)
            continue;

        if (!str_utils_compare_length(buf, sizeof(buf), "type", 4)) {
            if (buf[4] != ' ' || sscanf_s(buf + 5, "%d", (int32_t*)&light->type) != 1)
                goto End;

            light->has_type = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "ambient", 7)) {
            vec4u* ambient = &light->ambient;
            if (buf[7] != ' ' || sscanf_s(buf + 8, "%f %f %f %f",
                &ambient->x, &ambient->y, &ambient->z, &ambient->w) != 4)
                goto End;

            light->has_ambient = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "diffuse", 7)) {
            vec4u* diffuse = &light->diffuse;
            if (buf[7] != ' ' || sscanf_s(buf + 8, "%f %f %f %f",
                &diffuse->x, &diffuse->y, &diffuse->z, &diffuse->w) != 4)
                goto End;

            light->has_diffuse = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "specular", 8)) {
            vec4u* specular = &light->specular;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f %f",
                &specular->x, &specular->y, &specular->z, &specular->w) != 4)
                goto End;

            light->has_specular = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "position", 8)) {
            vec3* position = &light->position;
            float_t position_w = 0.0f;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f %f",
                &position->x, &position->y, &position->z, &position_w) != 4)
                goto End;

            light->has_position = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "spot_direction", 14)) {
            vec3* spot_direction = &light->spot_direction;
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%f %f %f",
                &spot_direction->x, &spot_direction->y, &spot_direction->z) != 3)
                goto End;

            light->has_spot_direction = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "spot_exponent", 13)) {
            if (buf[13] != ' ' || sscanf_s(buf + 14, "%f", &light->spot_exponent) != 1)
                goto End;

            light->has_spot_exponent = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "spot_cutoff", 11)) {
            if (buf[11] != ' ' || sscanf_s(buf + 12, "%f", &light->spot_cutoff) != 1)
                goto End;

            light->has_spot_cutoff = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "attenuation", 11)) {
            vec3* attenuation = &light->attenuation;
            if (buf[11] != ' ' || sscanf_s(buf + 12, "%f %f %f",
                &attenuation->x, &attenuation->y, &attenuation->z) != 3)
                goto End;

            light->has_attenuation = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "clipplane", 9)) {
            int32_t clip_plane[4];
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%d %d %d %d",
                &clip_plane[0], &clip_plane[1], &clip_plane[2], &clip_plane[3]) != 4)
                goto End;

            light->clip_plane[0] = clip_plane[0] ? true : false;
            light->clip_plane[1] = clip_plane[1] ? true : false;
            light->clip_plane[2] = clip_plane[2] ? true : false;
            light->clip_plane[3] = clip_plane[3] ? true : false;
            light->has_clip_plane = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "tonecurve", 9)) {
            vec3* tone_curve = &light->tone_curve;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f %f %f",
                &tone_curve->x, &tone_curve->y, &tone_curve->z) != 3)
                goto End;

            light->has_tone_curve = true;
        }
    }

    free(data);
    light->ready = true;
    return;

End:
    free(data);
}

static void light_param_light_write_inner(light_param_light* light, stream* s) {
    char buf[0x100];

    for (int32_t i = 0; i < LIGHT_SET_MAX; i++) {
        light_param_light_group* group = &light->group[i];
        io_write(s, "group_start", 11);
        light_param_light_write_int32_t(s, buf, sizeof(buf), i);
        io_write_char(s, '\n');

        for (int32_t j = 0; j < LIGHT_MAX; j++) {
            light_param_light_data* light = &group->data[j];
            io_write(s, "id_start", 8);
            light_param_light_write_int32_t(s, buf, sizeof(buf), j);
            io_write_char(s, '\n');

            if (light->has_type) {
                io_write(s, "type", 4);
                light_param_light_write_int32_t(s, buf, sizeof(buf), (int32_t)light->type);
                io_write_char(s, '\n');
            }

            if (light->has_ambient) {
                vec4u* ambient = &light->ambient;
                io_write(s, "ambient", 7);
                light_param_light_write_float_t(s, buf, sizeof(buf), ambient->x);
                light_param_light_write_float_t(s, buf, sizeof(buf), ambient->y);
                light_param_light_write_float_t(s, buf, sizeof(buf), ambient->z);
                light_param_light_write_float_t(s, buf, sizeof(buf), ambient->w);
                io_write_char(s, '\n');
            }

            if (light->has_diffuse) {
                vec4u* diffuse = &light->diffuse;
                io_write(s, "diffuse", 7);
                light_param_light_write_float_t(s, buf, sizeof(buf), diffuse->x);
                light_param_light_write_float_t(s, buf, sizeof(buf), diffuse->y);
                light_param_light_write_float_t(s, buf, sizeof(buf), diffuse->z);
                light_param_light_write_float_t(s, buf, sizeof(buf), diffuse->w);
                io_write_char(s, '\n');
            }

            if (light->has_specular) {
                vec4u* specular = &light->specular;
                io_write(s, "specular", 8);
                light_param_light_write_float_t(s, buf, sizeof(buf), specular->x);
                light_param_light_write_float_t(s, buf, sizeof(buf), specular->y);
                light_param_light_write_float_t(s, buf, sizeof(buf), specular->z);
                light_param_light_write_float_t(s, buf, sizeof(buf), specular->w);
                io_write_char(s, '\n');
            }

            if (light->has_position) {
                vec3* position = &light->position;
                io_write(s, "position", 8);
                light_param_light_write_float_t(s, buf, sizeof(buf), position->x);
                light_param_light_write_float_t(s, buf, sizeof(buf), position->y);
                light_param_light_write_float_t(s, buf, sizeof(buf), position->z);
                light_param_light_write_float_t(s, buf, sizeof(buf), 0.0f);
                io_write_char(s, '\n');
            }

            if (light->has_spot_direction) {
                vec3* spot_direction = &light->spot_direction;
                io_write(s, "spot_direction", 14);
                light_param_light_write_float_t(s, buf, sizeof(buf), spot_direction->x);
                light_param_light_write_float_t(s, buf, sizeof(buf), spot_direction->y);
                light_param_light_write_float_t(s, buf, sizeof(buf), spot_direction->z);
                io_write_char(s, '\n');
            }

            if (light->has_spot_exponent) {
                float_t spot_exponent = light->spot_exponent;
                io_write(s, "spot_exponent", 13);
                light_param_light_write_float_t(s, buf, sizeof(buf), spot_exponent);
                io_write_char(s, '\n');
            }

            if (light->has_spot_cutoff) {
                float_t spot_cutoff = light->spot_cutoff;
                io_write(s, "spot_cutoff", 11);
                light_param_light_write_float_t(s, buf, sizeof(buf), spot_cutoff);
                io_write_char(s, '\n');
            }

            if (light->has_attenuation) {
                vec3* attenuation = &light->attenuation;
                io_write(s, "attenuation", 11);
                light_param_light_write_float_t(s, buf, sizeof(buf), attenuation->x);
                light_param_light_write_float_t(s, buf, sizeof(buf), attenuation->y);
                light_param_light_write_float_t(s, buf, sizeof(buf), attenuation->z);
                io_write_char(s, '\n');
            }

            if (light->has_clip_plane) {
                bool* clip_plane = light->clip_plane;
                io_write(s, "clipplane", 10);
                light_param_light_write_int32_t(s, buf, sizeof(buf), clip_plane[0] ? 1 : 0);
                light_param_light_write_int32_t(s, buf, sizeof(buf), clip_plane[1] ? 1 : 0);
                light_param_light_write_int32_t(s, buf, sizeof(buf), clip_plane[2] ? 1 : 0);
                light_param_light_write_int32_t(s, buf, sizeof(buf), clip_plane[3] ? 1 : 0);
                io_write_char(s, '\n');
            }

            if (light->has_tone_curve) {
                vec3* tone_curve = &light->tone_curve;
                io_write(s, "tonecurve", 10);
                light_param_light_write_float_t(s, buf, sizeof(buf), tone_curve->x);
                light_param_light_write_float_t(s, buf, sizeof(buf), tone_curve->y);
                light_param_light_write_float_t(s, buf, sizeof(buf), tone_curve->z);
                io_write_char(s, '\n');
            }

            io_write(s, "id_end", 6);
            light_param_light_write_int32_t(s, buf, sizeof(buf), j);
            io_write_char(s, '\n');
        }

        io_write(s, "group_end", 9);
        light_param_light_write_int32_t(s, buf, sizeof(buf), i);
        io_write_char(s, '\n');
    }

    io_write(s, "EOF", 3);
    io_write_char(s, '\n');
}

static char* light_param_light_read_line(char* buf, int32_t size, char* src) {
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

inline static void light_param_light_write_int32_t(stream* s, char* buf, size_t buf_size, int32_t value) {
    sprintf_s(buf, buf_size, " %d", value);
    io_write_utf8_string(s, buf);
}

inline static void light_param_light_write_float_t(stream* s, char* buf, size_t buf_size, float_t value) {
    sprintf_s(buf, buf_size, " %#.6g", value);
    io_write_utf8_string(s, buf);
}
