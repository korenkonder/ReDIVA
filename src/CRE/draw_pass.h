/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"

typedef enum draw_object_type {
    DRAW_OBJECT_OPAQUE                    = 0x00,
    DRAW_OBJECT_TRANSLUCENT               = 0x01,
    DRAW_OBJECT_TRANSLUCENT_NO_SHADOW     = 0x02,
    DRAW_OBJECT_TRANSPARENT               = 0x03,
    DRAW_OBJECT_SHADOW_CHARA              = 0x04,
    DRAW_OBJECT_SHADOW_STAGE              = 0x05,
    DRAW_OBJECT_TYPE_6                    = 0x06,
    DRAW_OBJECT_TYPE_7                    = 0x07,
    DRAW_OBJECT_SHADOW_OBJECT_CHARA       = 0x08,
    DRAW_OBJECT_SHADOW_OBJECT_STAGE       = 0x09,
    DRAW_OBJECT_REFLECT_CHARA_OPAQUE      = 0x0A,
    DRAW_OBJECT_REFLECT_CHARA_TRANSLUCENT = 0x0B,
    DRAW_OBJECT_REFLECT_CHARA_TRANSPARENT = 0x0C,
    DRAW_OBJECT_REFLECT_OPAQUE            = 0x0D,
    DRAW_OBJECT_REFLECT_TRANSLUCENT       = 0x0E,
    DRAW_OBJECT_REFLECT_TRANSPARENT       = 0x0F,
    DRAW_OBJECT_REFRACT_OPAQUE            = 0x10,
    DRAW_OBJECT_REFRACT_TRANSLUCENT       = 0x11,
    DRAW_OBJECT_REFRACT_TRANSPARENT       = 0x12,
    DRAW_OBJECT_HRC                       = 0x13,
    DRAW_OBJECT_OPAQUE_TYPE_20            = 0x14,
    DRAW_OBJECT_TRANSPARENT_TYPE_21       = 0x15,
    DRAW_OBJECT_TRANSLUCENT_TYPE_22       = 0x16,
    DRAW_OBJECT_OPAQUE_TYPE_23            = 0x17,
    DRAW_OBJECT_TRANSPARENT_TYPE_24       = 0x18,
    DRAW_OBJECT_TRANSLUCENT_TYPE_25       = 0x19,
    DRAW_OBJECT_OPAQUE_TYPE_26            = 0x1A,
    DRAW_OBJECT_TRANSPARENT_TYPE_27       = 0x1B,
    DRAW_OBJECT_TRANSLUCENT_TYPE_28       = 0x1C,
    DRAW_OBJECT_RIPPLE                    = 0x1D,
} draw_object_type;
