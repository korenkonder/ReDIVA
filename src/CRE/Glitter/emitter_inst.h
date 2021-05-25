/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_emitter_inst* FASTCALL glitter_emitter_inst_init(glitter_emitter* a1,
    glitter_effect_inst* a2, float_t emission);
extern void FASTCALL glitter_emitter_inst_free(GPM, GLT,
    glitter_emitter_inst* a1, float_t emission);
extern void FASTCALL glitter_emitter_inst_emit(GPM, GLT,
    glitter_emitter_inst* a1, float_t delta_frame, float_t emission);
extern bool FASTCALL glitter_emitter_inst_has_ended(glitter_emitter_inst* emitter, bool a2);
extern uint8_t FASTCALL glitter_x_emitter_inst_random_get_step(glitter_emitter_inst* emitter);
extern void FASTCALL glitter_emitter_inst_reset(glitter_emitter_inst* a1);
extern void FASTCALL glitter_emitter_inst_update(GPM, GLT,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_update_init(GPM, GLT,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_dispose(glitter_emitter_inst* ei);
