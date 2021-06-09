/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fbo_pp.h"
#include "fbo_helper.h"
#include "../KKdLib/half_t.h"

const GLenum fbo_pp_s_attachments[] = {
    GL_COLOR_ATTACHMENT0,
};

const GLenum fbo_pp_d_attachments[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
};

extern int32_t tone_map_ubo_binding;

static void fbo_pp_bind_fbo(fbo_pp* pfbo, vec2i* res);

fbo_pp* fbo_pp_init() {
    fbo_pp* pfbo = force_malloc(sizeof(fbo_pp));
    glGenFramebuffers(11, pfbo->fbo);
    glGenTextures(15, pfbo->tcb);
    return pfbo;
}

void fbo_pp_initialize(fbo_pp* pfbo, vec2i* res, int32_t vao,
    shader_fbo* b_shader, shader_fbo* t_shader, int32_t tone_map_ubo) {
    if (!pfbo)
        return;

    pfbo->vao = vao;
    pfbo->tone_map_ubo = tone_map_ubo;
    pfbo->dst_pixel = 0;
    memset(&pfbo->tone_map, 0, sizeof(texture));
    pfbo->b_shader = b_shader;
    pfbo->t_shader = t_shader;

    shader_fbo_set_vec2_array(&pfbo->b_shader[4], "direction", 6, ((vec2[]) {
        { 1.0f / 128.0f, 0.0f },
        { 2.0f / 128.0f, 0.0f },
        { 3.0f / 128.0f, 0.0f },
        { 4.0f / 128.0f, 0.0f },
        { 5.0f / 128.0f, 0.0f },
        { 6.0f / 128.0f, 0.0f },
    }));
    shader_fbo_set_vec2_array(&pfbo->b_shader[5], "direction", 6, ((vec2[]) {
        { 1.0f / 64.0f, 0.0f },
        { 2.0f / 64.0f, 0.0f },
        { 3.0f / 64.0f, 0.0f },
        { 4.0f / 64.0f, 0.0f },
        { 5.0f / 64.0f, 0.0f },
        { 6.0f / 64.0f, 0.0f },
    }));
    shader_fbo_set_vec2_array(&pfbo->b_shader[6], "direction", 6, ((vec2[]) {
        { 1.0f / 32.0f, 0.0f},
        { 2.0f / 32.0f, 0.0f },
        { 3.0f / 32.0f, 0.0f },
        { 4.0f / 32.0f, 0.0f },
        { 5.0f / 32.0f, 0.0f },
        { 6.0f / 32.0f, 0.0f },
    }));
    shader_fbo_set_vec2_array(&pfbo->b_shader[7], "direction", 6, ((vec2[]) {
        { 0.0f, 1.0f / 72.0f },
        { 0.0f, 2.0f / 72.0f },
        { 0.0f, 3.0f / 72.0f },
        { 0.0f, 4.0f / 72.0f },
        { 0.0f, 5.0f / 72.0f },
        { 0.0f, 6.0f / 72.0f },
    }));
    shader_fbo_set_vec2_array(&pfbo->b_shader[8], "direction", 6, ((vec2[]) {
        { 0.0f, 1.0f / 36.0f },
        { 0.0f, 2.0f / 36.0f },
        { 0.0f, 3.0f / 36.0f },
        { 0.0f, 4.0f / 36.0f },
        { 0.0f, 5.0f / 36.0f },
        { 0.0f, 6.0f / 36.0f },
    }));
    shader_fbo_set_vec2_array(&pfbo->b_shader[9], "direction", 6, ((vec2[]) {
        { 0.0f, 1.0f / 18.0f },
        { 0.0f, 2.0f / 18.0f },
        { 0.0f, 3.0f / 18.0f },
        { 0.0f, 4.0f / 18.0f },
        { 0.0f, 5.0f / 18.0f },
        { 0.0f, 6.0f / 18.0f },
    }));
    shader_fbo_set_vec4(&pfbo->b_shader[10], "res", ((vec4) { 256.0f, 144.0f, 1.0f / 256.0f, 1.0f / 144.0f }));

    glGenFramebuffers(11, pfbo->fbo);
    glGenTextures(15, pfbo->tcb);

    bind_framebuffer(pfbo->fbo[0]);
    fbo_helper_gen_texture_image(pfbo->tcb[0], 256, 144, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image(pfbo->tcb[1], 256, 144, GL_R16F, GL_RED, GL_HALF_FLOAT, 1);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[1]);
    fbo_helper_gen_texture_image(pfbo->tcb[2], 128, 72, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image(pfbo->tcb[3], 128, 72, GL_R16F, GL_RED, GL_HALF_FLOAT, 1);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[2]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tcb[4], 128, 72, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[3]);
    fbo_helper_gen_texture_image(pfbo->tcb[5], 64, 36, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image(pfbo->tcb[6], 64, 36, GL_R16F, GL_RED, GL_HALF_FLOAT, 1);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[4]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tcb[7], 64, 36, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[5]);
    fbo_helper_gen_texture_image(pfbo->tcb[8], 32, 18, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_gen_texture_image(pfbo->tcb[9], 32, 18, GL_R16F, GL_RED, GL_HALF_FLOAT, 1);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[6]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tcb[10], 32, 18, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[7]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tcb[11], 8, 8, GL_R32F, GL_RED, GL_FLOAT, 0);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[8]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tcb[12], 256, 144, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[9]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tcb[13], 32, 1, GL_R32F, GL_RED, GL_FLOAT, 0); 
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]){ GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();

    bind_framebuffer(pfbo->fbo[10]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    fbo_helper_gen_texture_image(pfbo->tcb[14], 1, 1, GL_R32F, GL_RED, GL_FLOAT, 0);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
    fbo_helper_get_error_code();
    bind_tex2d(0);

    fbo_pp_bind_fbo(pfbo, res);
}

void fbo_pp_resize(fbo_pp* pfbo, vec2i* res) {
    if (!pfbo)
        return;

    fbo_pp_bind_fbo(pfbo, res);
}

void fbo_pp_tone_map_set(fbo_pp* pfbo, vec2* tone_map_data, int32_t count) {
    texture_data data;
    memset(&data, 0, sizeof(texture_data));
    data.type = TEXTURE_1D;
    data.width = count;
    data.pixel_format = GL_RG;
    data.pixel_internal_format = GL_RG16F;
    data.pixel_type = GL_FLOAT;
    data.mag_filter = GL_LINEAR;
    data.min_filter = GL_LINEAR;
    data.wrap_mode_s = GL_CLAMP_TO_EDGE;

    data.data = tone_map_data;
    texture_load(&pfbo->tone_map, &data);
}

void fbo_pp_draw(fbo_pp* pfbo, int32_t in_tcb,
    void* out_fbo, void(*out_fbo_func_begin)(void*), void(*out_fbo_func_end)(void*)) {
    bind_vertex_array(pfbo->vao);
    int i = 0;
    if (pfbo->count_down > 0) {
        shader_fbo_use(&pfbo->b_shader[0]);
        for (; i < pfbo->count_down; i++) {
            glViewport(0, 0, pfbo->res_down[i].x, pfbo->res_down[i].y); // 0
            bind_framebuffer(pfbo->fbo_down[i]);
            bind_index_tex2d(0, i == 0 ? in_tcb : pfbo->tcb_down[i - 1]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        i--;
    }

    glViewport(0, 0, 256, 144); // 1
    bind_framebuffer(pfbo->fbo[0]);
    glDrawBuffers(2, fbo_pp_d_attachments);
    shader_fbo_use(&pfbo->b_shader[1]);
    bind_index_tex2d(0, pfbo->count_down > 0 ? pfbo->tcb_down[i] : in_tcb);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 128, 72); // 2
    bind_framebuffer(pfbo->fbo[1]);
    glDrawBuffers(2, fbo_pp_d_attachments);
    shader_fbo_use(&pfbo->b_shader[2]);
    bind_index_tex2d(0, pfbo->tcb[0]);
    bind_index_tex2d(1, pfbo->tcb[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 64, 36); // 3
    bind_framebuffer(pfbo->fbo[3]);
    glDrawBuffers(2, fbo_pp_d_attachments);
    shader_fbo_use(&pfbo->b_shader[2]);
    bind_index_tex2d(0, pfbo->tcb[2]);
    bind_index_tex2d(1, pfbo->tcb[3]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 32, 18); // 4
    bind_framebuffer(pfbo->fbo[5]);
    glDrawBuffers(2, fbo_pp_d_attachments);
    shader_fbo_use(&pfbo->b_shader[2]);
    bind_index_tex2d(0, pfbo->tcb[5]);
    bind_index_tex2d(1, pfbo->tcb[6]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 8, 8); // 5
    bind_framebuffer(pfbo->fbo[7]);
    shader_fbo_use(&pfbo->b_shader[3]);
    bind_index_tex2d(0, pfbo->tcb[9]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 128, 72); // 6
    bind_framebuffer(pfbo->fbo[2]);
    shader_fbo_use(&pfbo->b_shader[4]);
    bind_index_tex2d(0, pfbo->tcb[2]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 64, 36); // 7
    bind_framebuffer(pfbo->fbo[4]);
    shader_fbo_use(&pfbo->b_shader[5]);
    bind_index_tex2d(0, pfbo->tcb[5]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 32, 18); // 8
    bind_framebuffer(pfbo->fbo[6]);
    shader_fbo_use(&pfbo->b_shader[6]);
    bind_index_tex2d(0, pfbo->tcb[8]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 128, 72); // 9
    bind_framebuffer(pfbo->fbo[1]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    shader_fbo_use(&pfbo->b_shader[7]);
    bind_index_tex2d(0, pfbo->tcb[4]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 64, 36); // 10
    bind_framebuffer(pfbo->fbo[3]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    shader_fbo_use(&pfbo->b_shader[8]);
    bind_index_tex2d(0, pfbo->tcb[7]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 32, 18); // 11
    bind_framebuffer(pfbo->fbo[5]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    shader_fbo_use(&pfbo->b_shader[9]);
    bind_index_tex2d(0, pfbo->tcb[10]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 256, 144); // 12
    bind_framebuffer(pfbo->fbo[8]);
    shader_fbo_use(&pfbo->b_shader[10]);
    bind_index_tex2d(0, pfbo->tcb[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 256, 144); // 13
    bind_framebuffer(pfbo->fbo[0]);
    glDrawBuffers(1, fbo_pp_s_attachments);
    shader_fbo_use(&pfbo->b_shader[11]);
    bind_index_tex2d(0, pfbo->tcb[12]);
    bind_index_tex2d(1, pfbo->tcb[2]);
    bind_index_tex2d(2, pfbo->tcb[5]);
    bind_index_tex2d(3, pfbo->tcb[8]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(pfbo->dst_pixel, 0, 1, 1); // 14
    bind_framebuffer(pfbo->fbo[9]);
    shader_fbo_use(&pfbo->b_shader[12]);
    bind_index_tex2d(0, pfbo->tcb[11]);
    bind_index_tex2d(1, pfbo->tcb[6]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, 1, 1); // 15
    bind_framebuffer(pfbo->fbo[10]);
    shader_fbo_use(&pfbo->b_shader[13]);
    bind_index_tex2d(0, pfbo->tcb[13]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    int32_t tone_map_shader_flags[7];
    memset(tone_map_shader_flags, 0, sizeof(int32_t) * 7);
    tone_map_shader_flags[0] = uniform_value[U_TONE_MAP];
    tone_map_shader_flags[1] = uniform_value[U_FLARE];
    tone_map_shader_flags[2] = uniform_value[U_SCENE_FADE];
    tone_map_shader_flags[3] = uniform_value[U_AET_BACK];
    tone_map_shader_flags[4] = uniform_value[U1C];
    tone_map_shader_flags[5] = uniform_value[U_NPR];
    tone_map_shader_flags[6] = uniform_value[U25];
    bind_uniform_buffer_base(tone_map_ubo_binding, pfbo->tone_map_ubo);
    glViewport(0, 0, pfbo->res.x, pfbo->res.y); // 16
    out_fbo_func_begin(out_fbo);
    shader_fbo_use(pfbo->t_shader);
    shader_fbo_set_int_array(pfbo->t_shader, "mode", 7, tone_map_shader_flags);
    shader_fbo_set_vec2(pfbo->t_shader, "scale",
        ((vec2){ 1.0f, (float_t)pfbo->res.y / (float_t)pfbo->res.x }));
    shader_fbo_set_mat4_array(pfbo->t_shader, "uv_mat", 2, GL_FALSE,
        ((mat4[]) { mat4_identity, mat4_identity }));
    bind_index_tex2d(0, in_tcb);
    bind_index_tex2d(1, pfbo->tcb[0]);
    bind_index_tex1d(2, pfbo->tone_map.id);
    bind_index_tex2d(3, pfbo->tcb[14]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    out_fbo_func_end(out_fbo);

    pfbo->dst_pixel++;
    if (pfbo->dst_pixel > 31)
        pfbo->dst_pixel = 0;
}

void fbo_pp_dispose(fbo_pp* pfbo) {
    if (!pfbo)
        return;

    glDeleteFramebuffers(pfbo->full_count_down, pfbo->fbo_down);
    glDeleteTextures(pfbo->full_count_down, pfbo->tcb_down);
    texture_free(&pfbo->tone_map);
    glDeleteFramebuffers(11, pfbo->fbo);
    glDeleteTextures(15, pfbo->tcb);
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
    int32_t* tcb_down = pfbo->tcb_down;
    vec2i* res_down = pfbo->res_down;

    if (!fbo_down)
        fbo_down = force_malloc_s(int32_t, i);
    else if (pfbo->count_down < i) {
        void* temp = force_malloc_s(int32_t, i);
        memcpy(temp, fbo_down, sizeof(int32_t) * pfbo->count_down);
        free(fbo_down);
        fbo_down = temp;
    }

    if (!tcb_down)
        tcb_down = force_malloc_s(int32_t, i);
    else if (pfbo->count_down < i) {
        void* temp = force_malloc_s(int32_t, i);
        memcpy(temp, tcb_down, sizeof(int32_t) * pfbo->count_down);
        free(tcb_down);
        tcb_down = temp;
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
        if (tcb_down[i] == 0)
            glGenTextures(1, &tcb_down[i]);
    }

    i = 0;
    r = pfbo->res;
    while (r.x > 512 && r.y > 288) {
        res_down[i].x = r.x /= 2;
        res_down[i].y = r.y /= 2;
        i++;
    }

    for (i = 0; i < pfbo->count_down; i++) {
        bind_framebuffer(fbo_down[i]);
        glDrawBuffers(1, fbo_pp_s_attachments);
        fbo_helper_gen_texture_image(tcb_down[i], res_down[i].x, res_down[i].y,
            GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 0);
        fbo_helper_get_error_code();
    }

    bind_framebuffer(0);

    pfbo->fbo_down = fbo_down;
    pfbo->tcb_down = tcb_down;
    pfbo->res_down = res_down;
}
