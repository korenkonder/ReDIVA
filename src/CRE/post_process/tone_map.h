/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/light_param/glow.hpp"
#include "../../KKdLib/vec.h"
#include "../shared.h"
#include "../render_texture.h"

#define POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES 32

struct post_process_tone_map_shader_data {
    vec4 p_exposure;
    vec4 p_flare_coef;
    vec4 p_fade_color;
    vec4 p_tone_scale;
    vec4 p_tone_offset;
    vec4 p_fade_func;
    vec4 p_inv_tone;
};

struct post_process_tone_map_data {
    vec2 tex[16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES];
    float_t exposure;
    bool auto_exposure;
    float_t gamma;
    float_t gamma_rate;
    int32_t saturate_power;
    float_t saturate_coeff;
    vec4 scene_fade;
    int32_t scene_fade_blend_func;
    vec3 tone_trans_start;
    vec3 tone_trans_end;
    tone_map_method tone_map_method;
    float_t lens_flare;
    float_t lens_shaft;
    float_t lens_ghost;
    float_t lens_flare_power;
    float_t lens_flare_appear_power;
    bool update;
    bool update_tex;
};

struct post_process_tone_map {
    post_process_tone_map_data data;
    post_process_tone_map_shader_data shader_data;
    GLuint tone_map;
};

extern post_process_tone_map* post_process_tone_map_init();
extern void post_process_apply_tone_map(post_process_tone_map* tm,
    render_texture* in_tex, texture* light_proj_tex, texture* back_2d_tex,
    render_texture* rt, render_texture* buf_rt, render_texture* contour_rt,
    GLuint in_tex_0, GLuint in_tex_1, int32_t npr_param);
extern void post_process_tone_map_init_fbo(post_process_tone_map* tm);
extern void post_process_tone_map_dispose(post_process_tone_map* tm);

extern void post_process_tone_map_initialize_data(post_process_tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, int32_t saturate_power, float_t saturate_coeff,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, tone_map_method tone_map_method);
extern void post_process_tone_map_initialize_rate(post_process_tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, float_t gamma_rate, int32_t saturate_power, float_t saturate_coeff,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, tone_map_method tone_map_method);
extern float_t post_process_tone_map_get_exposure(post_process_tone_map* tm);
extern void post_process_tone_map_set_exposure(post_process_tone_map* tm, float_t value);
extern bool post_process_tone_map_get_auto_exposure(post_process_tone_map* tm);
extern void post_process_tone_map_set_auto_exposure(post_process_tone_map* tm, bool value);
extern float_t post_process_tone_map_get_gamma(post_process_tone_map* tm);
extern void post_process_tone_map_set_gamma(post_process_tone_map* tm, float_t value);
extern float_t post_process_tone_map_get_gamma_rate(post_process_tone_map* tm);
extern void post_process_tone_map_set_gamma_rate(post_process_tone_map* tm, float_t value);
extern int32_t post_process_tone_map_get_saturate_power(post_process_tone_map* tm);
extern void post_process_tone_map_set_saturate_power(post_process_tone_map* tm, int32_t value);
extern float_t post_process_tone_map_get_saturate_coeff(post_process_tone_map* tm);
extern void post_process_tone_map_set_saturate_coeff(post_process_tone_map* tm, float_t value);
extern void post_process_tone_map_get_scene_fade(post_process_tone_map* tm, vec4* value);
extern void post_process_tone_map_set_scene_fade(post_process_tone_map* tm, vec4* value);
extern void post_process_tone_map_get_scene_fade_color(post_process_tone_map* tm, vec3* value);
extern void post_process_tone_map_set_scene_fade_color(post_process_tone_map* tm, vec3* value);
extern float_t post_process_tone_map_get_scene_fade_alpha(post_process_tone_map* tm);
extern void post_process_tone_map_set_scene_fade_alpha(post_process_tone_map* tm, float_t value);
extern int32_t post_process_tone_map_get_scene_fade_blend_func(post_process_tone_map* tm);
extern void post_process_tone_map_set_scene_fade_blend_func(post_process_tone_map* tm, int32_t value);
extern void post_process_tone_map_get_tone_trans_start(post_process_tone_map* tm, vec3* value);
extern void post_process_tone_map_set_tone_trans_start(post_process_tone_map* tm, vec3* value);
extern void post_process_tone_map_get_tone_trans_end(post_process_tone_map* tm, vec3* value);
extern void post_process_tone_map_set_tone_trans_end(post_process_tone_map* tm, vec3* value);
extern tone_map_method post_process_tone_map_get_tone_map_method(post_process_tone_map* tm);
extern void post_process_tone_map_set_tone_map_method(post_process_tone_map* tm, tone_map_method value);
extern float_t post_process_tone_map_get_lens_flare(post_process_tone_map* tm);
extern void post_process_tone_map_set_lens_flare(post_process_tone_map* tm, float_t value);
extern float_t post_process_tone_map_get_lens_shaft(post_process_tone_map* tm);
extern void post_process_tone_map_set_lens_shaft(post_process_tone_map* tm, float_t value);
extern float_t post_process_tone_map_get_lens_ghost(post_process_tone_map* tm);
extern void post_process_tone_map_set_lens_ghost(post_process_tone_map* tm, float_t value);
