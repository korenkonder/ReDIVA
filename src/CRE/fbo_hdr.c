/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_hdr.h"
#include "fbo_helper.h"

const GLenum fbo_hdr_c_attachments[] = {
    GL_COLOR_ATTACHMENT1,
    GL_DEPTH_ATTACHMENT,
};

const GLenum fbo_hdr_d_attachments[] = {
    GL_COLOR_ATTACHMENT1,
};

const GLenum fbo_hdr_f_attachments[] = {
    GL_COLOR_ATTACHMENT0,
};

static void fbo_hdr_bind_fbo(fbo_hdr* hfbo, vec2i* res);

fbo_hdr* fbo_hdr_init() {
    fbo_hdr* hfbo = force_malloc(sizeof(fbo_hdr));
    glGenFramebuffers(1, &hfbo->fbo);
    glGenTextures(1, &hfbo->color_tcb);
    glGenTextures(1, &hfbo->depth_tcb);
    glGenTextures(1, &hfbo->buf_tcb);
    return hfbo;
}

void fbo_hdr_initialize(fbo_hdr* hfbo, vec2i* res, int32_t vao, shader_fbo* fxaa_shader) {
    if (!hfbo)
        return;

    hfbo->vao = vao;
    hfbo->fxaa_shader = fxaa_shader;
    fbo_hdr_bind_fbo(hfbo, res);
}

void fbo_hdr_resize(fbo_hdr* hfbo, vec2i* res) {
    if (!hfbo)
        return;

    fbo_hdr_bind_fbo(hfbo, res);
}

void fbo_hdr_draw_fxaa(fbo_hdr* hfbo, int32_t preset) {
    if (!hfbo)
        return;

    bind_framebuffer(hfbo->fbo);
    glDrawBuffers(1, fbo_hdr_d_attachments);
    shader_fbo_use(&hfbo->fxaa_shader[clamp(preset, 3, 5) - 3]);
    bind_index_tex2d(0, hfbo->color_tcb);
    bind_vertex_array(hfbo->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    fbo_helper_blit_same(hfbo->fbo, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0,
        0, 0, hfbo->res.x, hfbo->res.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void fbo_hdr_set_fbo_begin(fbo_hdr* hfbo) {
    bind_framebuffer(hfbo->fbo);
    glDrawBuffers(1, fbo_hdr_d_attachments);
}

void fbo_hdr_set_fbo_end(fbo_hdr* hfbo) {
    fbo_helper_blit_same(hfbo->fbo, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0,
        0, 0, hfbo->res.x, hfbo->res.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void fbo_hdr_dispose(fbo_hdr* hfbo) {
    if (!hfbo)
        return;

    glDeleteFramebuffers(1, &hfbo->fbo);
    glDeleteTextures(1, &hfbo->color_tcb);
    glDeleteTextures(1, &hfbo->depth_tcb);
    glDeleteTextures(1, &hfbo->buf_tcb);
    free(hfbo);
}

static void fbo_hdr_bind_fbo(fbo_hdr* hfbo, vec2i* res) {
    hfbo->res.x = res->x > 1 ? res->x : 1;
    hfbo->res.y = res->y > 1 ? res->y : 1;

    bind_framebuffer(hfbo->fbo);
    fbo_helper_gen_texture_image(hfbo->color_tcb, hfbo->res.x, hfbo->res.y, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image(hfbo->depth_tcb, hfbo->res.x, hfbo->res.y,
        GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 16);
    fbo_helper_gen_texture_image(hfbo->buf_tcb, hfbo->res.x, hfbo->res.y, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 1);
    glDrawBuffers(2, fbo_hdr_c_attachments);
    fbo_helper_get_error_code();
    bind_framebuffer(0);
}
