/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "static_var.h"
#include "timer.h"

extern timer render_timer;

int32_t sv_max_texture_buffer_size = 0x100000;
int32_t sv_max_texture_size = 2048;
int32_t sv_max_texture_max_anisotropy = 1;

bool sv_anisotropy_changed = false;
int32_t sv_old_anisotropy = 1;
int32_t sv_anisotropy = 1;

size_t frame_counter = 0;

bool select_frame_speed = true;
float_t frame_speed = 1.0f;
float_t target_fps = 60.0f;
float_t current_fps = 60.0f;

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
    sv_anisotropy = 1 << (int32_t)roundf(log2f((float_t)clamp(value, 1, sv_max_texture_max_anisotropy)));
    if (sv_anisotropy != sv_old_anisotropy)
        sv_anisotropy_changed = true;
    sv_old_anisotropy = sv_anisotropy;
}

float_t get_delta_frame() {
    if (!select_frame_speed)
        return frame_speed;

    double_t freq_ratio = timer_get_freq_ratio(&render_timer);
    return (float_t)(freq_ratio * frame_speed);
}

uint32_t get_frame_counter() {
    return (uint32_t)frame_counter;
}

void uniform_value_reset() {
    uniform_value[U_ANISO] = 0;
    uniform_value[U0B] = 0;
    uniform_value[U_TEXTURE_BLEND] = 0;
    uniform_value[U_FOG_HEIGHT] = 0;
    uniform_value[U_FOG] = 0;
    uniform_value[U_SPECULAR_IBL] = 0;
    uniform_value[U_TEX_0_TYPE] = 0;
    uniform_value[U_TEX_1_TYPE] = 0;
    uniform_value[U_TEXTURE_COUNT] = 0;
    uniform_value[U_ENV_MAP] = 0;
    uniform_value[U_TRANSLUCENCY] = 0;
    uniform_value[U_NORMAL] = 0;
    uniform_value[U_TRANSPARENCY] = 0;
    uniform_value[U_LIGHT_0] = 0;
    uniform_value[U_SPECULAR] = 0;
    uniform_value[U45] = 0;
}
