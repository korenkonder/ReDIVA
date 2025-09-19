/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glow.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"
#include "shared.hpp"

static void light_param_glow_read_inner(light_param_glow* glow, stream& s);
static void light_param_glow_write_inner(light_param_glow* glow, stream& s);

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
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            light_param_glow_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_glow::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            light_param_glow_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_glow::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    light_param_glow_read_inner(this, s);
}

void light_param_glow::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        light_param_glow_write_inner(this, s);
    free_def(path_txt);
}

void light_param_glow::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        light_param_glow_write_inner(this, s);
    free_def(path_txt);
}

void light_param_glow::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    light_param_glow_write_inner(this, s);
    s.copy(data, size);
}

bool light_param_glow::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    light_param_glow* glow = (light_param_glow*)data;
    glow->read(path.c_str());

    return glow->ready;
}

static void light_param_glow_read_inner(light_param_glow* glow, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    while (d = light_param_read_line(buf, sizeof(buf), d)) {
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
            vec3& flare = glow->flare;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f",
                &flare.x, &flare.y, &flare.z) != 3)
                goto End;

            glow->has_flare = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "sigma", 5)) {
            vec3& sigma = glow->sigma;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f",
                &sigma.x, &sigma.y, &sigma.z) != 3)
                goto End;

            glow->has_sigma = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "intensity", 9)) {
            vec3& intensity = glow->intensity;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f %f %f",
                &intensity.x, &intensity.y, &intensity.z) != 3)
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
            vec4& fade_color = glow->fade_color;
            int32_t* blend_func = &glow->fade_color_blend_func;
            if (buf[10] != ' ' || sscanf_s(buf + 11, "%f %f %f %f %d",
                &fade_color.x, &fade_color.y, &fade_color.z, &fade_color.w, blend_func) != 5)
                goto End;

            glow->has_fade_color = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "tone_transform", 14)) {
            vec3& start = glow->tone_transform_start;
            vec3& end = glow->tone_transform_end;
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%f %f %f %f %f %f",
                &start.x, &start.y, &start.z, &end.x, &end.y, &end.z) != 6)
                goto End;

            glow->has_tone_transform = true;
        }
    }

    free_def(data);
    glow->ready = true;
    return;

End:
    free_def(data);
}

static void light_param_glow_write_inner(light_param_glow* glow, stream& s) {
    char buf[0x200];

    if (glow->has_exposure) {
        s.write("exposure", 8);
        light_param_write_float_t(s, buf, sizeof(buf), glow->exposure);
        s.write_char('\n');
    }

    if (glow->has_gamma) {
        s.write("gamma", 5);
        light_param_write_float_t(s, buf, sizeof(buf), glow->gamma);
        s.write_char('\n');
    }

    if (glow->has_saturate_power) {
        s.write("saturate_power", 14);
        light_param_write_int32_t(s, buf, sizeof(buf), glow->saturate_power);
        s.write_char('\n');
    }

    if (glow->has_saturate_coef) {
        s.write("saturate_coef", 13);
        light_param_write_float_t(s, buf, sizeof(buf), glow->saturate_coef);
        s.write_char('\n');
    }

    if (glow->has_flare) {
        vec3& flare = glow->flare;
        s.write("flare", 5);
        light_param_write_float_t(s, buf, sizeof(buf), flare.x);
        light_param_write_float_t(s, buf, sizeof(buf), flare.y);
        light_param_write_float_t(s, buf, sizeof(buf), flare.z);
        s.write_char('\n');
    }

    if (glow->has_sigma) {
        vec3& sigma = glow->sigma;
        s.write("sigma", 5);
        light_param_write_float_t(s, buf, sizeof(buf), sigma.x);
        light_param_write_float_t(s, buf, sizeof(buf), sigma.y);
        light_param_write_float_t(s, buf, sizeof(buf), sigma.z);
        s.write_char('\n');
    }

    if (glow->has_intensity) {
        vec3& intensity = glow->intensity;
        s.write("intensity", 9);
        light_param_write_float_t(s, buf, sizeof(buf), intensity.x);
        light_param_write_float_t(s, buf, sizeof(buf), intensity.y);
        light_param_write_float_t(s, buf, sizeof(buf), intensity.z);
        s.write_char('\n');
    }

    if (glow->has_auto_exposure) {
        s.write("exposure", 8);
        light_param_write_int32_t(s, buf, sizeof(buf), glow->auto_exposure ? 1 : 0);
        s.write_char('\n');
    }

    if (glow->has_tone_map_method) {
        s.write("tone_map_method", 15);
        light_param_write_int32_t(s, buf, sizeof(buf), (int32_t)glow->tone_map_method);
        s.write_char('\n');
    }

    if (glow->has_fade_color) {
        vec4& fade_color = glow->fade_color;
        int32_t blend_func = glow->fade_color_blend_func;
        s.write("fade_color", 10);
        light_param_write_float_t(s, buf, sizeof(buf), fade_color.x);
        light_param_write_float_t(s, buf, sizeof(buf), fade_color.y);
        light_param_write_float_t(s, buf, sizeof(buf), fade_color.z);
        light_param_write_float_t(s, buf, sizeof(buf), fade_color.w);
        light_param_write_int32_t(s, buf, sizeof(buf), blend_func);
        s.write_char('\n');
    }

    if (glow->has_tone_transform) {
        vec3& start = glow->tone_transform_start;
        vec3& end = glow->tone_transform_end;
        s.write("tone_transform", 14);
        light_param_write_float_t(s, buf, sizeof(buf), start.x);
        light_param_write_float_t(s, buf, sizeof(buf), start.y);
        light_param_write_float_t(s, buf, sizeof(buf), start.z);
        light_param_write_float_t(s, buf, sizeof(buf), end.x);
        light_param_write_float_t(s, buf, sizeof(buf), end.y);
        light_param_write_float_t(s, buf, sizeof(buf), end.z);
        s.write_char('\n');
    }

    s.write("EOF", 3);
    s.write_char('\n');
}
