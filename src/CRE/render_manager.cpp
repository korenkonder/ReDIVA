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
#include "camera.hpp"
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
#include "shadow.hpp"
#include "sprite.hpp"
#include "sss.hpp"
#include "static_var.hpp"
#include "texture.hpp"

extern render_context* rctx_ptr;

void apply_esm_filter(render_data_context& rend_data_ctx,
    RenderTexture* dst, RenderTexture* buf, RenderTexture* src,
    float_t sigma, float_t offset, bool enable_lit_proj);
void apply_esm_min_filter(render_data_context& rend_data_ctx,
    RenderTexture* dst, RenderTexture* buf, RenderTexture* src);
static bool litproj_clear(render_data_context& rend_data_ctx, light_proj* litproj, cam_data& cam);
static int32_t draw_pass_3d_get_translucent_count(render_context* rctx);
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

void apply_blur_filter_sub(render_data_context& rend_data_ctx, RenderTexture* dst, RenderTexture* src,
    ImgfBoxSampl filter, const vec2 res_scale, const vec4 scale, const vec4 offset);

static void rndpass_create_texture(int32_t multisample);
static void rndpass_free_texture();

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
        render(), sync_gpu(), cpu_time(), gpu_time(), draw_pass_3d(), field_11E(), silhouette(),
        silhouette_high(), show_ref_map(), reflect_type(), clear(), tex_index(), width(), height(),
        multisample_framebuffer(), multisample_renderbuffer(), multisample(), check_state(), show_vector_flags(),
        show_vector_length(), show_vector_z_offset(), show_stage_shadow(), effect_texture(), npr_param(),
        npr_mask(), reflect_texture_mask(), reflect_tone_curve(), field_31F(), light_stage_ambient(), ss4x() {
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

    // 0x140502390
    void RenderManager::calc_shadow() {
        shadow_ptr->reset_group();

        if (shadow) {
            shadow_ptr->set_view_mtx(&rctx_ptr->camera->cmat);

            light_data& light_chara = rctx_ptr->light_set[LIGHT_SET_MAIN].lights[LIGHT_CHARA];

            vec3 dir;
            light_chara.get_position(dir);
            dir = -dir;

            float_t length = vec3::length(dir);
            if (length < 0.000001f)
                dir = { 0.0f, 1.0f, 0.0f };
            else if (length != 0.0)
                dir *= (1.0f / length);
            shadow_ptr->set_light_dir(&dir);

            for (int32_t i = 0; i < 2; i++)
                if (rctx_ptr->disp_manager->get_obj_count((mdl::ObjType)((int32_t)mdl::OBJ_TYPE_SHADOW_CHARA + i)))
                    shadow_ptr->enable_group(i);
        }

        shadow_ptr->calc_shadowmap();
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
        silhouette = false;
        silhouette_high = false;

        reflect_blur_num = 1;
        reflect_blur_filter = IMGF_BOX_SAMPL_4;
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

        ss4x = false;
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

    void RenderManager::set_reflect_blur(int32_t reflect_blur_num, ImgfBoxSampl reflect_blur_filter) {
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
        spr::preFlush();
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
        spr::postFlush();

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
                rctx->litproj->draw_texture.end_render(rend_data_ctx.state);
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
            int32_t group[2];
            shad->begin_make_shadow_textures(group);

            cam_data cam;
            for (int32_t i = 0, index = 0; i < 2; i++) {
                if (!obj_shadow_count[i])
                    continue;

                shad->begin_make_silhouette_map(rend_data_ctx, cam, group[i], index);
                rend_data_ctx.set_batch_scene_camera(cam);
                rctx->disp_manager->draw(rend_data_ctx, (mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_CHARA + group[i]), cam);
                if (rctx->disp_manager->get_obj_count(
                    (mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_OBJECT_CHARA + group[i])) > 0) {
                    rend_data_ctx.state.set_color_mask(show_stage_shadow
                        ? GL_TRUE : GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    rctx->disp_manager->draw(rend_data_ctx,
                        (mdl::ObjType)(mdl::OBJ_TYPE_SHADOW_OBJECT_CHARA + i), cam);
                    rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                }
                shad->end_make_silhouette_map(rend_data_ctx, group[i], index);
                index++;
            }
        }
        else {
            shad->begin_make_shadow_textures(0);
            shad->clear(rend_data_ctx.state);
        }
        shad->end_make_shadow_textures(rend_data_ctx.state);
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

            RenderTexture& refl_tex = get_render_texture(0);
            if (sss->set(rend_data_ctx, refl_tex)) {
                rndr::Render* rend = render;

                for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
                    rctx->light_set[i].data_set(rend_data_ctx, rctx->face, (light_set_id)i);

                if (shadow)
                    shadow_ptr->bind_shadow(rend_data_ctx, rctx);
                else
                    shadow_ptr->unbind_shadow(rend_data_ctx, rctx);

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
                shadow_ptr->unbind_shadow(rend_data_ctx, rctx);
                rend_data_ctx.shader_flags.arr[U_NPR] = 0;

                if (npr_param == 1) {
                    if (sss->enable && sss->downsample) {
                        rend_data_ctx.shader_flags.arr[U_NPR] = 1;
                        rend->draw_contour(rend_data_ctx, reflect_cam);
                    }
                    else if (ss4x) {
                        refl_tex.begin_render(rend_data_ctx.state);
                        refl_tex.set_viewport(rend_data_ctx.state);
                        rend_data_ctx.state.clear(GL_DEPTH_BUFFER_BIT);
                        rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SSS_SKIN;
                        rend_data_ctx.state.enable_depth_test();
                        rend_data_ctx.state.set_depth_func(GL_LEQUAL);
                        rend_data_ctx.state.set_depth_mask(GL_TRUE);
                        rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SSS, reflect_cam);
                        rend_data_ctx.state.disable_depth_test();
                        rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
                        refl_tex.end_render(rend_data_ctx.state);
                        rend_data_ctx.shader_flags.arr[U_NPR] = 1;
                        rend->draw_contour(rend_data_ctx, reflect_cam);
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
                shadow_ptr->bind_shadow(rend_data_ctx, rctx);
            else
                shadow_ptr->unbind_shadow(rend_data_ctx, rctx);

            rend_data_ctx.set_batch_scene_camera(cam);

            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SSS_SKIN;
            rend_data_ctx.state.enable_depth_test();
            rend_data_ctx.state.set_depth_func(GL_LEQUAL);
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SSS, cam);
            rend_data_ctx.state.disable_depth_test();
            rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
            shadow_ptr->unbind_shadow(rend_data_ctx, rctx);
            rend_data_ctx.shader_flags.arr[U_NPR] = 0;

            if (npr_param == 1) {
                if (sss->enable && sss->downsample) {
                    rend_data_ctx.shader_flags.arr[U_NPR] = 1;
                    rend->draw_contour(rend_data_ctx, cam);
                }
                else if (ss4x) {
                    rend->fb_fbo[0].begin_render(rend_data_ctx.state);
                    rend->fb_fbo[0].set_viewport(rend_data_ctx.state);
                    rend_data_ctx.state.clear(GL_DEPTH_BUFFER_BIT);
                    rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SSS_SKIN;
                    rend_data_ctx.state.enable_depth_test();
                    rend_data_ctx.state.set_depth_func(GL_LEQUAL);
                    rend_data_ctx.state.set_depth_mask(GL_TRUE);
                    rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SSS, cam);
                    rend_data_ctx.state.disable_depth_test();
                    rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
                    rend->fb_fbo[0].end_render(rend_data_ctx.state);
                    rend_data_ctx.shader_flags.arr[U_NPR] = 1;
                    rend->draw_contour(rend_data_ctx, cam);
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
        RenderTexture& refl_tex = get_render_texture(0);
        RenderTexture& refl_buf_tex = rctx->reflect_buffer;
        if (rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_OPAQUE)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSPARENT)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSLUCENT)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_OPAQUE)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_TRANSPARENT)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT)) {
            refl_tex.begin_render(rend_data_ctx.state);
            refl_tex.set_viewport(rend_data_ctx.state);
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
                    shadow_ptr->bind_shadow(rend_data_ctx, rctx);
                else
                    shadow_ptr->unbind_shadow(rend_data_ctx, rctx);

                if (effect_texture)
                    rend_data_ctx.state.active_bind_texture_2d(14, effect_texture->glid);
                else
                    rend_data_ctx.state.active_bind_texture_2d(14, rctx->empty_texture_2d->glid);

                rend_data_ctx.shader_flags.arr[U_TEX_REFLECTMAP] = 0;

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
                    shadow_ptr->unbind_shadow(rend_data_ctx, rctx);
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
                image_filter_scale(rend_data_ctx, &refl_tex, refl_buf_tex.get_texture());
            }

            shader::unbind(rend_data_ctx.state);
            refl_tex.end_render(rend_data_ctx.state);
        }
        else {
            refl_tex.begin_render(rend_data_ctx.state);
            vec4 clear_color;
            rend_data_ctx.state.get_clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            rend_data_ctx.state.clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            refl_tex.end_render(rend_data_ctx.state);
        }
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_refract(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        rend_data_ctx.state.begin_event("pass_refract");
        RenderTexture& reflect_texture = get_render_texture(1);
        if (rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFRACT_OPAQUE)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFRACT_TRANSPARENT)
            || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFRACT_TRANSLUCENT)) {
            reflect_texture.begin_render(rend_data_ctx.state);
            reflect_texture.set_viewport(rend_data_ctx.state);
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
            shader::unbind(rend_data_ctx.state);
            reflect_texture.end_render(rend_data_ctx.state);
        }
        else {
            reflect_texture.begin_render(rend_data_ctx.state);
            vec4 clear_color;
            rend_data_ctx.state.get_clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            rend_data_ctx.state.clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            reflect_texture.end_render(rend_data_ctx.state);
        }
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
            rctx->screen_buffer.begin_render(rend_data_ctx.state);
            clear_color_set_gl(rend_data_ctx.state);
            //rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            rctx->screen_buffer.end_render(rend_data_ctx.state);
        }

        rndr::Render* rend = render;
        if (!rctx->sss_data->enable || !rctx->sss_data->downsample) {
            rend->begin_render(rend_data_ctx.state);
            if (spr::getObjListCount(spr::SPR_TARGET_BACK)) {
                rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
                rend_data_ctx.state.clear_depth(1.0f);
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                clear_color_set_gl(rend_data_ctx.state);
                rend_data_ctx.state.clear_depth(1.0f);
                if (clear)
                    rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                else
                    rend_data_ctx.state.clear(GL_DEPTH_BUFFER_BIT);
            }
            rend->end_render(rend_data_ctx.state);
        }
        else {
            if (spr::getObjListCount(spr::SPR_TARGET_BACK)) {
                rend->begin_render(rend_data_ctx.state);
                rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
                rend->end_render(rend_data_ctx.state);
            }
            else if (clear) {
                rend->begin_render(rend_data_ctx.state);
                clear_color_set_gl(rend_data_ctx.state);
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
                rend->end_render(rend_data_ctx.state);
            }
        }

        gl_get_error_print();
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_pre_sprite(render_data_context& rend_data_ctx) {
        if (!spr::getObjListCount(spr::SPR_TARGET_BACK))
            return;

        rend_data_ctx.state.begin_event("pass_pre_sprite");
        rndr::Render* rend = render;
        rend->begin_render(rend_data_ctx.state, true);
        rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
        rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
        rend_data_ctx.state.set_depth_mask(GL_FALSE);
        rend_data_ctx.state.disable_depth_test();
        rend_data_ctx.state.enable_blend();
        rend_data_ctx.state.disable_cull_face();
        spr::flush(rend_data_ctx, spr::SPR_TARGET_BACK, true,
            rend->user_fbo.get_texture(),
            rctx_ptr->camera->set_projection_matrix_2d(true));
        rend_data_ctx.state.enable_cull_face();
        rend_data_ctx.state.disable_blend();
        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        shader::unbind(rend_data_ctx.state);
        rend->end_render(rend_data_ctx.state);
        gl_get_error_print();
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_3d(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        render->begin_render(rend_data_ctx.state);
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
            shadow_ptr->bind_shadow(rend_data_ctx, rctx);
        else
            shadow_ptr->unbind_shadow(rend_data_ctx, rctx);

        if (effect_texture)
            rend_data_ctx.state.active_bind_texture_2d(14, effect_texture->glid);
        else
            rend_data_ctx.state.active_bind_texture_2d(14, rctx->empty_texture_2d->glid);

        RenderTexture* refl_tex = 0;
        if (pass_sw[RND_PASSID_REFLECT] && reflect) {
            refl_tex = &get_render_texture(0);
            refl_tex->bind_texture(rend_data_ctx.state, 15);
            rend_data_ctx.state.active_bind_texture_2d(15, refl_tex->get_texture_glid());
            rend_data_ctx.shader_flags.arr[U_TEX_REFLECTMAP] = 1;
        }
        else {
            rend_data_ctx.state.active_bind_texture_2d(15, rctx->empty_texture_2d->glid);
            rend_data_ctx.shader_flags.arr[U_TEX_REFLECTMAP] = 0;
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
                mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_GLITTER, 1, cam, field_31F);
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, 1, cam, field_31F);
            rctx->disp_manager->obj_sort(rend_data_ctx,
                mdl::OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_OPAQUE, 1, cam, field_31F);
        }

        if (alpha_z_sort)
            rctx->disp_manager->obj_sort(rend_data_ctx, mdl::OBJ_TYPE_TRANSLUCENT_SORT_BY_RADIUS, 2, cam);

        if (opaque_z_sort)
            rctx->disp_manager->obj_sort(rend_data_ctx, mdl::OBJ_TYPE_OPAQUE, 0, cam);

        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        if (silhouette)
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SILHOUETTE, cam);

        if (draw_pass_3d[DRAW_PASS_3D_OPAQUE])
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_OPAQUE, cam);
        rend_data_ctx.state.disable_depth_test();

        Glitter::glt_particle_manager->DispScenes(rend_data_ctx, Glitter::DISP_OPAQUE, cam);

        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        if (draw_pass_3d[DRAW_PASS_3D_TRANSPARENT])
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_TRANSPARENT, cam);

        if (silhouette_high)
            rctx->disp_manager->draw(rend_data_ctx, mdl::OBJ_TYPE_SILHOUETTE_HIGH, cam);

        render->query_chara_exposure(rend_data_ctx, cam);

        if (npr_param == 1)
            pass_3d_contour(rend_data_ctx);

        render->draw_sun(rend_data_ctx, cam);
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
            vec2 persp_scale = 1.0f;
            vec2 persp_offset;
            rctx->render->perspective(persp_offset);
            screen_cam.calc_persp_proj_mat_offset(persp_scale, persp_offset);
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

        if (refl_tex)
            refl_tex->unbind_texture(rend_data_ctx.state);

        if (shadow)
            shadow_ptr->unbind_shadow(rend_data_ctx, rctx);
        pass_sprite_surf(rend_data_ctx);
        shader::unbind(rend_data_ctx.state);
        render->end_render(rend_data_ctx.state);
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
                light_proj_tex = litproj->draw_texture.get_texture();
        }

        render->draw_flare(rend_data_ctx, cam, light_proj_tex, npr_param);
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_sprite(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        rend_data_ctx.state.begin_event("pass_sprite");
        if (spr::getObjListCount(spr::SPR_TARGET_FRONT)) {
            rend_data_ctx.state.set_viewport(0, 0, width, height);

            if (Vulkan::use)
                rctx->screen_buffer.begin_render(rend_data_ctx.state);
#ifdef USE_OPENGL
            else if (multisample && multisample_framebuffer) {
                rend_data_ctx.state.bind_framebuffer(multisample_framebuffer);
                rend_data_ctx.state.enable_multisample();
                rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
                rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
            }
            else
                rctx->screen_buffer.begin_render(rend_data_ctx.state);
#endif

            rend_data_ctx.state.disable_depth_test();
            rend_data_ctx.state.enable_blend();
            rend_data_ctx.state.disable_cull_face();
            spr::flush(rend_data_ctx, spr::SPR_TARGET_FRONT, true,
                rctx->screen_overlay_buffer.get_texture(),
                rctx->camera->set_projection_matrix_2d(false));
            rend_data_ctx.state.enable_cull_face();
            rend_data_ctx.state.disable_blend();
            rend_data_ctx.state.enable_depth_test();

            if (Vulkan::use)
                rctx->screen_buffer.end_render(rend_data_ctx.state);
#ifdef USE_OPENGL
            if (multisample && multisample_framebuffer) {
                rend_data_ctx.state.bind_framebuffer(rctx->screen_buffer.get_fb());
                rend_data_ctx.state.disable_multisample();
                rend_data_ctx.state.bind_read_framebuffer(multisample_framebuffer);
                rend_data_ctx.state.blit_framebuffer(0, 0, width, height,
                    0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                rend_data_ctx.state.bind_read_framebuffer(0);
            }
            else
                rctx->screen_buffer.end_render(rend_data_ctx.state);
#endif
            shader::unbind(rend_data_ctx.state);

            gl_get_error_print();
        }
        rend_data_ctx.state.end_event();
    }

    void RenderManager::pass_3d_contour(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;
        RenderTexture* rt;
        RenderTexture* contour_fbo;
        if (reflect_draw) {
            rt = &get_render_texture(0);
            contour_fbo = &rctx->reflect_buffer;
    }
        else {
            rt = &render->fb_fbo[0];
            contour_fbo = render->contour_fbo;
        }

        rend_data_ctx.state.begin_event("`anonymous-namespace'::draw_npr_frame");
        rend_data_ctx.state.enable_blend();
        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rend_data_ctx.state.set_blend_equation(GL_FUNC_ADD);
        rend_data_ctx.state.disable_depth_test();

        quad_shader_data quad_data = {};
        quad_data.g_texcoord_modifier = { 0.5f, 0.5f, 0.5f, 0.5f };
        rend_data_ctx.state.write_uniform_buffer(rctx->quad_ubo, quad_data);

        contour_params_shader_data contour_params_data = {};
        const double_t max_distance = cam.get_max_distance();
        const double_t min_distance = cam.get_min_distance();
        contour_params_data.g_near_far = {
            (float_t)(max_distance * (1.0 / (max_distance - min_distance))),
            (float_t)(-(max_distance * min_distance) * (1.0 / (max_distance - min_distance))),
            (float_t)min_distance, (float_t)max_distance
        };
        rend_data_ctx.state.write_uniform_buffer(rctx->contour_params_ubo, contour_params_data);

        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_CONTOUR_NPR);
        rend_data_ctx.state.active_bind_texture_2d(14, rt->get_depth_texture_glid());
        rend_data_ctx.state.active_bind_texture_2d(16, contour_fbo->get_texture_glid());
        rend_data_ctx.state.active_bind_texture_2d(17, contour_fbo->get_depth_texture_glid());
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
        if (!spr::getObjListCount(spr::SPR_TARGET_FRONT_3D_SURF))
            return;

        render_context* rctx = rctx_ptr;
        rndr::Render* rend = render;

        rend_data_ctx.state.set_depth_mask(GL_FALSE);
        rend_data_ctx.state.disable_depth_test();
        rend_data_ctx.state.enable_blend();
        rend_data_ctx.state.disable_cull_face();
        spr::flush(rend_data_ctx, spr::SPR_TARGET_FRONT_3D_SURF, true,
            rend->user_fbo.get_texture(),
            rctx->camera->set_projection_matrix_2d(false));
    }
}

void image_filter_scale(render_data_context& rend_data_ctx,
    RenderTexture* dst, texture* src, const vec4& scale) {
    if (!dst || !dst->get_texture() || !dst->get_texture()->glid || !src || !src->glid)
        return;

    render_context* rctx = rctx_ptr;

    rend_data_ctx.state.begin_event("`anonymous-namespace'::Impl::apply_no_filter_sub");

    dst->begin_render(rend_data_ctx.state);
    dst->set_viewport(rend_data_ctx.state);

    image_filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = { 1.0f, 1.0f, 0.0f, 0.0f };
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rend_data_ctx.state.write_uniform_buffer(rctx->image_filter_scene_ubo, filter_scene);

    image_filter_batch_shader_data image_filter_batch = {};
    image_filter_batch.g_color_scale = scale;
    image_filter_batch.g_color_offset = 0.0f;
    image_filter_batch.g_texture_lod = 0.0f;
    rend_data_ctx.state.write_uniform_buffer(rctx->image_filter_batch_ubo, image_filter_batch);

    rend_data_ctx.shader_flags.arr[U_IMAGE_FILTER] = 5;
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_IMGFILT);
    rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->image_filter_scene_ubo);
    rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->image_filter_batch_ubo);
    rend_data_ctx.state.active_bind_texture_2d(0, src->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    rend_data_ctx.state.end_event();
}

// 0x140502A10
void rndpass_init(int32_t anti_alias, int32_t min_render, int32_t ss_alpha_mask, bool ss4x) {
    rndr::RenderManager& render_manager = *rctx_ptr->render_manager;

    render_manager.reset();
    render_manager.ss4x = ss4x;
    RenderTexture::init();

    ScreenParam& screen_param = get_screen_param();
    ScreenParam& render_screen_param = get_render_screen_param();
    render_manager.render = rctx_ptr->render;
    render_manager.render->create_render_buffer(render_screen_param.width,
        render_screen_param.height, anti_alias, min_render, ss_alpha_mask);
    render_manager.render->set_viewport(render_screen_param.xoffset,
        render_screen_param.yoffset, render_screen_param.width, render_screen_param.height);
    render_manager.width = screen_param.width;
    render_manager.height = screen_param.height;
    rndpass_create_texture(1);
    render_manager.render->create_other();

    init_shadow();
    render_manager.shadow_ptr = get_shadow();
    render_manager.shadow_ptr->create();

    rctx_ptr->sss_data->init();
    gl_get_error_print();
}

// 0x140502770
void rndpass_finish() {
    rndr::RenderManager& render_manager = *rctx_ptr->render_manager;

    rndpass_free_texture();
    render_manager.render->destroy();
    finish_shadow();
    rctx_ptr->sss_data->free();
}

// 0x1403B6560
void apply_esm_filter(render_data_context& rend_data_ctx,
    RenderTexture* dst, RenderTexture* buf, RenderTexture* src,
    float_t sigma, float_t offset, bool enable_lit_proj) {
    texture* dst_tex = dst->get_texture();
    texture* buf_tex = buf->get_texture();
    texture* src_tex = dst->get_texture();
    if (src)
        src_tex = src->get_depth_texture();

    if (!dst_tex || !buf_tex || !src_tex || dst_tex->width != buf_tex->width || dst_tex->height != buf_tex->height)
        return;

    render_context* rctx = rctx_ptr;

    rend_data_ctx.state.begin_event("`anonymous-namespace'::Impl::apply_esm_filter");
    esm_filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = 0.0f;
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rend_data_ctx.state.write_uniform_buffer(rctx->esm_filter_scene_ubo, filter_scene);

    esm_filter_batch_shader_data esm_filter_batch = {};
    double_t v6 = 1.0 / (sqrt(M_PI * 2.0) * sigma);
    double_t v8 = -1.0 / (2.0 * sigma * sigma);
    for (int32_t i = 0; i < 8; i++)
        ((float_t*)esm_filter_batch.g_gauss)[i] = (float_t)(exp((double_t)((ssize_t)i * i) * v8) * v6);

    rend_data_ctx.shader_flags.arr[U_LIGHT_PROJ] = enable_lit_proj ? 1 : 0;
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_ESMGAUSS);
    rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->esm_filter_scene_ubo);
    rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->esm_filter_batch_ubo);

    rend_data_ctx.state.set_viewport(0, 0, dst_tex->width, dst_tex->height);

    buf->begin_render(rend_data_ctx.state);
    esm_filter_batch.g_params = { 1.0f / (float_t)dst_tex->width, 0.0f, offset, offset };
    rend_data_ctx.state.write_uniform_buffer(rctx->esm_filter_batch_ubo, esm_filter_batch);

    rend_data_ctx.state.active_bind_texture_2d(0, src_tex->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

    dst->begin_render(rend_data_ctx.state);
    esm_filter_batch.g_params = { 0.0f, 1.0f / (float_t)dst_tex->height, offset, offset };
    rend_data_ctx.state.write_uniform_buffer(rctx->esm_filter_batch_ubo, esm_filter_batch);

    rend_data_ctx.state.active_bind_texture_2d(0, buf_tex->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    shader::unbind(rend_data_ctx.state);
    rend_data_ctx.state.end_event();
}

// 0x1403B6950
void apply_esm_min_filter(render_data_context& rend_data_ctx,
    RenderTexture* dst, RenderTexture* buf, RenderTexture* src) {
    texture* dst_tex = dst->get_texture();
    texture* buf_tex = buf->get_texture();
    texture* src_tex = src->get_texture();
    rend_data_ctx.state.begin_event("`anonymous-namespace'::Impl::apply_esm_min_filter");
    if (!dst_tex || !dst_tex->glid || !buf_tex || !buf_tex->glid || !src_tex || !src_tex->glid) {
        rend_data_ctx.state.end_event();
        return;
    }

    render_context* rctx = rctx_ptr;

    rend_data_ctx.state.begin_event("minimize");
    esm_filter_scene_shader_data filter_scene = {};
    filter_scene.g_transform = 0.0f;
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rend_data_ctx.state.write_uniform_buffer(rctx->esm_filter_scene_ubo, filter_scene);

    esm_filter_batch_shader_data esm_filter_batch = {};
    esm_filter_batch.g_gauss[0] = 0.0f;
    esm_filter_batch.g_gauss[1] = 0.0f;

    rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->esm_filter_scene_ubo);
    rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->esm_filter_batch_ubo);

    rend_data_ctx.state.set_viewport(0, 0, dst_tex->width, dst_tex->height);

    buf->begin_render(rend_data_ctx.state);
    esm_filter_batch.g_params = { 1.0f / (float_t)src_tex->width,
        1.0f / (float_t)src_tex->height, 0.0f, 0.0f };
    rend_data_ctx.state.write_uniform_buffer(rctx->esm_filter_batch_ubo, esm_filter_batch);

    rend_data_ctx.shader_flags.arr[U_ESM_FILTER] = 0;
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_ESMFILT);
    rend_data_ctx.state.active_bind_texture_2d(0, src_tex->glid);
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.bind_vertex_array(rctx->common_vao);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    rend_data_ctx.state.end_event();

    rend_data_ctx.state.begin_event("erosion");
    dst->begin_render(rend_data_ctx.state);
    esm_filter_batch.g_params = { 0.75f / (float_t)buf_tex->width,
        0.75f / (float_t)buf_tex->height, 0.0f, 0.0f };
    rend_data_ctx.state.write_uniform_buffer(rctx->esm_filter_batch_ubo, esm_filter_batch);

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

    litproj->draw_texture.begin_render(rend_data_ctx.state);
    litproj->draw_texture.set_viewport(rend_data_ctx.state);
    rend_data_ctx.state.enable_depth_test();
    rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    rend_data_ctx.state.clear_depth(1.0f);
    rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!light_proj::set_mat(rend_data_ctx, cam, true)) {
        litproj->draw_texture.end_render(rend_data_ctx.state);
        return false;
    }

    render_context* rctx = rctx_ptr;
    rctx->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_LITPROJ;
    rend_data_ctx.state.active_bind_texture_2d(17, tex->glid);
    rend_data_ctx.state.bind_sampler(17, rctx->render_samplers[2]);
    rend_data_ctx.state.active_bind_texture_2d(18, litproj->shadow_texture[0].get_texture_glid());
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

static void draw_pass_3d_translucent(render_data_context& rend_data_ctx,
    render_context* rctx, mdl::ObjType opaque,
    mdl::ObjType transparent, mdl::ObjType translucent, cam_data& cam) {
    if (rctx->disp_manager->get_obj_count(opaque) < 1
        && rctx->disp_manager->get_obj_count(transparent) < 1
        && rctx->disp_manager->get_obj_count(translucent) < 1)
        return;

    rndr::Render* rend = rctx->render;

    RenderTexture& rt = reflect_draw
        ? rctx->render_manager->get_render_texture(0) : rend->fb_fbo[0];

    int32_t alpha_array[256];
    int32_t count = draw_pass_3d_translucent_count_layers(rctx,
        alpha_array, opaque, transparent, translucent, cam);
    for (int32_t i = 0; i < count; i++) {
        int32_t alpha = alpha_array[i];
        rend->begin_render_transparency(rend_data_ctx, &rt);
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
        rend->end_render_transparency(rend_data_ctx, &rt, (float_t)alpha * (float_t)(1.0 / 255.0));
    }
}

static void draw_pass_3d_translucent(render_data_context& rend_data_ctx,
    render_context* rctx, mdl::ObjTypeScreen opaque,
    mdl::ObjTypeScreen transparent, mdl::ObjTypeScreen translucent, cam_data& cam) {
    if (rctx->disp_manager->get_obj_count(opaque) < 1
        && rctx->disp_manager->get_obj_count(transparent) < 1
        && rctx->disp_manager->get_obj_count(translucent) < 1)
        return;

    rndr::Render* rend = rctx->render;

    RenderTexture& rt = reflect_draw
        ? rctx->render_manager->get_render_texture(0) : rend->fb_fbo[0];

    int32_t alpha_array[256];
    int32_t count = draw_pass_3d_translucent_count_layers(rctx,
        alpha_array, opaque, transparent, translucent, cam);
    for (int32_t i = 0; i < count; i++) {
        int32_t alpha = alpha_array[i];
        rend->begin_render_transparency(rend_data_ctx, &rt);
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
        rend->end_render_transparency(rend_data_ctx, &rt, (float_t)alpha * (float_t)(1.0 / 255.0));
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
    extern bool reflect_full;
    if (rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_OPAQUE)
        || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSPARENT)
        || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSLUCENT_SORT_BY_RADIUS)
        || rctx->disp_manager->get_obj_count(mdl::OBJ_TYPE_REFLECT_TRANSLUCENT)) {
        refl_tex.begin_render(rend_data_ctx.state);
        refl_tex.set_viewport(rend_data_ctx.state);

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
            render_manager->shadow_ptr->bind_shadow(rend_data_ctx, rctx);
        else
            render_manager->shadow_ptr->unbind_shadow(rend_data_ctx, rctx);

        if (render_manager->effect_texture)
            rend_data_ctx.state.active_bind_texture_2d(14, render_manager->effect_texture->glid);
        else
            rend_data_ctx.state.active_bind_texture_2d(14, rctx->empty_texture_2d->glid);

        rend_data_ctx.shader_flags.arr[U_TEX_REFLECTMAP] = 0;

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
            render_manager->shadow_ptr->unbind_shadow(rend_data_ctx, rctx);
        reflect_draw = false;

        rend_data_ctx.state.set_cull_face_mode(GL_BACK);
        rend_data_ctx.state.disable_depth_test();

        for (int32_t i = render_manager->reflect_blur_num, j = 0; i > 0; i--, j++) {
            apply_blur_filter_sub(rend_data_ctx, &refl_buf_tex, &refl_tex,
                render_manager->reflect_blur_filter, 1.0f, 1.0f, 0.0f);
            image_filter_scale(rend_data_ctx, &refl_tex, refl_buf_tex.get_texture());
        }

        shader::unbind(rend_data_ctx.state);
        refl_tex.end_render(rend_data_ctx.state);
    }
    else {
        refl_tex.begin_render(rend_data_ctx.state);
        vec4 clear_color;
        rend_data_ctx.state.get_clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        rend_data_ctx.state.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
        rend_data_ctx.state.clear(GL_COLOR_BUFFER_BIT);
        rend_data_ctx.state.clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        refl_tex.end_render(rend_data_ctx.state);
    }
    rend_data_ctx.state.end_event();
}

// Added
void apply_blur_filter_sub(render_data_context& rend_data_ctx, RenderTexture* dst, RenderTexture* src,
    ImgfBoxSampl filter, const vec2 res_scale, const vec4 scale, const vec4 offset) {
    if (!dst || !src)
        return;

    render_context* rctx = rctx_ptr;

    rend_data_ctx.state.begin_event("`anonymous-namespace'::Impl::apply_blur_filter_sub");

    dst->begin_render(rend_data_ctx.state);
    dst->set_viewport(rend_data_ctx.state);

    image_filter_scene_shader_data filter_scene = {};
    float_t w = res_scale.x / (float_t)src->get_width();
    float_t h = res_scale.y / (float_t)src->get_height();
    filter_scene.g_transform = { w, h, 0.0f, 0.0f };
    filter_scene.g_texcoord = { 1.0f, 1.0f, 0.0f, 0.0f };
    rend_data_ctx.state.write_uniform_buffer(rctx->image_filter_scene_ubo, filter_scene);

    image_filter_batch_shader_data image_filter_batch = {};
    if (filter == IMGF_BOX_SAMPL_32)
        image_filter_batch.g_color_scale = scale * (float_t)(1.0 / 8.0);
    else
        image_filter_batch.g_color_scale = scale * (float_t)(1.0 / 4.0);
    image_filter_batch.g_color_offset = offset;
    image_filter_batch.g_texture_lod = 0.0f;
    rend_data_ctx.state.write_uniform_buffer(rctx->image_filter_batch_ubo, image_filter_batch);

    rend_data_ctx.shader_flags.arr[U_IMAGE_FILTER] = filter == IMGF_BOX_SAMPL_32 ? 1 : 0;
    rend_data_ctx.state.bind_vertex_array(rctx->box_vao);
    shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_IMGFILT);
    rend_data_ctx.state.bind_uniform_buffer_base(0, rctx->image_filter_scene_ubo);
    rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->image_filter_batch_ubo);
    rend_data_ctx.state.active_bind_texture_2d(0, src->get_texture_glid());
    rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
    rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, (GLint)(filter * 4LL), 4);
    rend_data_ctx.state.end_event();
}

// 0x140502560
static void rndpass_create_texture(int32_t multisample) {
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
        rt.create_texture(tex_data->width, tex_data->height, tex_data->max_level,
            tex_data->color_format, tex_data->depth_format);
        rt.begin_render(gl_state);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    gl_state.bind_framebuffer(0);
}

// 0x1405027A0
static void rndpass_free_texture() {
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
        i.destroy();
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
