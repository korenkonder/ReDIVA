/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern void FASTCALL glitter_animation_parse_file(f2_struct* st, vector_ptr_glitter_curve* vec);
extern bool FASTCALL glitter_animation_unparse_file(f2_struct* st, vector_ptr_glitter_curve* vec, bool use_big_endian);
