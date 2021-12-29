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

typedef struct light_param_fog_group {
    bool has_type;
    fog_type type;
    bool has_density;
    float_t density;
    bool has_linear;
    float_t linear_start;
    float_t linear_end;
    bool has_color;
    vec4u color;
} light_param_fog_group;

typedef struct light_param_fog {
    bool ready;

    light_param_fog_group group[FOG_MAX];
} light_param_fog;

extern void light_param_fog_init(light_param_fog* fog);
extern void light_param_fog_read(light_param_fog* fog, char* path);
extern void light_param_fog_wread(light_param_fog* fog, wchar_t* path);
extern void light_param_fog_mread(light_param_fog* fog, void* data, size_t length);
extern void light_param_fog_write(light_param_fog* fog, char* path);
extern void light_param_fog_wwrite(light_param_fog* fog, wchar_t* path);
extern void light_param_fog_mwrite(light_param_fog* fog, void** data, size_t* length);
extern bool light_param_fog_load_file(void* data, char* path, char* file, uint32_t hash);
extern void light_param_fog_free(light_param_fog* fog);
