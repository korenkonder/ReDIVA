/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_manager.hpp"
#include "../KKdLib/mat.hpp"
#include "Glitter/glitter.hpp"
#include "light_param/fog.hpp"
#include "light_param/light.hpp"
#include "rob/rob.hpp"
#include "clear_color.hpp"
#include "gl_state.hpp"
#include "post_process.hpp"
#include "render_texture.hpp"
#include "shader_ft.hpp"
#include "sprite.hpp"
#include "static_var.hpp"
#include "task_effect.hpp"
#include "texture.hpp"

static void blur_filter_apply(render_context* rctx, GLuint tex_0, GLuint tex_1,
    blur_filter_mode filter, const vec2 res_scale, const vec4 scale, const vec4 offset);
static void draw_pass_shadow_begin_make_shadowmap(render_context* rctx,
    shadow* shad, int32_t index, int32_t a3);
static void draw_pass_shadow_end_make_shadowmap(render_context* rctx,
    shadow* shad, int32_t index, int32_t a3);
static void draw_pass_shadow_filter(render_context* rctx, render_texture* a1, render_texture* a2,
    render_texture* a3, float_t sigma, float_t far_texel_offset, bool enable_lit_proj);
static void draw_pass_shadow_esm_filter(render_context* rctx,
    render_texture* dst, render_texture* buf, render_texture* src);
static bool draw_pass_shadow_litproj(render_context* rctx, light_proj* litproj);
static void draw_pass_sss_contour(render_context* rctx, post_process* pp);
static void draw_pass_sss_filter(render_context* rctx, sss_data* a1);
static int32_t draw_pass_3d_get_translucent_count(render_context* rctx);
static void draw_pass_3d_shadow_reset(render_context* rctx);
static void draw_pass_3d_shadow_set(shadow* shad, render_context* rctx);
extern void draw_pass_3d_translucent(render_context* rctx, bool opaque_enable,
    bool transparent_enable, bool translucent_enable, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent);
static int32_t draw_pass_3d_translucent_count_layers(render_context* rctx,
    int32_t* alpha_array, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent);
static void draw_pass_3d_translucent_has_objects(render_context* rctx, bool* arr, mdl::ObjType type);

extern bool draw_grid_3d;
extern void draw_pass_3d_grid(render_context* rctx);

extern light_param_data_storage* light_param_data_storage_data;

namespace rndr {
    void RenderManager::render_all(render_context* rctx) {
        static const int32_t ibl_texture_index[] = {
            9, 10, 11, 12, 13
        };

        camera* cam = rctx->camera;

        for (int32_t i = 0; i < 5; i++)
            gl_state_active_bind_texture_cube_map(ibl_texture_index[i],
                light_param_data_storage_data->textures[i]);

        rctx->obj_scene.g_framebuffer_size = {
            1.0f / (float_t)rctx->post_process.render_width,
            1.0f / (float_t)rctx->post_process.render_height,
            (float_t)rctx->post_process.render_width,
            (float_t)rctx->post_process.render_height
        };

        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        gl_state_enable_depth_test();
        gl_state_set_depth_func(GL_LEQUAL);
        gl_state_set_depth_mask(GL_TRUE);
        for (int32_t i = RND_PASSID_SHADOWMAP; i < RND_PASSID_NUM; i++)
            RenderManager::render_single_pass((RenderPassID)i, rctx);

        rctx->disp_manager.check_vertex_arrays();
        gl_state_bind_vertex_array(0);
        gl_state_disable_primitive_restart();
        gl_state_bind_uniform_buffer_base(0, 0);
        gl_state_bind_uniform_buffer_base(1, 0);
        gl_state_bind_uniform_buffer_base(2, 0);
        gl_state_bind_uniform_buffer_base(3, 0);
        gl_state_bind_uniform_buffer_base(4, 0);
    }

    void RenderManager::render_single_pass(RenderPassID id, render_context* rctx) {
        cpu_time[id] = 0.0;
        gpu_time[id] = 0.0;
        if (!pass_sw[id]) {
            gl_state_get_error();
            return;
        }

        render_pass_begin();
        switch (id) {
        case RND_PASSID_SHADOWMAP:
            pass_shadowmap(rctx);
            break;
        case RND_PASSID_SS_SSS:
            pass_ss_sss(rctx);
            break;
        case RND_PASSID_REFLECT:
            pass_reflect(rctx);
            break;
        case RND_PASSID_REFRACT:
            pass_refract(rctx);
            break;
        case RND_PASSID_USER:
            pass_user(rctx);
            break;
        case RND_PASSID_CLEAR:
            pass_clear(rctx);
            break;
        case RND_PASSID_SPRITE_BG:
            pass_sprite_bg(rctx);
            break;
        case RND_PASSID_ALL_3D:
            pass_all_3d(rctx);
            break;
        case RND_PASSID_SHOW_VECTOR:
            pass_show_vector(rctx);
            break;
        case RND_PASSID_POSTPROCESS:
            pass_postprocess(rctx);
            break;
        case RND_PASSID_SPRITE_FG:
            pass_sprite_fg(rctx);
            break;
        }
        render_pass_end(id);
        gl_state_get_error();
    }

    void RenderManager::render_pass_begin() {
        if (sync_gpu)
            glFinish();
        time.get_timestamp();
    }

    void RenderManager::render_pass_end(RenderPassID id) {
        cpu_time[id] = time.calc_time();
        if (sync_gpu) {
            time_struct t;
            glFinish();
            gpu_time[id] = t.calc_time();
        }
        else
            gpu_time[id] = 0;
    }

    void RenderManager::pass_shadowmap(render_context* rctx) {
        rctx->camera->update_data();
        if (rctx->litproj->set(rctx)) {
            rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

            rctx->disp_manager.draw(mdl::OBJ_TYPE_OPAQUE);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSPARENT);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSLUCENT);

            rctx->draw_state.shader_index = -1;
            uniform_value[U0A] = 0;
            draw_pass_shadow_filter(rctx, &rctx->litproj->shadow_texture[0],
                &rctx->litproj->shadow_texture[1], 0, 1.5f, 0.01f, true);

            if (draw_pass_shadow_litproj(rctx, rctx->litproj)) {
                draw_pass_set_camera(rctx);

                for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                    rctx->light_set[i].data_set(rctx->face, (light_set_id)i);

                rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

                rctx->disp_manager.draw(mdl::OBJ_TYPE_OPAQUE);
                rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSPARENT);
                rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT, 1);
                gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                gl_state_enable_blend();
                rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSLUCENT);
                gl_state_disable_blend();
                rctx->draw_state.shader_index = -1;
                gl_state_bind_framebuffer(0);
            }
        }

        rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

        bool v3 = false;
        int32_t v10[2];
        for (int32_t i = 0; i < 2; i++) {
            v10[i] = rctx->disp_manager.get_obj_count((mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_CHARA + i));
            if (v10[i])
                v3 = true;
        }

        ::shadow* shad = shadow_ptr;
        if (shadow && v3) {
            int32_t v11[2];
            v11[0] = shad->field_200[0];
            v11[1] = shad->field_200[1];
            shad->field_158[0] = &shad->field_8[0];
            shad->field_158[1] = &shad->field_8[1];
            shad->field_158[2] = &shad->field_8[2];
            shad->field_158[3] = &shad->field_8[7]; // Extra for buf

            for (int32_t i = 0, j = 0; i < 2; i++) {
                if (!v10[i])
                    continue;

                draw_pass_shadow_begin_make_shadowmap(rctx, shad, v11[i], j);
                rctx->disp_manager.draw((mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_CHARA + v11[i]));
                if (rctx->disp_manager.get_obj_count((mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_OBJECT_CHARA + v11[i])) > 0) {
                    gl_state_set_color_mask(field_2F8 ? GL_TRUE : GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    rctx->disp_manager.draw((mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_OBJECT_CHARA + i));
                    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                }
                draw_pass_shadow_end_make_shadowmap(rctx, shad, v11[i], j);
                j++;
            }
        }
        else {
            shad->field_158[0] = &shad->field_8[0];
            shad->field_158[1] = &shad->field_8[1];
            shad->field_158[2] = &shad->field_8[2];
            shad->field_158[3] = &shad->field_8[7]; // Extra for buf

            for (int32_t i = 1; i < 3; i++)
                for (int32_t j = 0; j < 4; j++) {
                    shad->field_158[i]->bind(j);
                    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                }
        }
        gl_state_bind_framebuffer(0);
    }

    void RenderManager::pass_ss_sss(render_context* rctx) {
        ::sss_data* sss = &sss_data;
        if (!sss->init || !sss->enable)
            return;

        rctx->camera->update_data();
        post_process* pp = &rctx->post_process;
        //if (pp->render_width > 1280.0)
        //    sss->npr_contour = false;

        //if (sss->npr_contour) {
            pp->rend_texture.bind();
            glViewport(0, 0, pp->render_width, pp->render_height);
        //}
        //else {
        //    sss->textures[0].bind();
        //    glViewport(0, 0, 640, 360);
        //}

        glClearColor(sss->param.x, sss->param.y, sss->param.z, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_pass_set_camera(rctx);
        for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
            rctx->light_set[i].data_set(rctx->face, (light_set_id)i);

        if (shadow)
            draw_pass_3d_shadow_set(shadow_ptr, rctx);
        else
            draw_pass_3d_shadow_reset(rctx);

        rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

        rctx->draw_state.shader_index = SHADER_FT_SSS_SKIN;
        gl_state_enable_depth_test();
        gl_state_set_depth_func(GL_LEQUAL);
        gl_state_set_depth_mask(GL_TRUE);
        rctx->disp_manager.draw(mdl::OBJ_TYPE_SSS);
        gl_state_disable_depth_test();
        rctx->draw_state.shader_index = -1;
        draw_pass_3d_shadow_reset(rctx);
        uniform_value[U_NPR] = 0;

        if (npr_param == 1) {
            if (sss->enable && sss->npr_contour) {
                uniform_value[U_NPR] = 1;
                draw_pass_sss_contour(rctx, pp);
            }
            else if (npr) {
                rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

                pp->rend_texture.bind();
                glViewport(0, 0, pp->render_width, pp->render_height);
                glClear(GL_DEPTH_BUFFER_BIT);
                rctx->draw_state.shader_index = SHADER_FT_SSS_SKIN;
                gl_state_enable_depth_test();
                gl_state_set_depth_mask(GL_TRUE);
                rctx->disp_manager.draw(mdl::OBJ_TYPE_SSS);
                gl_state_disable_depth_test();
                rctx->draw_state.shader_index = -1;
                gl_state_bind_framebuffer(0);
                uniform_value[U_NPR] = 1;
                draw_pass_sss_contour(rctx, pp);
            }
        }

        if (!sss->npr_contour) {
            sss->textures[0].bind();
            glViewport(0, 0, 640, 360);

            filter_scene_shader_data filter_scene = {};
            filter_scene.g_transform = { 1.0f, 1.0f, 0.0f, 0.0f };
            filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
            rctx->filter_scene_ubo.WriteMapMemory(filter_scene);

            imgfilter_batch_shader_data imgfilter_batch = {};
            imgfilter_batch.g_color_scale = 1.0f;
            imgfilter_batch.g_color_offset = 0.0f;
            imgfilter_batch.g_texture_lod = 0.0f;
            rctx->imgfilter_batch_ubo.WriteMapMemory(imgfilter_batch);

            uniform_value[U_IMAGE_FILTER] = 5;
            shaders_ft.set(SHADER_FT_IMGFILT);
            rctx->filter_scene_ubo.Bind(0);
            rctx->imgfilter_batch_ubo.Bind(1);
            gl_state_active_bind_texture_2d(0, pp->rend_texture.color_texture->tex);
            render_texture::draw_custom();
        }

        draw_pass_sss_filter(rctx, sss);
        gl_state_bind_framebuffer(0);
    }

    void RenderManager::pass_reflect(render_context* rctx) {
        render_texture& refl_tex = rctx->render_manager.get_render_texture(0);
        render_texture& refl_buf_tex = rctx->render_manager.get_render_texture(11);
        refl_tex.bind();
        if (rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFLECT_OPAQUE)
            || rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSPARENT)
            || rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSLUCENT)
            || rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_OPAQUE)
            || rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_TRANSPARENT)
            || rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT)) {
            glViewport(0, 0, refl_tex.color_texture->width, refl_tex.color_texture->height);

            draw_pass_set_camera(rctx);

            for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                rctx->light_set[i].data_set(rctx->face, (light_set_id)i);
            for (int32_t i = FOG_DEPTH; i < FOG_BUMP; i++)
                rctx->fog[i].data_set((fog_id)i);

            rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            rctx->draw_state.shader_index = SHADER_FT_S_REFL;

            light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
            light_clip_plane clip_plane;
            set->lights[LIGHT_SPOT].get_clip_plane(clip_plane);
            uniform_value[U_REFLECT] = 2;
            uniform_value[U_CLIP_PLANE] = clip_plane.data[1];

            gl_state_enable_depth_test();
            gl_state_set_depth_func(GL_LEQUAL);
            gl_state_set_depth_mask(GL_TRUE);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_REFLECT_OPAQUE, 0, field_31D);
            if (reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP) {
                gl_state_enable_blend();
                gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                gl_state_set_blend_equation(GL_FUNC_ADD);
                rctx->disp_manager.draw(mdl::OBJ_TYPE_REFLECT_TRANSLUCENT);
                gl_state_disable_blend();
            }

            if (reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP) {
                uniform_value[U_REFLECT] = field_31E;
                uniform_value[U_CLIP_PLANE] = clip_plane.data[0];
                rctx->disp_manager.draw(mdl::OBJ_TYPE_REFLECT_CHARA_OPAQUE);
            }
            gl_state_disable_depth_test();
            uniform_value[U_REFLECT] = 0;
            rctx->draw_state.shader_index = -1;

            for (int32_t i = reflect_blur_num, j = 0; i > 0; i--, j++)
                if (j % 2) {
                    refl_tex.bind();
                    blur_filter_apply(rctx, refl_tex.color_texture->tex,
                        refl_buf_tex.color_texture->tex, reflect_blur_filter, 1.0f, 1.0f, 0.0f);
                }
                else {
                    refl_buf_tex.bind();
                    blur_filter_apply(rctx, refl_buf_tex.color_texture->tex,
                        refl_tex.color_texture->tex, reflect_blur_filter, 1.0f, 1.0f, 0.0f);
                }

            if (reflect_blur_num % 2)
                fbo::blit(refl_buf_tex.fbos[0], refl_tex.fbos[0],
                    0, 0, refl_buf_tex.color_texture->width, refl_buf_tex.color_texture->height,
                    0, 0, refl_tex.color_texture->width, refl_tex.color_texture->height,
                    GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }
        else {
            vec4 clear_color;
            glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clear_color);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        }
        gl_state_bind_framebuffer(0);
    }

    void RenderManager::pass_refract(render_context* rctx) {
        render_texture& refract_texture = rctx->render_manager.get_render_texture(1);
        refract_texture.bind();
        if (rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFRACT_OPAQUE)
            || rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFRACT_TRANSPARENT)
            || rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_REFRACT_TRANSLUCENT)) {
            texture* refr_tex = refract_texture.color_texture;
            glViewport(0, 0, refr_tex->width, refr_tex->height);

            draw_pass_set_camera(rctx);

            for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                rctx->light_set[i].data_set(rctx->face, (light_set_id)i);
            for (int32_t i = FOG_DEPTH; i < FOG_BUMP; i++)
                rctx->fog[i].data_set((fog_id)i);

            rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

            vec4 clear_color;
            glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clear_color);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

            rctx->draw_state.shader_index = SHADER_FT_S_REFR;
            gl_state_enable_depth_test();
            gl_state_set_depth_func(GL_LEQUAL);
            gl_state_set_depth_mask(GL_TRUE);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_REFRACT_OPAQUE);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_REFRACT_TRANSPARENT);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_REFRACT_TRANSLUCENT);
            gl_state_disable_depth_test();
            rctx->draw_state.shader_index = -1;
        }
        else {
            vec4 clear_color;
            glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clear_color);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        }
        gl_state_bind_framebuffer(0);
    }

    void RenderManager::pass_user(render_context* rctx) {
        for (draw_user& i : user)
            if (i.func)
                i.func(i.data);
    }

    void RenderManager::pass_clear(render_context* rctx) {
        if (true/*field_128*/) {
            vec4 clear_color = get_clear_color();
            glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        post_process* pp = &rctx->post_process;
        if (!sss_data.enable || !sss_data.npr_contour) {
            pp->set_render_texture();

            if (sprite_manager_get_reqlist_count(2)) {
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClearDepth(1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                vec4 clear_color = get_clear_color();
                glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
                glClearDepth(1.0f);
                if (true/*field_128*/)
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                else
                    glClear(GL_DEPTH_BUFFER_BIT);
            }
            gl_state_bind_framebuffer(0);
        }
        else {
            if (sprite_manager_get_reqlist_count(2)) {
                pp->set_render_texture();

                vec4 clear_color = 0.0f;
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                gl_state_bind_framebuffer(0);
            }
            else if (true/*field_128*/) {
                pp->set_render_texture();

                vec4 clear_color = get_clear_color();
                glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);
                gl_state_bind_framebuffer(0);
            }
        }
        gl_state_get_error();
    }

    void RenderManager::pass_sprite_bg(render_context* rctx) {
        if (!sprite_manager_get_reqlist_count(2))
            return;

        post_process* pp = &rctx->post_process;
        pp->set_render_texture(true);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        gl_state_set_depth_mask(GL_FALSE);
        gl_state_disable_depth_test();
        gl_state_enable_blend();
        gl_state_disable_cull_face();
        sprite_manager_draw(rctx, 2, true,
            pp->aa->temp_buffer.color_texture,
            rctx->camera->view_projection_aet_3d);
        gl_state_enable_cull_face();
        gl_state_disable_blend();
        gl_state_enable_depth_test();
        gl_state_set_depth_mask(GL_TRUE);
        gl_state_bind_framebuffer(0);
        gl_state_get_error();
    }

    void RenderManager::pass_all_3d(render_context* rctx) {
        rctx->camera->update_data();
        rctx->post_process.set_render_texture();
        draw_pass_set_camera(rctx);
        if (!sss_data.enable || !sss_data.npr_contour || draw_pass_3d_get_translucent_count(rctx))
            glClear(GL_DEPTH_BUFFER_BIT);

        gl_state_set_depth_func(GL_LEQUAL);

        for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
            rctx->light_set[i].data_set(rctx->face, (light_set_id)i);
        for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++)
            rctx->fog[i].data_set((fog_id)i);

        if (shadow)
            draw_pass_3d_shadow_set(shadow_ptr, rctx);
        else
            draw_pass_3d_shadow_reset(rctx);

        rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

        if (pass_sw[RND_PASSID_REFLECT] && reflect) {
            render_texture& refl_tex = get_render_texture(0);
            gl_state_active_bind_texture_2d(15, refl_tex.color_texture->tex);
            uniform_value[U_WATER_REFLECT] = 1;
        }
        else {
            gl_state_active_bind_texture_2d(15, rctx->empty_texture_2d);
            uniform_value[U_WATER_REFLECT] = 0;
        }

        uniform_value[U12] = field_320 ? 1 : 0;

        if (effect_texture)
            gl_state_active_bind_texture_2d(14, effect_texture->tex);
        else
            gl_state_active_bind_texture_2d(14, rctx->empty_texture_2d);

        gl_state_active_bind_texture_2d(16, sss_data.textures[1].color_texture->tex);
        gl_state_active_texture(0);

        if (alpha_z_sort) {
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT, 1);
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT_NO_SHADOW, 2);
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1, 1);
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2, 1);
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3, 1);
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL, 1);
        }

        if (opaque_z_sort)
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_OPAQUE, 0);

        if (draw_pass_3d[DRAW_PASS_3D_OPAQUE]) {
            gl_state_enable_depth_test();
            gl_state_set_depth_mask(GL_TRUE);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_OPAQUE);
            gl_state_disable_depth_test();
        }

        Glitter::glt_particle_manager->DispScenes(Glitter::DISP_OPAQUE);

        if (draw_grid_3d)
            draw_pass_3d_grid(rctx);

        gl_state_enable_depth_test();
        gl_state_set_depth_mask(GL_TRUE);
        if (draw_pass_3d[DRAW_PASS_3D_TRANSPARENT])
            rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSPARENT);
        /*if (rctx->render_manager.field_120)
            rctx->disp_manager.draw(mdl::OBJ_TYPE_TYPE_7);*/

        rctx->post_process.exposure->get_exposure_chara_data(&rctx->post_process, rctx->camera);

        if (npr_param == 1)
            pass_3d_contour(rctx);

        rctx->post_process.draw_lens_flare(rctx->camera);
        //star_catalog_data.draw();

        draw_pass_3d_translucent(rctx,
            draw_pass_3d[DRAW_PASS_3D_OPAQUE],
            draw_pass_3d[DRAW_PASS_3D_TRANSPARENT],
            draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT],
            mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_3,
            mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3,
            mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3);

        snow_particle_draw();
        rain_particle_draw();
        leaf_particle_draw();
        particle_draw();
        gl_state_disable_depth_test();

        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        Glitter::glt_particle_manager->DispScenes(Glitter::DISP_ALPHA);
        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        /*if (npr_param == 1) {
            gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }*/

        gl_state_enable_depth_test();

        if (draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]) {
            gl_state_enable_blend();
            gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            gl_state_set_depth_mask(GL_FALSE);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSLUCENT_NO_SHADOW);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSLUCENT);
            gl_state_disable_blend();
        }

        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE); // X
        Glitter::glt_particle_manager->DispScenes(Glitter::DISP_TYPE_2);
        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        gl_state_set_depth_mask(GL_TRUE);
        draw_pass_3d_translucent(rctx,
            draw_pass_3d[DRAW_PASS_3D_OPAQUE],
            draw_pass_3d[DRAW_PASS_3D_TRANSPARENT],
            draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT],
            mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_2,
            mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2,
            mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2);
        gl_state_disable_depth_test();

        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        Glitter::glt_particle_manager->DispScenes(Glitter::DISP_NORMAL);
        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        gl_state_enable_depth_test();
        gl_state_set_depth_mask(GL_TRUE);
        draw_pass_3d_translucent(rctx,
            draw_pass_3d[DRAW_PASS_3D_OPAQUE],
            draw_pass_3d[DRAW_PASS_3D_TRANSPARENT],
            draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT],
            mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_1,
            mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1,
            mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1);

#if defined(CRE_DEV)
        if (Glitter::glt_particle_manager->CheckHasLocalEffect()) { // X
            camera* cam = rctx->camera;
            double_t fov = cam->get_fov();
            cam->set_fov(32.2673416137695);
            draw_pass_set_camera(rctx);

            rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

            rctx->disp_manager.draw(mdl::OBJ_TYPE_OPAQUE_LOCAL);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSPARENT_LOCAL);
            gl_state_enable_blend();
            rctx->disp_manager.draw(mdl::OBJ_TYPE_TRANSLUCENT_LOCAL);
            gl_state_disable_blend();

            draw_pass_3d_translucent(rctx,
                draw_pass_3d[DRAW_PASS_3D_OPAQUE],
                draw_pass_3d[DRAW_PASS_3D_TRANSPARENT],
                draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT],
                mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_2_LOCAL,
                mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2_LOCAL,
                mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL);

            gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
            Glitter::glt_particle_manager->DispScenes(Glitter::DISP_LOCAL);
            gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            cam->set_fov(fov);
            draw_pass_set_camera(rctx);
        }
#endif

        gl_state_active_bind_texture_2d(14, 0);
        gl_state_active_bind_texture_2d(15, 0);

        gl_state_disable_depth_test();

        if (shadow)
            draw_pass_3d_shadow_reset(rctx);
        shader::unbind();
        pass_sprite_fg_surf(rctx);
        gl_state_bind_framebuffer(0);
    }

    void RenderManager::pass_show_vector(render_context* rctx) {
        return;
        /*if (!show_vector_flags)
            return;

        rctx->camera->update_data();
        rctx->view_mat = rctx->camera->view;
        rctx->post_process.set_render_texture();
        draw_pass_set_camera(rctx);

        rctx->obj_scene_ubo.WriteMapMemory(rctx->obj_scene);

        if (alpha_z_sort) {
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT, 1);
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_TRANSLUCENT_NO_SHADOW, 2);
        }

        if (opaque_z_sort)
            rctx->disp_manager.obj_sort(&rctx->view_mat, mdl::OBJ_TYPE_OPAQUE, 0);

        //shaders_ft.env_vert_set(20, show_vector_length, show_vector_z_offset, 0.0f, 0.0f);
        for (int32_t i = 1; i < 4; i++) {
            if (!(show_vector_flags & (1 << (i - 1))))
                continue;

            if (draw_pass_3d[DRAW_PASS_3D_OPAQUE])
                draw_task_draw_objects_by_type_show_vector(rctx, mdl::OBJ_TYPE_OPAQUE, i);
            if (draw_pass_3d[DRAW_PASS_3D_TRANSPARENT])
                draw_task_draw_objects_by_type_show_vector(rctx, mdl::OBJ_TYPE_TRANSPARENT, i);
            if (draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]) {
                draw_task_draw_objects_by_type_show_vector(rctx, mdl::OBJ_TYPE_TRANSLUCENT_NO_SHADOW, i);
                draw_task_draw_objects_by_type_show_vector(rctx, mdl::OBJ_TYPE_TRANSLUCENT, i);
            }
        }*/
    }

    void RenderManager::pass_postprocess(render_context* rctx) {
        rctx->camera->update_data();

        texture* light_proj_tex = 0;
        light_proj* litproj = rctx->litproj;
        if (litproj && litproj->enable) {
            light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
            if (set->lights[LIGHT_PROJECTION].get_type() == LIGHT_SPOT
                && texture_storage_get_texture(litproj->texture_id))
                light_proj_tex = litproj->draw_texture.color_texture;
        }

        rctx->post_process.apply(rctx->camera, light_proj_tex, npr_param);
    }

    void RenderManager::pass_sprite_fg(render_context* rctx) {
        if (!sprite_manager_get_reqlist_count(0))
            return;

        post_process* pp = &rctx->post_process;
        glViewport(0, 0, pp->sprite_width, pp->sprite_height);

        if (multisample && multisample_framebuffer) {
            glBindFramebuffer(GL_FRAMEBUFFER, multisample_framebuffer);
            gl_state_enable_multisample();
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        else
            rctx->post_process.screen_texture.bind();

        gl_state_disable_depth_test();
        gl_state_enable_blend();
        gl_state_disable_cull_face();
        sprite_manager_draw(rctx, 0, true,
            pp->aa->temp_buffer.color_texture,
            rctx->camera->view_projection_aet_2d);
        gl_state_enable_cull_face();
        gl_state_disable_blend();
        gl_state_enable_depth_test();

        if (multisample && multisample_framebuffer) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            gl_state_disable_multisample();
            gl_state_bind_read_framebuffer(multisample_framebuffer);
            glBlitFramebuffer(0, 0, pp->sprite_width, pp->sprite_height,
                0, 0, pp->sprite_width, pp->sprite_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            gl_state_bind_read_framebuffer(0);
        }
        gl_state_get_error();
    }

    void RenderManager::pass_3d_contour(render_context* rctx) {
        render_texture* rt = &rctx->post_process.rend_texture;
        render_texture* contour_rt = &rctx->post_process.sss_contour_texture;

        gl_state_enable_blend();
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gl_state_set_blend_equation(GL_FUNC_ADD);
        gl_state_disable_depth_test();

        quad_shader_data quad_data = {};
        quad_data.g_texcoord_modifier = { 0.5f, 0.5f, 0.5f, 0.5f };
        rctx->quad_ubo.WriteMapMemory(quad_data);

        contour_params_shader_data contour_params_data = {};
        contour_params_data.g_near_far = rctx->g_near_far;
        rctx->contour_params_ubo.WriteMapMemory(contour_params_data);

        shaders_ft.set(SHADER_FT_CONTOUR_NPR);
        gl_state_active_bind_texture_2d(16, contour_rt->color_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_state_active_bind_texture_2d(17, contour_rt->depth_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_state_active_bind_texture_2d(14, rt->depth_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        rctx->quad_ubo.Bind(0);
        rctx->contour_params_ubo.Bind(2);
        render_texture::draw(&shaders_ft);

        if (effect_texture)
            gl_state_active_bind_texture_2d(14, effect_texture->tex);
        else
            gl_state_active_bind_texture_2d(14, rctx->empty_texture_2d);
        gl_state_enable_depth_test();
        gl_state_disable_blend();
    }

    void RenderManager::pass_sprite_fg_surf(render_context* rctx) {
        if (!sprite_manager_get_reqlist_count(1))
            return;

        post_process* pp = &rctx->post_process;

        gl_state_set_depth_mask(GL_FALSE);
        gl_state_disable_depth_test();
        gl_state_disable_depth_test();
        gl_state_enable_blend();
        gl_state_disable_cull_face();
        sprite_manager_draw(rctx, 1, true,
            pp->aa->temp_buffer.color_texture,
            rctx->camera->view_projection_aet_2d);
    }
}

void image_filter_scale(render_context* rctx, GLuint dst, GLuint src, const vec4 scale) {
    if (!dst || !src)
        return;

    texture_param tex_params[2];
    texture_params_get(dst, &tex_params[0], src, &tex_params[1]);

    filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = { 1.0f, 1.0f, 0.0f, 0.0f };
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rctx->filter_scene_ubo.WriteMapMemory(filter_scene);

    imgfilter_batch_shader_data imgfilter_batch = {};
    imgfilter_batch.g_color_scale = 1.0f;
    imgfilter_batch.g_color_offset = 0.0f;
    imgfilter_batch.g_texture_lod = 0.0f;
    rctx->imgfilter_batch_ubo.WriteMapMemory(imgfilter_batch);

    uniform_value[U_IMAGE_FILTER] = 5;
    shaders_ft.set(SHADER_FT_IMGFILT);
    rctx->filter_scene_ubo.Bind(0);
    rctx->imgfilter_batch_ubo.Bind(1);
    gl_state_active_bind_texture_2d(0, src);
    render_texture::draw_custom();

    texture_params_restore(&tex_params[0], &tex_params[1]);
}

void draw_pass_set_camera(render_context* rctx) {
    camera* cam = rctx->camera;
    cam->update_data();
    rctx->view_mat = cam->view;
    rctx->proj_mat = cam->projection;
    rctx->vp_mat = cam->view_projection;
    rctx->obj_scene.set_projection_view(cam->view, cam->projection);
    cam->get_view_point(rctx->obj_scene.g_view_position);

    rctx->g_near_far = {
        (float_t)(cam->max_distance / (cam->max_distance - cam->min_distance)),
        (float_t)(-(cam->max_distance * cam->min_distance) / (cam->max_distance - cam->min_distance)),
        0.0f, 0.0f
    };
}

static void draw_pass_shadow_begin_make_shadowmap(render_context* rctx,
    shadow* shad, int32_t index, int32_t a3) {
    texture* tex = shad->field_158[0]->color_texture;
    shad->field_158[0]->bind();
    glViewport(0, 0, tex->width, tex->height);
    glScissor(0, 0, tex->width, tex->height);
    gl_state_enable_depth_test();
    gl_state_enable_scissor_test();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    if (!a3)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else if (shad->field_2F5)
        glClear(GL_COLOR_BUFFER_BIT);
    glScissor(1, 1, tex->width - 2, tex->height - 2);

    float_t offset = 0.0f;
    float_t range = shad->view_region * shad->range;
    vec3* interest;
    vec3* view_point;
    if (shad->field_2F5) {
        offset = -0.5f;
        if (index)
            offset = 0.5f;

        interest = &shad->interest[index];
        view_point = &shad->view_point[index];
    }
    else {
        interest = &shad->interest_shared;
        view_point = &shad->view_point_shared;
    }

    mat4 temp;
    mat4_translate(offset, 0.0f, 0.0f, &temp);

    mat4 proj;
    mat4_ortho(-range, range, -range, range, shad->z_near, shad->z_far, &proj);
    mat4_mult(&proj, &temp, &rctx->proj_mat);
    mat4_look_at(view_point, interest, &rctx->view_mat);

    rctx->draw_state.shader_index = SHADER_FT_SIL;
    uniform_value[U0A] = 0;
}

static void draw_pass_shadow_end_make_shadowmap(render_context* rctx,
    shadow* shad, int32_t index, int32_t a3) {
    gl_state_disable_depth_test();
    gl_state_disable_scissor_test();

    rctx->draw_state.shader_index = -1;
    if (a3 == shad->field_2EC - 1) {
        draw_pass_shadow_filter(rctx, &shad->field_8[3], &shad->field_8[4], shad->field_158[0],
            shad->field_2DC, shad->field_2E0 / (shad->field_208 * 2.0f), false);
        draw_pass_shadow_esm_filter(rctx, &shad->field_8[5], &shad->field_8[6], &shad->field_8[3]);
    }

    render_texture* rend_tex = shad->field_158[1 + index];
    render_texture* rend_buf_tex = shad->field_158[3];
    rend_tex->bind();

    render_texture* src = shad->field_158[0];
    image_filter_scale(rctx, rend_tex->color_texture->tex, src->color_texture->tex, 1.0f);

    if (shad->blur_filter_enable[index]) {
        for (int32_t i = shad->near_blur, j = 0; i > 0; i--, j++)
            if (j % 2) {
                rend_tex->bind();
                blur_filter_apply(rctx, rend_tex->color_texture->tex,
                    rend_buf_tex->color_texture->tex, shad->blur_filter, 1.0f, 1.0f, 0.0f);
            }
            else {
                rend_buf_tex->bind();
                blur_filter_apply(rctx, rend_buf_tex->color_texture->tex,
                    rend_tex->color_texture->tex, shad->blur_filter, 1.0f, 1.0f, 0.0f);
            }

        if (shad->near_blur % 2)
            fbo::blit(rend_buf_tex->fbos[0], rend_tex->fbos[0],
                0, 0, rend_buf_tex->color_texture->width, rend_buf_tex->color_texture->height,
                0, 0, rend_tex->color_texture->width, rend_tex->color_texture->height,
                GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }
    else {
        rend_tex->bind();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    gl_state_bind_framebuffer(0);
    gl_state_bind_texture_2d(rend_tex->color_texture->tex);
    glGenerateMipmap(GL_TEXTURE_2D);
    gl_state_bind_texture_2d(0);
}

static void draw_pass_shadow_filter(render_context* rctx, render_texture* a1, render_texture* a2,
    render_texture* a3, float_t sigma, float_t far_texel_offset, bool enable_lit_proj) {
    GLuint v7 = a1->color_texture->tex;
    GLuint v9 = a2->color_texture->tex;
    GLuint v11 = v7;
    if (a3)
        v11 = a3->depth_texture->tex;

    if (!v7 || !v9 || !v11)
        return;

    texture_param tex_params[2];
    texture_params_get(v7, &tex_params[0], v9, &tex_params[1]);
    if (tex_params[0].width != tex_params[1].width
        || tex_params[0].height != tex_params[1].height)
        return;

    filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = 0.0f;
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rctx->filter_scene_ubo.WriteMapMemory(filter_scene);

    esm_filter_batch_shader_data esm_filter_batch = {};
    esm_filter_batch.g_params = { 1.0f / (float_t)tex_params[0].width, 0.0f, far_texel_offset, far_texel_offset };
    double_t v6 = 1.0 / (sqrt(M_PI * 2.0) * sigma);
    double_t v8 = -1.0 / (2.0 * sigma * sigma);
    for (int32_t i = 0; i < 8; i++)
        ((float_t*)esm_filter_batch.g_gauss)[i] = (float_t)(exp((double_t)((ssize_t)i * i) * v8) * v6);

    uniform_value[U_LIGHT_PROJ] = enable_lit_proj ? 1 : 0;
    shaders_ft.set(SHADER_FT_ESMGAUSS);
    rctx->filter_scene_ubo.Bind(0);
    rctx->esm_filter_batch_ubo.Bind(1);

    a2->bind();
    esm_filter_batch.g_params = { 1.0f / (float_t)tex_params[0].width, 0.0f, far_texel_offset, far_texel_offset };
    rctx->esm_filter_batch_ubo.WriteMapMemory(esm_filter_batch);

    gl_state_active_bind_texture_2d(0, v11);
    if (a3) {
        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    }
    else {
        GLint swizzle[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    }
    render_texture::draw_custom();

    a1->bind();

    esm_filter_batch.g_params = { 0.0f, 1.0f / (float_t)tex_params[0].height, far_texel_offset, far_texel_offset };
    rctx->esm_filter_batch_ubo.WriteMapMemory(esm_filter_batch);

    gl_state_active_bind_texture_2d(0, v9);
    render_texture::draw_custom();
    shader::unbind();

    texture_params_restore(&tex_params[0], &tex_params[1]);
}

static void draw_pass_shadow_esm_filter(render_context* rctx,
    render_texture* dst, render_texture* buf, render_texture* src) {
    GLuint dst_tex = dst->color_texture->tex;
    GLuint buf_tex = buf->color_texture->tex;
    GLuint src_tex = src->color_texture->tex;
    if (!dst_tex || !buf_tex || !src_tex)
        return;

    filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = 0.0f;
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rctx->filter_scene_ubo.WriteMapMemory(filter_scene);

    esm_filter_batch_shader_data esm_filter_batch = {};
    esm_filter_batch.g_gauss[0] = 0.0f;
    esm_filter_batch.g_gauss[1] = 0.0f;

    rctx->filter_scene_ubo.Bind(0);
    rctx->esm_filter_batch_ubo.Bind(1);

    texture_param tex_params[3];
    texture_params_get(dst_tex, &tex_params[0], src_tex, &tex_params[1], buf_tex, &tex_params[2]);

    buf->bind();
    esm_filter_batch.g_params = { 1.0f / (float_t)tex_params[1].width,
        1.0f / (float_t)tex_params[1].height, 0.0f, 0.0f };
    rctx->esm_filter_batch_ubo.WriteMapMemory(esm_filter_batch);

    uniform_value[U_ESM_FILTER] = 0;
    shaders_ft.set(SHADER_FT_ESMFILT);
    gl_state_active_bind_texture_2d(0, src_tex);
    render_texture::draw(&shaders_ft);

    dst->bind();
    esm_filter_batch.g_params = { 0.75f / (float_t)tex_params[2].width,
        0.75f / (float_t)tex_params[2].height, 0.0f, 0.0f };
    rctx->esm_filter_batch_ubo.WriteMapMemory(esm_filter_batch);

    uniform_value[U_ESM_FILTER] = 1;
    shaders_ft.set(SHADER_FT_ESMFILT);
    gl_state_active_bind_texture_2d(0, buf_tex);
    render_texture::draw(&shaders_ft);

    texture_params_restore(&tex_params[0], &tex_params[1], &tex_params[2]);
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

    if (!light_proj::set_mat(rctx, 0)) {
        gl_state_bind_framebuffer(0);
        return false;
    }

    static const vec4 border_color = 0.0f;
    rctx->draw_state.shader_index = SHADER_FT_LITPROJ;
    gl_state_active_bind_texture_2d(17, tex->tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    gl_state_active_bind_texture_2d(18, litproj->shadow_texture[0].color_texture->tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    gl_state_active_texture(0);
    return true;
}

static void draw_pass_sss_contour(render_context* rctx, post_process* pp) {
    pp->sss_contour_texture.bind();
    gl_state_enable_depth_test();
    gl_state_set_depth_func(GL_ALWAYS);
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

    camera* cam = rctx->camera;
    float_t v3 = 1.0f / tanf((float_t)(cam->fov_rad * 0.5));

    vec3 direction = cam->interest - cam->view_point;
    float_t length = vec3::length(direction);
    float_t v7 = direction.y;
    if (length != 0.0f)
        v7 /= length;

    float_t v9 = fabsf(v7) - 0.1f;
    if (v9 < 0.0f)
        v9 = 0.0f;

    contour_coef_shader_data shader_data = {};
    shader_data.g_contour = { v9 * 0.004f + 0.0027f, 0.003f, v3 * 0.35f, 0.0008f };
    shader_data.g_near_far = rctx->g_near_far;
    rctx->contour_coef_ubo.WriteMapMemory(shader_data);

    shaders_ft.set(SHADER_FT_CONTOUR);
    rctx->contour_coef_ubo.Bind(2);
    render_texture::draw_quad(&shaders_ft, pp->render_width, pp->render_height);
}

static void draw_pass_sss_filter_calc_coef(double_t a1, size_t a2, double_t a3, size_t a4,
    const double_t* a5, const double_t* a6, const double_t* a7, const double_t* a8, vec4 params[64]) {
    if (a2 > 8)
        return;

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
}

static void draw_pass_sss_filter(render_context* rctx, sss_data* a1) {
    const int32_t sss_count = 6;
    vec3 interest = rctx->camera->interest;
    vec3 view_point = rctx->camera->view_point;

    vec3 chara_position[2];
    chara_position[0] = 0.0f;
    chara_position[1] = 0.0f;
    float_t chara_distance[2];
    for (int32_t i = 0; i < 2; i++) {
        chara_position[i] = interest;
        chara_distance[i] = 999999.0f;
        rob_chara_bone_data* v16 = rob_chara_array[i].bone_data;
        if (rob_chara_pv_data_array[i].type != ROB_CHARA_TYPE_NONE && rob_chara_array[i].data.field_0 & 1) {
            mat4* mat = rob_chara_bone_data_get_mats_mat(v16, MOTION_BONE_N_HARA_CP);
            if (mat) {
                mat4_get_translation(mat, &chara_position[i]);
                chara_distance[i] = vec3::distance(view_point, chara_position[i]);
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

    float_t length = vec3::distance(interest, closest_chara_position);
    if (length > 1.25f)
        interest = chara_position[0];

    float_t v29 = vec3::distance(view_point, interest);
    if (v29 < 0.25f)
        v29 = 0.25f;
    float_t v31 = tanf((float_t)(rctx->camera->fov_rad * 0.5)) * 5.0f;
    if (v31 < 0.25f)
        v31 = 0.25f;
    float_t v32 = v31 * v29;
    float_t v33 = 0.6f;
    float_t v34 = 1.0f / clamp_def(v32, 0.25f, 100.0f);
    if (v34 < 0.145f) {
        float_t v36 = v34 - 0.02f;
        if (v34 < 0.0f)
            v36 = 0.0f;
        v33 = (v36 * 8.0f) * 0.6f;
    }
    rctx->obj_batch.g_sss_param = { v33, 0.0f, 0.0f, 0.0f };

    gl_state_active_texture(0);
    if (a1->npr_contour) {
        a1->textures[0].bind();
        glViewport(0, 0, 640, 360);
        post_process* pp = &rctx->post_process;
        uniform_value[U_REDUCE] = 0;
        shaders_ft.set(SHADER_FT_REDUCE);
        gl_state_bind_texture_2d(pp->rend_texture.color_texture->tex);
        render_texture::draw_quad(&shaders_ft, 640, 360, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
    }
    a1->textures[2].bind();
    glViewport(0, 0, 320, 180);
    uniform_value[U_SSS_FILTER] = 0;
    shaders_ft.set(SHADER_FT_SSS_FILT);
    gl_state_bind_texture_2d(a1->textures[0].color_texture->tex);
    render_texture::draw_quad(&shaders_ft, 640, 360, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);

    sss_filter_gaussian_coef_shader_data shader_data = {};
    shader_data.g_param = { (float_t)(sss_count - 1), 0.0f, 1.0f, 1.0f };

    const double_t a5[] = { 0.4, 0.3, 0.3 };
    const double_t a6[] = { 1.0, 2.0, 5.0 };
    const double_t a7[] = { 0.2, 0.4, 1.2 };
    const double_t a8[] = { 0.3, 0.7, 2.0 };
    draw_pass_sss_filter_calc_coef(1.0, sss_count, v34, 3, a5, a6, a7, a8, shader_data.g_coef);

    rctx->sss_filter_gaussian_coef_ubo.WriteMapMemory(shader_data);
    a1->textures[1].bind();
    glViewport(0, 0, 320, 180);
    uniform_value[U_SSS_FILTER] = 3;
    shaders_ft.set(SHADER_FT_SSS_FILT);
    gl_state_bind_texture_2d(a1->textures[2].color_texture->tex);
    rctx->sss_filter_gaussian_coef_ubo.Bind(1);
    render_texture::draw_quad(&shaders_ft, 320, 180, 1.0f, 1.0f, 0.96f, 1.0f, 0.0f);
    gl_state_bind_texture_2d(0);
}

static int32_t draw_pass_3d_get_translucent_count(render_context* rctx) {
    int32_t count = 0;
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_1);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_2);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_3);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_2_LOCAL);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2_LOCAL);
    count += rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL);
    return count;
}

static void draw_pass_3d_shadow_reset(render_context* rctx) {
    gl_state_active_bind_texture_2d(6, 0);
    gl_state_active_bind_texture_2d(7, 0);
    rctx->obj_scene.set_g_self_shadow_receivers(0, mat4_identity);
    rctx->obj_scene.set_g_self_shadow_receivers(1, mat4_identity);
    rctx->draw_state.self_shadow = false;
    rctx->draw_state.light = false;
}

static void draw_pass_3d_shadow_set(shadow* shad, render_context* rctx) {
    if (shad->self_shadow && shad->field_2EC > 0) {
        gl_state_active_bind_texture_2d(19, shad->field_8[3].color_texture->tex);
        gl_state_active_bind_texture_2d(20, shad->field_8[5].color_texture->tex);
        gl_state_active_texture(0);
        float_t esm_param = (shad->field_2D8 * shad->field_208 * 2.0f) * 1.442695f;
        rctx->obj_scene.g_esm_param = { esm_param, 0.0f, 0.0f, 0.0f };
        rctx->draw_state.self_shadow = true;
    }
    else {
        gl_state_active_bind_texture_2d(19, rctx->empty_texture_2d);
        gl_state_active_bind_texture_2d(20, rctx->empty_texture_2d);
        gl_state_active_texture(0);
        rctx->draw_state.self_shadow = false;
    }

    if (shad->field_2EC > 0) {
        rctx->draw_state.light = true;
        uniform_value[U_LIGHT_1] = shad->field_2EC > 1 ? 1 : 0;

        float_t range = shad->view_region * shad->range;
        for (int32_t i = 0; i < 2; i++) {
            float_t v6 = 0.0f;
            vec3* interest;
            vec3* view_point;
            if (shad->field_2F5) {
                v6 = i ? 0.5f : -0.5f;

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
            mat4_ortho(-range, range, -range, range, shad->z_near, shad->z_far, &proj);
            mat4_mult(&proj, &temp, &proj);

            mat4 view;
            mat4_look_at(view_point, interest, &view);
            mat4_mult(&view, &proj, &view);
            rctx->obj_scene.set_g_self_shadow_receivers(i, view);
        }

        for (int32_t i = 0, j = 0; i < 2; i++) {
            if (!shad->field_2F0[i]) {
                gl_state_active_bind_texture_2d(6 + j, rctx->empty_texture_2d);
                continue;
            }

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

        rctx->obj_scene.g_shadow_ambient = { ambient.x, ambient.y, ambient.z, 1.0f };
        rctx->obj_scene.g_shadow_ambient1 = { 1.0f - ambient.x, 1.0f - ambient.y, 1.0f - ambient.z, 0.0f };
    }
    else {
        rctx->draw_state.light = false;

        for (int32_t i = 0; i < 2; i++)
            gl_state_active_bind_texture_2d(6 + i, shad->field_158[1 + i]->color_texture->tex);
        gl_state_active_texture(0);

        rctx->obj_scene.g_shadow_ambient = 1.0f;
        rctx->obj_scene.g_shadow_ambient1 = 0.0f;
    }
}

static void draw_pass_3d_translucent(render_context* rctx, bool opaque_enable,
    bool transparent_enable, bool translucent_enable, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent) {
    if (rctx->disp_manager.get_obj_count(opaque) < 1
        && rctx->disp_manager.get_obj_count(transparent) < 1
        && rctx->disp_manager.get_obj_count(translucent) < 1)
        return;

    post_process* pp = &rctx->post_process;

    int32_t alpha_array[256];
    int32_t count = draw_pass_3d_translucent_count_layers(rctx,
        alpha_array, opaque, transparent, translucent);
    for (int32_t i = 0; i < count; i++) {
        int32_t alpha = alpha_array[i];
        fbo::blit(pp->rend_texture.fbos[0], pp->transparency_texture.fbos[0],
            0, 0, pp->render_width, pp->render_height,
            0, 0, pp->render_width, pp->render_height,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        if (opaque_enable && rctx->disp_manager.get_obj_count(opaque))
            rctx->disp_manager.draw_translucent(opaque, alpha);
        if (transparent_enable && rctx->disp_manager.get_obj_count(transparent))
            rctx->disp_manager.draw_translucent(transparent, alpha);
        if (translucent_enable && rctx->disp_manager.get_obj_count(translucent)) {
            gl_state_enable_blend();
            rctx->disp_manager.draw_translucent(translucent, alpha);
            gl_state_disable_blend();
        }

        transparency_batch_shader_data shader_data = {};
        shader_data.g_opacity = { (float_t)alpha * (float_t)(1.0 / 255.0), 0.0f, 0.0f, 0.0f };
        rctx->transparency_batch_ubo.WriteMapMemory(shader_data);

        pp->buf_texture.bind();
        shaders_ft.set(SHADER_FT_TRANSPARENCY);
        rctx->transparency_batch_ubo.Bind(0);
        gl_state_active_bind_texture_2d(0, pp->transparency_texture.color_texture->tex);
        gl_state_active_bind_texture_2d(1, pp->rend_texture.color_texture->tex);
        render_texture::draw_custom();

        fbo::blit(pp->buf_texture.fbos[0], pp->rend_texture.fbos[0],
            0, 0, pp->render_width, pp->render_height,
            0, 0, pp->render_width, pp->render_height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

static int32_t draw_pass_3d_translucent_count_layers(render_context* rctx,
    int32_t* alpha_array, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent) {
    bool arr[256] = { 0 };

    draw_pass_3d_translucent_has_objects(rctx, arr, opaque);
    draw_pass_3d_translucent_has_objects(rctx, arr, transparent);
    draw_pass_3d_translucent_has_objects(rctx, arr, translucent);

    int32_t count = 0;
    for (int32_t i = 0xFF; i >= 1; i--)
        if (arr[i]) {
            count++;
            *alpha_array++ = i;
        }
    return count;
}

static void draw_pass_3d_translucent_has_objects(render_context* rctx, bool* arr, mdl::ObjType type) {
    std::vector<mdl::ObjData*>& vec = rctx->disp_manager.obj[type];
    for (mdl::ObjData*& i : vec)
        switch (i->kind) {
        case mdl::OBJ_KIND_NORMAL: {
            int32_t alpha = (int32_t)(i->args.sub_mesh.blend_color.w * 255.0f);
            alpha = clamp_def(alpha, 0, 255);
            arr[alpha] = true;
        } break;
        case mdl::OBJ_KIND_TRANSLUCENT: {
            for (uint32_t j = 0; j < i->args.translucent.count; j++) {
                int32_t alpha = (int32_t)(i->args.translucent.sub_mesh[j]->blend_color.w * 255.0f);
                alpha = clamp_def(alpha, 0, 255);
                arr[alpha] = true;
            }
        } break;
        }
}

static void blur_filter_apply(render_context* rctx, GLuint dst, GLuint src,
    blur_filter_mode filter, const vec2 res_scale, const vec4 scale, const vec4 offset) {
    if (!dst || !src)
        return;

    texture_param tex_params[2];
    texture_params_get(dst, &tex_params[0], src, &tex_params[1]);

    filter_scene_shader_data filter_scene = {};
    float_t w = res_scale.x / (float_t)tex_params[1].width;
    float_t h = res_scale.y / (float_t)tex_params[1].height;
    filter_scene.g_transform = { w, h, 0.0f, 0.0f };
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rctx->filter_scene_ubo.WriteMapMemory(filter_scene);

    imgfilter_batch_shader_data imgfilter_batch = {};
    if (filter == BLUR_FILTER_32)
        imgfilter_batch.g_color_scale = scale * (float_t)(1.0 / 8.0);
    else
        imgfilter_batch.g_color_scale = scale * (float_t)(1.0 / 4.0);
    imgfilter_batch.g_color_offset = offset;
    imgfilter_batch.g_texture_lod = 0.0f;
    rctx->imgfilter_batch_ubo.WriteMapMemory(imgfilter_batch);

    uniform_value[U_IMAGE_FILTER] = filter == BLUR_FILTER_32 ? 1 : 0;
    gl_state_bind_vertex_array(rctx->box_vao);
    shaders_ft.set(SHADER_FT_IMGFILT);
    rctx->filter_scene_ubo.Bind(0);
    rctx->imgfilter_batch_ubo.Bind(1);
    gl_state_active_bind_texture_2d(0, src);
    glDrawArrays(GL_TRIANGLE_STRIP, (GLint)(filter * 4LL), 4);

    texture_params_restore(&tex_params[0], &tex_params[1]);
}
