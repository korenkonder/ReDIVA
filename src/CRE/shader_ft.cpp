/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader_ft.hpp"
#include "../KKdLib/str_utils.hpp"
#include "gl_state.hpp"
#include "random.hpp"
#include "render_context.hpp"

enum shader_ft_sub_enum {
    SHADER_FT_SUB_SHADER_FFP = 0,
    SHADER_FT_SUB_BLINN_VERT,
    SHADER_FT_SUB_BLINN_FRAG,
    SHADER_FT_SUB_ITEM_BLINN,
    SHADER_FT_SUB_STAGE_BLINN,
    SHADER_FT_SUB_SKIN_DEFAULT,
    SHADER_FT_SUB_SSS_SKIN,
    SHADER_FT_SUB_SSS_FILTER_MIN,
    SHADER_FT_SUB_SSS_FILTER_MIN_NPR,
    SHADER_FT_SUB_SSS_FILTER_GAUSS_2D,
    SHADER_FT_SUB_HAIR_DEFAULT,
    SHADER_FT_SUB_HAIR_NPR1,
    SHADER_FT_SUB_CLOTH_DEFAULT,
    SHADER_FT_SUB_CLOTH_ANISO,
    SHADER_FT_SUB_CLOTH_NPR1,
    SHADER_FT_SUB_TIGHTS,
    SHADER_FT_SUB_SKY_DEFAULT,
    SHADER_FT_SUB_GLASS_EYE,
    SHADER_FT_SUB_ESM_GAUSS,
    SHADER_FT_SUB_ESM_FILTER_MIN,
    SHADER_FT_SUB_ESM_FILTER_EROSION,
    SHADER_FT_SUB_LIT_PROJ,
    SHADER_FT_SUB_SIMPLE,
    SHADER_FT_SUB_SILHOUETTE,
    SHADER_FT_SUB_LAMBERT,
    SHADER_FT_SUB_CONSTANT,
    SHADER_FT_SUB_TONEMAP,
    SHADER_FT_SUB_TONEMAP_NPR1,
    SHADER_FT_SUB_GHOST,
    SHADER_FT_SUB_REDUCE_TEX_REDUCE_2,
    SHADER_FT_SUB_REDUCE_TEX_REDUCE_2_ALPHAMASK,
    SHADER_FT_SUB_REDUCE_TEX_REDUCE_4,
    SHADER_FT_SUB_REDUCE_TEX_REDUCE_4_EXTRACT,
    SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_2,
    SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_BLUR,
    SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_4,
    SHADER_FT_SUB_MAGNIFY_CONE,
    SHADER_FT_SUB_MAGNIFY_CONE2,
    SHADER_FT_SUB_MAGNIFY_DIFF,
    SHADER_FT_SUB_MAGNIFY_DIFF2,
    SHADER_FT_SUB_MAGNIFY_LINEAR,
    SHADER_FT_SUB_MLAA_EDGE,
    SHADER_FT_SUB_MLAA_AREA,
    SHADER_FT_SUB_MLAA_BLEND,
    SHADER_FT_SUB_CONTOUR,
    SHADER_FT_SUB_CONTOUR_NPR,
    SHADER_FT_SUB_EXPOSURE_MINIFY,
    SHADER_FT_SUB_EXPOSURE_MEASURE,
    SHADER_FT_SUB_EXPOSURE_AVERAGE,
    SHADER_FT_SUB_PP_GAUSS_USUAL,
    SHADER_FT_SUB_PP_GAUSS_CONE,
    SHADER_FT_SUB_SUN,
    SHADER_FT_SUB_SUN_NO_TEXTURED,
    SHADER_FT_SUB_WATER01,
    SHADER_FT_SUB_WATER_PARTICLE,
    SHADER_FT_SUB_SNOW_PARTICLE,
    SHADER_FT_SUB_SNOW_PARTICLE_CPU,
    SHADER_FT_SUB_LEAF_PARTICLE,
    SHADER_FT_SUB_STAR,
    SHADER_FT_SUB_STAR_MILKY_WAY,
    SHADER_FT_SUB_FLOOR,
    SHADER_FT_SUB_PUDDLE,
    SHADER_FT_SUB_SIMPLE_REFLECT,
    SHADER_FT_SUB_SIMPLE_REFRACT,
    SHADER_FT_SUB_RIPPLE_EMIT,
    SHADER_FT_SUB_RAIN,
    SHADER_FT_SUB_RIPPLE,
    SHADER_FT_SUB_FOG_PTCL,
    SHADER_FT_SUB_PARTICLE,
    SHADER_FT_SUB_GLITTER_PARTICLE,
    SHADER_FT_SUB_FONT,
    SHADER_FT_SUB_BOX4,
    SHADER_FT_SUB_BOX8,
    SHADER_FT_SUB_COPY,
    SHADER_FT_SUB_SPRITE,
    SHADER_FT_SUB_DOF_RENDER_TILE,
    SHADER_FT_SUB_DOF_GATHER_TILE,
    SHADER_FT_SUB_DOF_DOWNSAMPLE,
    SHADER_FT_SUB_DOF_MAIN_FILTER,
    SHADER_FT_SUB_DOF_UPSAMPLE,
    SHADER_FT_SUB_TRANSPARENCY,
    SHADER_FT_SUB_SHADER_END,
};

static const int32_t blinn_vert_vpt_unival_max[] = {
    1, 1, 0,
};

static const int32_t blinn_vert_fpt_unival_max[] = {
    0, 0, 1,
};

static const int32_t blinn_frag_vpt_unival_max[] = {
    1, 1, 0,
};

static const int32_t blinn_frag_fpt_unival_max[] = {
    0, 0, 1,
};

static const int32_t item_blinn_vpt_unival_max[] = {
    1, 1, 0,
};

static const int32_t item_blinn_fpt_unival_max[] = {
    0, 0, 1,
};

static const int32_t stage_blinn_vpt_unival_max[] = {
    1, 1, 0,
};

static const int32_t stage_blinn_fpt_unival_max[] = {
    0, 0, 1,
};

static const int32_t skin_default_vpt_unival_max[] = {
    1, 0,
};

static const int32_t skin_default_fpt_unival_max[] = {
    0, 1,
};

static const int32_t sss_skin_vpt_unival_max[] = {
    1, 0,
};

static const int32_t sss_skin_fpt_unival_max[] = {
    0, 1,
};

static const int32_t sss_filter_min_vpt_unival_max[] = {
    -1,
};

static const int32_t sss_filter_min_fpt_unival_max[] = {
    -1,
};

static const int32_t sss_filter_min_npr_fpt_unival_max[] = {
    -1,
};

static const int32_t sss_filter_gauss_2d_vpt_unival_max[] = {
   -1,
};

static const int32_t sss_filter_gauss_2d_fpt_unival_max[] = {
   -1,
};

static const int32_t hair_default_vpt_unival_max[] = {
    1, 0,
};

static const int32_t hair_default_fpt_unival_max[] = {
    0, 1,
};

static const int32_t hair_npr1_fpt_unival_max[] = {
    0, 1,
};

static const int32_t cloth_default_vpt_unival_max[] = {
    1, 0,
};

static const int32_t cloth_default_fpt_unival_max[] = {
    0, 1,
};

static const int32_t cloth_aniso_fpt_unival_max[] = {
    0, 1,
};

static const int32_t cloth_npr1_fpt_unival_max[] = {
    0, 1,
};

static const int32_t tights_vpt_unival_max[] = {
    1, 0,
};

static const int32_t tights_fpt_unival_max[] = {
    0, 1,
};

static const int32_t sky_default_vpt_unival_max[] = {
    1, 0,
};

static const int32_t sky_default_fpt_unival_max[] = {
    0, 1,
};

static const int32_t glass_eye_vpt_unival_max[] = {
    1,
};

static const int32_t glass_eye_fpt_unival_max[] = {
    0,
};

static const int32_t esm_gauss_vpt_unival_max[] = {
    -1,
};

static const int32_t esm_gauss_fpt_unival_max[] = {
    -1,
};

static const int32_t esm_filter_min_vpt_unival_max[] = {
    -1,
};

static const int32_t esm_filter_min_fpt_unival_max[] = {
    -1,
};

static const int32_t esm_filter_erosion_vpt_unival_max[] = {
    -1,
};

static const int32_t esm_filter_erosion_fpt_unival_max[] = {
    -1,
};

static const int32_t lit_proj_vpt_unival_max[] = {
    1, 0,
};

static const int32_t lit_proj_fpt_unival_max[] = {
    0, 1,
};

static const int32_t simple_vpt_unival_max[] = {
    1,
};

static const int32_t simple_fpt_unival_max[] = {
    0,
};

static const int32_t silhouette_vpt_unival_max[] = {
    1, 1,
};

static const int32_t silhouette_fpt_unival_max[] = {
    0, 0,
};

static const int32_t lambert_vpt_unival_max[] = {
    1, 1, 0,
};

static const int32_t lambert_fpt_unival_max[] = {
    0, 0, 1,
};

static const int32_t constant_vpt_unival_max[] = {
    1, 1, 0,
};

static const int32_t constant_fpt_unival_max[] = {
    0, 0, 1,
};

static const int32_t tone_map_vpt_unival_max[] = {
    0, 1,
};

static const int32_t tone_map_fpt_unival_max[] = {
    2, 1,
};

static const int32_t tone_map_npr1_vpt_unival_max[] = {
    0, 1,
};

static const int32_t tone_map_npr1_fpt_unival_max[] = {
    0, 1,
};

static const int32_t reduce_tex_reduce_2_vpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_2_fpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_2_alphamask_vpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_2_alphamask_fpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_4_vpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_4_fpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_4_extract_vpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_4_extract_fpt_unival_max[] = {
    -1,
};

static const int32_t ghost_vpt_unival_max[] = {
    -1,
};

static const int32_t ghost_fpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_composite_2_vpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_composite_2_fpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_composite_blur_vpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_composite_blur_fpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_composite_4_vpt_unival_max[] = {
    -1,
};

static const int32_t reduce_tex_reduce_composite_4_fpt_unival_max[] = {
    -1,
};

static const int32_t magnify_linear_vpt_unival_max[] = {
    -1,
};

static const int32_t magnify_linear_fpt_unival_max[] = {
    -1,
};

static const int32_t magnify_diff_vpt_unival_max[] = {
    -1,
};

static const int32_t magnify_diff_fpt_unival_max[] = {
    -1,
};

static const int32_t magnify_diff2_vpt_unival_max[] = {
    -1,
};

static const int32_t magnify_diff2_fpt_unival_max[] = {
    -1,
};

static const int32_t magnify_cone_vpt_unival_max[] = {
    -1,
};

static const int32_t magnify_cone_fpt_unival_max[] = {
    -1,
};

static const int32_t magnify_cone2_vpt_unival_max[] = {
    -1,
};

static const int32_t magnify_cone2_fpt_unival_max[] = {
    -1,
};

static const int32_t mlaa_edge_vpt_unival_max[] = {
    0, 0,
};

static const int32_t mlaa_edge_fpt_unival_max[] = {
    1, 0,
};

static const int32_t mlaa_area_vpt_unival_max[] = {
    0, 0,
};

static const int32_t mlaa_area_fpt_unival_max[] = {
    0, 2,
};

static const int32_t mlaa_blend_vpt_unival_max[] = {
    0, 0,
};

static const int32_t mlaa_blend_fpt_unival_max[] = {
    1, 0,
};

static const int32_t contour_vpt_unival_max[] = {
    -1,
};

static const int32_t contour_fpt_unival_max[] = {
    -1,
};

static const int32_t contour_npr_vpt_unival_max[] = {
    -1,
};

static const int32_t contour_npr_fpt_unival_max[] = {
    -1,
};

static const int32_t exposure_minify_vpt_unival_max[] = {
    -1,
};

static const int32_t exposure_minify_fpt_unival_max[] = {
    -1,
};

static const int32_t exposure_measure_vpt_unival_max[] = {
    -1,
};

static const int32_t exposure_measure_fpt_unival_max[] = {
    -1,
};

static const int32_t exposure_average_vpt_unival_max[] = {
    -1,
};

static const int32_t exposure_average_fpt_unival_max[] = {
    -1,
};

static const int32_t pp_gauss_usual_vpt_unival_max[] = {
    -1,
};

static const int32_t pp_gauss_usual_fpt_unival_max[] = {
    -1,
};

static const int32_t pp_gauss_cone_vpt_unival_max[] = {
    -1,
};

static const int32_t pp_gauss_cone_fpt_unival_max[] = {
    -1,
};

static const int32_t sun_vpt_unival_max[] = {
    -1,
};

static const int32_t sun_fpt_unival_max[] = {
    -1,
};

static const int32_t sun_no_textured_vpt_unival_max[] = {
    -1,
};

static const int32_t sun_no_textured_fpt_unival_max[] = {
    -1,
};

static const int32_t water01_vpt_unival_max[] = {
    1,
};

static const int32_t water01_fpt_unival_max[] = {
    0,
};

static const int32_t water_particle_vpt_unival_max[] = {
    -1,
};

static const int32_t water_particle_fpt_unival_max[] = {
    -1,
};

static const int32_t snow_particle_vpt_unival_max[] = {
    -1,
};

static const int32_t snow_particle_fpt_unival_max[] = {
    -1,
};

static const int32_t snow_particle_cpu_vpt_unival_max[] = {
    -1,
};

static const int32_t snow_particle_cpu_fpt_unival_max[] = {
    -1,
};

static const int32_t leaf_particle_vpt_unival_max[] = {
    -1,
};

static const int32_t leaf_particle_fpt_unival_max[] = {
    -1,
};

static const int32_t star_vpt_unival_max[] = {
    -1,
};

static const int32_t star_fpt_unival_max[] = {
    -1,
};

static const int32_t star_milky_way_vpt_unival_max[] = {
    -1,
};

static const int32_t star_milky_way_fpt_unival_max[] = {
    -1,
};

static const int32_t floor_vpt_unival_max[] = {
    0, 0,
};

static const int32_t floor_fpt_unival_max[] = {
    1, 1,
};

static const int32_t puddle_vpt_unival_max[] = {
    -1,
};

static const int32_t puddle_fpt_unival_max[] = {
    -1,
};

static const int32_t simple_reflect_vpt_unival_max[] = {
    1, 1, 0,
};

static const int32_t simple_reflect_fpt_unival_max[] = {
    0, 0, 1,
};

static const int32_t simple_refract_vpt_unival_max[] = {
    1,
};

static const int32_t simple_refract_fpt_unival_max[] = {
    0,
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
    -1,
};

static const int32_t glitter_particle_fpt_unival_max[] = {
    -1,
};

static const int32_t font_vpt_unival_max[] = {
    -1,
};

static const int32_t font_fpt_unival_max[] = {
    -1,
};

static const int32_t box4_vpt_unival_max[] = {
    -1,
};

static const int32_t box4_fpt_unival_max[] = {
    -1,
};

static const int32_t box8_vpt_unival_max[] = {
    -1,
};

static const int32_t box8_fpt_unival_max[] = {
    -1,
};

static const int32_t copy_vpt_unival_max[] = {
    -1,
};

static const int32_t copy_fpt_unival_max[] = {
    -1,
};

static const int32_t sprite_vpt_unival_max[] = {
    0, 0, 0,
};

static const int32_t sprite_fpt_unival_max[] = {
    3, 3, 2,
};

static const int32_t dof_common_vpt_unival_max[] = {
    0,
};

static const int32_t dof_render_tile_fpt_unival_max[] = {
    1,
};

static const int32_t dof_gather_tile_fpt_unival_max[] = {
    0,
};

static const int32_t dof_downsample_fpt_unival_max[] = {
    1,
};

static const int32_t dof_main_filter_fpt_unival_max[] = {
    1,
};

static const int32_t dof_upsample_fpt_unival_max[] = {
    1,
};

static const int32_t transparency_vpt_unival_max[] = {
    -1,
};

static const int32_t transparency_fpt_unival_max[] = {
    -1,
};

static const shader_sub_table BLINN_table[] = {
    {
        SHADER_FT_SUB_BLINN_VERT,
        blinn_vert_vpt_unival_max,
        blinn_vert_fpt_unival_max,
        "blinn_per_vert",
        "blinn_per_vert",
    },
    {
        SHADER_FT_SUB_BLINN_FRAG,
        blinn_frag_vpt_unival_max,
        blinn_frag_fpt_unival_max,
        "blinn_per_frag",
        "blinn_per_frag",
    },
};

static const shader_sub_table ITEM_table[] = {
    {
        SHADER_FT_SUB_ITEM_BLINN,
        item_blinn_vpt_unival_max,
        item_blinn_fpt_unival_max,
        "item_blinn",
        "item_blinn",
    },
};

static const shader_sub_table STAGE_table[] = {
    {
        SHADER_FT_SUB_STAGE_BLINN,
        stage_blinn_vpt_unival_max,
        stage_blinn_fpt_unival_max,
        "stage_blinn",
        "stage_blinn",
    },
};

static const shader_sub_table SKIN_table[] = {
    {
        SHADER_FT_SUB_SKIN_DEFAULT,
        skin_default_vpt_unival_max,
        skin_default_fpt_unival_max,
        "skin_default",
        "skin_default",
    },
};

static const shader_sub_table SSS_SKIN_table[] = {
    {
        SHADER_FT_SUB_SSS_SKIN,
        sss_skin_vpt_unival_max,
        sss_skin_fpt_unival_max,
        "sss_skin",
        "sss_skin",
    },
};

static const shader_sub_table SSS_FILT_table[] = {
    {
        SHADER_FT_SUB_SSS_FILTER_MIN,
        sss_filter_min_vpt_unival_max,
        sss_filter_min_fpt_unival_max,
        "sss_filter_min",
        "sss_filter_min",
    },
    {
        SHADER_FT_SUB_SSS_FILTER_MIN_NPR,
        sss_filter_min_vpt_unival_max,
        sss_filter_min_npr_fpt_unival_max,
        "sss_filter_min",
        "sss_filter_min_npr",
    },
    {
        SHADER_FT_SUB_SSS_FILTER_GAUSS_2D,
        sss_filter_gauss_2d_vpt_unival_max,
        sss_filter_gauss_2d_fpt_unival_max,
        "sss_filter_gauss_2d",
        "sss_filter_gauss_2d",
    },
};

static const shader_sub_table HAIR_table[] = {
    {
        SHADER_FT_SUB_HAIR_DEFAULT,
        hair_default_vpt_unival_max,
        hair_default_fpt_unival_max,
        "hair_default",
        "hair_default",
    },
    {
        SHADER_FT_SUB_HAIR_NPR1,
        hair_default_vpt_unival_max,
        hair_npr1_fpt_unival_max,
        "hair_default",
        "hair_npr1",
    },
};

static const shader_sub_table CLOTH_table[] = {
    {
        SHADER_FT_SUB_CLOTH_DEFAULT,
        cloth_default_vpt_unival_max,
        cloth_default_fpt_unival_max,
        "cloth_default",
        "cloth_default",
    },
    {
        SHADER_FT_SUB_CLOTH_ANISO,
        cloth_default_vpt_unival_max,
        cloth_aniso_fpt_unival_max,
        "cloth_default",
        "cloth_aniso",
    },
    {
        SHADER_FT_SUB_CLOTH_NPR1,
        cloth_default_vpt_unival_max,
        cloth_npr1_fpt_unival_max,
        "cloth_default",
        "cloth_npr1",
    },
};

static const shader_sub_table TIGHTS_table[] = {
    {
        SHADER_FT_SUB_TIGHTS,
        tights_vpt_unival_max,
        tights_fpt_unival_max,
        "tights",
        "tights",
    },
};

static const shader_sub_table SKY_table[] = {
    {
        SHADER_FT_SUB_SKY_DEFAULT,
        sky_default_vpt_unival_max,
        sky_default_fpt_unival_max,
        "sky_default",
        "sky_default",
    },
};

static const shader_sub_table GLASEYE_table[] = {
    {
        SHADER_FT_SUB_GLASS_EYE,
        glass_eye_vpt_unival_max,
        glass_eye_fpt_unival_max,
        "glass_eye",
        "glass_eye",
    },
};

static const shader_sub_table ESMGAUSS_table[] = {
    {
        SHADER_FT_SUB_ESM_GAUSS,
        esm_gauss_vpt_unival_max,
        esm_gauss_fpt_unival_max,
        "esm_gauss",
        "esm_gauss",
    },
};

static const shader_sub_table ESMFILT_table[] = {
    {
        SHADER_FT_SUB_ESM_FILTER_MIN,
        esm_filter_min_vpt_unival_max,
        esm_filter_min_fpt_unival_max,
        "esm_filter_min",
        "esm_filter_min",
    },
    {
        SHADER_FT_SUB_ESM_FILTER_EROSION,
        esm_filter_erosion_vpt_unival_max,
        esm_filter_erosion_fpt_unival_max,
        "esm_filter_erosion",
        "esm_filter_erosion",
    },
};

static const shader_sub_table LITPROJ_table[] = {
    {
        SHADER_FT_SUB_LIT_PROJ,
        lit_proj_vpt_unival_max,
        lit_proj_fpt_unival_max,
        "lit_proj",
        "lit_proj",
    },
};

static const shader_sub_table SIMPLE_table[] = {
    {
        SHADER_FT_SUB_SIMPLE,
        simple_vpt_unival_max,
        simple_fpt_unival_max,
        "simple",
        "simple",
    },
};

static const shader_sub_table SIL_table[] = {
    {
        SHADER_FT_SUB_SILHOUETTE,
        silhouette_vpt_unival_max,
        silhouette_fpt_unival_max,
        "silhouette",
        "silhouette",
    },
};

static const shader_sub_table LAMBERT_table[] = {
    {
        SHADER_FT_SUB_LAMBERT,
        lambert_vpt_unival_max,
        lambert_fpt_unival_max,
        "lambert",
        "lambert",
    },
};

static const shader_sub_table CONSTANT_table[] = {
    {
        SHADER_FT_SUB_CONSTANT,
        constant_vpt_unival_max,
        constant_fpt_unival_max,
        "constant",
        "constant",
    },
};

static const shader_sub_table TONEMAP_table[] = {
    {
        SHADER_FT_SUB_TONEMAP,
        tone_map_vpt_unival_max,
        tone_map_fpt_unival_max,
        "tone_map",
        "tone_map",
    },
    {
        SHADER_FT_SUB_TONEMAP_NPR1,
        tone_map_npr1_vpt_unival_max,
        tone_map_npr1_fpt_unival_max,
        "tone_map_npr1",
        "tone_map_npr1",
    },
};

static const shader_sub_table REDUCE_table[] = {
    {
        SHADER_FT_SUB_REDUCE_TEX_REDUCE_2,
        reduce_tex_reduce_2_vpt_unival_max,
        reduce_tex_reduce_2_fpt_unival_max,
        "reduce_tex_reduce_2",
        "reduce_tex_reduce_2",
    },
    {
        SHADER_FT_SUB_REDUCE_TEX_REDUCE_2_ALPHAMASK,
        reduce_tex_reduce_2_alphamask_vpt_unival_max,
        reduce_tex_reduce_2_alphamask_fpt_unival_max,
        "reduce_tex_reduce_2_alphamask",
        "reduce_tex_reduce_2_alphamask",
    },
    {
        SHADER_FT_SUB_REDUCE_TEX_REDUCE_4,
        reduce_tex_reduce_4_vpt_unival_max,
        reduce_tex_reduce_4_fpt_unival_max,
        "reduce_tex_reduce_4",
        "reduce_tex_reduce_4",
    },
    {
        SHADER_FT_SUB_REDUCE_TEX_REDUCE_4_EXTRACT,
        reduce_tex_reduce_4_extract_vpt_unival_max,
        reduce_tex_reduce_4_extract_fpt_unival_max,
        "reduce_tex_reduce_4_extract",
        "reduce_tex_reduce_4_extract",
    },
    {
        SHADER_FT_SUB_GHOST,
        ghost_vpt_unival_max,
        ghost_fpt_unival_max,
        "ghost",
        "ghost",
    },
    {
        SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_2,
        reduce_tex_reduce_composite_2_vpt_unival_max,
        reduce_tex_reduce_composite_2_fpt_unival_max,
        "reduce_tex_reduce_composite_2",
        "reduce_tex_reduce_composite_2",
    },
    {
        SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_BLUR,
        reduce_tex_reduce_composite_blur_vpt_unival_max,
        reduce_tex_reduce_composite_blur_fpt_unival_max,
        "reduce_tex_reduce_composite_blur",
        "reduce_tex_reduce_composite_blur",
    },
    {
        SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_4,
        reduce_tex_reduce_composite_4_vpt_unival_max,
        reduce_tex_reduce_composite_4_fpt_unival_max,
        "reduce_tex_reduce_composite_4",
        "reduce_tex_reduce_composite_4",
    },
};

static const shader_sub_table MAGNIFY_table[] = {
    {
        SHADER_FT_SUB_MAGNIFY_LINEAR,
        magnify_linear_vpt_unival_max,
        magnify_linear_fpt_unival_max,
        "magnify_linear",
        "magnify_linear",
    },
    {
        SHADER_FT_SUB_MAGNIFY_DIFF,
        magnify_diff_vpt_unival_max,
        magnify_diff_fpt_unival_max,
        "magnify_diff",
        "magnify_diff",
    },
    {
        SHADER_FT_SUB_MAGNIFY_DIFF2,
        magnify_diff2_vpt_unival_max,
        magnify_diff2_fpt_unival_max,
        "magnify_diff2",
        "magnify_diff2",
    },
    {
        SHADER_FT_SUB_MAGNIFY_CONE,
        magnify_cone_vpt_unival_max,
        magnify_cone_fpt_unival_max,
        "magnify_cone",
        "magnify_cone",
    },
    {
        SHADER_FT_SUB_MAGNIFY_CONE2,
        magnify_cone2_vpt_unival_max,
        magnify_cone2_fpt_unival_max,
        "magnify_cone2",
        "magnify_cone2",
    },
};

static const shader_sub_table MLAA_table[] = {
    {
        SHADER_FT_SUB_MLAA_EDGE,
        mlaa_edge_vpt_unival_max,
        mlaa_edge_fpt_unival_max,
        "mlaa_edge",
        "mlaa_edge",
    },
    {
        SHADER_FT_SUB_MLAA_AREA,
        mlaa_area_vpt_unival_max,
        mlaa_area_fpt_unival_max,
        "mlaa_area",
        "mlaa_area",
    },
    {
        SHADER_FT_SUB_MLAA_BLEND,
        mlaa_blend_vpt_unival_max,
        mlaa_blend_fpt_unival_max,
        "mlaa_blend",
        "mlaa_blend",
    },
};

static const shader_sub_table CONTOUR_table[] = {
    {
        SHADER_FT_SUB_CONTOUR,
        contour_vpt_unival_max,
        contour_fpt_unival_max,
        "contour",
        "contour",
    },
};

static const shader_sub_table CONTOUR_NPR_table[] = {
    {
        SHADER_FT_SUB_CONTOUR_NPR,
        contour_npr_vpt_unival_max,
        contour_npr_fpt_unival_max,
        "contour_npr",
        "contour_npr",
    },
};

static const shader_sub_table EXPOSURE_table[] = {
    {
        SHADER_FT_SUB_EXPOSURE_MINIFY,
        exposure_minify_vpt_unival_max,
        exposure_minify_fpt_unival_max,
        "exposure_minify",
        "exposure_minify",
    },
    {
        SHADER_FT_SUB_EXPOSURE_MEASURE,
        exposure_measure_vpt_unival_max,
        exposure_measure_fpt_unival_max,
        "exposure_measure",
        "exposure_measure",
    },
    {
        SHADER_FT_SUB_EXPOSURE_AVERAGE,
        exposure_average_vpt_unival_max,
        exposure_average_fpt_unival_max,
        "exposure_average",
        "exposure_average",
    },
};

static const shader_sub_table GAUSS_table[] = {
    {
        SHADER_FT_SUB_PP_GAUSS_USUAL,
        pp_gauss_usual_vpt_unival_max,
        pp_gauss_usual_fpt_unival_max,
        "pp_gauss_usual",
        "pp_gauss_usual",
    },
    {
        SHADER_FT_SUB_PP_GAUSS_CONE,
        pp_gauss_cone_vpt_unival_max,
        pp_gauss_cone_fpt_unival_max,
        "pp_gauss_cone",
        "pp_gauss_cone",
    },
};

static const shader_sub_table SUN_table[] = {
    {
        SHADER_FT_SUB_SUN,
        sun_vpt_unival_max,
        sun_fpt_unival_max,
        "sun",
        "sun",
    },
};

static const shader_sub_table SUN_NO_TEXTURED_table[] = {
    {
        SHADER_FT_SUB_SUN_NO_TEXTURED,
        sun_no_textured_vpt_unival_max,
        sun_no_textured_fpt_unival_max,
        "sun_no_textured",
        "sun_no_textured",
    },
};

static const shader_sub_table WATER01_table[] = {
    {
        SHADER_FT_SUB_WATER01,
        water01_vpt_unival_max,
        water01_fpt_unival_max,
        "water01",
        "water01",
    },
};

static const shader_sub_table W_PTCL_table[] = {
    {
        SHADER_FT_SUB_WATER_PARTICLE,
        water_particle_vpt_unival_max,
        water_particle_fpt_unival_max,
        "water_particle",
        "water_particle",
    },
};

static const shader_sub_table SNOW_PT_table[] = {
    {
        SHADER_FT_SUB_SNOW_PARTICLE,
        snow_particle_vpt_unival_max,
        snow_particle_fpt_unival_max,
        "snow_particle",
        "snow_particle",
    },
    {
        SHADER_FT_SUB_SNOW_PARTICLE_CPU,
        snow_particle_cpu_vpt_unival_max,
        snow_particle_cpu_fpt_unival_max,
        "snow_particle_cpu",
        "snow_particle_cpu",
    },
};

static const shader_sub_table LEAF_PT_table[] = {
    {
        SHADER_FT_SUB_LEAF_PARTICLE,
        leaf_particle_vpt_unival_max,
        leaf_particle_fpt_unival_max,
        "leaf_particle",
        "leaf_particle",
    },
};

static const shader_sub_table STAR_table[] = {
    {
        SHADER_FT_SUB_STAR,
        star_vpt_unival_max,
        star_fpt_unival_max,
        "star",
        "star",
    },
    {
        SHADER_FT_SUB_STAR_MILKY_WAY,
        star_milky_way_vpt_unival_max,
        star_milky_way_fpt_unival_max,
        "star_milky_way",
        "star_milky_way",
    },
};

static const shader_sub_table FLOOR_table[] = {
    {
        SHADER_FT_SUB_FLOOR,
        floor_vpt_unival_max,
        floor_fpt_unival_max,
        "floor",
        "floor",
    },
};

static const shader_sub_table PUDDLE_table[] = {
    {
        SHADER_FT_SUB_PUDDLE,
        puddle_vpt_unival_max,
        puddle_fpt_unival_max,
        "puddle",
        "puddle",
    },
};

static const shader_sub_table S_REFL_table[] = {
    {
        SHADER_FT_SUB_SIMPLE_REFLECT,
        simple_reflect_vpt_unival_max,
        simple_reflect_fpt_unival_max,
        "simple_reflect",
        "simple_reflect",
    },
};

static const shader_sub_table S_REFR_table[] = {
    {
        SHADER_FT_SUB_SIMPLE_REFRACT,
        simple_refract_vpt_unival_max,
        simple_refract_fpt_unival_max,
        "simple_refract",
        "simple_refract",
    },
};

static const shader_sub_table RIPEMIT_table[] = {
    {
        SHADER_FT_SUB_RIPPLE_EMIT,
        ripple_emit_vpt_unival_max,
        ripple_emit_fpt_unival_max,
        "ripple_emit",
        "ripple_emit",
    },
};

static const shader_sub_table RAIN_table[] = {
    {
        SHADER_FT_SUB_RAIN,
        rain_vpt_unival_max,
        rain_fpt_unival_max,
        "rain",
        "rain",
    },
};

static const shader_sub_table RIPPLE_table[] = {
    {
        SHADER_FT_SUB_RIPPLE,
        ripple_vpt_unival_max,
        ripple_fpt_unival_max,
        "ripple",
        "ripple",
    },
};

static const shader_sub_table FOGPTCL_table[] = {
    {
        SHADER_FT_SUB_FOG_PTCL,
        fog_ptcl_vpt_unival_max,
        fog_ptcl_fpt_unival_max,
        "fog_ptcl",
        "fog_ptcl",
    },
};

static const shader_sub_table PARTICL_table[] = {
    {
        SHADER_FT_SUB_PARTICLE,
        particle_vpt_unival_max,
        particle_fpt_unival_max,
        "particle",
        "particle",
    },
};

static const shader_sub_table GLITTER_PT_table[] = {
    {
        SHADER_FT_SUB_GLITTER_PARTICLE,
        glitter_particle_vpt_unival_max,
        glitter_particle_fpt_unival_max,
        "glitter_particle",
        "glitter_particle",
    },
};

static const shader_sub_table FONT_table[] = {
    {
        SHADER_FT_SUB_FONT,
        font_vpt_unival_max,
        font_fpt_unival_max,
        "font",
        "font",
    },
};

static const shader_sub_table IMGFILT_table[] = {
    {
        SHADER_FT_SUB_BOX4,
        box4_vpt_unival_max,
        box4_fpt_unival_max,
        "box4",
        "box4",
    },
    {
        SHADER_FT_SUB_BOX8,
        box8_vpt_unival_max,
        box8_fpt_unival_max,
        "box8",
        "box8",
    },
    {
        SHADER_FT_SUB_COPY,
        copy_vpt_unival_max,
        copy_fpt_unival_max,
        "copy",
        "copy",
    },
};

static const shader_sub_table SPRITE_table[] = {
    {
        SHADER_FT_SUB_SPRITE,
        sprite_vpt_unival_max,
        sprite_fpt_unival_max,
        "sprite",
        "sprite",
    },
};

static const shader_sub_table DOF_table[] = {
    {
        SHADER_FT_SUB_DOF_RENDER_TILE,
        dof_common_vpt_unival_max,
        dof_render_tile_fpt_unival_max,
        "dof_common",
        "dof_render_tile",
    },
    {
        SHADER_FT_SUB_DOF_GATHER_TILE,
        dof_common_vpt_unival_max,
        dof_gather_tile_fpt_unival_max,
        "dof_common",
        "dof_gather_tile",
    },
    {
        SHADER_FT_SUB_DOF_DOWNSAMPLE,
        dof_common_vpt_unival_max,
        dof_downsample_fpt_unival_max,
        "dof_common",
        "dof_downsample",
    },
    {
        SHADER_FT_SUB_DOF_MAIN_FILTER,
        dof_common_vpt_unival_max,
        dof_main_filter_fpt_unival_max,
        "dof_common",
        "dof_main_filter",
    },
    {
        SHADER_FT_SUB_DOF_UPSAMPLE,
        dof_common_vpt_unival_max,
        dof_upsample_fpt_unival_max,
        "dof_common",
        "dof_upsample",
    },
};

static const shader_sub_table TRANSPARENCY_table[] = {
    {
        SHADER_FT_SUB_TRANSPARENCY,
        transparency_vpt_unival_max,
        transparency_fpt_unival_max,
        "transparency",
        "transparency",
    },
};

static const uniform_name BLINN_uniform[] = {
    U_SKINNING,
    U_MORPH,
    U_ALPHA_TEST,
};

static const uniform_name ITEM_uniform[] = {
    U_SKINNING,
    U_MORPH,
    U_ALPHA_TEST,
};

static const uniform_name STAGE_uniform[] = {
    U_SKINNING,
    U_MORPH,
    U_ALPHA_TEST,
};

static const uniform_name SKIN_uniform[] = {
    U_SKINNING,
    U_ALPHA_TEST,
};

static const uniform_name SSS_SKIN_uniform[] = {
    U_SKINNING,
    U_ALPHA_TEST,
};

static const uniform_name SSS_FILT_uniform[] = {
    U_INVALID,
};

static const uniform_name HAIR_uniform[] = {
    U_SKINNING,
    U_ALPHA_TEST,
};

static const uniform_name CLOTH_uniform[] = {
    U_SKINNING,
    U_ALPHA_TEST,
};

static const uniform_name TIGHTS_uniform[] = {
    U_SKINNING,
    U_ALPHA_TEST,
};

static const uniform_name SKY_uniform[] = {
    U_MORPH,
    U_ALPHA_TEST,
};

static const uniform_name GLASEYE_uniform[] = {
    U_SKINNING,
};

static const uniform_name ESMGAUSS_uniform[] = {
    U_INVALID,
};

static const uniform_name ESMFILT_uniform[] = {
    U_INVALID,
};

static const uniform_name LITPROJ_uniform[] = {
    U_SKINNING,
    U_ALPHA_TEST,
};

static const uniform_name SIMPLE_uniform[] = {
    U_SKINNING,
};

static const uniform_name SIL_uniform[] = {
    U_SKINNING,
    U_MORPH,
};

static const uniform_name LAMBERT_uniform[] = {
    U_SKINNING,
    U_MORPH,
    U_ALPHA_TEST,
};

static const uniform_name CONSTANT_uniform[] = {
    U_SKINNING,
    U_MORPH,
    U_ALPHA_TEST,
};

static const uniform_name TONEMAP_uniform[] = {
    U_TONE_MAP,
    U_AET_BACK,
};

static const uniform_name REDUCE_uniform[] = {
    U_INVALID,
};

static const uniform_name MAGNIFY_uniform[] = {
    U_INVALID,
};

static const uniform_name MLAA_uniform[] = {
    U_ALPHA_MASK,
    U_MLAA_SEARCH,
};

static const uniform_name CONTOUR_uniform[] = {
    U_INVALID,
};

static const uniform_name CONTOUR_NPR_uniform[] = {
    U_INVALID,
};

static const uniform_name EXPOSURE_uniform[] = {
    U_INVALID,
};

static const uniform_name GAUSS_uniform[] = {
    U_INVALID,
};

static const uniform_name SUN_uniform[] = {
    U_INVALID,
};

static const uniform_name SUN_NO_TEXTURED_uniform[] = {
    U_INVALID,
};

static const uniform_name WATER01_uniform[] = {
    U_MORPH,
};

static const uniform_name W_PTCL_uniform[] = {
    U_INVALID,
};

static const uniform_name SNOW_PT_uniform[] = {
    U_INVALID,
};

static const uniform_name LEAF_PT_uniform[] = {
    U_INVALID,
};

static const uniform_name STAR_uniform[] = {
    U_INVALID,
};

static const uniform_name FLOOR_uniform[] = {
    U_MORPH,
    U_ALPHA_TEST,
};

static const uniform_name PUDDLE_uniform[] = {
    U_INVALID,
};

static const uniform_name S_REFL_uniform[] = {
    U_SKINNING,
    U_MORPH,
    U_ALPHA_TEST,
};

static const uniform_name S_REFR_uniform[] = {
    U_SKINNING,
};

static const uniform_name RIPEMIT_uniform[] = {
    U_RIPPLE,
    U_RIPPLE_EMIT,
};

static const uniform_name RAIN_uniform[] = {
    U_INVALID,
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
    U_INVALID,
};

static const uniform_name FONT_uniform[] = {
    U_INVALID,
};

static const uniform_name IMGFILT_uniform[] = {
    U_INVALID,
};

static const uniform_name SPRITE_uniform[] = {
    U_TEX_0_TYPE,
    U_TEX_1_TYPE,
    U_COMBINER,
};

static const uniform_name DOF_uniform[] = {
    U_DOF,
};

static const uniform_name TRANSPARENCY_uniform[] = {
    U_INVALID,
};

struct glass_eye_struct {
    vec4 field_0;
    vec4 field_10;
    float_t field_20;
    float_t field_24;
    float_t field_28;
    float_t field_2C;
    vec3 field_30;
    vec3 field_3C;
    vec3 field_48;
    float_t field_54;
    vec3 field_58;
    float_t field_64;
    vec3 field_68;
    vec3 field_74;
    vec3 field_80;
    float_t field_8C;
    vec3 field_90;
    uint32_t frame;
    vec4 field_A0;
    bool field_B0;
    bool field_B1;
    bool field_B2;
    bool field_B3;
    int32_t field_B4;
    int32_t field_B8;
    int32_t field_BC;
};

#define shader_table_struct(n) \
{ \
    #n, \
    SHADER_FT_##n, \
    sizeof(n##_table) / sizeof(shader_sub_table), \
    n##_table, \
    n##_uniform[0] > 0 ? sizeof(n##_uniform) / sizeof(uniform_name) : 0, \
    n##_uniform, \
}

const shader_table shader_ft_table[] = {
    {
        "SHADER_FFP",
        SHADER_FT_FFP,
        0,
        0,
        0,
        0,
    },
    shader_table_struct(BLINN),
    shader_table_struct(ITEM),
    shader_table_struct(STAGE),
    shader_table_struct(SKIN),
    shader_table_struct(SSS_SKIN),
    shader_table_struct(SSS_FILT),
    shader_table_struct(HAIR),
    shader_table_struct(CLOTH),
    shader_table_struct(TIGHTS),
    shader_table_struct(SKY),
    shader_table_struct(GLASEYE),
    shader_table_struct(ESMGAUSS),
    shader_table_struct(ESMFILT),
    shader_table_struct(LITPROJ),
    shader_table_struct(SIMPLE),
    shader_table_struct(SIL),
    shader_table_struct(LAMBERT),
    shader_table_struct(CONSTANT),
    shader_table_struct(TONEMAP),
    shader_table_struct(REDUCE),
    shader_table_struct(MAGNIFY),
    shader_table_struct(MLAA),
    shader_table_struct(CONTOUR),
    shader_table_struct(CONTOUR_NPR),
    shader_table_struct(EXPOSURE),
    shader_table_struct(GAUSS),
    shader_table_struct(SUN),
    shader_table_struct(SUN_NO_TEXTURED),
    shader_table_struct(WATER01),
    shader_table_struct(W_PTCL),
    shader_table_struct(SNOW_PT),
    shader_table_struct(LEAF_PT),
    shader_table_struct(STAR),
    shader_table_struct(FLOOR),
    shader_table_struct(PUDDLE),
    shader_table_struct(S_REFL),
    shader_table_struct(S_REFR),
    shader_table_struct(RIPEMIT),
    shader_table_struct(RAIN),
    shader_table_struct(RIPPLE),
    shader_table_struct(FOGPTCL),
    shader_table_struct(PARTICL),
    shader_table_struct(GLITTER_PT),
    shader_table_struct(FONT),
    shader_table_struct(IMGFILT),
    shader_table_struct(SPRITE),
    shader_table_struct(DOF),
    shader_table_struct(TRANSPARENCY),
};

#undef shader_table_struct

const size_t shader_ft_table_size =
    sizeof(shader_ft_table) / sizeof(shader_table);

static void glass_eye_calc(glass_eye_struct* glass_eye);
static void glass_eye_set(glass_eye_struct* glass_eye);
static void shader_bind_blinn(shader_set_data* set, shader* shad);
static void shader_bind_cloth(shader_set_data* set, shader* shad);
static void shader_bind_hair(shader_set_data* set, shader* shad);
static void shader_bind_glass_eye(shader_set_data* set, shader* shad);
static void shader_bind_tone_map(shader_set_data* set, shader* shad);
static void shader_bind_sss_filter(shader_set_data* set, shader* shad);
static void shader_bind_esm_filter(shader_set_data* set, shader* shad);
static void shader_bind_reduce_tex(shader_set_data* set, shader* shad);
static void shader_bind_magnify(shader_set_data* set, shader* shad);
static void shader_bind_mlaa(shader_set_data* set, shader* shad);
static void shader_bind_exposure(shader_set_data* set, shader* shad);
static void shader_bind_gauss(shader_set_data* set, shader* shad);
static void shader_bind_snow_particle(shader_set_data* set, shader* shad);
static void shader_bind_star(shader_set_data* set, shader* shad);
static void shader_bind_imgfilter(shader_set_data* set, shader* shad);
static void shader_bind_dof(shader_set_data* set, shader* shad);

const shader_bind_func shader_ft_bind_func_table[] = {
    {
        SHADER_FT_BLINN,
        shader_bind_blinn,
    },
    {
        SHADER_FT_CLOTH,
        shader_bind_cloth,
    },
    {
        SHADER_FT_HAIR,
        shader_bind_hair,
    },
    {
        SHADER_FT_GLASEYE,
        shader_bind_glass_eye,
    },
    {
        SHADER_FT_TONEMAP,
        shader_bind_tone_map,
    },
    {
        SHADER_FT_SSS_FILT,
        shader_bind_sss_filter,
    },
    {
        SHADER_FT_ESMFILT,
        shader_bind_esm_filter,
    },
    {
        SHADER_FT_REDUCE,
        shader_bind_reduce_tex,
    },
    {
        SHADER_FT_MAGNIFY,
        shader_bind_magnify,
    },
    {
        SHADER_FT_MLAA,
        shader_bind_mlaa,
    },
    {
        SHADER_FT_EXPOSURE,
        shader_bind_exposure,
    },
    {
        SHADER_FT_GAUSS,
        shader_bind_gauss,
    },
    {
        SHADER_FT_SNOW_PT,
        shader_bind_snow_particle,
    },
    {
        SHADER_FT_STAR,
        shader_bind_star,
    },
    {
        SHADER_FT_IMGFILT,
        shader_bind_imgfilter,
    },
    {
        SHADER_FT_DOF,
        shader_bind_dof,
    },
};

const size_t shader_ft_bind_func_table_size =
    sizeof(shader_ft_bind_func_table) / sizeof(shader_bind_func);

shader_set_data shaders_ft;

static glass_eye_struct glass_eye = {
    { 5.0f, 5.0f, 0.5f, 0.5f },
    { 2.5f, 2.5f, 0.5f, 0.5f },
    1.0f,
    1.45f,
    0.02f,
    0.85f,
    { 1.0f, 1.2f, 0.45f },
    { 1.0f, 1.2f, 0.25f },
    { 0.5f, 0.6f, 0.4f },
    -0.2f,
    { 1.0f, 1.0f, 1.0f },
    0.0f,
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    0.0,
    { 0.0f, 0.0f, 0.0f },
    0,
    { 0.0f, 0.0f, 0.0f, 0.0f },
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

int32_t shader_ft_get_index_by_name(const char* name) {
    if (!str_utils_compare(name, "EYEBALL"))
        return SHADER_FT_GLASEYE;
    return -1;
}

const char* shader_ft_get_name_by_index(int32_t index) {
    if (index == SHADER_FT_GLASEYE)
        return "EYEBALL";
    return 0;
}

static void glass_eye_calc(glass_eye_struct* glass_eye) {
    float_t v2 = glass_eye->field_28;
    glass_eye->field_64 = glass_eye->field_2C / v2;
    glass_eye->field_90 = glass_eye->field_58;
    glass_eye->field_68 = glass_eye->field_30 * v2;
    glass_eye->field_74 = glass_eye->field_3C * v2;
    glass_eye->field_80 = glass_eye->field_48 * v2;
    glass_eye->field_8C = glass_eye->field_54 * v2;
    if (!glass_eye->field_B0) {
        glass_eye->field_A0 = 0.0f;
        return;
    }

    uint32_t frame = get_frame_counter();
    if (frame == glass_eye->frame)
        return;

    glass_eye->frame = frame;
    if (frame == (frame / 90) * 90) {
        glass_eye->field_A0.x = (rand_state_array_get_float(0) - 0.5f) * 0.015f;
        glass_eye->field_A0.y = (rand_state_array_get_float(0) - 0.5f) * 0.015f;
    }

    float_t v17 = (float_t)(uint8_t)frame * (float_t)(M_PI * (1.0 / 128.0));
    float_t v19 = sinf(sinf(v17 * 27.0f) + v17 * 2.0f);
    float_t v20 = sinf(v17 * 23.0f);
    glass_eye->field_A0.z = (v19 + v20) * 0.5f * 0.01f;
    glass_eye->field_A0.w = 0.0f;
}

static void glass_eye_set(glass_eye_struct* glass_eye) {
    glass_eye_batch_shader_data glass_eye_batch = {};

    vec4 temp;
    *(vec3*)&temp = glass_eye->field_68 * glass_eye->field_68;
    temp.w = temp.z;
    *(vec3*)&temp = vec3::rcp(*(vec3*)&temp);
    glass_eye_batch.g_ellipsoid_radius = temp;

    *(vec3*)&temp = glass_eye->field_68;
    temp.w = 1.0f;
    glass_eye_batch.g_ellipsoid_scale = temp;

    glass_eye_batch.g_tex_model_param = glass_eye->field_0;
    glass_eye_batch.g_tex_offset = glass_eye->field_A0;

    *(vec3*)&temp = glass_eye->field_90 * glass_eye->field_90;
    temp.w = temp.z;
    *(vec3*)&temp = vec3::rcp(*(vec3*)&temp);
    glass_eye_batch.g_eb_radius = temp;

    glass_eye_batch.g_eb_tex_model_param = glass_eye->field_10;

    float_t v2 = (glass_eye->field_20 - glass_eye->field_24) / (glass_eye->field_20 + glass_eye->field_24);
    v2 *= v2;
    glass_eye_batch.g_fresnel = { 1.0f - v2, v2, 0.0f, 0.0f };

    float_t v3 = (glass_eye->field_20 * glass_eye->field_20) / (glass_eye->field_24 * glass_eye->field_24);
    glass_eye_batch.g_refract1 = { v3, 1.0f - v3, glass_eye->field_20 / glass_eye->field_24, 0.0f };

    float_t v4 = (glass_eye->field_24 * glass_eye->field_24) / (glass_eye->field_20 * glass_eye->field_20);
    glass_eye_batch.g_refract2 = { v4, 1.0f - v4, glass_eye->field_24 / glass_eye->field_20, 0.0f };

    *(vec3*)&temp = glass_eye->field_74 * glass_eye->field_74;
    temp.w = -1.0f;
    *(vec3*)&temp = vec3::rcp(*(vec3*)&temp);
    glass_eye_batch.g_iris_radius = temp;

    *(vec3*)&temp = glass_eye->field_68 * glass_eye->field_68;
    temp.w = -1.0f;
    *(vec3*)&temp = vec3::rcp(*(vec3*)&temp);
    glass_eye_batch.g_cornea_radius = temp;

    *(vec3*)&temp = glass_eye->field_80 * glass_eye->field_80;
    temp.w = -1.0f;
    *(vec3*)&temp = vec3::rcp(*(vec3*)&temp);
    glass_eye_batch.g_pupil_radius = temp;

    *(vec2*)&temp = *(vec2*)&glass_eye->field_0 * *(vec2*)&glass_eye->field_74;
    temp.z = glass_eye->field_64 * 1.442695f;
    temp.w = glass_eye->field_8C;
    *(vec2*)&temp = vec2::rcp(*(vec2*)&temp);
    glass_eye_batch.g_tex_scale = temp;

    extern render_context* rctx_ptr;
    rctx_ptr->glass_eye_batch_ubo.WriteMemory(glass_eye_batch);
    rctx_ptr->glass_eye_batch_ubo.Bind(5);
}

static void shader_bind_blinn(shader_set_data* set, shader* shad) {
    shad->bind(set, uniform_value[U_NORMAL]
        ? SHADER_FT_SUB_BLINN_FRAG : SHADER_FT_SUB_BLINN_VERT);
}

static void shader_bind_cloth(shader_set_data* set, shader* shad) {
    shad->bind(set, uniform_value[U_NPR] ? SHADER_FT_SUB_CLOTH_NPR1
        : (uniform_value[U_ANISO] ? SHADER_FT_SUB_CLOTH_ANISO : SHADER_FT_SUB_CLOTH_DEFAULT));
}

static void shader_bind_hair(shader_set_data* set, shader* shad) {
    shad->bind(set, uniform_value[U_NPR] ? SHADER_FT_SUB_HAIR_NPR1 : SHADER_FT_SUB_HAIR_DEFAULT);
}

static void shader_bind_glass_eye(shader_set_data* set, shader* shad) {
    uniform_value[U_EYEBALL] = 0;
    if (set->shaders[SHADER_FT_GLASEYE].bind(set, SHADER_FT_SUB_GLASS_EYE) >= 0) {
        glass_eye_calc(&glass_eye);
        glass_eye_set(&glass_eye);
    }
}

static void shader_bind_tone_map(shader_set_data* set, shader* shad) {
    shad->bind(set, uniform_value[U_NPR] == 1
        ? SHADER_FT_SUB_TONEMAP_NPR1 : SHADER_FT_SUB_TONEMAP);
}

static void shader_bind_sss_filter(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_SSS_FILTER]) {
    case 0:
        shad->bind(set, uniform_value[U_NPR]
            ? SHADER_FT_SUB_SSS_FILTER_MIN_NPR : SHADER_FT_SUB_SSS_FILTER_MIN);
        break;
    case 3:
        shad->bind(set, SHADER_FT_SUB_SSS_FILTER_GAUSS_2D);
        break;
    }
}

static void shader_bind_esm_filter(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_ESM_FILTER]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_ESM_FILTER_MIN);
        break;
    case 1:
        shad->bind(set, SHADER_FT_SUB_ESM_FILTER_EROSION);
        break;
    }
}

static void shader_bind_reduce_tex(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_REDUCE]) {
    case 0:
        switch (uniform_value[U_ALPHA_MASK]) {
        case 0:
            shad->bind(set, SHADER_FT_SUB_REDUCE_TEX_REDUCE_2);
            break;
        default:
            shad->bind(set, SHADER_FT_SUB_REDUCE_TEX_REDUCE_2_ALPHAMASK);
            break;
        }
        break;
    case 1:
        switch (uniform_value[U_ALPHA_MASK]) {
        case 0:
            shad->bind(set, SHADER_FT_SUB_REDUCE_TEX_REDUCE_4);
            break;
        }
        break;
    case 3:
        switch (uniform_value[U_ALPHA_MASK]) {
        case 0:
            shad->bind(set, SHADER_FT_SUB_REDUCE_TEX_REDUCE_4_EXTRACT);
            break;
        }
        break;
    case 4:
        switch (uniform_value[U_ALPHA_MASK]) {
        case 0:
            shad->bind(set, SHADER_FT_SUB_GHOST);
            break;
        }
        break;
    case 5:
        switch (uniform_value[U_ALPHA_MASK]) {
        case 0:
            shad->bind(set, SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_2);
            break;
        }
        break;
    case 6:
        switch (uniform_value[U_ALPHA_MASK]) {
        case 0:
            shad->bind(set, SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_BLUR);
            break;
        }
        break;
    case 7:
        switch (uniform_value[U_ALPHA_MASK]) {
        case 0:
            shad->bind(set, SHADER_FT_SUB_REDUCE_TEX_REDUCE_COMPOSITE_4);
            break;
        }
        break;
    }
}

static void shader_bind_magnify(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_MAGNIFY]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_MAGNIFY_LINEAR);
        break;
    case 2:
        shad->bind(set, SHADER_FT_SUB_MAGNIFY_DIFF);
        break;
    case 3:
        shad->bind(set, SHADER_FT_SUB_MAGNIFY_DIFF2);
        break;
    case 4:
        shad->bind(set, SHADER_FT_SUB_MAGNIFY_CONE);
        break;
    case 5:
        shad->bind(set, SHADER_FT_SUB_MAGNIFY_CONE2);
        break;
    }
}

static void shader_bind_mlaa(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_MLAA]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_MLAA_EDGE);
        break;
    case 1:
        shad->bind(set, SHADER_FT_SUB_MLAA_AREA);
        break;
    case 2:
        shad->bind(set, SHADER_FT_SUB_MLAA_BLEND);
        break;
    }
}

static void shader_bind_exposure(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_EXPOSURE]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_EXPOSURE_MINIFY);
        break;
    case 1:
        shad->bind(set, SHADER_FT_SUB_EXPOSURE_MEASURE);
        break;
    case 2:
        shad->bind(set, SHADER_FT_SUB_EXPOSURE_AVERAGE);
        break;
    }
}

static void shader_bind_gauss(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_GAUSS]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_PP_GAUSS_USUAL);
        break;
    case 1:
        shad->bind(set, SHADER_FT_SUB_PP_GAUSS_CONE);
        break;
    }
}

static void shader_bind_snow_particle(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_SNOW_PARTICLE]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_SNOW_PARTICLE_CPU);
        break;
    case 1:
        shad->bind(set, SHADER_FT_SUB_SNOW_PARTICLE);
        break;
    }
}

static void shader_bind_star(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_STAR]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_STAR);
        break;
    case 1:
        shad->bind(set, SHADER_FT_SUB_STAR_MILKY_WAY);
        break;
    }
}

static void shader_bind_imgfilter(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_IMAGE_FILTER]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_BOX4);
        break;
    case 1:
        shad->bind(set, SHADER_FT_SUB_BOX8);
        break;
    case 5:
        shad->bind(set, SHADER_FT_SUB_COPY);
        break;
    }
}

static void shader_bind_dof(shader_set_data* set, shader* shad) {
    switch (uniform_value[U_DOF_STAGE]) {
    case 0:
        shad->bind(set, SHADER_FT_SUB_DOF_RENDER_TILE);
        break;
    case 1:
        shad->bind(set, SHADER_FT_SUB_DOF_GATHER_TILE);
        break;
    case 2:
        shad->bind(set, SHADER_FT_SUB_DOF_DOWNSAMPLE);
        break;
    case 3:
        shad->bind(set, SHADER_FT_SUB_DOF_MAIN_FILTER);
        break;
    case 4:
        shad->bind(set, SHADER_FT_SUB_DOF_UPSAMPLE);
        break;
    }
}
