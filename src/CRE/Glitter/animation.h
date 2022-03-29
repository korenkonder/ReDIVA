/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern void glitter_animation_add_value(GLT, GlitterAnimation* anim,
    float_t val, glitter_curve_type_flags flags);
#endif
extern bool glitter_animation_parse_file(GLT, f2_struct* st,
    GlitterAnimation* anim, glitter_curve_type_flags flags);
extern bool glitter_animation_unparse_file(GLT, f2_struct* st,
    GlitterAnimation* anim, glitter_curve_type_flags flags);
