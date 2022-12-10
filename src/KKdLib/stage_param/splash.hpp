/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"
#include <string>

struct stage_param_splash {
    bool ready;

    std::string splash_tex_name;
    std::string splash_obj_name;
    float_t particle_size;
    int32_t emit_num;
    float_t emission_ratio_attn;
    float_t emission_velocity_scale;
    float_t ripple_emission;
    vec4 color;
    bool in_water;
    bool blink;

    stage_param_splash();
    ~stage_param_splash();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
