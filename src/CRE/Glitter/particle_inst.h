/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_particle_inst* FASTCALL glitter_particle_inst_init(GPM, glitter_particle* a1,
    glitter_effect_inst* a2, glitter_emitter_inst* a3, glitter_random* random, float_t emission);
extern void FASTCALL glitter_particle_inst_emit(GPM,
    glitter_particle_inst* a1, int32_t a3, int32_t count, float_t emission);
extern void FASTCALL glitter_particle_inst_free(glitter_particle_inst* a1, bool free);
extern bool FASTCALL glitter_particle_inst_has_ended(glitter_particle_inst* particle, bool a2);
extern void FASTCALL glitter_particle_inst_reset(glitter_particle_inst* a1);
extern void FASTCALL glitter_particle_inst_dispose(glitter_particle_inst* pi);
