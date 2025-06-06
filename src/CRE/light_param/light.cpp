/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.hpp"
#include "../../KKdLib/mat.hpp"
#include "../render_context.hpp"

struct light_data_color {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

extern bool light_chara_ambient;
extern vec4 npr_cloth_spec_color;

static void light_get_direction_from_position(vec4* pos_dir, light_data* light, bool force = false);
static void light_get_light_color(light_data* light, light_data_color& value, light_id id);

light_data::light_data() : type(), ambient(), diffuse(), specular(), position(),
spot_direction(), spot_exponent(), spot_cutoff(), attenuation(),
ibl_color0(), ibl_color1(), ibl_direction(), tone_curve(), clip_plane() {
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
    set_ibl_color0({ 0.0f, 0.0f, 0.0f, 0.0f });
    set_ibl_color1({ 0.0f, 0.0f, 0.0f, 0.0f });
    set_ibl_direction({ 0.0f, 0.0f, 0.0f, 0.0f });
    set_tone_curve({ 0.0f, 0.0f, 0.0f });
    set_clip_plane({ true, true, true, true });
}

light_set::light_set() : ambient_intensity(),
irradiance_r(), irradiance_g(), irradiance_b() {
    set_ambient_intensity({ 0.2f, 0.2f, 0.2f, 1.0f });
    lights[LIGHT_CHARA].set_diffuse({ 1.0f, 1.0f, 1.0f, 1.0f });
    lights[LIGHT_CHARA].set_specular({ 1.0f, 1.0f, 1.0f, 1.0f });
    set_irradiance(mat4_identity, mat4_identity, mat4_identity);
}

light_type light_data::get_type() const {
    return type;
}

void light_data::set_type(light_type value) {
    type = value;
    if (value < LIGHT_POINT)
        position.w = 0.0f;
    else
        position.w = 1.0f;
}

void light_data::get_ambient(vec4& value) const {
    value = ambient;
}

void light_data::set_ambient(const vec4& value) {
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

void light_data::get_diffuse(vec4& value) const {
    value = diffuse;
}

void light_data::set_diffuse(const vec4& value) {
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

void light_data::get_specular(vec4& value) const {
    value = specular;
}

void light_data::set_specular(const vec4& value) {
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

void light_data::get_position(vec3& value) const {
    value = *(vec3*)&position;
}

void light_data::set_position(const vec3& value) {
    *(vec3*)&position = value;
}

void light_data::get_position(vec4& value) const {
    value = position;
}

void light_data::set_position(const vec4& value) {
    position = value;
}

void light_data::get_spot_direction(vec3& value) const {
    value = spot_direction;
}

void light_data::set_spot_direction(const vec3& value) {
    spot_direction = value;
}

float_t light_data::get_spot_exponent() const {
    return spot_exponent;
}

void light_data::set_spot_exponent(float_t value) {
    spot_exponent = value;
}

float_t light_data::get_spot_cutoff() const {
    return spot_cutoff;
}

void light_data::set_spot_cutoff(float_t value) {
    spot_cutoff = value;
}

float_t light_data::get_constant() const {
    return attenuation.constant;
}

void light_data::set_constant(float_t value) {
    attenuation.constant = value;
}

float_t light_data::get_linear() const {
    return attenuation.linear;
}

void light_data::set_linear(float_t value) {
    attenuation.linear = value;
}

float_t light_data::get_quadratic() const {
    return attenuation.quadratic;
}

void light_data::set_quadratic(float_t value) {
    attenuation.quadratic = value;
}

void light_data::get_attenuation(light_attenuation& value) const {
    value = attenuation;
}

void light_data::set_attenuation(const light_attenuation& value) {
    attenuation = value;
}

void light_data::get_ibl_color0(vec4& value) const {
    value = ibl_color0;
}

void light_data::set_ibl_color0(const vec4& value) {
    ibl_color0 = value;
}

void light_data::get_ibl_color1(vec4& value) const {
    value = ibl_color1;
}

void light_data::set_ibl_color1(const vec4& value) {
    ibl_color1 = value;
}

void light_data::get_ibl_direction(vec4& value) const {
    value = ibl_direction;
}

void light_data::set_ibl_direction(const vec4& value) {
    ibl_direction = value;
}

void light_data::get_tone_curve(light_tone_curve& value) const {
    value = tone_curve;
}

void light_data::set_tone_curve(const light_tone_curve& value) {
    tone_curve = value;
}

void light_data::get_clip_plane(light_clip_plane& value) const {
    value = clip_plane;
}

void light_data::set_clip_plane(const light_clip_plane& value) {
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

void light_set::data_set(render_data_context& rend_data_ctx, face& face, light_set_id id) {
    static const float_t spec_coef = (float_t)(1.0 / (1.0 - cos(18.0 * DEG_TO_RAD)));
    static const float_t luce_coef = (float_t)(1.0 / (1.0 - cos(45.0 * DEG_TO_RAD)));

    light_data_color light_color[LIGHT_MAX];
    for (int32_t i = 0; i < LIGHT_MAX; i++)
        light_get_light_color(&lights[i], light_color[i], (light_id)i);

    vec4 light_env_stage_diffuse = light_color[LIGHT_STAGE].diffuse;
    vec4 light_env_stage_specular = light_color[LIGHT_STAGE].specular;

    vec4 light_env_chara_diffuse = light_color[LIGHT_CHARA].diffuse;
    vec4 light_env_chara_ambient = light_color[LIGHT_CHARA].ambient;
    vec4 light_env_chara_specular = light_color[LIGHT_CHARA].specular;

    vec4 light_env_reflect_diffuse;
    vec4 light_env_reflect_ambient;
    lights[LIGHT_REFLECT].get_diffuse(light_env_reflect_diffuse);
    lights[LIGHT_REFLECT].get_ambient(light_env_reflect_ambient);

    vec4 light_env_proj_diffuse;
    vec4 light_env_proj_specular;
    vec4 light_env_proj_position;
    lights[LIGHT_PROJECTION].get_diffuse(light_env_proj_diffuse);
    lights[LIGHT_PROJECTION].get_specular(light_env_proj_specular);
    lights[LIGHT_PROJECTION].get_position(light_env_proj_position);
    if (lights[LIGHT_PROJECTION].get_type() == LIGHT_PARALLEL)
        light_get_direction_from_position(&light_env_proj_position, 0, true);

    vec4 light_chara_dir;
    vec4 light_chara_ibl_color0;
    vec4 light_chara_ibl_color1;
    vec4 light_chara_specular;
    lights[LIGHT_CHARA].get_position(light_chara_dir);
    light_get_direction_from_position(&light_chara_dir, &lights[LIGHT_CHARA]);
    lights[LIGHT_CHARA].get_ibl_color0(light_chara_ibl_color0);
    lights[LIGHT_CHARA].get_ibl_color1(light_chara_ibl_color1);
    lights[LIGHT_CHARA].get_specular(light_chara_specular);

    vec4 light_chara_spec = light_chara_specular * light_chara_ibl_color0 * spec_coef;
    vec4 light_chara_luce = light_chara_ibl_color0 * luce_coef;
    vec4 light_chara_back = light_chara_specular * light_chara_ibl_color1 * spec_coef;

    vec4 light_stage_dir;
    vec4 light_stage_ibl_color;
    vec4 light_stage_diffuse;
    vec4 light_stage_specular;
    lights[LIGHT_STAGE].get_position(light_stage_dir);
    light_get_direction_from_position(&light_stage_dir, &lights[LIGHT_STAGE]);
    lights[LIGHT_STAGE].get_ibl_color0(light_stage_ibl_color);
    lights[LIGHT_STAGE].get_diffuse(light_stage_diffuse);
    lights[LIGHT_STAGE].get_specular(light_stage_specular);

    vec4 light_stage_diff = light_stage_diffuse * light_stage_ibl_color;
    vec4 light_stage_spec = light_stage_specular * light_stage_ibl_color * spec_coef;

    mat4 irradiance_r_transforms;
    mat4 irradiance_g_transforms;
    mat4 irradiance_b_transforms;
    get_irradiance(irradiance_r_transforms, irradiance_g_transforms, irradiance_b_transforms);

    float_t v27 = face.get_offset();
    float_t v28 = (1.0f - expf(v27 * -0.45f)) * 2.0f;
    v27 = max_def(v27, 0.0f);
    vec4 light_face_diff = { v28 * 0.1f, v27 * 0.06f, 1.0f, 1.0f };

    vec4 chara_color_rim;
    vec4 chara_color0;
    vec4 chara_color1;
    if (lights[LIGHT_CHARA_COLOR].get_type() == LIGHT_PARALLEL) {
        lights[LIGHT_CHARA_COLOR].get_ambient(chara_color_rim);
        lights[LIGHT_CHARA_COLOR].get_diffuse(chara_color0);
        lights[LIGHT_CHARA_COLOR].get_specular(chara_color1);
        if (chara_color1.w > 1.0f)
            chara_color1.w = 1.0f;
    }
    else {
        chara_color_rim = 0.0f;
        chara_color0 = 0.0f;
        chara_color1 = 0.0f;
    }

    vec4 chara_f_dir;
    vec4 chara_f_ambient;
    vec4 chara_f_diffuse;
    vec4 chara_tc_param;
    if (lights[LIGHT_TONE_CURVE].get_type() == LIGHT_PARALLEL) {
        rend_data_ctx.shader_flags.arr[U_TONE_CURVE] = 1;
        lights[LIGHT_TONE_CURVE].get_position(chara_f_dir);
        light_get_direction_from_position(&chara_f_dir, &lights[LIGHT_TONE_CURVE]);
        lights[LIGHT_TONE_CURVE].get_ambient(chara_f_ambient);
        lights[LIGHT_TONE_CURVE].get_diffuse(chara_f_diffuse);

        light_tone_curve tone_curve;
        lights[LIGHT_TONE_CURVE].get_tone_curve(tone_curve);
        chara_tc_param.x = tone_curve.start_point;
        float_t end_point = min_def(tone_curve.end_point, 1.0f) - tone_curve.start_point;
        if (end_point > 0.0f)
            chara_tc_param.y = 1.0f / end_point;
        else
            chara_tc_param.y = 0.0f;
        chara_tc_param.z = tone_curve.coefficient;
        chara_tc_param.w = 0.0f;
    }
    else {
        rend_data_ctx.shader_flags.arr[U_TONE_CURVE] = 0;
        chara_f_dir = { 0.0f, 1.0f, 0.0f, 0.0f };
        chara_f_ambient = 0.0f;
        chara_f_diffuse = 0.0f;
        chara_tc_param = 0.0f;
    }

    vec4 light_reflect_dir;
    lights[LIGHT_CHARA].get_position(light_reflect_dir);
    if (fabsf(light_reflect_dir.x) <= 0.000001f && fabsf(light_reflect_dir.y) <= 0.000001f
        && fabsf(light_reflect_dir.z) <= 0.000001f)
        *(vec3*)&light_reflect_dir = vec3(0.0f, 1.0f, 0.0f);
    else {
        float_t length = vec3::length(*(vec3*)&light_reflect_dir);
        if (length != 0.0f)
            *(vec3*)&light_reflect_dir *= 1.0f / length;
    }
    light_reflect_dir.w = 1.0f;

    vec4 clip_plane;
    if (lights[LIGHT_REFLECT].get_type() == LIGHT_SPOT) {
        vec3 spot_direction;
        vec4 position;
        lights[LIGHT_REFLECT].get_spot_direction(spot_direction);
        lights[LIGHT_REFLECT].get_position(position);
        *(vec3*)&clip_plane =  -spot_direction;
        light_get_direction_from_position(&clip_plane, &lights[LIGHT_REFLECT], true);
        clip_plane.w = -vec3::dot(*(vec3*)&position, *(vec3*)&clip_plane);
    }
    else if (lights[LIGHT_REFLECT].get_type() == LIGHT_POINT)
        clip_plane = { 0.0f, -1.0f, 0.0f, 9999.0f };
    else
        clip_plane = { 0.0f, -1.0f, 0.0f, 0.0f };

    vec4 light_chara_ibl_direction;
    vec4 light_chara_position;
    lights[LIGHT_CHARA].get_ibl_direction(light_chara_ibl_direction);
    lights[LIGHT_CHARA].get_position(light_chara_position);

    mat4 normal_tangent_transforms = mat4_identity;

    float_t length = vec3::length(*(vec3*)&light_chara_ibl_direction);
    if (length >= 0.000001f) {
        vec3 ibl_direction = *(vec3*)&light_chara_ibl_direction * (1.0f / length);

        length = vec3::length(*(vec3*)&light_chara_position);
        if (length >= 0.000001f) {
            vec3 position = *(vec3*)&light_chara_position * (1.0f / length);

            vec3 axis = vec3::cross(ibl_direction, position);
            length = vec3::length(axis);

            float_t v52 = vec3::dot(ibl_direction, position);
            float_t angle = fabsf(atan2f(length, v52));
            if (angle >= 0.01f && angle <= 3.131592653589793f) {
                if (length != 0.0f)
                    axis *= 1.0f / length;

                mat4_set(&axis, -angle, &normal_tangent_transforms);
            }
        }
    }

    rend_data_ctx.set_scene_light(irradiance_r_transforms, irradiance_g_transforms, irradiance_b_transforms,
        light_env_stage_diffuse, light_env_stage_specular, light_env_chara_diffuse,
        light_env_chara_ambient, light_env_chara_specular,
        light_env_reflect_diffuse, light_env_reflect_ambient,
        light_env_proj_diffuse, light_env_proj_specular,
        light_env_proj_position, light_stage_dir, light_stage_diff,
        light_stage_spec, light_chara_dir, light_chara_spec,
        light_chara_luce, light_chara_back, light_face_diff,
        chara_color0, chara_color1, chara_f_dir, chara_f_ambient,
        chara_f_diffuse, chara_tc_param, normal_tangent_transforms,
        light_reflect_dir, clip_plane, npr_cloth_spec_color);

    extern const GLuint* light_param_data_storage_data_get_ibl_textures();
    const GLuint* textures = light_param_data_storage_data_get_ibl_textures();
    static const int32_t ibl_texture_index[] = {
        9, 10, 11, 12, 13
    };

    for (int32_t i = 0; i < 5; i++)
        rend_data_ctx.state.active_bind_texture_cube_map(ibl_texture_index[i], textures[i]);
}

static void light_get_direction_from_position(vec4* pos_dir, light_data* light, bool force) {
    if (force || light->get_type() == LIGHT_PARALLEL) {
        float_t length = vec3::length(*(vec3*)pos_dir);
        if (length > 0.000001f)
            *(vec3*)pos_dir *= 1.0f / length;
        else
            *(vec3*)pos_dir = { 0.0f, 1.0f, 0.0f };
        pos_dir->w = 0.0f;
    }
}

static void light_get_light_color(light_data* light, light_data_color& value, light_id id) {
    if (id || light_chara_ambient)
        value.ambient = light->ambient;
    else
        value.ambient = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (!id && false)
        value.diffuse = light->diffuse * 4.0f;
    else
        value.diffuse = light->diffuse;

    value.specular = light->specular;
}
