/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_emitter_inst* FASTCALL glitter_emitter_inst_init(glitter_emitter* a1,
    glitter_scene* a2, glitter_effect_inst* a3);
extern void FASTCALL glitter_emitter_inst_copy(glitter_emitter_inst* a1,
    glitter_emitter_inst* a2, glitter_scene* a3);
extern void FASTCALL glitter_emitter_inst_emit(glitter_emitter_inst* a1, glitter_scene* a2);
extern void FASTCALL glitter_emitter_inst_emit_init(glitter_emitter_inst* a1,
    glitter_scene* a2, glitter_effect_inst* a3, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_emit_step(glitter_emitter_inst* a1,
    glitter_scene* a2, glitter_effect_inst* a3, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_free(glitter_emitter_inst* a1, glitter_scene* a2, bool free);
extern void FASTCALL glitter_emitter_inst_get_mesh_by_type(glitter_emitter_inst* a1,
    int32_t index, vec3* scale, vec3* base_translation, vec3* direction);
extern void FASTCALL glitter_emitter_inst_get_value(glitter_emitter_inst* a1, float_t frame, int32_t random);
extern bool FASTCALL glitter_emitter_inst_has_ended(glitter_emitter_inst* emitter, bool a2);
extern void FASTCALL glitter_emitter_inst_render_group_init(glitter_emitter_inst* emitter, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_reset(glitter_emitter_inst* a1);
extern void FASTCALL glitter_emitter_inst_update_mat(glitter_emitter_inst* a1, glitter_effect_inst* a2);
extern void FASTCALL glitter_emitter_inst_update_value_frame(glitter_emitter_inst* a1,
    glitter_effect_inst* a2, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_update_value_init(glitter_emitter_inst* a1,
    float_t frame, float_t delta_frame);
extern void FASTCALL glitter_emitter_inst_dispose(glitter_emitter_inst* ei);
