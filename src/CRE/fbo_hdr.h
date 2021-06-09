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
    int32_t fbo;
    int32_t color_tcb;
    int32_t depth_tcb;
    int32_t buf_tcb;
    shader_fbo* fxaa_shader;
    vec2i res;
    bool fxaa;
} fbo_hdr;

extern const GLenum fbo_hdr_c_attachments[];
extern const GLenum fbo_hdr_d_attachments[];
extern const GLenum fbo_hdr_f_attachments[];

extern fbo_hdr* fbo_hdr_init();
extern void fbo_hdr_initialize(fbo_hdr* hfbo, vec2i* res, int32_t vao, shader_fbo* fxaa_shader);
extern void fbo_hdr_resize(fbo_hdr* hfbo, vec2i* res);
extern void fbo_hdr_draw_fxaa(fbo_hdr* hfbo, int32_t preset);
extern void fbo_hdr_set_fbo_begin(fbo_hdr* hfbo);
extern void fbo_hdr_set_fbo_end(fbo_hdr* hfbo);
extern void fbo_hdr_dispose(fbo_hdr* hfbo);
