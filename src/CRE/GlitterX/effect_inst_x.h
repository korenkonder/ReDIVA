/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern glitter_effect_inst* FASTCALL glitter_x_effect_inst_init(GPM,
    glitter_effect* a1, size_t id, float_t emission, bool appear_now);
extern void FASTCALL glitter_x_effect_inst_calc_draw(GPM, glitter_effect_inst* a1,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*));
extern void FASTCALL glitter_x_effect_inst_draw(GPM, glitter_effect_inst* a1, int32_t alpha);
extern int32_t FASTCALL glitter_x_effect_inst_get_alpha(glitter_effect_inst* a1);
extern int32_t FASTCALL glitter_x_effect_inst_get_fog(glitter_effect_inst* a1);
extern bool FASTCALL glitter_x_effect_inst_has_ended(glitter_effect_inst* effect, bool a2);
extern void FASTCALL glitter_x_effect_inst_reset(glitter_effect_inst* a1, float_t emission);
extern void FASTCALL glitter_x_effect_inst_update(GPM, glitter_effect_inst* a1,
    float_t delta_frame, float_t emission);
extern void FASTCALL glitter_x_effect_inst_dispose(glitter_effect_inst* ei);
