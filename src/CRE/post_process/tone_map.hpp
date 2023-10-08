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

struct post_process_tone_map {
    vec2 tex_data[16 * POST_PROCESS_TONE_MAP_SAT_GAMMA_SAMPLES];
    bool update_lut;
    bool saturate_lock;
    float_t lens_shaft_scale;
    float_t lens_shaft_inv_scale;
    float_t lens_flare_power;
    float_t lens_flare_appear_power;
    int32_t saturate_index;
    int32_t scene_fade_index;
    int32_t tone_trans_index;
    float_t saturate_coeff[2];
    vec3 scene_fade_color[2];
    float_t scene_fade_alpha[2];
    int32_t scene_fade_blend_func[2];
    vec3 tone_trans_scale[2];
    vec3 tone_trans_offset[2];
    vec3 tone_trans_start[2];
    vec3 tone_trans_end[2];
    tone_map_method tone_map_method;
    float_t exposure;
    float_t exposure_rate;
    bool auto_exposure;
    float_t gamma;
    float_t gamma_rate;
    int32_t saturate_power;
    float_t lens_flare;
    float_t lens_shaft;
    float_t lens_ghost;
    bool update;
    post_process_tone_map_shader_data shader_data;
    GL::UniformBuffer tone_map_ubo;
    GLuint tone_map_tex;

    post_process_tone_map();
    ~post_process_tone_map();

    void apply(RenderTexture* in_tex, texture* light_proj_tex, texture* back_2d_tex,
        RenderTexture* rt, RenderTexture* buf_rt,/* RenderTexture* contour_rt,*/
        GLuint in_tex_0, GLuint in_tex_1, int32_t npr_param, void* pp);
    bool get_auto_exposure();
    float_t get_exposure();
    float_t get_exposure_rate();
    float_t get_gamma();
    float_t get_gamma_rate();
    vec3 get_lens();
    float_t get_lens_flare_appear_power();
    float_t get_lens_flare_power();
    float_t get_saturate_coeff();
    int32_t get_saturate_power();
    vec4 get_scene_fade();
    float_t get_scene_fade_alpha();
    int32_t get_scene_fade_blend_func();
    vec3 get_scene_fade_color();
    ::tone_map_method get_tone_map_method();
    void get_tone_trans(vec3& start, vec3& end);
    void init_fbo();
    void reset_saturate_coeff(int32_t index, bool lock);
    void reset_scene_fade(int32_t index);
    void reset_tone_trans(int32_t index);
    void set_auto_exposure(bool value);
    void set_exposure(float_t value);
    void set_exposure_rate(float_t value);
    void set_gamma(float_t value);
    void set_gamma_rate(float_t value);
    void set_lens(vec3 value);
    void set_lens_flare_appear_power(float_t value);
    void set_lens_flare_power(float_t value);
    void set_saturate_coeff(float_t value, int32_t index, bool lock);
    void set_saturate_power(int32_t value);
    void set_scene_fade(const vec4& value, int32_t index);
    void set_scene_fade_alpha(float_t value, int32_t index);
    void set_scene_fade_blend_func(int32_t value, int32_t index);
    void set_scene_fade_color(const vec3& value, int32_t index);
    void set_tone_map_method(::tone_map_method value);
    void set_tone_trans(const vec3& start, const vec3& end, int32_t index);
};
