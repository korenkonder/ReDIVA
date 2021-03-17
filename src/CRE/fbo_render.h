/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "shader.h"

typedef struct fbo_render {
    int32_t vao;
    int32_t fbo;
    int32_t rbo;
    int32_t tcb[5];
    int32_t samples;
    shader_fbo c_shader[10];
    shader_fbo g_shader[5];
    vec2i res;
    GLenum target;
} fbo_render;

extern const GLenum fbo_render_c_attachments[];
extern const GLenum fbo_render_f_attachments[];
extern const GLenum fbo_render_g_attachments[];

extern fbo_render* fbo_render_init();
extern void fbo_render_initialize(fbo_render* rfbo, vec2i* res,
    int32_t vao, shader_fbo* c_shader, shader_fbo* g_shader);
extern void fbo_render_resize(fbo_render* rfbo, vec2i* res);
extern void fbo_render_draw_c(fbo_render* rfbo, bool depth);
extern void fbo_render_draw_g(fbo_render* rfbo, int32_t dir_lights_tcb,
    int32_t dir_lights_count, int32_t point_lights_tcb, int32_t point_lights_count);
extern void fbo_render_dispose(fbo_render* rfbo);
