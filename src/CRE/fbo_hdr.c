/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_hdr.h"
#include "fbo_helper.h"

static void fbo_hdr_bind_fbo(fbo_hdr* hfbo, vec2i* res);

fbo_hdr* fbo_hdr_init() {
    fbo_hdr* hfbo = force_malloc(sizeof(fbo_hdr));
    glGenSamplers(1, &hfbo->sampler);
    return hfbo;
}

void fbo_hdr_initialize(fbo_hdr* hfbo, vec2i* res, int32_t vao, shader_glsl* fxaa_shader) {
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

    gl_state_bind_framebuffer(hfbo->buf.fbos[0]);
    shader_glsl_use(&hfbo->fxaa_shader[clamp(preset, 3, 5) - 3]);
    gl_state_active_bind_texture_2d(0, hfbo->color.color_texture->texture);
    gl_state_bind_sampler(0, hfbo->sampler);
    gl_state_bind_vertex_array(hfbo->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    fbo_helper_blit(hfbo->buf.fbos[0], GL_COLOR_ATTACHMENT0,
        hfbo->color.fbos[0], GL_COLOR_ATTACHMENT0,
        0, 0, hfbo->width, hfbo->height,
        0, 0, hfbo->width, hfbo->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void fbo_hdr_set_fbo_begin(fbo_hdr* hfbo) {
    gl_state_bind_framebuffer(hfbo->buf.fbos[0]);
}

void fbo_hdr_set_fbo_end(fbo_hdr* hfbo) {
    fbo_helper_blit(hfbo->buf.fbos[0], GL_COLOR_ATTACHMENT0,
        hfbo->color.fbos[0], GL_COLOR_ATTACHMENT0,
        0, 0, hfbo->width, hfbo->height,
        0, 0, hfbo->width, hfbo->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void fbo_hdr_dispose(fbo_hdr* hfbo) {
    if (!hfbo)
        return;

    render_texture_free(&hfbo->color);
    render_texture_free(&hfbo->back_2d);
    render_texture_free(&hfbo->buf);
    free(hfbo);
}

static void fbo_hdr_bind_fbo(fbo_hdr* hfbo, vec2i* res) {
    hfbo->width = max(res->x, 1);
    hfbo->height = max(res->y, 1);

    render_texture_init(&hfbo->color, hfbo->width, hfbo->height, 0, GL_R11F_G11F_B10F, GL_DEPTH24_STENCIL8);
    render_texture_init(&hfbo->back_2d, hfbo->width, hfbo->height, 0, GL_R11F_G11F_B10F, 0);
    render_texture_init(&hfbo->buf, hfbo->width, hfbo->height, 0, GL_R11F_G11F_B10F, 0);

    glSamplerParameteri(hfbo->sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(hfbo->sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(hfbo->sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(hfbo->sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
