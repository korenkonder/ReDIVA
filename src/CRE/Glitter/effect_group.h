/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect_group* FASTCALL glitter_effect_group_init();
extern bool FASTCALL glitter_effect_group_parse_file(glitter_file_reader* a1,
    f2_header* header, glitter_effect_group* a3);
extern void FASTCALL glitter_effect_group_dispose(glitter_effect_group* eg);
