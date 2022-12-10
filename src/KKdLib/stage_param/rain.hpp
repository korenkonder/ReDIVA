/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"
#include <string>

struct stage_param_rain {
    bool ready;

    std::string tex_name;
    int32_t num_rain;
    vec4 color;
    vec3 velocity;
    vec3 vel_range;
    vec2 psize;
    vec3 range;
    vec3 offset;

    stage_param_rain();
    ~stage_param_rain();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
