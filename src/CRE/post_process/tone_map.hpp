/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/light_param/glow.hpp"
#include "../../KKdLib/vec.hpp"
#include "../GL/uniform_buffer.hpp"
#include "../shared.hpp"
#include "../render_texture.hpp"

#define POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES 32

struct post_process_tone_map_shader_data {
    vec4 g_exposure;
    vec4 g_flare_coef;
    vec4 g_fade_color;
    vec4 g_tone_scale; //xyz=tone_scale, w=fade_func
    vec4 g_tone_offset; //xyz=tone_offset, w=inv_tone
    vec4 g_texcoord_transforms[8];
};

struct post_process_tone_map_data {
    vec2 tex_data[16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES];
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

    post_process_tone_map_data();
};

struct post_process_tone_map {
    post_process_tone_map_data data;
    post_process_tone_map_shader_data shader_data;
    GL::UniformBuffer tone_map_ubo;
    GLuint tone_map_tex;

    post_process_tone_map();
    ~post_process_tone_map();

    void apply(render_texture* in_tex, texture* light_proj_tex, texture* back_2d_tex,
        render_texture* rt, render_texture* buf_rt,/* render_texture* contour_rt,*/
        GLuint in_tex_0, GLuint in_tex_1, int32_t npr_param, void* pp);
    void init_fbo();
    void initialize_data(float_t exposure, bool auto_exposure,
        float_t gamma, int32_t saturate_power, float_t saturate_coeff,
        const vec3& scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
        const vec3& tone_trans_start, const vec3& tone_trans_end, tone_map_method tone_map_method);
    void initialize_data(float_t exposure, bool auto_exposure,
        float_t gamma, float_t gamma_rate, int32_t saturate_power, float_t saturate_coeff,
        const vec3& scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
        const vec3& tone_trans_start, const vec3& tone_trans_end, tone_map_method tone_map_method);

    bool get_auto_exposure();
    void set_auto_exposure(bool value);
    float_t get_exposure();
    void set_exposure(float_t value);
    float_t get_gamma();
    void set_gamma(float_t value);
    float_t get_gamma_rate();
    void set_gamma_rate(float_t value);
    float_t get_lens_flare();
    void set_lens_flare(float_t value);
    float_t get_lens_flare_appear_power();
    void set_lens_flare_appear_power(float_t value);
    float_t get_lens_flare_power();
    void set_lens_flare_power(float_t value);
    float_t get_lens_ghost();
    void set_lens_ghost(float_t value);
    float_t get_lens_shaft();
    void set_lens_shaft(float_t value);
    float_t get_saturate_coeff();
    void set_saturate_coeff(float_t value);
    int32_t get_saturate_power();
    void set_saturate_power(int32_t value);
    vec4 get_scene_fade();
    void set_scene_fade(const vec4& value);
    float_t get_scene_fade_alpha();
    void set_scene_fade_alpha(float_t value);
    int32_t get_scene_fade_blend_func();
    void set_scene_fade_blend_func(int32_t value);
    vec3 get_scene_fade_color();
    void set_scene_fade_color(const vec3& value);
    tone_map_method get_tone_map_method();
    void set_tone_map_method(tone_map_method value);
    void get_tone_trans(vec3& start, vec3& end);
    void set_tone_trans(const vec3& start, const vec3& end);
    vec3 get_tone_trans_end();
    void set_tone_trans_end(const vec3& value);
    vec3 get_tone_trans_start();
    void set_tone_trans_start(const vec3& value);
};
