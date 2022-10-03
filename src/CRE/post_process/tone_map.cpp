/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "tone_map.hpp"
#include "../fbo.hpp"
#include "../gl_state.hpp"
#include "../post_process.hpp"
#include "../shader_ft.hpp"

static void post_process_tone_map_calculate_data(post_process_tone_map* tm);
static void post_process_tone_map_calculate_tex(post_process_tone_map* tm);

post_process_tone_map_data::post_process_tone_map_data() : tex(), exposure(), auto_exposure(), gamma(),
gamma_rate(), saturate_power(), saturate_coeff(), scene_fade_blend_func(), tone_map_method(), lens_flare(),
lens_shaft(), lens_ghost(), lens_flare_power(), lens_flare_appear_power(), update(), update_tex() {

}

post_process_tone_map::post_process_tone_map() :  shader_data(), tone_map() {

}

post_process_tone_map::~post_process_tone_map() {
    if (!this)
        return;

    glDeleteTextures(1, &tone_map);
}

void post_process_tone_map::apply(render_texture* in_tex, texture* light_proj_tex, texture* back_2d_tex,
    render_texture* rt, render_texture* buf_rt, render_texture* contour_rt,
    GLuint in_tex_0, GLuint in_tex_1, int32_t npr_param) {
    if (!this)
        return;

    if (data.update) {
        post_process_tone_map_calculate_data(this);
        data.update = false;
    }

    if (data.update_tex) {
        post_process_tone_map_calculate_tex(this);

        glBindTexture(GL_TEXTURE_1D, tone_map);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RG16F,
            16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES, 0, GL_RG, GL_FLOAT, data.tex);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteriv(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        glBindTexture(GL_TEXTURE_1D, 0);
        data.update_tex = false;
    }

    mat4 lens_flare_mat;
    mat4 lens_shaft_mat;
    mat4_scale(1.0f, (float_t)rt->color_texture->height
        / (float_t)rt->color_texture->width, 1.0f, &lens_flare_mat);
    mat4_scale(1.0f, (float_t)rt->color_texture->height
        / (float_t)rt->color_texture->width, 1.0f, &lens_shaft_mat);

    uniform_value[U16] = 0;
    uniform_value[U_TONE_MAP] = (int32_t)data.tone_map_method;
    uniform_value[U_FLARE] = 0;
    uniform_value[U_SCENE_FADE] = data.scene_fade.w > 0.009999999f ? 1 : 0;
    uniform_value[U_AET_BACK] = 0;
    uniform_value[U_LIGHT_PROJ] = 0;
    uniform_value[U_NPR] = 0;
    uniform_value[U25] = 0;

    glViewport(0, 0, rt->color_texture->width, rt->color_texture->height);
    buf_rt->bind();
    shaders_ft.set(SHADER_FT_TONEMAP);
    shaders_ft.state_matrix_set_texture(4, lens_flare_mat);
    shaders_ft.state_matrix_set_texture(5, lens_shaft_mat);
    shaders_ft.local_vert_set(1, shader_data.p_exposure);
    shaders_ft.local_frag_set(1, shader_data.p_flare_coef);
    shaders_ft.local_frag_set(2, shader_data.p_fade_color);
    shaders_ft.local_frag_set(4, shader_data.p_tone_scale);
    shaders_ft.local_frag_set(5, shader_data.p_tone_offset);
    shaders_ft.local_frag_set(6, shader_data.p_fade_func);
    shaders_ft.local_frag_set(7, shader_data.p_inv_tone);
    gl_state_active_bind_texture_2d(0, in_tex->color_texture->tex);
    gl_state_active_bind_texture_2d(1, in_tex_0);
    gl_state_active_bind_texture_2d(2, 0);
    gl_state_active_texture(2);
    glBindTexture(GL_TEXTURE_1D, tone_map);
    gl_state_active_bind_texture_2d(3, in_tex_1);
    if (uniform_value[U_LIGHT_PROJ] && light_proj_tex)
        gl_state_active_bind_texture_2d(6, light_proj_tex->tex);
    else if (uniform_value[U_AET_BACK] && back_2d_tex)
        gl_state_active_bind_texture_2d(6, back_2d_tex->tex);
    if (npr_param == 1) {
        gl_state_active_bind_texture_2d(16, contour_rt->color_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_state_active_bind_texture_2d(17, contour_rt->depth_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_state_active_bind_texture_2d(14, rt->depth_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    render_texture::draw_custom(&shaders_ft);
    gl_state_active_texture(2);
    glBindTexture(GL_TEXTURE_1D, 0);
    uniform_value[U16] = 1;

    fbo::blit(buf_rt->fbos[0], rt->fbos[0],
        0, 0, buf_rt->color_texture->width, buf_rt->color_texture->height,
        0, 0, rt->color_texture->width, rt->color_texture->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void post_process_tone_map::init_fbo() {
    if (!this)
        return;

    if (!tone_map)
        glGenTextures(1, &tone_map);
}

void post_process_tone_map::initialize_data(float_t exposure, bool auto_exposure,
    float_t gamma, int32_t saturate_power, float_t saturate_coeff,
    const vec3& scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    const vec3& tone_trans_start, const vec3& tone_trans_end, tone_map_method tone_map_method) {
    initialize_data(exposure, auto_exposure, 1.0f, gamma, saturate_power, saturate_coeff,
        scene_fade_color, scene_fade_alpha, scene_fade_blend_func,
        tone_trans_start, tone_trans_end, tone_map_method);
}

void post_process_tone_map::initialize_data(float_t exposure, bool auto_exposure,
    float_t gamma, float_t gamma_rate, int32_t saturate_power, float_t saturate_coeff,
    const vec3& scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    const vec3& tone_trans_start, const vec3& tone_trans_end, tone_map_method tone_map_method) {
    data.exposure = clamp_def(exposure, 0.0f, 4.0f);
    data.auto_exposure = auto_exposure;
    data.gamma = clamp_def(gamma, 0.2f, 2.2f);
    data.gamma_rate = clamp_def(gamma_rate, 0.5f, 2.0f);
    data.saturate_power = clamp_def(saturate_power, 1, 6);
    data.saturate_coeff = clamp_def(saturate_coeff, 0.0f, 1.0f);
    data.scene_fade.x = clamp_def(scene_fade_color.x, 0.0f, 1.0f);
    data.scene_fade.y = clamp_def(scene_fade_color.y, 0.0f, 1.0f);
    data.scene_fade.z = clamp_def(scene_fade_color.z, 0.0f, 1.0f);
    data.scene_fade.w = clamp_def(scene_fade_alpha, 0.0f, 1.0f);
    data.scene_fade_blend_func = clamp_def(scene_fade_blend_func, 0, 2);
    data.tone_trans_start.x = clamp_def(tone_trans_start.x, 0.0f, 1.0f);
    data.tone_trans_start.y = clamp_def(tone_trans_start.y, 0.0f, 1.0f);
    data.tone_trans_start.z = clamp_def(tone_trans_start.z, 0.0f, 1.0f);
    data.tone_trans_end.x = clamp_def(tone_trans_end.x, 0.0f, 1.0f);
    data.tone_trans_end.y = clamp_def(tone_trans_end.y, 0.0f, 1.0f);
    data.tone_trans_end.z = clamp_def(tone_trans_end.z, 0.0f, 1.0f);
    data.tone_map_method = clamp_def(tone_map_method, TONE_MAP_YCC_EXPONENT, TONE_MAP_RGB_LINEAR2);
    data.update_tex = false;
    data.update = true;
    data.update_tex = true;
}

bool post_process_tone_map::get_auto_exposure() {
    return data.auto_exposure;
}

void post_process_tone_map::set_auto_exposure(bool value) {
    if (value != data.auto_exposure) {
        data.auto_exposure = value;
        data.update = true;
        data.update_tex = true;
    }
}

float_t post_process_tone_map::get_exposure() {
    return data.exposure;
}

void post_process_tone_map::set_exposure(float_t value) {
    value = clamp_def(value, 0.0f, 4.0f);
    if (value != data.exposure) {
        data.exposure = value;
        data.update = true;
        data.update_tex = true;
    }
}

float_t post_process_tone_map::get_gamma() {
    return data.gamma;
}

void post_process_tone_map::set_gamma(float_t value) {
    value = clamp_def(value, 0.2f, 2.2f);
    if (value != data.gamma) {
        data.gamma = value;
        data.update = true;
        data.update_tex = true;
    }
}

float_t post_process_tone_map::get_gamma_rate() {
    return data.gamma_rate;
}

void post_process_tone_map::set_gamma_rate(float_t value) {
    value = clamp_def(value, 0.5f, 2.0f);
    if (value != data.gamma_rate) {
        data.gamma_rate = value;
        data.update_tex = true;
    }
}

float_t post_process_tone_map::get_lens_flare() {
    return data.lens_flare;
}

void post_process_tone_map::set_lens_flare(float_t value) {
    value = clamp_def(value, 0.0f, 1.0f);
    if (value != data.lens_flare) {
        data.lens_flare = value;
        data.update = true;
    }
}

float_t post_process_tone_map::get_lens_ghost() {
    return data.lens_ghost;
}

void post_process_tone_map::set_lens_ghost(float_t value) {
    data.lens_ghost = clamp_def(value, 0.0f, 1.0f);
}

float_t post_process_tone_map::get_lens_shaft() {
    return data.lens_shaft;
}

void post_process_tone_map::set_lens_shaft(float_t value) {
    value = clamp_def(value, 0.0f, 1.0f);
    if (value != data.lens_shaft) {
        data.lens_shaft = value;
        data.update = true;
    }
}

float_t post_process_tone_map::get_saturate_coeff() {
    return data.saturate_coeff;
}

void post_process_tone_map::set_saturate_coeff(float_t value) {
    value = clamp_def(value, 0.0f, 1.0f);
    if (value != data.saturate_coeff) {
        data.saturate_coeff = value;
        data.update_tex = true;
    }
}

int32_t post_process_tone_map::get_saturate_power() {
    return data.saturate_power;
}

void post_process_tone_map::set_saturate_power(int32_t value) {
    value = clamp_def(value, 1, 6);
    if (value != data.saturate_power) {
        data.saturate_power = value;
        data.update_tex = true;
    }
}

vec4 post_process_tone_map::get_scene_fade() {
    return data.scene_fade;
}

void post_process_tone_map::set_scene_fade(const vec4& value) {
    vec4 temp;
    temp.x = clamp_def(value.x, 0.0f, 1.0f);
    temp.y = clamp_def(value.y, 0.0f, 1.0f);
    temp.z = clamp_def(value.z, 0.0f, 1.0f);
    temp.z = clamp_def(value.w, 0.0f, 1.0f);
    if (memcmp(&temp, &data.scene_fade, sizeof(vec4))) {
        data.scene_fade = temp;
        data.update = true;
    }
}

float_t post_process_tone_map::get_scene_fade_alpha() {
    return data.scene_fade.w;
}

void post_process_tone_map::set_scene_fade_alpha(float_t value) {
    value = clamp_def(value, 0.0f, 1.0f);
    if (value != data.scene_fade.w) {
        data.scene_fade.w = value;
        data.update = true;
    }
}

int32_t post_process_tone_map::get_scene_fade_blend_func() {
    return data.scene_fade_blend_func;
}

void post_process_tone_map::set_scene_fade_blend_func(int32_t value) {
    value = clamp_def(value, 0, 2);
    if (value != data.scene_fade_blend_func) {
        data.scene_fade_blend_func = value;
        data.update = true;
    }
}

vec3 post_process_tone_map::get_scene_fade_color() {
    return *(vec3*)&data.scene_fade;
}

void post_process_tone_map::set_scene_fade_color(const vec3& value) {
    vec3 temp;
    temp.x = clamp_def(value.x, 0.0f, 1.0f);
    temp.y = clamp_def(value.y, 0.0f, 1.0f);
    temp.z = clamp_def(value.z, 0.0f, 1.0f);
    if (memcmp(&temp, &data.scene_fade, sizeof(vec3))) {
        *(vec3*)&data.scene_fade = temp;
        data.update = true;
    }
}

tone_map_method post_process_tone_map::get_tone_map_method() {
    return data.tone_map_method;
}

void post_process_tone_map::set_tone_map_method(tone_map_method value) {
    value = clamp_def(value, TONE_MAP_YCC_EXPONENT, TONE_MAP_RGB_LINEAR2);
    if (value != data.tone_map_method) {
        data.tone_map_method = value;
        data.update = true;
    }
}

void post_process_tone_map::get_tone_trans(vec3& start, vec3& end) {
    start = get_tone_trans_start();
    end = get_tone_trans_start();
}

void post_process_tone_map::set_tone_trans(const vec3& start, const vec3& end) {
    set_tone_trans_start(start);
    set_tone_trans_end(end);
}

vec3 post_process_tone_map::get_tone_trans_end() {
    return data.tone_trans_end;
}

void post_process_tone_map::set_tone_trans_end(const vec3& value) {
    vec3 temp;
    temp.x = clamp_def(value.x, 0.0f, 1.0f);
    temp.y = clamp_def(value.y, 0.0f, 1.0f);
    temp.z = clamp_def(value.z, 0.0f, 1.0f);
    if (memcmp(&temp, &data.tone_trans_end, sizeof(vec3))) {
        data.tone_trans_end = temp;
        data.update = true;
    }
}

vec3 post_process_tone_map::get_tone_trans_start() {
    return data.tone_trans_start;
}

void post_process_tone_map::set_tone_trans_start(const vec3& value) {
    vec3 temp;
    temp.x = clamp_def(value.x, 0.0f, 1.0f);
    temp.y = clamp_def(value.y, 0.0f, 1.0f);
    temp.z = clamp_def(value.z, 0.0f, 1.0f);
    if (memcmp(&temp, &data.tone_trans_start, sizeof(vec3))) {
        data.tone_trans_start = temp;
        data.update = true;
    }
}

static void post_process_tone_map_calculate_data(post_process_tone_map* tm) {
    vec3 tone_trans_scale = vec3::rcp(tm->data.tone_trans_end - tm->data.tone_trans_start);
    vec3 tone_trans_offset = -(tone_trans_scale * tm->data.tone_trans_start);

    post_process_tone_map_shader_data* v = &tm->shader_data;
    v->p_exposure.x = tm->data.exposure;
    v->p_exposure.y = 0.0625f;
    v->p_exposure.z = tm->data.exposure * 0.5f;
    v->p_exposure.w = tm->data.auto_exposure ? 1.0f : 0.0f;
    v->p_flare_coef.x = (tm->data.lens_flare * 2.0f)
        * (tm->data.lens_flare_appear_power + tm->data.lens_flare_power);
    v->p_flare_coef.y = tm->data.lens_shaft * 2.0f;
    v->p_flare_coef.z = 0.0f;
    v->p_flare_coef.w = 0.0f;
    v->p_fade_color = tm->data.scene_fade;
    if (tm->data.scene_fade_blend_func == 1 || tm->data.scene_fade_blend_func == 2)
        *(vec3*)&v->p_fade_color = *(vec3*)&v->p_fade_color * v->p_fade_color.w;
    v->p_tone_scale.x = tone_trans_scale.x;
    v->p_tone_scale.y = tone_trans_scale.y;
    v->p_tone_scale.z = tone_trans_scale.z;
    v->p_tone_offset.x = tone_trans_offset.x;
    v->p_tone_offset.y = tone_trans_offset.y;
    v->p_tone_offset.z = tone_trans_offset.z;
    v->p_fade_func.x = (float_t)tm->data.scene_fade_blend_func;
    v->p_inv_tone.x = tm->data.gamma > 0.0f ? 2.0f / (tm->data.gamma * 3.0f) : 0.0f;
}

static void post_process_tone_map_calculate_tex(post_process_tone_map* tm) {
    const float_t post_process_tone_map__scale = (float_t)(1.0 / (double_t)POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES);
    const int32_t post_process_tone_map__size = 16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES;

    int32_t i, j;
    int32_t saturate_power;
    float_t saturate_coeff;

    vec2* tex = tm->data.tex;
    float_t gamma_power, gamma, saturation;
    gamma_power = tm->data.gamma * tm->data.gamma_rate * 1.5f;
    saturate_power = tm->data.saturate_power;
    saturate_coeff = tm->data.saturate_coeff;

    tex[0].x = 0.0f;
    tex[0].y = 0.0f;
    for (i = 1; i < post_process_tone_map__size; i++) {
        gamma = powf(1.0f - expf(-i * post_process_tone_map__scale), gamma_power);
        saturation = gamma * 2.0f - 1.0f;
        for (j = 0; j < saturate_power; j++) {
            saturation *= saturation;
            saturation *= saturation;
            saturation *= saturation;
            saturation *= saturation;
        }

        tex[i].x = gamma;
        tex[i].y = gamma * saturate_coeff
            * ((float_t)POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES / (float_t)i) * (1.0f - saturation);
    }
}
