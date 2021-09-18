/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"

#define GAUSSIAN_KERNEL_SIZE 7

typedef struct radius {
    vec3 val[GAUSSIAN_KERNEL_SIZE];
    vec3 rgb;
    bool update;
} radius;

typedef struct intensity {
    vec3 val;
    vec3 rgb;
    bool update;
} intensity;

#define TONE_MAP_SAT_GAMMA_SAMPLES 32

typedef struct tone_map_data {
    vec4 p_exposure;
    vec4 p_flare_coef;
    vec4 p_fade_color;
    vec4 p_tone_scale;
    vec4 p_tone_offset;
    vec4 p_fade_func;
    vec4 p_inv_tone;
} tone_map_data;

typedef struct tone_map {
    vec2 tex[16 * TONE_MAP_SAT_GAMMA_SAMPLES];
    tone_map_data data;
    float_t exposure;
    bool auto_exposure;
    float_t gamma;
    float_t gamma_rate;
    int32_t saturate_power;
    float_t saturate_coeff;
    vec3 scene_fade_color;
    float_t scene_fade_alpha;
    int32_t scene_fade_blend_func;
    vec3 tone_trans_start;
    vec3 tone_trans_end;
    int32_t tone_map_method;
    bool update_tex;
} tone_map;

extern radius* radius_init();
extern void radius_initialize(radius* rad, vec3* rgb);
extern vec3* radius_get(radius* rad);
extern void radius_set(radius* rad, vec3* value);
extern void radius_dispose(radius* rad);

extern intensity* intensity_init();
extern void intensity_initialize(intensity* inten, vec3* rgb);
extern vec3* intensity_get(intensity* inten);
extern void intensity_set(intensity* inten, vec3* value);
extern void intensity_dispose(intensity* inten);

extern tone_map* tone_map_init();
extern void tone_map_initialize(tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, int32_t saturate_power, float_t saturate_coeff,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, int32_t tone_map_method);
extern void tone_map_initialize_rate(tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, float_t gamma_rate, int32_t saturate_power, float_t saturate_coeff,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, int32_t tone_map_method);
extern float_t tone_map_get_exposure(tone_map* tm);
extern void tone_map_set_exposure(tone_map* tm, float_t value);
extern bool tone_map_get_auto_exposure(tone_map* tm);
extern void tone_map_set_auto_exposure(tone_map* tm, bool value);
extern float_t tone_map_get_gamma(tone_map* tmsg);
extern void tone_map_set_gamma(tone_map* tmsg, float_t value);
extern float_t tone_map_get_gamma_rate(tone_map* tmsg);
extern void tone_map_set_gamma_rate(tone_map* tmsg, float_t value);
extern int32_t tone_map_get_saturate_power(tone_map* tmsg);
extern void tone_map_set_saturate_power(tone_map* tmsg, int32_t value);
extern float_t tone_map_get_saturate_coeff(tone_map* tmsg);
extern void tone_map_set_saturate_coeff(tone_map* tmsg, float_t value);
extern vec3* tone_map_get_scene_fade_color(tone_map* tm);
extern void tone_map_set_scene_fade_color(tone_map* tm, vec3* value);
extern float_t tone_map_get_scene_fade_alpha(tone_map* tm);
extern void tone_map_set_scene_fade_alpha(tone_map* tm, float_t value);
extern int32_t tone_map_get_scene_fade_blend_func(tone_map* tm);
extern void tone_map_set_scene_fade_blend_func(tone_map* tm, int32_t value);
extern vec3* tone_map_get_tone_trans_start(tone_map* tm);
extern void tone_map_set_tone_trans_start(tone_map* tm, vec3* value);
extern vec3* tone_map_get_tone_trans_end(tone_map* tm);
extern void tone_map_set_tone_trans_end(tone_map* tm, vec3* value);
extern int32_t tone_map_get_tone_map_method(tone_map* tm);
extern void tone_map_set_tone_map_method(tone_map* tm, int32_t value);
extern void tone_map_dispose(tone_map* tm);
