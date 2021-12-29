/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vec.h"

typedef enum tone_map_method {
    TONE_MAP_YCC_EXPONENT = 0,
    TONE_MAP_RGB_LINEAR   = 1,
    TONE_MAP_RGB_LINEAR2  = 2,
} tone_map_method;

typedef struct light_param_glow {
    bool ready;

    bool has_exposure;
    float_t exposure;
    bool has_gamma;
    float_t gamma;
    bool has_saturate_power;
    int32_t saturate_power;
    bool has_saturate_coef;
    float_t saturate_coef;
    bool has_flare;
    vec3 flare;
    bool has_sigma;
    vec3 sigma;
    bool has_intensity;
    vec3 intensity;
    bool has_auto_exposure;
    bool auto_exposure;
    bool has_tone_map_method;
    tone_map_method tone_map_method;
    bool has_fade_color;
    vec4u fade_color;
    int32_t fade_color_blend_func;
    bool has_tone_transform;
    vec3 tone_transform_start;
    vec3 tone_transform_end;
} light_param_glow;

extern void light_param_glow_init(light_param_glow* glow);
extern void light_param_glow_read(light_param_glow* glow, char* path);
extern void light_param_glow_wread(light_param_glow* glow, wchar_t* path);
extern void light_param_glow_mread(light_param_glow* glow, void* data, size_t length);
extern void light_param_glow_write(light_param_glow* glow, char* path);
extern void light_param_glow_wwrite(light_param_glow* glow, wchar_t* path);
extern void light_param_glow_mwrite(light_param_glow* glow, void** data, size_t* length);
extern bool light_param_glow_load_file(void* data, char* path, char* file, uint32_t hash);
extern void light_param_glow_free(light_param_glow* glow);
