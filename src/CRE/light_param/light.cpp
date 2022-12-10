/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.hpp"
#include "../../KKdLib/mat.hpp"
#include "../shader_ft.hpp"

extern bool light_chara_ambient;
extern vec4 npr_spec_color;

static void light_get_direction_from_position(vec4* pos_dir, light_data* light, bool force);

light_data::light_data() : type(), ambient(), diffuse(), specular(), position(),
spot_direction(), spot_exponent(), spot_cutoff(), attenuation(),
ibl_specular(), ibl_back(), ibl_direction(), tone_curve(), clip_plane() {
    set_type(LIGHT_OFF);
    set_ambient({ 0.0f, 0.0, 0.0f, 1.0f });
    set_diffuse({ 1.0f, 1.0, 1.0f, 0.0f });
    set_specular({ 1.0f, 1.0f, 1.0f, 0.0f });
    set_position({ 0.0f, 1.0f, 1.0f, 0.0f });
    set_spot_direction({ 0.0f, 0.0f, -1.0f });
    set_spot_exponent(0.0f);
    set_spot_cutoff(45.0f);
    set_constant(1.0f);
    set_linear(0.0f);
    set_quadratic(0.0f);
    set_ibl_specular({ 0.0f, 0.0f, 0.0f, 0.0f });
    set_ibl_back({ 0.0f, 0.0f, 0.0f, 0.0f });
    set_ibl_direction({ 0.0f, 0.0f, 0.0f, 0.0f });
    set_tone_curve({ 0.0f, 0.0f, 0.0f });
}

light_set::light_set() : ambient_intensity(),
irradiance_r(), irradiance_g(), irradiance_b() {
    set_ambient_intensity({ 0.2f, 0.2f, 0.2f, 1.0f });
    lights[LIGHT_CHARA].set_diffuse({ 1.0f, 1.0f, 1.0f, 1.0f });
    lights[LIGHT_CHARA].set_specular({ 1.0f, 1.0f, 1.0f, 1.0f });
    set_irradiance(mat4_identity, mat4_identity, mat4_identity);
}

light_type light_data::get_type() {
    return type;
}

void light_data::set_type(light_type value) {
    type = value;
    if (value < LIGHT_POINT)
        position.w = 0.0f;
    else
        position.w = 1.0f;
}

void light_data::get_ambient(vec4& value) {
    value = ambient;
}

void light_data::set_ambient(const vec4& value) {
    ambient = value;
}

void light_data::set_ambient(const vec4&& value) {
    ambient = value;
}

void light_data::set_ambient(const vec4& value, bool set[4]) {
    if (set[0])
        ambient.x = value.x;
    if (set[1])
        ambient.y = value.y;
    if (set[2])
        ambient.z = value.z;
    if (set[3])
        ambient.w = value.w;
}

void light_data::set_ambient(const vec4&& value, bool set[4]) {
    if (set[0])
        ambient.x = value.x;
    if (set[1])
        ambient.y = value.y;
    if (set[2])
        ambient.z = value.z;
    if (set[3])
        ambient.w = value.w;
}

void light_data::get_diffuse(vec4& value) {
    value = diffuse;
}

void light_data::set_diffuse(const vec4& value) {
    diffuse = value;
}

void light_data::set_diffuse(const vec4&& value) {
    diffuse = value;
}

void light_data::set_diffuse(const vec4& value, bool set[4]) {
    if (set[0])
        diffuse.x = value.x;
    if (set[1])
        diffuse.y = value.y;
    if (set[2])
        diffuse.z = value.z;
    if (set[3])
        diffuse.w = value.w;
}

void light_data::set_diffuse(const vec4&& value, bool set[4]) {
    if (set[0])
        diffuse.x = value.x;
    if (set[1])
        diffuse.y = value.y;
    if (set[2])
        diffuse.z = value.z;
    if (set[3])
        diffuse.w = value.w;
}

void light_data::get_specular(vec4& value) {
    value = specular;
}

void light_data::set_specular(const vec4& value) {
    specular = value;
}

void light_data::set_specular(const vec4&& value) {
    specular = value;
}

void light_data::set_specular(const vec4& value, bool set[4]) {
    if (set[0])
        specular.x = value.x;
    if (set[1])
        specular.y = value.y;
    if (set[2])
        specular.z = value.z;
    if (set[3])
        specular.w = value.w;
}

void light_data::set_specular(const vec4&& value, bool set[4]) {
    if (set[0])
        specular.x = value.x;
    if (set[1])
        specular.y = value.y;
    if (set[2])
        specular.z = value.z;
    if (set[3])
        specular.w = value.w;
}

void light_data::get_position(vec3& value) {
    value = *(vec3*)&position;
}

void light_data::set_position(const vec3& value) {
    *(vec3*)&position = value;
}

void light_data::set_position(const vec3&& value) {
    *(vec3*)&position = value;
}

void light_data::get_position(vec4& value) {
    value = position;
}

void light_data::set_position(const vec4& value) {
    position = value;
}

void light_data::set_position(const vec4&& value) {
    position = value;
}

void light_data::get_spot_direction(vec3& value) {
    value = spot_direction;
}

void light_data::set_spot_direction(const vec3& value) {
    spot_direction = value;
}

void light_data::set_spot_direction(const vec3&& value) {
    spot_direction = value;
}

float_t light_data::get_spot_exponent() {
    return spot_exponent;
}

void light_data::set_spot_exponent(float_t value) {
    spot_exponent = value;
}

float_t light_data::get_spot_cutoff() {
    return spot_cutoff;
}

void light_data::set_spot_cutoff(float_t value) {
    spot_cutoff = value;
}

float_t light_data::get_constant() {
    return attenuation.constant;
}

void light_data::set_constant(float_t value) {
    attenuation.constant = value;
}

float_t light_data::get_linear() {
    return attenuation.linear;
}

void light_data::set_linear(float_t value) {
    attenuation.linear = value;
}

float_t light_data::get_quadratic() {
    return attenuation.quadratic;
}

void light_data::set_quadratic(float_t value) {
    attenuation.quadratic = value;
}

void light_data::get_attenuation(light_attenuation& value) {
    value = attenuation;
}

void light_data::set_attenuation(const light_attenuation& value) {
    attenuation = value;
}

void light_data::set_attenuation(const light_attenuation&& value) {
    attenuation = value;
}

void light_data::get_ibl_specular(vec4& value) {
    value = ibl_specular;
}

void light_data::set_ibl_specular(const vec4& value) {
    ibl_specular = value;
}

void light_data::set_ibl_specular(const vec4&& value) {
    ibl_specular = value;
}

void light_data::get_ibl_back(vec4& value) {
    value = ibl_back;
}

void light_data::set_ibl_back(const vec4& value) {
    ibl_back = value;
}

void light_data::set_ibl_back(const vec4&& value) {
    ibl_back = value;
}

void light_data::get_ibl_direction(vec4& value) {
    value = ibl_direction;
}

void light_data::set_ibl_direction(const vec4& value) {
    ibl_direction = value;
}

void light_data::set_ibl_direction(const vec4&& value) {
    ibl_direction = value;
}

void light_data::get_tone_curve(light_tone_curve& value) {
    value = tone_curve;
}

void light_data::set_tone_curve(const light_tone_curve& value) {
    tone_curve = value;
}

void light_data::set_tone_curve(const light_tone_curve&& value) {
    tone_curve = value;
}

void light_data::get_clip_plane(light_clip_plane& value) {
    value = clip_plane;
}

void light_data::set_clip_plane(const light_clip_plane& value) {
    clip_plane = value;
}

void light_data::set_clip_plane(const light_clip_plane&& value) {
    clip_plane = value;
}

void light_set::get_ambient_intensity(vec4& value) {
    value = ambient_intensity;
}

void light_set::set_ambient_intensity(const vec4& value) {
    ambient_intensity = value;
}

void light_set::set_ambient_intensity(const vec4&& value) {
    ambient_intensity = value;
}

void light_set::get_irradiance(mat4& r, mat4& g, mat4& b) {
    r = irradiance_r;
    g = irradiance_g;
    b = irradiance_b;
}

void light_set::set_irradiance(const mat4& r, const mat4& g, const mat4& b) {
    irradiance_r = r;
    irradiance_g = g;
    irradiance_b = b;
}

void light_set::set_irradiance(const mat4&& r, const mat4&& g, const mat4&& b) {
    irradiance_r = r;
    irradiance_g = g;
    irradiance_b = b;
}

void light_set::data_set(face& face, light_set_id id) {
    vec4 ambient_intensity;
    get_ambient_intensity(ambient_intensity);
    shaders_ft.state_lightmodel_set_ambient(false, ambient_intensity);

    for (int32_t i = LIGHT_CHARA; i <= LIGHT_PROJECTION; i++) {
        light_data* light = &lights[i];
        if (light_chara_ambient || i != LIGHT_CHARA) {
            vec4 ambient = light->ambient;
            shaders_ft.state_light_set_ambient(i, ambient);
        }
        else
            shaders_ft.state_light_set_ambient(i, 0.0f, 0.0f, 0.0f, 1.0f);

        vec4 diffuse = light->diffuse;
        vec4 specular = light->specular;
        shaders_ft.state_light_set_diffuse(i, diffuse);
        shaders_ft.state_light_set_specular(i, specular);

        vec4 light_position = light->position;
        if (light->type == LIGHT_PARALLEL)
            light_get_direction_from_position(&light_position, 0, true);
        shaders_ft.state_light_set_position(i, light_position);

        light_attenuation attenuation = light->attenuation;
        shaders_ft.state_light_set_attenuation(i,
            attenuation.constant, attenuation.linear, attenuation.quadratic, light->spot_exponent);

        vec3 spot_direction = light->spot_direction;
        shaders_ft.state_light_set_spot_direction(i,
            spot_direction.x, spot_direction.y, spot_direction.z, light->spot_cutoff);
    }

    const float_t spec_coef = (float_t)(1.0 / (1.0 - cos(18.0 * DEG_TO_RAD)));
    const float_t luce_coef = (float_t)(1.0 / (1.0 - cos(45.0 * DEG_TO_RAD)));

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec4 ibl_specular;
    vec4 ibl_back;
    vec4 ibl_direction;
    vec4 temp;
    lights[LIGHT_CHARA].get_position(position);
    light_get_direction_from_position(&position, &lights[LIGHT_CHARA], false);
    lights[LIGHT_CHARA].get_ibl_specular(ibl_specular);
    lights[LIGHT_CHARA].get_ibl_back(ibl_back);
    lights[LIGHT_CHARA].get_diffuse(diffuse);
    lights[LIGHT_CHARA].get_specular(specular);

    shaders_ft.env_vert_set(5, position);
    shaders_ft.env_frag_set(5, position);

    temp = diffuse * ibl_specular;
    shaders_ft.env_vert_set(6, temp);
    shaders_ft.env_frag_set(6, temp);

    temp = specular * ibl_specular * spec_coef;
    shaders_ft.env_vert_set(7, temp);
    shaders_ft.env_frag_set(7, temp);

    temp = ibl_specular * luce_coef;
    shaders_ft.env_vert_set(8, temp);
    shaders_ft.env_frag_set(8, temp);

    temp = specular * ibl_back * spec_coef;
    shaders_ft.env_vert_set(9, temp);
    shaders_ft.env_frag_set(9, temp);

    lights[LIGHT_STAGE].get_position(position);
    light_get_direction_from_position(&position, &lights[LIGHT_STAGE], false);
    lights[LIGHT_STAGE].get_ibl_specular(ibl_specular);
    lights[LIGHT_STAGE].get_diffuse(diffuse);
    lights[LIGHT_STAGE].get_specular(specular);

    shaders_ft.env_vert_set(10, position);
    shaders_ft.env_frag_set(14, position);

    temp = diffuse * ibl_specular;
    shaders_ft.env_vert_set(11, temp);
    shaders_ft.env_frag_set(16, temp);

    temp = specular * ibl_specular * spec_coef;
    shaders_ft.env_vert_set(12, temp);
    shaders_ft.env_frag_set(17, temp);

    mat4 irradiance_r;
    mat4 irradiance_g;
    mat4 irradiance_b;
    get_irradiance(irradiance_r, irradiance_g, irradiance_b);
    shaders_ft.state_matrix_set_program(0, irradiance_r);
    shaders_ft.state_matrix_set_program(1, irradiance_g);
    shaders_ft.state_matrix_set_program(2, irradiance_b);

    float_t offset = face.get_offset();
    float_t v28 = (float_t)(1.0 - exp(offset * -0.44999999)) * 2.0f;
    offset = max_def(offset, 0.0f);
    shaders_ft.env_vert_set(0x11, v28 * 0.1f, offset * 0.06f, 1.0, 1.0);

    if (lights[LIGHT_CHARA_COLOR].get_type() == LIGHT_PARALLEL) {
        lights[LIGHT_CHARA_COLOR].get_ambient(ambient);
        lights[LIGHT_CHARA_COLOR].get_diffuse(diffuse);
        lights[LIGHT_CHARA_COLOR].get_specular(specular);
        if (specular.w > 1.0)
            specular.w = 1.0;
        shaders_ft.env_frag_set(33, ambient);
        shaders_ft.env_frag_set(34, diffuse);
        shaders_ft.env_frag_set(35, specular);
    }
    else {
        shaders_ft.env_frag_set(33, 0.0f);
        shaders_ft.env_frag_set(34, 0.0f);
        shaders_ft.env_frag_set(35, 0.0f);
    }

    if (lights[LIGHT_TONE_CURVE].get_type() == LIGHT_PARALLEL) {
        uniform_value[U_TONE_CURVE] = 1;
        lights[LIGHT_TONE_CURVE].get_position(position);
        light_get_direction_from_position(&position, &lights[LIGHT_TONE_CURVE], false);
        lights[LIGHT_TONE_CURVE].get_ambient(ambient);
        lights[LIGHT_TONE_CURVE].get_diffuse(diffuse);
        lights[LIGHT_TONE_CURVE].get_specular(specular);

        light_tone_curve tone_curve;
        lights[LIGHT_TONE_CURVE].get_tone_curve(tone_curve);
        tone_curve.end_point = min_def(tone_curve.end_point, 1.0f) - tone_curve.start_point;
        if (tone_curve.end_point > 0.0f)
            tone_curve.end_point = 1.0f / tone_curve.end_point;
        else
            tone_curve.end_point = 0.0f;

        shaders_ft.env_frag_set(36, position);
        shaders_ft.env_frag_set(37, ambient);
        shaders_ft.env_frag_set(38, diffuse);
        shaders_ft.env_frag_set(39, specular);
        shaders_ft.env_frag_set(40, tone_curve.start_point,
            tone_curve.end_point, tone_curve.coefficient, 0.0f);
    }
    else {
        uniform_value[U_TONE_CURVE] = 0;
        shaders_ft.env_frag_set(36, 0.0f, 1.0f, 0.0f, 0.0f);
        shaders_ft.env_frag_set(37, 0.0f, 0.0f, 0.0f, 0.0f);
        shaders_ft.env_frag_set(38, 0.0f, 0.0f, 0.0f, 0.0f);
        shaders_ft.env_frag_set(39, 0.0f, 0.0f, 0.0f, 0.0f);
        shaders_ft.env_frag_set(40, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    lights[LIGHT_CHARA].get_position(position);
    if (fabs(position.x) <= 0.000001f && fabs(position.z) <= 0.000001f && fabs(position.z) <= 0.000001f)
        position = { 0.0f, 1.0f, 0.0f, 1.0f };
    else {
        float_t length = vec3::length(*(vec3*)&position);
        if (length != 0.0f)
            *(vec3*)&position = *(vec3*)&position * (1.0f / length);
    }

    shaders_ft.env_vert_set(15, position.x, position.y, position.z, 1.0f);
    shaders_ft.env_vert_set(16, -position.x, -position.y, -position.z, 1.0f);

    if (lights[LIGHT_REFLECT].get_type() == LIGHT_SPOT) {
        vec4 spot_direction;
        lights[LIGHT_REFLECT].get_spot_direction(*(vec3*)&spot_direction);
        *(vec3*)&spot_direction =  -*(vec3*)&spot_direction;
        light_get_direction_from_position(&spot_direction, &lights[LIGHT_REFLECT], true);
        lights[LIGHT_REFLECT].get_position(position);
        spot_direction.w = -vec3::dot(*(vec3*)&position, *(vec3*)&spot_direction);

        shaders_ft.env_vert_set(28, spot_direction);
        shaders_ft.env_frag_set(22, spot_direction);
    }
    else if (lights[LIGHT_REFLECT].get_type() == LIGHT_POINT) {
        shaders_ft.env_vert_set(28, 0.0f, -1.0f, 0.0f, 9999.0f);
        shaders_ft.env_frag_set(22, 0.0f, -1.0f, 0.0f, 9999.0f);
    }
    else {
        shaders_ft.env_vert_set(28, 0.0f, -1.0f, 0.0f, 0.0f);
        shaders_ft.env_frag_set(22, 0.0f, -1.0f, 0.0f, 0.0f);
    }

    mat4 v63 = mat4_identity;
    mat4 v64 = mat4_identity;

    lights[LIGHT_CHARA].get_ibl_direction(ibl_direction);
    lights[LIGHT_CHARA].get_position(position);

    float_t length = vec3::length(*(vec3*)&ibl_direction);
    if (length >= 0.000001f) {
        *(vec3*)&ibl_direction = *(vec3*)&ibl_direction * (1.0f / length);

        length = vec3::length(*(vec3*)&position);
        if (length >= 0.000001f) {
            *(vec3*)&position = *(vec3*)&position * (1.0f / length);

            vec3 axis = vec3::cross(*(vec3*)&ibl_direction, *(vec3*)&position);
            length = vec3::length(axis);

            float_t v52 = vec3::dot(*(vec3*)&position, *(vec3*)&ibl_direction);
            float_t angle = fabsf(atan2f(length, v52));
            if (angle >= 0.01f && angle <= 3.131592653589793f) {
                if (length != 0.0f)
                    axis *= 1.0f / length;

                mat4_from_axis_angle(&axis, -angle, &v63);
                mat4_from_axis_angle(&axis, -angle, &v64);
            }
        }
    }

    mat4_transpose(&v63, &v63);
    shaders_ft.env_frag_set(26, v63.row0);
    shaders_ft.env_frag_set(27, v63.row1);
    shaders_ft.env_frag_set(28, v63.row2);
    shaders_ft.env_frag_set(42, npr_spec_color);
}

static void light_get_direction_from_position(vec4* pos_dir, light_data* light, bool force) {
    if (force || light->get_type() == LIGHT_PARALLEL) {
        float_t length = vec3::length(*(vec3*)pos_dir);
        if (length <= 0.000001)
            *(vec3*)pos_dir = { 0.0f, 1.0f, 0.0f };
        else
            *(vec3*)pos_dir = *(vec3*)pos_dir * (1.0f / length);
        pos_dir->w = 0.0f;
    }
}
