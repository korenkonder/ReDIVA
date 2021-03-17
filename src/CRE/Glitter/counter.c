/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "counter.h"

extern glitter_particle_manager* gpm;

int32_t FASTCALL Glitter__Counter__Get() {
    return gpm->counter;
}

void FASTCALL Glitter__Counter__Increment() {
    gpm->counter = (gpm->counter + 1) % 0x1000;
}
