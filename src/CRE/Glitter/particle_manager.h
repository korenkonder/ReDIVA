/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_particle_manager* FASTCALL glitter_particle_manager_init();
extern void FASTCALL glitter_particle_manager_update_file_reader(glitter_particle_manager* gpm);
extern void FASTCALL glitter_particle_manager_update_scene(glitter_particle_manager* gpm);
extern void FASTCALL glitter_particle_manager_draw(glitter_particle_manager* gpm, int32_t alpha);
extern void FASTCALL glitter_particle_manager_load_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_load_effect(glitter_particle_manager* gpm,
    uint64_t effect_group_hash, uint64_t effect_hash);
extern void FASTCALL glitter_particle_manager_free_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_free_effect_group(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_dispose(glitter_particle_manager* gpm);
