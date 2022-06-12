/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"

enum light_id {
    LIGHT_CHARA       = 0x00,
    LIGHT_STAGE       = 0x01,
    LIGHT_SUN         = 0x02,
    LIGHT_REFLECT     = 0x03,
    LIGHT_SHADOW      = 0x04,
    LIGHT_CHARA_COLOR = 0x05,
    LIGHT_TONE_CURVE  = 0x06,
    LIGHT_PROJECTION  = 0x07,
    LIGHT_MAX         = 0x08,
};

enum light_set_id {
    LIGHT_SET_MAIN = 0x00,
    LIGHT_SET_MAX  = 0x01,
};

enum light_type {
    LIGHT_OFF      = 0x00,
    LIGHT_PARALLEL = 0x01,
    LIGHT_POINT    = 0x02,
    LIGHT_SPOT     = 0x03,
};

struct light_param_light_data {
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

    light_param_light_data();
    ~light_param_light_data();
};

struct light_param_light_group {
    light_param_light_data data[LIGHT_MAX];

    light_param_light_group();
    ~light_param_light_group();
};

struct light_param_light {
    bool ready;

    light_param_light_group group[LIGHT_SET_MAX];

    light_param_light();
    ~light_param_light();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
