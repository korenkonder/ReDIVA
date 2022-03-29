/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern bool glitter_texture_hashes_pack_file(GlitterEffectGroup* a1, f2_struct* st);
extern bool glitter_texture_hashes_unpack_file(GlitterEffectGroup* a1, f2_struct* st);
extern bool glitter_texture_resource_pack_file(GlitterEffectGroup* a1, f2_struct* st);
extern bool glitter_texture_resource_unpack_file(GPM, GlitterEffectGroup* a1, f2_struct* st);
extern bool glitter_texture_load(GPM, GlitterEffectGroup* a1);
extern void glitter_texture_unload(GlitterEffectGroup* a1);
