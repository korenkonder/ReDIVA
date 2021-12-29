/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vec.h"

typedef enum light_id {
    LIGHT_CHARA       = 0x00,
    LIGHT_STAGE       = 0x01,
    LIGHT_SUN         = 0x02,
    LIGHT_REFLECT     = 0x03,
    LIGHT_SHADOW      = 0x04,
    LIGHT_CHARA_COLOR = 0x05,
    LIGHT_TONE_CURVE  = 0x06,
    LIGHT_PROJECTION  = 0x07,
    LIGHT_MAX         = 0x08,
} light_id;

typedef enum light_set_id {
    LIGHT_SET_MAIN = 0x00,
    LIGHT_SET_MAX  = 0x01,
} light_set_id;

typedef enum light_type {
    LIGHT_OFF      = 0x00,
    LIGHT_PARALLEL = 0x01,
    LIGHT_POINT    = 0x02,
    LIGHT_SPOT     = 0x03,
} light_type;

typedef struct light_param_light_data {
    bool has_type;
    light_type type;
    bool has_ambient;
    vec4u ambient;
    bool has_diffuse;
    vec4u diffuse;
    bool has_specular;
    vec4u specular;
    bool has_position;
    vec3 position;
    bool has_spot_direction;
    bool has_spot_exponent;
    bool has_spot_cutoff;
    bool has_attenuation;
    vec3 spot_direction;
    float_t spot_exponent;
    float_t spot_cutoff;
    vec3 attenuation;
    bool has_clip_plane;
    bool clip_plane[4];
    bool has_tone_curve;
    vec3 tone_curve;
} light_param_light_data;

typedef struct light_param_light_group {
    light_param_light_data data[LIGHT_MAX];
} light_param_light_group;

typedef struct light_param_light {
    bool ready;

    light_param_light_group group[LIGHT_SET_MAX];
} light_param_light;

extern void light_param_light_init(light_param_light* light);
extern void light_param_light_read(light_param_light* light, char* path);
extern void light_param_light_wread(light_param_light* light, wchar_t* path);
extern void light_param_light_mread(light_param_light* light, void* data, size_t length);
extern void light_param_light_write(light_param_light* light, char* path);
extern void light_param_light_wwrite(light_param_light* light, wchar_t* path);
extern void light_param_light_mwrite(light_param_light* light, void** data, size_t* length);
extern bool light_param_light_load_file(void* data, char* path, char* file, uint32_t hash);
extern void light_param_light_free(light_param_light* light);
