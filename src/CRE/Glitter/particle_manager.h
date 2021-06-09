/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_particle_manager* FASTCALL glitter_particle_manager_init();
extern void FASTCALL glitter_particle_manager_calc_draw(GPM,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*));
extern bool FASTCALL glitter_particle_manager_check_effect_group(GPM, uint64_t effect_group_hash);
extern bool FASTCALL glitter_particle_manager_check_file_reader(GPM, uint64_t effect_group_hash);
extern bool FASTCALL glitter_particle_manager_check_scene(GPM, uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_draw(GPM, int32_t alpha);
extern bool FASTCALL glitter_particle_manager_free_effect_group(GPM, uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_free_effect_groups(GPM);
extern bool FASTCALL glitter_particle_manager_free_scene(GPM, uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_free_scenes(GPM);
extern size_t FASTCALL glitter_particle_manager_get_ctrl_count(GPM, glitter_particle_type ptcl_type);
extern size_t FASTCALL glitter_particle_manager_get_disp_count(GPM, glitter_particle_type ptcl_type);
extern void glitter_particle_manager_get_frame(GPM, float_t* frame, int32_t* life_time);
extern void glitter_particle_manager_get_start_end_frame(GPM, int32_t* start_frame, int32_t* end_frame);
extern bool FASTCALL glitter_particle_manager_load_effect(GPM, uint64_t effect_group_hash, uint64_t effect_hash);
extern bool FASTCALL glitter_particle_manager_load_scene(GPM, uint64_t effect_group_hash);
extern void FASTCALL glitter_particle_manager_set_frame(GPM,
    glitter_effect_group* effect_group, glitter_scene** scene, float_t curr_frame,
    float_t prev_frame, uint32_t counter, glitter_random* random, bool reset);
extern bool FASTCALL glitter_particle_manager_test_load_effect(GPM,
    uint64_t effect_group_hash, uint64_t effect_hash);
extern bool FASTCALL glitter_particle_manager_test_load_scene(GPM, uint64_t effect_group_hash, bool appear_now);
extern void FASTCALL glitter_particle_manager_update_file_reader(GPM);
extern void FASTCALL glitter_particle_manager_update_scene(GPM);
extern void FASTCALL glitter_particle_manager_dispose(GPM);
