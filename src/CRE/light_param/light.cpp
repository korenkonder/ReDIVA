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
spot_direction(), spot_exponent(), spot_cutoff(), constant(), linear(), quadratic(),
ibl_specular(), ibl_back(), ibl_direction(), tone_curve(),clip_plane() {
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
    return constant;
}

void light_data::set_constant(float_t value) {
    constant = value;
}

float_t light_data::get_linear() {
    return linear;
}

void light_data::set_linear(float_t value) {
    linear = value;
}

float_t light_data::get_quadratic() {
    return quadratic;
}

void light_data::set_quadratic(float_t value) {
    quadratic = value;
}

void light_data::get_attenuation(vec3& value) {
    value.x = constant;
    value.y = linear;
    value.z = quadratic;
}

void light_data::set_attenuation(const vec3& value) {
    constant = value.x;
    linear = value.y;
    quadratic = value.z;
}

void light_data::set_attenuation(const vec3&& value) {
    constant = value.x;
    linear = value.y;
    quadratic = value.z;
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

void light_data::get_tone_curve(vec3& value) {
    value = tone_curve;
}

void light_data::set_tone_curve(const vec3& value) {
    tone_curve = value;
}

void light_data::set_tone_curve(const vec3&& value) {
    tone_curve = value;
}

void light_data::get_clip_plane(bool value[4]) {
    value[0] = clip_plane[0];
    value[1] = clip_plane[1];
    value[2] = clip_plane[2];
    value[3] = clip_plane[3];
}

void light_data::set_clip_plane(const bool value[4]) {
    clip_plane[0] = value[0];
    clip_plane[1] = value[1];
    clip_plane[2] = value[2];
    clip_plane[3] = value[3];
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

        shaders_ft.state_light_set_attenuation(i,
            light->constant, light->linear, light->quadratic, light->spot_exponent);

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

    vec4_mult(diffuse, ibl_specular, temp);
    shaders_ft.env_vert_set(6, temp);
    shaders_ft.env_frag_set(6, temp);

    vec4_mult(specular, ibl_specular, temp);
    vec4_mult_scalar(temp, spec_coef, temp);
    shaders_ft.env_vert_set(7, temp);
    shaders_ft.env_frag_set(7, temp);

    vec4_mult_scalar(ibl_specular, luce_coef, temp);
    shaders_ft.env_vert_set(8, temp);
    shaders_ft.env_frag_set(8, temp);

    vec4_mult(specular, ibl_back, temp);
    vec4_mult_scalar(temp, spec_coef, temp);
    shaders_ft.env_vert_set(9, temp);
    shaders_ft.env_frag_set(9, temp);

    lights[LIGHT_STAGE].get_position(position);
    light_get_direction_from_position(&position, &lights[LIGHT_STAGE], false);
    lights[LIGHT_STAGE].get_ibl_specular(ibl_specular);
    lights[LIGHT_STAGE].get_diffuse(diffuse);
    lights[LIGHT_STAGE].get_specular(specular);

    shaders_ft.env_vert_set(10, position);
    shaders_ft.env_frag_set(14, position);

    vec4_mult(diffuse, ibl_specular, temp);
    shaders_ft.env_vert_set(11, temp);
    shaders_ft.env_frag_set(16, temp);

    vec4_mult(specular, ibl_specular, temp);
    vec4_mult_scalar(temp, spec_coef, temp);
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
    offset = max(offset, 0.0f);
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
        shaders_ft.env_frag_set(33, vec4_null);
        shaders_ft.env_frag_set(34, vec4_null);
        shaders_ft.env_frag_set(35, vec4_null);
    }

    if (lights[LIGHT_TONE_CURVE].get_type() == LIGHT_PARALLEL) {
        uniform_value[U_TONE_CURVE] = 1;
        lights[LIGHT_TONE_CURVE].get_position(position);
        light_get_direction_from_position(&position, &lights[LIGHT_TONE_CURVE], false);
        lights[LIGHT_TONE_CURVE].get_ambient(ambient);
        lights[LIGHT_TONE_CURVE].get_diffuse(diffuse);
        lights[LIGHT_TONE_CURVE].get_specular(specular);

        vec3 tone_curve;
        lights[LIGHT_TONE_CURVE].get_tone_curve(tone_curve);
        tone_curve.y = min(tone_curve.y, 1.0f) - tone_curve.x;
        if (tone_curve.y > 0.0f)
            tone_curve.y = 1.0f / tone_curve.y;
        else
            tone_curve.y = 0.0f;

        shaders_ft.env_frag_set(36, position);
        shaders_ft.env_frag_set(37, ambient);
        shaders_ft.env_frag_set(38, diffuse);
        shaders_ft.env_frag_set(39, specular);
        shaders_ft.env_frag_set(40, tone_curve.x, tone_curve.y, tone_curve.z, 0.0f);
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
        float_t length;
        vec3_length(*(vec3*)&position, length);
        if (length != 0.0f)
            vec3_mult_scalar(*(vec3*)&position, 1.0f / length, *(vec3*)&position);
    }

    shaders_ft.env_vert_set(15, position.x, position.y, position.z, 1.0f);
    shaders_ft.env_vert_set(16, -position.x, -position.y, -position.z, 1.0f);

    if (lights[LIGHT_REFLECT].get_type() == LIGHT_SPOT) {
        vec4 spot_direction;
        lights[LIGHT_REFLECT].get_spot_direction(*(vec3*)&spot_direction);
        vec3_negate(*(vec3*)&spot_direction, *(vec3*)&spot_direction);
        light_get_direction_from_position(&spot_direction, &lights[LIGHT_REFLECT], true);
        lights[LIGHT_REFLECT].get_position(position);
        vec3_dot(*(vec3*)&position, *(vec3*)&spot_direction, spot_direction.w);
        spot_direction.w = -spot_direction.w;

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

    float_t length;
    vec3_length(*(vec3*)&ibl_direction, length);
    if (length >= 0.000001f) {
        vec3_mult_scalar(*(vec3*)&ibl_direction, 1.0f / length, *(vec3*)&ibl_direction);

        vec3_length(*(vec3*)&position, length);
        if (length >= 0.000001f) {
            vec3_mult_scalar(*(vec3*)&position, 1.0f / length, *(vec3*)&position);

            vec3 axis;
            vec3_cross(*(vec3*)&ibl_direction, *(vec3*)&position, axis)

            vec3_length(axis, length);

            float_t v52;
            vec3_dot(*(vec3*)&position, *(vec3*)&ibl_direction, v52);

            float_t angle = fabsf(atan2f(length, v52));
            if (angle >= 0.01f && angle <= 3.131592653589793f) {
                if (length != 0.0f)
                    vec3_mult_scalar(axis, 1.0f / length, axis);

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
        float_t length;
        vec3_length(*(vec3*)pos_dir, length);
        if (length <= 0.000001)
            *(vec3*)pos_dir = { 0.0f, 1.0f, 0.0f };
        else
            vec3_mult_scalar(*(vec3*)pos_dir, 1.0f / length, *(vec3*)pos_dir);
        pos_dir->w = 0.0f;
    }
}
