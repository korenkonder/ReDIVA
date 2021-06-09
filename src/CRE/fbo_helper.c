/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_helper.h"

void fbo_helper_blit(int32_t src_fbo, GLenum src_mode, int32_t dst_fbo, GLenum dst_mode,
    int32_t src_x0, int32_t src_y0, int32_t src_x1, int32_t src_y1,
    int32_t dst_x0, int32_t dst_y0, int32_t dst_x1, int32_t dst_y1, GLbitfield mask, GLenum filter) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src_fbo);
    glReadBuffer(src_mode);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_fbo);
    glDrawBuffer(dst_mode);
    glBlitFramebuffer(src_x0, src_y0, src_x1, src_y1,
        dst_x0, dst_y0, dst_x1, dst_y1, mask, filter);
}

void fbo_helper_blit_same(int32_t fbo, GLenum src_mode, GLenum dst_mode,
    int32_t x0, int32_t y0, int32_t x1, int32_t y1, GLbitfield mask, GLenum filter) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadBuffer(src_mode);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(dst_mode);
    glBlitFramebuffer(x0, y0, x1, y1, x0, y0, x1, y1, mask, filter);
}

void fbo_helper_gen_texture_image(int32_t tcb, int32_t width, int32_t height,
    GLenum internal_format, GLenum format, GLenum type, int32_t attachment) {
    bind_tex2d(tcb);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (attachment >= 0 && attachment <= 15)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, tcb, 0);
    else if (attachment == 16)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, tcb, 0);
}

void fbo_helper_get_error_code() {
    GLenum code = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (code != GL_FRAMEBUFFER_COMPLETE)
        printf("Framebuffer error: %04X\n", code);
}