/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "wind.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"
#include "shared.hpp"

static void light_param_wind_read_inner(light_param_wind* wind, stream& s);
static void light_param_wind_write_inner(light_param_wind* wind, stream& s);

light_param_wind::light_param_wind() : ready(), has_scale(), scale(), has_cycle(),
cycle(), has_rot(), rot_y(), rot_z(), has_bias(), bias(), has_spc(), spc() {

}

light_param_wind::~light_param_wind() {

}

void light_param_wind::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            light_param_wind_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_wind::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            light_param_wind_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_wind::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    light_param_wind_read_inner(this, s);
}

void light_param_wind::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        light_param_wind_write_inner(this, s);
    free_def(path_txt);
}

void light_param_wind::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        light_param_wind_write_inner(this, s);
    free_def(path_txt);
}

void light_param_wind::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    light_param_wind_write_inner(this, s);
    s.copy(data, size);
}

bool light_param_wind::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    light_param_wind* wind = (light_param_wind*)data;
    wind->read(path.c_str());

    return wind->ready;
}

light_param_wind_spc::light_param_wind_spc() : cos(), sin() {

}

static void light_param_wind_read_inner(light_param_wind* wind, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    while (d = light_param_read_line(buf, sizeof(buf), d)) {
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
            size_t index = 0;
            light_param_wind_spc spc;
            if (buf[3] != ' ' || sscanf_s(buf + 4, "%llu %f %f",
                &index, &spc.cos, &spc.sin) != 3)
                goto End;

            if (index >= 16)
                index = 0;

            wind->spc[index] = spc;
            wind->has_spc[index] = true;
        }
    }

    free_def(data);
    wind->ready = true;
    return;

End:
    free_def(data);
}

static void light_param_wind_write_inner(light_param_wind* wind, stream& s) {
    char buf[0x200];

    if (wind->has_scale) {
        s.write("scale", 5);
        light_param_write_float_t(s, buf, sizeof(buf), wind->scale);
        s.write_char('\n');
    }

    if (wind->has_cycle) {
        s.write("cycle", 5);
        light_param_write_float_t(s, buf, sizeof(buf), wind->cycle);
        s.write_char('\n');
    }

    if (wind->has_rot) {
        s.write("rot", 3);
        light_param_write_float_t(s, buf, sizeof(buf), wind->rot_y);
        light_param_write_float_t(s, buf, sizeof(buf), wind->rot_z);
        s.write_char('\n');
    }

    if (wind->has_bias) {
        s.write("bias", 4);
        light_param_write_float_t(s, buf, sizeof(buf), wind->bias);
        s.write_char('\n');
    }

    for (int32_t i = 0; i < 16; i++)
        if (wind->has_spc[i]) {
            light_param_wind_spc& spc = wind->spc[i];
            s.write("spc", 3);
            light_param_write_int32_t(s, buf, sizeof(buf), i);
            light_param_write_float_t(s, buf, sizeof(buf), spc.cos);
            light_param_write_float_t(s, buf, sizeof(buf), spc.sin);
            s.write_char('\n');
        }
}
