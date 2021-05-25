/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern glitter_locus_history* FASTCALL glitter_x_locus_history_init(size_t size);
extern void FASTCALL glitter_x_locus_history_append(glitter_locus_history* a1,
    glitter_render_element* a2, glitter_particle_inst* a3);
extern void FASTCALL glitter_x_locus_history_dispose(glitter_locus_history* lh);
