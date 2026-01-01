/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_manager.hpp"
#include "../KKdLib/mat.hpp"
#include "Glitter/glitter.hpp"
#include "light_param/fog.hpp"
#include "light_param/light.hpp"
#include "mdl/draw_object.hpp"
#include "rob/rob.hpp"
#include "Vulkan/gl_wrap.hpp"
#include "config.hpp"
#include "clear_color.hpp"
#include "effect.hpp"
#include "gl_state.hpp"
#include "light_param.hpp"
#include "reflect_full.hpp"
#include "render.hpp"
#include "render_context.hpp"
#include "render_texture.hpp"
#include "shader_ft.hpp"
#include "sprite.hpp"
#include "static_var.hpp"
#include "texture.hpp"

extern render_context* rctx_ptr;

static void draw_pass_shadow_begin_make_shadowmap(Shadow* shad,
    render_data_context& rend_data_ctx, cam_data& cam, int32_t index, int32_t a3);
static void draw_pass_shadow_end_make_shadowmap(Shadow* shad,
    render_data_context& rend_data_ctx, int32_t index, int32_t a3);
static void apply_esm_filter(render_data_context& rend_data_ctx,
    RenderTexture* dst, RenderTexture* buf, RenderTexture* src,
    float_t sigma, float_t far_texel_offset, bool enable_lit_proj);
static void apply_esm_min_filter(render_data_context& rend_data_ctx,
    RenderTexture* dst, RenderTexture* buf, RenderTexture* src);
static bool litproj_clear(render_data_context& rend_data_ctx, light_proj* litproj, cam_data& cam);
static int32_t draw_pass_3d_get_translucent_count(render_context* rctx);
static void draw_pass_3d_shadow_reset(render_data_context& rend_data_ctx, render_context* rctx);
static void draw_pass_3d_shadow_set(render_data_context& rend_data_ctx, Shadow* shad, render_context* rctx);
extern void draw_pass_3d_translucent(render_data_context& rend_data_ctx,
    render_context* rctx, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent, cam_data& cam);
extern void draw_pass_3d_translucent(render_data_context& rend_data_ctx,
    render_context* rctx, mdl::ObjTypeScreen opaque,
    mdl::ObjTypeScreen transparent, mdl::ObjTypeScreen translucent, cam_data& cam);
static int32_t draw_pass_3d_translucent_count_layers(render_context* rctx,
    int32_t* alpha_array, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent, cam_data& cam);
static int32_t draw_pass_3d_translucent_count_layers(render_context* rctx,
    int32_t* alpha_array, mdl::ObjTypeScreen opaque,
    mdl::ObjTypeScreen transparent, mdl::ObjTypeScreen translucent, cam_data& cam);
static void draw_pass_3d_translucent_has_objects(
    render_context* rctx, bool* arr, mdl::ObjType type, cam_data& cam);
static void draw_pass_3d_translucent_has_objects(
    render_context* rctx, bool* arr, mdl::ObjTypeScreen type, cam_data& cam);

static void draw_pass_reflect_full(render_data_context& rend_data_ctx, rndr::RenderManager* render_manager);

static void apply_blur_filter_sub(render_data_context& rend_data_ctx, RenderTexture* dst, RenderTexture* src,
    blur_filter_mode filter, const vec2 res_scale, const vec4 scale, const vec4 offset);

static void render_manager_free_render_textures();
static void render_manager_init_render_textures(int32_t multisample);

static void set_reflect_mat(render_data_context& rend_data_ctx, cam_data& cam);

namespace rndr {
    struct RenderTextureData {
        GLenum type;
        int32_t width;
        int32_t height;
        int32_t max_level;
        GLenum color_format;
        GLenum depth_format;
    };

    static const RenderTextureData render_manager_render_texture_data_array[] = {
        { GL_TEXTURE_2D, 0x200, 0x100, 0, GL_RGBA8  , GL_DEPTH_COMPONENT24 },
        { GL_TEXTURE_2D, 0x200, 0x100, 0, GL_RGBA16F, GL_DEPTH_COMPONENT24 },
        { GL_TEXTURE_2D, 0x400, 0x400, 0, GL_RGBA8  , GL_ZERO },
        { GL_TEXTURE_2D, 0x400, 0x400, 0, GL_RGBA8  , GL_ZERO },
        { GL_TEXTURE_2D, 0x400, 0x400, 0, GL_RGBA8  , GL_ZERO },
        { GL_TEXTURE_2D, 0x100, 0x100, 0, GL_RGBA8  , GL_ZERO },
        { GL_TEXTURE_2D, 0x100, 0x100, 0, GL_RGBA32F, GL_ZERO },
        { GL_TEXTURE_2D, 0x100, 0x100, 0, GL_RGBA8  , GL_ZERO },
    };

    static const int32_t render_manager_render_texture_index_array[][3] = {
        { 1, 1, 1 },
        { 0, 0, 0 },
        { 6, 6, 6 },
        { 6, 6, 6 },
        { 6, 6, 6 },
        { 2, 2, 2 },
        { 3, 3, 3 },
        { 4, 4, 4 },
        { 5, 5, 5 },
        { 6, 6, 6 },
        { 7, 7, 7 },
    };

    RenderManager::RenderManager() : pass_sw(), reflect_blur_num(), reflect_blur_filter(),
        render(), sync_gpu(), cpu_time(), gpu_time(), draw_pass_3d(), field_11E(), field_11F(),
        field_120(), show_ref_map(), reflect_type(), clear(), tex_index(), width(), height(),
        multisample_framebuffer(), multisample_renderbuffer(), multisample(), check_state(), show_vector_flags(),
        show_vector_length(), show_vector_z_offset(), show_stage_shadow(), effect_texture(), npr_param(),
        npr_mask(), reflect_texture_mask(), reflect_tone_curve(), field_31F(), light_stage_ambient(), npr() {
        for (bool& i : pass_sw)
            i = true;

        set_pass_sw(RND_PASSID_2, false);
        set_pass_sw(RND_PASSID_REFLECT, false);
        set_pass_sw(RND_PASSID_REFRACT, false);
        set_pass_sw(RND_PASSID_PRE_PROCESS, false);
        set_pass_sw(RND_PASSID_SHOW_VECTOR, false);

        shadow = true;
        opaque_z_sort = true;
        alpha_z_sort = true;

        for (bool& i : draw_pass_3d)
            i = true;
    }

    RenderManager::~RenderManager() {

    }

    void RenderManager::add_pre_process(int32_t type, draw_pre_process_func func, void* data) {
        pre_process.push_back({ type, func, data });
    }

    void RenderManager::clear_pre_process(int32_t type) {
        for (auto i = pre_process.begin(); i != pre_process.end(); i++)
            if (i->type == type) {
                pre_process.erase(i);
                break;
            }
    }

    reflect_refract_resolution_mode RenderManager::get_reflect_resolution_mode() {
        return (reflect_refract_resolution_mode)tex_index[0];
    }

    reflect_refract_resolution_mode RenderManager::get_refract_resolution_mode() {
        return (reflect_refract_resolution_mode)tex_index[1];
    }

    RenderTexture& RenderManager::get_render_texture(int32_t index) {
        return render_textures[render_manager_render_texture_index_array[index][tex_index[index]]];
    }

    void RenderManager::reset() {
        for (int32_t i = 0; i < RND_PASSID_NUM; i++) {
            pass_sw[i] = true;
            cpu_time[i] = 0.0;
            gpu_time[i] = 0.0;
        }

        set_pass_sw(RND_PASSID_2, false);
        set_pass_sw(RND_PASSID_REFLECT, false);
        set_pass_sw(RND_PASSID_REFRACT, false);
        set_pass_sw(RND_PASSID_PRE_PROCESS, false);
        set_pass_sw(RND_PASSID_SHOW_VECTOR, false);

        shadow_ptr = 0;
        sync_gpu = false;
        time.get_timestamp();

        shadow = true;
        opaque_z_sort = true;
        alpha_z_sort = true;
        field_11F = false;
        field_120 = false;

        reflect_blur_num = 1;
        reflect_blur_filter = BLUR_FILTER_4;
        show_ref_map = false;

        reflect_type = STAGE_DATA_REFLECT_DISABLE;
        check_state = false;
        show_vector_flags = 0;
        show_vector_length = 0.05f;
        show_vector_z_offset = 0.0f;
        show_stage_shadow = false;
        effect_texture = 0;

        reflect = false;
        refract = false;
        npr_param = 0;
        npr_mask = false;
        reflect_texture_mask = false;
        reflect_tone_curve = false;
        field_31F = false;
        light_stage_ambient = false;

        for (bool& i : draw_pass_3d)
            i = true;

        field_11E = false;
        clear = false;

        width = 0;
        height = 0;
        multisample_framebuffer = 0;
        multisample_renderbuffer = 0;
        multisample = false;//true;

        npr = false;
    }

    void RenderManager::resize(int32_t width, int32_t height) {
        if (this->width == width && this->height == height)
            return;

        this->width = width;
        this->height = height;

#ifdef USE_OPENGL
        if (!Vulkan::use) {
            if (!multisample_framebuffer)
                glGenFramebuffers(1, &multisample_framebuffer);

            if (!multisample_renderbuffer)
                glGenRenderbuffers(1, &multisample_renderbuffer);

            glBindFramebuffer(GL_FRAMEBUFFER, multisample_framebuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, multisample_renderbuffer);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGBA8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, multisample_renderbuffer);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
#endif
    }

    void RenderManager::set_clear(bool value) {
        clear = value;
    }

    void RenderManager::set_effect_texture(texture* value) {
        effect_texture = value;
    }

    void RenderManager::set_multisample(bool value) {
        multisample = value;
    }

    void RenderManager::set_npr_param(int32_t value) {
        npr_param = value;
    }

    void RenderManager::set_pass_sw(RenderPassID id, bool value) {
        pass_sw[id] = value;
    }

    void RenderManager::set_reflect(bool value) {
        reflect = value;
    }

    void RenderManager::set_reflect_blur(int32_t reflect_blur_num, blur_filter_mode reflect_blur_filter) {
        this->reflect_blur_num = reflect_blur_num;
        this->reflect_blur_filter = reflect_blur_filter;
    }

    void RenderManager::set_reflect_resolution_mode(reflect_refract_resolution_mode mode) {
        tex_index[0] = mode;
    }

    void RenderManager::set_reflect_type(int32_t type) {
        reflect_type = type;
    }

    void RenderManager::set_refract(bool value) {
        refract = value;
    }

    void RenderManager::set_refract_resolution_mode(reflect_refract_resolution_mode mode) {
        tex_index[1] = mode;
    }

    void RenderManager::set_shadow_false() {
        shadow = false;
    }

    void RenderManager::set_shadow_true() {
        shadow = true;
    }

    void RenderManager::render_all() {
        gl_state.get();
        sprite_manager_pre_draw();
        rctx_ptr->etc_obj_manager->pre_draw();

        {
            render_data_context rend_data_ctx(GL_REND_STATE_PRE_3D);
            rend_data_ctx.state.get();
            rend_data_ctx.state.bind_framebuffer(0);
            static const vec4 color_clear = 0.0f;
            rend_data_ctx.state.clear_buffer(GL_COLOR, 0, (float_t*)&color_clear);
            rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            rend_data_ctx.state.enable_depth_test();
            rend_data_ctx.state.set_depth_func(GL_LEQUAL);
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
        }

        {
            render_data_context rend_data_ctx(GL_REND_STATE_PRE_3D);
            rend_data_ctx.state.begin_event("rndpass_render_all_pass::Caller::execute_pre3d");
            for (int32_t i = RND_PASSID_SHADOW; i <= RND_PASSID_CLEAR; i++)
                RenderManager::render_single_pass(rend_data_ctx, (RenderPassID)i);
            rend_data_ctx.state.end_event();
            rend_data_ctx.state.finish();
        }

        {
            render_data_context rend_data_ctx(GL_REND_STATE_3D);
            rend_data_ctx.state.get();
            rend_data_ctx.state.begin_event("rndpass_render_all_pass::Caller::execute_3d");
            for (int32_t i = RND_PASSID_PRE_SPRITE; i <= RND_PASSID_3D; i++)
                RenderManager::render_single_pass(rend_data_ctx, (RenderPassID)i);
            rend_data_ctx.state.end_event();
            rend_data_ctx.state.finish();
        }

        {
            render_data_context rend_data_ctx(GL_REND_STATE_2D);
            rend_data_ctx.state.get();
            rend_data_ctx.state.begin_event("rndpass_render_all_pass::Caller::execute_2d");
            for (int32_t i = RND_PASSID_SHOW_VECTOR; i <= RND_PASSID_12; i++)
                RenderManager::render_single_pass(rend_data_ctx, (RenderPassID)i);
            rend_data_ctx.state.end_event();
            rend_data_ctx.state.finish();
        }

        rctx_ptr->etc_obj_manager->post_draw();
        sprite_manager_post_draw();

        render_data_context rend_data_ctx(GL_REND_STATE_POST_2D);
        rend_data_ctx.state.get();
        rend_data_ctx.state.bind_vertex_array(0);
        rend_data_ctx.state.disable_primitive_restart();
        rend_data_ctx.state.bind_uniform_buffer(0);
        rend_data_ctx.state.bind_uniform_buffer_base(0, 0);
        rend_data_ctx.state.bind_uniform_buffer_base(1, 0);
        rend_data_ctx.state.bind_uniform_buffer_base(2, 0);
        rend_data_ctx.state.bind_uniform_buffer_base(3, 0);
        rend_data_ctx.state.bind_uniform_buffer_base(4, 0);
        rend_data_ctx.state.bind_shader_storage_buffer(0);
        rend_data_ctx.state.bind_shader_storage_buffer_base(0, 0);
    }

    void RenderManager::rndpass_post_proc() {
        render_data_context rend_data_ctx(GL_REND_STATE_POST_2D);
        rend_data_ctx.state.begin_event("rndpass_post_proc");
        rctx_ptr->post_proc();
        render->post_proc();
        npr_mask = false;
        rend_data_ctx.state.end_event();
    }

    void RenderManager::rndpass_pre_proc() {
        render_data_context rend_data_ctx(GL_REND_STATE_PRE_3D);
        rend_data_ctx.state.begin_event("rndpass_pre_proc");
        render->pre_proc(rend_data_ctx);
        Glitter::glt_particle_manager->CalcDisp();
        rctx_ptr->pre_proc();
        rend_data_ctx.state.end_event();
    }

    void RenderManager::render_single_pass(render_data_context& rend_data_ctx, RenderPassID id) {
        cpu_time[id] = 0.0;
        gpu_time[id] = 0.0;
        if (!pass_sw[id]) {
            gl_get_error_print();
            return;
        }

        render_pass_begin();
        switch (id) {
        case RND_PASSID_SHADOW:
            pass_shadow(rend_data_ctx);
            break;
        case RND_PASSID_SS_SSS:
            pass_ss_sss(rend_data_ctx);
            break;
        case RND_PASSID_REFLECT:
            pass_reflect(rend_data_ctx);
            break;
        case RND_PASSID_REFRACT:
            pass_refract(rend_data_ctx);
            break;
        case RND_PASSID_PRE_PROCESS:
            pass_pre_process(rend_data_ctx);
            break;
        case RND_PASSID_CLEAR:
            pass_clear(rend_data_ctx);
            break;
        case RND_PASSID_PRE_SPRITE:
            pass_pre_sprite(rend_data_ctx);
            break;
        case RND_PASSID_3D:
            pass_3d(rend_data_ctx);
            break;
        case RND_PASSID_SHOW_VECTOR:
            pass_show_vector(rend_data_ctx);
            break;
        case RND_PASSID_POST_PROCESS:
            pass_post_process(rend_data_ctx);
            break;
        case RND_PASSID_SPRITE:
            pass_sprite(rend_data_ctx);
            break;
        }
        render_pass_end(id);
        gl_get_error_print();
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

    void RenderManager::pass_shadow(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        rend_data_ctx.state.begin_event("pass_shadow");
        rend_data_ctx.state.begin_event("texproj");
        cam_data texproj_cam;
        if (rctx->litproj->set(rend_data_ctx, texproj_cam)) {
            rend_data_ctx.set_batch_scene_camera(texproj_cam);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_OPAQUE, texproj_cam);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TRANSPARENT, texproj_cam);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TRANSLUCENT, texproj_cam);

            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
            rend_data_ctx.shader_flags.arr[U_DEPTH] = 0;
            apply_esm_filter(rend_data_ctx, &rctx->litproj->shadow_texture[0],
                &rctx->litproj->shadow_texture[1], 0, 1.5f, 0.01f, true);

            if (litproj_clear(rend_data_ctx, rctx->litproj, texproj_cam)) {
                for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                    rctx->light_set[i].data_set(rend_data_ctx, rctx->face, (light_set_id)i);

                rend_data_ctx.set_batch_scene_camera(cam);
                rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_OPAQUE, cam);
                rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TRANSPARENT, cam);
                rctx->disp_manager->obj_sort(rend_data_ctx, mdl::OBJ_TYPE_TRANSLUCENT, 1, cam);
                rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                rend_data_ctx.state.enable_blend();
                rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TRANSLUCENT, cam);
                rend_data_ctx.state.disable_blend();
                rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
                rend_data_ctx.state.bind_framebuffer(0);
            }
        }
        rend_data_ctx.state.end_event();

        bool make_shadowmap = false;
        int32_t obj_shadow_count[2];
        for (int32_t i = 0; i < 2; i++) {
            obj_shadow_count[i] = rctx->disp_manager->get_obj_count((mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_CHARA + i));
            if (obj_shadow_count[i])
                make_shadowmap = true;
        }

        Shadow* shad = shadow_ptr;
        if (shadow && make_shadowmap) {
            int32_t index[2];
            shad->set_curr_render_textures(index);

            cam_data cam;
            for (int32_t i = 0, j = 0; i < 2; i++) {
                if (!obj_shadow_count[i])
                    continue;

                draw_pass_shadow_begin_make_shadowmap(shad, rend_data_ctx, cam, index[i], j);
                rend_data_ctx.set_batch_scene_camera(cam);
                rctx->disp_manager->draw(rend_data_ctx, (mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_CHARA + index[i]), cam);
                if (rctx->disp_manager->get_obj_count(
                    (mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_OBJECT_CHARA + index[i])) > 0) {
                    rend_data_ctx.state.set_color_mask(show_stage_shadow
                        ? GL_TRUE : GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    rctx->disp_manager->draw(rend_data_ctx,
                        (mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_OBJECT_CHARA + i), cam);
                    rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                }
                draw_pass_shadow_end_make_shadowmap(shad, rend_data_ctx, index[i], j);
                j++;
            }
        }
        else {
            shad->set_curr_render_textures(0);
            shad->clear_textures(rend_data_ctx.state);
        }
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.bind_framebuffer(0);
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_ss_sss(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        ::sss_data* sss = rctx->sss_data;
        if (!sss->init_data || !sss->enable) {
            sss_param = 0.0f;
            sss_param_reflect = 0.0f;
            return;
        }

        rend_data_ctx.state.begin_event("pass_ss_sss");
        rndr::Render* rend = render;
        extern bool reflect_full;
        if ((reflect_full || sv_better_reflect && reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
            && pass_sw[rndr::RND_PASSID_REFLECT] && rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_SSS)) {
            rend_data_ctx.state.begin_event("reflect");
            reflect_draw = true;

            RenderTexture& refl_tex = rctx->render_manager->get_render_texture(0);
            if (sss->set(rend_data_ctx, refl_tex)) {
                rndr::Render* rend = render;

                for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                    rctx->light_set[i].data_set(rend_data_ctx, rctx->face, (light_set_id)i);

                if (shadow)
                    draw_pass_3d_shadow_set(rend_data_ctx, shadow_ptr, rctx);
                else
                    draw_pass_3d_shadow_reset(rend_data_ctx, rctx);

                cam_data reflect_cam = cam;
                set_reflect_mat(rend_data_ctx, reflect_cam);
                rend_data_ctx.set_batch_scene_camera(reflect_cam);

                rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SSS_SKIN;
                rend_data_ctx.state.enable_depth_test();
                rend_data_ctx.state.set_depth_func(GL_LEQUAL);
                rend_data_ctx.state.set_depth_mask(GL_TRUE);
                rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SSS, reflect_cam);
                rend_data_ctx.state.disable_depth_test();
                rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
                draw_pass_3d_shadow_reset(rend_data_ctx, rctx);
                rend_data_ctx.shader_flags.arr[U_NPR] = 0;

                if (npr_param == 1) {
                    if (sss->enable && sss->downsample) {
                        rend_data_ctx.shader_flags.arr[U_NPR] = 1;
                        rend->draw_sss_contour(rend_data_ctx, reflect_cam);
                    }
                    else if (npr) {
                        refl_tex.Bind(rend_data_ctx.state);
                        refl_tex.SetViewport(rend_data_ctx.state);
                        rend_data_ctx.state.clear(GL_DEPTH_BUFFER_BIT);
                        rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SSS_SKIN;
                        rend_data_ctx.state.enable_depth_test();
                        rend_data_ctx.state.set_depth_mask(GL_TRUE);
                        rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SSS, reflect_cam);
                        rend_data_ctx.state.disable_depth_test();
                        rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
                        rend_data_ctx.state.bind_framebuffer(0);
                        rend_data_ctx.shader_flags.arr[U_NPR] = 1;
                        rend->draw_sss_contour(rend_data_ctx, reflect_cam);
                    }
                }

                sss->apply_filter(rend_data_ctx);
                sss->reset(rend_data_ctx);
            }

            rend_data_ctx.set_npr(this);
            reflect_draw = false;
            rend_data_ctx.state.end_event();
        }

        if (sss->set(rend_data_ctx)) {
            for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                rctx->light_set[i].data_set(rend_data_ctx, rctx->face, (light_set_id)i);

            if (shadow)
                draw_pass_3d_shadow_set(rend_data_ctx, shadow_ptr, rctx);
            else
                draw_pass_3d_shadow_reset(rend_data_ctx, rctx);

            rend_data_ctx.set_batch_scene_camera(cam);

            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SSS_SKIN;
            rend_data_ctx.state.enable_depth_test();
            rend_data_ctx.state.set_depth_func(GL_LEQUAL);
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SSS, cam);
            rend_data_ctx.state.disable_depth_test();
            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
            draw_pass_3d_shadow_reset(rend_data_ctx, rctx);
            rend_data_ctx.shader_flags.arr[U_NPR] = 0;

            if (npr_param == 1) {
                if (sss->enable && sss->downsample) {
                    rend_data_ctx.shader_flags.arr[U_NPR] = 1;
                    rend->draw_sss_contour(rend_data_ctx, cam);
                }
                else if (npr) {
                    rend->rend_texture[0].Bind(rend_data_ctx.state);
                    rend->rend_texture[0].SetViewport(rend_data_ctx.state);
                    rend_data_ctx.state.clear(GL_DEPTH_BUFFER_BIT);
                    rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SSS_SKIN;
                    rend_data_ctx.state.enable_depth_test();
                    rend_data_ctx.state.set_depth_mask(GL_TRUE);
                    rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SSS, cam);
                    rend_data_ctx.state.disable_depth_test();
                    rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
                    rend_data_ctx.state.bind_framebuffer(0);
                    rend_data_ctx.shader_flags.arr[U_NPR] = 1;
                    rend->draw_sss_contour(rend_data_ctx, cam);
                }
            }

            sss->apply_filter(rend_data_ctx);
            sss->reset(rend_data_ctx);
        }

        rend_data_ctx.set_npr(this);
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_reflect(render_data_context& rend_data_ctx) {
        extern bool reflect_full;
        if (reflect_full) {
            draw_pass_reflect_full(rend_data_ctx, this);
            return;
        }

        render_context* rctx = rctx_ptr;
        rend_data_ctx.state.begin_event("pass_reflect");
        RenderTexture& refl_tex = rctx->render_manager->get_render_texture(0);
        RenderTexture& refl_buf_tex = rctx->reflect_buffer;
        refl_tex.Bind(rend_data_ctx.state);
        if (rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_OPAQUE)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSPARENT)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSLUCENT)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_OPAQUE)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_TRANSPARENT)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT)) {
            refl_tex.SetViewport(rend_data_ctx.state);
            rend_data_ctx.set_batch_scene_camera(cam);

            for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                rctx->light_set[i].data_set(rend_data_ctx, rctx->face, (light_set_id)i);
            for (int32_t i = FOG_DEPTH; i < FOG_BUMP; i++)
                rctx->fog[i].data_set(rend_data_ctx, (fog_id)i);

            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            if (sv_better_reflect && rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_SSS))
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            else
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_S_REFL;

            light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
            light_clip_plane clip_plane;
            set->lights[LIGHT_SPOT].get_clip_plane(clip_plane);
            rend_data_ctx.shader_flags.arr[U_REFLECT] = 2;
            rend_data_ctx.shader_flags.arr[U_CLIP_PLANE] = clip_plane.data[1] ? 1 : 0;

            rend_data_ctx.state.enable_depth_test();
            rend_data_ctx.state.set_depth_func(GL_LEQUAL);
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_OPAQUE, cam, 0, reflect_texture_mask);
            if (reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP) {
                rend_data_ctx.state.enable_blend();
                rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                rend_data_ctx.state.set_blend_equation(GL_FUNC_ADD);
                rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_TRANSLUCENT, cam);
                rend_data_ctx.state.disable_blend();
            }

            if (sv_better_reflect && reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP) {
                reflect_draw = true;
                rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;

                if (shadow)
                    draw_pass_3d_shadow_set(rend_data_ctx, shadow_ptr, rctx);
                else
                    draw_pass_3d_shadow_reset(rend_data_ctx, rctx);

                if (effect_texture)
                    rend_data_ctx.state.active_bind_texture_2d(14, effect_texture->glid);
                else
                    rend_data_ctx.state.active_bind_texture_2d(14, rctx->empty_texture_2d->glid);

                rend_data_ctx.shader_flags.arr[U_WATER_REFLECT] = 0;

                rctx->sss_data->set_texture(rend_data_ctx.state, 3);

                cam_data reflect_cam = cam;
                set_reflect_mat(rend_data_ctx, reflect_cam);
                rend_data_ctx.set_batch_scene_camera(reflect_cam);

                rend_data_ctx.shader_flags.arr[U_REFLECT] = 1;
                if (draw_pass_3d[DRAW_PASS_3D_OPAQUE]) {
                    rend_data_ctx.state.enable_depth_test();
                    rend_data_ctx.state.set_depth_mask(GL_TRUE);
                    rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_CHARA_OPAQUE, reflect_cam);
                    rend_data_ctx.state.disable_depth_test();
                }

                if (draw_pass_3d[DRAW_PASS_3D_TRANSPARENT]) {
                    rend_data_ctx.state.enable_depth_test();
                    rend_data_ctx.state.set_depth_mask(GL_TRUE);
                    rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_CHARA_TRANSPARENT, reflect_cam);
                    rend_data_ctx.state.disable_depth_test();
                }

                if (npr_param == 1)
                    pass_3d_contour(rend_data_ctx);

                if (draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]) {
                    rend_data_ctx.state.enable_blend();
                    rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    rend_data_ctx.state.enable_depth_test();
                    rend_data_ctx.state.set_depth_mask(GL_FALSE);
                    rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT, reflect_cam);
                    rend_data_ctx.state.disable_depth_test();
                    rend_data_ctx.state.disable_blend();
                }

                if (shadow)
                    draw_pass_3d_shadow_reset(rend_data_ctx, rctx);
                reflect_draw = false;
            }
            else if (reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP) {
                rend_data_ctx.shader_flags.arr[U_REFLECT] = reflect_tone_curve ? 1 : 0;
                rend_data_ctx.shader_flags.arr[U_CLIP_PLANE] = clip_plane.data[0] ? 1 : 0;
                rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_CHARA_OPAQUE, cam);
            }
            rend_data_ctx.state.disable_depth_test();
            rend_data_ctx.shader_flags.arr[U_REFLECT] = 0;
            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;

            for (int32_t i = reflect_blur_num, j = 0; i > 0; i--, j++) {
                apply_blur_filter_sub(rend_data_ctx, &refl_buf_tex, &refl_tex,
                    reflect_blur_filter, 1.0f, 1.0f, 0.0f);
                image_filter_scale(rend_data_ctx, &refl_tex, refl_buf_tex.color_texture);
            }
        }
        else {
            vec4 clear_color;
            rend_data_ctx.state.get_clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            rend_data_ctx.state.clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        }
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.bind_framebuffer(0);
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_refract(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        rend_data_ctx.state.begin_event("pass_refract");
        RenderTexture& refract_texture = rctx->render_manager->get_render_texture(1);
        refract_texture.Bind(rend_data_ctx.state);
        if (rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFRACT_OPAQUE)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFRACT_TRANSPARENT)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFRACT_TRANSLUCENT)) {
            refract_texture.SetViewport(rend_data_ctx.state);
            rend_data_ctx.set_batch_scene_camera(cam);

            for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                rctx->light_set[i].data_set(rend_data_ctx, rctx->face, (light_set_id)i);
            for (int32_t i = FOG_DEPTH; i < FOG_BUMP; i++)
                rctx->fog[i].data_set(rend_data_ctx, (fog_id)i);

            vec4 clear_color;
            rend_data_ctx.state.get_clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            rend_data_ctx.state.clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_S_REFR;
            rend_data_ctx.state.enable_depth_test();
            rend_data_ctx.state.set_depth_func(GL_LEQUAL);
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFRACT_OPAQUE, cam);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFRACT_TRANSPARENT, cam);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFRACT_TRANSLUCENT, cam);
            rend_data_ctx.state.disable_depth_test();
            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
        }
        else {
            vec4 clear_color;
            rend_data_ctx.state.get_clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            rend_data_ctx.state.clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        }
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.bind_framebuffer(0);
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_pre_process(render_data_context& rend_data_ctx) {
        rend_data_ctx.state.begin_event("pass_pre_process");
        for (draw_pre_process& i : pre_process)
            if (i.func)
                i.func(rend_data_ctx, i.data, cam);
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.bind_framebuffer(0);
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_clear(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        rend_data_ctx.state.begin_event("pass_clear");
        if (clear) {
            rctx->screen_buffer.Bind(rend_data_ctx.state);
            clear_color_set_gl(rend_data_ctx.state);
            //rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            rend_data_ctx.state.bind_framebuffer(0);
        }

        rndr::Render* rend = render;
        if (!rctx->sss_data->enable || !rctx->sss_data->downsample) {
            rend->bind_render_texture(rend_data_ctx.state);
            if (sprite_manager_get_reqlist_count(2)) {
                rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
                rend_data_ctx.state.clear_depth(1.0f);
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                rend_data_ctx.state.bind_framebuffer(0);
            }
            else {
                clear_color_set_gl(rend_data_ctx.state);
                rend_data_ctx.state.clear_depth(1.0f);
                if (clear)
                    rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                else
                    rend_data_ctx.state.clear(GL_DEPTH_BUFFER_BIT);
                rend_data_ctx.state.bind_framebuffer(0);
            }
        }
        else {
            if (sprite_manager_get_reqlist_count(2)) {
                rend->bind_render_texture(rend_data_ctx.state);
                rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
                rend_data_ctx.state.bind_framebuffer(0);
            }
            else if (clear) {
                rend->bind_render_texture(rend_data_ctx.state);
                clear_color_set_gl(rend_data_ctx.state);
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
                rend_data_ctx.state.bind_framebuffer(0);
            }
        }

        gl_get_error_print();
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_pre_sprite(render_data_context& rend_data_ctx) {
        if (!sprite_manager_get_reqlist_count(2))
            return;

        render_context* rctx = rctx_ptr;

        rend_data_ctx.state.begin_event("pass_pre_sprite");
        rndr::Render* rend = render;
        rend->bind_render_texture(rend_data_ctx.state, true);
        rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
        rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
        rend_data_ctx.state.set_depth_mask(GL_FALSE);
        rend_data_ctx.state.disable_depth_test();
        rend_data_ctx.state.enable_blend();
        rend_data_ctx.state.disable_cull_face();
        sprite_manager_draw(rend_data_ctx, 2, true,
            rend->temp_buffer.color_texture,
            rctx->camera->view_projection_aet_3d);
        rend_data_ctx.state.enable_cull_face();
        rend_data_ctx.state.disable_blend();
        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.bind_framebuffer(0);
        gl_get_error_print();
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_3d(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        render->bind_render_texture(rend_data_ctx.state);
        if (!rctx->sss_data->enable || !rctx->sss_data->downsample
            || draw_pass_3d_get_translucent_count(rctx)) {
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            rend_data_ctx.state.clear(GL_DEPTH_BUFFER_BIT);
        }

        rend_data_ctx.state.set_depth_func(GL_LEQUAL);

        rend_data_ctx.set_batch_scene_camera(cam);
        for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
            rctx->light_set[i].data_set(rend_data_ctx, rctx->face, (light_set_id)i);
        for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++)
            rctx->fog[i].data_set(rend_data_ctx, (fog_id)i);

        if (shadow)
            draw_pass_3d_shadow_set(rend_data_ctx, shadow_ptr, rctx);
        else
            draw_pass_3d_shadow_reset(rend_data_ctx, rctx);

        if (effect_texture)
            rend_data_ctx.state.active_bind_texture_2d(14, effect_texture->glid);
        else
            rend_data_ctx.state.active_bind_texture_2d(14, rctx->empty_texture_2d->glid);

        if (pass_sw[RND_PASSID_REFLECT] && reflect) {
            RenderTexture& refl_tex = get_render_texture(0);
            rend_data_ctx.state.active_bind_texture_2d(15, refl_tex.GetColorTex());
            rend_data_ctx.shader_flags.arr[U_WATER_REFLECT] = 1;
        }
        else {
            rend_data_ctx.state.active_bind_texture_2d(15, rctx->empty_texture_2d->glid);
            rend_data_ctx.shader_flags.arr[U_WATER_REFLECT] = 0;
        }

        rctx->sss_data->set_texture(rend_data_ctx.state, 1);

        rend_data_ctx.set_npr(this);
        rend_data_ctx.set_batch_sss_param(sss_param);

        rend_data_ctx.state.bind_sampler(14, rctx->render_samplers[0]);
        rend_data_ctx.state.bind_sampler(15, rctx->render_samplers[0]);
        rend_data_ctx.state.bind_sampler(16, rctx->render_samplers[0]);

        rend_data_ctx.shader_flags.arr[U_STAGE_AMBIENT] = light_stage_ambient ? 1 : 0;

        if (alpha_z_sort) {
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_TRANSLUCENT, 1, cam, field_31F);
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_TRANSLUCENT_SORT_BY_RADIUS, 2, cam);
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_GLITTER, 1, cam, field_31F);
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, 1, cam, field_31F);
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_OPAQUE, 1, cam, field_31F);
        }

        if (opaque_z_sort)
            rctx->disp_manager->obj_sort(rend_data_ctx, mdl::OBJ_TYPE_OPAQUE, 0, cam);

        if (draw_pass_3d[DRAW_PASS_3D_OPAQUE]) {
            rend_data_ctx.state.enable_depth_test();
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_OPAQUE, cam);
            rend_data_ctx.state.disable_depth_test();
        }

        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_OPAQUE, cam);

        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        if (draw_pass_3d[DRAW_PASS_3D_TRANSPARENT])
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TRANSPARENT, cam);
        if (rctx->render_manager->field_120)
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TYPE_7, cam);

        render->calc_exposure_chara_data(rend_data_ctx, cam);

        if (npr_param == 1)
            pass_3d_contour(rend_data_ctx);

        render->draw_lens_flare(rend_data_ctx, cam);
        star_catalog_draw(rend_data_ctx, cam);

        draw_pass_3d_translucent(rend_data_ctx, rctx,
            mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_OPAQUE,
            mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_OPAQUE,
            mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_OPAQUE, cam);

        snow_particle_draw(rend_data_ctx, cam);
        rain_particle_draw(rend_data_ctx, cam);
        leaf_particle_draw(rend_data_ctx, cam);
        particle_draw(rend_data_ctx, cam);
        rend_data_ctx.state.disable_depth_test();

        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_PRE_TRANSLUCENT, cam);
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        /*if (npr_param == 1) {
            rend_data_ctx.state.set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }*/

        rend_data_ctx.state.enable_depth_test();

        if (draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]) {
            rend_data_ctx.state.enable_blend();
            rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            rend_data_ctx.state.set_depth_mask(GL_FALSE);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TRANSLUCENT_SORT_BY_RADIUS, cam);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TRANSLUCENT, cam);
            rend_data_ctx.state.disable_blend();
        }

        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE); // X
        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_POST_TRANSLUCENT, cam);
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        draw_pass_3d_translucent(rend_data_ctx, rctx,
            mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_TRANSLUCENT,
            mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT,
            mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, cam);
        rend_data_ctx.state.disable_depth_test();

        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_NORMAL, cam);
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        draw_pass_3d_translucent(rend_data_ctx, rctx,
            mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_GLITTER,
            mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_GLITTER,
            mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_GLITTER, cam);

        if (Glitter::glt_particle_manager->CheckHasScreenEffect()) { // X
            rend_data_ctx.state.begin_event("screen");

            cam_data screen_cam = cam;
            screen_cam.set_fov(32.2673416137695f * DEG_TO_RAD_FLOAT);
            screen_cam.calc_persp_proj_mat_offset(1.0f, rctx->render.get_taa_offset());
            screen_cam.calc_view_proj_mat();
            rend_data_ctx.set_batch_scene_camera(screen_cam);

            if (alpha_z_sort) {
                rctx->disp_manager->obj_sort(rend_data_ctx,
                    mdl::OBJ_TYPE_SCREEN_TRANSLUCENT, 1, screen_cam);
                rctx->disp_manager->obj_sort(rend_data_ctx,
                    mdl::OBJ_TYPE_SCREEN_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, 1, screen_cam);
            }

            if (opaque_z_sort)
                rctx->disp_manager->obj_sort(rend_data_ctx, mdl::OBJ_TYPE_SCREEN_OPAQUE, 0, screen_cam);

            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SCREEN_OPAQUE, screen_cam);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SCREEN_TRANSPARENT, screen_cam);
            rend_data_ctx.state.enable_blend();
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SCREEN_TRANSLUCENT, screen_cam);
            rend_data_ctx.state.disable_blend();

            draw_pass_3d_translucent(rend_data_ctx, rctx,
                mdl::OBJ_TYPE_SCREEN_OPAQUE_ALPHA_ORDER_POST_TRANSLUCENT,
                mdl::OBJ_TYPE_SCREEN_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT,
                mdl::OBJ_TYPE_SCREEN_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, cam);

            rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
            Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_SCREEN, screen_cam);
            rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

            rend_data_ctx.state.end_event();

            rend_data_ctx.set_batch_scene_camera(cam);
        }

        rend_data_ctx.state.active_bind_texture_2d(14, 0);
        rend_data_ctx.state.active_bind_texture_2d(15, 0);

        rend_data_ctx.state.disable_depth_test();

        if (shadow)
            draw_pass_3d_shadow_reset(rend_data_ctx, rctx);
        pass_sprite_surf(rend_data_ctx);
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.bind_framebuffer(0);
    }

    void RenderManager::pass_show_vector(render_data_context& rend_data_ctx) {
        if (show_vector_flags) {
            rend_data_ctx.state.begin_event("pass_show_vector");
            rend_data_ctx.state.end_event();
        }
    }

    void RenderManager::pass_post_process(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        rend_data_ctx.state.begin_event("pass_post_process");

        rend_data_ctx.set_npr(this);

        texture* light_proj_tex = 0;
        light_proj* litproj = rctx->litproj;
        if (litproj && litproj->enable) {
            light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
            if (set->lights[LIGHT_PROJECTION].get_type() == LIGHT_SPOT
                && texture_manager_get_texture(litproj->texture_id))
                light_proj_tex = litproj->draw_texture.color_texture;
        }

        render->apply_post_process(rend_data_ctx, cam, light_proj_tex, npr_param);
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_sprite(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        if (!sprite_manager_get_reqlist_count(0))
            return;

        rend_data_ctx.state.begin_event("pass_sprite");
        rndr::Render* rend = render;
        rend_data_ctx.state.set_viewport(0, 0, width, height);

        if (Vulkan::use)
            rctx->screen_buffer.Bind(rend_data_ctx.state);
#ifdef USE_OPENGL
        else if (multisample && multisample_framebuffer) {
            rend_data_ctx.state.bind_framebuffer(multisample_framebuffer);
            rend_data_ctx.state.enable_multisample();
            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
        }
        else
            rctx->screen_buffer.Bind(rend_data_ctx.state);
#endif

        rend_data_ctx.state.disable_depth_test();
        rend_data_ctx.state.enable_blend();
        rend_data_ctx.state.disable_cull_face();
        sprite_manager_draw(rend_data_ctx, 0, true,
            rctx->screen_overlay_buffer.color_texture,
            rctx->camera->view_projection_aet_2d);
        rend_data_ctx.state.enable_cull_face();
        rend_data_ctx.state.disable_blend();
        rend_data_ctx.state.enable_depth_test();

#ifdef USE_OPENGL
        if (!Vulkan::use && multisample && multisample_framebuffer) {
            rend_data_ctx.state.bind_framebuffer(rctx->screen_buffer.fbos[0]);
            rend_data_ctx.state.disable_multisample();
            rend_data_ctx.state.bind_read_framebuffer(multisample_framebuffer);
            rend_data_ctx.state.blit_framebuffer(0, 0, width, height,
                0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            rend_data_ctx.state.bind_read_framebuffer(0);
        }
#endif
        gl_get_error_print();
        rend_data_ctx.state.bind_framebuffer(0);
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_3d_contour(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        RenderTexture* rt;
        RenderTexture* contour_rt;
        if (reflect_draw) {
            rt = &rctx->render_manager->get_render_texture(0);
            contour_rt = &rctx->reflect_buffer;
    }
        else {
            rt = &render->rend_texture[0];
            contour_rt = render->sss_contour_texture;
        }

        rend_data_ctx.state.begin_event("`anonymous-namespace'::draw_npr_frame");
        rend_data_ctx.state.enable_blend();
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rend_data_ctx.state.set_blend_equation(GL_FUNC_ADD);
        rend_data_ctx.state.disable_depth_test();

        quad_shader_data quad_data = {};
        quad_data.g_texcoord_modifier = { 0.5f, 0.5f, 0.5f, 0.5f };
        rctx->quad_ubo.WriteMemory(rend_data_ctx.state, quad_data);

        contour_params_shader_data contour_params_data = {};
        const double_t max_distance = cam.get_max_distance();
        const double_t min_distance = cam.get_min_distance();
        contour_params_data.g_near_far = {
            (float_t)(max_distance * (1.0 / (max_distance - min_distance))),
            (float_t)(-(max_distance * min_distance) * (1.0 / (max_distance - min_distance))),
            (float_t)min_distance, (float_t)max_distance
        };
        rctx->contour_params_ubo.WriteMemory(rend_data_ctx.state, contour_params_data);

        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_CONTOUR_NPR);
        rend_data_ctx.state.active_bind_texture_2d(14, rt->GetDepthTex());
        rend_data_ctx.state.active_bind_texture_2d(16, contour_rt->GetColorTex());
        rend_data_ctx.state.active_bind_texture_2d(17, contour_rt->GetDepthTex());
        rend_data_ctx.state.bind_sampler(14, rctx->render_samplers[1]);
        rend_data_ctx.state.bind_sampler(16, rctx->render_samplers[1]);
        rend_data_ctx.state.bind_sampler(17, rctx->render_samplers[1]);
        rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->quad_ubo);
        rend_data_ctx.state.bind_uniform_buffer_base(2, rctx->contour_params_ubo);
        rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
        rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

        if (effect_texture)
            rend_data_ctx.state.active_bind_texture_2d(14, effect_texture->glid);
        else
            rend_data_ctx.state.active_bind_texture_2d(14, rctx->empty_texture_2d->glid);
        rend_data_ctx.state.bind_sampler(14, rctx->render_samplers[0]);
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.disable_blend();
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_sprite_surf(render_data_context& rend_data_ctx) {
        if (!sprite_manager_get_reqlist_count(1))
            return;

        render_context* rctx = rctx_ptr;
        rndr::Render* rend = render;

        rend_data_ctx.state.set_depth_mask(GL_FALSE);
        rend_data_ctx.state.disable_depth_test();
        rend_data_ctx.state.enable_blend();
        rend_data_ctx.state.disable_cull_face();
        sprite_manager_draw(rend_data_ctx, 1, true,
            rend->temp_buffer.color_texture,
            rctx->camera->view_projection_aet_2d);
    }
}

void image_filter_scale(render_data_context& rend_data_ctx,
    RenderTexture* dst, texture* src, const vec4& scale) {
    if (!dst || !dst->color_texture->glid || !dst->color_texture || !src || !src->glid)
        return;

    render_context* rctx = rctx_ptr;

    rend_data_ctx.state.begin_event("`anonymous-namespace'::Impl::apply_no_filter_sub");

    dst->Bind(rend_data_ctx.state);
    dst->SetViewport(rend_data_ctx.state);

    filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = { 1.0f, 1.0f, 0.0f, 0.0f };
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rctx->filter_scene_ubo.WriteMemory(rend_data_ctx.state, filter_scene);

    imgfilter_batch_shader_data imgfilter_batch = {};
    imgfilter_batch.g_color_scale = scale;
    imgfilter_batch.g_color_offset = 0.0f;
    imgfilter_batch.g_texture_lod = 0.0f;
    rctx->imgfilter_batch_ubo.WriteMemory(rend_data_ctx.state, imgfilter_batch);

    rend_data_ctx.shader_flags.arr[U_IMAGE_FILTER] = 5;
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_IMGFILT);
    rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->filter_scene_ubo);
    rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->imgfilter_batch_ubo);
    rend_data_ctx.state.active_bind_texture_2d(0, src->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    rend_data_ctx.state.end_event();
}

void render_manager_init_data(int32_t ssaa, int32_t hd_res, int32_t ss_alpha_mask, bool npr) {
    rndr::RenderManager& render_manager = *rctx_ptr->render_manager;

    render_manager.reset();
    render_manager.npr = npr;
    render_texture_counter_reset();

    resolution_struct* res_wind = res_window_get();
    resolution_struct* res_wind_int = res_window_internal_get();
    render_manager.render = &rctx_ptr->render;
    render_manager.render->init_render_buffers(res_wind_int->width,
        res_wind_int->height, ssaa, hd_res, ss_alpha_mask);
    render_manager.render->set_screen_res(res_wind_int->x_offset,
        res_wind_int->y_offset, res_wind_int->width, res_wind_int->height);
    render_manager.width = res_wind->width;
    render_manager.height = res_wind->height;
    render_manager_init_render_textures(1);
    render_manager.render->init_post_process_buffers();

    shadow_ptr_init();
    render_manager.shadow_ptr = shadow_ptr_get();
    render_manager.shadow_ptr->init();

    rctx_ptr->sss_data->init();
    gl_get_error_print();
}

void render_manager_free_data() {
    rndr::RenderManager& render_manager = *rctx_ptr->render_manager;

    render_manager_free_render_textures();
    render_manager.render->free();
    shadow_ptr_free();
    rctx_ptr->sss_data->free();
}

static void draw_pass_shadow_begin_make_shadowmap(Shadow* shad,
    render_data_context& rend_data_ctx, cam_data& cam, int32_t index, int32_t a3) {
    render_context* rctx = rctx_ptr;
    shad->curr_render_textures[0]->Bind(rend_data_ctx.state);
    texture* tex = shad->curr_render_textures[0]->color_texture;
    rend_data_ctx.state.set_viewport(1, 1, tex->width - 2, tex->width - 2);
    rend_data_ctx.state.enable_depth_test();
    rend_data_ctx.state.clear_color(1.0f, 1.0f, 1.0f, 1.0f);
    if (!a3)
        rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else if (shad->separate)
        rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);

    float_t shadow_range = shad->get_shadow_range();
    float_t offset;
    vec3* interest;
    vec3* view_point;
    if (shad->separate) {
        offset = index ? 0.5f : -0.5f;
        interest = &shad->interest[index];
        view_point = &shad->view_point[index];
    }
    else {
        offset = 0.0f;
        interest = &shad->interest_shared;
        view_point = &shad->view_point_shared;
    }

    const float_t scale = (float_t)(tex->width / 2) / (float_t)((tex->width - 2) / 2);
    shad->calc_proj_view_mat(cam, *view_point, *interest, shadow_range, offset, scale);
    rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SIL;
    rend_data_ctx.shader_flags.arr[U_DEPTH] = 0;
}

static void draw_pass_shadow_end_make_shadowmap(Shadow* shad,
    render_data_context& rend_data_ctx, int32_t index, int32_t a3) {
    render_context* rctx = rctx_ptr;
    rend_data_ctx.state.disable_depth_test();

    rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
    if (a3 == shad->num_shadow - 1) {
        apply_esm_filter(rend_data_ctx, &shad->render_textures[3],
            &shad->render_textures[4], shad->curr_render_textures[0],
            shad->field_2DC, shad->field_2E0 / (shad->z_half_range * 2.0f), false);
        apply_esm_min_filter(rend_data_ctx, &shad->render_textures[5],
            &shad->render_textures[6], &shad->render_textures[3]);
    }

    RenderTexture* rend_tex = shad->curr_render_textures[1 + index];
    RenderTexture& rend_buf_tex = rctx_ptr->shadow_buffer;

    image_filter_scale(rend_data_ctx, rend_tex,
        shad->curr_render_textures[0]->color_texture);

    if (shad->blur_filter_enable[index]) {
        for (int32_t i = shad->near_blur, j = 0; i > 0; i--, j++) {
            apply_blur_filter_sub(rend_data_ctx, &rend_buf_tex, rend_tex,
                shad->blur_filter, 1.0f, 1.0f, 0.0f);
            image_filter_scale(rend_data_ctx, rend_tex, rend_buf_tex.color_texture);
        }
    }
    else {
        rend_buf_tex.Bind(rend_data_ctx.state);
        rend_data_ctx.state.clear_color(1.0f, 1.0f, 1.0f, 1.0f);
        rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
    }

    rend_data_ctx.state.bind_framebuffer(0);
    if (GLAD_GL_VERSION_4_5)
        rend_data_ctx.state.generate_texture_mipmap(rend_tex->GetColorTex());
    else {
        rend_data_ctx.state.bind_texture_2d(rend_tex->GetColorTex());
        rend_data_ctx.state.generate_mipmap(GL_TEXTURE_2D);
        rend_data_ctx.state.bind_texture_2d(0);
    }
}

static void apply_esm_filter(render_data_context& rend_data_ctx,
    RenderTexture* dst, RenderTexture* buf, RenderTexture* src,
    float_t sigma, float_t far_texel_offset, bool enable_lit_proj) {
    texture* dst_tex = dst->color_texture;
    texture* buf_tex = buf->color_texture;
    texture* src_tex = dst->color_texture;
    if (src)
        src_tex = src->depth_texture;

    if (!dst_tex || !buf_tex || !src_tex || dst_tex->width != buf_tex->width || dst_tex->height != buf_tex->height)
        return;

    render_context* rctx = rctx_ptr;

    rend_data_ctx.state.begin_event("`anonymous-namespace'::Impl::apply_esm_filter");
    filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = 0.0f;
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rctx->filter_scene_ubo.WriteMemory(rend_data_ctx.state, filter_scene);

    esm_filter_batch_shader_data esm_filter_batch = {};
    double_t v6 = 1.0 / (sqrt(M_PI * 2.0) * sigma);
    double_t v8 = -1.0 / (2.0 * sigma * sigma);
    for (int32_t i = 0; i < 8; i++)
        ((float_t*)esm_filter_batch.g_gauss)[i] = (float_t)(exp((double_t)((ssize_t)i * i) * v8) * v6);

    rend_data_ctx.shader_flags.arr[U_LIGHT_PROJ] = enable_lit_proj ? 1 : 0;
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_ESMGAUSS);
    rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->filter_scene_ubo);
    rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->esm_filter_batch_ubo);

    rend_data_ctx.state.set_viewport(0, 0, dst_tex->width, dst_tex->height);

    buf->Bind(rend_data_ctx.state);
    esm_filter_batch.g_params = { 1.0f / (float_t)dst_tex->width, 0.0f, far_texel_offset, far_texel_offset };
    rctx->esm_filter_batch_ubo.WriteMemory(rend_data_ctx.state, esm_filter_batch);

    rend_data_ctx.state.active_bind_texture_2d(0, src_tex->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

    dst->Bind(rend_data_ctx.state);
    esm_filter_batch.g_params = { 0.0f, 1.0f / (float_t)dst_tex->height, far_texel_offset, far_texel_offset };
    rctx->esm_filter_batch_ubo.WriteMemory(rend_data_ctx.state, esm_filter_batch);

    rend_data_ctx.state.active_bind_texture_2d(0, buf_tex->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    shader::unbind(rend_data_ctx.state);
    rend_data_ctx.state.end_event();
}

static void apply_esm_min_filter(render_data_context& rend_data_ctx,
    RenderTexture* dst, RenderTexture* buf, RenderTexture* src) {
    texture* dst_tex = dst->color_texture;
    texture* buf_tex = buf->color_texture;
    texture* src_tex = src->color_texture;
    rend_data_ctx.state.begin_event("`anonymous-namespace'::Impl::apply_esm_min_filter");
    if (!dst_tex || !dst_tex->glid || !buf_tex || !buf_tex->glid || !src_tex || !src_tex->glid) {
        rend_data_ctx.state.end_event();
        return;
    }

    render_context* rctx = rctx_ptr;

    rend_data_ctx.state.begin_event("minimize");
    filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = 0.0f;
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rctx->filter_scene_ubo.WriteMemory(rend_data_ctx.state, filter_scene);

    esm_filter_batch_shader_data esm_filter_batch = {};
    esm_filter_batch.g_gauss[0] = 0.0f;
    esm_filter_batch.g_gauss[1] = 0.0f;

    rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->filter_scene_ubo);
    rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->esm_filter_batch_ubo);

    rend_data_ctx.state.set_viewport(0, 0, dst_tex->width, dst_tex->height);

    buf->Bind(rend_data_ctx.state);
    esm_filter_batch.g_params = { 1.0f / (float_t)src_tex->width,
        1.0f / (float_t)src_tex->height, 0.0f, 0.0f };
    rctx->esm_filter_batch_ubo.WriteMemory(rend_data_ctx.state, esm_filter_batch);

    rend_data_ctx.shader_flags.arr[U_ESM_FILTER] = 0;
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_ESMFILT);
    rend_data_ctx.state.active_bind_texture_2d(0, src_tex->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    rend_data_ctx.state.end_event();

    rend_data_ctx.state.begin_event("erosion");
    dst->Bind(rend_data_ctx.state);
    esm_filter_batch.g_params = { 0.75f / (float_t)buf_tex->width,
        0.75f / (float_t)buf_tex->height, 0.0f, 0.0f };
    rctx->esm_filter_batch_ubo.WriteMemory(rend_data_ctx.state, esm_filter_batch);

    rend_data_ctx.shader_flags.arr[U_ESM_FILTER] = 1;
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_ESMFILT);
    rend_data_ctx.state.active_bind_texture_2d(0, buf_tex->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    rend_data_ctx.state.end_event();
    rend_data_ctx.state.end_event();
}

static bool litproj_clear(render_data_context& rend_data_ctx, light_proj* litproj, cam_data& cam) {
    if (!litproj)
        return false;

    texture* tex = texture_manager_get_texture(litproj->texture_id);
    if (!tex)
        return false;

    litproj->draw_texture.Bind(rend_data_ctx.state);
    litproj->draw_texture.SetViewport(rend_data_ctx.state);
    rend_data_ctx.state.enable_depth_test();
    rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    rend_data_ctx.state.clear_depth(1.0f);
    rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!light_proj::set_mat(rend_data_ctx, cam, true)) {
        rend_data_ctx.state.bind_framebuffer(0);
        return false;
    }

    render_context* rctx = rctx_ptr;
    rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_LITPROJ;
    rend_data_ctx.state.active_bind_texture_2d(17, tex->glid);
    rend_data_ctx.state.bind_sampler(17, rctx->render_samplers[2]);
    rend_data_ctx.state.active_bind_texture_2d(18, litproj->shadow_texture[0].GetColorTex());
    rend_data_ctx.state.bind_sampler(18, rctx->render_samplers[2]);
    rend_data_ctx.state.active_texture(0);
    return true;
}

static int32_t draw_pass_3d_get_translucent_count(render_context* rctx) {
    int32_t count = 0;
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_GLITTER);
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_GLITTER);
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_GLITTER);
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_TRANSLUCENT);
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT);
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT);
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_OPAQUE);
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_OPAQUE);
    count += rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_OPAQUE);
    return count;
}

static void draw_pass_3d_shadow_reset(render_data_context& rend_data_ctx, render_context* rctx) {
    rend_data_ctx.state.active_bind_texture_2d(6, rctx->empty_texture_2d->glid);
    rend_data_ctx.state.active_bind_texture_2d(7, rctx->empty_texture_2d->glid);
    rend_data_ctx.set_self_shadow(false);
    rend_data_ctx.set_shadow(false);
}

static void draw_pass_3d_shadow_set(render_data_context& rend_data_ctx, Shadow* shad, render_context* rctx) {
    float_t esm_param;
    if (shad->self_shadow && shad->num_shadow > 0) {
        rend_data_ctx.state.active_bind_texture_2d(19, shad->render_textures[3].GetColorTex());
        rend_data_ctx.state.active_bind_texture_2d(20, shad->render_textures[5].GetColorTex());
        rend_data_ctx.state.active_texture(0);
        esm_param = (shad->field_2D8 * shad->z_half_range * 2.0f) * 1.442695f;
        rend_data_ctx.set_self_shadow(true);
    }
    else {
        rend_data_ctx.state.active_bind_texture_2d(19, rctx->empty_texture_2d->glid);
        rend_data_ctx.state.active_bind_texture_2d(20, rctx->empty_texture_2d->glid);
        rend_data_ctx.state.active_texture(0);
        esm_param = 0.0f;
        rend_data_ctx.set_self_shadow(false);
    }

    rend_data_ctx.state.bind_sampler(19, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_sampler(20, rctx->render_samplers[0]);

    mat4 mats[2];
    mats[0] = mat4_identity;
    mats[1] = mat4_identity;
    vec4 shadow_ambient;
    vec4 shadow_ambient1;
    if (shad->num_shadow > 0) {
        rctx->draw_state->rend_data[rend_data_ctx.index].shadow = true;
        rend_data_ctx.shader_flags.arr[U_STAGE_SHADOW2] = shad->num_shadow > 1 ? 1 : 0;

        float_t shadow_range = shad->get_shadow_range();
        for (int32_t i = 0; i < 2; i++) {
            float_t offset;
            vec3* interest;
            vec3* view_point;
            if (shad->separate) {
                offset = i ? 0.5f : -0.5f;

                interest = &shad->interest[i];
                view_point = &shad->view_point[i];
            }
            else {
                offset = 0.0f;
                interest = &shad->interest_shared;
                view_point = &shad->view_point_shared;
            }

            mat4 temp;
            mat4_translate(0.5f, 0.5f, 0.5f, &temp);
            mat4_scale_rot(&temp, 0.5f, 0.5f, 0.5f, &temp);
            mat4_mul_translate(&temp, offset, 0.0f, 0.0f, &temp);

            mat4 proj;
            mat4_ortho(-shadow_range, shadow_range,
                -shadow_range, shadow_range, shad->z_near, shad->z_far, &proj);
            mat4_mul(&proj, &temp, &proj);

            mat4 view;
            mat4_look_at(view_point, interest, &view);
            mat4_mul(&view, &proj, &mats[i]);
        }

        int32_t j = 0;
        for (int32_t i = 0; i < 2; i++)
            if (shad->shadow_enable[i]) {
                rend_data_ctx.state.active_bind_texture_2d(6 + j, shad->curr_render_textures[1 + i]->GetColorTex());
                j++;
            }

        for (; j < 2; j++)
            rend_data_ctx.state.active_bind_texture_2d(6 + j, rctx->empty_texture_2d->glid);
        rend_data_ctx.state.active_texture(0);

        light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
        light_data* data = &set->lights[LIGHT_SHADOW];

        vec4 ambient;
        if (data->get_type() == LIGHT_PARALLEL)
            data->get_ambient(ambient);
        else
            *(vec3*)&ambient.x = shad->shadow_ambient;

        shadow_ambient = { ambient.x, ambient.y, ambient.z, 1.0f };
        shadow_ambient1 = { 1.0f - ambient.x, 1.0f - ambient.y, 1.0f - ambient.z, 0.0f };
    }
    else {
        rctx->draw_state->rend_data[rend_data_ctx.index].shadow = false;

        for (int32_t i = 0; i < 2; i++)
            rend_data_ctx.state.active_bind_texture_2d(6 + i, shad->curr_render_textures[1 + i]->GetColorTex());
        rend_data_ctx.state.active_texture(0);

        shadow_ambient = 1.0f;
        shadow_ambient1 = 0.0f;
    }

    rend_data_ctx.set_scene_shadow_params(esm_param, mats, shadow_ambient, shadow_ambient1);

    rend_data_ctx.state.bind_sampler(6, 0);
    rend_data_ctx.state.bind_sampler(7, 0);
}

static void draw_pass_3d_translucent(render_data_context& rend_data_ctx,
    render_context* rctx, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent, cam_data& cam) {
    if (rctx->disp_manager->get_obj_count(opaque) < 1
        && rctx->disp_manager->get_obj_count(transparent) < 1
        && rctx->disp_manager->get_obj_count(translucent) < 1)
        return;

    rndr::Render* rend = &rctx->render;

    RenderTexture& rt = reflect_draw
        ? rctx->render_manager->get_render_texture(0) : rend->rend_texture[0];

    int32_t alpha_array[256];
    int32_t count = draw_pass_3d_translucent_count_layers(rctx,
        alpha_array, opaque, transparent, translucent, cam);
    for (int32_t i = 0; i < count; i++) {
        int32_t alpha = alpha_array[i];
        rend->transparency_copy(rend_data_ctx, &rt);
        if (rctx->render_manager->draw_pass_3d[DRAW_PASS_3D_OPAQUE]
            && rctx->disp_manager->get_obj_count(opaque))
            rctx->disp_manager->draw(rend_data_ctx, opaque, cam, 0, true, alpha);
        if (rctx->render_manager->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT]
            && rctx->disp_manager->get_obj_count(transparent))
            rctx->disp_manager->draw(rend_data_ctx, transparent, cam, 0, true, alpha);
        if (rctx->render_manager->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]
            && rctx->disp_manager->get_obj_count(translucent)) {
            rend_data_ctx.state.enable_blend();
            rctx->disp_manager->draw(rend_data_ctx, translucent, cam, 0, true, alpha);
            rend_data_ctx.state.disable_blend();
        }
        rend->transparency_combine(rend_data_ctx, &rt, (float_t)alpha * (float_t)(1.0 / 255.0));
    }
}

static void draw_pass_3d_translucent(render_data_context& rend_data_ctx,
    render_context* rctx, mdl::ObjTypeScreen opaque,
    mdl::ObjTypeScreen transparent, mdl::ObjTypeScreen translucent, cam_data& cam) {
    if (rctx->disp_manager->get_obj_count(opaque) < 1
        && rctx->disp_manager->get_obj_count(transparent) < 1
        && rctx->disp_manager->get_obj_count(translucent) < 1)
        return;

    rndr::Render* rend = &rctx->render;

    RenderTexture& rt = reflect_draw
        ? rctx->render_manager->get_render_texture(0) : rend->rend_texture[0];

    int32_t alpha_array[256];
    int32_t count = draw_pass_3d_translucent_count_layers(rctx,
        alpha_array, opaque, transparent, translucent, cam);
    for (int32_t i = 0; i < count; i++) {
        int32_t alpha = alpha_array[i];
        rend->transparency_copy(rend_data_ctx, &rt);
        if (rctx->render_manager->draw_pass_3d[DRAW_PASS_3D_OPAQUE]
            && rctx->disp_manager->get_obj_count(opaque))
            rctx->disp_manager->draw(rend_data_ctx, opaque, cam, 0, true, alpha);
        if (rctx->render_manager->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT]
            && rctx->disp_manager->get_obj_count(transparent))
            rctx->disp_manager->draw(rend_data_ctx, transparent, cam, 0, true, alpha);
        if (rctx->render_manager->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]
            && rctx->disp_manager->get_obj_count(translucent)) {
            rend_data_ctx.state.enable_blend();
            rctx->disp_manager->draw(rend_data_ctx, translucent, cam, 0, true, alpha);
            rend_data_ctx.state.disable_blend();
        }
        rend->transparency_combine(rend_data_ctx, &rt, (float_t)alpha * (float_t)(1.0 / 255.0));
    }
}

static int32_t draw_pass_3d_translucent_count_layers(render_context* rctx,
    int32_t* alpha_array, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent, cam_data& cam) {
    bool arr[0x100] = { false };

    draw_pass_3d_translucent_has_objects(rctx, arr, opaque, cam);
    draw_pass_3d_translucent_has_objects(rctx, arr, transparent, cam);
    draw_pass_3d_translucent_has_objects(rctx, arr, translucent, cam);

    int32_t count = 0;
    for (int32_t i = 0xFF; i >= 0; i--)
        if (arr[i]) {
            count++;
            *alpha_array++ = i;
        }
    return count;
}

static int32_t draw_pass_3d_translucent_count_layers(render_context* rctx,
    int32_t* alpha_array, mdl::ObjTypeScreen opaque,
    mdl::ObjTypeScreen transparent, mdl::ObjTypeScreen translucent, cam_data& cam) {
    bool arr[0x100] = { false };

    draw_pass_3d_translucent_has_objects(rctx, arr, opaque, cam);
    draw_pass_3d_translucent_has_objects(rctx, arr, transparent, cam);
    draw_pass_3d_translucent_has_objects(rctx, arr, translucent, cam);

    int32_t count = 0;
    for (int32_t i = 0xFF; i >= 0; i--)
        if (arr[i]) {
            count++;
            *alpha_array++ = i;
        }
    return count;
}

static void draw_pass_3d_translucent_has_objects(
    render_context* rctx, bool* arr, mdl::ObjType type, cam_data& cam) {
    rctx->disp_manager->calc_obj_radius(cam, type);
    mdl::ObjList& vec = rctx->disp_manager->obj[type];
    for (mdl::ObjData*& i : vec)
        switch (i->kind) {
        case mdl::OBJ_KIND_NORMAL: {
            int32_t alpha = (int32_t)(i->args.sub_mesh.blend_color.w * 255.0f);
            alpha = clamp_def(alpha, 0, 255);
            arr[alpha] = true;
        } break;
        case mdl::OBJ_KIND_TRANSLUCENT: {
            for (int32_t j = 0; j < i->args.translucent.count; j++) {
                int32_t alpha = (int32_t)(i->args.translucent.sub_mesh[j]->blend_color.w * 255.0f);
                alpha = clamp_def(alpha, 0, 255);
                arr[alpha] = true;
            }
        } break;
        }
}

static void draw_pass_3d_translucent_has_objects(
    render_context* rctx, bool* arr, mdl::ObjTypeScreen type, cam_data& cam) {
    rctx->disp_manager->calc_obj_radius(cam, type);
    mdl::ObjList& vec = rctx->disp_manager->obj[type];
    for (mdl::ObjData*& i : vec)
        switch (i->kind) {
        case mdl::OBJ_KIND_NORMAL: {
            int32_t alpha = (int32_t)(i->args.sub_mesh.blend_color.w * 255.0f);
            alpha = clamp_def(alpha, 0, 255);
            arr[alpha] = true;
        } break;
        case mdl::OBJ_KIND_TRANSLUCENT: {
            for (int32_t j = 0; j < i->args.translucent.count; j++) {
                int32_t alpha = (int32_t)(i->args.translucent.sub_mesh[j]->blend_color.w * 255.0f);
                alpha = clamp_def(alpha, 0, 255);
                arr[alpha] = true;
            }
        } break;
        }
}

static void draw_pass_reflect_full(render_data_context& rend_data_ctx, rndr::RenderManager* render_manager) {
    render_context* rctx = rctx_ptr;
    rend_data_ctx.state.begin_event("pass_reflect");
    RenderTexture& refl_tex = render_manager->get_render_texture(0);
    RenderTexture& refl_buf_tex = rctx->reflect_buffer;
    refl_tex.Bind(rend_data_ctx.state);
    extern bool reflect_full;
    if (rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_OPAQUE)
        || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSPARENT)
        || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSLUCENT_SORT_BY_RADIUS)
        || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSLUCENT)) {
        refl_tex.SetViewport(rend_data_ctx.state);

        if (!rctx->sss_data->enable || !rctx->sss_data->downsample
            || draw_pass_3d_get_translucent_count(rctx)) {
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            rend_data_ctx.state.clear(GL_DEPTH_BUFFER_BIT);
        }

        rend_data_ctx.state.set_depth_func(GL_LEQUAL);

        for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
            rctx->light_set[i].data_set(rend_data_ctx, rctx->face, (light_set_id)i);
        for (int32_t i = FOG_DEPTH; i < FOG_BUMP; i++)
            rctx->fog[i].data_set(rend_data_ctx, (fog_id)i);

        rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
        if (rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_SSS))
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
        else
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_func(GL_LEQUAL);
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        rend_data_ctx.state.set_cull_face_mode(GL_FRONT);

        reflect_draw = true;
        rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;

        if (render_manager->shadow)
            draw_pass_3d_shadow_set(rend_data_ctx, render_manager->shadow_ptr, rctx);
        else
            draw_pass_3d_shadow_reset(rend_data_ctx, rctx);

        if (render_manager->effect_texture)
            rend_data_ctx.state.active_bind_texture_2d(14, render_manager->effect_texture->glid);
        else
            rend_data_ctx.state.active_bind_texture_2d(14, rctx->empty_texture_2d->glid);

        rend_data_ctx.shader_flags.arr[U_WATER_REFLECT] = 0;

        rctx->sss_data->set_texture(rend_data_ctx.state, 3);

        rend_data_ctx.set_npr(render_manager);
        rend_data_ctx.set_batch_sss_param(sss_param_reflect);

        rend_data_ctx.state.bind_sampler(14, rctx->render_samplers[0]);
        rend_data_ctx.state.bind_sampler(15, rctx->render_samplers[0]);
        rend_data_ctx.state.bind_sampler(16, rctx->render_samplers[0]);

        rend_data_ctx.shader_flags.arr[U_STAGE_AMBIENT] = render_manager->light_stage_ambient ? 1 : 0;

        cam_data cam = render_manager->cam;
        set_reflect_mat(rend_data_ctx, cam);
        rend_data_ctx.set_batch_scene_camera(cam);

        if (render_manager->alpha_z_sort) {
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_REFLECT_TRANSLUCENT, 1, cam, render_manager->field_31F);
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_REFLECT_TRANSLUCENT_SORT_BY_RADIUS, 2, cam);
        }

        if (render_manager->opaque_z_sort)
            rctx->disp_manager->obj_sort(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_OPAQUE, 0, cam);

        if (render_manager->draw_pass_3d[DRAW_PASS_3D_OPAQUE]) {
            rend_data_ctx.state.enable_depth_test();
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_OPAQUE, cam);
            rend_data_ctx.state.disable_depth_test();
        }

        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_OPAQUE, cam);

        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        if (render_manager->draw_pass_3d[DRAW_PASS_3D_TRANSPARENT]) {
            rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_TRANSPARENT, cam);
        }

        if (render_manager->npr_param == 1)
            render_manager->pass_3d_contour(rend_data_ctx);

        rend_data_ctx.state.disable_depth_test();

        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_PRE_TRANSLUCENT, cam);
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        /*if (npr_param == 1) {
            rend_data_ctx.state.set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }*/

        rend_data_ctx.state.enable_depth_test();

        if (render_manager->draw_pass_3d[DRAW_PASS_3D_TRANSLUCENT]) {
            rend_data_ctx.state.enable_blend();
            rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            rend_data_ctx.state.set_depth_mask(GL_FALSE);
            rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_TRANSLUCENT_SORT_BY_RADIUS, cam);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_REFLECT_TRANSLUCENT, cam);
            rend_data_ctx.state.disable_blend();
        }

        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE); // X
        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_POST_TRANSLUCENT, cam);
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        rend_data_ctx.state.disable_depth_test();

        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_NORMAL, cam);
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        rend_data_ctx.state.active_bind_texture_2d(14, 0);
        rend_data_ctx.state.active_bind_texture_2d(15, 0);

        if (render_manager->shadow)
            draw_pass_3d_shadow_reset(rend_data_ctx, rctx);
        reflect_draw = false;

        rend_data_ctx.state.set_cull_face_mode(GL_BACK);
        rend_data_ctx.state.disable_depth_test();

        for (int32_t i = render_manager->reflect_blur_num, j = 0; i > 0; i--, j++) {
            apply_blur_filter_sub(rend_data_ctx, &refl_buf_tex, &refl_tex,
                render_manager->reflect_blur_filter, 1.0f, 1.0f, 0.0f);
            image_filter_scale(rend_data_ctx, &refl_tex, refl_buf_tex.color_texture);
        }
    }
    else {
        vec4 clear_color;
        rend_data_ctx.state.get_clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
        rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
        rend_data_ctx.state.clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    }
    shader::unbind(rend_data_ctx.state);
    rend_data_ctx.state.bind_framebuffer(0);
    rend_data_ctx.state.end_event();
}

static void apply_blur_filter_sub(render_data_context& rend_data_ctx, RenderTexture* dst, RenderTexture* src,
    blur_filter_mode filter, const vec2 res_scale, const vec4 scale, const vec4 offset) {
    if (!dst || !src)
        return;

    render_context* rctx = rctx_ptr;

    rend_data_ctx.state.begin_event("`anonymous-namespace'::Impl::apply_blur_filter_sub");

    dst->Bind(rend_data_ctx.state);
    dst->SetViewport(rend_data_ctx.state);

    filter_scene_shader_data filter_scene = {};
    float_t w = res_scale.x / (float_t)src->GetWidth();
    float_t h = res_scale.y / (float_t)src->GetHeight();
    filter_scene.g_transform = { w, h, 0.0f, 0.0f };
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rctx->filter_scene_ubo.WriteMemory(rend_data_ctx.state, filter_scene);

    imgfilter_batch_shader_data imgfilter_batch = {};
    if (filter == BLUR_FILTER_32)
        imgfilter_batch.g_color_scale = scale * (float_t)(1.0 / 8.0);
    else
        imgfilter_batch.g_color_scale = scale * (float_t)(1.0 / 4.0);
    imgfilter_batch.g_color_offset = offset;
    imgfilter_batch.g_texture_lod = 0.0f;
    rctx->imgfilter_batch_ubo.WriteMemory(rend_data_ctx.state, imgfilter_batch);

    rend_data_ctx.shader_flags.arr[U_IMAGE_FILTER] = filter == BLUR_FILTER_32 ? 1 : 0;
    rend_data_ctx.state.bind_vertex_array(rctx->box_vao);
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_IMGFILT);
    rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->filter_scene_ubo);
    rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->imgfilter_batch_ubo);
    rend_data_ctx.state.active_bind_texture_2d(0, src->GetColorTex());
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, (GLint)(filter * 4LL), 4);
    rend_data_ctx.state.end_event();
}

static void render_manager_free_render_textures() {
    rndr::RenderManager& render_manager = *rctx_ptr->render_manager;

#ifdef USE_OPENGL
    if (!Vulkan::use) {
        if (render_manager.multisample_renderbuffer) {
            glDeleteRenderbuffers(1, &render_manager.multisample_renderbuffer);
            render_manager.multisample_renderbuffer = 0;
        }

        if (render_manager.multisample_framebuffer) {
            glDeleteFramebuffers(1, &render_manager.multisample_framebuffer);
            render_manager.multisample_framebuffer = 0;
        }
    }
#endif

    for (RenderTexture& i : render_manager.render_textures)
        i.Free();
}

static void render_manager_init_render_textures(int32_t multisample) {
    rndr::RenderManager& render_manager = *rctx_ptr->render_manager;

#ifdef USE_OPENGL
    if (!Vulkan::use && multisample) {
        glGenFramebuffers(1, &render_manager.multisample_framebuffer);
        glGenRenderbuffers(1, &render_manager.multisample_renderbuffer);

        gl_state.bind_framebuffer(render_manager.multisample_framebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, render_manager.multisample_renderbuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8,
            GL_RGBA8, render_manager.width, render_manager.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER, render_manager.multisample_renderbuffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        gl_state.bind_framebuffer(0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        if (glGetError()) {
            glDeleteRenderbuffers(1, &render_manager.multisample_renderbuffer);
            render_manager.multisample_renderbuffer = 0;
            glDeleteFramebuffers(1, &render_manager.multisample_framebuffer);
            render_manager.multisample_framebuffer = 0;
        }
    }
#endif

    for (int32_t i = 0; i < 9; i++) {
        const rndr::RenderTextureData* tex_data = &rndr::render_manager_render_texture_data_array[i];
        if (tex_data->type != GL_TEXTURE_2D)
            continue;

        RenderTexture& rt = render_manager.render_textures[i];
        rt.Init(tex_data->width, tex_data->height, tex_data->max_level,
            tex_data->color_format, tex_data->depth_format);
        rt.Bind(gl_state);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    gl_state.bind_framebuffer(0);
}

static void set_reflect_mat(render_data_context& rend_data_ctx, cam_data& cam) {
    const vec3 clip_plane = *(vec3*)&rend_data_ctx.data.buffer_scene_data.g_clip_plane;
    const vec4 temp = 2.0f * rend_data_ctx.data.buffer_scene_data.g_clip_plane;

    reflect_mat = mat4_identity;
    *(vec3*)&reflect_mat.row0 -= temp.x * clip_plane;
    *(vec3*)&reflect_mat.row1 -= temp.y * clip_plane;
    *(vec3*)&reflect_mat.row2 -= temp.z * clip_plane;
    *(vec3*)&reflect_mat.row3 -= temp.w * clip_plane;
    reflect_mat.row0.w = 0.0f;
    reflect_mat.row1.w = 0.0f;
    reflect_mat.row2.w = 0.0f;
    reflect_mat.row3.w = 1.0f;

    mat4_mul(&reflect_mat, &cam.view_mat, &cam.view_mat);
    cam.calc_view_proj_mat();

    vec3 view_point = cam.get_view_point();
    mat4_transform_point(&reflect_mat, &view_point, &view_point);
    cam.set_view_point(view_point);
}
