/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "static_var.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

extern void fbo_helper_blit(int32_t src_fbo, GLenum src_mode, int32_t dst_fbo, GLenum dst_mode,
    int32_t src_x0, int32_t src_y0, int32_t src_x1, int32_t src_y1,
    int32_t dst_x0, int32_t dst_y0, int32_t dst_x1, int32_t dst_y1, GLbitfield mask, GLenum filter);
extern void fbo_helper_blit_same(int32_t fbo, GLenum src_mode, GLenum dst_mode,
    int32_t x0, int32_t y0, int32_t x1, int32_t y1, GLbitfield mask, GLenum filter);
extern void fbo_helper_gen_texture_image(int32_t tcb, int32_t width, int32_t height,
    GLenum internal_format, GLenum format, GLenum type, int32_t attachment);
extern void fbo_helper_get_error_code();
