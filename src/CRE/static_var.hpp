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
#include "gl.hpp"

enum draw_pass_3d_type {
    DRAW_PASS_3D_OPAQUE = 0,
    DRAW_PASS_3D_TRANSLUCENT,
    DRAW_PASS_3D_TRANSPARENT,
    DRAW_PASS_3D_MAX,
};

enum gpu_vendor {
    GPU_VENDOR_UNKNOWN = 0,
    GPU_VENDOR_AMD,
    GPU_VENDOR_APPLE,
    GPU_VENDOR_NVIDIA,
    GPU_VENDOR_INTEL,
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
    U_DEPTH,
    U0B,
    U_ALPHA_BLEND,
    U_RIPPLE_EMIT,
    U_ESM_FILTER,
    U_EXPOSURE,
    U_SCENE_FADE,
    U_FADE,
    U_STAGE_AMBIENT,
    U_FLARE,
    U_FOG_STAGE,
    U_FOG_CHARA,
    U16,
    U_GAUSS,
    U_EYE_LENS,
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
    U_NPR_NORMAL,
    U_NPR,
    U_STAGE_SHADOW2,
    U_REFLECT,
    U_REDUCE,
    U_CHARA_SHADOW,
    U_CHARA_SHADOW2,
    U2D,
    U2E,
    U_SHOW_VECTOR,
    U_SKINNING,
    U_SNOW_PARTICLE,
    U_SPECULAR_IBL,
    U_COMBINER,
    U_TEX_0_TYPE,
    U_TEX_1_TYPE,
    U_SSS_FILTER,
    U_SSS_CHARA,
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
    U_STAGE_SHADOW,
    U_SPECULAR,
    U_TONE_MAP,
    U45,
    U_MAX,
    U_INVALID = -1,

    U_DOF       = U_TEX_0_TYPE,
    U_DOF_STAGE = U_TEX_1_TYPE,
};

struct uniform_value {
    int32_t arr[U_MAX];
};

extern int32_t sv_max_texture_size;
extern int32_t sv_max_texture_max_anisotropy;
extern int32_t sv_max_uniform_buffer_size;
extern int32_t sv_max_storage_buffer_size;
extern int32_t sv_min_uniform_buffer_alignment;
extern int32_t sv_min_storage_buffer_alignment;

extern bool sv_anisotropy_changed;
extern int32_t sv_anisotropy;

extern bool sv_better_reflect;

extern gpu_vendor sv_gpu_vendor;

extern bool sv_shared_storage_uniform_buffer;

extern const vec3 sv_rgb_to_luma;
extern const mat3 sv_rgb_to_ypbpr;
extern const mat3 sv_ypbpr_to_rgb;

extern void sv_anisotropy_set(int32_t value);
