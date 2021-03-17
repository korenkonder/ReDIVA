/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_helper.h"

void fbo_helper_gen_texture_image_ms(int32_t tcb, int32_t width, int32_t height, int32_t samples,
    GLenum internal_format, GLenum format, GLenum type, int32_t attachment) {
    if (samples > 1) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tcb);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, true);

        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, tcb);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, tcb, 0);
}

void fbo_helper_gen_renderbuffer(int32_t rbo, int32_t width, int32_t height, int32_t samples, GLenum internal_format) {
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    if (samples > 1)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internal_format, width, height);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void fbo_helper_get_error_code() {
    GLenum code = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (code != GL_FRAMEBUFFER_COMPLETE)
        printf("Framebuffer error: %04X\n", code);
}