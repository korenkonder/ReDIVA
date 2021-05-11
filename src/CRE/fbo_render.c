/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_render.h"
#include "fbo_helper.h"
#include "shared.h"

extern int32_t sv_samples;

const GLenum fbo_render_c_attachments[] = {
    GL_COLOR_ATTACHMENT3,
};

const GLenum fbo_render_g_attachments[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
};

static void fbo_render_bind_fbo(fbo_render* rfbo, vec2i* res);

fbo_render* fbo_render_init() {
    fbo_render* rfbo = force_malloc(sizeof(fbo_render));
    glGenFramebuffers(1, &rfbo->fbo);
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

    bind_framebuffer(rfbo->fbo);
    rfbo->target = rfbo->samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    fbo_helper_gen_texture_image_ms(rfbo->tcb[0], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 16);
    fbo_helper_gen_texture_image_ms(rfbo->tcb[1], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, 0);
    fbo_helper_gen_texture_image_ms(rfbo->tcb[2], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, 1);
    fbo_helper_gen_texture_image_ms(rfbo->tcb[3], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_10_10_10_2, 2);
    fbo_helper_gen_texture_image_ms(rfbo->tcb[4], rfbo->res.x, rfbo->res.y,
        rfbo->samples, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 3);
    fbo_helper_get_error_code();
    bind_framebuffer(0);
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
        bind_index_tex2d(0, rfbo->tcb[4]);
        bind_index_tex2d(1, rfbo->tcb[0]);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        glDepthMask(true);
    }
    else {
        shader_fbo* c_shader = rfbo->samples > 1 ? rfbo->samples > 2 ? rfbo->samples > 4 ? rfbo->samples > 8 ?
            &rfbo->c_shader[4] : &rfbo->c_shader[3] : &rfbo->c_shader[2] : &rfbo->c_shader[1] : &rfbo->c_shader[0];
        shader_fbo_use(c_shader);
        bind_index_tex2d(0, rfbo->tcb[4]);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(false);
    }
    bind_vertex_array(rfbo->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisablei(GL_BLEND, 0);
    glDisable(GL_STENCIL_TEST);
}

void fbo_render_draw_g(fbo_render* rfbo,
    int32_t light_dir_tcb, int32_t light_dir_count,
    int32_t light_point_tcb, int32_t light_point_count) {
    if (!rfbo)
        return;

    glDrawBuffers(1, fbo_render_c_attachments);
    shader_fbo* g_shader = rfbo->samples > 1 ? rfbo->samples > 2 ? rfbo->samples > 4 ? rfbo->samples > 8 ?
        &rfbo->g_shader[4] : &rfbo->g_shader[3] : &rfbo->g_shader[2] : &rfbo->g_shader[1] : &rfbo->g_shader[0];
    shader_fbo_use(g_shader);
    shader_fbo_set_int(g_shader, "lightDirCount", light_dir_count);
    shader_fbo_set_int(g_shader, "lightPointCount", light_point_count);

    for (int32_t i = 0; i < 4; i++)
        bind_index_tex2d(i, rfbo->tcb[i]);
    bind_index_tex2d(4, light_dir_tcb);
    bind_index_tex2d(5, light_point_tcb);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 0xFF, 0xFF);
    glStencilMask(0xFF);
    bind_vertex_array(rfbo->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_STENCIL_TEST);
}

void fbo_render_dispose(fbo_render* rfbo) {
    if (!rfbo)
        return;

    glDeleteFramebuffers(1, &rfbo->fbo);
    glDeleteTextures(5, rfbo->tcb);
    free(rfbo);
}
