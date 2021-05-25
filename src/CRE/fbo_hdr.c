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

static void fbo_hdr_bind_fbo(fbo_hdr* hfbo, vec2i* res, vec2i* res_2d);

fbo_hdr* fbo_hdr_init() {
    fbo_hdr* hfbo = force_malloc(sizeof(fbo_hdr));
    glGenFramebuffers(2, hfbo->fbo);
    glGenTextures(4, hfbo->tcb);
    return hfbo;
}

void fbo_hdr_initialize(fbo_hdr* hfbo, vec2i* res, vec2i* res_2d,
    int32_t vao, shader_fbo* f_shader, shader_fbo* h_shader) {
    if (!hfbo)
        return;

    hfbo->vao = vao;
    hfbo->f_shader = *f_shader;
    memcpy(hfbo->h_shader, h_shader, sizeof(shader_fbo) * 2);
    fbo_hdr_bind_fbo(hfbo, res, res_2d);
}

void fbo_hdr_resize(fbo_hdr* hfbo, vec2i* res, vec2i* res_2d) {
    if (!hfbo)
        return;

    fbo_hdr_bind_fbo(hfbo, res, res_2d);
}

static void fbo_hdr_bind_fbo(fbo_hdr* hfbo, vec2i* res, vec2i* res_2d) {
    hfbo->res.x = res->x > 1 ? res->x : 1;
    hfbo->res.y = res->y > 1 ? res->y : 1;

    hfbo->res_2d.x = res_2d->x > 1 ? res_2d->x : 1;
    hfbo->res_2d.y = res_2d->y > 1 ? res_2d->y : 1;

    bind_framebuffer(hfbo->fbo[0]);
    fbo_helper_gen_texture_image_ms(hfbo->tcb[0], hfbo->res.x, hfbo->res.y, 1, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image_ms(hfbo->tcb[1], hfbo->res.x, hfbo->res.y, 1,
        GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 16);
    fbo_helper_gen_texture_image_ms(hfbo->tcb[2], hfbo->res.x, hfbo->res.y, 1, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 1);
    glDrawBuffers(2, fbo_hdr_c_attachments);
    fbo_helper_get_error_code();

    bind_framebuffer(hfbo->fbo[1]);
    fbo_helper_gen_texture_image_ms(hfbo->tcb[3], hfbo->res_2d.x, hfbo->res_2d.y, 1, GL_RGBA8, GL_RGBA, GL_BYTE, 0);
    glDrawBuffers(1, fbo_hdr_f_attachments);
    fbo_helper_get_error_code();
    bind_framebuffer(0);
}

void fbo_hdr_draw_aa(fbo_hdr* hfbo) {
    if (!hfbo)
        return;

    glViewport(0, 0, hfbo->res_2d.x, hfbo->res_2d.y);
    bind_framebuffer(hfbo->fbo[1]);
    shader_fbo_use(&hfbo->h_shader[hfbo->fxaa ? 1 : 0]);
    bind_index_tex2d(0, hfbo->tcb[0]);
    bind_vertex_array(hfbo->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void fbo_hdr_draw(fbo_hdr* hfbo) {
    if (!hfbo)
        return;

    shader_fbo_use(&hfbo->f_shader);
    bind_index_tex2d(0, hfbo->tcb[3]);
    bind_vertex_array(hfbo->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void fbo_hdr_dispose(fbo_hdr* hfbo) {
    if (!hfbo)
        return;

    glDeleteFramebuffers(2, hfbo->fbo);
    glDeleteTextures(4, hfbo->tcb);
    free(hfbo);
}
