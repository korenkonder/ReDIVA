/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vec.h"

typedef struct light_param_wind_spc {
    float_t cos;
    float_t sin;
} light_param_wind_spc;

typedef struct light_param_wind {
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
} light_param_wind;

extern void light_param_wind_init(light_param_wind* wind);
extern void light_param_wind_read(light_param_wind* wind, char* path);
extern void light_param_wind_wread(light_param_wind* wind, wchar_t* path);
extern void light_param_wind_mread(light_param_wind* wind, void* data, size_t length);
extern void light_param_wind_write(light_param_wind* wind, char* path);
extern void light_param_wind_wwrite(light_param_wind* wind, wchar_t* path);
extern void light_param_wind_mwrite(light_param_wind* wind, void** data, size_t* length);
extern bool light_param_wind_load_file(void* data, char* path, char* file, uint32_t hash);
extern void light_param_wind_free(light_param_wind* wind);
