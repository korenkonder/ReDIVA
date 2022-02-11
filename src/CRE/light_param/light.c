/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.h"
#include "../../KKdLib/mat.h"
#include "../shader_ft.h"

extern bool light_chara_ambient;
extern vec4 npr_spec_color;

static void light_get_direction_from_position(vec4* pos_dir, light_data* light, bool force);

void light_set_init(light_set* set) {
    vec3 tv3;
    vec4 tv4;
    for (int32_t i = LIGHT_CHARA; i <= LIGHT_PROJECTION; i++) {
        light_data* light = &set->lights[i];
        light_set_type(light, LIGHT_OFF);
        tv4 = { 0.0f, 0.0, 0.0f, 1.0f };
        light_set_ambient(light, &tv4);
        tv4 = { 1.0f, 1.0, 1.0f, 0.0f };
        light_set_diffuse(light, &tv4);
        tv4 = { 1.0f, 1.0f, 1.0f, 0.0f };
        light_set_specular(light, &tv4);
        tv3 = { 0.0f, 1.0f, 1.0f };
        light_set_position(light, &tv3);
        tv3 = { 0.0f, 0.0f, -1.0f };
        light_set_spot_direction(light, &tv3);
        light_set_spot_exponent(light, 0.0f);
        light_set_spot_cutoff(light, 45.0f);
        light_set_constant(light, 1.0f);
        light_set_linear(light, 0.0f);
        light_set_quadratic(light, 0.0f);
        tv4 = { 0.0f, 0.0f, 0.0f, 0.0f };
        light_set_ibl_specular(light, &tv4);
        tv4 = { 0.0f, 0.0f, 0.0f, 0.0f };
        light_set_ibl_back(light, &tv4);
        tv4 = { 0.0f, 0.0f, 0.0f, 0.0f };
        light_set_ibl_direction(light, &tv4);
        tv3 = { 0.0f, 0.0f, 0.0f };
        light_set_tone_curve(light, &tv3);
    }
    tv4 = { 0.2f, 0.2f, 0.2f, 1.0f };
    light_set_set_ambient_intensity(set, &tv4);
    tv4 = { 1.0f, 1.0f, 1.0f, 1.0f };
    light_set_diffuse(&set->lights[LIGHT_CHARA], &tv4);
    tv4 = { 1.0f, 1.0f, 1.0f, 1.0f };
    light_set_specular(&set->lights[LIGHT_CHARA], &tv4);
    light_set_set_irradiance(set, (mat4*)&mat4_identity, (mat4*)&mat4_identity, (mat4*)&mat4_identity);
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

inline void light_set_ambient(light_data* light, vec4* value) {
    light->ambient = *value;
}

inline void light_get_diffuse(light_data* light, vec4* value) {
    *value = light->diffuse;
}

inline void light_set_diffuse(light_data* light, vec4* value) {
    light->diffuse = *value;
}

inline void light_get_specular(light_data* light, vec4* value) {
    *value = light->specular;
}

inline void light_set_specular(light_data* light, vec4* value) {
    light->specular = *value;
}

inline void light_get_position(light_data* light, vec3* value) {
    *value = *(vec3*)&light->position;
}

inline void light_set_position(light_data* light, vec3* value) {
    *(vec3*)&light->position = *value;
}

inline void light_get_position_vec4(light_data* light, vec4* value) {
    *value = light->position;
}

inline void light_set_position_vec4(light_data* light, vec4* value) {
    light->position = *value;
}

inline void light_get_spot_direction(light_data* light, vec3* value) {
    *value = light->spot_direction;
}

inline void light_set_spot_direction(light_data* light, vec3* value) {
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

inline void light_set_attenuation(light_data* light, vec3* value) {
    light->constant = value->x;
    light->linear = value->y;
    light->quadratic = value->z;
}

inline void light_get_ibl_specular(light_data* light, vec4* value) {
    *value = light->ibl_specular;
}

inline void light_set_ibl_specular(light_data* light, vec4* value) {
    light->ibl_specular = *value;
}

inline void light_get_ibl_back(light_data* light, vec4* value) {
    *value = light->ibl_back;
}

inline void light_set_ibl_back(light_data* light, vec4* value) {
    light->ibl_back = *value;
}

inline void light_get_ibl_direction(light_data* light, vec4* value) {
    *value = light->ibl_direction;
}

inline void light_set_ibl_direction(light_data* light, vec4* value) {
    light->ibl_direction = *value;
}

inline void light_get_tone_curve(light_data* light, vec3* value) {
    *value = light->tone_curve;
}

inline void light_set_tone_curve(light_data* light, vec3* value) {
    light->tone_curve = *value;
}

inline void light_get_clip_plane(light_data* light, bool value[4]) {
    value[0] = light->clip_plane[0];
    value[1] = light->clip_plane[1];
    value[2] = light->clip_plane[2];
    value[3] = light->clip_plane[3];
}

inline void light_set_clip_plane(light_data* light, bool value[4]) {
    light->clip_plane[0] = value[0];
    light->clip_plane[1] = value[1];
    light->clip_plane[2] = value[2];
    light->clip_plane[3] = value[3];
}

inline void light_set_get_ambient_intensity(light_set* set, vec4* value) {
    *value = set->ambient_intensity;
}

inline void light_set_set_ambient_intensity(light_set* set, vec4* value) {
    set->ambient_intensity = *value;
}

inline void light_set_get_irradiance(light_set* set, mat4* r, mat4* g, mat4* b) {
    *r = set->irradiance_r;
    *g = set->irradiance_g;
    *b = set->irradiance_b;
}

inline void light_set_get_irradiance_ptr(light_set* set, mat4** r, mat4** g, mat4** b) {
    *r = &set->irradiance_r;
    *g = &set->irradiance_g;
    *b = &set->irradiance_b;
}

inline void light_set_set_irradiance(light_set* set, mat4* r, mat4* g, mat4* b) {
    set->irradiance_r = *r;
    set->irradiance_g = *g;
    set->irradiance_b = *b;
}

void light_set_data_set(light_set* set, face* face, light_set_id id) {
    vec4 ambient_intensity;
    light_set_get_ambient_intensity(set, &ambient_intensity);
    shader_state_lightmodel_set_ambient_ptr(&shaders_ft, false, &ambient_intensity);

    for (int32_t i = LIGHT_CHARA; i <= LIGHT_PROJECTION; i++) {
        light_data* light = &set->lights[i];
        if (light_chara_ambient || i != LIGHT_CHARA)
            shader_state_light_set_ambient_ptr(&shaders_ft, i, &light->ambient);
        else
            shader_state_light_set_ambient(&shaders_ft, i, 0.0f, 0.0f, 0.0f, 1.0f);
        shader_state_light_set_diffuse_ptr(&shaders_ft, i, &light->diffuse);
        shader_state_light_set_specular_ptr(&shaders_ft, i, &light->specular);

        vec4 light_direction = light->position;
        light_get_direction_from_position(&light_direction, light, 0);
        shader_state_light_set_position_ptr(&shaders_ft, i, &light_direction);

        shader_state_light_set_attenuation(&shaders_ft, i,
            light->constant, light->linear, light->quadratic, light->spot_exponent);

        vec3 spot_direction = light->spot_direction;
        shader_state_light_set_spot_direction(&shaders_ft, i,
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
    light_get_direction_from_position(&position, &set->lights[LIGHT_CHARA], 0);
    light_get_ibl_specular(&set->lights[LIGHT_CHARA], &ibl_specular);
    light_get_ibl_back(&set->lights[LIGHT_CHARA], &ibl_back);
    light_get_diffuse(&set->lights[LIGHT_CHARA], &diffuse);
    light_get_specular(&set->lights[LIGHT_CHARA], &specular);

    shader_env_vert_set_ptr(&shaders_ft, 5, &position);
    shader_env_frag_set_ptr(&shaders_ft, 5, &position);

    vec4_mult(diffuse, ibl_specular, temp);
    shader_env_vert_set_ptr(&shaders_ft, 6, &temp);
    shader_env_frag_set_ptr(&shaders_ft, 6, &temp);

    vec4_mult(specular, ibl_specular, temp);
    vec4_mult_scalar(temp, spec_coef, temp);
    shader_env_vert_set_ptr(&shaders_ft, 7, &temp);
    shader_env_frag_set_ptr(&shaders_ft, 7, &temp);

    vec4_mult_scalar(ibl_specular, luce_coef, temp);
    shader_env_vert_set_ptr(&shaders_ft, 8, &temp);
    shader_env_frag_set_ptr(&shaders_ft, 8, &temp);

    vec4_mult(specular, ibl_back, temp);
    vec4_mult_scalar(temp, spec_coef, temp);
    shader_env_vert_set_ptr(&shaders_ft, 9, &temp);
    shader_env_frag_set_ptr(&shaders_ft, 9, &temp);

    light_get_position_vec4(&set->lights[LIGHT_STAGE], &position);
    light_get_direction_from_position(&position, &set->lights[LIGHT_STAGE], 0);
    light_get_ibl_specular(&set->lights[LIGHT_STAGE], &ibl_specular);
    light_get_diffuse(&set->lights[LIGHT_STAGE], &diffuse);
    light_get_specular(&set->lights[LIGHT_STAGE], &specular);

    shader_env_vert_set_ptr(&shaders_ft, 10, &position);
    shader_env_frag_set_ptr(&shaders_ft, 14, &position);

    vec4_mult(diffuse, ibl_specular, temp);
    shader_env_vert_set_ptr(&shaders_ft, 11, &temp);
    shader_env_frag_set_ptr(&shaders_ft, 16, &temp);

    vec4_mult(specular, ibl_specular, temp);
    vec4_mult_scalar(temp, spec_coef, temp);
    shader_env_vert_set_ptr(&shaders_ft, 12, &temp);
    shader_env_frag_set_ptr(&shaders_ft, 17, &temp);

    mat4* irradiance_r;
    mat4* irradiance_g;
    mat4* irradiance_b;
    light_set_get_irradiance_ptr(set, &irradiance_r, &irradiance_g, &irradiance_b);
    shader_state_matrix_set_program(&shaders_ft, 0, irradiance_r);
    shader_state_matrix_set_program(&shaders_ft, 1, irradiance_g);
    shader_state_matrix_set_program(&shaders_ft, 2, irradiance_b);

    float_t offset = face_get_offset(face);
    float_t v28 = (float_t)(1.0 - exp(offset * -0.44999999)) * 2.0f;
    offset = max(offset, 0.0f);
    shader_env_vert_set(&shaders_ft, 0x11, v28 * 0.1f, offset * 0.06f, 1.0, 1.0);

    if (light_get_type(&set->lights[LIGHT_CHARA_COLOR]) == LIGHT_PARALLEL) {
        light_get_ambient(&set->lights[LIGHT_CHARA_COLOR], &ambient);
        light_get_diffuse(&set->lights[LIGHT_CHARA_COLOR], &diffuse);
        light_get_specular(&set->lights[LIGHT_CHARA_COLOR], &specular);
        if (specular.w > 1.0)
            specular.w = 1.0;
        shader_env_frag_set_ptr(&shaders_ft, 33, &ambient);
        shader_env_frag_set_ptr(&shaders_ft, 34, &diffuse);
        shader_env_frag_set_ptr(&shaders_ft, 35, &specular);
    }
    else {
        shader_env_frag_set_ptr(&shaders_ft, 33, (vec4*)&vec4_null);
        shader_env_frag_set_ptr(&shaders_ft, 34, (vec4*)&vec4_null);
        shader_env_frag_set_ptr(&shaders_ft, 35, (vec4*)&vec4_null);
    }

    if (light_get_type(&set->lights[LIGHT_TONE_CURVE]) == LIGHT_PARALLEL) {
        uniform_value[U_TONE_CURVE] = 1;
        light_get_position_vec4(&set->lights[LIGHT_TONE_CURVE], &position);
        light_get_direction_from_position(&position, &set->lights[LIGHT_TONE_CURVE], 0);
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

        shader_env_frag_set_ptr(&shaders_ft, 36, &position);
        shader_env_frag_set_ptr(&shaders_ft, 37, &ambient);
        shader_env_frag_set_ptr(&shaders_ft, 38, &diffuse);
        shader_env_frag_set_ptr(&shaders_ft, 39, &specular);
        shader_env_frag_set(&shaders_ft, 40, tone_curve.x, tone_curve.y, tone_curve.z, 0.0f);
    }
    else {
        uniform_value[U_TONE_CURVE] = 0;
        shader_env_frag_set(&shaders_ft, 36, 0.0f, 1.0f, 0.0f, 0.0f);
        shader_env_frag_set(&shaders_ft, 37, 0.0f, 0.0f, 0.0f, 0.0f);
        shader_env_frag_set(&shaders_ft, 38, 0.0f, 0.0f, 0.0f, 0.0f);
        shader_env_frag_set(&shaders_ft, 39, 0.0f, 0.0f, 0.0f, 0.0f);
        shader_env_frag_set(&shaders_ft, 40, 0.0f, 0.0f, 0.0f, 0.0f);
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

    shader_env_vert_set(&shaders_ft, 15, position.x, position.y, position.z, 1.0f);
    shader_env_vert_set(&shaders_ft, 16, -position.x, -position.y, -position.z, 1.0f);

    if (light_get_type(&set->lights[LIGHT_REFLECT]) == LIGHT_SPOT) {
        vec4 spot_direction;
        light_get_spot_direction(&set->lights[LIGHT_REFLECT], (vec3*)&spot_direction);
        vec3_negate(*(vec3*)&spot_direction, *(vec3*)&spot_direction);
        light_get_direction_from_position(&spot_direction, &set->lights[LIGHT_REFLECT], 1);
        light_get_position_vec4(&set->lights[LIGHT_REFLECT], &position);
        vec3_dot(*(vec3*)&position, *(vec3*)&spot_direction, spot_direction.w);

        shader_env_vert_set_ptr(&shaders_ft, 28, &spot_direction);
        shader_env_frag_set_ptr(&shaders_ft, 22, &spot_direction);
    }
    else if (light_get_type(&set->lights[LIGHT_REFLECT]) == LIGHT_POINT) {
        shader_env_vert_set(&shaders_ft, 28, 0.0f, -1.0f, 0.0f, 9999.0f);
        shader_env_frag_set(&shaders_ft, 22, 0.0f, -1.0f, 0.0f, 9999.0f);
    }
    else {
        shader_env_vert_set(&shaders_ft, 28, 0.0f, -1.0f, 0.0f, 0.0f);
        shader_env_frag_set(&shaders_ft, 22, 0.0f, -1.0f, 0.0f, 0.0f);
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
    shader_env_frag_set_ptr(&shaders_ft, 26, &v63.row0);
    shader_env_frag_set_ptr(&shaders_ft, 27, &v63.row1);
    shader_env_frag_set_ptr(&shaders_ft, 28, &v63.row2);
    shader_env_frag_set_ptr(&shaders_ft, 42, &npr_spec_color);
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
