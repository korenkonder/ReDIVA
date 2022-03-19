/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vec.h"

class light_param_wind_spc {
public:
    float_t cos;
    float_t sin;

    light_param_wind_spc();
    ~light_param_wind_spc();
};

class light_param_wind {
public:
    bool ready;

    bool has_scale;
    float_t scale;
    bool has_cycle;
    float_t cycle;
    bool has_rot;
    float_t rot_y;
    float_t rot_z;
    bool has_bias;
    float_t bias;
    bool has_spc[16];
    light_param_wind_spc spc[16];

    light_param_wind();
    ~light_param_wind();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t length);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* length);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
