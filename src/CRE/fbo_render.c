/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_render.h"
#include "fbo_helper.h"
#include "shared.h"

extern int32_t sv_samples;

const GLenum fbo_render_attachments[] = {
    GL_COLOR_ATTACHMENT0,
};

static void fbo_render_bind_fbo(fbo_render* rfbo, vec2i* res);

fbo_render* fbo_render_init() {
    fbo_render* rfbo = force_malloc(sizeof(fbo_render));
    glGenFramebuffers(1, &rfbo->fbo);
    glGenTextures(2, rfbo->tcb);
    return rfbo;
}

void fbo_render_initialize(fbo_render* rfbo, vec2i* res,
    int32_t vao, shader_fbo* c_shader, shader_fbo* d_shader) {
    if (!rfbo)
        return;

    rfbo->vao = vao;
    rfbo->c_shader = c_shader;
    rfbo->d_shader = d_shader;
    fbo_render_bind_fbo(rfbo, res);
}

void fbo_render_resize(fbo_render* rfbo, vec2i* res) {
    if (!rfbo)
        return;

    fbo_render_bind_fbo(rfbo, res);
}

void fbo_render_draw(fbo_render* rfbo, bool depth) {
    if (!rfbo)
        return;

    glDisable(GL_BLEND);
    if (depth) {
        shader_fbo_use(rfbo->d_shader);
        bind_index_tex2d(0, rfbo->tcb[0]);
        bind_index_tex2d(1, rfbo->tcb[1]);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        glDepthMask(true);
        bind_vertex_array(rfbo->vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(false);
    }
    else {
        shader_fbo_use(rfbo->c_shader);
        bind_index_tex2d(0, rfbo->tcb[0]);
        bind_vertex_array(rfbo->vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void fbo_render_dispose(fbo_render* rfbo) {
    if (!rfbo)
        return;

    glDeleteFramebuffers(1, &rfbo->fbo);
    glDeleteTextures(2, rfbo->tcb);
    free(rfbo);
}

static void fbo_render_bind_fbo(fbo_render* rfbo, vec2i* res) {
    rfbo->res.x = res->x > 1 ? res->x : 1;
    rfbo->res.y = res->y > 1 ? res->y : 1;

    int32_t w = rfbo->res.x;
    int32_t h = rfbo->res.y;

    bind_framebuffer(rfbo->fbo);
    fbo_helper_gen_texture_image(rfbo->tcb[0], w, h,
        GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);                                             // Color
    fbo_helper_gen_texture_image(rfbo->tcb[1], w, h,
        GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 16);                   // Depth
    fbo_helper_get_error_code();
    bind_framebuffer(0);
}
