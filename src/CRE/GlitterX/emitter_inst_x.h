/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern glitter_emitter_inst* glitter_x_emitter_inst_init(glitter_emitter* a1,
    glitter_effect_inst* a2, float_t emission);
extern void glitter_x_emitter_inst_ctrl(GPM,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame);
extern void glitter_x_emitter_inst_ctrl_init(glitter_emitter_inst* a1,
    glitter_effect_inst* a2, float_t delta_frame);
extern void glitter_x_emitter_inst_emit(glitter_emitter_inst* a1,
    float_t delta_frame, float_t emission);
extern void glitter_x_emitter_inst_free(glitter_emitter_inst* a1, float_t emission);
extern bool glitter_x_emitter_inst_has_ended(glitter_emitter_inst* emitter, bool a2);
extern uint8_t glitter_x_emitter_inst_random_get_step(glitter_emitter_inst* a1);
extern void glitter_x_emitter_inst_random_set_value(glitter_emitter_inst* a1);
extern void glitter_x_emitter_inst_reset(glitter_emitter_inst* a1);
extern void glitter_x_emitter_inst_dispose(glitter_emitter_inst* ei);
