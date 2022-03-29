/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern bool glitter_emitter_parse_file(GlitterEffectGroup* a1,
    f2_struct* st, std::vector<glitter_emitter*>* vec, glitter_effect* effect);
extern bool glitter_emitter_unparse_file(GLT, GlitterEffectGroup* a1,
    f2_struct* st, glitter_emitter* a3, glitter_effect* effect);
