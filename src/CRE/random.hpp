/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include <random>

struct RandState {
    std::mt19937 mt;

    RandState();
    ~RandState();
};

extern void rand_state_array_init();
extern RandState* rand_state_array_get(int32_t id);
extern float_t rand_state_array_get_float(int32_t id);
extern uint32_t rand_state_array_get_int(int32_t id);
extern uint32_t rand_state_array_get_int(uint32_t min, uint32_t max, int32_t id);
extern void rand_state_array_set_seed(uint32_t seed, int32_t id);
extern void rand_state_array_4_set_seed_1393939();
extern void rand_state_array_free();
