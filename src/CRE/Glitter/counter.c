/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "counter.h"

int32_t glitter_counter_get(GPM) {
    return ++GPM_VAL->counter;
}
