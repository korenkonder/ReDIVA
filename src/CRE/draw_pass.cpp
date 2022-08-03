/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_pass.hpp"
#include "Glitter/glitter.hpp"
#include "light_param/fog.hpp"
#include "light_param/light.hpp"
#include "rob/rob.hpp"
#include "clear_color.hpp"
#include "gl_state.hpp"
#include "post_process.hpp"
#include "render_texture.hpp"
#include "shader_ft.hpp"
#include "shader_glsl.hpp"
#include "static_var.hpp"
#include "texture.hpp"

struct draw_preprocess {
    int32_t field_0;
    void(*draw_func)(void*);
    void* data;
};

struct texture_param {
    GLint width;
    GLint height;
};

static void blur_filter_apply(GLuint tex_0, GLuint tex_1, blur_filter_mode filter);
static void draw_pass_begin(draw_pass* a1);
static void draw_pass_shadow(render_context* rctx, draw_pass* a1);
static void draw_pass_shadow_filter(render_texture* a1, render_texture* a2,
    render_texture* a3, float_t sigma, float_t far_texel_offset, bool enable_lit_proj);
static bool draw_pass_shadow_litproj(render_context* rctx, light_proj* litproj);
static void draw_pass_sss(render_context* rctx, draw_pass* a1);
static void draw_pass_sss_contour(render_context* rctx, post_process* pp);
static void draw_pass_sss_filter(render_context* rctx, sss_data* a1);
static void draw_pass_reflect(render_context* rctx, draw_pass* a1);
static void draw_pass_refract(render_context* rctx, draw_pass* a1);
static void draw_pass_pre_process(render_context* rctx, draw_pass* a1);
static void draw_pass_3d(render_context* rctx, draw_pass* a1);
static int32_t draw_pass_3d_get_translucent_count(render_context* rctx);
static void draw_pass_3d_shadow_reset(render_context* rctx);
static void draw_pass_3d_shadow_set(shadow* shad, render_context* rctx);
static void draw_pass_show_vector(render_context* rctx, draw_pass* a1);
static void draw_pass_post_process(render_context* rctx, draw_pass* a1);
static void draw_pass_sprite(render_context* rctx, draw_pass* a1);
static void draw_pass_end(draw_pass* pass, draw_pass_type type);
static void draw_pass_3d_grid(render_context* rctx);
static void draw_pass_set_camera(camera* cam);
static void texture_params_get(GLuint tex_0, texture_param* tex_0_param,
    GLuint tex_1, texture_param* tex_1_param, GLuint tex_2, texture_param* tex_2_param);
static void texture_params_restore(texture_param* tex_0_param,
    texture_param* tex_1_param, texture_param* tex_2_param);

extern bool draw_grid_3d;
extern shader_glsl* grid_shader;
extern GLuint grid_vbo;
extern size_t grid_vertex_count;
extern light_param_data_storage* light_param_data_storage_data;

void draw_pass_main(render_context* rctx) {
    static const int32_t ibl_texture_index[] = {
        9, 10, 11, 12, 13
    };

    camera* cam = rctx->camera;

    for (int32_t i = 0; i < 5; i++)
        gl_state_active_bind_texture_cube_map(ibl_texture_index[i],
            light_param_data_storage_data->textures[i]);

    shaders_ft.env_frag_set(0,
        1.0f / (float_t)rctx->post_process.render_width,
        1.0f / (float_t)rctx->post_process.render_height,
        (float_t)rctx->post_process.render_width,
        (float_t)rctx->post_process.render_height);

    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    gl_state_enable_depth_test();
    gl_state_set_depth_func(GL_LEQUAL);
    gl_state_set_depth_mask(GL_TRUE);
    for (int32_t i = DRAW_PASS_SHADOW; i < DRAW_PASS_MAX; i++) {
        rctx->draw_pass.cpu_time[i] = 0.0;
        rctx->draw_pass.gpu_time[i] = 0.0;
        if (!rctx->draw_pass.enable[i]) {
            glGetError();
            continue;
        }

        draw_pass_begin(&rctx->draw_pass);
        switch (i) {
        case DRAW_PASS_SHADOW:
            draw_pass_shadow(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_SS_SSS:
            draw_pass_sss(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_REFLECT:
            draw_pass_reflect(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_REFRACT:
            draw_pass_refract(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_PRE_PROCESS:
            draw_pass_pre_process(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_CLEAR: {
            rctx->post_process.rend_texture.bind();
            vec4 _clear_color;
            vec4u8_to_vec4(clear_color, _clear_color);
            vec4_mult_scalar(_clear_color, (float_t)(1.0 / 255.0), _clear_color);
            glClearBufferfv(GL_COLOR, 0, (GLfloat*)&_clear_color);
            //sub_140501470(rctx, &rctx->draw_pass);
        } break;
        case DRAW_PASS_PRE_SPRITE:
            //sub_140501600(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_3D:
            draw_pass_3d(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_SHOW_VECTOR:
            draw_pass_show_vector(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_POST_PROCESS:
            draw_pass_post_process(rctx, &rctx->draw_pass);
            break;
        case DRAW_PASS_SPRITE:
            draw_pass_sprite(rctx, &rctx->draw_pass);
            break;
        }
        draw_pass_end(&rctx->draw_pass, (draw_pass_type)i);
        glGetError();
    }
    rctx->object_data.check_vertex_arrays();
    gl_state_bind_vertex_array(0);
}

inline static void draw_pass_begin(draw_pass* a1) {
    if (a1->wait_for_gpu)
        glFinish();
    a1->time.get_timestamp();
}

static void draw_pass_shadow(render_context* rctx, draw_pass* a1) {
    rctx->camera->update_data();
    rctx->view_mat = rctx->camera->view;
    if (rctx->litproj->set(rctx)) {
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_OPAQUE, 0, 0, true, -1);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSPARENT, 0, 0, true, -1);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSLUCENT, 0, 0, true, -1);

        rctx->draw_state.shader_index = -1;
        uniform_value[U0A] = 0;
        draw_pass_shadow_filter(&rctx->litproj->shadow_texture[0],
            &rctx->litproj->shadow_texture[1], 0, 1.5f, 0.01f, true);

        if (draw_pass_shadow_litproj(rctx, rctx->litproj)) {
            draw_pass_set_camera(rctx->camera);
            for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                rctx->light_set[i].data_set(rctx->face, (light_set_id)i);
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_OPAQUE, 0, 0, true, -1);
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSPARENT, 0, 0, true, -1);
            draw_task_sort(rctx, DRAW_OBJECT_TRANSLUCENT, 1);
            gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            gl_state_enable_blend();
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSLUCENT, 0, 0, true, -1);
            gl_state_disable_blend();
            rctx->draw_state.shader_index = -1;
            gl_state_bind_framebuffer(0);
        }
    }

    bool v3 = false;
    int32_t v10[2];
    for (int32_t i = 0; i < 2; i++) {
        v10[i] = draw_task_get_count(rctx, (draw_object_type)(DRAW_OBJECT_SHADOW_CHARA + i));
        if (v10[i])
            v3 = true;
    }

    shadow* shad = a1->shadow_ptr;
    if (a1->shadow && v3) {
        int32_t v11[2];
        v11[0] = shad->field_200[0];
        v11[1] = shad->field_200[1];
        shad->field_158[0] = &shad->field_8[0];
        shad->field_158[1] = &shad->field_8[1];
        shad->field_158[2] = &shad->field_8[2];

        int32_t v8 = DRAW_OBJECT_SHADOW_OBJECT_CHARA;
        for (int32_t i = 0, j = 0; i < 2; i++, v8++) {
            if (!v10[i])
                continue;

            int32_t index = v11[i];

            texture* tex = shad->field_158[0]->color_texture;
            shad->field_158[0]->bind();
            glViewport(0, 0, tex->width, tex->height);
            glScissor(0, 0, tex->width, tex->height);
            gl_state_enable_depth_test();
            gl_state_enable_scissor_test();
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClearDepthf(1.0f);
            if (!j)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            else if (shad->field_2F5)
                glClear(GL_COLOR_BUFFER_BIT);
            glScissor(1, 1, tex->width - 2, tex->height - 2);

            float_t v15 = 0.0f;
            float_t v16 = shad->view_region * shad->range;
            vec3* interest;
            vec3* view_point;
            if (shad->field_2F5) {
                v15 = -0.5f;
                if (index)
                    v15 = 0.5f;

                interest = &shad->interest[index];
                view_point = &shad->view_point[index];
            }
            else {
                interest = &shad->interest_shared;
                view_point = &shad->view_point_shared;
            }

            mat4 temp;
            mat4_translate(v15, 0.0f, 0.0f, &temp);

            mat4 proj;
            mat4_ortho(-v16, v16, -v16, v16, shad->z_near, shad->z_far, &proj);
            mat4_mult(&proj, &temp, &proj);
            shaders_ft.state_matrix_set_projection(proj, false);

            mat4_look_at(view_point, interest, &rctx->view_mat);

            rctx->draw_state.shader_index = SHADER_FT_SIL;
            uniform_value[U0A] = 0;

            draw_task_draw_objects_by_type(rctx, (draw_object_type)(DRAW_OBJECT_SHADOW_CHARA + v11[i]), 0, 0, 1, -1);
            if (draw_task_get_count(rctx, (draw_object_type)(DRAW_OBJECT_SHADOW_OBJECT_CHARA + v11[i])) > 0) {
                glColorMask(a1->field_2F8 != 0 ? GL_TRUE : GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                draw_task_draw_objects_by_type(rctx, (draw_object_type)v8, 0, 0, true, -1);
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }
            gl_state_disable_depth_test();
            gl_state_disable_scissor_test();

            rctx->draw_state.shader_index = -1;
            if (j == shad->field_2EC - 1) {
                draw_pass_shadow_filter(&shad->field_8[3], &shad->field_8[4], shad->field_158[0],
                    shad->field_2DC, shad->field_2E0 / (shad->field_208 * 2.0f), false);

                GLuint v5 = shad->field_8[5].color_texture->tex;
                GLuint v6 = shad->field_8[6].color_texture->tex;
                GLuint v7 = shad->field_8[3].color_texture->tex;
                if (v5 && v6 && v7) {
                    texture_param tex_params[3];
                    texture_params_get(v5, &tex_params[0], v7, &tex_params[1], v6, &tex_params[2]);
                    shad->field_8[6].bind();
                    uniform_value[U_ESM_FILTER] = 0;
                    shaders_ft.set(SHADER_FT_ESMFILT);
                    shaders_ft.local_frag_set(0, 1.0f / (float_t)tex_params[1].width,
                        1.0f / (float_t)tex_params[1].height, 0.0f, 0.0f);
                    gl_state_active_bind_texture_2d(0, v7);
                    shaders_ft.state_matrix_set_mvp(mat4_identity);
                    render_texture::draw_custom(&shaders_ft);

                    shad->field_8[5].bind();
                    uniform_value[U_ESM_FILTER] = 1;
                    shaders_ft.set(SHADER_FT_ESMFILT);
                    shaders_ft.local_frag_set(0, 0.75f / (float_t)tex_params[2].width,
                        0.75f / (float_t)tex_params[2].height, 0.0f, 0.0f);
                    gl_state_active_bind_texture_2d(0, v6);
                    shaders_ft.state_matrix_set_mvp(mat4_identity);
                    render_texture::draw_custom(&shaders_ft);
                    texture_params_restore(&tex_params[0], &tex_params[1], &tex_params[2]);
                }
            }

            render_texture* rend_tex = shad->field_158[1 + index];
            rend_tex->bind();

            GLuint v11 = shad->field_158[1 + index]->color_texture->tex;
            GLuint v12 = shad->field_158[0]->color_texture->tex;
            if (v11 && v12) {
                texture_param tex_params[3];
                texture_params_get(v11, &tex_params[0], v12, &tex_params[1], 0, 0);
                uniform_value[U_IMAGE_FILTER] = 5;
                shaders_ft.set(SHADER_FT_IMGFILT);
                shaders_ft.local_frag_set(0, vec4_identity);
                shaders_ft.state_matrix_set_texture(0, mat4_identity);
                shaders_ft.state_matrix_set_texture(1, mat4_identity);
                shaders_ft.state_matrix_set_texture(2, mat4_identity);
                shaders_ft.state_matrix_set_texture(3, mat4_identity);
                shaders_ft.state_matrix_set_texture(4, mat4_identity);
                shaders_ft.state_matrix_set_texture(5, mat4_identity);
                shaders_ft.state_matrix_set_texture(6, mat4_identity);
                shaders_ft.state_matrix_set_texture(7, mat4_identity);
                gl_state_active_bind_texture_2d(0, v12);
                shaders_ft.state_matrix_set_mvp(mat4_identity);
                render_texture::draw_custom(&shaders_ft);
                texture_params_restore(&tex_params[0], &tex_params[1], 0);
            }

            rend_tex->bind();
            if (shad->blur_filter_enable[index])
                for (int32_t i = 0; i < shad->near_blur; i++)
                    blur_filter_apply(rend_tex->color_texture->tex,
                        rend_tex->color_texture->tex, shad->blur_filter);
            else {
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            gl_state_bind_framebuffer(0);
            gl_state_bind_texture_2d(rend_tex->color_texture->tex);
            glGenerateMipmap(GL_TEXTURE_2D);
            gl_state_bind_texture_2d(0);
            j++;
        }
    }
    else {
        shad->field_158[0] = &shad->field_8[0];
        shad->field_158[1] = &shad->field_8[1];
        shad->field_158[2] = &shad->field_8[2];

        for (int32_t i = 1; i < 3; i++)
            for (int32_t j = 0; j < 4; j++) {
                shad->field_158[i]->bind(j);
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
            }
    }
    gl_state_bind_framebuffer(0);
}

static void draw_pass_shadow_filter(render_texture* a1, render_texture* a2,
    render_texture* a3, float_t sigma, float_t far_texel_offset, bool enable_lit_proj) {
    GLuint v7 = a1->color_texture->tex;
    GLuint v9 = a2->color_texture->tex;
    GLuint v11 = v7;
    if (a3)
        v11 = a3->depth_texture->tex;

    if (!v7 || !v9 || !v11)
        return;

    texture_param tex_params[2];
    texture_params_get(v7, &tex_params[0], v9, &tex_params[1], 0, 0);
    if (tex_params[0].width != tex_params[1].width
        || tex_params[0].height != tex_params[1].height)
        return;

    vec4 v15[2];
    double_t v6 = 1.0 / (sqrt(M_PI * 2.0) * sigma);
    double_t v8 = -1.0 / (2.0 * sigma * sigma);
    for (int32_t i = 0; i < 8; i++)
        ((float_t*)v15)[i] = (float_t)(exp((double_t)((ssize_t)i * i) * v8) * v6);

    a2->bind();
    uniform_value[U_LIGHT_PROJ] = enable_lit_proj ? 1 : 0;
    shaders_ft.set(SHADER_FT_ESMGAUSS);
    shaders_ft.local_frag_set(0, 1.0f / (float_t)tex_params[0].width, 0.0f, 0.0f, 0.0f);
    shaders_ft.local_frag_set(1, v15[0]);
    shaders_ft.local_frag_set(2, v15[1]);
    shaders_ft.local_frag_set(3, far_texel_offset, far_texel_offset, 0.0f, 0.0f);

    gl_state_active_bind_texture_2d(0, v11);
    if (a3) {
        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    }
    else {
        GLint swizzle[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    }
    shaders_ft.state_matrix_set_mvp(mat4_identity);
    render_texture::draw_custom(&shaders_ft);

    a1->bind();
    shaders_ft.local_frag_set(0, 0.0f, 1.0f / (float_t)tex_params[0].height, 0.0f, 0.0f);
    shaders_ft.local_frag_set(1, v15[0]);
    shaders_ft.local_frag_set(2, v15[1]);
    shaders_ft.local_frag_set(3, far_texel_offset, far_texel_offset, 0.0f, 0.0f);
    gl_state_active_bind_texture_2d(0, v9);
    shaders_ft.state_matrix_set_mvp(mat4_identity);
    render_texture::draw_custom(&shaders_ft);
    shader::unbind();
    texture_params_restore(&tex_params[0], &tex_params[1], 0);
}

static bool draw_pass_shadow_litproj(render_context* rctx, light_proj* litproj) {
    if (!litproj)
        return false;

    texture* tex = texture_storage_get_texture(litproj->texture_id);
    if (!tex)
        return false;

    litproj->draw_texture.bind();

    glViewport(0, 0, litproj->draw_texture.color_texture->width,
        litproj->draw_texture.color_texture->height);
    gl_state_enable_depth_test();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!light_proj::set_mat(rctx, true)) {
        gl_state_bind_framebuffer(0);
        return false;
    }

    rctx->draw_state.shader_index = SHADER_FT_LITPROJ;
    gl_state_active_bind_texture_2d(17, tex->tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&vec4_null);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    gl_state_active_bind_texture_2d(18, litproj->shadow_texture[0].color_texture->tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&vec4_null);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    gl_state_active_texture(0);
    return true;
}

static void draw_pass_sss(render_context* rctx, draw_pass* a1) {
    sss_data* sss = &a1->sss_data;
    if (!sss->init)
        return;

    if (!sss->enable) {
        shaders_ft.env_frag_set(25, 0.0f, 0.0f, 0.0f, 0.0f);
        return;
    }

    rctx->camera->update_data();
    rctx->view_mat = rctx->camera->view;
    post_process* pp = &rctx->post_process;
    //if (pp->render_width > 1280.0)
    //    sss->npr_contour = false;

    if (sss->npr_contour) {
        pp->rend_texture.bind();
        glViewport(0, 0, pp->render_width, pp->render_height);
    }
    else {
        sss->textures[0].bind();
        glViewport(0, 0, 640, 360);
    }

    glClearColor(sss->param.x, sss->param.y, sss->param.z, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_pass_set_camera(rctx->camera);
    for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
        rctx->light_set[i].data_set(rctx->face, (light_set_id)i);

    if (a1->shadow)
        draw_pass_3d_shadow_set(a1->shadow_ptr, rctx);
    else
        draw_pass_3d_shadow_reset(rctx);

    rctx->draw_state.shader_index = SHADER_FT_SSS_SKIN;
    gl_state_enable_depth_test();
    gl_state_set_depth_func(GL_LEQUAL);
    gl_state_set_depth_mask(GL_TRUE);
    draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_SSS, 0, 0, 1, -1);
    gl_state_disable_depth_test();
    rctx->draw_state.shader_index = -1;
    draw_pass_3d_shadow_reset(rctx);
    uniform_value[U_NPR] = 0;

    if (a1->npr_param == 1) {
        if (sss->enable && sss->npr_contour) {
            uniform_value[U_NPR] = 1;
            draw_pass_sss_contour(rctx, pp);
        }
        else if (a1->npr) {
            pp->rend_texture.bind();
            glViewport(0, 0, pp->render_width, pp->render_height);
            glClear(GL_DEPTH_BUFFER_BIT);
            rctx->draw_state.shader_index = SHADER_FT_SSS_SKIN;
            gl_state_enable_depth_test();
            gl_state_set_depth_mask(GL_TRUE);
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_SSS, 1, 0, 1, -1);
            gl_state_disable_depth_test();
            rctx->draw_state.shader_index = -1;
            gl_state_bind_framebuffer(0);
            uniform_value[U_NPR] = 1;
            draw_pass_sss_contour(rctx, pp);
        }
    }
    draw_pass_sss_filter(rctx, sss);
    gl_state_bind_framebuffer(0);
}

static void draw_pass_sss_contour(render_context* rctx, post_process* pp) {
    pp->sss_contour_texture.bind();
    shaders_ft.state_matrix_set_modelview(0, mat4_identity, false);
    shaders_ft.state_matrix_set_projection(mat4_identity, true);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);
    glViewport(0, 0, pp->render_width, pp->render_height);
    gl_state_active_bind_texture_2d(0, pp->rend_texture.color_texture->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl_state_active_bind_texture_2d(1, pp->rend_texture.depth_texture->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl_state_active_texture(0);
    shaders_ft.set(SHADER_FT_CONTOUR);

    camera* cam = rctx->camera;
    float_t v3 = 1.0f / tanf((float_t)(cam->fov_rad * 0.5));

    vec3 direction;
    vec3_sub(cam->interest, cam->view_point, direction);
    vec3_normalize(direction, direction);
    float_t length;
    vec3_length(direction, length);
    float_t v7 = direction.y;
    if (length != 0.0)
        v7 /= length;

    float_t v9 = fabsf(v7) - 0.1f;
    if (v9 < 0.0f)
        v9 = 0.0f;

    shaders_ft.local_frag_set(4, v9 * 0.004f + 0.0027f, 0.003f, v3 * 0.35f, 0.0008f);
    render_texture::draw_params(&shaders_ft, pp->render_width, pp->render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

static void draw_pass_sss_filter_calc_coef(double_t a1, size_t a2, double_t a3, size_t a4,
    double_t* a5, double_t* a6, double_t* a7, double_t* a8) {
    if (a2 > 8)
        return;

    vec4 params[64];
    memset(params, 0, sizeof(vec4) * 64);

    for (size_t i = a4; i; i--) {
        float_t* v20 = (float_t*)params;
        double_t v56[3];
        v56[0] = *a6;
        v56[1] = *a7;
        v56[2] = *a8;

        for (size_t j = 0; j < 3; j++) {
            double_t v22 = 0.0;
            double_t v23 = 0.0;
            double_t v54[8];
            double_t v24 = 1.0 / (a3 * v56[j]);
            v24 *= v24;
            for (size_t k = 0; k < a2; k++) {
                double_t v25 = exp(-0.5 * (v23 * v23) * v24);
                v54[k] = v25;
                v23 += a1;
                v22 += v25;
            }

            double_t v27 = 1.0 / v22;
            for (size_t k = 0; k < a2; k++)
                v54[k] *= v27;

            float_t* v35 = v20;
            for (size_t k = 0; k < a2; k++) {
                double_t v37 = v54[k] * *a5;
                for (size_t v36 = 0; v36 < a2; v36++) {
                    *v35 += (float_t)(v37 * v54[v36]);
                    v35 += 4;
                }
            }
            v20++;
        }
        a5++;
        a6++;
        a7++;
        a8++;
    }
    shaders_ft.local_frag_set(4, a2 * a2, params);
}

static void draw_pass_sss_filter(render_context* rctx, sss_data* a1) {
    const int32_t sss_count = 6;
    vec3 interest = rctx->camera->interest;
    vec3 view_point = rctx->camera->view_point;

    vec3 chara_position[2];
    chara_position[0] = vec3_null;
    chara_position[1] = vec3_null;
    float_t chara_distance[2];
    for (int32_t i = 0; i < 2; i++) {
        chara_position[i] = interest;
        chara_distance[i] = 999999.0f;
        rob_chara_bone_data* v16 = rob_chara_array[i].bone_data;
        if (rob_chara_pv_data_array[i].type != ROB_CHARA_TYPE_NONE && rob_chara_array[i].data.field_0 & 1) {
            mat4* mat = rob_chara_bone_data_get_mats_mat(v16, MOTION_BONE_N_HARA_CP);
            if (mat) {
                mat4_get_translation(mat, &chara_position[i]);
                vec3_distance(view_point, chara_position[i], chara_distance[i]);
            }
        }
    }

    vec3 closest_chara_position;
    if (chara_distance[0] > chara_distance[1]) {
        closest_chara_position = chara_position[1];
        chara_position[0] = chara_position[1];
    }
    else
        closest_chara_position = chara_position[0];

    float_t length;
    vec3_distance(interest, closest_chara_position, length);
    if (length > 1.25f)
        interest = chara_position[0];

    float_t v29;
    vec3_distance(view_point, interest, v29);
    if (v29 < 0.25f)
        v29 = 0.25f;
    float_t v31 = tanf((float_t)(rctx->camera->fov_rad * 0.5)) * 5.0f;
    if (v31 < 0.25f)
        v31 = 0.25f;
    float_t v32 = v31 * v29;
    float_t v33 = 0.6f;
    float_t v34 = 1.0f / clamp(v32, 0.25f, 100.0f);
    if (v34 < 0.145) {
        float_t v36 = v34 - 0.02f;
        if (v34 < 0.0f)
            v36 = 0.0f;
        v33 = (v36 * 8.0f) * 0.6f;
    }

    shaders_ft.env_frag_set(25, v33, 0.0f, 0.0f, 0.0f);

    shaders_ft.state_matrix_set_projection(mat4_identity, false);
    shaders_ft.state_matrix_set_modelview(0, mat4_identity, true);

    gl_state_active_texture(0);
    if (a1->npr_contour) {
        a1->textures->bind();
        glViewport(0, 0, 640, 360);
        post_process* pp = &rctx->post_process;
        gl_state_bind_texture_2d(pp->rend_texture.color_texture->tex);
        uniform_value[U_REDUCE] = 0;
        shaders_ft.set(SHADER_FT_REDUCE);

        shaders_ft.state_matrix_set_texture(0, mat4_identity);
        render_texture::draw_params(&shaders_ft, 640, 360, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
    }
    a1->textures[2].bind();
    glViewport(0, 0, 320, 180);
    gl_state_bind_texture_2d(a1->textures[0].color_texture->tex);
    uniform_value[U_SSS_FILTER] = 0;
    shaders_ft.set(SHADER_FT_SSS_FILT);
    render_texture::draw_params(&shaders_ft, 640, 360, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
    a1->textures[1].bind();
    uniform_value[U_SSS_FILTER] = 3;
    shaders_ft.set(SHADER_FT_SSS_FILT);
    shaders_ft.local_frag_set(1, 5.0f, 0.0f, 0.0f, 0.0f);

    double_t a5[3];
    double_t a6[3];
    double_t a7[3];
    double_t a8[3];
    a5[0] = 0.4;
    a5[1] = 0.3;
    a5[2] = 0.3;
    a6[0] = 1.0;
    a6[1] = 2.0;
    a6[2] = 5.0;
    a7[0] = 0.2;
    a7[1] = 0.4;
    a7[2] = 1.2;
    a8[0] = 0.3;
    a8[1] = 0.7;
    a8[2] = 2.0;
    draw_pass_sss_filter_calc_coef(1.0, sss_count, v34, 3, a5, a6, a7, a8);

    shaders_ft.local_frag_set(1, (float_t)(sss_count - 1), 0.0f, 1.0f, 1.0f);
    glViewport(0, 0, 320, 180);
    gl_state_bind_texture_2d(a1->textures[2].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 320, 180, 1.0f, 1.0f, 0.96f, 1.0f, 0.0f);
    gl_state_bind_texture_2d(0);
}

static void draw_pass_reflect(render_context* rctx, draw_pass* a1) {
    render_texture& reflect_texture = rctx->draw_pass.reflect_texture;
    reflect_texture.bind();
    if (draw_task_get_count(rctx, DRAW_OBJECT_REFLECT_OPAQUE)
        || draw_task_get_count(rctx, DRAW_OBJECT_REFLECT_TRANSPARENT)
        || draw_task_get_count(rctx, DRAW_OBJECT_REFLECT_TRANSLUCENT)
        || draw_task_get_count(rctx, DRAW_OBJECT_REFLECT_CHARA_OPAQUE)
        || draw_task_get_count(rctx, DRAW_OBJECT_REFLECT_CHARA_TRANSPARENT)
        || draw_task_get_count(rctx, DRAW_OBJECT_REFLECT_CHARA_TRANSLUCENT)) {
        texture* refl_tex = reflect_texture.color_texture;
        glViewport(0, 0, refl_tex->width, refl_tex->height);

        draw_pass_set_camera(rctx->camera);

        for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
            rctx->light_set[i].data_set(rctx->face, (light_set_id)i);
        for (int32_t i = FOG_DEPTH; i < FOG_BUMP; i++)
            rctx->fog[i].data_set((fog_id)i);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        rctx->draw_state.shader_index = SHADER_FT_S_REFL;
        bool clip_plane[4];
        light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
        set->lights[LIGHT_SPOT].get_clip_plane(clip_plane);
        uniform_value[U_REFLECT] = 2;
        uniform_value[U_CLIP_PLANE] = clip_plane[1];
        gl_state_enable_depth_test();
        gl_state_set_depth_func(GL_LEQUAL);
        gl_state_set_depth_mask(GL_TRUE);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_REFLECT_OPAQUE, 0, 0, a1->field_31D, -1);
        if (a1->reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP) {
            gl_state_enable_blend();
            gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            gl_state_set_blend_equation(GL_FUNC_ADD);
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_REFLECT_TRANSLUCENT, 0, 0, 1, -1);
            gl_state_disable_blend();
        }

        if (a1->reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP) {
            uniform_value[U_REFLECT] = a1->field_31E;
            uniform_value[U_CLIP_PLANE] = clip_plane[0];
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_REFLECT_CHARA_OPAQUE, 0, 0, 1, -1);
        }
        gl_state_disable_depth_test();
        uniform_value[U_REFLECT] = 0;
        rctx->draw_state.shader_index = -1;

        reflect_texture.bind();
        for (int32_t i = a1->reflect_blur_num; i > 0; i--)
            blur_filter_apply(reflect_texture.color_texture->tex,
                reflect_texture.color_texture->tex, a1->reflect_blur_filter);
    }
    else {
        vec4 clear_color;
        glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clear_color);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    }
}

static void draw_pass_refract(render_context* rctx, draw_pass* a1) {
    render_texture& refract_texture = rctx->draw_pass.refract_texture;
    refract_texture.bind();
    if (draw_task_get_count(rctx, DRAW_OBJECT_REFRACT_OPAQUE)
        || draw_task_get_count(rctx, DRAW_OBJECT_REFRACT_TRANSPARENT)
        || draw_task_get_count(rctx, DRAW_OBJECT_REFRACT_TRANSLUCENT)) {
        texture* refr_tex = refract_texture.color_texture;
        glViewport(0, 0, refr_tex->width, refr_tex->height);

        draw_pass_set_camera(rctx->camera);

        for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
            rctx->light_set[i].data_set(rctx->face, (light_set_id)i);
        for (int32_t i = FOG_DEPTH; i < FOG_BUMP; i++)
            rctx->fog[i].data_set((fog_id)i);

        vec4 clear_color;
        glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clear_color);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

        rctx->draw_state.shader_index = SHADER_FT_S_REFR;
        gl_state_enable_depth_test();
        gl_state_set_depth_func(GL_LEQUAL);
        gl_state_set_depth_mask(GL_TRUE);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_REFRACT_OPAQUE, 0, 0, 1, -1);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_REFRACT_TRANSPARENT, 0, 0, 1, -1);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_REFRACT_TRANSLUCENT, 0, 0, 1, -1);
        gl_state_disable_depth_test();
        rctx->draw_state.shader_index = -1;
    }
    else {
        vec4 clear_color;
        glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clear_color);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    }
}

static void draw_pass_pre_process(render_context* rctx, draw_pass* a1) {

}

static void draw_pass_3d(render_context* rctx, draw_pass* a1) {
    rctx->camera->update_data();
    rctx->view_mat = rctx->camera->view;
    rctx->post_process.rend_texture.bind();
    glViewport(0, 0, rctx->post_process.render_width, rctx->post_process.render_height);
    draw_pass_set_camera(rctx->camera);
    if (!a1->sss_data.enable || !a1->sss_data.npr_contour || draw_pass_3d_get_translucent_count(rctx))
        glClear(GL_DEPTH_BUFFER_BIT);

    gl_state_set_depth_func(GL_LEQUAL);

    for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
        rctx->light_set[i].data_set(rctx->face, (light_set_id)i);
    for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++)
        rctx->fog[i].data_set((fog_id)i);

    if (a1->shadow)
        draw_pass_3d_shadow_set(a1->shadow_ptr, rctx);
    else
        draw_pass_3d_shadow_reset(rctx);

    if (a1->enable[DRAW_PASS_REFLECT] && a1->reflect && a1->reflect_texture.color_texture) {
        gl_state_active_bind_texture_2d(15, a1->reflect_texture.color_texture->tex);
        uniform_value[U_WATER_REFLECT] = 1;
    }
    else
        uniform_value[U_WATER_REFLECT] = 0;

    if (a1->sss_texture)
        gl_state_active_bind_texture_2d(14, a1->sss_texture->tex);

    gl_state_active_bind_texture_2d(16, a1->sss_data.textures[1].color_texture->tex);
    gl_state_active_texture(0);

    if (a1->alpha_z_sort) {
        draw_task_sort(rctx, DRAW_OBJECT_TRANSLUCENT, 1);
        draw_task_sort(rctx, DRAW_OBJECT_TRANSLUCENT_NO_SHADOW, 2);
        draw_task_sort(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_1, 1);
        draw_task_sort(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2, 1);
        draw_task_sort(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_3, 1);
    }

    if (a1->opaque_z_sort)
        draw_task_sort(rctx, DRAW_OBJECT_OPAQUE, 0);

    if (a1->draw_pass_3d[DRAW_PASS_3D_OPAQUE]) {
        gl_state_enable_depth_test();
        gl_state_set_depth_mask(GL_TRUE);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_OPAQUE, 0, 0, 1, -1);
        gl_state_disable_depth_test();
    }

    Glitter::glt_particle_manager->DispScenes(Glitter::DISP_OPAQUE);

    if (draw_grid_3d)
        draw_pass_3d_grid(rctx);

    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);
    if (a1->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT])
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSPARENT, 0, 0, 1, -1);
    /*if (rctx->draw_pass.field_120)
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TYPE_7, 0, 0, 1, -1);*/

    rctx->post_process.exposure->get_exposure_chara_data(&rctx->post_process, rctx->camera);

    //post_process_draw_lens_flare(a1->post_process);
    //draw_stars(&stars);

    draw_task_draw_objects_by_type_translucent(rctx,
        a1->draw_pass_3d[DRAW_PASS_3D_OPAQUE],
        a1->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT],
        a1->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT],
        DRAW_OBJECT_OPAQUE_ALPHA_ORDER_3,
        DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_3,
        DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_3);

    //draw_snow_particle();
    //draw_rain();
    //draw_leaf_particle();
    //draw_particle();
    gl_state_disable_depth_test();

    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    Glitter::glt_particle_manager->DispScenes(Glitter::DISP_ALPHA);
    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    if (a1->npr_param == 1) {
        gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    gl_state_enable_depth_test();

    if (a1->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]) {
        gl_state_enable_blend();
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gl_state_set_depth_mask(GL_FALSE);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSLUCENT_NO_SHADOW, 0, 0, 1, -1);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSLUCENT, 0, 0, 1, -1);
        gl_state_disable_blend();
    }

    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE); // X
    Glitter::glt_particle_manager->DispScenes(Glitter::DISP_TYPE_2);
    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    gl_state_set_depth_mask(GL_TRUE);
    draw_task_draw_objects_by_type_translucent(rctx,
        a1->draw_pass_3d[DRAW_PASS_3D_OPAQUE],
        a1->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT],
        a1->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT],
        DRAW_OBJECT_OPAQUE_ALPHA_ORDER_2,
        DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2,
        DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2);
    gl_state_disable_depth_test();

    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    Glitter::glt_particle_manager->DispScenes(Glitter::DISP_NORMAL);
    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);
    draw_task_draw_objects_by_type_translucent(rctx,
        a1->draw_pass_3d[DRAW_PASS_3D_OPAQUE],
        a1->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT],
        a1->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT],
        DRAW_OBJECT_OPAQUE_ALPHA_ORDER_1,
        DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_1,
        DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_1);

    if (Glitter::glt_particle_manager->CheckHasLocalEffect()) { // X
        camera* cam = rctx->camera;
        double_t fov = cam->get_fov();
        cam->set_fov(32.2673416137695);
        draw_pass_set_camera(cam);

        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_OPAQUE_LOCAL, 0, 0, 1, -1);
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSPARENT_LOCAL, 0, 0, 1, -1);
        gl_state_enable_blend();
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSLUCENT_LOCAL, 0, 0, 1, -1);
        gl_state_disable_blend();

        draw_task_draw_objects_by_type_translucent(rctx,
            a1->draw_pass_3d[DRAW_PASS_3D_OPAQUE],
            a1->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT],
            a1->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT],
            DRAW_OBJECT_OPAQUE_ALPHA_ORDER_2_LOCAL,
            DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2_LOCAL,
            DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2_LOCAL);

        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        Glitter::glt_particle_manager->DispScenes(Glitter::DISP_LOCAL);
        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        cam->set_fov(fov);
        draw_pass_set_camera(cam);
    }

    if (a1->sss_texture)
        gl_state_active_bind_texture_2d(14, 0);

    gl_state_disable_depth_test();

    if (a1->enable[DRAW_PASS_REFLECT] && a1->reflect)
        gl_state_active_bind_texture_2d(15, 0);
    /*if (a1->shadow)
        draw_pass_3d_shadow_reset();*/
    shader::unbind();
}

static int32_t draw_pass_3d_get_translucent_count(render_context* rctx) {
    int32_t count = 0;
    count += draw_task_get_count(rctx, DRAW_OBJECT_OPAQUE_ALPHA_ORDER_1);
    count += draw_task_get_count(rctx, DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_1);
    count += draw_task_get_count(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_1);
    count += draw_task_get_count(rctx, DRAW_OBJECT_OPAQUE_ALPHA_ORDER_2);
    count += draw_task_get_count(rctx, DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2);
    count += draw_task_get_count(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2);
    count += draw_task_get_count(rctx, DRAW_OBJECT_OPAQUE_ALPHA_ORDER_3);
    count += draw_task_get_count(rctx, DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_3);
    count += draw_task_get_count(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_3);
    return count;
}

static void draw_pass_3d_shadow_reset(render_context* rctx) {
    gl_state_active_bind_texture_2d(6, 0);
    gl_state_active_bind_texture_2d(7, 0);
    shaders_ft.state_matrix_set_texture(6, mat4_identity);
    shaders_ft.state_matrix_set_texture(7, mat4_identity);
    rctx->draw_state.self_shadow = false;
    rctx->draw_state.light = false;
}

static void draw_pass_3d_shadow_set(shadow* shad, render_context* rctx) {
    if (shad->self_shadow && shad->field_2EC > 0) {
        gl_state_active_bind_texture_2d(19, shad->field_8[3].color_texture->tex);
        gl_state_active_bind_texture_2d(20, shad->field_8[5].color_texture->tex);
        gl_state_active_texture(0);
        shaders_ft.env_frag_set(23,
            ((shad->field_2D8 * shad->field_208) * 2.0f) * 1.442695f, 0.0f, 0.0f, 0.0f);
        rctx->draw_state.self_shadow = true;
    }
    else
        rctx->draw_state.self_shadow = false;

    if (shad->field_2EC > 0) {
        rctx->draw_state.light = true;
        uniform_value[U_LIGHT_1] = shad->field_2EC > 1 ? 1 : 0;

        float_t v7 = shad->view_region * shad->range;
        for (int32_t i = 0; i < 2; i++) {
            float_t v6 = 0.0f;
            vec3* interest;
            vec3* view_point;
            if (shad->field_2F5) {
                v6 = -0.5f;
                if (i)
                    v6 = 0.5f;

                interest = &shad->interest[i];
                view_point = &shad->view_point[i];
            }
            else {
                interest = &shad->interest_shared;
                view_point = &shad->view_point_shared;
            }

            mat4 temp;
            mat4_translate(0.5f, 0.5f, 0.5f, &temp);
            mat4_scale_rot(&temp, 0.5f, 0.5f, 0.5f, &temp);
            mat4_translate_mult(&temp, v6, 0.0f, 0.0f, &temp);

            mat4 proj;
            mat4_ortho(-v7, v7, -v7, v7, shad->z_near, shad->z_far, &proj);
            mat4_mult(&proj, &temp, &proj);

            mat4 view;
            mat4_look_at(view_point, interest, &view);
            mat4_mult(&view, &proj, &view);
            shaders_ft.state_matrix_set_texture(6ULL + i, view);
        }

        for (int32_t i = 0, j = 0; i < 2; i++) {
            if (!shad->field_2F0[i])
                continue;

            gl_state_active_bind_texture_2d(6 + j, shad->field_158[1 + i]->color_texture->tex);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
            j++;
        }
        gl_state_active_texture(0);

        light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
        light_data* data = &set->lights[LIGHT_SHADOW];

        vec4 ambient;
        if (data->get_type() == LIGHT_PARALLEL)
            data->get_ambient(ambient);
        else {
            ambient.x = shad->ambient;
            ambient.y = shad->ambient;
            ambient.z = shad->ambient;
        }

        shaders_ft.env_frag_set(12, ambient.x, ambient.y, ambient.z, 1.0f);
        shaders_ft.env_frag_set(13, 1.0f - ambient.x, 1.0f - ambient.y, 1.0f - ambient.z, 0.0f);
    }
    else {
        rctx->draw_state.light = false;

        for (int32_t i = 0; i < 2; i++)
            gl_state_active_bind_texture_2d(6 + i, shad->field_158[1 + i]->color_texture->tex);
        gl_state_active_texture(0);

        shaders_ft.env_frag_set(12, vec4_identity);
        shaders_ft.env_frag_set(13, vec4_null);
    }
}

static void draw_pass_show_vector(render_context* rctx, draw_pass* a1) {
    if (!a1->show_vector_flags)
        return;

    rctx->camera->update_data();
    rctx->view_mat = rctx->camera->view;
    rctx->post_process.rend_texture.bind();
    glViewport(0, 0, rctx->post_process.render_width, rctx->post_process.render_height);
    draw_pass_set_camera(rctx->camera);

    shaders_ft.env_vert_set(20, a1->show_vector_length, a1->show_vector_z_offset, 0.0f, 0.0f);
    for (int32_t i = 1; i < 4; i++) {
        if (~a1->show_vector_flags & (1 << (i - 1)))
            continue;

        if (a1->draw_pass_3d[DRAW_PASS_3D_OPAQUE])
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_OPAQUE, 0, i, 1, -1);
        if (a1->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT])
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSPARENT, 0, i, 1, -1);
        if (a1->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]) {
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSLUCENT_NO_SHADOW, 0, i, 1, -1);
            draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_TRANSLUCENT, 0, i, 1, -1);
        }
    }
}

static void draw_pass_post_process(render_context* rctx, draw_pass* a1) {
    rctx->camera->update_data();

    texture* light_proj_tex = 0;
    light_proj* litproj = rctx->litproj;
    if (litproj && litproj->enable) {
        light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
        if (set->lights[LIGHT_PROJECTION].get_type() == LIGHT_SPOT
            && texture_storage_get_texture(litproj->texture_id))
            light_proj_tex = litproj->draw_texture.color_texture;
    }

    rctx->post_process.apply(rctx->camera, light_proj_tex, a1->npr_param);
}

static void draw_pass_sprite(render_context* rctx, draw_pass* a1) {
}

inline static void draw_pass_end(draw_pass* pass, draw_pass_type type) {
    pass->cpu_time[type] = pass->time.calc_time();
    if (pass->wait_for_gpu) {
        time_struct t;
        glFinish();
        pass->gpu_time[type] = t.calc_time();
    }
    else
        pass->gpu_time[type] = 0;
}

static void draw_pass_3d_grid(render_context* rctx) {
    rctx->camera->update_data();

    gl_state_enable_blend();
    gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_state_set_blend_equation(GL_FUNC_ADD);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);

    grid_shader->use();
    grid_shader->set("vp", false, rctx->camera->view_projection);
    gl_state_bind_array_buffer(grid_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        sizeof(float_t) * 3, (void*)0); // Pos
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_INT,
        sizeof(float_t) * 3, (void*)(sizeof(float_t) * 2)); // Color
    gl_state_bind_array_buffer(0);
    glDrawArrays(GL_LINES, 0, (GLsizei)grid_vertex_count);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    gl_state_use_program(0);

    gl_state_disable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_blend();
}

static void draw_pass_set_camera(camera* cam) {
    cam->update_data();
    shaders_ft.state_matrix_set_modelview(0, cam->view, false);
    shaders_ft.state_matrix_set_projection(cam->projection, true);
    shaders_ft.state_matrix_set_program(5, cam->view);

    shaders_ft.env_frag_set(20,
        (float_t)(0.5 / (cam->fov_rad * cam->aspect)),
        (float_t)(0.5 / cam->fov_rad),
        0.0f, 0.0f);
    shaders_ft.env_frag_set(32,
        (float_t)(cam->max_distance / (cam->max_distance - cam->min_distance)),
        (float_t)(-(cam->max_distance * cam->min_distance) / (cam->max_distance - cam->min_distance)),
        0.0f, 0.0f);
}

static void texture_params_get(GLuint tex_0, texture_param* tex_0_param,
    GLuint tex_1, texture_param* tex_1_param, GLuint tex_2, texture_param* tex_2_param) {
    gl_state_disable_depth_test();
    if (tex_0_param) {
        tex_0_param->width = 0;
        tex_0_param->height = 0;
        if (tex_0) {
            gl_state_bind_texture_2d(tex_0);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_0_param->width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_0_param->height);
        }
    }

    if (tex_1_param) {
        tex_1_param->width = 0;
        tex_1_param->height = 0;
        if (tex_1) {
            gl_state_bind_texture_2d(tex_1);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_1_param->width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_1_param->height);
        }
    }

    if (tex_2_param) {
        tex_2_param->width = 0;
        tex_2_param->height = 0;
        if (tex_2) {
            gl_state_bind_texture_2d(tex_2);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_2_param->width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_2_param->height);
        }
    }
    gl_state_bind_texture_2d(0);

    if (tex_0_param) {
        mat4 mat;
        mat4_ortho(0.0f, (float_t)tex_0_param->width,
            0.0f, (float_t)tex_0_param->height, -1.0f, 1.0f, &mat);
        shaders_ft.state_matrix_set_projection(mat, false);
        shaders_ft.state_matrix_set_modelview(0, mat4_identity, true);
        glViewport(0, 0, tex_0_param->width, tex_0_param->height);
    }
}

static void texture_params_restore(texture_param* tex_0_param,
    texture_param* tex_1_param, texture_param* tex_2_param) {
    for (int32_t i = 0; i < 4; i++)
        gl_state_active_bind_texture_2d(i, 0);
}

static void blur_filter_apply(GLuint tex_0, GLuint tex_1, blur_filter_mode filter) {
    if (!tex_0 || !tex_1)
        return;

    texture_param tex_params[2];
    texture_params_get(tex_0, &tex_params[0], tex_1, &tex_params[1], 0, 0);
    uniform_value[U_IMAGE_FILTER] = filter == BLUR_FILTER_32 ? 1 : 0;
    shaders_ft.set(SHADER_FT_IMGFILT);

    float_t scale = filter == BLUR_FILTER_32 ? (float_t)(1.0 / 8.0) : (float_t)(1.0 / 4.0);
    shaders_ft.local_frag_set(0, scale, scale, scale, scale);
    shaders_ft.local_frag_set(1, 0.0f, 0.0f, 0.0f, 0.0f);
    float_t w = 1.0f / (float_t)tex_params[1].width;
    float_t h = 1.0f / (float_t)tex_params[1].height;
    switch (filter) {
    case BLUR_FILTER_4: {
        mat4 m[4];
        mat4_translate( 1.0f * w,  0.0f * h, 0.0f, &m[0]);
        mat4_translate( 0.0f * w,  1.0f * h, 0.0f, &m[1]);
        mat4_translate( 1.0f * w, -1.0f * h, 0.0f, &m[2]);
        mat4_translate( 0.0f * w, -1.0f * h, 0.0f, &m[3]);
        shaders_ft.state_matrix_set_texture(0, m[0]);
        shaders_ft.state_matrix_set_texture(1, m[1]);
        shaders_ft.state_matrix_set_texture(2, m[2]);
        shaders_ft.state_matrix_set_texture(3, m[3]);
    } break;
    case BLUR_FILTER_9: {
        mat4 m[4];
        mat4_translate(-0.5f * w,  0.5f * h, 0.0f, &m[0]);
        mat4_translate( 0.5f * w,  0.5f * h, 0.0f, &m[1]);
        mat4_translate(-0.5f * w, -0.5f * h, 0.0f, &m[2]);
        mat4_translate( 0.5f * w, -0.5f * h, 0.0f, &m[3]);
        shaders_ft.state_matrix_set_texture(0, m[0]);
        shaders_ft.state_matrix_set_texture(1, m[1]);
        shaders_ft.state_matrix_set_texture(2, m[2]);
        shaders_ft.state_matrix_set_texture(3, m[3]);
    } break;
    case BLUR_FILTER_16: {
        mat4 m[4];
        mat4_translate( 0.5f * w, -1.5f * h, 0.0f, &m[0]);
        mat4_translate(-1.5f * w,  0.5f * h, 0.0f, &m[1]);
        mat4_translate( 1.5f * w, -0.5f * h, 0.0f, &m[2]);
        mat4_translate(-0.5f * w,  1.5f * h, 0.0f, &m[3]);
        shaders_ft.state_matrix_set_texture(0, m[0]);
        shaders_ft.state_matrix_set_texture(1, m[1]);
        shaders_ft.state_matrix_set_texture(2, m[2]);
        shaders_ft.state_matrix_set_texture(3, m[3]);
    } break;
    case BLUR_FILTER_32: {
        mat4 m[8];
        mat4_translate( 3.5f * w, -3.5f * h, 0.0f, &m[0]);
        mat4_translate(-1.5f * w, -3.5f * h, 0.0f, &m[1]);
        mat4_translate( 1.5f * w, -1.5f * h, 0.0f, &m[2]);
        mat4_translate(-3.5f * w, -1.5f * h, 0.0f, &m[3]);
        mat4_translate( 3.5f * w,  1.5f * h, 0.0f, &m[4]);
        mat4_translate( 1.5f * w,  1.5f * h, 0.0f, &m[5]);
        mat4_translate( 1.5f * w,  3.5f * h, 0.0f, &m[6]);
        mat4_translate(-3.5f * w,  3.5f * h, 0.0f, &m[7]);
        shaders_ft.state_matrix_set_texture(0, m[0]);
        shaders_ft.state_matrix_set_texture(1, m[1]);
        shaders_ft.state_matrix_set_texture(2, m[2]);
        shaders_ft.state_matrix_set_texture(3, m[3]);
        shaders_ft.state_matrix_set_texture(4, m[4]);
        shaders_ft.state_matrix_set_texture(5, m[5]);
        shaders_ft.state_matrix_set_texture(6, m[6]);
        shaders_ft.state_matrix_set_texture(7, m[7]);
    } break;
    }
    gl_state_active_bind_texture_2d(0, tex_1);
    shaders_ft.state_matrix_set_mvp(mat4_identity);
    render_texture::draw_custom(&shaders_ft);
    texture_params_restore(&tex_params[0], &tex_params[1], 0);
}
