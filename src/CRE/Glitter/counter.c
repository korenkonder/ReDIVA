/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "counter.h"

extern glitter_particle_manager* gpm;

int32_t FASTCALL glitter_counter_get() {
    if (gpm->f2)
        return gpm->counter & 0xFF;
    else
        return gpm->counter & 0xFFF;
}

void FASTCALL glitter_counter_increment() {
    if (gpm->f2)
        gpm->counter = (gpm->counter + 1) & 0xFF;
    else
        gpm->counter = (gpm->counter + 1) & 0xFFF;
}
