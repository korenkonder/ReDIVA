/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog.hpp"
#include "../shader_ft.h"

fog::fog() {
    type = FOG_NONE;
    density = 0.0f;
    start = 20.0f;
    end = 1000.0f;
    index = 0;
    color = { 1.0f, 1.0f, 1.0f, 0.0f };
}

inline fog_type fog_get_type(fog* fog) {
    return fog->type;
}

inline void fog_set_type(fog* fog, fog_type value) {
    fog->type = value;
}

inline float_t fog_get_density(fog* fog) {
    return fog->density;
}

inline void fog_set_density(fog* fog, float_t value) {
    fog->density = value;
}

inline float_t fog_get_start(fog* fog) {
    return fog->start;
}

inline void fog_set_start(fog* fog, float_t value) {
    fog->start = value;
}

inline float_t fog_get_end(fog* fog) {
    return fog->end;
}

inline void fog_set_end(fog* fog, float_t value) {
    fog->end = value;
}

inline int32_t fog_get_index(fog* fog) {
    return fog->index;
}

inline void fog_set_index(fog* fog, int32_t value) {
    fog->index = value;
}

inline void fog_get_color(fog* fog, vec4* value) {
    *value = fog->color;
}

inline void fog_set_color(fog* fog, vec4* value) {
    fog->color = *value;
}

void fog_data_set(fog* fog, fog_id id) {
    fog_type type = fog_get_type(fog);
    if (type == FOG_NONE)
        return;

    float_t density = fog_get_density(fog);
    float_t start = fog_get_start(fog);
    float_t end = fog_get_end(fog);
    if (start >= end)
        start = end - 0.01f;

    vec4 params;
    params.x = density;
    params.y = start;
    params.z = end;
    params.w = 1.0f / (end - start);

    switch (id) {
    case FOG_DEPTH: {
        vec4 color;
        fog_get_color(fog, &color);
        shaders_ft.env_vert_set(29, color);
        shaders_ft.env_frag_set(4, color);

        vec4_mult_scalar(color, (float_t)(1.0 / 8.0), color);
        shaders_ft.state_fog_set_color(color);
        shaders_ft.state_fog_set_params(params);
    } break;
    case FOG_HEIGHT: {
        vec4 color;
        fog_get_color(fog, &color);
        shaders_ft.env_vert_set(30, color);
        shaders_ft.env_frag_set(11, color);
        shaders_ft.env_vert_set(14, params);
    } break;
    case FOG_BUMP: {
        shaders_ft.env_vert_set(19, params);
    } break;
    }
}
