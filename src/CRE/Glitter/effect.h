/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect* glitter_effect_init(GLT);
extern glitter_effect* glitter_effect_copy(glitter_effect* e);
extern bool glitter_effect_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_old_ptr_glitter_effect* vec);
extern bool glitter_effect_unparse_file(GLT, glitter_effect_group* a1,
    f2_struct* st, glitter_effect* a3);
extern void glitter_effect_dispose(glitter_effect* e);

extern object_info glitter_effect_ext_anim_get_object_info(uint64_t hash);
