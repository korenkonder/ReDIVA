/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vec.h"

typedef enum fog_id {
    FOG_DEPTH  = 0x00,
    FOG_HEIGHT = 0x01,
    FOG_BUMP   = 0x02,
    FOG_MAX    = 0x03,
} fog_id;

typedef enum fog_type {
    FOG_NONE   = 0x00,
    FOG_LINEAR = 0x01,
    FOG_EXP    = 0x02,
    FOG_EXP2   = 0x03,
} fog_type;

class light_param_fog_group {
public:
    bool has_type;
    fog_type type;
    bool has_density;
    float_t density;
    bool has_linear;
    float_t linear_start;
    float_t linear_end;
    bool has_color;
    vec4u color;

    light_param_fog_group();
    ~light_param_fog_group();
};

class light_param_fog {
public:
    bool ready;

    light_param_fog_group group[FOG_MAX];

    light_param_fog();
    void read(char* path);
    void read(wchar_t* path);
    void read(void* data, size_t length);
    void write(char* path);
    void write(wchar_t* path);
    void write(void** data, size_t* length);
    ~light_param_fog();
};
extern bool light_param_fog_load_file(void* data, char* path, char* file, uint32_t hash);
