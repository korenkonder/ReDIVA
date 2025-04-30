/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/light_param/glow.hpp"
#include "../KKdLib/vec.hpp"
#include "GL/uniform_buffer.hpp"
#include "renderer/dof.hpp"
#include "renderer/transparency.hpp"
#include "camera.hpp"

struct cam_data;
struct p_gl_rend_state;
struct render_data_context;

namespace rndr {
    struct Render {
        enum MagFilterType {
            MAG_FILTER_NEAREST = 0,
            MAG_FILTER_BILINEAR,
            MAG_FILTER_SHARPEN_5_TAP,
            MAG_FILTER_SHARPEN_4_TAP,
            MAG_FILTER_CONE_4_TAP,
            MAG_FILTER_CONE_2_TAP,
            MAG_FILTER_MAX,
        };

        enum FrameTextureType {
            FRAME_TEXTURE_PRE_PP = 0,
            FRAME_TEXTURE_POST_PP,
            FRAME_TEXTURE_FB,
            FRAME_TEXTURE_MAX,
        };

        struct ExposureCharaData {
            vec4 spot_coefficients[8];
            float_t spot_weight;
            GLuint query[3];
            GLuint query_data[3];

            ExposureCharaData();
            ~ExposureCharaData();

            void reset();
        };

        struct FrameTextureData {
            texture* texture;
            RenderTexture render_texture;
            FrameTextureType type;

            FrameTextureData();
            ~FrameTextureData();
        };

        struct FrameTexture {
            FrameTextureData data[4];
            bool capture;

            FrameTexture();
            ~FrameTexture();
        };

        int32_t downsample_count;
        int32_t downsample_max_count;
        int32_t ssaa;
        int32_t taa;
        int32_t mlaa;
        int32_t cam_blur;
        int32_t hd_res;
        int32_t base_downsample;
        int32_t ss_alpha_mask;
        RenderTexture rend_texture[5];
        RenderTexture taa_buffer[3];
        texture* taa_tex[3];
        RenderTexture aet_back_texture;
        texture* aet_back_tex;
        RenderTexture downsample_texture;
        RenderTexture reduce_texture[5];
        texture* reduce_tex[5];
        RenderTexture field_340;
        GLuint reduce_tex_draw;
        RenderTexture exposure_texture;
        GLuint exposure_tex;
        texture* exposure_history;
        RenderTexture field_3B8;
        RenderTexture field_3E8;
        GLuint tonemap_lut_texture;
        RenderTexture mlaa_buffer;
        RenderTexture temp_buffer;
        GLuint mlaa_area_texture;
        RenderTexture* sss_contour_texture;
        int32_t texture_counter;
        GLuint lens_shaft_query[3];
        GLuint lens_flare_query[3];
        GLuint lens_shaft_query_data[3];
        GLuint lens_flare_query_data[3];
        int32_t lens_flare_query_index;
        GLuint lens_flare_texture;
        GLuint lens_shaft_texture;
        GLuint lens_ghost_texture;
        int32_t lens_ghost_count;
        int32_t width;
        int32_t height;
        int32_t inner_width;
        int32_t inner_height;
        int32_t render_width[5];
        int32_t render_height[5];
        int32_t render_post_width[5];
        int32_t render_post_height[5];
        float_t render_post_width_scale;
        float_t render_post_height_scale;
        int32_t reduce_width[5];
        int32_t reduce_height[5];
        int32_t taa_texture_selector;
        int32_t taa_texture;
        float_t taa_blend;
        vec3 view_point;
        vec3 interest;
        vec3 view_point_prev;
        vec3 interest_prev;
        mat4 cam_view_projection;
        mat4 cam_view_projection_prev;
        int32_t stage_index;
        int32_t stage_index_prev;
        bool reset_exposure;
        int32_t screen_x_offset;
        int32_t screen_y_offset;
        int32_t screen_width;
        int32_t screen_height;
        int32_t update_lut;
        int32_t saturate_lock;
        ExposureCharaData exposure_chara_data[6];
        int32_t exposure_history_counter;
        int32_t exposure_query_index;
        vec3 lens_flare_pos;
        float_t lens_shaft_scale;
        float_t lens_shaft_inv_scale;
        float_t lens_flare_power;
        float_t field_A10;
        float_t lens_flare_appear_power;
        texture* render_textures_data[16];
        RenderTexture render_textures[16];
        texture* movie_textures_data[1];
        RenderTexture movie_textures[1];
        int32_t aet_back;
        renderer::DOF3* dof;
        texture* transparency_tex[1];
        renderer::Transparency* transparency;
        int32_t saturate_index;
        int32_t scene_fade_index;
        int32_t tone_trans_index;
        float_t saturate_coeff[2];
        vec3 scene_fade_color[2];
        float_t scene_fade_alpha[2];
        int32_t scene_fade_blend_func[2];
        vec3 tone_trans_scale[2];
        vec3 tone_trans_offset[2];
        vec3 tone_trans_start[2];
        vec3 tone_trans_end[2];
        tone_map_method tone_map;
        float_t exposure;
        float_t exposure_rate;
        int32_t auto_exposure;
        float_t gamma;
        float_t gamma_rate;
        int32_t saturate_power;
        MagFilterType mag_filter;
        float_t fade_alpha;
        int32_t fade;
        float_t lens_flare;
        float_t lens_shaft;
        float_t lens_ghost;
        vec3 radius;
        vec3 intensity;
        int32_t update;
        FrameTexture frame_texture[6];

        Render();
        ~Render();

        void apply_post_process(render_data_context& rend_data_ctx,
            const cam_data& cam, texture* light_proj_tex, int32_t npr_param);
        void bind_render_texture(p_gl_rend_state& p_gl_rend_st, bool aet_back = false);
        void calc_exposure_chara_data(render_data_context& rend_data_ctx, const cam_data& cam);
        void draw_quad(render_data_context& rend_data_ctx,
            int32_t width, int32_t height, float_t s0, float_t t0, float_t s1, float_t t1,
            float_t scale, float_t param_x, float_t param_y, float_t param_z, float_t param_w);
        void draw_lens_flare(render_data_context& rend_data_ctx, const cam_data& cam);
        bool frame_texture_cont_capture_set(bool value);
        void frame_texture_free();
        int32_t frame_texture_load(int32_t slot, FrameTextureType type, texture* tex);
        void frame_texture_reset();
        bool frame_texture_slot_capture_set(int32_t index);
        bool frame_texture_unload(int32_t slot, texture* tex);
        void free();
        bool get_auto_exposure();
        int32_t get_cam_blur();
        void get_dof_data(float_t& focus, float_t& focus_range, float_t& fuzzing_range, float_t& ratio);
        bool get_dof_enable();
        bool get_dof_update();
        float_t get_exposure();
        float_t get_exposure_rate();
        float_t get_gamma();
        float_t get_gamma_rate();
        vec3 get_intensity();
        vec3 get_lens();
        float_t get_lens_flare_appear_power();
        float_t get_lens_flare_power();
        MagFilterType get_mag_filter();
        int32_t get_mlaa();
        vec3 get_radius();
        float_t get_saturate_coeff();
        int32_t get_saturate_power();
        vec4 get_scene_fade();
        float_t get_scene_fade_alpha();
        int32_t get_scene_fade_blend_func();
        vec3 get_scene_fade_color();
        int32_t get_taa();
        vec2 get_taa_offset();
        tone_map_method get_tone_map();
        void get_tone_trans(vec3& start, vec3& end);
        void init_render_buffers(int32_t width, int32_t height,
            int32_t ssaa, int32_t hd_res, int32_t ss_alpha_mask);
        void init_post_process_buffers();
        int32_t movie_texture_set(texture* movie_texture);
        void movie_texture_free(texture* movie_texture);
        void post_proc();
        void pre_proc(render_data_context& rend_data_ctx);
        int32_t render_texture_set(texture* render_texture, bool task_photo);
        void render_texture_free(texture* render_texture, bool task_photo);
        void reset_saturate_coeff(int32_t index, bool lock);
        void reset_scene_fade(int32_t index);
        void reset_tone_trans(int32_t index);
        void resize(int32_t width, int32_t height);
        void set_auto_exposure(bool value);
        void set_cam_blur(int32_t value);
        void set_dof_data(float_t focus, float_t focus_range, float_t fuzzing_range, float_t ratio);
        void set_dof_enable(bool value);
        void set_dof_update(bool value);
        void set_exposure(float_t value);
        void set_exposure_rate(float_t value);
        void set_gamma(float_t value);
        void set_gamma_rate(float_t value);
        void set_intensity(const vec3& value);
        void set_lens(vec3 value);
        void set_mag_filter(MagFilterType value);
        void set_mlaa(int32_t value);
        void set_radius(const vec3& value);
        void set_saturate_coeff(float_t value, int32_t index, bool lock);
        void set_saturate_power(int32_t value);
        void set_scene_fade(const vec4& value, int32_t index);
        void set_scene_fade_alpha(float_t value, int32_t index);
        void set_scene_fade_blend_func(int32_t value, int32_t index);
        void set_scene_fade_color(const vec3& value, int32_t index);
        void set_screen_res(int32_t x_offset, int32_t y_offset, int32_t width, int32_t height);
        void set_taa(int32_t value);
        void set_tone_map(tone_map_method value);
        void set_tone_trans(const vec3& start, const vec3& end, int32_t index);
        void transparency_combine(render_data_context& rend_data_ctx, float_t alpha);
        void transparency_copy(render_data_context& rend_data_ctx);
        void update_res(bool set, int32_t base_downsample);

    private:
        void apply_mlaa(render_data_context& rend_data_ctx,
            int32_t destination, int32_t source, int32_t ss_alpha_mask);
        void apply_tone_map(render_data_context& rend_data_ctx, texture* light_proj_tex, int32_t npr_param);
        void calc_exposure(render_data_context& rend_data_ctx, const cam_data& cam);
        void calc_gaussian_blur(render_data_context& rend_data_ctx, float_t start, float_t step,
            int32_t kernel_size, float_t radius_scale, float_t intensity_scale);
        void calc_taa_blend();
        void copy_to_frame_texture(render_data_context& rend_data_ctx,
            GLuint pre_pp_tex, int32_t wight, int32_t height, GLuint post_pp_tex);
        void draw_lens_ghost(render_data_context& rend_data_ctx);
        void downsample(render_data_context& rend_data_ctx);
        void generate_mlaa_area_texture();
        void get_blur(render_data_context& rend_data_ctx);
        void update_tone_map_lut(p_gl_rend_state& p_gl_rend_st);
    };
}

