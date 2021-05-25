/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_emitter* FASTCALL glitter_emitter_init(GLT);;
extern glitter_emitter* FASTCALL glitter_emitter_copy(GLT, glitter_emitter* e);
extern bool FASTCALL glitter_emitter_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_emitter* vec, glitter_effect* effect);
extern bool FASTCALL glitter_emitter_unparse_file(GLT, glitter_effect_group* a1,
    f2_struct* st, glitter_emitter* a3, glitter_effect* effect);
extern void FASTCALL glitter_emitter_dispose(glitter_emitter* e);
