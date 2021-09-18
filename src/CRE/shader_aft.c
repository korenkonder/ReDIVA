/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader_aft.h"
#include "gl_state.h"

typedef enum shader_aft_sub_enum {
    SHADER_AFT_SUB_SHADER_FFP           = 0x00,
    SHADER_AFT_SUB_BLINN_VERT           = 0x01,
    SHADER_AFT_SUB_BLINN_FRAG           = 0x02,
    SHADER_AFT_SUB_ITEM_BLINN           = 0x03,
    SHADER_AFT_SUB_STAGE_BLINN          = 0x04,
    SHADER_AFT_SUB_SKIN_DEFAULT         = 0x05,
    SHADER_AFT_SUB_SSS_SKIN             = 0x06,
    SHADER_AFT_SUB_SSS_FILTER           = 0x07,
    SHADER_AFT_SUB_HAIR_DEFAULT         = 0x08,
    SHADER_AFT_SUB_HAIR_ANISO           = 0x09,
    SHADER_AFT_SUB_HAIR_NPR1            = 0x0A,
    SHADER_AFT_SUB_CLOTH_DEFAULT        = 0x0B,
    SHADER_AFT_SUB_CLOTH_ANISO          = 0x0C,
    SHADER_AFT_SUB_CLOTH_NPR1           = 0x0D,
    SHADER_AFT_SUB_TIGHTS               = 0x0E,
    SHADER_AFT_SUB_SKY_DEFAULT          = 0x0F,
    SHADER_AFT_SUB_EYE_BALL             = 0x10,
    SHADER_AFT_SUB_EYE_LENS             = 0x11,
    SHADER_AFT_SUB_GLASS_EYE            = 0x12,
    SHADER_AFT_SUB_MEMBRANE             = 0x13,
    SHADER_AFT_SUB_SHADOWMAP            = 0x14,
    SHADER_AFT_SUB_ESM                  = 0x15,
    SHADER_AFT_SUB_ESM_GAUSS            = 0x16,
    SHADER_AFT_SUB_ESM_FILTER           = 0x17,
    SHADER_AFT_SUB_LIT_PROJ             = 0x18,
    SHADER_AFT_SUB_SIMPLE               = 0x19,
    SHADER_AFT_SUB_SILHOUETTE           = 0x1A,
    SHADER_AFT_SUB_LAMBERT              = 0x1B,
    SHADER_AFT_SUB_CONSTANT             = 0x1C,
    SHADER_AFT_SUB_PEEL                 = 0x1D,
    SHADER_AFT_SUB_TONE_MAP             = 0x1E,
    SHADER_AFT_SUB_TONE_MAP_NPR1        = 0x1F,
    SHADER_AFT_SUB_REDUCE_TEX           = 0x20,
    SHADER_AFT_SUB_MAGNIFY              = 0x21,
    SHADER_AFT_SUB_MLAA                 = 0x22,
    SHADER_AFT_SUB_CONTOUR              = 0x23,
    SHADER_AFT_SUB_EXPOSURE             = 0x24,
    SHADER_AFT_SUB_PP_GAUSS             = 0x25,
    SHADER_AFT_SUB_SUN                  = 0x26,
    SHADER_AFT_SUB_FADE                 = 0x27,
    SHADER_AFT_SUB_WATER01              = 0x28,
    SHADER_AFT_SUB_WATER02              = 0x29,
    SHADER_AFT_SUB_WATER_RING           = 0x2A,
    SHADER_AFT_SUB_WATER_PARTICLE       = 0x2B,
    SHADER_AFT_SUB_SNOW_PARTICLE        = 0x2C,
    SHADER_AFT_SUB_LEAF_PARTICLE        = 0x2D,
    SHADER_AFT_SUB_STAR                 = 0x2E,
    SHADER_AFT_SUB_SNOW_RING            = 0x2F,
    SHADER_AFT_SUB_SNOW_FOOTPRINT       = 0x30,
    SHADER_AFT_SUB_SNOW_TEX_SPACE_LIGHT = 0x31,
    SHADER_AFT_SUB_SNOW_CALC_NORMAL     = 0x32,
    SHADER_AFT_SUB_FLOOR                = 0x33,
    SHADER_AFT_SUB_PUDDLE               = 0x34,
    SHADER_AFT_SUB_SIMPLE_REFLECT       = 0x35,
    SHADER_AFT_SUB_SIMPLE_REFRACT       = 0x36,
    SHADER_AFT_SUB_RIPPLE_EMIT          = 0x37,
    SHADER_AFT_SUB_RAIN                 = 0x38,
    SHADER_AFT_SUB_VOLUME_LIGHT         = 0x39,
    SHADER_AFT_SUB_FENCE_ALPHA          = 0x3A,
    SHADER_AFT_SUB_RIPPLE               = 0x3B,
    SHADER_AFT_SUB_FOG_PTCL             = 0x3C,
    SHADER_AFT_SUB_PARTICLE             = 0x3D,
    SHADER_AFT_SUB_GLITTER_PARTICLE     = 0x3E,
    SHADER_AFT_SUB_SHOW_VECTOR          = 0x3F,
    SHADER_AFT_SUB_FONT                 = 0x40,
    SHADER_AFT_SUB_MOVIE                = 0x41,
    SHADER_AFT_SUB_IMGFILTER            = 0x42,
    SHADER_AFT_SUB_SPRITE               = 0x43,
    SHADER_AFT_SUB_SHADER_END           = 0x44,
} shader_aft_sub_enum;

static const int32_t blinn_vert_vpt_unival_max[] = {
    1, 1, 0, 1, 0, 0, 0, 3, 1, 1, 0,
};

static const int32_t blinn_vert_fpt_unival_max[] = {
    0, 0, 2, 1, 0, 1, 1, 3, 0, 0, 1,
};

static const int32_t blinn_frag_vpt_unival_max[] = {
    1, 1, 0, 1, 0, 0, 0, 3, 1, 1, 0,
};

static const int32_t blinn_frag_fpt_unival_max[] = {
    0, 0, 2, 1, 1, 1, 1, 3, 0, 0, 1,
};

static const int32_t item_blinn_vpt_unival_max[] = {
    1, 1, 1, 0, 0, 1, 0, 0, 0, 2, 0, 1, 0, 0,
};

static const int32_t item_blinn_fpt_unival_max[] = {
    1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
};

static const int32_t stage_blinn_vpt_unival_max[] = {
    1, 1, 1, 1, 0, 0, 1, 0, 3, 0, 0,
};

static const int32_t stage_blinn_fpt_unival_max[] = {
    0, 0, 0, 0, 2, 1, 1, 1, 3, 1, 1,
};

static const int32_t skin_default_vpt_unival_max[] = {
    1, 1, 1, 0, 2, 0, 1, 0, 0, 0,
};

static const int32_t skin_default_fpt_unival_max[] = {
    1, 0, 1, 1, 0, 1, 0, 1, 1, 1,
};

static const int32_t sss_skin_vpt_unival_max[] = {
    1, 1, 0, 0, 0, 1, 0, 0,
};

static const int32_t sss_skin_fpt_unival_max[] = {
    1, 0, 1, 1, 1, 0, 1, 1,
};

static const int32_t sss_filter_vpt_unival_max[] = {
    0, 3, 0,
};

static const int32_t sss_filter_fpt_unival_max[] = {
    1, 3, 1,
};

static const int32_t hair_default_vpt_unival_max[] = {
    1, 1, 0, 0, 1, 0, 0, 3, 0, 2, 1, 0, 0, 0,
};

static const int32_t hair_default_fpt_unival_max[] = {
    1, 0, 2, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0,
};

static const int32_t hair_aniso_vpt_unival_max[] = {
    1, 1, 0, 0, 1, 0, 0, 3, 0, 2, 1, 0, 0, 0,
};

static const int32_t hair_aniso_fpt_unival_max[] = {
    1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1,
};

static const int32_t hair_npr1_vpt_unival_max[] = {
    1, 1, 0, 0, 1, 0, 0, 3, 0, 2, 1, 0, 0, 0,
};

static const int32_t hair_npr1_fpt_unival_max[] = {
    1, 0, 0, 0, 0, 1, 0, 3, 1, 0, 0, 0, 0, 0,
};

static const int32_t cloth_default_vpt_unival_max[] = {
    1, 1, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0,
};

static const int32_t cloth_default_fpt_unival_max[] = {
    1, 0, 2, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1,
};

static const int32_t cloth_aniso_vpt_unival_max[] = {
    1, 1, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0,
};

static const int32_t cloth_aniso_fpt_unival_max[] = {
    1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 3, 0, 0, 1, 1,
};

static const int32_t cloth_npr1_vpt_unival_max[] = {
    1, 1, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0,
};

static const int32_t cloth_npr1_fpt_unival_max[] = {
    1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0,
};

static const int32_t tights_vpt_unival_max[] = {
    1, 1, 0, 0, 0, 2, 0, 1, 0, 0, 0,
};

static const int32_t tights_fpt_unival_max[] = {
    1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1,
};

static const int32_t sky_default_vpt_unival_max[] = {
    0, 0, 0, 1, 1, 3,
};

static const int32_t sky_default_fpt_unival_max[] = {
    3, 3, 2, 0, 0, 3,
};

static const int32_t eye_ball_vpt_unival_max[] = {
    1, 0,
};

static const int32_t eye_ball_fpt_unival_max[] = {
    0, 1,
};

static const int32_t eye_lens_vpt_unival_max[] = {
    1, 0, 0,
};

static const int32_t eye_lens_fpt_unival_max[] = {
    0, 1, 1,
};

static const int32_t glass_eye_vpt_unival_max[] = {
    1, 1, 0, 2, 0, 1, 0, 0,
};

static const int32_t glass_eye_fpt_unival_max[] = {
    1, 1, 1, 0, 1, 0, 1, 1,
};

static const int32_t membrane_vpt_unival_max[] = {
    1, 1, 0,
};

static const int32_t membrane_fpt_unival_max[] = {
    0, 1, 3,
};

static const int32_t shadowmap_vpt_unival_max[] = {
    1, 2, 1, 0, 0,
};

static const int32_t shadowmap_fpt_unival_max[] = {
    0, 2, 1, 1, 5,
};

static const int32_t esm_vpt_unival_max[] = {
    1, 1, 2, 1,
};

static const int32_t esm_fpt_unival_max[] = {
    0, 0, 2, 1,
};

static const int32_t esm_gauss_vpt_unival_max[] = {
    0,
};

static const int32_t esm_gauss_fpt_unival_max[] = {
    1,
};

static const int32_t esm_filter_vpt_unival_max[] = {
    0,
};

static const int32_t esm_filter_fpt_unival_max[] = {
    1,
};

static const int32_t lit_proj_vpt_unival_max[] = {
    1, 1, 0, 1, 0, 0,
};

static const int32_t lit_proj_fpt_unival_max[] = {
    0, 0, 1, 1, 2, 1,
};

static const int32_t simple_vpt_unival_max[] = {
    1, 1, 0, 0, 1, 0,
};

static const int32_t simple_fpt_unival_max[] = {
    0, 0, 2, 0, 0, 3,
};

static const int32_t silhouette_vpt_unival_max[] = {
    1, 1, 1, 2, 1, 1, 0,
};

static const int32_t silhouette_fpt_unival_max[] = {
    0, 0, 1, 0, 0, 0, 1,
};

static const int32_t lambert_vpt_unival_max[] = {
    1, 1, 0, 1, 3, 1, 1, 0,
};

static const int32_t lambert_fpt_unival_max[] = {
    0, 0, 2, 1, 3, 0, 0, 1,
};

static const int32_t constant_vpt_unival_max[] = {
    1, 1, 0, 3, 1, 1,
};

static const int32_t constant_fpt_unival_max[] = {
    0, 0, 2, 3, 0, 0,
};

static const int32_t peel_vpt_unival_max[] = {
    0, 1,
};

static const int32_t peel_fpt_unival_max[] = {
    1, 0,
};

static const int32_t tone_map_vpt_unival_max[] = {
    0, 0, 0, 0, 1, 0, 0, 0,
};

static const int32_t tone_map_fpt_unival_max[] = {
    0, 2, 2, 1, 1, 1, 0, 0,
};

static const int32_t tone_map_npr1_vpt_unival_max[] = {
    0, 0, 0, 0, 1, 0, 0, 0,
};

static const int32_t tone_map_npr1_fpt_unival_max[] = {
    1, 0, 0, 1, 1, 0, 0, 1,
};

static const int32_t reduce_tex_vpt_unival_max[] = {
    8, 1,
};

static const int32_t reduce_tex_fpt_unival_max[] = {
    8, 1,
};

static const int32_t magnify_vpt_unival_max[] = {
    7,
};

static const int32_t magnify_fpt_unival_max[] = {
    7,
};

static const int32_t mlaa_vpt_unival_max[] = {
    0, 0, 2, 0,
};

static const int32_t mlaa_fpt_unival_max[] = {
    1, 1, 2, 2,
};

static const int32_t contour_vpt_unival_max[] = {
    0, 1,
};

static const int32_t contour_fpt_unival_max[] = {
    1, 1,
};

static const int32_t exposure_vpt_unival_max[] = {
    2,
};

static const int32_t exposure_fpt_unival_max[] = {
    2,
};

static const int32_t pp_gauss_vpt_unival_max[] = {
    1,
};

static const int32_t pp_gauss_fpt_unival_max[] = {
    1,
};

static const int32_t sun_vpt_unival_max[] = {
    -1,
};

static const int32_t sun_fpt_unival_max[] = {
    -1,
};

static const int32_t fade_vpt_unival_max[] = {
    5,
};

static const int32_t fade_fpt_unival_max[] = {
    5,
};

static const int32_t water01_vpt_unival_max[] = {
    1, 1, 0, 0, 0, 0, 0, 1,
};

static const int32_t water01_fpt_unival_max[] = {
    0, 0, 2, 1, 1, 1, 1, 1,
};

static const int32_t water02_vpt_unival_max[] = {
    -1,
};

static const int32_t water02_fpt_unival_max[] = {
    -1,
};

static const int32_t water_ring_vpt_unival_max[] = {
    1, 0,
};

static const int32_t water_ring_fpt_unival_max[] = {
    1, 1,
};

static const int32_t water_particle_vpt_unival_max[] = {
    -1,
};

static const int32_t water_particle_fpt_unival_max[] = {
    -1,
};

static const int32_t snow_particle_vpt_unival_max[] = {
    1,
};

static const int32_t snow_particle_fpt_unival_max[] = {
    1,
};

static const int32_t leaf_particle_vpt_unival_max[] = {
    -1,
};

static const int32_t leaf_particle_fpt_unival_max[] = {
    -1,
};

static const int32_t star_vpt_unival_max[] = {
    1,
};

static const int32_t star_fpt_unival_max[] = {
    1,
};

static const int32_t snow_ring_vpt_unival_max[] = {
    0, 0, 0, 1, 0,
};

static const int32_t snow_ring_fpt_unival_max[] = {
    2, 1, 1, 1, 1,
};

static const int32_t snow_footprint_vpt_unival_max[] = {
    -1,
};

static const int32_t snow_footprint_fpt_unival_max[] = {
    -1,
};

static const int32_t snow_tex_space_light_vpt_unival_max[] = {
    -1,
};

static const int32_t snow_tex_space_light_fpt_unival_max[] = {
    -1,
};

static const int32_t snow_calc_normal_vpt_unival_max[] = {
    -1,
};

static const int32_t snow_calc_normal_fpt_unival_max[] = {
    -1,
};

static const int32_t floor_vpt_unival_max[] = {
    0, 0, 1, 0, 3, 1, 1,
};

static const int32_t floor_fpt_unival_max[] = {
    2, 1, 1, 1, 3, 0, 0,
};

static const int32_t puddle_vpt_unival_max[] = {
    0, 1, 0, 3,
};

static const int32_t puddle_fpt_unival_max[] = {
    1, 1, 1, 3,
};

static const int32_t simple_reflect_vpt_unival_max[] = {
    1, 1, 2, 1, 1, 0, 0, 0, 0, 0,
};

static const int32_t simple_reflect_fpt_unival_max[] = {
    0, 0, 2, 0, 0, 1, 1, 2, 1, 1,
};

static const int32_t simple_refract_vpt_unival_max[] = {
    1, 0,
};

static const int32_t simple_refract_fpt_unival_max[] = {
    0, 1,
};

static const int32_t ripple_emit_vpt_unival_max[] = {
    0, 0,
};

static const int32_t ripple_emit_fpt_unival_max[] = {
    1, 1,
};

static const int32_t rain_vpt_unival_max[] = {
    -1,
};

static const int32_t rain_fpt_unival_max[] = {
    -1,
};

static const int32_t volume_light_vpt_unival_max[] = {
    -1,
};

static const int32_t volume_light_fpt_unival_max[] = {
    -1,
};

static const int32_t fence_alpha_vpt_unival_max[] = {
    1, 3,
};

static const int32_t fence_alpha_fpt_unival_max[] = {
    0, 3,
};

static const int32_t ripple_vpt_unival_max[] = {
    0,
};

static const int32_t ripple_fpt_unival_max[] = {
    1,
};

static const int32_t fog_ptcl_vpt_unival_max[] = {
    -1,
};

static const int32_t fog_ptcl_fpt_unival_max[] = {
    -1,
};

static const int32_t particle_vpt_unival_max[] = {
    -1,
};

static const int32_t particle_fpt_unival_max[] = {
    -1,
};

static const int32_t glitter_particle_vpt_unival_max[] = {
    2, 0, 3, 0,
};

static const int32_t glitter_particle_fpt_unival_max[] = {
    2, 3, 3, 3,
};

static const int32_t show_vector_vpt_unival_max[] = {
    1, 1, 3,
};

static const int32_t show_vector_fpt_unival_max[] = {
    0, 0, 0,
};

static const int32_t font_vpt_unival_max[] = {
    0,
};

static const int32_t font_fpt_unival_max[] = {
    1,
};

static const int32_t movie_vpt_unival_max[] = {
    0,
};

static const int32_t movie_fpt_unival_max[] = {
    1,
};

static const int32_t imgfilter_vpt_unival_max[] = {
    0,
};

static const int32_t imgfilter_fpt_unival_max[] = {
    5,
};

static const int32_t sprite_vpt_unival_max[] = {
    0, 0, 0,
};

static const int32_t sprite_fpt_unival_max[] = {
    3, 3, 2,
};

static const shader_sub_table BLINN_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_BLINN_VERT,
        .vp_unival_max = blinn_vert_vpt_unival_max,
        .fp_unival_max = blinn_vert_fpt_unival_max,
        .vp = "blinn_per_vert",
        .fp = "blinn_per_vert",
    },
    {
        .sub_index = SHADER_AFT_SUB_BLINN_FRAG,
        .vp_unival_max = blinn_frag_vpt_unival_max,
        .fp_unival_max = blinn_frag_fpt_unival_max,
        .vp = "blinn_per_frag",
        .fp = "blinn_per_frag",
    },
};

static const shader_sub_table ITEM_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_ITEM_BLINN,
        .vp_unival_max = item_blinn_vpt_unival_max,
        .fp_unival_max = item_blinn_fpt_unival_max,
        .vp = "item_blinn",
        .fp = "item_blinn",
    },
};

static const shader_sub_table STAGE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_STAGE_BLINN,
        .vp_unival_max = stage_blinn_vpt_unival_max,
        .fp_unival_max = stage_blinn_fpt_unival_max,
        .vp = "stage_blinn",
        .fp = "stage_blinn",
    },
};

static const shader_sub_table SKIN_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SKIN_DEFAULT,
        .vp_unival_max = skin_default_vpt_unival_max,
        .fp_unival_max = skin_default_fpt_unival_max,
        .vp = "skin_default",
        .fp = "skin_default",
    },
};

static const shader_sub_table SSS_SKIN_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SSS_SKIN,
        .vp_unival_max = sss_skin_vpt_unival_max,
        .fp_unival_max = sss_skin_fpt_unival_max,
        .vp = "sss_skin",
        .fp = "sss_skin",
    },
};

static const shader_sub_table SSS_FILT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SSS_FILTER,
        .vp_unival_max = sss_filter_vpt_unival_max,
        .fp_unival_max = sss_filter_fpt_unival_max,
        .vp = "sss_filter",
        .fp = "sss_filter",
    },
};

static const shader_sub_table HAIR_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_HAIR_DEFAULT,
        .vp_unival_max = hair_default_vpt_unival_max,
        .fp_unival_max = hair_default_fpt_unival_max,
        .vp = "hair_default",
        .fp = "hair_default",
    },
    {
        .sub_index = SHADER_AFT_SUB_HAIR_ANISO,
        .vp_unival_max = hair_aniso_vpt_unival_max,
        .fp_unival_max = hair_aniso_fpt_unival_max,
        .vp = "hair_default",
        .fp = "hair_aniso",
    },
    {
        .sub_index = SHADER_AFT_SUB_HAIR_NPR1,
        .vp_unival_max = hair_npr1_vpt_unival_max,
        .fp_unival_max = hair_npr1_fpt_unival_max,
        .vp = "hair_default",
        .fp = "hair_npr1",
    },
};

static const shader_sub_table CLOTH_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_CLOTH_DEFAULT,
        .vp_unival_max = cloth_default_vpt_unival_max,
        .fp_unival_max = cloth_default_fpt_unival_max,
        .vp = "cloth_default",
        .fp = "cloth_default",
    },
    {
        .sub_index = SHADER_AFT_SUB_CLOTH_ANISO,
        .vp_unival_max = cloth_aniso_vpt_unival_max,
        .fp_unival_max = cloth_aniso_fpt_unival_max,
        .vp = "cloth_default",
        .fp = "cloth_aniso",
    },
    {
        .sub_index = SHADER_AFT_SUB_CLOTH_NPR1,
        .vp_unival_max = cloth_npr1_vpt_unival_max,
        .fp_unival_max = cloth_npr1_fpt_unival_max,
        .vp = "cloth_default",
        .fp = "cloth_npr1",
    },
};

static const shader_sub_table TIGHTS_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_TIGHTS,
        .vp_unival_max = tights_vpt_unival_max,
        .fp_unival_max = tights_fpt_unival_max,
        .vp = "tights",
        .fp = "tights",
    },
};

static const shader_sub_table SKY_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SKY_DEFAULT,
        .vp_unival_max = sky_default_vpt_unival_max,
        .fp_unival_max = sky_default_fpt_unival_max,
        .vp = "sky_default",
        .fp = "sky_default",
    },
};

static const shader_sub_table EYEBALL_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_EYE_BALL,
        .vp_unival_max = eye_ball_vpt_unival_max,
        .fp_unival_max = eye_ball_fpt_unival_max,
        .vp = "eye_ball",
        .fp = "eye_ball",
    },
};

static const shader_sub_table EYELENS_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_EYE_LENS,
        .vp_unival_max = eye_lens_vpt_unival_max,
        .fp_unival_max = eye_lens_fpt_unival_max,
        .vp = "eye_lens",
        .fp = "eye_lens",
    },
};

static const shader_sub_table GLASEYE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_GLASS_EYE,
        .vp_unival_max = glass_eye_vpt_unival_max,
        .fp_unival_max = glass_eye_fpt_unival_max,
        .vp = "glass_eye",
        .fp = "glass_eye",
    },
};

static const shader_sub_table MEMBRAN_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_MEMBRANE,
        .vp_unival_max = membrane_vpt_unival_max,
        .fp_unival_max = membrane_fpt_unival_max,
        .vp = "membrane",
        .fp = "membrane",
    },
};

static const shader_sub_table SHDMAP_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SHADOWMAP,
        .vp_unival_max = shadowmap_vpt_unival_max,
        .fp_unival_max = shadowmap_fpt_unival_max,
        .vp = "shadowmap",
        .fp = "shadowmap",
    },
};

static const shader_sub_table ESM_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_ESM,
        .vp_unival_max = esm_vpt_unival_max,
        .fp_unival_max = esm_fpt_unival_max,
        .vp = "esm",
        .fp = "esm",
    },
};

static const shader_sub_table ESMGAUSS_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_ESM_GAUSS,
        .vp_unival_max = esm_gauss_vpt_unival_max,
        .fp_unival_max = esm_gauss_fpt_unival_max,
        .vp = "esm_gauss",
        .fp = "esm_gauss",
    },
};

static const shader_sub_table ESMFILT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_ESM_FILTER,
        .vp_unival_max = esm_filter_vpt_unival_max,
        .fp_unival_max = esm_filter_fpt_unival_max,
        .vp = "esm_filter",
        .fp = "esm_filter",
    },
};

static const shader_sub_table LITPROJ_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_LIT_PROJ,
        .vp_unival_max = lit_proj_vpt_unival_max,
        .fp_unival_max = lit_proj_fpt_unival_max,
        .vp = "lit_proj",
        .fp = "lit_proj",
    },
};

static const shader_sub_table SIMPLE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SIMPLE,
        .vp_unival_max = simple_vpt_unival_max,
        .fp_unival_max = simple_fpt_unival_max,
        .vp = "simple",
        .fp = "simple",
    },
};

static const shader_sub_table SIL_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SILHOUETTE,
        .vp_unival_max = silhouette_vpt_unival_max,
        .fp_unival_max = silhouette_fpt_unival_max,
        .vp = "silhouette",
        .fp = "silhouette",
    },
};

static const shader_sub_table LAMBERT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_LAMBERT,
        .vp_unival_max = lambert_vpt_unival_max,
        .fp_unival_max = lambert_fpt_unival_max,
        .vp = "lambert",
        .fp = "lambert",
    },
};

static const shader_sub_table CONSTANT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_CONSTANT,
        .vp_unival_max = constant_vpt_unival_max,
        .fp_unival_max = constant_fpt_unival_max,
        .vp = "constant",
        .fp = "constant",
    },
};

static const shader_sub_table PEEL_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_PEEL,
        .vp_unival_max = peel_vpt_unival_max,
        .fp_unival_max = peel_fpt_unival_max,
        .vp = "depth_peel",
        .fp = "depth_peel",
    },
};

static const shader_sub_table TONEMAP_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_TONE_MAP,
        .vp_unival_max = tone_map_vpt_unival_max,
        .fp_unival_max = tone_map_fpt_unival_max,
        .vp = "tone_map",
        .fp = "tone_map",
    },
    {
        .sub_index = SHADER_AFT_SUB_TONE_MAP_NPR1,
        .vp_unival_max = tone_map_npr1_vpt_unival_max,
        .fp_unival_max = tone_map_npr1_fpt_unival_max,
        .vp = "tone_map",
        .fp = "tone_map_npr1",
    },
};

static const shader_sub_table REDUCE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_REDUCE_TEX,
        .vp_unival_max = reduce_tex_vpt_unival_max,
        .fp_unival_max = reduce_tex_fpt_unival_max,
        .vp = "reduce_tex",
        .fp = "reduce_tex",
    },
};

static const shader_sub_table MAGNIFY_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_MAGNIFY,
        .vp_unival_max = magnify_vpt_unival_max,
        .fp_unival_max = magnify_fpt_unival_max,
        .vp = "magnify",
        .fp = "magnify",
    },
};

static const shader_sub_table MLAA_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_MLAA,
        .vp_unival_max = mlaa_vpt_unival_max,
        .fp_unival_max = mlaa_fpt_unival_max,
        .vp = "mlaa",
        .fp = "mlaa",
    },
};

static const shader_sub_table CONTOUR_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_CONTOUR,
        .vp_unival_max = contour_vpt_unival_max,
        .fp_unival_max = contour_fpt_unival_max,
        .vp = "contour",
        .fp = "contour",
    },
};

static const shader_sub_table EXPOSURE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_EXPOSURE,
        .vp_unival_max = exposure_vpt_unival_max,
        .fp_unival_max = exposure_fpt_unival_max,
        .vp = "exposure",
        .fp = "exposure",
    },
};

static const shader_sub_table GAUSS_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_PP_GAUSS,
        .vp_unival_max = pp_gauss_vpt_unival_max,
        .fp_unival_max = pp_gauss_fpt_unival_max,
        .vp = "pp_gauss",
        .fp = "pp_gauss",
    },
};

static const shader_sub_table SUN_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SUN,
        .vp_unival_max = sun_vpt_unival_max,
        .fp_unival_max = sun_fpt_unival_max,
        .vp = "sun",
        .fp = "sun",
    },
};

static const shader_sub_table FADE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_FADE,
        .vp_unival_max = fade_vpt_unival_max,
        .fp_unival_max = fade_fpt_unival_max,
        .vp = "fade",
        .fp = "fade",
    },
};

static const shader_sub_table WATER01_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_WATER01,
        .vp_unival_max = water01_vpt_unival_max,
        .fp_unival_max = water01_fpt_unival_max,
        .vp = "water01",
        .fp = "water01",
    },
};

static const shader_sub_table WATER02_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_WATER02,
        .vp_unival_max = water02_vpt_unival_max,
        .fp_unival_max = water02_fpt_unival_max,
        .vp = "water02",
        .fp = "water02",
    },
};

static const shader_sub_table WATRING_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_WATER_RING,
        .vp_unival_max = water_ring_vpt_unival_max,
        .fp_unival_max = water_ring_fpt_unival_max,
        .vp = "water_ring",
        .fp = "water_ring",
    },
};

static const shader_sub_table W_PTCL_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_WATER_PARTICLE,
        .vp_unival_max = water_particle_vpt_unival_max,
        .fp_unival_max = water_particle_fpt_unival_max,
        .vp = "water_particle",
        .fp = "water_particle",
    },
};

static const shader_sub_table SNOW_PT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SNOW_PARTICLE,
        .vp_unival_max = snow_particle_vpt_unival_max,
        .fp_unival_max = snow_particle_fpt_unival_max,
        .vp = "snow_particle",
        .fp = "snow_particle",
    },
};

static const shader_sub_table LEAF_PT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_LEAF_PARTICLE,
        .vp_unival_max = leaf_particle_vpt_unival_max,
        .fp_unival_max = leaf_particle_fpt_unival_max,
        .vp = "leaf_particle",
        .fp = "leaf_particle",
    },
};

static const shader_sub_table STAR_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_STAR,
        .vp_unival_max = star_vpt_unival_max,
        .fp_unival_max = star_fpt_unival_max,
        .vp = "star",
        .fp = "star",
    },
};

static const shader_sub_table SNORING_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SNOW_RING,
        .vp_unival_max = snow_ring_vpt_unival_max,
        .fp_unival_max = snow_ring_fpt_unival_max,
        .vp = "snow_ring",
        .fp = "snow_ring",
    },
};

static const shader_sub_table SN_FOOT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SNOW_FOOTPRINT,
        .vp_unival_max = snow_footprint_vpt_unival_max,
        .fp_unival_max = snow_footprint_fpt_unival_max,
        .vp = "snow_footprint",
        .fp = "snow_footprint",
    },
};

static const shader_sub_table SN_TSL_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SNOW_TEX_SPACE_LIGHT,
        .vp_unival_max = snow_tex_space_light_vpt_unival_max,
        .fp_unival_max = snow_tex_space_light_fpt_unival_max,
        .vp = "snow_tex_space_light",
        .fp = "snow_tex_space_light",
    },
};

static const shader_sub_table SN_NRM_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SNOW_CALC_NORMAL,
        .vp_unival_max = snow_calc_normal_vpt_unival_max,
        .fp_unival_max = snow_calc_normal_fpt_unival_max,
        .vp = "snow_calc_normal",
        .fp = "snow_calc_normal",
    },
};

static const shader_sub_table FLOOR_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_FLOOR,
        .vp_unival_max = floor_vpt_unival_max,
        .fp_unival_max = floor_fpt_unival_max,
        .vp = "floor",
        .fp = "floor",
    },
};

static const shader_sub_table PUDDLE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_PUDDLE,
        .vp_unival_max = puddle_vpt_unival_max,
        .fp_unival_max = puddle_fpt_unival_max,
        .vp = "puddle",
        .fp = "puddle",
    },
};

static const shader_sub_table S_REFL_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SIMPLE_REFLECT,
        .vp_unival_max = simple_reflect_vpt_unival_max,
        .fp_unival_max = simple_reflect_fpt_unival_max,
        .vp = "simple_reflect",
        .fp = "simple_reflect",
    },
};

static const shader_sub_table S_REFR_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SIMPLE_REFRACT,
        .vp_unival_max = simple_refract_vpt_unival_max,
        .fp_unival_max = simple_refract_fpt_unival_max,
        .vp = "simple_refract",
        .fp = "simple_refract",
    },
};

static const shader_sub_table RIPEMIT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_RIPPLE_EMIT,
        .vp_unival_max = ripple_emit_vpt_unival_max,
        .fp_unival_max = ripple_emit_fpt_unival_max,
        .vp = "ripple_emit",
        .fp = "ripple_emit",
    },
};

static const shader_sub_table RAIN_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_RAIN,
        .vp_unival_max = rain_vpt_unival_max,
        .fp_unival_max = rain_fpt_unival_max,
        .vp = "rain",
        .fp = "rain",
    },
};

static const shader_sub_table VOLLIT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_VOLUME_LIGHT,
        .vp_unival_max = volume_light_vpt_unival_max,
        .fp_unival_max = volume_light_fpt_unival_max,
        .vp = "volume_light",
        .fp = "volume_light",
    },
};

static const shader_sub_table FENCE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_FENCE_ALPHA,
        .vp_unival_max = fence_alpha_vpt_unival_max,
        .fp_unival_max = fence_alpha_fpt_unival_max,
        .vp = "fence_alpha",
        .fp = "fence_alpha",
    },
};

static const shader_sub_table RIPPLE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_RIPPLE,
        .vp_unival_max = ripple_vpt_unival_max,
        .fp_unival_max = ripple_fpt_unival_max,
        .vp = "ripple",
        .fp = "ripple",
    },
};

static const shader_sub_table FOGPTCL_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_FOG_PTCL,
        .vp_unival_max = fog_ptcl_vpt_unival_max,
        .fp_unival_max = fog_ptcl_fpt_unival_max,
        .vp = "fog_ptcl",
        .fp = "fog_ptcl",
    },
};

static const shader_sub_table PARTICL_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_PARTICLE,
        .vp_unival_max = particle_vpt_unival_max,
        .fp_unival_max = particle_fpt_unival_max,
        .vp = "particle",
        .fp = "particle",
    },
};

static const shader_sub_table GLITTER_PT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_GLITTER_PARTICLE,
        .vp_unival_max = glitter_particle_vpt_unival_max,
        .fp_unival_max = glitter_particle_fpt_unival_max,
        .vp = "glitter_particle",
        .fp = "glitter_particle",
    },
};

static const shader_sub_table SHOWVEC_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SHOW_VECTOR,
        .vp_unival_max = show_vector_vpt_unival_max,
        .fp_unival_max = show_vector_fpt_unival_max,
        .vp = "show_vector",
        .fp = "show_vector",
    },
};

static const shader_sub_table FONT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_FONT,
        .vp_unival_max = font_vpt_unival_max,
        .fp_unival_max = font_fpt_unival_max,
        .vp = "font",
        .fp = "font",
    },
};

static const shader_sub_table MOVIE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_MOVIE,
        .vp_unival_max = movie_vpt_unival_max,
        .fp_unival_max = movie_fpt_unival_max,
        .vp = "movie",
        .fp = "movie",
    },
};

static const shader_sub_table IMGFILT_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_IMGFILTER,
        .vp_unival_max = imgfilter_vpt_unival_max,
        .fp_unival_max = imgfilter_fpt_unival_max,
        .vp = "imgfilter",
        .fp = "imgfilter",
    },
};

static const shader_sub_table SPRITE_table[] = {
    {
        .sub_index = SHADER_AFT_SUB_SPRITE,
        .vp_unival_max = sprite_vpt_unival_max,
        .fp_unival_max = sprite_fpt_unival_max,
        .vp = "sprite",
        .fp = "sprite",
    },
};

static const uniform_name BLINN_uniform[] = {
    U16,
    U_BONE_MAT,
    U_TEXTURE_COUNT,
    U_LIGHT_0,
    U_NORMAL,
    U_SPECULAR,
    U_ENV_MAP,
    U_FOG_HEIGHT,
    U_MORPH,
    U_MORPH_COLOR,
    U_LIGHT_1,
};

static const uniform_name ITEM_uniform[] = {
    U16,
    U_BONE_MAT,
    U_MORPH,
    U_SPECULAR_IBL,
    U_SPECULAR,
    U_NORMAL,
    U_TEXTURE_COUNT,
    U_ENV_MAP,
    U_SELF_SHADOW,
    U_FOG,
    U_ALPHA_TEST,
    U_SHADOW,
    U_CHARA_COLOR,
    U_TONE_CURVE,
};

static const uniform_name STAGE_uniform[] = {
    U16,
    U_BONE_MAT,
    U_MORPH,
    U_MORPH_COLOR,
    U_TEXTURE_COUNT,
    U_ENV_MAP,
    U_LIGHT_0,
    U_NORMAL,
    U_FOG_HEIGHT,
    U_LIGHT_1,
    U12,
};

static const uniform_name SKIN_uniform[] = {
    U16,
    U_BONE_MAT,
    U_NORMAL,
    U_SELF_SHADOW,
    U_FOG,
    U_ALPHA_TEST,
    U_SHADOW,
    U_NPR,
    U_CHARA_COLOR,
    U_TONE_CURVE,
};

static const uniform_name SSS_SKIN_uniform[] = {
    U16,
    U_BONE_MAT,
    U37,
    U_ALPHA_TEST,
    U_SELF_SHADOW,
    U_SHADOW,
    U_NPR,
    U26,
};

static const uniform_name SSS_FILT_uniform[] = {
    U16,
    U_SSS_FILTER,
    U_NPR,
};

static const uniform_name HAIR_uniform[] = {
    U16,
    U_BONE_MAT,
    U_SPECULAR_IBL,
    U_SPECULAR,
    U_NORMAL,
    U_TRANSPARENCY,
    U_TRANSLUCENCY,
    U_ANISO,
    U_SELF_SHADOW,
    U_FOG,
    U_SHADOW,
    U_NPR,
    U_CHARA_COLOR,
    U_TONE_CURVE,
};

static const uniform_name CLOTH_uniform[] = {
    U16,
    U_BONE_MAT,
    U_SPECULAR_IBL,
    U_SPECULAR,
    U_NORMAL,
    U_TRANSPARENCY,
    U_ENV_MAP,
    U_SELF_SHADOW,
    U_FOG,
    U_ALPHA_TEST,
    U_ANISO,
    U_SHADOW,
    U_NPR,
    U_CHARA_COLOR,
    U_TONE_CURVE,
};

static const uniform_name TIGHTS_uniform[] = {
    U16,
    U_BONE_MAT,
    U_SPECULAR,
    U_ENV_MAP,
    U_SELF_SHADOW,
    U_FOG,
    U_ALPHA_TEST,
    U_SHADOW,
    U_NPR,
    U_CHARA_COLOR,
    U_TONE_CURVE,
};

static const uniform_name SKY_uniform[] = {
    U_TEX_0_TYPE,
    U_TEX_1_TYPE,
    U_TEXTURE_COUNT,
    U_MORPH,
    U_MORPH_COLOR,
    U_FOG_HEIGHT,
};

static const uniform_name EYEBALL_uniform[] = {
    U_BONE_MAT,
    U_ENV_MAP,
};

static const uniform_name EYELENS_uniform[] = {
    U_BONE_MAT,
    U_SPECULAR,
    U_ENV_MAP,
};

static const uniform_name GLASEYE_uniform[] = {
    U16,
    U_BONE_MAT,
    U_SELF_SHADOW,
    U_FOG,
    U18,
    U_SHADOW,
    U_CHARA_COLOR,
    U_TONE_CURVE,
};

static const uniform_name MEMBRAN_uniform[] = {
    U_BONE_MAT,
    U_NORMAL,
    U_MEMBRANE,
};

static const uniform_name SHDMAP_uniform[] = {
    U_BONE_MAT,
    U_TEXTURE_COUNT,
    U_TRANSPARENCY,
    U_LIGHT_1,
    U2D,
};

static const uniform_name ESM_uniform[] = {
    U16,
    U_BONE_MAT,
    U_TEXTURE_COUNT,
    U_TRANSPARENCY,
};

static const uniform_name ESMGAUSS_uniform[] = {
    U_LIGHT_PROJ,
};

static const uniform_name ESMFILT_uniform[] = {
    U_ESM_FILTER,
};

static const uniform_name LITPROJ_uniform[] = {
    U16,
    U_BONE_MAT,
    U_SPECULAR,
    U_NORMAL,
    U_TEXTURE_COUNT,
    U_TRANSPARENCY,
};

static const uniform_name SIMPLE_uniform[] = {
    U16,
    U_BONE_MAT,
    U_TEXTURE_COUNT,
    U_NORMAL,
    U_INSTANCE,
    U2E,
};

static const uniform_name SIL_uniform[] = {
    U16,
    U_BONE_MAT,
    U_TRANSPARENCY,
    U_TEXTURE_COUNT,
    U_MORPH,
    U_MORPH_COLOR,
    U0A,
};

static const uniform_name LAMBERT_uniform[] = {
    U16,
    U_BONE_MAT,
    U_TEXTURE_COUNT,
    U_LIGHT_0,
    U_FOG_HEIGHT,
    U_MORPH,
    U_MORPH_COLOR,
    U_LIGHT_1,
};

static const uniform_name CONSTANT_uniform[] = {
    U16,
    U_BONE_MAT,
    U_TEXTURE_COUNT,
    U_FOG_HEIGHT,
    U_MORPH,
    U_MORPH_COLOR,
};

static const uniform_name PEEL_uniform[] = {
    U_DEPTH_PEEL,
    U_BONE_MAT,
};

static const uniform_name TONEMAP_uniform[] = {
    U16,
    U_TONE_MAP,
    U_FLARE,
    U_SCENE_FADE,
    U_AET_BACK,
    U_LIGHT_PROJ,
    U_NPR,
    U25,
};

static const uniform_name REDUCE_uniform[] = {
    U2A,
    U01,
};

static const uniform_name MAGNIFY_uniform[] = {
    U_MAGNIFY,
};

static const uniform_name MLAA_uniform[] = {
    U16,
    U01,
    U_MLAA,
    U20,
};

static const uniform_name CONTOUR_uniform[] = {
    U16,
    U24,
};

static const uniform_name EXPOSURE_uniform[] = {
    U_EXPOSURE,
};

static const uniform_name GAUSS_uniform[] = {
    U_GAUSS,
};

static const uniform_name SUN_uniform[] = {
    U_INVALID,
};

static const uniform_name FADE_uniform[] = {
    U_FADE,
};

static const uniform_name WATER01_uniform[] = {
    U_MORPH,
    U_MORPH_COLOR,
    U_TEXTURE_COUNT,
    U_NORMAL,
    U_WATER_REFLECT,
    U_ENV_MAP,
    U_SPECULAR,
    U_FOG_HEIGHT,
};

static const uniform_name WATER02_uniform[] = {
    U_INVALID,
};

static const uniform_name WATRING_uniform[] = {
    U_LIGHT_0,
    U_LIGHT_1,
};

static const uniform_name W_PTCL_uniform[] = {
    U_INVALID,
};

static const uniform_name SNOW_PT_uniform[] = {
    U_SNOW_PARTICLE,
};

static const uniform_name LEAF_PT_uniform[] = {
    U_INVALID,
};

static const uniform_name STAR_uniform[] = {
    U_STAR,
};

static const uniform_name SNORING_uniform[] = {
    U_TEXTURE_COUNT,
    U_NORMAL,
    U_SPECULAR,
    U_LIGHT_0,
    U_LIGHT_1,
};

static const uniform_name SN_FOOT_uniform[] = {
    U_INVALID,
};

static const uniform_name SN_TSL_uniform[] = {
    U_INVALID,
};

static const uniform_name SN_NRM_uniform[] = {
    U_INVALID,
};

static const uniform_name FLOOR_uniform[] = {
    U_TEXTURE_COUNT,
    U_NORMAL,
    U_LIGHT_0,
    U_LIGHT_1,
    U_FOG_HEIGHT,
    U_MORPH,
    U_MORPH_COLOR,
};

static const uniform_name PUDDLE_uniform[] = {
    U_NORMAL,
    U_LIGHT_0,
    U_LIGHT_1,
    U_FOG_HEIGHT,
};

static const uniform_name S_REFL_uniform[] = {
    U16,
    U_BONE_MAT,
    U_REFLECT,
    U_MORPH,
    U_MORPH_COLOR,
    U_ALPHA_TEST,
    U_CLIP_PLANE,
    U_TEXTURE_COUNT,
    U_TONE_CURVE,
    U_CHARA_COLOR,
};

static const uniform_name S_REFR_uniform[] = {
    U_BONE_MAT,
    U_TRANSPARENCY,
};

static const uniform_name RIPEMIT_uniform[] = {
    U_RIPPLE,
    U_RIPPLE_EMIT,
};

static const uniform_name RAIN_uniform[] = {
    U_INVALID,
};

static const uniform_name VOLLIT_uniform[] = {
    U_INVALID,
};

static const uniform_name FENCE_uniform[] = {
    U_INSTANCE,
    U_FOG_HEIGHT,
};

static const uniform_name RIPPLE_uniform[] = {
    U_RIPPLE,
};

static const uniform_name FOGPTCL_uniform[] = {
    U_INVALID,
};

static const uniform_name PARTICL_uniform[] = {
    U_INVALID,
};

static const uniform_name GLITTER_PT_uniform[] = {
    U_TEXTURE_COUNT,
    U_TEXTURE_BLEND,
    U_FOG_HEIGHT,
    U_ALPHA_BLEND,
};

static const uniform_name SHOWVEC_uniform[] = {
    U16,
    U_BONE_MAT,
    U_SHOW_VECTOR,
};

static const uniform_name FONT_uniform[] = {
    U16,
};

static const uniform_name MOVIE_uniform[] = {
    U_MOVIE,
};

static const uniform_name IMGFILT_uniform[] = {
    U_IMAGE_FILTER,
};

static const uniform_name SPRITE_uniform[] = {
    U_TEX_0_TYPE,
    U_TEX_1_TYPE,
    U_SPRITE_BLEND,
};

static const bool BLINN_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool ITEM_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool STAGE_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool SKIN_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool SSS_SKIN_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool SSS_FILT_permut[] = {
    false,
    false,
    false,
};

static const bool HAIR_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool CLOTH_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool TIGHTS_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool SKY_permut[] = {
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool EYEBALL_permut[] = {
    false,
    false,
};

static const bool EYELENS_permut[] = {
    false,
    false,
    false,
};

static const bool GLASEYE_permut[] = {
    true,
    true,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool MEMBRAN_permut[] = {
    false,
    false,
    false,
};

static const bool SHDMAP_permut[] = {
    false,
    false,
    false,
    false,
    true,
};

static const bool ESM_permut[] = {
    true,
    false,
    false,
    false,
};

static const bool ESMGAUSS_permut[] = {
    false,
};

static const bool ESMFILT_permut[] = {
    false,
};

static const bool LITPROJ_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
};

static const bool SIMPLE_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
};

static const bool SIL_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool LAMBERT_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool CONSTANT_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
};

static const bool PEEL_permut[] = {
    false,
    false,
};

static const bool TONEMAP_permut[] = {
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool REDUCE_permut[] = {
    false,
    false,
};

static const bool MAGNIFY_permut[] = {
    false,
};

static const bool MLAA_permut[] = {
    false,
    false,
    false,
    true,
};

static const bool CONTOUR_permut[] = {
    false,
    false,
};

static const bool EXPOSURE_permut[] = {
    false,
};

static const bool GAUSS_permut[] = {
    false,
};

static const bool SUN_permut[] = {
    false,
};

static const bool FADE_permut[] = {
    false,
};

static const bool WATER01_permut[] = {
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool WATER02_permut[] = {
    false,
};

static const bool WATRING_permut[] = {
    false,
    false,
};

static const bool W_PTCL_permut[] = {
    false,
};

static const bool SNOW_PT_permut[] = {
    false,
};

static const bool LEAF_PT_permut[] = {
    false,
};

static const bool STAR_permut[] = {
    false,
};

static const bool SNORING_permut[] = {
    false,
    false,
    false,
    false,
    false,
};

static const bool SN_FOOT_permut[] = {
    false,
};

static const bool SN_TSL_permut[] = {
    false,
};

static const bool SN_NRM_permut[] = {
    false,
};

static const bool FLOOR_permut[] = {
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool PUDDLE_permut[] = {
    false,
    false,
    false,
    false,
};

static const bool S_REFL_permut[] = {
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const bool S_REFR_permut[] = {
    false,
    false,
};

static const bool RIPEMIT_permut[] = {
    false,
    false,
};

static const bool RAIN_permut[] = {
    false,
};

static const bool VOLLIT_permut[] = {
    false,
};

static const bool FENCE_permut[] = {
    false,
    false,
};

static const bool RIPPLE_permut[] = {
    false,
};

static const bool FOGPTCL_permut[] = {
    false,
};

static const bool PARTICL_permut[] = {
    false,
};

static const bool GLITTER_PT_permut[] = {
    false,
    false,
    false,
    false,
};

static const bool SHOWVEC_permut[] = {
    true,
    false,
    false,
};

static const bool FONT_permut[] = {
    false,
};

static const bool MOVIE_permut[] = {
    true,
};

static const bool IMGFILT_permut[] = {
    false,
};

static const bool SPRITE_permut[] = {
    false,
    false,
    false,
};

static const shader_table SHADER_FFP_shader = {
    .name = "SHADER_FFP",
    .num_sub = 0,
    .sub = 0,
    .num_uniform = 0,
    .use_uniform = 0,
};

#define shader_table_struct(n) \
static const shader_table n##_shader = { \
    .name = #n, \
    .index = SHADER_AFT_##n, \
    .num_sub = sizeof(n##_table) / sizeof(shader_sub_table), \
    .sub = n##_table, \
    .num_uniform = sizeof(n##_uniform) / sizeof(uniform_name), \
    .use_uniform = n##_uniform, \
    .use_permut = n##_permut, \
}; \

shader_table_struct(BLINN)
shader_table_struct(ITEM)
shader_table_struct(STAGE)
shader_table_struct(SKIN)
shader_table_struct(SSS_SKIN)
shader_table_struct(SSS_FILT)
shader_table_struct(HAIR)
shader_table_struct(CLOTH)
shader_table_struct(TIGHTS)
shader_table_struct(SKY)
shader_table_struct(EYEBALL)
shader_table_struct(EYELENS)
shader_table_struct(GLASEYE)
shader_table_struct(MEMBRAN)
shader_table_struct(SHDMAP)
shader_table_struct(ESM)
shader_table_struct(ESMGAUSS)
shader_table_struct(ESMFILT)
shader_table_struct(LITPROJ)
shader_table_struct(SIMPLE)
shader_table_struct(SIL)
shader_table_struct(LAMBERT)
shader_table_struct(CONSTANT)
shader_table_struct(PEEL)
shader_table_struct(TONEMAP)
shader_table_struct(REDUCE)
shader_table_struct(MAGNIFY)
shader_table_struct(MLAA)
shader_table_struct(CONTOUR)
shader_table_struct(EXPOSURE)
shader_table_struct(GAUSS)
shader_table_struct(SUN)
shader_table_struct(FADE)
shader_table_struct(WATER01)
shader_table_struct(WATER02)
shader_table_struct(WATRING)
shader_table_struct(W_PTCL)
shader_table_struct(SNOW_PT)
shader_table_struct(LEAF_PT)
shader_table_struct(STAR)
shader_table_struct(SNORING)
shader_table_struct(SN_FOOT)
shader_table_struct(SN_TSL)
shader_table_struct(SN_NRM)
shader_table_struct(FLOOR)
shader_table_struct(PUDDLE)
shader_table_struct(S_REFL)
shader_table_struct(S_REFR)
shader_table_struct(RIPEMIT)
shader_table_struct(RAIN)
shader_table_struct(VOLLIT)
shader_table_struct(FENCE)
shader_table_struct(RIPPLE)
shader_table_struct(FOGPTCL)
shader_table_struct(PARTICL)
shader_table_struct(GLITTER_PT)
shader_table_struct(SHOWVEC)
shader_table_struct(FONT)
shader_table_struct(MOVIE)
shader_table_struct(IMGFILT)
shader_table_struct(SPRITE)
#undef shader_table_struct

static const shader_table* shader_aft_table[] = {
    [SHADER_AFT_FFP       ] = &SHADER_FFP_shader,
    [SHADER_AFT_BLINN     ] = &BLINN_shader,
    [SHADER_AFT_ITEM      ] = &ITEM_shader,
    [SHADER_AFT_STAGE     ] = &STAGE_shader,
    [SHADER_AFT_SKIN      ] = &SKIN_shader,
    [SHADER_AFT_SSS_SKIN  ] = &SSS_SKIN_shader,
    [SHADER_AFT_SSS_FILT  ] = &SSS_FILT_shader,
    [SHADER_AFT_HAIR      ] = &HAIR_shader,
    [SHADER_AFT_CLOTH     ] = &CLOTH_shader,
    [SHADER_AFT_TIGHTS    ] = &TIGHTS_shader,
    [SHADER_AFT_SKY       ] = &SKY_shader,
    [SHADER_AFT_EYEBALL   ] = &EYEBALL_shader,
    [SHADER_AFT_EYELENS   ] = &EYELENS_shader,
    [SHADER_AFT_GLASEYE   ] = &GLASEYE_shader,
    [SHADER_AFT_MEMBRAN   ] = &MEMBRAN_shader,
    [SHADER_AFT_SHDMAP    ] = &SHDMAP_shader,
    [SHADER_AFT_ESM       ] = &ESM_shader,
    [SHADER_AFT_ESMGAUSS  ] = &ESMGAUSS_shader,
    [SHADER_AFT_ESMFILT   ] = &ESMFILT_shader,
    [SHADER_AFT_LITPROJ   ] = &LITPROJ_shader,
    [SHADER_AFT_SIMPLE    ] = &SIMPLE_shader,
    [SHADER_AFT_SIL       ] = &SIL_shader,
    [SHADER_AFT_LAMBERT   ] = &LAMBERT_shader,
    [SHADER_AFT_CONSTANT  ] = &CONSTANT_shader,
    [SHADER_AFT_PEEL      ] = &PEEL_shader,
    [SHADER_AFT_TONEMAP   ] = &TONEMAP_shader,
    [SHADER_AFT_REDUCE    ] = &REDUCE_shader,
    [SHADER_AFT_MAGNIFY   ] = &MAGNIFY_shader,
    [SHADER_AFT_MLAA      ] = &MLAA_shader,
    [SHADER_AFT_CONTOUR   ] = &CONTOUR_shader,
    [SHADER_AFT_EXPOSURE  ] = &EXPOSURE_shader,
    [SHADER_AFT_GAUSS     ] = &GAUSS_shader,
    [SHADER_AFT_SUN       ] = &SUN_shader,
    [SHADER_AFT_FADE      ] = &FADE_shader,
    [SHADER_AFT_WATER01   ] = &WATER01_shader,
    [SHADER_AFT_WATER02   ] = &WATER02_shader,
    [SHADER_AFT_WATRING   ] = &WATRING_shader,
    [SHADER_AFT_W_PTCL    ] = &W_PTCL_shader,
    [SHADER_AFT_SNOW_PT   ] = &SNOW_PT_shader,
    [SHADER_AFT_LEAF_PT   ] = &LEAF_PT_shader,
    [SHADER_AFT_STAR      ] = &STAR_shader,
    [SHADER_AFT_SNORING   ] = &SNORING_shader,
    [SHADER_AFT_SN_FOOT   ] = &SN_FOOT_shader,
    [SHADER_AFT_SN_TSL    ] = &SN_TSL_shader,
    [SHADER_AFT_SN_NRM    ] = &SN_NRM_shader,
    [SHADER_AFT_FLOOR     ] = &FLOOR_shader,
    [SHADER_AFT_PUDDLE    ] = &PUDDLE_shader,
    [SHADER_AFT_S_REFL    ] = &S_REFL_shader,
    [SHADER_AFT_S_REFR    ] = &S_REFR_shader,
    [SHADER_AFT_RIPEMIT   ] = &RIPEMIT_shader,
    [SHADER_AFT_RAIN      ] = &RAIN_shader,
    [SHADER_AFT_VOLLIT    ] = &VOLLIT_shader,
    [SHADER_AFT_FENCE     ] = &FENCE_shader,
    [SHADER_AFT_RIPPLE    ] = &RIPPLE_shader,
    [SHADER_AFT_FOGPTCL   ] = &FOGPTCL_shader,
    [SHADER_AFT_PARTICL   ] = &PARTICL_shader,
    [SHADER_AFT_GLITTER_PT] = &GLITTER_PT_shader,
    [SHADER_AFT_SHOWVEC   ] = &SHOWVEC_shader,
    [SHADER_AFT_FONT      ] = &FONT_shader,
    [SHADER_AFT_MOVIE     ] = &MOVIE_shader,
    [SHADER_AFT_IMGFILT   ] = &IMGFILT_shader,
    [SHADER_AFT_SPRITE    ] = &SPRITE_shader,
};

static const size_t shader_aft_table_size =
    sizeof(shader_aft_table) / sizeof(shader_table*);

static void shader_bind_blinn(shader_set_data* set, shader* shad);
static void shader_bind_cloth(shader_set_data* set, shader* shad);
static void shader_bind_hair(shader_set_data* set, shader* shad);
static void shader_bind_membrane(shader_set_data* set, shader* shad);
static void shader_bind_eye_ball(shader_set_data* set, shader* shader);
static void shader_bind_tone_map(shader_set_data* set, shader* shader);

static const shader_bind_func shader_aft_bind_func_table[] = {
    {
        .index = SHADER_AFT_BLINN,
        .bind_func = shader_bind_blinn,
    },
    {
        .index = SHADER_AFT_CLOTH,
        .bind_func = shader_bind_cloth,
    },
    {
        .index = SHADER_AFT_HAIR,
        .bind_func = shader_bind_hair,
    },
    {
        .index = SHADER_AFT_MEMBRAN,
        .bind_func = shader_bind_membrane,
    },
    {
        .index = SHADER_AFT_EYEBALL,
        .bind_func = shader_bind_eye_ball,
    },
    {
        .index = SHADER_AFT_TONEMAP,
        .bind_func = shader_bind_tone_map,
    },
};

static const size_t shader_aft_bind_func_table_size =
    sizeof(shader_aft_bind_func_table) / sizeof(shader_bind_func);

void shader_aft_load(shader_set_data* set, farc* f, bool ignore_cache) {
    shader_load(set, f, false, false, "aft", shader_aft_table, shader_aft_table_size,
        shader_aft_bind_func_table, shader_aft_bind_func_table_size);
}

static void shader_bind_blinn(shader_set_data* set, shader* shad) {
    shader_bind(shad, uniform_value[U_NORMAL]
        ? SHADER_AFT_SUB_BLINN_FRAG : SHADER_AFT_SUB_BLINN_VERT);
}

static void shader_bind_cloth(shader_set_data* set, shader* shad) {
    shader_bind(shad, uniform_value[U_NPR] ? SHADER_AFT_SUB_CLOTH_NPR1
        : (uniform_value[U_ANISO] ? SHADER_AFT_SUB_CLOTH_ANISO : SHADER_AFT_SUB_CLOTH_DEFAULT));
}

static void shader_bind_hair(shader_set_data* set, shader* shad) {
    shader_bind(shad, uniform_value[U_NPR] ? SHADER_AFT_SUB_HAIR_NPR1
        : (uniform_value[U_ANISO] ? SHADER_AFT_SUB_HAIR_ANISO : SHADER_AFT_SUB_HAIR_DEFAULT));
}

static void shader_bind_membrane(shader_set_data* set, shader* shad) {
    uniform_value[U_MEMBRANE] = 3;
    if (shader_bind(shad, SHADER_AFT_SUB_MEMBRANE) < 0)
        return;

    /*uint32_t(* sub_140192E00)() = (void*)0x0000000140192E00;
    uint32_t v1 = sub_140192E00();
    mat4 mat = mat4_identity;
    mat4_rotate_x_mult(&mat, (float_t)((v1 & 0x1FF) * (M_PI / 256.0)), &mat);
    mat4_rotate_z_mult(&mat, (float_t)((v1 % 0x168) * (M_PI / 180.0)), &mat);*/

    //vec4 vec = (vec4){ 1.0f, 0.0f, 0.0f, 0.0f };
    //mat4_mult_vec(&mat, &vec, &vec);

    //glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 10, vec.x, vec.y, vec.z, 0.0);
}

static void shader_bind_eye_ball(shader_set_data* set, shader* shader) {
    uniform_value[U18] = 0;
    if (shader_bind(&set->shaders[SHADER_AFT_GLASEYE], SHADER_AFT_SUB_GLASS_EYE) >= 0) {
        /*float_t glass_eye = (float*)0x0000000140CA2D70;
        void (* glass_eye_calc)(float* glass_eye) = (void*)0x00000001405E53D0;
        void (* glass_eye_set)(float* glass_eye) = (void*)0x00000001405E4750;
        glass_eye_calc(glass_eye);
        glass_eye_set(glass_eye);*/
    }
}

static void shader_bind_tone_map(shader_set_data* set, shader* shader) {
    shader_bind(shader, uniform_value[U_NPR] == 1
        ? SHADER_AFT_SUB_TONE_MAP_NPR1 : SHADER_AFT_SUB_TONE_MAP);
}
