/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect* FASTCALL glitter_effect_init();
extern void FASTCALL glitter_effect_dispose(glitter_effect* e);

extern bool FASTCALL Glitter__Effect__ParseFile(glitter_file_reader* a1, f2_header* header, glitter_effect* a3);
extern int32_t FASTCALL Glitter__Effect__UnpackFile__GetIndex(glitter_file_reader* a1, int32_t a2);
extern bool FASTCALL Glitter__Effect__UnpackFile(glitter_file_reader* a1,
    int32_t* data, glitter_effect* a3, uint32_t efct_version);
