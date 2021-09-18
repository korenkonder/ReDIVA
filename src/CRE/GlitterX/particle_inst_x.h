/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern glitter_particle_inst* glitter_x_particle_inst_init(glitter_particle* a1,
    glitter_effect_inst* a2, glitter_emitter_inst* a3, glitter_random* random, float_t emission);
extern void glitter_x_particle_inst_emit(glitter_particle_inst* a1,
    int32_t dup_count, int32_t count, float_t emission);
extern void glitter_x_particle_inst_free(glitter_particle_inst* a1, bool free);
extern bool glitter_x_particle_inst_has_ended(glitter_particle_inst* particle, bool a2);
extern void glitter_x_particle_inst_reset(glitter_particle_inst* a1);
extern void glitter_x_particle_inst_dispose(glitter_particle_inst* pi);
