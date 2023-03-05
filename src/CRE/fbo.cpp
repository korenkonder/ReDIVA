/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo.hpp"
#include "gl_state.hpp"

fbo::fbo() : flags(), width(), height(), buffer(), count(), textures() {

}

fbo::~fbo() {
    free_data();
}

void fbo::init_data(int32_t width, int32_t height,
    GLuint* color_textures, int32_t count, GLuint depth_texture) {
    if (buffer) {
        glDeleteFramebuffers(1, &buffer);
        buffer = 0;
    }

    if (textures) {
        delete[] textures;
        textures = 0;
    }

    count = min_def(count, 8);

    this->width = width;
    this->height = height;
    this->count = count;
    flags = (fbo_flags)0;
    if (depth_texture) {
        this->flags = FBO_DEPTH_ATTACHMENT;
        this->count++;
    }

    textures = new GLuint[this->count];
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

void fbo::free_data() {
    if (buffer) {
        glDeleteFramebuffers(1, &buffer);
        buffer = 0;
    }

    if (textures) {
        delete[] textures;
        textures = 0;
    }
}

void fbo::blit(GLuint src_fbo, GLuint dst_fbo,
    GLint src_x, GLint src_y, GLint src_width, GLint src_height,
    GLint dst_x, GLint dst_y, GLint dst_width, GLint dst_height, GLbitfield mask, GLenum filter) {
    gl_state_bind_read_framebuffer(src_fbo);
    gl_state_bind_draw_framebuffer(dst_fbo);
    glBlitFramebuffer(src_x, src_y, src_x + src_width, src_y + src_height,
        dst_x, dst_y, dst_x + dst_width, dst_y + dst_height, mask, filter);
}
