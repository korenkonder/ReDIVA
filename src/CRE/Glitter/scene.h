/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_scene* FASTCALL glitter_scene_init(glitter_effect_group* eg);
extern bool FASTCALL glitter_scene_copy(glitter_scene* a1, glitter_effect_inst* a2, glitter_scene* a3);
extern void FASTCALL glitter_scene_emit(glitter_scene* a1, float_t frame_speed);
extern bool FASTCALL glitter_scene_has_ended(glitter_scene* scene, bool a2);
extern void FASTCALL glitter_scene_init_effect(glitter_scene* a1,
    glitter_effect* a2, int32_t id, bool appear_now);
extern void FASTCALL glitter_scene_update_scene(glitter_scene* a1, float_t delta_frame);
extern void FASTCALL glitter_scene_update_value_frame(glitter_scene* a1, float_t delta_frame);
extern void FASTCALL glitter_scene_dispose(glitter_scene* s);
