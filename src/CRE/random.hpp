/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include <random>

struct RandState {
    std::mt19937 mt;

    RandState();
    virtual ~RandState();
};

extern void rand_state_array_init();
extern RandState* rand_state_array_get(int32_t id);
extern float_t rand_state_array_get_float(int32_t id);
extern uint32_t rand_state_array_get_int(int32_t id);
extern uint32_t rand_state_array_get_int(uint32_t min, uint32_t max, int32_t id);
extern void rand_state_array_free();
