/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern void glitter_animation_add_value(GLT, glitter_animation* anim,
    float_t val, glitter_curve_type_flags flags);
#endif
extern void glitter_animation_copy(glitter_animation* src, glitter_animation* dst);
extern void glitter_animation_free(glitter_animation* anim);
extern bool glitter_animation_parse_file(GLT, f2_struct* st,
    glitter_animation* vec, glitter_curve_type_flags flags);
extern bool glitter_animation_unparse_file(GLT, f2_struct* st,
    glitter_animation* vec, glitter_curve_type_flags flags);
