/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern bool glitter_diva_effect_parse_file(GPM,
    GlitterFileReader* fr, f2_struct* st, object_database* obj_db);
extern bool glitter_diva_effect_unparse_file(GLT, GlitterEffectGroup* a1, f2_struct* st);
