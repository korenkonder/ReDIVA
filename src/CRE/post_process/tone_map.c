/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "tone_map.h"
#include "../fbo.h"
#include "../gl_state.h"
#include "../post_process.h"
#include "../shader_ft.h"

static void post_process_tone_map_free_fbo(post_process_tone_map* tm);
static void post_process_tone_map_calculate_data(post_process_tone_map* tm);
static void post_process_tone_map_calculate_tex(post_process_tone_map* tm);

post_process_tone_map* post_process_tone_map_init() {
    post_process_tone_map* exp = force_malloc_s(post_process_tone_map, 1);
    return exp;
}

void post_process_apply_tone_map(post_process_tone_map* tm,
    render_texture* in_tex, texture* light_proj_tex, texture* back_2d_tex,
    render_texture* rt, render_texture* buf_rt, render_texture* contour_rt,
    GLuint in_tex_0, GLuint in_tex_1, int32_t npr_param) {
    if (!tm)
        return;

    if (tm->data.update) {
        post_process_tone_map_calculate_data(tm);
        tm->data.update = false;
    }

    if (tm->data.update_tex) {
        post_process_tone_map_calculate_tex(tm);

        glBindTexture(GL_TEXTURE_1D, tm->tone_map);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RG16F,
            16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES, 0, GL_RG, GL_FLOAT, tm->data.tex);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteriv(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        glBindTexture(GL_TEXTURE_1D, 0);
        tm->data.update_tex = false;
    }

    mat4 lens_flare_mat;
    mat4 lens_shaft_mat;
    mat4_scale(1.0f, (float_t)rt->color_texture->height
        / (float_t)rt->color_texture->width, 1.0f, &lens_flare_mat);
    mat4_scale(1.0f, (float_t)rt->color_texture->height
        / (float_t)rt->color_texture->width, 1.0f, &lens_shaft_mat);

    uniform_value[U16] = 0;
    uniform_value[U_TONE_MAP] = (int32_t)tm->data.tone_map_method;
    uniform_value[U_FLARE] = 0;
    uniform_value[U_SCENE_FADE] = tm->data.scene_fade.w > 0.009999999f ? 1 : 0;
    uniform_value[U_AET_BACK] = 0;
    uniform_value[U_LIGHT_PROJ] = 0;
    uniform_value[U_NPR] = 0;
    uniform_value[U25] = 0;

    glViewport(0, 0, rt->color_texture->width, rt->color_texture->height);
    render_texture_bind(buf_rt, 0);
    shader_set(&shaders_ft, SHADER_FT_TONEMAP);
    shader_state_matrix_set_texture(&shaders_ft, 4, &lens_flare_mat);
    shader_state_matrix_set_texture(&shaders_ft, 5, &lens_shaft_mat);
    shader_local_vert_set_ptr(&shaders_ft, 1, &tm->shader_data.p_exposure);
    shader_local_frag_set_ptr(&shaders_ft, 1, &tm->shader_data.p_flare_coef);
    shader_local_frag_set_ptr(&shaders_ft, 2, &tm->shader_data.p_fade_color);
    shader_local_frag_set_ptr(&shaders_ft, 4, &tm->shader_data.p_tone_scale);
    shader_local_frag_set_ptr(&shaders_ft, 5, &tm->shader_data.p_tone_offset);
    shader_local_frag_set_ptr(&shaders_ft, 6, &tm->shader_data.p_fade_func);
    shader_local_frag_set_ptr(&shaders_ft, 7, &tm->shader_data.p_inv_tone);
    gl_state_active_bind_texture_2d(0, in_tex->color_texture->texture);
    gl_state_active_bind_texture_2d(1, in_tex_0);
    gl_state_active_bind_texture_2d(2, 0);
    gl_state_active_texture(2);
    glBindTexture(GL_TEXTURE_1D, tm->tone_map);
    gl_state_active_bind_texture_2d(3, in_tex_1);
    if (uniform_value[U_LIGHT_PROJ] && light_proj_tex)
        gl_state_active_bind_texture_2d(6, light_proj_tex->texture);
    else if (uniform_value[U_AET_BACK] && back_2d_tex)
        gl_state_active_bind_texture_2d(6, back_2d_tex->texture);
    if (npr_param == 1) {
        gl_state_active_bind_texture_2d(16, contour_rt->color_texture->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_state_active_bind_texture_2d(17, contour_rt->depth_texture->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_state_active_bind_texture_2d(14, rt->depth_texture->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    render_texture_draw_custom(&shaders_ft);
    gl_state_active_texture(2);
    glBindTexture(GL_TEXTURE_1D, 0);
    uniform_value[U16] = 1;

    fbo_blit(buf_rt->fbos[0], rt->fbos[0],
        0, 0, buf_rt->color_texture->width, buf_rt->color_texture->height,
        0, 0, rt->color_texture->width, rt->color_texture->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void post_process_tone_map_init_fbo(post_process_tone_map* tm) {
    if (!tm)
        return;

    if (!tm->tone_map)
        glGenTextures(1, &tm->tone_map);
}

void post_process_tone_map_dispose(post_process_tone_map* tm) {
    if (!tm)
        return;

    post_process_tone_map_free_fbo(tm);
    free(tm);
}

static void post_process_tone_map_free_fbo(post_process_tone_map* tm) {
    glDeleteTextures(1, &tm->tone_map);
}

void post_process_tone_map_initialize_data(post_process_tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, int32_t saturate_power, float_t saturate_coeff,
    vec3* scene_fade, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, tone_map_method tone_map_method) {
    post_process_tone_map_initialize_rate(tm, exposure, auto_exposure, 1.0f,
        gamma, saturate_power, saturate_coeff, scene_fade, scene_fade_alpha, scene_fade_blend_func,
        tone_trans_start, tone_trans_end, tone_map_method);
}

void post_process_tone_map_initialize_rate(post_process_tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, float_t gamma_rate, int32_t saturate_power, float_t saturate_coeff,
    vec3* scene_fade, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, tone_map_method tone_map_method) {
    tm->data.exposure = clamp(exposure, 0.0f, 4.0f);
    tm->data.auto_exposure = auto_exposure;
    tm->data.gamma = clamp(gamma, 0.2f, 2.2f);
    tm->data.gamma_rate = clamp(gamma_rate, 0.5f, 2.0f);
    tm->data.saturate_power = clamp(saturate_power, 1, 6);
    tm->data.saturate_coeff = clamp(saturate_coeff, 0.0f, 1.0f);
    tm->data.scene_fade.x = clamp(scene_fade->x, 0.0f, 1.0f);
    tm->data.scene_fade.y = clamp(scene_fade->y, 0.0f, 1.0f);
    tm->data.scene_fade.z = clamp(scene_fade->z, 0.0f, 1.0f);
    tm->data.scene_fade.w = clamp(scene_fade_alpha, 0.0f, 1.0f);
    tm->data.scene_fade_blend_func = clamp(scene_fade_blend_func, 0, 2);
    tm->data.tone_trans_start.x = clamp(tone_trans_start->x, 0.0f, 1.0f);
    tm->data.tone_trans_start.y = clamp(tone_trans_start->y, 0.0f, 1.0f);
    tm->data.tone_trans_start.z = clamp(tone_trans_start->z, 0.0f, 1.0f);
    tm->data.tone_trans_end.x = clamp(tone_trans_end->x, 0.0f, 1.0f);
    tm->data.tone_trans_end.y = clamp(tone_trans_end->y, 0.0f, 1.0f);
    tm->data.tone_trans_end.z = clamp(tone_trans_end->z, 0.0f, 1.0f);
    tm->data.tone_map_method = clamp(tone_map_method, TONE_MAP_YCC_EXPONENT, TONE_MAP_RGB_LINEAR2);
    tm->data.update_tex = false;
    tm->data.update = true;
    tm->data.update_tex = true;
}

inline float_t post_process_tone_map_get_exposure(post_process_tone_map* tm) {
    return tm->data.exposure;
}

inline void post_process_tone_map_set_exposure(post_process_tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 4.0f);
    if (value != tm->data.exposure) {
        tm->data.exposure = value;
        tm->data.update = true;
        tm->data.update_tex = true;
    }
}

inline bool post_process_tone_map_get_auto_exposure(post_process_tone_map* tm) {
    return tm->data.auto_exposure;
}

inline void post_process_tone_map_set_auto_exposure(post_process_tone_map* tm, bool value) {
    if (value != tm->data.auto_exposure) {
        tm->data.auto_exposure = value;
        tm->data.update = true;
        tm->data.update_tex = true;
    }
}

inline float_t post_process_tone_map_get_gamma(post_process_tone_map* tm) {
    return tm->data.gamma;
}

inline void post_process_tone_map_set_gamma(post_process_tone_map* tm, float_t value) {
    value = clamp(value, 0.2f, 2.2f);
    if (value != tm->data.gamma) {
        tm->data.gamma = value;
        tm->data.update = true;
        tm->data.update_tex = true;
    }
}

inline float_t post_process_tone_map_get_gamma_rate(post_process_tone_map* tm) {
    return tm->data.gamma_rate;
}

inline void post_process_tone_map_set_gamma_rate(post_process_tone_map* tm, float_t value) {
    value = clamp(value, 0.5f, 2.0f);
    if (value != tm->data.gamma_rate) {
        tm->data.gamma_rate = value;
        tm->data.update_tex = true;
    }
}

inline int32_t post_process_tone_map_get_saturate_power(post_process_tone_map* tm) {
    return tm->data.saturate_power;
}

inline void post_process_tone_map_set_saturate_power(post_process_tone_map* tm, int32_t value) {
    value = clamp(value, 1, 6);
    if (value != tm->data.saturate_power) {
        tm->data.saturate_power = value;
        tm->data.update_tex = true;
    }
}

inline float_t post_process_tone_map_get_saturate_coeff(post_process_tone_map* tm) {
    return tm->data.saturate_coeff;
}

inline void post_process_tone_map_set_saturate_coeff(post_process_tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tm->data.saturate_coeff) {
        tm->data.saturate_coeff = value;
        tm->data.update_tex = true;
    }
}

inline void post_process_tone_map_get_scene_fade(post_process_tone_map* tm, vec4* value) {
    *value = tm->data.scene_fade;
}

inline void post_process_tone_map_set_scene_fade(post_process_tone_map* tm, vec4* value) {
    vec4 temp;
    temp.x = clamp(value->x, 0.0f, 1.0f);
    temp.y = clamp(value->y, 0.0f, 1.0f);
    temp.z = clamp(value->z, 0.0f, 1.0f);
    temp.w = clamp(value->w, 0.0f, 1.0f);
    if (temp.x != tm->data.scene_fade.x
        || temp.y != tm->data.scene_fade.y
        || temp.z != tm->data.scene_fade.z
        || temp.w != tm->data.scene_fade.w) {
        tm->data.scene_fade = temp;
        tm->data.update = true;
    }
}

inline void post_process_tone_map_get_scene_fade_color(post_process_tone_map* tm, vec3* value) {
    *value = *(vec3*)&tm->data.scene_fade;
}

inline void post_process_tone_map_set_scene_fade_color(post_process_tone_map* tm, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 1.0f);
    temp.y = clamp(value->y, 0.0f, 1.0f);
    temp.z = clamp(value->z, 0.0f, 1.0f);
    if (temp.x != tm->data.scene_fade.x
        || temp.y != tm->data.scene_fade.y
        || temp.z != tm->data.scene_fade.z) {
        *(vec3*)&tm->data.scene_fade = temp;
        tm->data.update = true;
    }
}

inline float_t post_process_tone_map_get_scene_fade_alpha(post_process_tone_map* tm) {
    return tm->data.scene_fade.w;
}

inline void post_process_tone_map_set_scene_fade_alpha(post_process_tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tm->data.scene_fade.w) {
        tm->data.scene_fade.w = value;
        tm->data.update = true;
    }
}

inline int32_t post_process_tone_map_get_scene_fade_blend_func(post_process_tone_map* tm) {
    return tm->data.scene_fade_blend_func;
}

inline void post_process_tone_map_set_scene_fade_blend_func(post_process_tone_map* tm, int32_t value) {
    value = clamp(value, 0, 2);
    if (value != tm->data.scene_fade_blend_func) {
        tm->data.scene_fade_blend_func = value;
        tm->data.update = true;
    }
}

inline void post_process_tone_map_get_tone_trans_start(post_process_tone_map* tm, vec3* value) {
    *value = tm->data.tone_trans_start;
}

inline void post_process_tone_map_set_tone_trans_start(post_process_tone_map* tm, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 1.0f);
    temp.y = clamp(value->y, 0.0f, 1.0f);
    temp.z = clamp(value->z, 0.0f, 1.0f);
    if (temp.x != tm->data.tone_trans_start.x
        || temp.y != tm->data.tone_trans_start.y
        || temp.z != tm->data.tone_trans_start.z) {
        tm->data.tone_trans_start = temp;
        tm->data.update = true;
    }
}

inline void post_process_tone_map_get_tone_trans_end(post_process_tone_map* tm, vec3* value) {
    *value = tm->data.tone_trans_end;
}

inline void post_process_tone_map_set_tone_trans_end(post_process_tone_map* tm, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.009999999776f, 1.0f);
    temp.y = clamp(value->y, 0.009999999776f, 1.0f);
    temp.z = clamp(value->z, 0.009999999776f, 1.0f);
    if (temp.x != tm->data.tone_trans_end.x
        || temp.y != tm->data.tone_trans_end.y
        || temp.z != tm->data.tone_trans_end.z) {
        tm->data.tone_trans_end = temp;
        tm->data.update = true;
    }
}

inline tone_map_method post_process_tone_map_get_tone_map_method(post_process_tone_map* tm) {
    return tm->data.tone_map_method;
}

inline void post_process_tone_map_set_tone_map_method(post_process_tone_map* tm, tone_map_method value) {
    value = clamp(value, TONE_MAP_YCC_EXPONENT, TONE_MAP_RGB_LINEAR2);
    if (value != tm->data.tone_map_method) {
        tm->data.tone_map_method = value;
        tm->data.update = true;
    }
}

inline float_t post_process_tone_map_get_lens_flare(post_process_tone_map* tm) {
    return tm->data.lens_flare;
}

inline void post_process_tone_map_set_lens_flare(post_process_tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tm->data.lens_flare) {
        tm->data.lens_flare = value;
        tm->data.update = true;
    }
}

inline float_t post_process_tone_map_get_lens_shaft(post_process_tone_map* tm) {
    return tm->data.lens_shaft;
}

inline void post_process_tone_map_set_lens_shaft(post_process_tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tm->data.lens_shaft) {
        tm->data.lens_shaft = value;
        tm->data.update = true;
    }
}

inline float_t post_process_tone_map_get_lens_ghost(post_process_tone_map* tm) {
    return tm->data.lens_ghost;
}

inline void post_process_tone_map_set_lens_ghost(post_process_tone_map* tm, float_t value) {
    tm->data.lens_ghost = clamp(value, 0.0f, 1.0f);
}

static void post_process_tone_map_calculate_data(post_process_tone_map* tm) {
    vec3 tone_trans, tone_trans_scale, tone_trans_offset;
    vec3_sub(tm->data.tone_trans_end, tm->data.tone_trans_start, tone_trans);
    vec3_rcp(tone_trans, tone_trans_scale);
    vec3_mult(tone_trans_scale, tm->data.tone_trans_start, tone_trans_offset);
    vec3_negate(tone_trans_offset, tone_trans_offset);

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
        vec3_mult_scalar(*(vec3*)&v->p_fade_color, v->p_fade_color.w, *(vec3*)&v->p_fade_color);
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
