/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect* FASTCALL glitter_effect_init();
extern bool FASTCALL glitter_effect_parse_file(glitter_file_reader* a1, f2_header* header, glitter_effect* a3);
extern bool FASTCALL glitter_effect_unpack_file(int32_t* data, glitter_effect* a3, uint32_t efct_version);
extern int32_t FASTCALL glitter_effect_unpack_file_get_index(int32_t a1);
extern void FASTCALL glitter_effect_dispose(glitter_effect* e);
