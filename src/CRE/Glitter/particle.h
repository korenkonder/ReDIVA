/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_particle* FASTCALL glitter_particle_init(GLT);
extern glitter_particle* FASTCALL glitter_particle_copy(GLT, glitter_particle* p);
extern bool FASTCALL glitter_particle_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_particle* vec, glitter_effect* effect);
extern bool FASTCALL glitter_particle_unparse_file(GLT, glitter_effect_group* a1,
    f2_struct* st, glitter_particle* a3, glitter_effect* effect);
extern void FASTCALL glitter_particle_dispose(glitter_particle* p);
