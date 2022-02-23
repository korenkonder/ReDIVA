/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern bool glitter_effect_group_parse_file(glitter_effect_group* eg, f2_struct* st);
extern bool glitter_effect_group_unparse_file(GLT, glitter_effect_group* eg, f2_struct* st);
#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern bool glitter_effect_group_check_model(glitter_effect_group* eg);
extern void glitter_effect_group_load_model(glitter_effect_group* eg, void* ds);
extern void glitter_effect_group_free_model(glitter_effect_group* eg);
#endif
