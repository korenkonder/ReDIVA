/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../str_utils.hpp"
#include "shared.hpp"

static void light_param_light_read_inner(light_param_light* light, stream& s);
static void light_param_light_write_inner(light_param_light* light, stream& s);

light_param_light::light_param_light() : ready() {

}

light_param_light::~light_param_light() {

}

void light_param_light::read(const char* path) {
    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            light_param_light_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_light::read(const wchar_t* path) {
    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            light_param_light_read_inner(this, s);
    }
    free_def(path_txt);
}

void light_param_light::read(const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);
    light_param_light_read_inner(this, s);
}

void light_param_light::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        light_param_light_write_inner(this, s);
    free_def(path_txt);
}

void light_param_light::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        light_param_light_write_inner(this, s);
    free_def(path_txt);
}

void light_param_light::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    light_param_light_write_inner(this, s);
    s.copy(data, size);
}

bool light_param_light::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    light_param_light* light = (light_param_light*)data;
    light->read(path.c_str());

    return light->ready;
}

light_attenuation::light_attenuation() : linear(), quadratic() {
    constant = 1.0f;
}

light_attenuation::light_attenuation(float_t constant, float_t linear, float_t quadratic) {
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;
}

light_clip_plane::light_clip_plane() {
    data[0] = true;
    data[1] = true;
    data[2] = true;
    data[3] = true;
}

light_clip_plane::light_clip_plane(bool data[4]) {
    this->data[0] = data[0];
    this->data[1] = data[1];
    this->data[2] = data[2];
    this->data[3] = data[3];
}

light_clip_plane::light_clip_plane(bool data0, bool data1, bool data2, bool data3) {
    data[0] = data0;
    data[1] = data1;
    data[2] = data2;
    data[3] = data3;
}

light_tone_curve::light_tone_curve() : start_point(), end_point(), coefficient() {

}

light_tone_curve::light_tone_curve(float_t start_point, float_t end_point, float_t coefficient) {
    this->start_point = start_point;
    this->end_point = end_point;
    this->coefficient = coefficient;
}

light_param_light_data::light_param_light_data() : type(), has_type(), has_ambient(), has_diffuse(),
has_specular(), has_position(), has_spot_direction(), has_spot_exponent(), spot_exponent(),
has_spot_cutoff(), has_attenuation(), has_clip_plane(), has_tone_curve() {
    spot_direction = { 0.0f, 0.0f, -1.0f };
    spot_exponent = 0.0f;
    spot_cutoff = 45.0f;
}

light_param_light_group::light_param_light_group() {

}

static void light_param_light_read_inner(light_param_light* light, stream& s) {
    char* data = force_malloc<char>(s.length + 1);
    s.read(data, s.length);
    data[s.length] = 0;

    char buf[0x200];
    const char* d = data;

    int32_t group_id = -1;
    int32_t light_id = -1;
    while (d = light_param_read_line(buf, sizeof(buf), d)) {
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
            vec4& ambient = light->ambient;
            if (buf[7] != ' ' || sscanf_s(buf + 8, "%f %f %f %f",
                &ambient.x, &ambient.y, &ambient.z, &ambient.w) != 4)
                goto End;

            light->has_ambient = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "diffuse", 7)) {
            vec4& diffuse = light->diffuse;
            if (buf[7] != ' ' || sscanf_s(buf + 8, "%f %f %f %f",
                &diffuse.x, &diffuse.y, &diffuse.z, &diffuse.w) != 4)
                goto End;

            light->has_diffuse = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "specular", 8)) {
            vec4& specular = light->specular;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f %f",
                &specular.x, &specular.y, &specular.z, &specular.w) != 4)
                goto End;

            light->has_specular = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "position", 8)) {
            vec3& position = light->position;
            float_t position_w = 0.0f;
            if (buf[8] != ' ' || sscanf_s(buf + 9, "%f %f %f %f",
                &position.x, &position.y, &position.z, &position_w) != 4)
                goto End;

            light->has_position = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "spot_direction", 14)) {
            vec3& spot_direction = light->spot_direction;
            if (buf[14] != ' ' || sscanf_s(buf + 15, "%f %f %f",
                &spot_direction.x, &spot_direction.y, &spot_direction.z) != 3)
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
            light_attenuation& attenuation = light->attenuation;
            if (buf[11] != ' ' || sscanf_s(buf + 12, "%f %f %f",
                &attenuation.constant, &attenuation.linear, &attenuation.quadratic) != 3)
                goto End;

            light->has_attenuation = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "clipplane", 9)) {
            int32_t clip_plane[4];
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%d %d %d %d",
                &clip_plane[0], &clip_plane[1], &clip_plane[2], &clip_plane[3]) != 4)
                goto End;

            light->clip_plane.data[0] = !!clip_plane[0];
            light->clip_plane.data[1] = !!clip_plane[1];
            light->clip_plane.data[2] = !!clip_plane[2];
            light->clip_plane.data[3] = !!clip_plane[3];
            light->has_clip_plane = true;
        }
        else if (!str_utils_compare_length(buf, sizeof(buf), "tonecurve", 9)) {
            light_tone_curve& tone_curve = light->tone_curve;
            if (buf[9] != ' ' || sscanf_s(buf + 10, "%f %f %f",
                &tone_curve.start_point, &tone_curve.end_point, &tone_curve.coefficient) != 3)
                goto End;

            light->has_tone_curve = true;
        }
    }

    free_def(data);
    light->ready = true;
    return;

End:
    free_def(data);
}

static void light_param_light_write_inner(light_param_light* light, stream& s) {
    char buf[0x200];

    for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++) {
        light_param_light_group* group = &light->group[i];
        s.write("group_start", 11);
        light_param_write_int32_t(s, buf, sizeof(buf), i);
        s.write_char('\n');

        for (int32_t j = LIGHT_CHARA; j < LIGHT_MAX; j++) {
            light_param_light_data* light = &group->data[j];
            s.write("id_start", 8);
            light_param_write_int32_t(s, buf, sizeof(buf), j);
            s.write_char('\n');

            if (light->has_type) {
                s.write("type", 4);
                light_param_write_int32_t(s, buf, sizeof(buf), (int32_t)light->type);
                s.write_char('\n');
            }

            if (light->has_ambient) {
                vec4& ambient = light->ambient;
                s.write("ambient", 7);
                light_param_write_float_t(s, buf, sizeof(buf), ambient.x);
                light_param_write_float_t(s, buf, sizeof(buf), ambient.y);
                light_param_write_float_t(s, buf, sizeof(buf), ambient.z);
                light_param_write_float_t(s, buf, sizeof(buf), ambient.w);
                s.write_char('\n');
            }

            if (light->has_diffuse) {
                vec4& diffuse = light->diffuse;
                s.write("diffuse", 7);
                light_param_write_float_t(s, buf, sizeof(buf), diffuse.x);
                light_param_write_float_t(s, buf, sizeof(buf), diffuse.y);
                light_param_write_float_t(s, buf, sizeof(buf), diffuse.z);
                light_param_write_float_t(s, buf, sizeof(buf), diffuse.w);
                s.write_char('\n');
            }

            if (light->has_specular) {
                vec4& specular = light->specular;
                s.write("specular", 8);
                light_param_write_float_t(s, buf, sizeof(buf), specular.x);
                light_param_write_float_t(s, buf, sizeof(buf), specular.y);
                light_param_write_float_t(s, buf, sizeof(buf), specular.z);
                light_param_write_float_t(s, buf, sizeof(buf), specular.w);
                s.write_char('\n');
            }

            if (light->has_position) {
                vec3& position = light->position;
                s.write("position", 8);
                light_param_write_float_t(s, buf, sizeof(buf), position.x);
                light_param_write_float_t(s, buf, sizeof(buf), position.y);
                light_param_write_float_t(s, buf, sizeof(buf), position.z);
                light_param_write_float_t(s, buf, sizeof(buf), 0.0f);
                s.write_char('\n');
            }

            if (light->has_spot_direction) {
                vec3& spot_direction = light->spot_direction;
                s.write("spot_direction", 14);
                light_param_write_float_t(s, buf, sizeof(buf), spot_direction.x);
                light_param_write_float_t(s, buf, sizeof(buf), spot_direction.y);
                light_param_write_float_t(s, buf, sizeof(buf), spot_direction.z);
                s.write_char('\n');
            }

            if (light->has_spot_exponent) {
                float_t spot_exponent = light->spot_exponent;
                s.write("spot_exponent", 13);
                light_param_write_float_t(s, buf, sizeof(buf), spot_exponent);
                s.write_char('\n');
            }

            if (light->has_spot_cutoff) {
                float_t spot_cutoff = light->spot_cutoff;
                s.write("spot_cutoff", 11);
                light_param_write_float_t(s, buf, sizeof(buf), spot_cutoff);
                s.write_char('\n');
            }

            if (light->has_attenuation) {
                light_attenuation& attenuation = light->attenuation;
                s.write("attenuation", 11);
                light_param_write_float_t(s, buf, sizeof(buf), attenuation.constant);
                light_param_write_float_t(s, buf, sizeof(buf), attenuation.linear);
                light_param_write_float_t(s, buf, sizeof(buf), attenuation.quadratic);
                s.write_char('\n');
            }

            if (light->has_clip_plane) {
                light_clip_plane& clip_plane = light->clip_plane;
                s.write("clipplane", 10);
                light_param_write_int32_t(s, buf, sizeof(buf), clip_plane.data[0] ? 1 : 0);
                light_param_write_int32_t(s, buf, sizeof(buf), clip_plane.data[1] ? 1 : 0);
                light_param_write_int32_t(s, buf, sizeof(buf), clip_plane.data[2] ? 1 : 0);
                light_param_write_int32_t(s, buf, sizeof(buf), clip_plane.data[3] ? 1 : 0);
                s.write_char('\n');
            }

            if (light->has_tone_curve) {
                light_tone_curve& tone_curve = light->tone_curve;
                s.write("tonecurve", 10);
                light_param_write_float_t(s, buf, sizeof(buf), tone_curve.start_point);
                light_param_write_float_t(s, buf, sizeof(buf), tone_curve.end_point);
                light_param_write_float_t(s, buf, sizeof(buf), tone_curve.coefficient);
                s.write_char('\n');
            }

            s.write("id_end", 6);
            light_param_write_int32_t(s, buf, sizeof(buf), j);
            s.write_char('\n');
        }

        s.write("group_end", 9);
        light_param_write_int32_t(s, buf, sizeof(buf), i);
        s.write_char('\n');
    }

    s.write("EOF", 3);
    s.write_char('\n');
}
