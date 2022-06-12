/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.hpp"
#include "../../KKdLib/mat.hpp"
#include "../shader_ft.h"

extern bool light_chara_ambient;
extern vec4 npr_spec_color;

static void light_get_direction_from_position(vec4* pos_dir, light_data* light, bool force);

light_data::light_data() : type(), ambient(), diffuse(), specular(), position(),
spot_direction(), spot_exponent(), spot_cutoff(), constant(), linear(), quadratic(),
ibl_specular(), ibl_back(), ibl_direction(), tone_curve(),clip_plane() {
    vec3 temp3;
    vec4 temp4;
    light_set_type(this, LIGHT_OFF);
    temp4 = { 0.0f, 0.0, 0.0f, 1.0f };
    light_set_ambient(this, &temp4);
    temp4 = { 1.0f, 1.0, 1.0f, 0.0f };
    light_set_diffuse(this, &temp4);
    temp4 = { 1.0f, 1.0f, 1.0f, 0.0f };
    light_set_specular(this, &temp4);
    temp3 = { 0.0f, 1.0f, 1.0f };
    light_set_position(this, &temp3);
    temp3 = { 0.0f, 0.0f, -1.0f };
    light_set_spot_direction(this, &temp3);
    light_set_spot_exponent(this, 0.0f);
    light_set_spot_cutoff(this, 45.0f);
    light_set_constant(this, 1.0f);
    light_set_linear(this, 0.0f);
    light_set_quadratic(this, 0.0f);
    temp4 = { 0.0f, 0.0f, 0.0f, 0.0f };
    light_set_ibl_specular(this, &temp4);
    temp4 = { 0.0f, 0.0f, 0.0f, 0.0f };
    light_set_ibl_back(this, &temp4);
    temp4 = { 0.0f, 0.0f, 0.0f, 0.0f };
    light_set_ibl_direction(this, &temp4);
    temp3 = { 0.0f, 0.0f, 0.0f };
    light_set_tone_curve(this, &temp3);
}

light_set::light_set() : ambient_intensity(),
irradiance_r(), irradiance_g(), irradiance_b() {
    vec4 temp;
    temp = { 0.2f, 0.2f, 0.2f, 1.0f };
    light_set_set_ambient_intensity(this, &temp);
    temp = { 1.0f, 1.0f, 1.0f, 1.0f };
    light_set_diffuse(&lights[LIGHT_CHARA], &temp);
    temp = { 1.0f, 1.0f, 1.0f, 1.0f };
    light_set_specular(&lights[LIGHT_CHARA], &temp);
    light_set_set_irradiance(this, &mat4_identity, &mat4_identity, &mat4_identity);
}

inline light_type light_get_type(light_data* light) {
    return light->type;
}

inline void light_set_type(light_data* light, light_type value) {
    light->type = value;
    if (value < LIGHT_POINT)
        light->position.w = 0.0f;
    else
        light->position.w = 1.0f;
}

inline void light_get_ambient(light_data* light, vec4* value) {
    *value = light->ambient;
}

inline void light_set_ambient(light_data* light, const vec4* value) {
    light->ambient = *value;
}

inline void light_get_diffuse(light_data* light, vec4* value) {
    *value = light->diffuse;
}

inline void light_set_diffuse(light_data* light, const vec4* value) {
    light->diffuse = *value;
}

inline void light_get_specular(light_data* light, vec4* value) {
    *value = light->specular;
}

inline void light_set_specular(light_data* light, const vec4* value) {
    light->specular = *value;
}

inline void light_get_position(light_data* light, vec3* value) {
    *value = *(vec3*)&light->position;
}

inline void light_set_position(light_data* light, const  vec3* value) {
    *(vec3*)&light->position = *value;
}

inline void light_get_position_vec4(light_data* light, vec4* value) {
    *value = light->position;
}

inline void light_set_position_vec4(light_data* light, const  vec4* value) {
    light->position = *value;
}

inline void light_get_spot_direction(light_data* light, vec3* value) {
    *value = light->spot_direction;
}

inline void light_set_spot_direction(light_data* light, const vec3* value) {
    light->spot_direction = *value;
}

inline float_t light_get_spot_exponent(light_data* light) {
    return light->spot_exponent;
}

inline void light_set_spot_exponent(light_data* light, float_t value) {
    light->spot_exponent = value;
}

inline float_t light_get_spot_cutoff(light_data* light) {
    return light->spot_cutoff;
}

inline void light_set_spot_cutoff(light_data* light, float_t value) {
    light->spot_cutoff = value;
}

inline float_t light_get_constant(light_data* light) {
    return light->constant;
}

inline void light_set_constant(light_data* light, float_t value) {
    light->constant = value;
}

inline float_t light_get_linear(light_data* light) {
    return light->linear;
}

inline void light_set_linear(light_data* light, float_t value) {
    light->linear = value;
}

inline float_t light_get_quadratic(light_data* light) {
    return light->quadratic;
}

inline void light_set_quadratic(light_data* light, float_t value) {
    light->quadratic = value;
}

inline void light_get_attenuation(light_data* light, vec3* value) {
    value->x = light->constant;
    value->y = light->linear;
    value->z = light->quadratic;
}

inline void light_set_attenuation(light_data* light, const vec3* value) {
    light->constant = value->x;
    light->linear = value->y;
    light->quadratic = value->z;
}

inline void light_get_ibl_specular(light_data* light, vec4* value) {
    *value = light->ibl_specular;
}

inline void light_set_ibl_specular(light_data* light, const vec4* value) {
    light->ibl_specular = *value;
}

inline void light_get_ibl_back(light_data* light, vec4* value) {
    *value = light->ibl_back;
}

inline void light_set_ibl_back(light_data* light, const vec4* value) {
    light->ibl_back = *value;
}

inline void light_get_ibl_direction(light_data* light, vec4* value) {
    *value = light->ibl_direction;
}

inline void light_set_ibl_direction(light_data* light, const vec4* value) {
    light->ibl_direction = *value;
}

inline void light_get_tone_curve(light_data* light, vec3* value) {
    *value = light->tone_curve;
}

inline void light_set_tone_curve(light_data* light, const vec3* value) {
    light->tone_curve = *value;
}

inline void light_get_clip_plane(light_data* light, bool value[4]) {
    value[0] = light->clip_plane[0];
    value[1] = light->clip_plane[1];
    value[2] = light->clip_plane[2];
    value[3] = light->clip_plane[3];
}

inline void light_set_clip_plane(light_data* light, const bool value[4]) {
    light->clip_plane[0] = value[0];
    light->clip_plane[1] = value[1];
    light->clip_plane[2] = value[2];
    light->clip_plane[3] = value[3];
}

inline void light_set_get_ambient_intensity(light_set* set, vec4* value) {
    *value = set->ambient_intensity;
}

inline void light_set_set_ambient_intensity(light_set* set, const vec4* value) {
    set->ambient_intensity = *value;
}

inline void light_set_get_irradiance(light_set* set, mat4* r, mat4* g, mat4* b) {
    *r = set->irradiance_r;
    *g = set->irradiance_g;
    *b = set->irradiance_b;
}

inline void light_set_set_irradiance(light_set* set, const mat4* r, const mat4* g, const mat4* b) {
    set->irradiance_r = *r;
    set->irradiance_g = *g;
    set->irradiance_b = *b;
}

void light_set_data_set(light_set* set, face* face, light_set_id id) {
    vec4 ambient_intensity;
    light_set_get_ambient_intensity(set, &ambient_intensity);
    shaders_ft.state_lightmodel_set_ambient(false, ambient_intensity);

    for (int32_t i = LIGHT_CHARA; i <= LIGHT_PROJECTION; i++) {
        light_data* light = &set->lights[i];
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
    light_get_position_vec4(&set->lights[LIGHT_CHARA], &position);
    light_get_direction_from_position(&position, &set->lights[LIGHT_CHARA], false);
    light_get_ibl_specular(&set->lights[LIGHT_CHARA], &ibl_specular);
    light_get_ibl_back(&set->lights[LIGHT_CHARA], &ibl_back);
    light_get_diffuse(&set->lights[LIGHT_CHARA], &diffuse);
    light_get_specular(&set->lights[LIGHT_CHARA], &specular);

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

    light_get_position_vec4(&set->lights[LIGHT_STAGE], &position);
    light_get_direction_from_position(&position, &set->lights[LIGHT_STAGE], false);
    light_get_ibl_specular(&set->lights[LIGHT_STAGE], &ibl_specular);
    light_get_diffuse(&set->lights[LIGHT_STAGE], &diffuse);
    light_get_specular(&set->lights[LIGHT_STAGE], &specular);

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
    light_set_get_irradiance(set, &irradiance_r, &irradiance_g, &irradiance_b);
    shaders_ft.state_matrix_set_program(0, irradiance_r);
    shaders_ft.state_matrix_set_program(1, irradiance_g);
    shaders_ft.state_matrix_set_program(2, irradiance_b);

    float_t offset = face_get_offset(face);
    float_t v28 = (float_t)(1.0 - exp(offset * -0.44999999)) * 2.0f;
    offset = max(offset, 0.0f);
    shaders_ft.env_vert_set(0x11, v28 * 0.1f, offset * 0.06f, 1.0, 1.0);

    if (light_get_type(&set->lights[LIGHT_CHARA_COLOR]) == LIGHT_PARALLEL) {
        light_get_ambient(&set->lights[LIGHT_CHARA_COLOR], &ambient);
        light_get_diffuse(&set->lights[LIGHT_CHARA_COLOR], &diffuse);
        light_get_specular(&set->lights[LIGHT_CHARA_COLOR], &specular);
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

    if (light_get_type(&set->lights[LIGHT_TONE_CURVE]) == LIGHT_PARALLEL) {
        uniform_value[U_TONE_CURVE] = 1;
        light_get_position_vec4(&set->lights[LIGHT_TONE_CURVE], &position);
        light_get_direction_from_position(&position, &set->lights[LIGHT_TONE_CURVE], false);
        light_get_ambient(&set->lights[LIGHT_TONE_CURVE], &ambient);
        light_get_diffuse(&set->lights[LIGHT_TONE_CURVE], &diffuse);
        light_get_specular(&set->lights[LIGHT_TONE_CURVE], &specular);
        vec3 tone_curve;
        light_get_tone_curve(&set->lights[LIGHT_TONE_CURVE], &tone_curve);
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

    light_get_position_vec4(&set->lights[LIGHT_CHARA], &position);
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

    if (light_get_type(&set->lights[LIGHT_REFLECT]) == LIGHT_SPOT) {
        vec4 spot_direction;
        light_get_spot_direction(&set->lights[LIGHT_REFLECT], (vec3*)&spot_direction);
        vec3_negate(*(vec3*)&spot_direction, *(vec3*)&spot_direction);
        light_get_direction_from_position(&spot_direction, &set->lights[LIGHT_REFLECT], true);
        light_get_position_vec4(&set->lights[LIGHT_REFLECT], &position);
        vec3_dot(*(vec3*)&position, *(vec3*)&spot_direction, spot_direction.w);
        spot_direction.w = -spot_direction.w;

        shaders_ft.env_vert_set(28, spot_direction);
        shaders_ft.env_frag_set(22, spot_direction);
    }
    else if (light_get_type(&set->lights[LIGHT_REFLECT]) == LIGHT_POINT) {
        shaders_ft.env_vert_set(28, 0.0f, -1.0f, 0.0f, 9999.0f);
        shaders_ft.env_frag_set(22, 0.0f, -1.0f, 0.0f, 9999.0f);
    }
    else {
        shaders_ft.env_vert_set(28, 0.0f, -1.0f, 0.0f, 0.0f);
        shaders_ft.env_frag_set(22, 0.0f, -1.0f, 0.0f, 0.0f);
    }

    mat4 v63 = mat4_identity;
    mat4 v64 = mat4_identity;

    light_get_ibl_direction(&set->lights[LIGHT_CHARA], &ibl_direction);
    light_get_position_vec4(&set->lights[LIGHT_CHARA], &position);

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
    if (force || light_get_type(light) == LIGHT_PARALLEL) {
        float_t length;
        vec3_length(*(vec3*)pos_dir, length);
        if (length <= 0.000001)
            *(vec3*)pos_dir = { 0.0f, 1.0f, 0.0f };
        else
            vec3_mult_scalar(*(vec3*)pos_dir, 1.0f / length, *(vec3*)pos_dir);
        pos_dir->w = 0.0f;
    }
}
