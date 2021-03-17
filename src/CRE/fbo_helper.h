/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

extern void fbo_helper_gen_texture_image_ms(int32_t tcb, int32_t width, int32_t height, int32_t samples,
    GLenum internal_format, GLenum format, GLenum type, int32_t attachment);
extern void fbo_helper_gen_renderbuffer(int32_t rbo, int32_t width,
    int32_t height, int32_t samples, GLenum internal_format);
extern void fbo_helper_get_error_code();
