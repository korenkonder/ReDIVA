/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "material.h"

const material_blend material_blend_default = {
    .enable = true,
    .src_factor_rgb   = GL_SRC_ALPHA,
    .dst_factor_rgb   = GL_ONE_MINUS_SRC_ALPHA,
    .src_factor_alpha = GL_ONE,
    .dst_factor_alpha = GL_ONE_MINUS_SRC_ALPHA,
    .mode_rgb         = GL_FUNC_ADD,
    .mode_alpha       = GL_FUNC_ADD,
};
