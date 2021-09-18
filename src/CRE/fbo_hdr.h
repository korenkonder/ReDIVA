/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "shader_glsl.h"
#include "fbo.h"
#include "render_texture.h"

typedef struct fbo_hdr {
    int32_t width;
    int32_t height;
    render_texture color;
    render_texture back_2d;
    render_texture buf;
    GLuint sampler;
    GLuint vao;
    shader_glsl* fxaa_shader;
    bool fxaa;
} fbo_hdr;

extern fbo_hdr* fbo_hdr_init();
extern void fbo_hdr_initialize(fbo_hdr* hfbo, vec2i* res, int32_t vao, shader_glsl* fxaa_shader);
extern void fbo_hdr_resize(fbo_hdr* hfbo, vec2i* res);
extern void fbo_hdr_draw_fxaa(fbo_hdr* hfbo, int32_t preset);
extern void fbo_hdr_set_fbo_begin(fbo_hdr* hfbo);
extern void fbo_hdr_set_fbo_end(fbo_hdr* hfbo);
extern void fbo_hdr_dispose(fbo_hdr* hfbo);
