/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_scene* FASTCALL glitter_scene_init(glitter_effect_group* eg);
extern void FASTCALL glitter_scene_init_effect(glitter_scene* a1, glitter_effect* a2, int32_t id, bool a4);
extern void FASTCALL glitter_scene_dispose(glitter_scene* s);

extern void FASTCALL Glitter__Scene__UpdateSceneSub(glitter_scene_sub* a1, float_t delta_frame);
extern bool FASTCALL Glitter__Scene__Copy(glitter_scene* a1, glitter_effect_inst* a2, glitter_scene* a3);
extern bool FASTCALL Glitter__Scene__HasEnded(glitter_scene* scene, bool a2);
extern void FASTCALL Glitter__Scene__Emit(glitter_scene* a1, float_t frame_speed);
extern void FASTCALL Glitter__Scene__UpdateScene(glitter_scene* a1, float_t delta_frame);
extern void FASTCALL Glitter__Scene__UpdateValueFrame(glitter_scene* a1, float_t delta_frame);
