/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

struct time_struct {
    LARGE_INTEGER timestamp;
    double_t inv_freq;
};

extern void time_struct_init(time_struct* t);
extern double_t time_struct_calc_time(time_struct* t);
extern void time_struct_get_timestamp(time_struct* t);
