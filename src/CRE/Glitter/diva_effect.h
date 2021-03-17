/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern bool FASTCALL glitter_diva_effect_parse_file(glitter_file_reader* fr, f2_struct* st);
extern bool FASTCALL glitter_diva_effect_unparse_file(glitter_effect_group* a1, f2_struct* st, bool use_big_endian);
