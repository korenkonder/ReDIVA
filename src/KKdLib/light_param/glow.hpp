/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"

struct light_param_glow {
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
    float_t flare[3];
    bool has_sigma;
    float_t sigma[3];
    bool has_intensity;
    float_t intensity[3];
    bool has_auto_exposure;
    bool auto_exposure;
    bool has_tone_map_method;
    int32_t tone_map_method;
    bool has_fade_color;
    vec4 fade_color;
    int32_t fade_color_blend_func;
    bool has_tone_transform;
    float_t tone_transform_start[3];
    float_t tone_transform_end[3];

    light_param_glow();
    ~light_param_glow();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};
