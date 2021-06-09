/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "hash.h"
#include "texture.h"

typedef struct material_blend {
    bool enable;
    uint16_t src_factor_rgb;
    uint16_t dst_factor_rgb;
    uint16_t src_factor_alpha;
    uint16_t dst_factor_alpha;
    uint16_t mode_rgb;
    uint16_t mode_alpha;
} material_blend;

typedef struct material_param {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float_t shininess;
} material_param;

typedef struct material {
    texture_set* texture;
    material_blend blend;
    material_param param;
    bool translucent;
} material;

typedef struct material_data {
    hash texture;
    material_blend blend;
    bool translucent;
} material_data;

extern const material_blend material_blend_default;
extern const material_param material_param_default;
