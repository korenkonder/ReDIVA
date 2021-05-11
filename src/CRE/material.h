/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "hash.h"
#include "texture.h"

typedef struct material_blend {
    bool enable;
    int16_t src_factor_rgb;
    int16_t dst_factor_rgb;
    int16_t src_factor_alpha;
    int16_t dst_factor_alpha;
    int16_t mode_rgb;
    int16_t mode_alpha;
} material_blend;

typedef struct material {
    texture_set texture;
    material_blend blend;
    bool translucent;
} material;

typedef struct material_update {
    hash texture;
    material_blend blend;
    bool translucent;
} material_update;

extern const material_blend material_blend_default;
