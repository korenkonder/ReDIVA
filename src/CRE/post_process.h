/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "post_process/aa.h"
#include "post_process/blur.h"
#include "post_process/dof.h"
#include "post_process/exposure.h"
#include "post_process/tone_map.h"
#include "camera.h"

typedef enum post_process_mag_filter_type {
    POST_PROCESS_MAG_FILTER_NEAREST       = 0,
    POST_PROCESS_MAG_FILTER_BILINEAR      = 1,
    POST_PROCESS_MAG_FILTER_SHARPEN_5_TAP = 2,
    POST_PROCESS_MAG_FILTER_SHARPEN_4_TAP = 3,
    POST_PROCESS_MAG_FILTER_CONE_4_TAP    = 4,
    POST_PROCESS_MAG_FILTER_CONE_2_TAP    = 5,
    POST_PROCESS_MAG_FILTER_MAX           = 6,
} post_process_mag_filter_type;

typedef struct post_process_struct {
    bool ssaa;
    bool mlaa;
    int32_t parent_bone_node;
    render_texture render_texture;
    render_texture buf_texture;
    render_texture sss_contour_texture;
    render_texture pre_texture;
    render_texture post_texture;
    render_texture fbo_texture;
    render_texture alpha_layer_texture;
    render_texture screen_texture;
    texture* render_textures_data[16];
    render_texture render_textures[16];
    texture* movie_textures_data[1];
    render_texture movie_textures[1];
    post_process_aa* aa;
    post_process_blur* blur;
    post_process_dof* dof;
    post_process_exposure* exposure;
    post_process_tone_map* tone_map;
    GLuint samplers[2];
    shader_glsl alpha_layer_shader;
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
} post_process_struct;

extern void post_process_init(post_process_struct* pp);
extern void post_process_apply(post_process_struct* pp, camera* cam, texture* light_proj_tex, int32_t npr_param);
extern void post_process_init_fbo(post_process_struct* pp, int32_t render_width, int32_t render_height,
    int32_t sprite_width, int32_t sprite_height, int32_t screen_width, int32_t screen_height);
extern void post_process_reset(post_process_struct* pp);
extern int32_t post_process_movie_texture_set(post_process_struct* pp, texture* movie_texture);
extern void post_process_movie_texture_free(post_process_struct* pp, texture* movie_texture);
extern int32_t post_process_render_texture_set(post_process_struct* pp, texture* render_texture, bool task_photo);
extern void post_process_render_texture_free(post_process_struct* pp, texture* render_texture, bool task_photo);
extern void post_process_update(post_process_struct* pp, camera* cam);
extern void post_process_free(post_process_struct* pp);
