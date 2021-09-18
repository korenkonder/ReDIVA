/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_render.h"
#include "fbo_helper.h"

extern int32_t sv_samples;

const GLenum fbo_render_attachments[] = {
    GL_COLOR_ATTACHMENT0,
};

static void fbo_render_bind_fbo(fbo_render* rfbo, vec2i* res);

fbo_render* fbo_render_init() {
    fbo_render* rfbo = force_malloc(sizeof(fbo_render));
    return rfbo;
}

void fbo_render_initialize(fbo_render* rfbo, vec2i* res,
    int32_t vao, shader_glsl* c_shader, shader_glsl* d_shader) {
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

    gl_state_disable_blend();
    if (depth) {
        shader_glsl_use(rfbo->d_shader);
        gl_state_active_bind_texture_2d(0, rfbo->tex.color_texture->texture);
        gl_state_active_bind_texture_2d(1, rfbo->tex.depth_texture->texture);
        gl_state_enable_depth_test();
        gl_state_set_depth_func(GL_ALWAYS);
        gl_state_set_depth_mask(GL_TRUE);
        gl_state_bind_vertex_array(rfbo->vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
        gl_state_disable_depth_test();
        gl_state_set_depth_mask(GL_FALSE);
    }
    else {
        shader_glsl_use(rfbo->c_shader);
        gl_state_active_bind_texture_2d(0, rfbo->tex.color_texture->texture);
        gl_state_bind_vertex_array(rfbo->vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    }
}

void fbo_render_dispose(fbo_render* rfbo) {
    if (!rfbo)
        return;

    render_texture_free(&rfbo->tex);
    free(rfbo);
}

static void fbo_render_bind_fbo(fbo_render* rfbo, vec2i* res) {
    rfbo->res.x = res->x > 1 ? res->x : 1;
    rfbo->res.y = res->y > 1 ? res->y : 1;
    render_texture_init(&rfbo->tex, rfbo->res.x, rfbo->res.y, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
}
