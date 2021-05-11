/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_scene* FASTCALL glitter_scene_init(glitter_effect_group* eg);
extern bool FASTCALL glitter_scene_has_ended(glitter_scene* scene, bool a2);
extern void FASTCALL glitter_scene_init_effect(GPM, glitter_scene* a1,
    glitter_effect* a2, size_t id, bool appear_now);
extern void FASTCALL glitter_scene_update_value_frame(GPM, glitter_scene* a1, float_t delta_frame);
extern void FASTCALL glitter_scene_dispose(glitter_scene* s);
