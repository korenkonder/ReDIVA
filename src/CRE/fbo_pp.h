/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "post_process.h"
#include "shader_glsl.h"
#include "texture.h"

typedef struct fbo_pp {
    int32_t vao;
    int32_t fbo[11];
    int32_t tex[11];
    int32_t exposure_history;
    shader_glsl* b_shader;
    vec2i res;
    int32_t* fbo_down;
    int32_t* tex_down;
    vec2i* res_down;
    int32_t count_down;
    int32_t full_count_down;
    GLuint tone_map;
    int32_t exposure_history_counter;
} fbo_pp;

extern const GLenum fbo_pp_s_attachments[];

extern fbo_pp* fbo_pp_init();
extern void fbo_pp_initialize(fbo_pp* pfbo, vec2i* res, int32_t vao, shader_glsl* b_shader);
extern void fbo_pp_resize(fbo_pp* pfbo, vec2i* res);
extern void fbo_pp_tone_map_set(fbo_pp* pfbo, vec2* tone_map_data, int32_t count);
extern void fbo_pp_draw(fbo_pp* pfbo, tone_map* tm,
    texture* in_tex, texture* light_proj_tex, texture* back_2d_tex,
    void* out_fbo, void(*out_fbo_func_begin)(void*), void(*out_fbo_func_end)(void*));
extern void fbo_pp_dispose(fbo_pp* pfbo);
