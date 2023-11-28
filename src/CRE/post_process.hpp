/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include "GL/uniform_buffer.hpp"
#include "post_process/blur.hpp"
#include "post_process/dof.hpp"
#include "post_process/exposure.hpp"
#include "post_process/tone_map.hpp"
#include "post_process/transparency.hpp"
#include "camera.hpp"

enum post_process_frame_texture_type {
    POST_PROCESS_FRAME_TEXTURE_PRE_PP = 0,
    POST_PROCESS_FRAME_TEXTURE_POST_PP,
    POST_PROCESS_FRAME_TEXTURE_FB,
    POST_PROCESS_FRAME_TEXTURE_MAX,
};

enum post_process_mag_filter_type {
    POST_PROCESS_MAG_FILTER_NEAREST = 0,
    POST_PROCESS_MAG_FILTER_BILINEAR,
    POST_PROCESS_MAG_FILTER_SHARPEN_5_TAP,
    POST_PROCESS_MAG_FILTER_SHARPEN_4_TAP,
    POST_PROCESS_MAG_FILTER_CONE_4_TAP,
    POST_PROCESS_MAG_FILTER_CONE_2_TAP,
    POST_PROCESS_MAG_FILTER_MAX,
};

struct sun_quad_shader_data {
    vec4 g_transform[4];
    vec4 g_emission;
};

struct post_process_frame_texture_render_texture {
    texture* texture;
    RenderTexture render_texture;
    post_process_frame_texture_type type;

    post_process_frame_texture_render_texture();
    ~post_process_frame_texture_render_texture();
};


struct post_process_frame_texture {
    post_process_frame_texture_render_texture render_textures[4];
    bool capture;

    post_process_frame_texture();
    ~post_process_frame_texture();
};

struct post_process {
    bool ssaa;
    bool mlaa;
    int32_t ss_alpha_mask;
    RenderTexture rend_texture;
    RenderTexture buf_texture;
    RenderTexture aet_back_texture;
    texture* aet_back_tex;
    RenderTexture transparency_texture;
    RenderTexture screen_texture;
    texture* render_textures_data[16];
    RenderTexture render_textures[16];
    texture* movie_textures_data[1];
    RenderTexture movie_textures[1];
    int32_t aet_back;
    post_process_blur* blur;
    post_process_dof* dof;
    post_process_exposure* exposure;
    post_process_tone_map* tone_map;
    post_process_transparency* transparency;
    GLuint samplers[3];
    GLuint query_vao;
    GLuint lens_ghost_vao;
    GLuint lens_ghost_vbo;
    RenderTexture mlaa_buffer;
    RenderTexture temp_buffer;
    GLuint mlaa_area_texture;
    RenderTexture* sss_contour_texture;
    GL::UniformBuffer sun_quad_ubo;
    int32_t texture_counter;
    GLuint lens_shaft_query[3];
    GLuint lens_flare_query[3];
    GLuint lens_shaft_query_data[3];
    GLuint lens_flare_query_data[3];
    int32_t lens_flare_query_index;
    texture* lens_flare_texture;
    texture* lens_shaft_texture;
    texture* lens_ghost_texture;
    int32_t lens_ghost_count;
    vec3 lens_flare_pos;
    float_t lens_shaft_scale;
    float_t lens_shaft_inv_scale;
    float_t lens_flare_power;
    float_t field_A10;
    float_t lens_flare_appear_power;
    int32_t render_width;
    int32_t render_height;
    vec3 view_point;
    vec3 interest;
    vec3 view_point_prev;
    vec3 interest_prev;
    int32_t stage_index;
    int32_t stage_index_prev;
    bool reset_exposure;
    int32_t sprite_width;
    int32_t sprite_height;
    int32_t screen_x_offset;
    int32_t screen_y_offset;
    int32_t screen_width;
    int32_t screen_height;
    post_process_mag_filter_type mag_filter;
    post_process_frame_texture frame_texture[6];

    post_process();
    ~post_process();

    void apply(camera* cam, texture* light_proj_tex, int32_t npr_param);
    void apply_mlaa(GLuint* samplers, int32_t ss_alpha_mask);
    void ctrl(camera* cam);
    void draw_lens_flare(camera* cam);
    void draw_lens_ghost(RenderTexture* rt);
    void init_fbo(int32_t render_width, int32_t render_height,
        int32_t sprite_width, int32_t sprite_height, int32_t screen_width, int32_t screen_height);
    bool frame_texture_cont_capture_set(bool value);
    void frame_texture_free();
    int32_t frame_texture_load(int32_t slot, post_process_frame_texture_type type, texture* tex);
    void frame_texture_reset();
    void frame_texture_reset_capture();
    bool frame_texture_slot_capture_set(int32_t index);
    bool frame_texture_unload(int32_t slot, texture* tex);
    void get_frame_texture(GLuint tex, post_process_frame_texture_type type);
    int32_t movie_texture_set(texture* movie_texture);
    void movie_texture_free(texture* movie_texture);
    int32_t render_texture_set(texture* render_texture, bool task_photo);
    void render_texture_free(texture* render_texture, bool task_photo);
    void reset();
    void set_render_texture(bool aet_back = false);
};

