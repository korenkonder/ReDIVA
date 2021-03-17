/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "static_var.h"

int32_t sv_max_samples = 1;
int32_t sv_max_texture_buffer_size = 0x100000;
int32_t sv_max_texture_size = 2048;
int32_t sv_max_texture_max_anisotropy = 1;

bool sv_anisotropy_changed = false;
int32_t sv_old_anisotropy = 1;
int32_t sv_anisotropy = 1;

bool sv_samples_changed = false;
int32_t sv_old_samples = 1;
int32_t sv_samples = 1;

bool sv_fxaa_changed = false;
bool sv_old_fxaa = false;
bool sv_fxaa = false;

const vec3 sv_rgb_to_luma = {
    0.2126f, 0.7152f, 0.0722f
};

const mat3 sv_rgb_to_ypbpr = {
    {0.21260f, -0.11458f, 0.50000f},
    {0.71520f, -0.38542f, -0.45415f},
    {0.07220f, 0.50000f, -0.04585f},
};

const mat3 sv_ypbpr_to_rgb = {
    {1.00000f, 1.00000f, 1.00000f},
    {0.00000f, -0.18732f, 1.85560f},
    {1.57480f, -0.46812f, 0.00000f},
};

inline bool sv_can_use_msaa() {
    return sv_max_samples > 1;
}

void sv_anisotropy_set(int32_t value) {
    sv_samples = 1 << (int32_t)log2(clamp(value, 1, sv_max_samples));
    if (sv_anisotropy != sv_old_anisotropy)
        sv_anisotropy_changed = true;
    sv_old_anisotropy = sv_anisotropy;
}

void sv_samples_set(int32_t value) {
    sv_samples = 1 << (int32_t)log2(clamp(value, 1, sv_max_samples));
    if (sv_samples != sv_old_samples)
        sv_samples_changed = true;
    sv_old_samples = sv_samples;
}

void sv_fxaa_set(bool value) {
    sv_fxaa = value;
    if (sv_fxaa != sv_old_fxaa)
        sv_fxaa_changed = true;
    sv_old_fxaa = sv_fxaa;
}
