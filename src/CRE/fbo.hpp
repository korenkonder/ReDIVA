/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

enum fbo_flags {
    FBO_DEPTH_ATTACHMENT = 0x01,
};

struct fbo {
    fbo_flags flags;
    int32_t width;
    int32_t height;
    GLuint buffer;
    int32_t count;
    GLuint* textures;

    fbo();
    ~fbo();

    void init_data(int32_t width, int32_t height,
        GLuint* color_textures, int32_t count, GLuint depth_texture);
    void free_data();

    static void blit(GLuint src_fbo, GLuint dst_fbo,
        GLint src_x0, GLint src_y0, GLint src_x1, GLint src_y1,
        GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1, GLbitfield mask, GLenum filter);
};
