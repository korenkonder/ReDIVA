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
        GLint src_x, GLint src_y, GLint src_width, GLint src_height,
        GLint dst_x, GLint dst_y, GLint dst_width, GLint dst_height, GLbitfield mask, GLenum filter);
};
