/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern bool glitter_effect_parse_file(GlitterEffectGroup* a1,
    f2_struct* st, std::vector<glitter_effect*>* vec);
extern bool glitter_effect_unparse_file(GLT, GlitterEffectGroup* a1,
    f2_struct* st, glitter_effect* a3);

extern object_info glitter_effect_ext_anim_get_object_info(uint64_t hash);
