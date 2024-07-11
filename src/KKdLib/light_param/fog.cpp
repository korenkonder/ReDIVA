/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"
#include "shared.hpp"

static void light_param_fog_read_inner(light_param_fog* fog, stream& s);
static void light_param_fog_write_inner(light_param_fog* fog, stream& s);

light_param_fog::light_param_fog() : ready() {

}

light_param_fog::~light_param_fog() {

}

void light_param_fog::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            light_param_fog_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_fog::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            light_param_fog_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_fog::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    light_param_fog_read_inner(this, s);
}

void light_param_fog::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        light_param_fog_write_inner(this, s);
    free_def(path_txt);
}

void light_param_fog::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        light_param_fog_write_inner(this, s);
    free_def(path_txt);
}

void light_param_fog::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    light_param_fog_write_inner(this, s);
    s.copy(data, size);
}

bool light_param_fog::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    light_param_fog* fog = (light_param_fog*)data;
    fog->read(path.c_str());

    return fog->ready;
}

light_param_fog_group::light_param_fog_group() : has_type(), type(), has_density(),
density(), has_linear(), linear_start(), linear_end(), has_color(), color() {

}

static void light_param_fog_read_inner(light_param_fog* fog, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    int32_t group_id = -1;
    while (d = light_param_read_line(buf, sizeof(buf), d)) {
        light_param_fog_group* group = &fog->group[group_id];

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
        else if (group_id == -1)
            continue;

        if (!str_utils_compare_length(buf, sizeof(buf), "type", 4)) {
            if (buf[4] != ' ' || sscanf_s(buf + 5, "%d", (int32_t*)&group->type) != 1)
                goto End;

            group->has_type = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "density", 7)) {
            if (buf[7] != ' ' || sscanf_s(buf + 8, "%f", &group->density) != 1)
                goto End;

            group->has_density = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "linear", 6)) {
            if (buf[6] != ' ' || sscanf_s(buf + 7, "%f %f",
                &group->linear_start, &group->linear_end) != 2)
                goto End;

            group->has_linear = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "color", 5)) {
            vec4& color = group->color;
            if (buf[5] != ' ' || sscanf_s(buf + 6, "%f %f %f %f",
                &color.x, &color.y, &color.z, &color.w) != 4)
                goto End;

            group->has_color = true;
        }
    }

    free_def(data);
    fog->ready = true;
    return;

End:
    free_def(data);
}

static void light_param_fog_write_inner(light_param_fog* fog, stream& s) {
    char buf[0x200];

    for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++) {
        light_param_fog_group* group = &fog->group[i];
        s.write("group_start", 11);
        light_param_write_int32_t(s, buf, sizeof(buf), i);
        s.write_char('\n');

        if (group->has_type) {
            s.write("type", 4);
            light_param_write_int32_t(s, buf, sizeof(buf), (int32_t)group->type);
            s.write_char('\n');
        }

        if (group->has_density) {
            s.write("density", 7);
            light_param_write_float_t(s, buf, sizeof(buf), group->density);
            s.write_char('\n');
        }

        if (group->has_linear) {
            s.write("linear", 6);
            light_param_write_float_t(s, buf, sizeof(buf), group->linear_start);
            light_param_write_float_t(s, buf, sizeof(buf), group->linear_end);
            s.write_char('\n');
        }

        if (group->has_color) {
            vec4& color = group->color;
            s.write("color", 5);
            light_param_write_float_t(s, buf, sizeof(buf), color.x);
            light_param_write_float_t(s, buf, sizeof(buf), color.y);
            light_param_write_float_t(s, buf, sizeof(buf), color.z);
            light_param_write_float_t(s, buf, sizeof(buf), color.w);
            s.write_char('\n');
        }

        s.write("group_end", 9);
        light_param_write_int32_t(s, buf, sizeof(buf), i);
        s.write_char('\n');
    }

    s.write("EOF", 3);
    s.write_char('\n');
}
