/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo.hpp"
#include "gl_state.h"

fbo::fbo() {

}

fbo::fbo(int32_t width, int32_t height,
    GLuint* color_textures, int32_t count, GLuint depth_texture) : fbo() {
    if (buffer) {
        glDeleteFramebuffers(1, &buffer);
        buffer = 0;
    }
    free(textures);

    count = min(count, 8);

    this->width = width;
    this->height = height;
    this->count = count;
    flags = (fbo_flags)0;
    if (depth_texture) {
        this->flags = FBO_DEPTH_ATTACHMENT;
        this->count++;
    }

    textures = force_malloc_s(GLuint, this->count);
    for (int32_t i = 0; i < count; i++)
        textures[i] = color_textures[i];

    if (flags & FBO_DEPTH_ATTACHMENT)
        textures[count] = depth_texture;

    glGenFramebuffers(1, &buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    for (int32_t i = 0; i < count; i++)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textures[i], 0);

    if (flags & FBO_DEPTH_ATTACHMENT)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures[count], 0);

    GLenum color_attachments[8];
    for (int32_t i = 0; i < count; i++)
        color_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(count, color_attachments);
    glCheckFramebufferStatus(GL_FRAMEBUFFER);
}

fbo::~fbo() {
    if (buffer) {
        glDeleteFramebuffers(1, &buffer);
        buffer = 0;
    }
    free(textures);
    textures = 0;
}

void fbo::blit(GLuint src_fbo, GLuint dst_fbo,
    GLint src_x0, GLint src_y0, GLint src_x1, GLint src_y1,
    GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1, GLbitfield mask, GLenum filter) {
    gl_state_bind_framebuffer(dst_fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_fbo);
    glBlitFramebuffer(src_x0, src_y0, src_x1, src_y1,
        dst_x0, dst_y0, dst_x1, dst_y1, mask, filter);
}
