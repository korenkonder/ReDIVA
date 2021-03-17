/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect* FASTCALL glitter_effect_init();
extern bool FASTCALL glitter_effect_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_effect* vec);
extern bool FASTCALL glitter_effect_unparse_file(glitter_effect_group* a1,
    f2_struct* st, glitter_effect* a3, bool use_big_endian);
extern void FASTCALL glitter_effect_dispose(glitter_effect* e);
