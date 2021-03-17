/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_render.h"
#include "fbo_helper.h"

extern int32_t sv_samples;

const GLenum fbo_render_c_attachments[] = {
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT0,
};

const GLenum fbo_render_f_attachments[] = {
    GL_COLOR_ATTACHMENT4,
};

const GLenum fbo_render_g_attachments[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
};

static void fbo_render_bind_fbo(fbo_render* rfbo, vec2i* res);

fbo_render* fbo_render_init() {
    fbo_render* rfbo = force_malloc(sizeof(fbo_render));
    glGenFramebuffers(1, &rfbo->fbo);
    glGenRenderbuffers(1, &rfbo->rbo);
    glGenTextures(5, rfbo->tcb);
    return rfbo;
}

void fbo_render_initialize(fbo_render* rfbo, vec2i* res, int32_t vao,
    shader_fbo* c_shader, shader_fbo* g_shader) {
    if (!rfbo)
        return;

    rfbo->vao = vao;
    memcpy(rfbo->c_shader, c_shader, sizeof(shader_fbo) * 10);
    memcpy(rfbo->g_shader, g_shader, sizeof(shader_fbo) * 5);
    fbo_render_bind_fbo(rfbo, res);
}

void fbo_render_resize(fbo_render* rfbo, vec2i* res) {
    if (!rfbo)
        return;

    fbo_render_bind_fbo(rfbo, res);
}

static void fbo_render_bind_fbo(fbo_render* rfbo, vec2i* res) {
    rfbo->res.x = res->x > 1 ? res->x : 1;
    rfbo->res.y = res->y > 1 ? res->y : 1;
    rfbo->samples = sv_samples;

    glBindFramebuffer(GL_FRAMEBUFFER, rfbo->fbo);
    rfbo->target = rfbo->samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    fbo_helper_gen_texture_image_ms(rfbo->tcb[0], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image_ms(rfbo->tcb[1], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_RGBA8_SNORM, GL_RGBA, GL_BYTE, 1);
    fbo_helper_gen_texture_image_ms(rfbo->tcb[2], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, 2);
    fbo_helper_gen_texture_image_ms(rfbo->tcb[3], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, 3);
    fbo_helper_gen_texture_image_ms(rfbo->tcb[4], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 4);
    fbo_helper_gen_renderbuffer(rfbo->rbo, rfbo->res.x, rfbo->res.y, rfbo->samples, GL_DEPTH24_STENCIL8);
    fbo_helper_get_error_code();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void fbo_render_draw_c(fbo_render* rfbo, bool depth) {
    if (!rfbo)
        return;

    glEnablei(GL_BLEND, 0);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x00, 0xFF);
    glStencilMask(0xFF);
    if (depth) {
        shader_fbo* c_shader = rfbo->samples > 1 ? rfbo->samples > 2 ? rfbo->samples > 4 ? rfbo->samples > 8 ?
            &rfbo->c_shader[9] : &rfbo->c_shader[8] : &rfbo->c_shader[7] : &rfbo->c_shader[6] : &rfbo->c_shader[5];
        shader_fbo_use(c_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(rfbo->target, rfbo->tcb[4]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(rfbo->target, rfbo->tcb[0]);
    }
    else {
        shader_fbo* c_shader = rfbo->samples > 1 ? rfbo->samples > 2 ? rfbo->samples > 4 ? rfbo->samples > 8 ?
            &rfbo->c_shader[4] : &rfbo->c_shader[3] : &rfbo->c_shader[2] : &rfbo->c_shader[1] : &rfbo->c_shader[0];
        shader_fbo_use(c_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(rfbo->target, rfbo->tcb[4]);
    }
    glBindVertexArray(rfbo->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisablei(GL_BLEND, 0);
    glDisable(GL_STENCIL_TEST);
}

void fbo_render_draw_g(fbo_render* rfbo, int32_t dir_lights_tcb,
    int32_t dir_lights_count, int32_t point_lights_tcb, int32_t point_lights_count) {
    if (!rfbo)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, rfbo->fbo);
    glDrawBuffers(1, fbo_render_f_attachments);
    shader_fbo* g_shader = rfbo->samples > 1 ? rfbo->samples > 2 ? rfbo->samples > 4 ? rfbo->samples > 8 ?
        &rfbo->g_shader[4] : &rfbo->g_shader[3] : &rfbo->g_shader[2] : &rfbo->g_shader[1] : &rfbo->g_shader[0];
    shader_fbo_use(g_shader);
    for (int32_t i = 0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(rfbo->target, rfbo->tcb[i]);
    }
    shader_fbo_set_int(g_shader, "dirLightsCount", dir_lights_count);
    shader_fbo_set_int(g_shader, "pointLightsCount", point_lights_count);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, dir_lights_tcb);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, point_lights_tcb);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 0xFF, 0xFF);
    glStencilMask(0xFF);
    glBindVertexArray(rfbo->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_STENCIL_TEST);
}

void fbo_render_dispose(fbo_render* rfbo) {
    if (!rfbo)
        return;

    glDeleteFramebuffers(1, &rfbo->fbo);
    glDeleteRenderbuffers(1, &rfbo->rbo);
    glDeleteTextures(5, rfbo->tcb);
    free(rfbo);
}
