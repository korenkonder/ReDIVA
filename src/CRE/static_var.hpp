/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/light_param/fog.hpp"
#include "../KKdLib/light_param/light.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include <glad/glad.h>

enum draw_pass_3d_type {
    DRAW_PASS_3D_OPAQUE = 0,
    DRAW_PASS_3D_TRANSLUCENT,
    DRAW_PASS_3D_TRANSPARENT,
    DRAW_PASS_3D_MAX,
};

enum uniform_name {
    U_NONE = 0,
    U_ALPHA_MASK,
    U_ALPHA_TEST,
    U_ANISO,
    U_AET_BACK,
    U_TEXTURE_BLEND,
    U_CHARA_COLOR,
    U_CLIP_PLANE,
    U08,
    U_DEPTH_PEEL,
    U0A,
    U0B,
    U_ALPHA_BLEND,
    U_RIPPLE_EMIT,
    U_ESM_FILTER,
    U_EXPOSURE,
    U_SCENE_FADE,
    U_FADE,
    U12,
    U_FLARE,
    U_FOG_HEIGHT,
    U_FOG,
    U16,
    U_GAUSS,
    U18,
    U_IMAGE_FILTER,
    U_INSTANCE,
    U_TONE_CURVE,
    U_LIGHT_PROJ,
    U_MAGNIFY,
    U_MEMBRANE,
    U_MLAA,
    U_MLAA_SEARCH,
    U_MORPH_COLOR,
    U_MORPH,
    U_MOVIE,
    U24,
    U25,
    U26,
    U_NPR,
    U_LIGHT_1,
    U_REFLECT,
    U_REDUCE,
    U_SELF_SHADOW,
    U_SHADOW,
    U2D,
    U2E,
    U_SHOW_VECTOR,
    U_BONE_MAT,
    U_SNOW_PARTICLE,
    U_SPECULAR_IBL,
    U_COMBINER,
    U_TEX_0_TYPE,
    U_TEX_1_TYPE,
    U_SSS_FILTER,
    U37,
    U_STAR,
    U_TEXTURE_COUNT,
    U_ENV_MAP,
    U_RIPPLE,
    U_TRANSLUCENCY,
    U_NORMAL,
    U_TRANSPARENCY,
    U_WATER_REFLECT,
    U40,
    U41,
    U_LIGHT_0,
    U_SPECULAR,
    U_TONE_MAP,
    U45,
    U_DOF,       // Added
    U_DOF_STAGE, // Added
    U_MAX,
    U_INVALID = -1,
};

extern int32_t sv_max_texture_buffer_size;
extern int32_t sv_max_texture_size;
extern int32_t sv_max_texture_max_anisotropy;

extern bool sv_anisotropy_changed;
extern int32_t sv_anisotropy;

extern int32_t uniform_value[U_MAX];

extern const vec3 sv_rgb_to_luma;
extern const mat3 sv_rgb_to_ypbpr;
extern const mat3 sv_ypbpr_to_rgb;

extern const vec4u8 color_black;
extern const vec4u8 color_grey;
extern const vec4u8 color_white;
extern const vec4u8 color_red;
extern const vec4u8 color_green;
extern const vec4u8 color_blue;
extern const vec4u8 color_cyan;
extern const vec4u8 color_magenta;
extern const vec4u8 color_yellow;
extern const vec4u8 color_dark_red;
extern const vec4u8 color_dark_green;
extern const vec4u8 color_dark_blue;
extern const vec4u8 color_dark_cyan;
extern const vec4u8 color_dark_magenta;
extern const vec4u8 color_dark_yellow;

extern void sv_anisotropy_set(int32_t value);
extern float_t get_anim_frame_speed();
extern float_t get_delta_frame();
extern uint32_t get_frame_counter();
extern float_t get_target_anim_fps();
extern void uniform_value_reset();
