/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern void FASTCALL glitter_animation_copy(GLT,
    glitter_animation* src, glitter_animation* dst);
extern void FASTCALL glitter_animation_free(glitter_animation* anim);
extern bool FASTCALL glitter_animation_parse_file(GLT, f2_struct* st,
    glitter_animation* vec, glitter_curve_type_flags flags);
extern bool FASTCALL glitter_animation_unparse_file(GLT, f2_struct* st,
    glitter_animation* vec, glitter_curve_type_flags flags);
