/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "counter_x.h"

int32_t FASTCALL glitter_x_counter_get(GPM) {
    return ++GPM_VAL->counter;
}
