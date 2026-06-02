/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/light_param/glow.hpp"
#include "../KKdLib/vec.hpp"
#include "GL/uniform_buffer.hpp"
#include "camera.hpp"
#include "render_texture.hpp"

#define TONE_MAP_SAT_GAMMA_SAMPLES 32

enum MagFilterType {
    MAG_FILTER_NEAREST = 0,
    MAG_FILTER_BILINEAR,
    MAG_FILTER_SHARPEN_5_TAP,
    MAG_FILTER_SHARPEN_4_TAP,
    MAG_FILTER_CONE_4_TAP,
    MAG_FILTER_CONE_2_TAP,
    MAG_FILTER_MAX,
};

enum ToneMapMethod {
    TONE_MAP_YCC_EXPONENT = 0,
    TONE_MAP_RGB_LINEAR,
    TONE_MAP_RGB_LINEAR2,
    TONE_MAP_MAX,
};

struct cam_data;
struct p_gl_rend_state;
struct render_data_context;

namespace renderer {
    class DOF3;
    class Transparency;
};

namespace rndr {
    class Render {
    public:
        enum CaptureSlot {
            CAPTURE_SLOT_CONT = 0,
            CAPTURE_SLOT_A,
            CAPTURE_SLOT_B,
            CAPTURE_SLOT_C,
            CAPTURE_SLOT_D,
            CAPTURE_SLOT_E,
            CAPTURE_SLOT_MAX,
        };

        enum CaptureType {
            CAPTURE_PRE_PP = 0,
            CAPTURE_POST_PP,
            CAPTURE_FB,
            CAPTURE_MAX,
        };

        static const int32_t fb_level_max = 5;
        static const int32_t num_scr_tex = 3;
        static const int32_t num_blur_tex = 5;
        static const int32_t num_fb_copy = 16;
        static const int32_t num_fb_movie = 1;
        static const int32_t exposure_tex_width = 32;
        static const int32_t tone_map_tex_width = 16 * TONE_MAP_SAT_GAMMA_SAMPLES;
        static const int32_t mag_filter_tex_width = 256;
        static const int32_t refract_tex_width = 512;
        static const int32_t refract_tex_height = 256;
        static const int32_t fade_tex_width = 512;
        static const int32_t fade_tex_height = 256;
        static const int32_t num_queries = 3;

        struct ExposureCharaData {
            vec4 spot_coefficients[8];
            float_t spot_weight;
            GLuint query[num_queries];
            GLuint query_data[num_queries];

            ExposureCharaData();
            ~ExposureCharaData();

            void reset();
        };

        struct CaptureData {
            texture* txhd;
            RenderTexture fbo;
            CaptureType type;

            CaptureData();
            ~CaptureData();
        };

        struct Capture {
            CaptureData data[4];
            bool capture;

            Capture();
            ~Capture();
        };

        int32_t fb_level;
        int32_t blur_level;
        int32_t anti_alias;
        int32_t temporal_anti_alias;
        int32_t morphological_anti_alias;
        int32_t cam_blur;
        int32_t min_render;
        int32_t min_res;
        int32_t ss_alpha_mask;
        RenderTexture fb_fbo[fb_level_max];
        RenderTexture scr_fbo[num_scr_tex];
        texture* scr_txhd[num_scr_tex];
        RenderTexture composite_back_fbo;
        texture* composite_back_txhd;
        RenderTexture work_fbo;
        RenderTexture blur_fbo[5];
        texture* blur_txhd[5];
        RenderTexture bloom_fbo;
        GLuint bloom_tex;
        RenderTexture exposure_avg_fbo;
        GLuint exposure_avg_tex;
        texture* exposure_txhd;
        RenderTexture refract_fbo;
        RenderTexture fade_fbo;
        GLuint tone_map_tex;
        RenderTexture mlaa_fbo;
        RenderTexture user_fbo;
        GLuint mlaa_area_tex;
        RenderTexture* contour_fbo;
        int32_t tex_count;
        GLuint sun_queries_center[num_queries];
        GLuint sun_queries_sphere[num_queries];
        GLuint sun_pixels_center[num_queries];
        GLuint sun_pixels_sphere[num_queries];
        int32_t query_read_index;
        GLuint flare_textures[3];
        int32_t num_ghosts;
        int32_t scr_width;
        int32_t scr_height;
        int32_t fb_width_org;
        int32_t fb_height_org;
        int32_t fb_width[5];
        int32_t fb_height[5];
        int32_t fb_tex_width[5];
        int32_t fb_tex_height[5];
        float_t fb_tex_width_scale;
        float_t fb_tex_height_scale;
        int32_t fb_work_width[5];
        int32_t fb_work_height[5];
        int32_t scr_index;
        int32_t scr_disp_index;
        float_t scr_tex_rate;
        vec3 cam_pos;
        vec3 cam_intr;
        vec3 prev_cam_pos;
        vec3 prev_cam_intr;
        mat4 cam_view_proj;
        mat4 prev_cam_view_proj;
        int32_t stage_num;
        int32_t prev_stage_num;
        bool reset_exposure;
        int32_t view_x;
        int32_t view_y;
        int32_t view_w;
        int32_t view_h;
        int32_t recalc_tone_map;
        int32_t saturate_lock;
        ExposureCharaData exposure_chara_data[6];
        int32_t exposure_write_index;
        int32_t exposure_chara_write_index;
        vec3 flare_pos;
        float_t flare_scale;
        float_t flare_scale_default;
        float_t flare_alpha;
        float_t flare_alpha_enable;
        float_t flare_alpha_offset;
        const texture* fb_copy_txhd[num_fb_copy];
        RenderTexture fb_copy_fbo[num_fb_copy];
        const texture* fb_movie_txhd[num_fb_movie];
        RenderTexture fb_movie_fbo[num_fb_movie];
        int32_t composite_back;
        renderer::DOF3* dof;
        texture* transparency_tex[1];
        renderer::Transparency* transparency;
        int32_t saturate_index;
        int32_t fade_index;
        int32_t tone_trans_index;
        float_t saturate_coef[2];
        float_t fade_color[2][3];
        float_t fade_rate[2];
        int32_t fade_blend_func[2];
        float_t tone_trans_scale[2][3];
        float_t tone_trans_offset[2][3];
        float_t tone_trans_start[2][3];
        float_t tone_trans_end[2][3];
        ToneMapMethod tone_map_method;
        float_t exposure;
        float_t exposure_rate;
        int32_t auto_exposure;
        float_t gamma;
        float_t gamma_rate;
        int32_t saturate_power;
        MagFilterType mag_filter;
        float_t fade_tex_rate;
        int32_t fade_tex_type;
        float_t flare_coef[3];
        float_t sigma[3];
        float_t filter_inten[3];
        int32_t update;
        Capture capture[6];

    private:
        void draw_quad_simple(render_data_context& rend_data_ctx, float_t trnsl);
        void draw_quad_sun(render_data_context& rend_data_ctx, const mat4& transform, const vec4& emission);
        void draw_quad_ghost(int32_t index, float_t opacity, mat4* mat, float_t*& data);
        void measure_exposure(render_data_context& rend_data_ctx, const cam_data& cam);
        void reduce_texture(render_data_context& rend_data_ctx);
        void handle_blur_texture(render_data_context& rend_data_ctx);
        void tone_map(render_data_context& rend_data_ctx, texture* light_proj_tex, int32_t npr_param);
        void tone_map(render_data_context& rend_data_ctx, int64_t a3, int64_t a4,
            int32_t render_width, int32_t render_height, GLuint color_tex, GLuint bloom_tex,
            GLuint composite_back_tex, GLuint litproj_tex, float_t litproj_quality, int32_t width, int32_t height,
            float_t s0, float_t t0, const vec4& exposure, const vec3& tone_scale, const vec3& tone_offset,
            int32_t fade_blend_func, const vec4& fade_color, int32_t tone_map, float_t gamma,
            const vec2& flare_pos, float_t flare_scale, float_t flare_coef, float_t shaft_coef,
            GLuint contour_color_tex, GLuint contour_depth_tex, GLuint scene_depth_tex,
            bool npr1, bool a31, bool npr_mask); // Added
        void mlaa_calc_area_value(float_t& val_left, float_t& val_right,
            int32_t cross1, int32_t cross2, int32_t dleft, int32_t dright);
        void mlaa_calc_area_texture(uint8_t* data, int32_t cross1, int32_t cross2);
        void mlaa_gen_area_texture();
        void mlaa(render_data_context& rend_data_ctx,
            int32_t destination, int32_t source, int32_t ss_alpha_mask);
        void set_gauss_coef(render_data_context& rend_data_ctx, float_t start, float_t step,
            int32_t kernel_size, float_t radius_scale, float_t intensity_scale);
        void set_tone_map_texture(p_gl_rend_state& p_gl_rend_st);
        void calc_scr_tex_rate();

    public:
        Render();
        ~Render();

        void create_render_buffer(int32_t width, int32_t height,
            int32_t aa, int32_t minify, int32_t ss_alpha_mask);
        void create_other();
        void destroy();
        void calc_draw_size(bool first_time, int32_t min_res);
        void begin_render(p_gl_rend_state& p_gl_rend_st, bool composite_back = false);
        void end_render(p_gl_rend_state& p_gl_rend_st);
        void begin_render_transparency(render_data_context& rend_data_ctx, RenderTexture* rt);
        void end_render_transparency(render_data_context& rend_data_ctx, RenderTexture* rt, float_t alpha);
        void pre_proc(render_data_context& rend_data_ctx);
        void post_proc();
        void draw_quad(render_data_context& rend_data_ctx,
            int32_t texel_w, int32_t texel_h, float_t s0, float_t t0, float_t s1, float_t t1,
            float_t scale, float_t param_x, float_t param_y, float_t param_z, float_t param_w);
        void query_chara_exposure(render_data_context& rend_data_ctx, const cam_data& cam);
        void draw_sun_request(const GLuint* flares, float_t scale, int32_t ghosts);
        void draw_sun(render_data_context& rend_data_ctx, const cam_data& cam);
        void draw_ghost(render_data_context& rend_data_ctx);
        void draw_flare(render_data_context& rend_data_ctx,
            const cam_data& cam, texture* light_proj_tex, int32_t npr_param);
        void draw_contour(render_data_context& rend_data_ctx, const cam_data& cam);
        int32_t register_fb_copy(const texture* txhd, bool task_photo = false);
        void remove_fb_copy(const texture* txhd, bool task_photo = false);
        int32_t register_fb_movie(const texture* txhd);
        void remove_fb_movie(const texture* txhd);
        GLuint get_scr_tex();
        void perspective(vec2& offset, float_t scale = 1.0f) const;

        void init_capture();
        void free_capture();
        int32_t register_fb_man_cap(CaptureSlot slot, CaptureType type, texture* txhd);
        bool unregister_fb_man_cap(CaptureSlot slot, texture* txhd);
        bool is_fb_man_cap();
        bool set_frame_texture_aoto_cap(bool value);
        bool capture_slot_enable(CaptureSlot slot);
        void process_capture(render_data_context& rend_data_ctx,
            GLuint pre_pp_tex, int32_t wight, int32_t height, GLuint post_pp_tex);

        void set_tone_map_method(int32_t value);
        void set_exposure(float_t value);
        void set_exposure_rate(float_t value);
        void set_auto_exposure(int32_t value);
        void set_gamma(float_t value);
        void set_gamma_rate(float_t value);
        void set_saturate_power(int32_t value);
        void set_saturate_coef(float_t value, int32_t index = 0, bool lock = false);
        void set_saturate_coef_default(int32_t index = 0, bool lock = false);
        void set_mag_filter(int32_t value);
        void set_temporal_aa(int32_t value);
        void set_morphological_aa(int32_t value);
        void set_cam_blur(int32_t value);
        void set_fade_color(const vec4* in_value, int32_t index = 0);
        void set_fade_color(const float_t* value, int32_t index = 0);
        void set_fade_rate(float_t value, int32_t index = 0);
        void set_fade_color_default(int32_t index = 0);
        void set_fade_blend_func(int32_t value, int32_t index = 0);
        void set_tone_trans(const float_t* start, const float_t* end, int32_t index = 0);
        void set_tone_trans_default(int32_t index = 0);
        void set_fade_tex_rate(float_t value);
        void set_fade_tex_type(int32_t value);
        void set_flare_coef(const float_t* value);
        void set_sigma(const float_t* value);
        void set_intensity(const float_t* value);
        void set_viewport(int32_t x, int32_t y, int32_t w, int32_t h);
        void get_tone_map_method(int32_t* value) const;
        void get_exposure(float_t* value) const;
        void get_exposure_rate(float_t* value) const;
        void get_auto_exposure(int32_t* value) const;
        void get_gamma(float_t* value) const;
        void get_gamma_rate(float_t* value) const;
        void get_saturate_power(int32_t* value) const;
        void get_saturate_coef(float_t* value) const;
        void get_mag_filter(int32_t* value) const;
        void get_temporal_aa(int32_t* value) const;
        void get_morphological_aa(int32_t* value) const;
        void get_cam_blur(int32_t* value) const;
        void get_fade_color(vec4* value) const;
        void get_fade_color(float_t* value) const;
        void get_fade_rate(float_t* value) const;
        void get_fade_blend_func(int32_t* value) const;
        void get_tone_trans(float_t* start_arg, float_t* end_arg) const;
        void get_flare_coef(float_t* value) const;
        void get_sigma(float_t* value) const;
        void get_intensity(float_t* value) const;
        void get_render_param(float_t* fb_width_arg, float_t* fb_height_arg,
            float_t* fb_tex_width_arg, float_t* fb_tex_height_arg) const;

        void enable_dof_set(bool value);
        void update_dof_set(bool value); // Added
        bool get_dof_enable();
        bool get_dof_update(); // Added
        void get_dof_data(float_t& focus, float_t& focus_range, float_t& fuzzing_range, float_t& ratio);
        void set_dof_data(float_t focus, float_t focus_range, float_t fuzzing_range, float_t ratio);

        void resize(int32_t width, int32_t height); // Added
    };
}

