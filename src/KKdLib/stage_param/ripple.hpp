/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"
#include <vector>

struct stage_param_ripple {
    bool ready;

    size_t rain_ripple_num;
    float_t rain_ripple_min_value;
    float_t rain_ripple_max_value;
    float_t ground_y;
    float_t emit_pos_scale;
    float_t emit_pos_ofs_x;
    float_t emit_pos_ofs_z;
    float_t wake_attn;
    float_t speed;
    std::string ripple_tex_name;
    bool use_float_ripplemap;
    float_t rob_emitter_size;
    std::vector<vec3> emitter_list;
    size_t emitter_num;
    float_t emitter_size;

    stage_param_ripple();
    ~stage_param_ripple();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
