/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"
#include <string>

struct stage_param_snow_colli_ground {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;

    stage_param_snow_colli_ground();
};

struct stage_param_snow {
    bool ready;

    std::string tex_name;
    int32_t num_snow;
    int32_t num_snow_gpu;
    vec4 color;
    vec3 velocity;
    vec3 vel_range;
    vec3 range;
    vec3 range_gpu;
    vec3 offset;
    vec3 offset_gpu;
    stage_param_snow_colli_ground colli_ground;

    stage_param_snow();
    ~stage_param_snow();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
