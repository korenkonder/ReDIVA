/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern glitter_effect_inst* glitter_x_effect_inst_init(GPM,
    glitter_effect* a1, size_t id, float_t emission, bool appear_now);
extern void glitter_x_effect_inst_calc_disp(GPM, glitter_effect_inst* a1);
extern void glitter_x_effect_inst_disp(GPM, glitter_effect_inst* a1, draw_pass_3d_type alpha);
extern void glitter_x_effect_inst_free(glitter_effect_inst* a1, float_t emission, bool free);
extern draw_pass_3d_type glitter_x_effect_inst_get_alpha(glitter_effect_inst* a1);
extern fog_id glitter_x_effect_inst_get_fog(glitter_effect_inst* a1);
extern bool glitter_x_effect_inst_has_ended(glitter_effect_inst* effect, bool a2);
extern void glitter_x_effect_inst_reset(glitter_effect_inst* a1, float_t emission);
extern void glitter_x_effect_inst_ctrl(GPM, glitter_effect_inst* a1,
    float_t delta_frame, float_t emission);
extern void glitter_x_effect_inst_dispose(glitter_effect_inst* ei);
