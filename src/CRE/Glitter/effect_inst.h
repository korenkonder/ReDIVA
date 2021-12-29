/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect_inst* glitter_effect_inst_init(GPM, GLT,
    glitter_effect* a1, size_t id, float_t emission, bool appear_now);
extern void glitter_effect_inst_calc_draw(GPM, glitter_effect_inst* a1);
extern void glitter_effect_inst_draw(GPM, glitter_effect_inst* a1, draw_pass_3d_type alpha);
extern draw_pass_3d_type glitter_effect_inst_get_alpha(glitter_effect_inst* a1);
extern size_t glitter_effect_inst_get_ctrl_count(glitter_effect_inst* a1, glitter_particle_type type);
extern size_t glitter_effect_inst_get_disp_count(glitter_effect_inst* a1, glitter_particle_type type);
extern fog_type glitter_effect_inst_get_fog(glitter_effect_inst* a1);
extern bool glitter_effect_inst_has_ended(glitter_effect_inst* effect, bool a2);
extern void glitter_effect_inst_reset(GPM, GLT, glitter_effect_inst* a1, float_t emission);
extern void glitter_effect_inst_update(GPM, GLT, glitter_effect_inst* a1,
    float_t delta_frame, float_t emission);
extern void glitter_effect_inst_dispose(glitter_effect_inst* ei);
