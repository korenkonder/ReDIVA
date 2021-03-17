/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect_inst* FASTCALL glitter_effect_inst_init(glitter_effect* a1,
    glitter_scene* a2, size_t id, bool appear_now);
extern void FASTCALL glitter_effect_inst_copy(glitter_effect_inst* a1,
    glitter_effect_inst* a2, glitter_scene* a3);
extern void FASTCALL glitter_effect_inst_emit(glitter_effect_inst* a1,
    glitter_scene* a2, float_t delta_frame);
extern void FASTCALL glitter_effect_inst_emit_init(glitter_effect_inst* a1,
    glitter_scene* a2, float_t delta_frame);
extern void FASTCALL glitter_effect_inst_free(glitter_effect_inst* a1, glitter_scene* a2, bool a3);
extern void FASTCALL glitter_effect_inst_get_a3da(glitter_effect_inst* a1);
extern int32_t FASTCALL glitter_effect_inst_get_alpha(glitter_effect_inst* a1);
extern int32_t FASTCALL glitter_effect_inst_get_fog(glitter_effect_inst* a1);
extern void FASTCALL glitter_effect_inst_get_value(glitter_effect_inst* a1, float_t frame, int32_t random);
extern bool FASTCALL glitter_effect_inst_has_ended(glitter_effect_inst* effect, bool a2);
extern void FASTCALL glitter_effect_inst_reset(glitter_effect_inst* a1, glitter_scene* a2);
extern void FASTCALL glitter_effect_inst_reset_sub(glitter_effect_inst* a1, glitter_scene* a2);
extern void FASTCALL glitter_effect_inst_update_mat(glitter_effect_inst* a1);
extern void FASTCALL glitter_effect_inst_update_value_frame(glitter_effect_inst* a1, float_t delta_frame);
extern void FASTCALL glitter_effect_inst_update_value_init(glitter_effect_inst* a1, float_t delta_frame);
extern void FASTCALL glitter_effect_inst_dispose(glitter_effect_inst* ei);
