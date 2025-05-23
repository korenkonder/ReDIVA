/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include "gl.hpp"
#include "render.hpp"
#include "shadow.hpp"
#include <list>

enum reflect_refract_resolution_mode {
    REFLECT_REFRACT_RESOLUTION_256x256 = 0,
    REFLECT_REFRACT_RESOLUTION_512x256,
    REFLECT_REFRACT_RESOLUTION_512x512,
    REFLECT_REFRACT_RESOLUTION_MAX,
};

typedef void(*draw_pre_process_func)(struct render_data_context& rend_data_ctx,
    void* data, const struct cam_data& cam);

struct draw_pre_process {
    int32_t type;
    draw_pre_process_func func;
    void* data;
};

struct render_data_context;

namespace rndr {
    enum RenderPassID {
        RND_PASSID_SHADOW = 0,
        RND_PASSID_SS_SSS,
        RND_PASSID_2,
        RND_PASSID_REFLECT,
        RND_PASSID_REFRACT,
        RND_PASSID_PRE_PROCESS,
        RND_PASSID_CLEAR,
        RND_PASSID_PRE_SPRITE,
        RND_PASSID_3D,
        RND_PASSID_SHOW_VECTOR,
        RND_PASSID_POST_PROCESS,
        RND_PASSID_SPRITE,
        RND_PASSID_12,
        RND_PASSID_NUM,
    };

    struct RenderManager {
        bool pass_sw[RND_PASSID_NUM];
        Shadow* shadow_ptr;
        bool reflect;
        bool refract;
        int32_t reflect_blur_num;
        blur_filter_mode reflect_blur_filter;
        rndr::Render* render;
        bool sync_gpu;
        double_t cpu_time[RND_PASSID_NUM];
        double_t gpu_time[RND_PASSID_NUM];
        time_struct time;
        bool shadow;
        bool opaque_z_sort;
        bool alpha_z_sort;
        bool draw_pass_3d[DRAW_PASS_3D_MAX];
        bool field_11E;
        bool field_11F;
        bool field_120;
        bool show_ref_map;
        int32_t reflect_type; // stage_data_reflect_type
        bool clear;
        int32_t tex_index[11];
        RenderTexture render_textures[8];
        int32_t width;
        int32_t height;
        GLuint multisample_framebuffer;
        GLuint multisample_renderbuffer;
        bool multisample;
        bool check_state;
        int32_t show_vector_flags;
        float_t show_vector_length;
        float_t show_vector_z_offset;
        bool show_stage_shadow;
        std::list<draw_pre_process> pre_process;
        texture* effect_texture;
        int32_t npr_param;
        bool field_31C;
        bool reflect_texture_mask;
        bool reflect_tone_curve;
        bool field_31F;
        bool light_stage_ambient;
        bool npr;
        cam_data cam;

        RenderManager();
        ~RenderManager();

        void add_pre_process(int32_t type, draw_pre_process_func func, void* data);
        void clear_pre_process(int32_t type);
        reflect_refract_resolution_mode get_reflect_resolution_mode();
        reflect_refract_resolution_mode get_refract_resolution_mode();
        RenderTexture& get_render_texture(int32_t index);
        void reset();
        void resize(int32_t width, int32_t height);
        void set_clear(bool value);
        void set_effect_texture(texture* value);
        void set_multisample(bool value);
        void set_npr_param(int32_t value);
        void set_pass_sw(rndr::RenderPassID id, bool value);
        void set_reflect(bool value);
        void set_reflect_blur(int32_t reflect_blur_num, blur_filter_mode reflect_blur_filter);
        void set_reflect_resolution_mode(reflect_refract_resolution_mode mode);
        void set_reflect_type(int32_t type);
        void set_refract(bool value);
        void set_refract_resolution_mode(reflect_refract_resolution_mode mode);
        void set_shadow_false();
        void set_shadow_true();

        void render_all();
        void rndpass_post_proc();
        void rndpass_pre_proc();

        void render_single_pass(render_data_context& rend_data_ctx, rndr::RenderPassID id);

        void render_pass_begin();
        void render_pass_end(rndr::RenderPassID id);

        void pass_shadow(render_data_context& rend_data_ctx);
        void pass_ss_sss(render_data_context& rend_data_ctx);
        void pass_reflect(render_data_context& rend_data_ctx);
        void pass_refract(render_data_context& rend_data_ctx);
        void pass_pre_process(render_data_context& rend_data_ctx);
        void pass_clear(render_data_context& rend_data_ctx);
        void pass_pre_sprite(render_data_context& rend_data_ctx);
        void pass_3d(render_data_context& rend_data_ctx);
        void pass_show_vector(render_data_context& rend_data_ctx);
        void pass_post_process(render_data_context& rend_data_ctx);
        void pass_sprite(render_data_context& rend_data_ctx);

        void pass_3d_contour(render_data_context& rend_data_ctx);
        void pass_sprite_surf(render_data_context& rend_data_ctx);
    };
}

extern void image_filter_scale(render_data_context& rend_data_ctx,
    RenderTexture* dst, texture* src, const vec4& scale = 1.0f);

extern void render_manager_init_data(int32_t ssaa, int32_t hd_res, int32_t ss_alpha_mask, bool npr);
extern void render_manager_free_data();
