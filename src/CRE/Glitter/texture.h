/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern bool FASTCALL glitter_texture_hashes_pack_file(glitter_effect_group* a1, f2_struct* st, bool use_big_endian);
extern bool FASTCALL glitter_texture_hashes_unpack_file(glitter_effect_group* a1, f2_struct* st);
extern bool FASTCALL glitter_texture_resource_pack_file(glitter_effect_group* a1, f2_struct* st, bool use_big_endian);
extern bool FASTCALL glitter_texture_resource_unpack_file(glitter_effect_group* a1, f2_struct* st);
