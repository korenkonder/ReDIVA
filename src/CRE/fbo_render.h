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
    int32_t tcb[2];
    shader_fbo* c_shader;
    shader_fbo* d_shader;
    vec2i res;
} fbo_render;

extern const GLenum fbo_render_attachments[];

extern fbo_render* fbo_render_init();
extern void fbo_render_initialize(fbo_render* rfbo, vec2i* res,
    int32_t vao, shader_fbo* c_shader, shader_fbo* d_shader);
extern void fbo_render_resize(fbo_render* rfbo, vec2i* res);
extern void fbo_render_draw(fbo_render* rfbo, bool depth);
extern void fbo_render_dispose(fbo_render* rfbo);
