/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/prj/prj_assert.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include "static_var.hpp"

extern void fbo_blit(struct p_gl_rend_state& p_gl_rend_st, GLuint src_fbo, GLuint dst_fbo,
    GLint src_x, GLint src_y, GLint src_width, GLint src_height,
    GLint dst_x, GLint dst_y, GLint dst_width, GLint dst_height, GLbitfield mask, GLenum filter);

static GLenum get_gl_error() {
    GLenum error = glGetError();
    if (error)
        prj_tracef("** OpenGL ERROR:%s:%d: %d\n", __FILE__, __LINE__, error);
    return error;
}

static void get_gl_error_all() {
    const int32_t max_count = 0x100;
    int32_t count = 0;
    while (get_gl_error() && count < max_count)
        count++;

    if (count == max_count)
        prj_tracef("GL returned more than %d consecutive errors!\n", max_count);
}
