/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern bool glitter_texture_hashes_pack_file(glitter_effect_group* a1, f2_struct* st);
extern bool glitter_texture_hashes_unpack_file(glitter_effect_group* a1, f2_struct* st);
extern bool glitter_texture_resource_pack_file(glitter_effect_group* a1, f2_struct* st);
extern bool glitter_texture_resource_unpack_file(GPM, glitter_effect_group* a1, f2_struct* st);
extern bool glitter_texture_load(GPM, glitter_effect_group* a1);
extern void glitter_texture_unload(glitter_effect_group* a1);
