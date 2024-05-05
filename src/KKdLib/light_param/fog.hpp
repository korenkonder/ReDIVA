/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"

enum fog_id {
    FOG_DEPTH  = 0x00,
    FOG_HEIGHT = 0x01,
    FOG_BUMP   = 0x02,
    FOG_MAX    = 0x03,
};

enum fog_type {
    FOG_NONE   = 0x00,
    FOG_LINEAR = 0x01,
    FOG_EXP    = 0x02,
    FOG_EXP2   = 0x03,
};

struct light_param_fog_group {
    bool has_type;
    fog_type type;
    bool has_density;
    float_t density;
    bool has_linear;
    float_t linear_start;
    float_t linear_end;
    bool has_color;
    vec4 color;

    light_param_fog_group();
};

struct light_param_fog {
    bool ready;

    light_param_fog_group group[FOG_MAX];

    light_param_fog();
    ~light_param_fog();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};
