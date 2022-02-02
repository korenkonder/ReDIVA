/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "exposure.h"
#include "../fbo.h"
#include "../gl_state.h"
#include "../post_process.h"
#include "../shader_ft.h"

static void post_process_exposure_free_fbo(post_process_exposure* exp);

post_process_exposure* post_process_exposure_init() {
    post_process_exposure* exp = force_malloc(sizeof(post_process_exposure));
    return exp;
}

void post_process_get_exposure(post_process_exposure* exp,
    bool reset_exposure, GLuint in_tex_0, GLuint in_tex_1) {
    if (!exp)
        return;

    uniform_value[U_EXPOSURE] = 1;
    render_texture_shader_set(&shaders_ft, SHADER_FT_EXPOSURE);

    if (reset_exposure)
        glViewport(0, 0, 32, 1);
    else
        glViewport(exp->exposure_history_counter, 0, 1, 1);
    render_texture_bind(&exp->exposure_history, 0);
    gl_state_active_bind_texture_2d(0, in_tex_0);
    gl_state_active_bind_texture_2d(1, in_tex_1);
    render_texture_draw_params(&shaders_ft, 1, 1, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_EXPOSURE] = 2;
    render_texture_shader_set(&shaders_ft, SHADER_FT_EXPOSURE);

    glViewport(0, 0, 1, 1);
    render_texture_bind(&exp->exposure, 0);
    gl_state_active_bind_texture_2d(0, exp->exposure_history.color_texture->texture);
    render_texture_draw_params(&shaders_ft, 1, 1, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    exp->exposure_history_counter++;
    exp->exposure_history_counter %= 32;
}

void post_process_exposure_init_fbo(post_process_exposure* exp) {
    if (!exp)
        return;

    exp->exposure_history_counter = 0;

    if (!exp->exposure_history.color_texture)
        render_texture_init(&exp->exposure_history, 32, 1, 0, GL_R32F, 0);
    if (!exp->exposure.color_texture)
        render_texture_init(&exp->exposure, 1, 1, 0, GL_R32F, 0);
}

void post_process_exposure_dispose(post_process_exposure* exp) {
    if (!exp)
        return;

    post_process_exposure_free_fbo(exp);
    free(exp);
}

static void post_process_exposure_free_fbo(post_process_exposure* exp) {
    render_texture_free(&exp->exposure_history);
    render_texture_free(&exp->exposure);
}
