/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "tone_map.hpp"
#include "../fbo.hpp"
#include "../gl_state.hpp"
#include "../post_process.hpp"
#include "../render_context.hpp"
#include "../shader_ft.hpp"

static void post_process_tone_map_calculate_data(post_process_tone_map* tm);
static void post_process_tone_map_calculate_tex(post_process_tone_map* tm);

extern render_context* rctx_ptr;

post_process_tone_map_data::post_process_tone_map_data() : tex_data(), exposure(), auto_exposure(), gamma(),
gamma_rate(), saturate_power(), saturate_coeff(), scene_fade_blend_func(), tone_map_method(), lens_flare(),
lens_shaft(), lens_ghost(), lens_flare_power(), lens_flare_appear_power(), update(), update_tex() {

}

post_process_tone_map::post_process_tone_map() : shader_data(), tone_map_ubo(), tone_map_tex() {
    tone_map_ubo.Create(sizeof(post_process_tone_map_shader_data));
}

post_process_tone_map::~post_process_tone_map() {
    if (!this)
        return;

    tone_map_ubo.Destroy();
    glDeleteTextures(1, &tone_map_tex);
}

void post_process_tone_map::apply(RenderTexture* in_tex, texture* light_proj_tex, texture* back_2d_tex,
    RenderTexture* rt, RenderTexture* buf_rt,/* RenderTexture* contour_rt,*/
    GLuint in_tex_0, GLuint in_tex_1, int32_t npr_param, void* pp_data) {
    if (!this)
        return;

    gl_state_begin_event("PostProcess::tone_map");
    post_process* pp = (post_process*)pp_data;

    set_lens_flare_power(pp->lens_flare_power);
    set_lens_flare_appear_power(pp->lens_flare_appear_power);

    if (data.update) {
        post_process_tone_map_calculate_data(this);
        data.update = false;
    }

    if (data.update_tex) {
        post_process_tone_map_calculate_tex(this);

        if (GLAD_GL_VERSION_4_5)
            glTextureSubImage2D(tone_map_tex, 0, 0, 0,
                16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES, 1, GL_RG, GL_FLOAT, data.tex_data);
        else {
            gl_state_bind_texture_2d(tone_map_tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES, 1, GL_RG, GL_FLOAT, data.tex_data);
            gl_state_bind_texture_2d(0);
        }
        data.update_tex = false;
    }

    uniform_value[U_TONE_MAP] = (int32_t)data.tone_map_method;
    uniform_value[U_FLARE] = 0;
    uniform_value[U_SCENE_FADE] = data.scene_fade.w > 0.009999999f ? 1 : 0;
    uniform_value[U_AET_BACK] = 0;
    uniform_value[U_LIGHT_PROJ] = 0;
    //uniform_value[U25] = 0;

    gl_state_active_bind_texture_2d(0, in_tex->color_texture->tex);
    gl_state_active_bind_texture_2d(1, in_tex_0);
    gl_state_active_bind_texture_2d(2, tone_map_tex);
    gl_state_active_bind_texture_2d(3, in_tex_1);
    gl_state_active_bind_texture_2d(4, rctx_ptr->empty_texture_2d);
    gl_state_active_bind_texture_2d(5, rctx_ptr->empty_texture_2d);
    gl_state_active_bind_texture_2d(6, rctx_ptr->empty_texture_2d);

    if (back_2d_tex) {
        gl_state_active_bind_texture_2d(6, back_2d_tex->tex);
        uniform_value[U_AET_BACK] = 1;
    }

    if (light_proj_tex) {
        gl_state_active_bind_texture_2d(6, light_proj_tex->tex);
        uniform_value[U_LIGHT_PROJ] = 1;
    }

    shader_data.g_texcoord_transforms[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
    shader_data.g_texcoord_transforms[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
    shader_data.g_texcoord_transforms[2] = { 1.0f, 0.0f, 0.0f, 0.0f };
    shader_data.g_texcoord_transforms[3] = { 0.0f, 1.0f, 0.0f, 0.0f };

    if (pp->lens_flare_texture) {
        static const vec4 border_color = 0.0f;

        const float_t aspect = (float_t)pp->render_height / (float_t)pp->render_width;

        uniform_value[U_FLARE] = 1;
        gl_state_active_bind_texture_2d(4, pp->lens_flare_texture->tex);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        mat4 mat;
        mat4_translate(0.5f, 0.5f, 0.0f, &mat);
        mat4_scale_rot(&mat, 0.75f, 0.75f, 1.0f, &mat);
        mat4_rotate_z_mult(&mat, (pp->lens_flare_pos.x / (float_t)pp->render_width)
            * 25.0f * DEG_TO_RAD_FLOAT, &mat);
        mat4_translate_mult(&mat, -((1.0f / (float_t)pp->render_width) * pp->lens_flare_pos.x),
            (pp->lens_flare_pos.y - (float_t)pp->render_height)
                * (1.0f / (float_t)pp->render_width), 0.0f, &mat);
        mat4_scale_rot(&mat, 1.0f, aspect, 1.0f, &mat);
        mat4_transpose(&mat, &mat);
        shader_data.g_texcoord_transforms[4] = mat.row0;
        shader_data.g_texcoord_transforms[5] = mat.row1;

        if (pp->lens_shaft_scale < 50.0f) {
            uniform_value[U_FLARE] = 2;
            gl_state_active_bind_texture_2d(5, pp->lens_shaft_texture->tex);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            mat4_translate(0.5f, 0.5f, 0.0f, &mat);
            mat4_scale_rot(&mat, pp->lens_shaft_scale, pp->lens_shaft_scale, 1.0f, &mat);
            mat4_rotate_z_mult(&mat, (pp->lens_flare_pos.x / (float_t)pp->render_width)
                * 60.0f * DEG_TO_RAD_FLOAT, &mat);
            mat4_translate_mult(&mat, -((1.0f / (float_t)pp->render_width) * pp->lens_flare_pos.x),
                (pp->lens_flare_pos.y - (float_t)pp->render_height)
                    * (1.0f / (float_t)pp->render_width), 0.0f, &mat);
            mat4_scale_rot(&mat, 1.0f, aspect, 1.0f, &mat);
            mat4_transpose(&mat, &mat);
            shader_data.g_texcoord_transforms[6] = mat.row0;
            shader_data.g_texcoord_transforms[7] = mat.row1;
        }
        else {
            shader_data.g_texcoord_transforms[6] = { 1.0f, 0.0f, 0.0f, 0.0f };
            shader_data.g_texcoord_transforms[7] = { 0.0f, 1.0f, 0.0f, 0.0f };
        }
    }
    else {
        shader_data.g_texcoord_transforms[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        shader_data.g_texcoord_transforms[5] = { 0.0f, 1.0f, 0.0f, 0.0f };
        shader_data.g_texcoord_transforms[6] = { 1.0f, 0.0f, 0.0f, 0.0f };
        shader_data.g_texcoord_transforms[7] = { 0.0f, 1.0f, 0.0f, 0.0f };
    }

    if (npr_param == 1) {
        /*gl_state_active_bind_texture_2d(16, contour_rt->color_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_state_active_bind_texture_2d(17, contour_rt->depth_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
        gl_state_active_bind_texture_2d(14, rt->depth_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    tone_map_ubo.WriteMemory(shader_data);

    glViewport(0, 0, rt->color_texture->width, rt->color_texture->height);
    buf_rt->Bind();
    shaders_ft.set(SHADER_FT_TONEMAP);
    tone_map_ubo.Bind(1);
    RenderTexture::Draw(&shaders_ft);
    gl_state_active_bind_texture_2d(2, 0);

    fbo::blit(buf_rt->fbos[0], rt->fbos[0],
        0, 0, buf_rt->color_texture->width, buf_rt->color_texture->height,
        0, 0, rt->color_texture->width, rt->color_texture->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    gl_state_end_event();
}

void post_process_tone_map::init_fbo() {
    if (!this)
        return;

    if (!tone_map_tex) {
        if (GLAD_GL_VERSION_4_5) {
            glCreateTextures(GL_TEXTURE_2D, 1, &tone_map_tex);

            glTextureStorage2D(tone_map_tex, 1, GL_RG16F, 16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES, 1);
            glTextureParameteri(tone_map_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(tone_map_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            const GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
            glTextureParameteri(tone_map_tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(tone_map_tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteriv(tone_map_tex, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        }
        else {
            glGenTextures(1, &tone_map_tex);

            gl_state_bind_texture_2d(tone_map_tex);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG16F, 16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            const GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
            gl_state_bind_texture_2d(0);
        }
    }
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
    *(vec3*)&data.scene_fade.x = vec3::clamp(scene_fade_color, 0.0f, 1.0f);
    data.scene_fade.w = clamp_def(scene_fade_alpha, 0.0f, 1.0f);
    data.scene_fade_blend_func = clamp_def(scene_fade_blend_func, 0, 2);
    data.tone_trans_start = vec3::clamp(tone_trans_start, 0.0f, 1.0f);
    data.tone_trans_end = vec3::clamp(tone_trans_end, 0.0f, 1.0f);
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
    if (value != data.lens_flare) {
        data.lens_flare = value;
        data.update = true;
    }
}

float_t post_process_tone_map::get_lens_flare_appear_power() {
    return data.lens_flare_appear_power;
}

void post_process_tone_map::set_lens_flare_appear_power(float_t value) {
    if (value != data.lens_flare_appear_power) {
        data.lens_flare_appear_power = value;
        data.update = true;
    }
}

float_t post_process_tone_map::get_lens_flare_power() {
    return data.lens_flare_power;
}

void post_process_tone_map::set_lens_flare_power(float_t value) {
    if (value != data.lens_flare_power) {
        data.lens_flare_power = value;
        data.update = true;
    }
}

float_t post_process_tone_map::get_lens_ghost() {
    return data.lens_ghost;
}

void post_process_tone_map::set_lens_ghost(float_t value) {
    data.lens_ghost = value;
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
    vec4 temp = vec4::clamp(value, 0.0f, 1.0f);
    if (temp != data.scene_fade) {
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
    vec3 temp = vec3::clamp(value, 0.0f, 1.0f);
    if (temp != *(vec3*)&data.scene_fade) {
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
    vec3 temp = vec3::clamp(value, 0.0f, 1.0f);
    if (temp != data.tone_trans_end) {
        data.tone_trans_end = temp;
        data.update = true;
    }
}

vec3 post_process_tone_map::get_tone_trans_start() {
    return data.tone_trans_start;
}

void post_process_tone_map::set_tone_trans_start(const vec3& value) {
    vec3 temp = vec3::clamp(value, 0.0f, 1.0f);
    if (temp != data.tone_trans_start) {
        data.tone_trans_start = temp;
        data.update = true;
    }
}

static void post_process_tone_map_calculate_data(post_process_tone_map* tm) {
    vec3 tone_trans_scale = vec3::rcp(tm->data.tone_trans_end - tm->data.tone_trans_start);
    vec3 tone_trans_offset = -(tone_trans_scale * tm->data.tone_trans_start);

    post_process_tone_map_shader_data* tmshd = &tm->shader_data;
    tmshd->g_exposure.x = tm->data.exposure;
    tmshd->g_exposure.y = 0.0625f;
    tmshd->g_exposure.z = tm->data.exposure * 0.5f;
    tmshd->g_exposure.w = tm->data.auto_exposure ? 1.0f : 0.0f;
    tmshd->g_flare_coef.x = (tm->data.lens_flare * 2.0f)
        * (tm->data.lens_flare_appear_power + tm->data.lens_flare_power);
    tmshd->g_flare_coef.y = tm->data.lens_shaft * 2.0f;
    tmshd->g_flare_coef.z = 0.0f;
    tmshd->g_flare_coef.w = 0.0f;
    tmshd->g_fade_color = tm->data.scene_fade;
    if (tm->data.scene_fade_blend_func == 1 || tm->data.scene_fade_blend_func == 2)
        *(vec3*)&tmshd->g_fade_color = *(vec3*)&tmshd->g_fade_color * tmshd->g_fade_color.w;
    tmshd->g_tone_scale.x = tone_trans_scale.x;
    tmshd->g_tone_scale.y = tone_trans_scale.y;
    tmshd->g_tone_scale.z = tone_trans_scale.z;
    tmshd->g_tone_offset.x = tone_trans_offset.x;
    tmshd->g_tone_offset.y = tone_trans_offset.y;
    tmshd->g_tone_offset.z = tone_trans_offset.z;
    tmshd->g_tone_scale.w = (float_t)tm->data.scene_fade_blend_func;
    tmshd->g_tone_offset.w = tm->data.gamma > 0.0f ? 2.0f / (tm->data.gamma * 3.0f) : 0.0f;
}

static void post_process_tone_map_calculate_tex(post_process_tone_map* tm) {
    const float_t post_process_tone_map_scale = (float_t)(1.0 / (double_t)POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES);
    const int32_t post_process_tone_map_size = 16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES;

    int32_t i, j;
    int32_t saturate_power;
    float_t saturate_coeff;

    vec2* tex_data = tm->data.tex_data;
    float_t gamma_power, gamma, saturation;
    gamma_power = tm->data.gamma * tm->data.gamma_rate * 1.5f;
    saturate_power = tm->data.saturate_power;
    saturate_coeff = tm->data.saturate_coeff;

    tex_data->x = 0.0f;
    tex_data->y = 0.0f;
    tex_data++;
    for (i = 1; i < post_process_tone_map_size; i++, tex_data++) {
        gamma = powf(1.0f - expf(-i * post_process_tone_map_scale), gamma_power);
        saturation = gamma * 2.0f - 1.0f;
        for (j = 0; j < saturate_power; j++) {
            saturation *= saturation;
            saturation *= saturation;
            saturation *= saturation;
            saturation *= saturation;
        }

        tex_data->x = gamma;
        tex_data->y = gamma * saturate_coeff
            * ((float_t)POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES / (float_t)i) * (1.0f - saturation);
    }
}
