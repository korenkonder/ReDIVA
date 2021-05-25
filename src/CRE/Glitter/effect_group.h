/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect_group* FASTCALL glitter_effect_group_init(GLT);
extern bool FASTCALL glitter_effect_group_parse_file(glitter_effect_group* a1, f2_struct* st);
extern bool FASTCALL glitter_effect_group_unparse_file(GLT, glitter_effect_group* a1, f2_struct* st);
extern void FASTCALL glitter_effect_group_dispose(glitter_effect_group* eg);