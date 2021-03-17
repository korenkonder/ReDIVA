/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_emitter* FASTCALL glitter_emitter_init();;
extern bool FASTCALL glitter_emitter_parse_file(glitter_file_reader* a1, f2_header* a2, glitter_effect* a3);
extern bool FASTCALL glitter_emitter_unpack_file(glitter_file_reader* a1,
    int32_t* data, glitter_emitter* a3, uint32_t emit_version);
extern void FASTCALL glitter_emitter_dispose(glitter_emitter* e);
