/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"
#include "../shared.h"
#include "../render_texture.h"

typedef struct post_process_aa {
    int32_t width;
    int32_t height;
    render_texture mlaa_buffer[2];
    GLuint mlaa_area_texture;
} post_process_aa;

extern post_process_aa* post_process_aa_init();
extern void post_process_apply_mlaa(post_process_aa* aa, render_texture* rt,
    render_texture* buf_rt, GLuint* samplers, int32_t a4);
extern void post_process_aa_init_fbo(post_process_aa* aa, int32_t width, int32_t height);
extern void post_process_aa_dispose(post_process_aa* aa);
