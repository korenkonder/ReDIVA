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

post_process_tone_map::post_process_tone_map() : saturate_lock(), lens_shaft_scale(),
lens_flare_power(), lens_flare_appear_power(), saturate_index(), scene_fade_index(),
tone_trans_index(), saturate_coeff(), scene_fade_alpha(), scene_fade_blend_func(),
tone_map_method(), update(), shader_data(), tone_map_ubo(), tone_map_tex() {
    exposure = 2.0f;
    exposure_rate = 1.0f;
    auto_exposure = true;
    gamma = 1.0f;
    gamma_rate = 1.0f;
    saturate_power = 1;

    for (int32_t i = 0; i < 2; i++) {
        reset_saturate_coeff(i, 0);
        reset_scene_fade(i);
        reset_tone_trans(i);
    }

    update_lut = true;
    lens_shaft_inv_scale = 1.0f;
    lens_flare = 1.0f;
    lens_shaft = 1.0f;
    lens_ghost = 1.0f;

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

    post_process_tone_map_calculate_data(this);

    if (update_lut) {
        update_lut = false;
        saturate_lock = false;

        post_process_tone_map_calculate_tex(this);

        if (GLAD_GL_VERSION_4_5)
            glTextureSubImage2D(tone_map_tex, 0, 0, 0,
                16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES, 1, GL_RG, GL_FLOAT, tex_data);
        else {
            gl_state_bind_texture_2d(tone_map_tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES, 1, GL_RG, GL_FLOAT, tex_data);
            gl_state_bind_texture_2d(0);
        }
    }

    uniform_value[U_TONE_MAP] = (int32_t)tone_map_method;
    uniform_value[U_FLARE] = 0;
    uniform_value[U_SCENE_FADE] = scene_fade_alpha[scene_fade_index] > 0.009999999f ? 1 : 0;
    uniform_value[U_AET_BACK] = 0;
    uniform_value[U_LIGHT_PROJ] = 0;
    //uniform_value[U25] = 0;

    gl_state_active_bind_texture_2d(0, in_tex->GetColorTex());
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
        gl_state_active_bind_texture_2d(4, pp->lens_flare_texture);
        gl_state_bind_sampler(4, pp->samplers[2]);
        mat4 mat;
        mat4_translate(0.5f, 0.5f, 0.0f, &mat);
        mat4_scale_rot(&mat, 0.75f, 0.75f, 1.0f, &mat);
        mat4_mul_rotate_z(&mat, (pp->lens_flare_pos.x / (float_t)pp->render_width)
            * 25.0f * DEG_TO_RAD_FLOAT, &mat);
        mat4_mul_translate(&mat, -((1.0f / (float_t)pp->render_width) * pp->lens_flare_pos.x),
            (pp->lens_flare_pos.y - (float_t)pp->render_height)
                * (1.0f / (float_t)pp->render_width), 0.0f, &mat);
        mat4_scale_rot(&mat, 1.0f, aspect, 1.0f, &mat);
        mat4_transpose(&mat, &mat);
        shader_data.g_texcoord_transforms[4] = mat.row0;
        shader_data.g_texcoord_transforms[5] = mat.row1;

        if (pp->lens_shaft_scale < 50.0f) {
            uniform_value[U_FLARE] = 2;
            gl_state_active_bind_texture_2d(5, pp->lens_shaft_texture);
            gl_state_bind_sampler(5, pp->samplers[2]);
            mat4_translate(0.5f, 0.5f, 0.0f, &mat);
            mat4_scale_rot(&mat, pp->lens_shaft_scale, pp->lens_shaft_scale, 1.0f, &mat);
            mat4_mul_rotate_z(&mat, (pp->lens_flare_pos.x / (float_t)pp->render_width)
                * 60.0f * DEG_TO_RAD_FLOAT, &mat);
            mat4_mul_translate(&mat, -((1.0f / (float_t)pp->render_width) * pp->lens_flare_pos.x),
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
        /*gl_state_active_bind_texture_2d(16, contour_rt->GetColorTex());
        gl_state_bind_sampler(16, pp->samplers[1]);
        gl_state_active_bind_texture_2d(17, contour_rt->GetDepthTex());
        gl_state_bind_sampler(17, pp->samplers[1]);*/
        gl_state_active_bind_texture_2d(14, rt->GetDepthTex());
        gl_state_bind_sampler(14, pp->samplers[1]);
    }

    tone_map_ubo.WriteMemory(shader_data);

    buf_rt->Bind();
    buf_rt->SetViewport();
    shaders_ft.set(SHADER_FT_TONEMAP);
    tone_map_ubo.Bind(1);
    RenderTexture::Draw(&shaders_ft);
    gl_state_active_bind_texture_2d(2, 0);

    fbo::blit(buf_rt->fbos[0], rt->fbos[0],
        0, 0, buf_rt->GetWidth(), buf_rt->GetHeight(),
        0, 0, rt->GetWidth(), rt->GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
    gl_state_end_event();
}

bool post_process_tone_map::get_auto_exposure() {
    return auto_exposure;
}

float_t post_process_tone_map::get_exposure() {
    return exposure;
}

float_t post_process_tone_map::get_exposure_rate() {
    return exposure_rate;
}

float_t post_process_tone_map::get_gamma() {
    return gamma;
}

float_t post_process_tone_map::get_gamma_rate() {
    return gamma_rate;
}

vec3 post_process_tone_map::get_lens() {
    vec3 value;
    value.x = lens_flare;
    value.y = lens_shaft;
    value.z = lens_ghost;
    return value;
}

float_t post_process_tone_map::get_lens_flare_appear_power() {
    return lens_flare_appear_power;
}

float_t post_process_tone_map::get_lens_flare_power() {
    return lens_flare_power;
}

float_t post_process_tone_map::get_saturate_coeff() {
    return saturate_coeff[0];
}

int32_t post_process_tone_map::get_saturate_power() {
    return saturate_power;
}

vec4 post_process_tone_map::get_scene_fade() {
    vec4 scene_fade;
    *(vec3*)&scene_fade = scene_fade_color[0];
    scene_fade.w = scene_fade_alpha[0];
    return scene_fade;
}

float_t post_process_tone_map::get_scene_fade_alpha() {
    return scene_fade_alpha[0];
}

int32_t post_process_tone_map::get_scene_fade_blend_func() {
    return scene_fade_blend_func[0];
}

vec3 post_process_tone_map::get_scene_fade_color() {
    return scene_fade_color[0];
}

::tone_map_method post_process_tone_map::get_tone_map_method() {
    return tone_map_method;
}

void post_process_tone_map::get_tone_trans(vec3& start, vec3& end) {
    start = tone_trans_start[0];
    end = tone_trans_end[0];
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

void post_process_tone_map::reset_saturate_coeff(int32_t index, bool lock) {
    set_saturate_coeff(1.0f, index, lock);

    if (index == 1)
        saturate_index = 0;
}

void post_process_tone_map::reset_scene_fade(int32_t index) {
    scene_fade_color[index] = 0.0f;
    scene_fade_alpha[index] = 0.0f;
    scene_fade_blend_func[index] = 0;

    if (index == 1)
        scene_fade_index = 0;
    update = true;
}

void post_process_tone_map::reset_tone_trans(int32_t index) {
    tone_trans_scale[index] = 1.0f;
    tone_trans_offset[index] = 0.0f;
    tone_trans_start[index] = 0.0f;
    tone_trans_end[index] = 1.0f;

    if (index == 1)
        tone_trans_index = 0;
    update = true;
}

void post_process_tone_map::set_auto_exposure(bool value) {
    auto_exposure = value;
}

void post_process_tone_map::set_exposure(float_t value) {
    exposure = value;
}

void post_process_tone_map::set_exposure_rate(float_t value) {
    exposure_rate = value;
}

void post_process_tone_map::set_gamma(float_t value) {
    if (value != gamma) {
        gamma = value;
        update_lut = true;
    }
}

void post_process_tone_map::set_gamma_rate(float_t value) {
    if (value != gamma_rate) {
        gamma_rate = value;
        update_lut = true;
    }
}

void post_process_tone_map::set_lens(vec3 value) {
    lens_flare = value.x;
    lens_shaft = value.y;
    lens_ghost = value.z;
    update = true;
}

void post_process_tone_map::set_lens_flare_appear_power(float_t value) {
    lens_flare_appear_power = value;
}

void post_process_tone_map::set_lens_flare_power(float_t value) {
    lens_flare_power = value;
}

void post_process_tone_map::set_saturate_coeff(float_t value, int32_t index, bool lock) {
    if (saturate_lock)
        return;

    saturate_coeff[index] = value;
    update_lut = true;
    update = true;

    if (lock) {
        saturate_lock = true;
        saturate_index = 0;
    }
    else if (index == 1)
        saturate_index = 1;
}

void post_process_tone_map::set_saturate_power(int32_t value) {
    if (value != saturate_power) {
        saturate_power = value;
        update_lut = true;
    }
}

void post_process_tone_map::set_scene_fade(const vec4& value, int32_t index) {
    scene_fade_color[index] = *(vec3*)&value;
    scene_fade_alpha[index] = value.w;

    if (index == 1)
        scene_fade_index = 1;
    update = true;
}

void post_process_tone_map::set_scene_fade_alpha(float_t value, int32_t index) {
    scene_fade_alpha[index] = value;

    if (index == 1)
        scene_fade_index = 1;
    update = true;
}

void post_process_tone_map::set_scene_fade_blend_func(int32_t value, int32_t index) {
    if (value < 0 || value > 2)
        return;

    scene_fade_blend_func[index] = value;

    if (index == 1)
        scene_fade_index = 1;
    update = true;
}


void post_process_tone_map::set_scene_fade_color(const vec3& value, int32_t index) {
    scene_fade_color[index] = value;

    if (index == 1)
        scene_fade_index = 1;
    update = true;
}

void post_process_tone_map::set_tone_map_method(::tone_map_method value) {
    tone_map_method = value;
}

void post_process_tone_map::set_tone_trans(const vec3& start, const vec3& end, int32_t index) {
    for (int32_t i = 0; i < 3; i++)
        if (fabsf((&end.x)[i] - (&start.x)[i]) < 0.0001f) {
            reset_tone_trans(0);
            return;
        }

    tone_trans_end[index] = end;
    tone_trans_start[index] = start;

    vec3 scale = vec3::rcp(end - start);
    tone_trans_scale[index] = scale;
    tone_trans_offset[index] = -(scale * start);

    if (index == 1)
        tone_trans_index = 1;
    update = true;
}

static void post_process_tone_map_calculate_data(post_process_tone_map* tm) {
    int32_t scene_fade_blend_func = tm->scene_fade_blend_func[tm->scene_fade_index];

    post_process_tone_map_shader_data* tmshd = &tm->shader_data;
    tmshd->g_exposure.x = tm->exposure * tm->exposure_rate;
    tmshd->g_exposure.y = 0.0625f;
    tmshd->g_exposure.z = tm->exposure * tm->exposure_rate * 0.5f;
    tmshd->g_exposure.w = tm->auto_exposure ? 1.0f : 0.0f;
    tmshd->g_flare_coef.x = (tm->lens_flare * 2.0f) * (tm->lens_flare_appear_power + tm->lens_flare_power);
    tmshd->g_flare_coef.y = tm->lens_shaft * 2.0f;
    tmshd->g_flare_coef.z = 0.0f;
    tmshd->g_flare_coef.w = 0.0f;
    *(vec3*)&tmshd->g_fade_color = tm->scene_fade_color[tm->scene_fade_index];
    tmshd->g_fade_color.w = tm->scene_fade_alpha[tm->scene_fade_index];
    if (scene_fade_blend_func == 1 || scene_fade_blend_func == 2)
        *(vec3*)&tmshd->g_fade_color = *(vec3*)&tmshd->g_fade_color * tmshd->g_fade_color.w;
    *(vec3*)&tmshd->g_tone_scale = tm->tone_trans_scale[tm->tone_trans_index];
    *(vec3*)&tmshd->g_tone_offset = tm->tone_trans_offset[tm->tone_trans_index];
    tmshd->g_tone_scale.w = (float_t)scene_fade_blend_func;
    tmshd->g_tone_offset.w = tm->gamma > 0.0f ? 2.0f / (tm->gamma * 3.0f) : 0.0f;
}

static void post_process_tone_map_calculate_tex(post_process_tone_map* tm) {
    const float_t post_process_tone_map_scale = (float_t)(1.0 / (double_t)POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES);
    const int32_t post_process_tone_map_size = 16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES;

    int32_t i, j;
    int32_t saturate_power;
    float_t saturate_coeff;

    vec2* tex_data = tm->tex_data;
    float_t gamma_power, gamma, saturation;
    gamma_power = tm->gamma * tm->gamma_rate * 1.5f;
    saturate_power = tm->saturate_power;
    saturate_coeff = tm->saturate_coeff[tm->saturate_index];

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
