/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_dof.h"
#include "fbo_helper.h"

const GLenum fbo_dof_d_attachments[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
};

const GLenum fbo_dof_s_attachments[] = {
    GL_COLOR_ATTACHMENT0,
};

static void fbo_dof_bind_fbo(fbo_dof* dfbo, vec2i* res);

fbo_dof* fbo_dof_init() {
    fbo_dof* dfbo = force_malloc(sizeof(fbo_dof));
    glGenFramebuffers(4, dfbo->fbo);
    glGenTextures(6, dfbo->tcb);
    return dfbo;
}

void fbo_dof_initialize(fbo_dof* dfbo, vec2i* res, int32_t vao,
    shader_fbo* d_shader, int32_t dof_common_ubo, int32_t texcoords_ubo) {
    if (!dfbo)
        return;

    dfbo->vao = vao;
    dfbo->dof_common_ubo = dof_common_ubo;
    dfbo->texcoords_ubo = texcoords_ubo;
    memcpy(dfbo->d_shader, d_shader, sizeof(shader_fbo) * 10);
    fbo_dof_bind_fbo(dfbo, res);
}

void fbo_dof_resize(fbo_dof* dfbo, vec2i* res) {
    if (!dfbo)
        return;

    fbo_dof_bind_fbo(dfbo, res);
}

static void fbo_dof_bind_fbo(fbo_dof* dfbo, vec2i* res) {
    dfbo->res.x = res->x > 20 ? res->x : 20;
    dfbo->res.y = res->y > 20 ? res->y : 20;

    dfbo->res_2.x = res->x > 20 ? res->x / 2 : 10;
    dfbo->res_2.y = res->y > 20 ? res->y / 2 : 10;

    dfbo->res_20.x = res->x > 20 ? res->x / 20 : 1;
    dfbo->res_20.y = res->y > 20 ? res->y / 20 : 1;

    bind_framebuffer(dfbo->fbo[0]);
    glDrawBuffers(1, fbo_dof_s_attachments);
    fbo_helper_gen_texture_image_ms(dfbo->tcb[0], dfbo->res_20.x, dfbo->res_20.y,
        1, GL_RG16F, GL_RG, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    bind_framebuffer(dfbo->fbo[1]);
    glDrawBuffers(1, fbo_dof_s_attachments);
    fbo_helper_gen_texture_image_ms(dfbo->tcb[1], dfbo->res_20.x, dfbo->res_20.y,
        1, GL_RG16F, GL_RG, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    bind_framebuffer(dfbo->fbo[2]);
    glDrawBuffers(2, fbo_dof_d_attachments);
    fbo_helper_gen_texture_image_ms(dfbo->tcb[2], dfbo->res_2.x, dfbo->res_2.y,
        1, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image_ms(dfbo->tcb[3], dfbo->res_2.x, dfbo->res_2.y,
        1, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, 1);
    fbo_helper_get_error_code();

    bind_framebuffer(dfbo->fbo[3]);
    glDrawBuffers(2, fbo_dof_d_attachments);
    fbo_helper_gen_texture_image_ms(dfbo->tcb[4], dfbo->res_2.x, dfbo->res_2.y,
        1, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image_ms(dfbo->tcb[5], dfbo->res_2.x, dfbo->res_2.y,
        1, GL_R16F, GL_RED, GL_HALF_FLOAT, 1);
    fbo_helper_get_error_code();
    bind_framebuffer(0);
}

void fbo_dof_draw(fbo_dof* dfbo, int32_t color_tcb, int32_t depth_tcb,
    int32_t out_fbo, bool dof_f2) {
    if (!dfbo)
        return;

    int o = dof_f2 ? 5 : 0;
    glBindVertexArray(dfbo->vao);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, dfbo->dof_common_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, dfbo->texcoords_ubo);

    glViewport(0, 0, dfbo->res_20.x, dfbo->res_20.y);
    bind_framebuffer(dfbo->fbo[0]);
    shader_fbo_use(&dfbo->d_shader[o]);
    bind_index_tex2d(0, depth_tcb);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    bind_framebuffer(dfbo->fbo[1]);
    shader_fbo_use(&dfbo->d_shader[o + 1]);
    bind_index_tex2d(0, dfbo->tcb[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, dfbo->res_2.x, dfbo->res_2.y);
    bind_framebuffer(dfbo->fbo[2]);
    shader_fbo_use(&dfbo->d_shader[o + 2]);
    bind_index_tex2d(0, depth_tcb);
    bind_index_tex2d(1, color_tcb);
    bind_index_tex2d(2, dfbo->tcb[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    bind_framebuffer(dfbo->fbo[3]);
    shader_fbo_use(&dfbo->d_shader[o + 3]);
    bind_index_tex2d(0, dfbo->tcb[3]);
    bind_index_tex2d(1, dfbo->tcb[2]);
    bind_index_tex2d(2, dfbo->tcb[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, dfbo->res.x, dfbo->res.y);
    bind_framebuffer(out_fbo);
    shader_fbo_use(&dfbo->d_shader[o + 4]);
    bind_index_tex2d(0, dfbo->tcb[4]);
    bind_index_tex2d(1, dfbo->tcb[5]);
    bind_index_tex2d(2, dfbo->tcb[1]);
    bind_index_tex2d(3, color_tcb);
    bind_index_tex2d(4, depth_tcb);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void fbo_dof_dispose(fbo_dof* dfbo) {
    if (!dfbo)
        return;

    glDeleteFramebuffers(4, dfbo->fbo);
    glDeleteTextures(6, dfbo->tcb);
    free(dfbo);
}
