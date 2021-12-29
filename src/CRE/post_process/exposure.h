/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"
#include "../shared.h"
#include "../render_texture.h"

typedef struct post_process_exposure {
    render_texture exposure_history;
    render_texture exposure;
    int32_t exposure_history_counter;
} post_process_exposure;

extern post_process_exposure* post_process_exposure_init();
extern void post_process_get_exposure(post_process_exposure* exp, GLuint in_tex_0, GLuint in_tex_1);
extern void post_process_exposure_init_fbo(post_process_exposure* exp);
extern void post_process_exposure_dispose(post_process_exposure* exp);
