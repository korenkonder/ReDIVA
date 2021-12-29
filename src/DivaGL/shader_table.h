/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../CRE/shared.h"

typedef enum aft_shader_sub_enum {
    SHADER_SUB_FFP       = 0,
    BLINN_VERT           = 1,
    BLINN_FRAG           = 2,
    ITEM_BLINN           = 3,
    STAGE_BLINN          = 4,
    SKIN_DEFAULT         = 5,
    SSS_SKIN             = 6,
    SSS_FILTER           = 7,
    HAIR_DEFAULT         = 8,
    HAIR_ANISO           = 9,
    HAIR_NPR1            = 10,
    CLOTH_DEFAULT        = 11,
    CLOTH_ANISO          = 12,
    CLOTH_NPR1           = 13,
    TIGHTS               = 14,
    SKY_DEFAULT          = 15,
    EYE_BALL             = 16,
    EYE_LENS             = 17,
    GLASS_EYE            = 18,
    MEMBRANE             = 19,
    SHADOWMAP            = 20,
    ESM                  = 21,
    ESM_GAUSS            = 22,
    ESM_FILTER           = 23,
    LIT_PROJ             = 24,
    SIMPLE               = 25,
    SILHOUETTE           = 26,
    LAMBERT              = 27,
    CONSTANT             = 28,
    PEEL                 = 29,
    TONEMAP              = 30,
    TONEMAP_NPR1         = 31,
    REDUCE_TEX           = 32,
    MAGNIFY              = 33,
    MLAA                 = 34,
    CONTOUR              = 35,
    EXPOSURE             = 36,
    PP_GAUSS             = 37,
    SUN                  = 38,
    FADE                 = 39,
    WATER01              = 40,
    WATER02              = 41,
    WATER_RING           = 42,
    WATER_PARTICLE       = 43,
    SNOW_PARTICLE        = 44,
    LEAF_PARTICLE        = 45,
    STAR                 = 46,
    SNOW_RING            = 47,
    SNOW_FOOTPRINT       = 48,
    SNOW_TEX_SPACE_LIGHT = 49,
    SNOW_CALC_NORMAL     = 50,
    FLOOR                = 51,
    PUDDLE               = 52,
    SIMPLE_REFLECT       = 53,
    SIMPLE_REFRACT       = 54,
    RIPPLE_EMIT          = 55,
    RAIN                 = 56,
    VOLUME_LIGHT         = 57,
    FENCE_ALPHA          = 58,
    RIPPLE               = 59,
    FOG_PTCL             = 60,
    PARTICLE             = 61,
    GLITTER_PARTICLE     = 62,
    SHOW_VECTOR          = 63,
    FONT                 = 64,
    MOVIE                = 65,
    IMGFILTER            = 66,
    SPRITE               = 67,
} aft_shader_sub_enum;

typedef enum aft_shader_enum {
    SHADER_FFP  = 0,
    _BLINN      = 1,
    _ITEM       = 2,
    _STAGE      = 3,
    _SKIN       = 4,
    _SSS_SKIN   = 5,
    _SSS_FILT   = 6,
    _HAIR       = 7,
    _CLOTH      = 8,
    _TIGHTS     = 9,
    _SKY        = 10,
    _EYEBALL    = 11,
    _EYELENS    = 12,
    _GLASEYE    = 13,
    _MEMBRAN    = 14,
    _SHDMAP     = 15,
    _ESM        = 16,
    _ESMGAUSS   = 17,
    _ESMFILT    = 18,
    _LITPROJ    = 19,
    _SIMPLE     = 20,
    _SIL        = 21,
    _LAMBERT    = 22,
    _CONSTANT   = 23,
    _PEEL       = 24,
    _TONEMAP    = 25,
    _REDUCE     = 26,
    _MAGNIFY    = 27,
    _MLAA       = 28,
    _CONTOUR    = 29,
    _EXPOSURE   = 30,
    _GAUSS      = 31,
    _SUN        = 32,
    _FADE       = 33,
    _WATER01    = 34,
    _WATER02    = 35,
    _WATRING    = 36,
    _W_PTCL     = 37,
    _SNOW_PT    = 38,
    _LEAF_PT    = 39,
    _START      = 40,
    _SNORING    = 41,
    _SN_FOOT    = 42,
    _SN_TSL     = 43,
    _SN_NRM     = 44,
    _FLOOR      = 45,
    _PUDDLE     = 46,
    _S_REFL     = 47,
    _S_REFR     = 48,
    _RIPEMIT    = 49,
    _RAIN       = 50,
    _VOLLIT     = 51,
    _FENCE      = 52,
    _RIPPLE     = 53,
    _FOGPTCL    = 54,
    _PARTICL    = 55,
    _GLITTER_PT = 56,
    _SHOWVEC    = 57,
    _FONT       = 58,
    _MOVIE      = 59,
    _IMGFILT    = 60,
    _SPRITE     = 61,
    SHADER_END  = 62,
} aft_shader_enum;

typedef struct aft_shader_sub_table_struct {
    aft_shader_sub_enum sub_name;
    int32_t padding;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    int32_t num_vp;
    int32_t num_fp;
    const char** vp;
    const char** fp;
} aft_shader_sub_table_struct;

typedef struct aft_shader_table_struct {
    const char* name;
    aft_shader_enum name_enum;
    int32_t num_sub;
    const aft_shader_sub_table_struct* sub;
    int32_t num_uniform;
    int32_t padding;
    int32_t* use_uniform;
} aft_shader_table_struct;

typedef struct aft_shader_sub {
    aft_shader_sub_enum sub_name;
    int32_t padding;
    int32_t* vp_unival_max;
    int32_t* fp_unival_max;
    int32_t num_vp;
    int32_t num_fp;
    int32_t* vp;
    int32_t* fp;
} aft_shader_sub;

typedef struct aft_shader {
    const char* name;
    aft_shader_enum name_enum;
    int32_t num_sub;
    aft_shader_sub* subs;
    int32_t num_uniform;
    int32_t padding;
    int32_t* use_uniform;
    void* bind_func;
} aft_shader;

typedef struct aft_shader_bind_func {
    aft_shader_enum name;
    int32_t padding;
    void* bind_func;
} aft_shader_bind_func;

extern aft_shader* shaders;
extern aft_shader_table_struct* aft_shader_table;
extern aft_shader_bind_func* shader_name_bind_func_table;
extern int32_t* current_vp;
extern int32_t* current_fp;

extern void FASTCALL aft_shader_set(aft_shader_enum name);                             // 0x00000001405E4CE0
extern void FASTCALL aft_shader_bind_blinn(aft_shader* shader);                        // 0x00000001405E4320
extern void FASTCALL aft_shader_bind_cloth(aft_shader* shader);                        // 0x00000001405E4340
extern void FASTCALL aft_shader_bind_hair(aft_shader* shader);                         // 0x00000001405E4370
extern void FASTCALL aft_shader_bind_membrane(aft_shader* shader);                     // 0x00000001405E43A0
extern void FASTCALL aft_shader_bind_eye_ball(aft_shader* shader);                     // 0x00000001405E4530
extern void FASTCALL aft_shader_bind_tone_map(aft_shader* shader);                     // 0x00000001405E4580
extern int32_t FASTCALL aft_shader_bind(aft_shader* shader, aft_shader_sub_enum name); // 0x00000001405E4B50
extern void FASTCALL aft_shader_unbind();                                              // 0x00000001405E5660
extern void FASTCALL aft_shader_load_all_shaders();                                    // 0x00000001405E4FC0
