/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "counter.h"

int32_t FASTCALL glitter_counter_get(GPM) {
    return ++gpm->counter;
}
