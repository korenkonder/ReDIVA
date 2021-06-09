/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "shader.h"

typedef struct fbo_dof {
    int32_t vao;
    int32_t fbo[4];
    int32_t tcb[6];
    shader_fbo* d_shader;
    vec2i res;
    vec2i res_2;
    vec2i res_20;
    int32_t dof_common_ubo;
    int32_t texcoords_ubo;
} fbo_dof;

extern const GLenum fbo_dof_d_attachments[];
extern const GLenum fbo_dof_s_attachments[];

extern fbo_dof* fbo_dof_init();
extern void fbo_dof_initialize(fbo_dof* dfbo, vec2i* res, int32_t vao,
    shader_fbo* d_shader, int32_t dof_common_ubo, int32_t texcoords_ubo);
extern void fbo_dof_resize(fbo_dof* dfbo, vec2i* res);
extern void fbo_dof_draw(fbo_dof* dfbo, int32_t color_tcb, int32_t depth_tcb, bool dof_f2,
    void* out_fbo, void(*out_fbo_func_begin)(void*), void(*out_fbo_func_end)(void*));
extern void fbo_dof_dispose(fbo_dof* dfbo);
