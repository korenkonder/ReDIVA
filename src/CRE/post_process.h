/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"

#define GAUSSIAN_KERNEL_SIZE 7

typedef struct radius {
    bool update;
    float_t val[4 * GAUSSIAN_KERNEL_SIZE];
    vec3 rgb;
} radius;

typedef struct intensity {
    bool update;
    vec3 val;
    vec3 rgb;
} intensity;

#define TONE_MAP_SAT_GAMMA_SAMPLES 32

typedef struct tone_map_sat_gamma {
    bool update;
    vec2 val[16 * TONE_MAP_SAT_GAMMA_SAMPLES];
    float_t gamma;
    float_t gamma_rate;
    int32_t saturate1;
    float_t saturate2;
} tone_map_sat_gamma;

typedef struct tone_map_data {
    bool update;
    float_t val[16];
    float_t exposure;
    bool auto_exposure;
    vec3 scene_fade_color;
    float_t scene_fade_alpha;
    int32_t scene_fade_blend_func;
    vec3 tone_trans_start;
    vec3 tone_trans_end;
    int32_t tone_map_method;
} tone_map_data;

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

extern tone_map_sat_gamma* tone_map_sat_gamma_init();
extern void tone_map_sat_gamma_initialize_rate(tone_map_sat_gamma* tmsg,
    float_t gamma, float_t gamma_rate, int32_t saturate1, float_t saturate2);
extern void tone_map_sat_gamma_initialize(tone_map_sat_gamma* tmsg,
    float_t gamma, int32_t saturate1, float_t saturate2);
extern float_t tone_map_sat_gamma_get_gamma(tone_map_sat_gamma* tmsg);
extern void tone_map_sat_gamma_set_gamma(tone_map_sat_gamma* tmsg, float_t value);
extern float_t tone_map_sat_gamma_get_gamma_rate(tone_map_sat_gamma* tmsg);
extern void tone_map_sat_gamma_set_gamma_rate(tone_map_sat_gamma* tmsg, float_t value);
extern int32_t tone_map_sat_gamma_get_saturate1(tone_map_sat_gamma* tmsg);
extern void tone_map_sat_gamma_set_saturate1(tone_map_sat_gamma* tmsg, int32_t value);
extern float_t tone_map_sat_gamma_get_saturate2(tone_map_sat_gamma* tmsg);
extern void tone_map_sat_gamma_set_saturate2(tone_map_sat_gamma* tmsg, float_t value);
extern void tone_map_sat_gamma_dispose(tone_map_sat_gamma* tmsg);

extern tone_map_data* tone_map_data_init();
extern void tone_map_data_initialize(tone_map_data* tmd, float_t exposure, bool auto_exposure,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, int32_t tone_map_method);
extern float_t tone_map_data_get_exposure(tone_map_data* tmd);
extern void tone_map_data_set_exposure(tone_map_data* tmd, float_t value);
extern bool tone_map_data_get_auto_exposure(tone_map_data* tmd);
extern void tone_map_data_set_auto_exposure(tone_map_data* tmd, bool value);
extern vec3* tone_map_data_get_scene_fade_color(tone_map_data* tmd);
extern void tone_map_data_set_scene_fade_color(tone_map_data* tmd, vec3* value);
extern float_t tone_map_data_get_scene_fade_alpha(tone_map_data* tmd);
extern void tone_map_data_set_scene_fade_alpha(tone_map_data* tmd, float_t value);
extern int32_t tone_map_data_get_scene_fade_blend_func(tone_map_data* tmd);
extern void tone_map_data_set_scene_fade_blend_func(tone_map_data* tmd, int32_t value);
extern vec3* tone_map_data_get_tone_trans_start(tone_map_data* tmd);
extern void tone_map_data_set_tone_trans_start(tone_map_data* tmd, vec3* value);
extern vec3* tone_map_data_get_tone_trans_end(tone_map_data* tmd);
extern void tone_map_data_set_tone_trans_end(tone_map_data* tmd, vec3* value);
extern int32_t tone_map_data_get_tone_map_method(tone_map_data* tmd);
extern void tone_map_data_set_tone_map_method(tone_map_data* tmd, int32_t value);
extern void tone_map_data_dispose(tone_map_data* tmd);
