/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect* FASTCALL glitter_effect_init(GPM);
extern glitter_effect* FASTCALL glitter_effect_copy(GPM, glitter_effect* e);
extern bool FASTCALL glitter_effect_parse_file(GPM,
    glitter_effect_group* a1, f2_struct* st, vector_ptr_glitter_effect* vec);
extern bool FASTCALL glitter_effect_unparse_file(GPM,
    glitter_effect_group* a1, f2_struct* st, glitter_effect* a3);
extern void FASTCALL glitter_effect_dispose(glitter_effect* e);
