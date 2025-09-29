/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "static_var.hpp"

int32_t sv_max_texture_size = 2048;
int32_t sv_max_texture_max_anisotropy = 1;
int32_t sv_max_uniform_buffer_size = 0x10000;
int32_t sv_max_storage_buffer_size = 0x8000000;
int32_t sv_min_uniform_buffer_alignment = 0x100;
int32_t sv_min_storage_buffer_alignment = 0x100;

bool sv_anisotropy_changed = false;
int32_t sv_old_anisotropy = 1;
int32_t sv_anisotropy = 1;

bool sv_better_reflect = false;

gpu_vendor sv_gpu_vendor = GPU_VENDOR_UNKNOWN;

bool sv_shared_storage_uniform_buffer = true;

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
