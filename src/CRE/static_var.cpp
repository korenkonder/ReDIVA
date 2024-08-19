/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "static_var.hpp"

int32_t sv_max_texture_size = 2048;
int32_t sv_max_texture_max_anisotropy = 1;
int32_t sv_max_uniform_buffer_size = 0x10000;
int32_t sv_min_uniform_buffer_alignment = 0x40;

bool sv_anisotropy_changed = false;
int32_t sv_old_anisotropy = 1;
int32_t sv_anisotropy = 1;

int32_t uniform_value[U_MAX];

const vec3 sv_rgb_to_luma = {
    0.2126f, 0.7152f, 0.0722f
};

const mat3 sv_rgb_to_ypbpr = {
    { 0.21260f, -0.11458f,  0.50000f },
    { 0.71520f, -0.38542f, -0.45415f },
    { 0.07220f,  0.50000f, -0.04585f },
};

const mat3 sv_ypbpr_to_rgb = {
    { 1.00000f,  1.00000f, 1.00000f },
    { 0.00000f, -0.18732f, 1.85560f },
    { 1.57480f, -0.46812f, 0.00000f },
};

void sv_anisotropy_set(int32_t value) {
    sv_anisotropy = 1 << (int32_t)prj::roundf(log2f((float_t)clamp_def(value, 1, sv_max_texture_max_anisotropy)));
    if (sv_anisotropy != sv_old_anisotropy)
        sv_anisotropy_changed = true;
    sv_old_anisotropy = sv_anisotropy;
}

void uniform_value_reset() {
    uniform_value[U_ANISO] = 0;
    uniform_value[U0B] = 0;
    uniform_value[U_TEXTURE_BLEND] = 0;
    uniform_value[U_FOG_STAGE] = 0;
    uniform_value[U_FOG_CHARA] = 0;
    uniform_value[U_SPECULAR_IBL] = 0;
    uniform_value[U_TEX_0_TYPE] = 0;
    uniform_value[U_TEX_1_TYPE] = 0;
    uniform_value[U_TEXTURE_COUNT] = 0;
    uniform_value[U_ENV_MAP] = 0;
    uniform_value[U_TRANSLUCENCY] = 0;
    uniform_value[U_NORMAL] = 0;
    uniform_value[U_TRANSPARENCY] = 0;
    uniform_value[U_STAGE_SHADOW] = 0;
    uniform_value[U_SPECULAR] = 0;
    uniform_value[U45] = 0;
}
