/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"
#include "../shared.h"
#include "../dof.h"
#include "../fbo.h"
#include "../render_texture.h"

typedef struct post_process_dof {
    int32_t width;
    int32_t height;
    GLuint textures[6];
    fbo_struct fbo[4];
    GLuint samplers[2];
    GLuint vao;
    GLuint program[9];
    GLuint ubo[2];
} post_process_dof;

typedef struct post_process_dof_data {
    vec4 g_depth_params;
    vec4 g_spread_scale;
    vec4 g_depth_params2;
} post_process_dof_data;

extern void post_process_dof_init(post_process_dof* dof);
extern void post_process_apply_dof(post_process_dof* dof, render_texture* rt, dof_struct* dof_data);
extern void post_process_dof_init_fbo(post_process_dof* dof, int32_t width, int32_t height);
extern void post_process_dof_free(post_process_dof* dof);
