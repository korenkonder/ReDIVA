/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "shader.h"

typedef struct fbo_hdr {
    int32_t vao;
    int32_t fbo[2];
    int32_t tcb[3];
    shader_fbo f_shader;
    shader_fbo h_shader[2];
    vec2i res;
    vec2i res_2d;
    bool fxaa;
} fbo_hdr;

extern const GLenum fbo_hdr_c_attachments[];
extern const GLenum fbo_hdr_f_attachments[];

extern fbo_hdr* fbo_hdr_init();
extern void fbo_hdr_initialize(fbo_hdr* hfbo, vec2i* res, vec2i* res_2d,
    int32_t vao, shader_fbo* f_shader, shader_fbo* h_shader);
extern void fbo_hdr_resize(fbo_hdr* hfbo, vec2i* res, vec2i* res_2d);
extern void fbo_hdr_draw_aa(fbo_hdr* hfbo);
extern void fbo_hdr_draw(fbo_hdr* hfbo);
extern void fbo_hdr_dispose(fbo_hdr* hfbo);
