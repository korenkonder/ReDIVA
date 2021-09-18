/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo.h"

void fbo_init(fbo_struct* fbo, int32_t width, int32_t height,
    GLuint* color_textures, int32_t count, GLuint depth_texture) {
    if (fbo->fbo)
        glDeleteFramebuffers(1, &fbo->fbo);
    free(fbo->textures);

    count = min(count, 8);

    fbo->width = width;
    fbo->height = height;
    fbo->count = count;
    fbo->flags = 0;
    if (depth_texture) {
        fbo->flags = FBO_DEPTH_ATTACHMENT;
        fbo->count = count + 1;
    }

    fbo->textures = force_malloc_s(GLuint, fbo->count);
    for (int32_t i = 0; i < count; i++)
        fbo->textures[i] = color_textures[i];

    if (fbo->flags & FBO_DEPTH_ATTACHMENT)
        fbo->textures[count] = depth_texture;

    glGenFramebuffers(1, &fbo->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    for (int32_t i = 0; i < count; i++)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, fbo->textures[i], 0);

    if (fbo->flags & FBO_DEPTH_ATTACHMENT)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo->textures[fbo->count - 1], 0);

    GLenum color_attachments[8];
    for (int32_t i = 0; i < count; i++)
        color_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(count, color_attachments);
    glCheckFramebufferStatus(GL_FRAMEBUFFER);
}

void fbo_free(fbo_struct* fbo) {
    if (fbo->fbo)
        glDeleteFramebuffers(1, &fbo->fbo);
    free(fbo->textures);
}
