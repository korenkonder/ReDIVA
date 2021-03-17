/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_particle_manager* FASTCALL glitter_particle_manager_init();
extern bool FASTCALL glitter_particle_manager_check_effect_group(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern bool FASTCALL glitter_particle_manager_check_file_reader(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern bool FASTCALL glitter_particle_manager_check_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_draw(glitter_particle_manager* gpm, int32_t alpha);
extern bool FASTCALL glitter_particle_manager_free_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern bool FASTCALL glitter_particle_manager_free_effect_group(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern size_t FASTCALL glitter_particle_manager_get_ctrl_count(glitter_particle_manager* gpm,
    glitter_particle_type type);
extern size_t FASTCALL glitter_particle_manager_get_disp_count(glitter_particle_manager* gpm,
    glitter_particle_type type);
extern void glitter_particle_manager_get_time(glitter_particle_manager* gpm,
    float_t* frame, float_t* life_time);
extern bool FASTCALL glitter_particle_manager_load_effect(glitter_particle_manager* gpm,
    uint64_t effect_group_hash, uint64_t effect_hash);
extern bool FASTCALL glitter_particle_manager_load_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_reset_scene_disp_counter(glitter_particle_manager* gpm);
extern bool FASTCALL glitter_particle_manager_test_load_effect(glitter_particle_manager* gpm,
    uint64_t effect_group_hash, uint64_t effect_hash);
extern bool FASTCALL glitter_particle_manager_test_load_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_update_file_reader(glitter_particle_manager* gpm);
extern void FASTCALL glitter_particle_manager_update_scene(glitter_particle_manager* gpm);
extern void FASTCALL glitter_particle_manager_dispose(glitter_particle_manager* gpm);
