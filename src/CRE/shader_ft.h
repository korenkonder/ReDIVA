/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "shader.h"

typedef enum shader_ft_enum {
    SHADER_FT_FFP        = 0x00,
    SHADER_FT_BLINN      = 0x01,
    SHADER_FT_ITEM       = 0x02,
    SHADER_FT_STAGE      = 0x03,
    SHADER_FT_SKIN       = 0x04,
    SHADER_FT_SSS_SKIN   = 0x05,
    SHADER_FT_SSS_FILT   = 0x06,
    SHADER_FT_HAIR       = 0x07,
    SHADER_FT_CLOTH      = 0x08,
    SHADER_FT_TIGHTS     = 0x09,
    SHADER_FT_SKY        = 0x0A,
    SHADER_FT_EYEBALL    = 0x0B,
    SHADER_FT_EYELENS    = 0x0C,
    SHADER_FT_GLASEYE    = 0x0D,
    SHADER_FT_MEMBRAN    = 0x0E,
    SHADER_FT_SHDMAP     = 0x0F,
    SHADER_FT_ESM        = 0x10,
    SHADER_FT_ESMGAUSS   = 0x11,
    SHADER_FT_ESMFILT    = 0x12,
    SHADER_FT_LITPROJ    = 0x13,
    SHADER_FT_SIMPLE     = 0x14,
    SHADER_FT_SIL        = 0x15,
    SHADER_FT_LAMBERT    = 0x16,
    SHADER_FT_CONSTANT   = 0x17,
    SHADER_FT_PEEL       = 0x18,
    SHADER_FT_TONEMAP    = 0x19,
    SHADER_FT_REDUCE     = 0x1A,
    SHADER_FT_MAGNIFY    = 0x1B,
    SHADER_FT_MLAA       = 0x1C,
    SHADER_FT_CONTOUR    = 0x1D,
    SHADER_FT_EXPOSURE   = 0x1E,
    SHADER_FT_GAUSS      = 0x1F,
    SHADER_FT_SUN        = 0x20,
    SHADER_FT_FADE       = 0x21,
    SHADER_FT_WATER01    = 0x22,
    SHADER_FT_WATER02    = 0x23,
    SHADER_FT_WATRING    = 0x24,
    SHADER_FT_W_PTCL     = 0x25,
    SHADER_FT_SNOW_PT    = 0x26,
    SHADER_FT_LEAF_PT    = 0x27,
    SHADER_FT_STAR       = 0x28,
    SHADER_FT_SNORING    = 0x29,
    SHADER_FT_SN_FOOT    = 0x2A,
    SHADER_FT_SN_TSL     = 0x2B,
    SHADER_FT_SN_NRM     = 0x2C,
    SHADER_FT_FLOOR      = 0x2D,
    SHADER_FT_PUDDLE     = 0x2E,
    SHADER_FT_S_REFL     = 0x2F,
    SHADER_FT_S_REFR     = 0x30,
    SHADER_FT_RIPEMIT    = 0x31,
    SHADER_FT_RAIN       = 0x32,
    SHADER_FT_VOLLIT     = 0x33,
    SHADER_FT_FENCE      = 0x34,
    SHADER_FT_RIPPLE     = 0x35,
    SHADER_FT_FOGPTCL    = 0x36,
    SHADER_FT_PARTICL    = 0x37,
    SHADER_FT_GLITTER_PT = 0x38,
    SHADER_FT_SHOWVEC    = 0x39,
    SHADER_FT_FONT       = 0x3A,
    SHADER_FT_MOVIE      = 0x3B,
    SHADER_FT_IMGFILT    = 0x3C,
    SHADER_FT_SPRITE     = 0x3D,
    SHADER_FT_END        = 0x3E,
} shader_ft_enum;

extern shader_set_data shaders_ft;

extern void shader_ft_load(shader_set_data* set, farc* f, bool ignore_cache);
