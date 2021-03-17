/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_curve* FASTCALL glitter_curve_init();
extern bool FASTCALL glitter_curve_get_value(glitter_curve* c,
    float_t frame, float_t* value, int32_t random);
extern void FASTCALL glitter_curve_parse_file(f2_struct* st, vector_ptr_glitter_curve* vec);
extern void FASTCALL glitter_curve_unparse_file(f2_struct* st, glitter_curve* c, bool use_big_endian);
extern void FASTCALL glitter_curve_dispose(glitter_curve* c);
