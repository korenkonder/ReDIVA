/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "shader.h"
#include "texture.h"

typedef struct fbo_pp {
    int32_t vao;
    int32_t fbo[12];
    int32_t tcb[16];
    shader_fbo b_shader[16];
    shader_fbo t_shader;
    vec2i res;
    int32_t* fbo_down;
    int32_t* tcb_down;
    vec2i* res_down;
    int32_t count_down;
    int32_t full_count_down;
    texture tone_map;
    int32_t tone_map_ubo;
    int32_t dst_pixel;
    bool scene_fade;
    int32_t tone_map_method;
} fbo_pp;

extern const GLenum fbo_pp_s_attachments[];
extern const GLenum fbo_pp_d_attachments[];

extern fbo_pp* fbo_pp_init();
extern void fbo_pp_initialize(fbo_pp* pfbo, vec2i* res, int32_t vao,
    shader_fbo* b_shader, shader_fbo* t_shader, int32_t tone_map_ubo);
extern void fbo_pp_resize(fbo_pp* pfbo, vec2i* res);
extern void fbo_pp_tone_map_set(fbo_pp* pfbo, vec2* tone_map_data, int32_t count);
extern void fbo_pp_draw(fbo_pp* pfbo, int32_t in_tcb, int32_t out_fbo,
    int32_t out_fbo_attachments_count, const GLenum* out_fbo_attachments);
extern void fbo_pp_dispose(fbo_pp* pfbo);
