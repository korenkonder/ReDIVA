/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "render_texture.h"
#include "shader_glsl.h"

typedef struct fbo_render {
    int32_t vao;
    render_texture tex;
    shader_glsl* c_shader;
    shader_glsl* d_shader;
    vec2i res;
} fbo_render;

extern const GLenum fbo_render_attachments[];

extern fbo_render* fbo_render_init();
extern void fbo_render_initialize(fbo_render* rfbo, vec2i* res,
    int32_t vao, shader_glsl* c_shader, shader_glsl* d_shader);
extern void fbo_render_resize(fbo_render* rfbo, vec2i* res);
extern void fbo_render_draw(fbo_render* rfbo, bool depth);
extern void fbo_render_dispose(fbo_render* rfbo);
