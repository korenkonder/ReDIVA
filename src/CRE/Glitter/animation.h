/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern void FASTCALL glitter_animation_copy(GPM,
    vector_ptr_glitter_curve* src, vector_ptr_glitter_curve* dst);
extern bool FASTCALL glitter_animation_parse_file(GPM, f2_struct* st,
    vector_ptr_glitter_curve* vec, glitter_curve_type_flags flags);
extern bool FASTCALL glitter_animation_unparse_file(GPM, f2_struct* st,
    vector_ptr_glitter_curve* vec, glitter_curve_type_flags flags);
