/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

typedef struct random {
    int32_t inext;
    int32_t inextp;
    int32_t seed_array[56];
} random;

random* random_init(int32_t seed);
int32_t random_next(random* r);
double_t random_next_double(random* r);
void random_dispose(random* r);
