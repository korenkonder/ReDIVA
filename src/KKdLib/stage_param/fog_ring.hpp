/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"
#include <string>

struct stage_param_fog_ring {
    bool ready;

    int32_t num_ptcls;
    float_t ring_size;
    vec3 wind_dir;
    std::string tex_name;
    vec4 color;
    float_t ptcl_size;
    float_t density;
    float_t density_offset;

    stage_param_fog_ring();
    ~stage_param_fog_ring();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
