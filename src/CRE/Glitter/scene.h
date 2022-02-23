/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern void glitter_scene_calc_disp(GPM, glitter_scene* scene);
#endif
extern void glitter_scene_ctrl(GPM, glitter_scene* scene, float_t delta_frame);
extern void glitter_scene_disp(GPM, glitter_scene* scene, draw_pass_3d_type alpha);
extern size_t glitter_scene_get_ctrl_count(glitter_scene* scene, glitter_particle_type ptcl_type);
extern size_t glitter_scene_get_disp_count(glitter_scene* scene, glitter_particle_type ptcl_type);
extern void glitter_scene_get_frame(glitter_scene* scene, float_t* frame, int32_t* life_time);
extern void glitter_scene_get_start_end_frame(glitter_scene* scene, int32_t* start_frame, int32_t* end_frame);
extern bool glitter_scene_free_effect(GPM, glitter_scene* scene, uint64_t effect_hash, bool free);
extern bool glitter_scene_has_ended(glitter_scene* scene, bool a2);
extern void glitter_scene_init_effect(GPM, glitter_scene* scene,
    glitter_effect* a2, size_t id, bool appear_now);
extern bool glitter_scene_reset_effect(GPM, glitter_scene* scene, uint64_t effect_hash);
