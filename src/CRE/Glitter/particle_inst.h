/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_particle_inst* FASTCALL glitter_particle_inst_init(glitter_particle* a1,
    glitter_scene* a2, glitter_effect_inst* a3);
extern void FASTCALL glitter_particle_inst_copy(glitter_particle_inst* a1,
    glitter_particle_inst* a2, glitter_scene* a3);
extern void FASTCALL glitter_particle_inst_emit(glitter_particle_inst* a1,
    glitter_scene* a2, glitter_emitter_inst* a3, int32_t a4, int32_t count);
extern void FASTCALL glitter_particle_inst_free(glitter_particle_inst* a1, bool free);
extern bool FASTCALL glitter_particle_inst_get_value(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, float_t frame);
extern bool FASTCALL glitter_particle_inst_has_ended(glitter_particle_inst* particle, bool a2);
extern glitter_particle_inst* FASTCALL glitter_particle_inst_init_child(glitter_scene* a1,
    glitter_particle_inst* a2);
extern glitter_render_group* FASTCALL glitter_particle_inst_init_render_group(glitter_scene_sub* a1,
    glitter_particle_inst* a2);
extern void FASTCALL glitter_particle_inst_render_group_init(glitter_particle_inst* particle, float_t delta_frame);
extern void FASTCALL glitter_particle_inst_reset(glitter_particle_inst* a1);
extern void FASTCALL glitter_particle_inst_set_mat(glitter_particle_inst* a1);
extern void FASTCALL glitter_particle_inst_dispose(glitter_particle_inst* pi);
