/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_emitter_inst* FASTCALL glitter_emitter_inst_init(GPM,
    glitter_emitter* a1, glitter_effect_inst* a2, float_t emission);
extern void FASTCALL glitter_emitter_inst_free(GPM,
    glitter_emitter_inst* a1, float_t emission);
extern void FASTCALL glitter_emitter_inst_emit(GPM,
    glitter_emitter_inst* a1, float_t delta_frame, float_t emission);
extern void FASTCALL glitter_emitter_inst_get_mesh_by_type(GPM, glitter_emitter_inst* a1,
    int32_t index, vec3* scale, vec3* base_translation, vec3* direction, glitter_random* random);
extern bool FASTCALL glitter_emitter_inst_has_ended(glitter_emitter_inst* emitter, bool a2);
extern uint8_t FASTCALL glitter_emitter_inst_random_get_step(glitter_emitter_inst* emitter);
extern void FASTCALL glitter_emitter_inst_reset(glitter_emitter_inst* a1);
extern void FASTCALL glitter_emitter_inst_update_value_frame(GPM,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_update_value_init(GPM,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_dispose(glitter_emitter_inst* ei);
