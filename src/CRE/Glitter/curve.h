/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_curve* FASTCALL glitter_curve_init(GPM);
extern glitter_curve* FASTCALL glitter_curve_copy(GPM, glitter_curve* c);
extern bool FASTCALL glitter_curve_get_value(GPM, glitter_curve* c,
    float_t frame, float_t* value, int32_t random_value, glitter_random* random);
extern bool FASTCALL glitter_curve_parse_file(GPM,
    f2_struct* st, uint32_t version, glitter_curve** c);
#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern void FASTCALL glitter_curve_recalculate(GPM,
    glitter_curve* curve, vector_glitter_curve_key* dest);
#endif
extern bool FASTCALL glitter_curve_unparse_file(GPM,
    f2_struct* st, glitter_curve* c);
extern void FASTCALL glitter_curve_dispose(glitter_curve* c);
