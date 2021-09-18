/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_pp.h"
#include "fbo_helper.h"
#include "../KKdLib/half_t.h"
#include "shader_aft.h"

const GLenum fbo_pp_s_attachments[] = {
    GL_COLOR_ATTACHMENT0,
};

extern shader_set_data shaders_aft;

static void fbo_pp_bind_fbo(fbo_pp* pfbo, vec2i* res);

fbo_pp* fbo_pp_init() {
    fbo_pp* pfbo = force_malloc(sizeof(fbo_pp));
    glGenFramebuffers(11, pfbo->fbo);
    glGenTextures(10, pfbo->tex);
    glGenTextures(1, &pfbo->exposure_history);
    glGenTextures(1, &pfbo->tone_map);
    return pfbo;
}

void fbo_pp_initialize(fbo_pp* pfbo, vec2i* res, int32_t vao, shader_glsl* b_shader) {
    if (!pfbo)
        return;

    pfbo->vao = vao;
    pfbo->exposure_history_counter = 0;
    pfbo->b_shader = b_shader;

    shader_glsl_set_vec2_array(&pfbo->b_shader[4], "direction", 6, ((vec2[]) {
        { 1.0f / 128.0f, 0.0f },
        { 2.0f / 128.0f, 0.0f },
        { 3.0f / 128.0f, 0.0f },
        { 4.0f / 128.0f, 0.0f },
        { 5.0f / 128.0f, 0.0f },
        { 6.0f / 128.0f, 0.0f },
    }));
    shader_glsl_set_vec2_array(&pfbo->b_shader[5], "direction", 6, ((vec2[]) {
        { 1.0f / 64.0f, 0.0f },
        { 2.0f / 64.0f, 0.0f },
        { 3.0f / 64.0f, 0.0f },
        { 4.0f / 64.0f, 0.0f },
        { 5.0f / 64.0f, 0.0f },
        { 6.0f / 64.0f, 0.0f },
    }));
    shader_glsl_set_vec2_array(&pfbo->b_shader[6], "direction", 6, ((vec2[]) {
        { 1.0f / 32.0f, 0.0f},
        { 2.0f / 32.0f, 0.0f },
        { 3.0f / 32.0f, 0.0f },
        { 4.0f / 32.0f, 0.0f },
        { 5.0f / 32.0f, 0.0f },
        { 6.0f / 32.0f, 0.0f },
    }));
    shader_glsl_set_vec2_array(&pfbo->b_shader[7], "direction", 6, ((vec2[]) {
        { 0.0f, 1.0f / 72.0f },
        { 0.0f, 2.0f / 72.0f },
        { 0.0f, 3.0f / 72.0f },
        { 0.0f, 4.0f / 72.0f },
        { 0.0f, 5.0f / 72.0f },
        { 0.0f, 6.0f / 72.0f },
    }));
    shader_glsl_set_vec2_array(&pfbo->b_shader[8], "direction", 6, ((vec2[]) {
        { 0.0f, 1.0f / 36.0f },
        { 0.0f, 2.0f / 36.0f },
        { 0.0f, 3.0f / 36.0f },
        { 0.0f, 4.0f / 36.0f },
        { 0.0f, 5.0f / 36.0f },
        { 0.0f, 6.0f / 36.0f },
    }));
    shader_glsl_set_vec2_array(&pfbo->b_shader[9], "direction", 6, ((vec2[]) {
        { 0.0f, 1.0f / 18.0f },
        { 0.0f, 2.0f / 18.0f },
        { 0.0f, 3.0f / 18.0f },
        { 0.0f, 4.0f / 18.0f },
        { 0.0f, 5.0f / 18.0f },
        { 0.0f, 6.0f / 18.0f },
    }));
    shader_glsl_set_vec4_value(&pfbo->b_shader[10], "res",
        256.0f, 144.0f, 1.0f / 256.0f, 1.0f / 144.0f);

    glGenFramebuffers(11, pfbo->fbo);
    glGenTextures(11, pfbo->tex);

    gl_state_bind_framebuffer(pfbo->fbo[0]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[0], 256, 144, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[1]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[1], 128, 72, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[2]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[2], 128, 72, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[3]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[3], 64, 36, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[4]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[4], 64, 36, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[5]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[5], 32, 18, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[6]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[6], 32, 18, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[7]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[7], 8, 8, GL_R32F, GL_RED, GL_FLOAT, 0);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[8]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[8], 256, 144,
        GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, 0);
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[9]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->exposure_history, 32, 1, GL_R32F, GL_RED, GL_FLOAT, 0);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]){ GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();

    gl_state_bind_framebuffer(pfbo->fbo[10]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tex[9], 1, 1, GL_R32F, GL_RED, GL_FLOAT, 0);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();
    gl_state_bind_texture_2d(0);

    fbo_pp_bind_fbo(pfbo, res);
}

void fbo_pp_resize(fbo_pp* pfbo, vec2i* res) {
    if (!pfbo)
        return;

    fbo_pp_bind_fbo(pfbo, res);
}

void fbo_pp_tone_map_set(fbo_pp* pfbo, vec2* tone_map_data, int32_t count) {
    gl_state_bind_texture_2d(0);
    glBindTexture(GL_TEXTURE_1D, pfbo->tone_map);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG16F, count, 0, GL_RG, GL_FLOAT, tone_map_data);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteriv(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_GREEN });
    glBindTexture(GL_TEXTURE_1D, 0);
}

void fbo_pp_draw(fbo_pp* pfbo, tone_map* tm,
    texture* in_tex, texture* light_proj_tex, texture* back_2d_tex,
    void* out_fbo, void(*out_fbo_func_begin)(void*), void(*out_fbo_func_end)(void*)) {
    gl_state_bind_vertex_array(pfbo->vao);
    int i = 0;
    if (pfbo->count_down > 0) {
        shader_glsl_use(&pfbo->b_shader[0]);
        for (; i < pfbo->count_down; i++) {
            glViewport(0, 0, pfbo->res_down[i].x, pfbo->res_down[i].y); // 0
            gl_state_bind_framebuffer(pfbo->fbo_down[i]);
            gl_state_active_bind_texture_2d(0, i == 0 ? in_tex->texture : pfbo->tex_down[i - 1]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
        }
        i--;
    }

    glViewport(0, 0, 256, 144); // 1
    gl_state_bind_framebuffer(pfbo->fbo[0]);
    shader_glsl_use(&pfbo->b_shader[1]);
    gl_state_active_bind_texture_2d(0, pfbo->count_down > 0 ? pfbo->tex_down[i] : in_tex->target);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 128, 72); // 2
    gl_state_bind_framebuffer(pfbo->fbo[1]);
    shader_glsl_use(&pfbo->b_shader[2]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 64, 36); // 3
    gl_state_bind_framebuffer(pfbo->fbo[3]);
    shader_glsl_use(&pfbo->b_shader[2]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 32, 18); // 4
    gl_state_bind_framebuffer(pfbo->fbo[5]);
    shader_glsl_use(&pfbo->b_shader[2]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[3]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 8, 8); // 5
    gl_state_bind_framebuffer(pfbo->fbo[7]);
    shader_glsl_use(&pfbo->b_shader[3]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[5]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 128, 72); // 6
    gl_state_bind_framebuffer(pfbo->fbo[2]);
    shader_glsl_use(&pfbo->b_shader[4]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 64, 36); // 7
    gl_state_bind_framebuffer(pfbo->fbo[4]);
    shader_glsl_use(&pfbo->b_shader[5]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[3]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 32, 18); // 8
    gl_state_bind_framebuffer(pfbo->fbo[6]);
    shader_glsl_use(&pfbo->b_shader[6]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[5]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 128, 72); // 9
    gl_state_bind_framebuffer(pfbo->fbo[1]);
    shader_glsl_use(&pfbo->b_shader[7]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[2]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 64, 36); // 10
    gl_state_bind_framebuffer(pfbo->fbo[3]);
    shader_glsl_use(&pfbo->b_shader[8]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[4]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 32, 18); // 11
    gl_state_bind_framebuffer(pfbo->fbo[5]);
    shader_glsl_use(&pfbo->b_shader[9]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[6]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 256, 144); // 12
    gl_state_bind_framebuffer(pfbo->fbo[8]);
    shader_glsl_use(&pfbo->b_shader[10]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 256, 144); // 13
    gl_state_bind_framebuffer(pfbo->fbo[0]);
    shader_glsl_use(&pfbo->b_shader[11]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[8]);
    gl_state_active_bind_texture_2d(1, pfbo->tex[1]);
    gl_state_active_bind_texture_2d(2, pfbo->tex[3]);
    gl_state_active_bind_texture_2d(3, pfbo->tex[5]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(pfbo->exposure_history_counter, 0, 1, 1); // 14
    gl_state_bind_framebuffer(pfbo->fbo[9]);
    shader_glsl_use(&pfbo->b_shader[12]);
    gl_state_active_bind_texture_2d(0, pfbo->tex[7]);
    gl_state_active_bind_texture_2d(1, pfbo->tex[3]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glViewport(0, 0, 1, 1); // 15
    gl_state_bind_framebuffer(pfbo->fbo[10]);
    shader_glsl_use(&pfbo->b_shader[13]);
    gl_state_active_bind_texture_2d(0, pfbo->exposure_history);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    mat4 lens_flare_mat = mat4_identity;
    mat4 lens_shaft_mat = mat4_identity;
    mat4_scale_mult(&lens_flare_mat, 1.0f, (float_t)pfbo->res.y / (float_t)pfbo->res.x, 1.0, &lens_flare_mat);
    mat4_scale_mult(&lens_shaft_mat, 1.0f, (float_t)pfbo->res.y / (float_t)pfbo->res.x, 1.0, &lens_shaft_mat);

    uniform_value[U16] = 0;
    uniform_value[U_TONE_MAP] = tm->tone_map_method;
    uniform_value[U_FLARE] = 0;
    uniform_value[U_SCENE_FADE] = tm->scene_fade_alpha > 0.009999999f ? 1 : 0;
    uniform_value[U_AET_BACK] = 0;
    uniform_value[U_LIGHT_PROJ] = 0;
    uniform_value[U_NPR] = 0;
    uniform_value[U25] = 0;

    glViewport(0, 0, pfbo->res.x, pfbo->res.y); // 16
    out_fbo_func_begin(out_fbo);
    shader_set(&shaders_aft, SHADER_AFT_TONEMAP);
    shader_state_matrix_set_mvp_separate(&shaders_aft,
        (mat4*)&mat4_identity, (mat4*)&mat4_identity, (mat4*)&mat4_identity);
    shader_state_matrix_set_texture(&shaders_aft, 4, &lens_flare_mat);
    shader_state_matrix_set_texture(&shaders_aft, 5, &lens_shaft_mat);
    shader_local_vert_set_ptr(&shaders_aft, 1, &tm->data.p_exposure);
    shader_local_frag_set_ptr(&shaders_aft, 1, &tm->data.p_flare_coef);
    shader_local_frag_set_ptr(&shaders_aft, 2, &tm->data.p_fade_color);
    shader_local_frag_set_ptr(&shaders_aft, 4, &tm->data.p_tone_scale);
    shader_local_frag_set_ptr(&shaders_aft, 5, &tm->data.p_tone_offset);
    shader_local_frag_set_ptr(&shaders_aft, 6, &tm->data.p_fade_func);
    shader_local_frag_set_ptr(&shaders_aft, 7, &tm->data.p_inv_tone);
    gl_state_active_bind_texture_2d(0, in_tex->texture);
    gl_state_active_bind_texture_2d(1, pfbo->tex[0]);
    gl_state_active_bind_texture_2d(2, 0);
    gl_state_active_texture(2);
    glBindTexture(GL_TEXTURE_1D, pfbo->tone_map);
    gl_state_active_bind_texture_2d(3, pfbo->tex[9]);
    if (uniform_value[U_LIGHT_PROJ])
        gl_state_active_bind_texture_2d(6, light_proj_tex->target);
    else if (uniform_value[U_AET_BACK])
        gl_state_active_bind_texture_2d(6, back_2d_tex->target);
    shader_draw_arrays(&shaders_aft, GL_TRIANGLE_STRIP, 0, 3);
    gl_state_active_texture(2);
    glBindTexture(GL_TEXTURE_1D, 0);
    out_fbo_func_end(out_fbo);

    pfbo->exposure_history_counter++;
    pfbo->exposure_history_counter %= 32;
}

void fbo_pp_dispose(fbo_pp* pfbo) {
    if (!pfbo)
        return;

    glDeleteFramebuffers(pfbo->full_count_down, pfbo->fbo_down);
    glDeleteTextures(pfbo->full_count_down, pfbo->tex_down);
    glDeleteFramebuffers(11, pfbo->fbo);
    glDeleteTextures(10, pfbo->tex);
    glGenTextures(1, &pfbo->exposure_history);
    glGenTextures(1, &pfbo->tone_map);
    free(pfbo->fbo_down);
    free(pfbo->tex_down);
    free(pfbo->res_down);
    free(pfbo);
}

static void fbo_pp_bind_fbo(fbo_pp* pfbo, vec2i* res) {

    pfbo->res.x = res->x > 1 ? res->x : 1;
    pfbo->res.y = res->y > 1 ? res->y : 1;

    int i = 0;
    vec2i r = pfbo->res;
    while (r.x > 512 && r.y > 288) {
        r.x /= 2;
        r.y /= 2;
        i++;
    }

    if (i < pfbo->full_count_down)
        pfbo->full_count_down = pfbo->count_down;
    else
        pfbo->full_count_down = i;

    int32_t* fbo_down = pfbo->fbo_down;
    int32_t* tex_down = pfbo->tex_down;
    vec2i* res_down = pfbo->res_down;

    if (!fbo_down)
        fbo_down = force_malloc_s(int32_t, i);
    else if (pfbo->count_down < i) {
        void* temp = force_malloc_s(int32_t, i);
        memcpy(temp, fbo_down, sizeof(int32_t) * pfbo->count_down);
        free(fbo_down);
        fbo_down = temp;
    }

    if (!tex_down)
        tex_down = force_malloc_s(int32_t, i);
    else if (pfbo->count_down < i) {
        void* temp = force_malloc_s(int32_t, i);
        memcpy(temp, tex_down, sizeof(int32_t) * pfbo->count_down);
        free(tex_down);
        tex_down = temp;
    }

    if (!res_down)
        res_down = force_malloc_s(vec2i, i);
    else if (pfbo->count_down < i) {
        void* temp = force_malloc_s(vec2i, i);
        memcpy(temp, res_down, sizeof(vec2i) * pfbo->count_down);
        free(res_down);
        res_down = temp;
    }

    pfbo->count_down = i;

    for (i = 0; i < pfbo->count_down; i++) {
        if (fbo_down[i] == 0)
            glGenFramebuffers(1, &fbo_down[i]);
        if (tex_down[i] == 0)
            glGenTextures(1, &tex_down[i]);
    }

    i = 0;
    r = pfbo->res;
    while (r.x > 512 && r.y > 288) {
        res_down[i].x = r.x /= 2;
        res_down[i].y = r.y /= 2;
        i++;
    }

    for (i = 0; i < pfbo->count_down; i++) {
        gl_state_bind_framebuffer(fbo_down[i]);
        glDrawBuffers(1, fbo_pp_s_attachments);
        fbo_helper_gen_texture_image(tex_down[i], res_down[i].x, res_down[i].y,
            GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
        fbo_helper_get_error_code();
    }

    gl_state_bind_framebuffer(0);

    pfbo->fbo_down = fbo_down;
    pfbo->tex_down = tex_down;
    pfbo->res_down = res_down;
}
