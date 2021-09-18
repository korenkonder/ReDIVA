/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "static_var.h"
#include "timer.h"

int32_t sv_max_texture_buffer_size = 0x100000;
int32_t sv_max_texture_size = 2048;
int32_t sv_max_texture_max_anisotropy = 1;

bool sv_anisotropy_changed = false;
int32_t sv_old_anisotropy = 1;
int32_t sv_anisotropy = 1;

bool sv_fxaa_changed = false;
bool sv_old_fxaa = false;
bool sv_fxaa = false;

bool sv_fxaa_preset_changed = false;
int32_t sv_old_fxaa_preset = 5;
int32_t sv_fxaa_preset = 5;

int32_t uniform_value[U_MAX];

bool select_frame_speed = false;
float_t frame_speed = 1.0f;

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

void sv_fxaa_set(bool value) {
    sv_fxaa = value;
    if (sv_fxaa != sv_old_fxaa)
        sv_fxaa_changed = true;
    sv_old_fxaa = sv_fxaa;
}

void sv_fxaa_preset_set(int32_t value) {
    sv_fxaa_preset = clamp(value, 3, 5);
    if (sv_fxaa_preset != sv_old_fxaa_preset)
        sv_fxaa_changed = true;
    sv_old_fxaa_preset = sv_fxaa_preset;
}

extern timer render_timer;

float_t get_frame_speed() {
    if (!select_frame_speed)
        return frame_speed;

    double_t freq = timer_get_freq(&render_timer);
    double_t freq_hist = timer_get_freq_hist(&render_timer);
    return (float_t)(min(freq / freq_hist, 1.0) * frame_speed);
}

typedef union vertex_shader_data {
    struct {
        vec4 position;
        vec4 weight;
        vec4 normal;
        vec4 color;
        vec4 color1;
        vec4 fogcoord;
        vec4 attrib6;
        vec4 attrib7;
        union {
            struct {
                vec4 texcoord0;
                vec4 texcoord1;
                vec4 texcoord2;
                vec4 texcoord3;
                vec4 texcoord4;
                vec4 texcoord5;
                vec4 texcoord6;
                vec4 texcoord7;
            };
            vec4 texcoord[8];
        };
    };
    vec4 attrib[16];
} vertex_shader_data;

GLuint vertex_shader_vao;
GLuint vertex_shader_vbo;
vertex_shader_data vertex_shader[];
