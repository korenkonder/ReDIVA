/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"
#include <string>

struct stage_param_data_leaf_lie_plane_xz {
    float_t min_x;
    float_t max_x;
    float_t min_z;
    float_t max_z;

    stage_param_data_leaf_lie_plane_xz();
};

struct stage_param_leaf {
    bool ready;

    std::string tex_name;
    vec4 color;
    float_t psize;
    int32_t num_initial_ptcls;
    float_t frame_speed_coef;
    float_t emit_interval;
    vec3 wind;
    vec3 range;
    vec3 offset;
    stage_param_data_leaf_lie_plane_xz lie_plane_xz;
    bool split_tex;

    stage_param_leaf();
    ~stage_param_leaf();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};
