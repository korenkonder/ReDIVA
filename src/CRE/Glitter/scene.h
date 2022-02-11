/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_scene* glitter_scene_init(glitter_effect_group* eg);
extern void glitter_scene_ctrl(GPM,
    glitter_scene* scene, float_t delta_frame);
extern size_t glitter_scene_get_ctrl_count(glitter_scene* scene, glitter_particle_type ptcl_type);
extern size_t glitter_scene_get_disp_count(glitter_scene* scene, glitter_particle_type ptcl_type);
extern void glitter_scene_get_frame(glitter_scene* scene, float_t* frame, int32_t* life_time);
extern void glitter_scene_get_start_end_frame(glitter_scene* scene, int32_t* start_frame, int32_t* end_frame);
extern bool glitter_scene_has_ended(glitter_scene* scene, bool a2);
extern void glitter_scene_init_effect(GPM, glitter_scene* scene,
    glitter_effect* a2, size_t id, bool appear_now);
extern void glitter_scene_dispose(glitter_scene* s);
