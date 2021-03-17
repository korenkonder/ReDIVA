/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_particle* FASTCALL glitter_particle_init();
extern void FASTCALL glitter_particle_dispose(glitter_particle* p);

extern bool FASTCALL Glitter__Particle__ParseFile(glitter_file_reader* a1,
    f2_header* header, glitter_emitter* a3, glitter_effect* a4);
extern bool FASTCALL Glitter__Particle__UnpackFile(glitter_file_reader* a1,
    int64_t data, glitter_particle* a3, uint32_t ptcl_version, glitter_effect* a5);
